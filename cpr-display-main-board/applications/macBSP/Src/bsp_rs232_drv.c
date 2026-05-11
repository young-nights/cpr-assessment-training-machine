/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-11-21     18452       the first version
 */
#include "bsp_sys.h"
#include "bsp_rs232_drv.h"


#define DBG_TAG "rs232"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

#define RS232_EVT_RX_IND    (1<<0)  // Receive Indicate - 接收表明
#define RS232_EVT_RX_TMO    (1<<1)  // 字节间超时（一帧结束）
#define RS232_EVT_RX_BREAK  (1<<1)  // 用于标记总线"断裂"

struct rs232_inst
{
    rt_device_t serial;     // 串口设备句柄
    rt_mutex_t lock;        // 互斥锁句柄
    rt_event_t evt;         // 事件句柄
    rt_uint8_t status;      // 连接状态
    rt_int32_t recv_tmo;    // 接收超时时间
    rt_int32_t byte_tmo;    // 接收字节的时间区间
#ifdef RS232_USING_DMA_TX
    struct rt_completion tx_comp;//send completion
#endif
};


#ifdef RS232_USING_DMA_TX
static rt_err_t rs232_send_comp_hook(rt_device_t dev, void *buffer)
{
    rs232_inst_t *hinst = (rs232_inst_t *)(dev->user_data);
    if(hinst){
        rt_completion_done(&(hinst->tx_comp));
    }
    return(RT_EOK);
}
#endif


static rt_err_t rs232_recv_ind_hook(rt_device_t dev, rt_size_t size)
{
    rs232_inst_t *hinst = (rs232_inst_t *)(dev->user_data);

    if (hinst->evt){
        rt_event_send(hinst->evt, RS232_EVT_RX_IND);
    }
    return(RT_EOK);
}



/* rs232_cal_byte_tmo() 就是“波特率→毫秒”的换算表，把 3.5 字符时间换算出来并锁在 [1,20] ms 安全区，给接收状态机当“帧间隔”用 */
static int rs232_cal_byte_tmo(int baudrate)
{
    /* 发送 40 bit 需要的时间折成毫秒 */
    int tmo = (40 * 1000) / baudrate;

    /* 算得太小（高速波特率）时，给个下限，防止系统定时器精度不够导致超时 */
    if (tmo < RS232_BYTE_TMO_MIN){
        tmo = RS232_BYTE_TMO_MIN;
    }
    /* 算得太大（低速波特率）时，给上限，避免一帧要等几百毫秒，系统响应太慢 */
    else if (tmo > RS232_BYTE_TMO_MAX){
        tmo = RS232_BYTE_TMO_MAX;
    }
    return (tmo);
}




static int rs232_dev_open(rs232_inst_t *hinst)
{
#ifdef RS232_USING_DMA_RX

    #ifdef RS232_USING_DMA_TX
    if (rt_device_open(hinst->serial, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_DMA_RX | RT_DEVICE_FLAG_DMA_TX) == RT_EOK)
    {
        return(RT_EOK);
    }
    #endif

    #ifdef RS232_USING_INT_TX
    if (rt_device_open(hinst->serial, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_DMA_RX | RT_DEVICE_FLAG_INT_TX) == RT_EOK)
    {
        return(RT_EOK);
    }
    #endif

    if (rt_device_open(hinst->serial, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_DMA_RX) == RT_EOK)
    {
        return(RT_EOK);
    }

#endif

    #ifdef RS232_USING_DMA_TX
    if (rt_device_open(hinst->serial, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_DMA_TX) == RT_EOK)
    {
        return(RT_EOK);
    }
    #endif

    #ifdef RS232_USING_INT_TX
    if (rt_device_open(hinst->serial, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_INT_TX) == RT_EOK)
    {
        return(RT_EOK);
    }
    #endif

    if (rt_device_open(hinst->serial, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX) == RT_EOK)
    {
        return(RT_EOK);
    }

    return(-RT_ERROR);
}





/*
 * @brief   create rs232 instance dynamically
 * @param   serial      - serial device name
 * @param   baudrate    - serial baud rate
 * @param   parity      - serial parity mode
 * @retval  instance handle
 */
rs232_inst_t * rs232_create(const char *name, int baudrate, int parity)
{
    rs232_inst_t *hinst;
    rt_device_t rs232_dev;

    /* 1.查找设备 */
    rs232_dev = rt_device_find(name);
    if (rs232_dev == RT_NULL)
    {
        LOG_E("rs232 instance initiliaze error, the serial device(%s) no found.", name);
        return(RT_NULL);
    }

    /* 2.类型安检：RT-Thread里所有设备都用统一设备框架注册，但类型分得细——只有 RT_Device_Class_Char 才是“串口” */
    if (rs232_dev->type != RT_Device_Class_Char)
    {
        LOG_E("rs232 instance initiliaze error, the serial device(%s) type is not char.", name);
        return(RT_NULL);
    }

    /* 3. 给RS232创建一个内存块 */
    hinst = rt_malloc(sizeof(struct rs232_inst));
    if (hinst == RT_NULL)
    {
        LOG_E("rs232 create fail. no memory for rs485 create instance.");
        return(RT_NULL);
    }

    /* 4. 创建一个互斥锁：保证多线程同时调用 send/recv 不会撞车，同时也防止低优先级进程长期占用把高优先级饿死 */
    hinst->lock = rt_mutex_create(name, RT_IPC_FLAG_FIFO);
    if (hinst->lock == RT_NULL)
    {
        rt_free(hinst);
        LOG_E("rs232 create fail. no memory for rs232 create mutex.");
        return(RT_NULL);
    }

    /* 5. 给RS-232实例配一个事件对象，用来让中断通知线程“有字节到了 / 帧结束了 */
    hinst->evt = rt_event_create(name, RT_IPC_FLAG_FIFO);
    if (hinst->evt == RT_NULL)
    {
        rt_mutex_delete(hinst->lock);
        rt_free(hinst);
        LOG_E("rs232 create fail. no memory for rs232 create event.");
        return(RT_NULL);
    }

#ifdef RS232_USING_DMA_TX
    rt_completion_init(&(hinst->tx_comp));
#endif

    hinst->serial = rs232_dev;
    hinst->status = 0;
    hinst->recv_tmo = 500;  // 应答超时500ms
    hinst->byte_tmo = rs232_cal_byte_tmo(baudrate);

    rs232_config(hinst, baudrate, 8, parity, 0);

    LOG_D("rs485 create success.");

    return(hinst);
}



/*
 * @brief   destory rs232 instance created dynamically
 * @param   hinst       - instance handle
 * @retval  0 - success, other - error
 */
int rs232_destory(rs232_inst_t * hinst)
{
    if (hinst == RT_NULL){
        LOG_E("rs232 destory fail. hinst is NULL.");
        return(-RT_ERROR);
    }

    rs232_disconn(hinst);

    /* 锁存在才删，删完立即清零指针，防止野指针和重复删除 */
    if (hinst->lock){
        rt_mutex_delete(hinst->lock);
        hinst->lock = RT_NULL;
    }

    /* 安全删除事件集 */
    if (hinst->evt){
        rt_event_delete(hinst->evt);
        hinst->evt = RT_NULL;
    }

    /* 释放空间 */
    rt_free(hinst);

    LOG_D("rs232 destory success.");

    return(RT_EOK);
}



/*
 * @brief   config rs232 params
 * @param   hinst       - instance handle
 * @param   baudrate    - baudrate of communication
 * @param   databits    - data bits, 5~8
 * @param   parity      - parity bit, 0~2, 0 - none, 1 - odd, 2 - even
 * @param   stopbits    - stop bits, 0~1, 0 - 1 stop bit, 1 - 2 stop bits
 * @retval  0 - success, other - error
 */
int rs232_config(rs232_inst_t * hinst, int baudrate, int databits, int parity, int stopbits)
{
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

    if (hinst == RT_NULL){
        LOG_E("rs232 config fail. hinst is NULL.");
        return(-RT_ERROR);
    }

    hinst->byte_tmo = rs232_cal_byte_tmo(baudrate);

    config.baud_rate = baudrate;
    config.data_bits = databits;
    config.parity = parity;
    config.stop_bits = stopbits;
    rt_device_control(hinst->serial, RT_DEVICE_CTRL_CONFIG, &config);

    return(RT_EOK);
}

/*
 * @brief   set wait datas timeout for receiving
 * @param   hinst       - instance handle
 * @param   tmo_ms      - receive wait timeout, 0--no wait, <0--wait forever, >0--wait timeout, default = 0
 * @retval  0 - success, other - error
 */
int rs232_set_recv_tmo(rs232_inst_t * hinst, int tmo_ms)
{
    if (hinst == RT_NULL){
        LOG_E("rs232 set recv timeout fail. hinst is NULL.");
        return(-RT_ERROR);
    }

    /* 把超时时间写进实例 */
    hinst->recv_tmo = tmo_ms;

    LOG_D("rs232 set recv timeout success. the value is %d.", tmo_ms);

    return(RT_EOK);
}

/*
 * @brief   set byte interval timeout for receiving
 * @param   hinst       - instance handle
 * @param   tmo_ms      - byte interval timeout, default is calculated from baudrate
 * @retval  0 - success, other - error
 * @note    “帧结束标尺”的旋钮，设得太短易断帧、太长拖响应，函数只做范围限定和存值，具体判帧逻辑在接收流程里兑现
 */
int rs232_set_byte_tmo(rs232_inst_t * hinst, int tmo_ms)
{
    if (hinst == RT_NULL){
        LOG_E("rs232 set byte timeout fail. hinst is NULL.");
        return(-RT_ERROR);
    }

    if (tmo_ms < RS232_BYTE_TMO_MIN){
        tmo_ms = RS232_BYTE_TMO_MIN;
    }
    else if (tmo_ms > RS232_BYTE_TMO_MAX){
        tmo_ms = RS232_BYTE_TMO_MAX;
    }

    hinst->byte_tmo = tmo_ms;

    LOG_D("rs232 set byte timeout success. the value is %d.", tmo_ms);

    return(RT_EOK);
}

/*
 * @brief   open rs232 connect
 * @param   hinst       - instance handle
 * @retval  0 - success, other - error
 */
int rs232_connect(rs232_inst_t * hinst)
{
    if (hinst == RT_NULL){
        LOG_E("rs232 connect fail. hinst is NULL.");
        return(-RT_ERROR);
    }

    if (hinst->status == 1){
        LOG_D("rs232 is connected.");
        return(RT_EOK);
    }

    if ( rs232_dev_open(hinst) != RT_EOK){
        LOG_E("rs232 instance connect error. serial open fail.");
        return(-RT_ERROR);
    }


    hinst->serial->user_data = hinst;
    hinst->serial->rx_indicate = rs232_recv_ind_hook;
#ifdef RS232_USING_DMA_TX
    hinst->serial->tx_complete = rs232_send_comp_hook;
#endif
    hinst->status = 1;

    LOG_D("rs232 connect success.");

    return(RT_EOK);
}

/*
 * @brief   close rs232 connect
 * @param   hinst       - instance handle
 * @retval  0 - success, other - error
 */
int rs232_disconn(rs232_inst_t * hinst)
{
    if (hinst == RT_NULL)
    {
        LOG_E("rs232 disconnect fail. hinst is NULL.");
        return(-RT_ERROR);
    }

    if (hinst->status == 0)//is not connected
    {
        LOG_D("rs232 is not connected.");
        return(RT_EOK);
    }

    rt_mutex_take(hinst->lock, RT_WAITING_FOREVER);

    if (hinst->serial)
    {
        hinst->serial->rx_indicate = RT_NULL;
#ifdef RS485_USING_DMA_TX
        hinst->serial->tx_complete = RT_NULL;
#endif
        rt_device_close(hinst->serial);
    }

    hinst->status = 0;

    rt_mutex_release(hinst->lock);

    LOG_D("rs232 disconnect success.");

    return(RT_EOK);
}

/*
 * @brief   receive datas from rs232
 * @param   hinst       - instance handle
 * @param   buf         - buffer addr
 * @param   size        - maximum length of received datas
 * @retval  >=0 - length of received datas, <0 - error
 */
int rs232_recv(rs232_inst_t * hinst, void *buf, int size)
{
    int recv_len = 0;
    rt_uint32_t e = 0;
    rt_tick_t timeout;

    if (hinst == RT_NULL || buf == RT_NULL || size == 0)
    {
        LOG_E("rs232 receive fail. param error.");
        return(-RT_ERROR);
    }

    if (hinst->status == 0)
    {
        LOG_E("rs232 receive fail. it is not connected.");
        return(-RT_ERROR);
    }

    if (rt_mutex_take(hinst->lock, RT_WAITING_FOREVER) != RT_EOK)
    {
        LOG_E("rs232 receive fail. it is destoried.");
        return(-RT_ERROR);
    }

    /* 转换为 tick，防止用户传 -1（永等）时出错 */
    rt_tick_t ack_tick  = (hinst->recv_tmo > 0) ? rt_tick_from_millisecond(hinst->recv_tmo) : RT_WAITING_FOREVER;
    rt_tick_t byte_tick = rt_tick_from_millisecond(hinst->byte_tmo);


    while (recv_len < size)
    {
        /* 1. 非阻塞读：把环形缓冲区里所有已到的数据一次读完 */
        int len = rt_device_read(hinst->serial, 0,
                                 (char*)buf + recv_len,
                                 size - recv_len);

        if (len > 0)
        {
            recv_len += len;
            continue;
        }

        /* 动态选择超时：第一次用长应答超时，之后用短字节间超时 */
        timeout = (recv_len == 0) ? ack_tick : byte_tick;

        /* 2. 没数据了 → 等待 rx_indicate 事件 */
        if (rt_event_recv(hinst->evt,
                          RS232_EVT_RX_IND,                    // 只等一个事件
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          timeout, &e) != RT_EOK)
        {
            /* 超时：第一次是应答超时 → 没帧；后续是字节间超时 → 一帧结束 */
            break;
        }
        /* 收到事件 → 继续循环读数据（不需要 reset，清除了就行） */
    }

    rt_mutex_release(hinst->lock);

    /* 返回值：0=超时（无数据或帧结束），>0=收到完整一帧 */
    return recv_len;
}

/*
 * @brief   send datas to rs232
 * @param   hinst       - instance handle
 * @param   buf         - buffer addr
 * @param   size        - length of send datas
 * @retval  >=0 - length of sent datas, <0 - error
 */
int rs232_send(rs232_inst_t * hinst, void *buf, int size)
{
    int send_len = 0;

    if (hinst == RT_NULL || buf == RT_NULL || size == 0)
    {
        LOG_E("rs232 send fail. param is error.");
        return(-RT_ERROR);
    }

    if (hinst->status == 0)
    {
        LOG_E("rs232 send fail. it is not connected.");
        return(-RT_ERROR);
    }

    if (rt_mutex_take(hinst->lock, RT_WAITING_FOREVER) != RT_EOK)
    {
        LOG_E("rs232 send fail. it is destoried.");
        return(-RT_ERROR);
    }

#ifdef RS232_USING_DMA_TX
    rt_completion_init(&hinst->tx_comp);
    send_len = rt_device_write(hinst->serial, 0, buf, size);
    if (send_len == size)
    {
        rt_err_t ret = rt_completion_wait(&hinst->tx_comp, RS232_TX_COMP_TMO_MAX);
        if (ret != RT_EOK)
        {
            /* 超时！说明硬件可能有问题 */
            LOG_E("rs232 dma tx timeout! %d bytes not sent", size);
            rt_mutex_release(hinst->lock);
            return -RT_ERROR;           // 关键：失败要返回负数！
        }
    }
    else{
        /* 驱动只接受了部分数据（极少见，但要处理） */
        rt_mutex_release(hinst->lock);
        LOG_W("rs232 dma tx only accepted %d/%d bytes", send_len, size);
        return send_len > 0 ? send_len : -RT_ERROR;
    }
#else
    send_len = rt_device_write(hinst->serial, 0, buf, size);
#endif

    rt_mutex_release(hinst->lock);

    return(send_len);
}

/*
 * @brief   break rs485 receive wait
 * @param   hinst       - instance handle
 * @retval  0 - success, other - error
 */
int rs232_break_recv(rs232_inst_t * hinst)
{
    if ((hinst == RT_NULL) || (hinst->evt == RT_NULL))
    {
        return(-RT_ERROR);
    }

    rt_event_send(hinst->evt, RS232_EVT_RX_BREAK);

    return (RT_EOK);
}

/*
 * @brief   send data to rs485 and then receive response data from rs485
 * @param   hinst       - instance handle
 * @param   send_buf    - send buffer addr
 * @param   send_len    - length of send datas
 * @param   recv_buf    - recv buffer addr
 * @param   recv_size   - maximum length of received datas
 * @retval  >=0 - length of received datas, <0 - error
 */
int rs232_send_then_recv(rs232_inst_t * hinst, void *send_buf, int send_len, void *recv_buf, int recv_size)
{
    int recv_len = 0;

    if (hinst == RT_NULL || send_buf == RT_NULL || send_len == 0 || recv_buf == RT_NULL || recv_size == 0)
    {
        LOG_E("rs232 send then recv fail. param is error.");
        return(-RT_ERROR);
    }

    if (hinst->status == 0)
    {
        LOG_E("rs232 send_then_recv fail. it is not connected.");
        return(-RT_ERROR);
    }

    if (rt_mutex_take(hinst->lock, RT_WAITING_FOREVER) != RT_EOK)
    {
        LOG_E("rs232 send_then_recv fail. it is destoried.");
        return(-RT_ERROR);
    }

    /* 1. 发送（内部不再拿锁，直接用已持有的锁） */
   int ret = rs232_send(hinst, send_buf, send_len);
    if (ret != send_len)
    {
        LOG_E("rs232 send failed in send_then_recv: %d/%d", ret, send_len);
        rt_mutex_release(hinst->lock);
        return -RT_ERROR;
    }

    /* 2. 直接接收 */
    recv_len = rs232_recv(hinst, recv_buf, recv_size);

    /* 3. 统一释放锁 */
    rt_mutex_release(hinst->lock);

    return recv_len;   // 返回实际收到的字节数，0=超时，>0=收到一帧
}








#ifdef RS232_USING_TEST

#ifndef RS232_TEST_SERIAL
#define RS232_TEST_SERIAL       "uart4"         //default test serial
#endif

#ifndef RS232_TEST_BAUDRATE
#define RS232_TEST_BAUDRATE     9600            //defalut test baudrate
#endif

#ifndef RS232_TEST_PARITY
#define RS232_TEST_PARITY       0               //defalut test parity
#endif

#ifndef RS232_TEST_BUF_SIZE
#define RS232_TEST_BUF_SIZE     1024            //default test buffer size
#endif

#ifndef RS232_TEST_RECV_TMO
#define RS232_TEST_RECV_TMO     30000           //default test recicve timeout
#endif

static rs232_inst_t * rs232_test_hinst = RT_NULL;
static char rs232_test_buf[RS232_TEST_BUF_SIZE];


static const char *cmd_info[] =
{
    "Usage: \n",
    "rs232 create [serial] [baudrate] [parity] [pin] [level] - create rs232 instance.\n",
    "rs232 destory                                           - destory rs232 instance.\n",
    "rs232 set_recv_tmo [tmo_ms]                             - set recieve timeout.\n",
    "rs232 set_byte_tmo [tmo_ms]                             - set byte timeout.\n",
    "rs232 connect                                           - open rs232 connect.\n",
    "rs232 disconn                                           - close rs232 connect.\n",
    "rs232 recv [size]                                       - receive from rs232.\n",
    "rs232 send [size]                                       - send to rs232.\n",
    "rs232 cfg [baudrate] [databits] [parity] [stopbits]     - config rs232.\n",
    "rs232 send_then_recv [send_size] [recv_size]            - send to rs232 and then receive from rs232.\n",
    "\n"
};

static void rs232_show_cmd_info(void)
{
    for(int i=0; i<sizeof(cmd_info)/sizeof(char*); i++)
    {
        rt_kprintf(cmd_info[i]);
    }
}


/**
 * @brief rs232 模块测试命令（FinSH shell 命令）
 *
 * 使用方式：
 *   rs485 create [uart] [baud] [parity] [pin] [level]
 *   rs485 send 16
 *   rs485 recv 32
 *   rs485 send_then_recv 8 32
 *   rs485 destory
 *
 * @param argc  参数总数（包含命令本身）
 * @param argv  参数字符串数组
 */
static void rs232_test(int argc, char **argv)
{

    /* ============================================================= */
    /* 1. 参数检查：至少要有 2 个参数（命令 + 子命令）                                     */
    /* ============================================================= */
    if (argc < 2)
    {
        rs232_show_cmd_info();    // 打印帮助信息
        return ;
    }

    /* ============================================================= */
    /* 2. 子命令：create —— 创建 rs232 实例                                                       */
    /* ============================================================= */
    if (strcmp(argv[1], "create") == 0)
    {
        // 默认参数（可通过宏配置）
        char *serial = RS232_TEST_SERIAL;
        int baudrate = RS232_TEST_BAUDRATE;
        int parity = RS232_TEST_PARITY;

        // 防止重复创建
        if (rs232_test_hinst != NULL)
        {
            rt_kprintf("the test instance is not NULL, please deinit/destory first.\n");
            return;
        }

        // 解析用户输入参数（从 argv[2] 开始）
        if (argc >= 3)  serial   = argv[2];
        if ( argc >= 4) baudrate = atoi(argv[3]);
        if (argc >= 5)  parity   = atoi(argv[4]);

        // 创建底层 RS232 实例
        rs232_test_hinst = rs232_create(serial, baudrate, parity);


        if (rs232_test_hinst != NULL)
        {
            rt_kprintf("rs232 instance create success.\n");
            rt_kprintf("rs232 serial            : %s \n", serial);
            rt_kprintf("rs232 baudrate          : %d \n", baudrate);
            rt_kprintf("rs232 parity            : %d \n", parity);

            // 设置默认接收超时
            rs232_set_recv_tmo(rs232_test_hinst, RS232_TEST_RECV_TMO);
            rt_kprintf("rs232 receive timeout   : %d \n", RS232_TEST_RECV_TMO);
        }
        return;
    }

    /* ============================================================= */
    /* 3. 子命令：destory —— 销毁实例                                                                  */
    /* ============================================================= */
    if (strcmp(argv[1], "destory") == 0)
    {
        rs232_destory(rs232_test_hinst);
        rs232_test_hinst = NULL;
        return;
    }

    /* ============================================================= */
    /* 4. 子命令：set_recv_tmo —— 设置应答超时                                                 */
    /* ============================================================= */
    if (strcmp(argv[1], "set_recv_tmo") == 0)
    {
        int tmo_ms = RS232_TEST_RECV_TMO;
        if (argc >= 3)
        {
            tmo_ms = atoi(argv[2]);
        }
        rs232_set_recv_tmo(rs232_test_hinst, tmo_ms);
        return;
    }

    /* ============================================================= */
    /* 5. 子命令：set_byte_tmo —— 设置字节间超时                                             */
    /* ============================================================= */
    if (strcmp(argv[1], "set_byte_tmo") == 0)
    {
        int tmo_ms = 0;
        if (argc >= 3)
        {
            tmo_ms = atoi(argv[2]);
        }
        rs232_set_byte_tmo(rs232_test_hinst, tmo_ms);
        return;
    }

    /* ============================================================= */
    /* 6. 子命令：connect / disconn —— 连接/断开设备                                    */
    /* ============================================================= */
    if (strcmp(argv[1], "connect") == 0)
    {
        if (rs232_test_hinst == NULL)
        {
            rt_kprintf("the test instance is NULL, please create first.\n");
            return;
        }
        if (rs232_connect(rs232_test_hinst) == RT_EOK)
        {
            rt_kprintf("rs232 instance connect success.\n");
        }
        else
        {
            rt_kprintf("rs232 instance connect fail.\n");
        }
        return;
    }


    if (strcmp(argv[1], "disconn") == 0)
    {
        rs232_disconn(rs232_test_hinst);
        return;
    }

    /* ============================================================= */
    /* 7. 子命令：recv —— 接收数据                                                                        */
    /* ============================================================= */
    if (strcmp(argv[1], "recv") == 0)
    {
        int size = RS232_TEST_BUF_SIZE;
        int len = 0;

        if (rs232_test_hinst == NULL)
        {
            rt_kprintf("the test instance is NULL, please create first.\n");
            return;
        }
        if (argc >= 3)
        {
            size = atoi(argv[2]);
            if (size > RS232_TEST_BUF_SIZE)
            {
                size = RS232_TEST_BUF_SIZE;
            }
        }
        rt_kprintf("rs232 start receiving, max length : %d .\n", size);
        len = rs232_recv(rs232_test_hinst, rs232_test_buf, size);
        if (len == 0)
        {
            rt_kprintf("rs232 receive timeout.\n");
            return;
        }
        rt_kprintf("rs232 received %d datas (hex) : ", len);
        for (int i=0; i<len; i++)
        {
            rt_kprintf("%02X ", rs232_test_buf[i]);
        }
        rt_kprintf("\n");
        return;
    }


    /* ============================================================= */
    /* 8. 子命令：send —— 发送数据                                                                        */
    /* ============================================================= */
    if (strcmp(argv[1], "send") == 0)
    {
        int size = RS232_TEST_BUF_SIZE;

        if (rs232_test_hinst == NULL)
        {
            rt_kprintf("the test instance is NULL, please create first.\n");
            return;
        }
        if (argc >= 3)
        {
            size = atoi(argv[2]);
            if (size > RS232_TEST_BUF_SIZE)
            {
                size = RS232_TEST_BUF_SIZE;
            }
        }
        for (int i=0; i<size; i++)
        {
            rs232_test_buf[i] = i;
        }
        size = rs232_send(rs232_test_hinst, rs232_test_buf, size);
        rt_kprintf("rs232 transmit completed. length : %d .\n", size);
        return;
    }


    /* ============================================================= */
    /* 9. 子命令：cfg —— 配置串口参数                                                                   */
    /* ============================================================= */
    if (strcmp(argv[1], "cfg") == 0)
    {
        int baudrate = RS232_TEST_BAUDRATE;
        int databits = 8;
        int parity = RS232_TEST_PARITY;
        int stopbits = 1;

        if (rs232_test_hinst == NULL)
        {
            rt_kprintf("the test instance is NULL, please create first.\n");
            return;
        }
        if (argc >= 3)
        {
            baudrate = atoi(argv[2]);
        }
        if ( argc >= 4)
        {
            databits = atoi(argv[3]);
        }
        if (argc >= 5)
        {
            parity = atoi(argv[4]);
        }
        if (argc >= 6)
        {
            stopbits = atoi(argv[5]);
        }
        rs232_config(rs232_test_hinst, baudrate, databits, parity, stopbits);
        return;
    }


    /* ============================================================= */
    /* 10. 子命令：send_then_recv —— 一键发收（主站典型操作）                    */
    /* ============================================================= */
    if (strcmp(argv[1], "send_then_recv") == 0)
    {
        int send_size = RS232_TEST_BUF_SIZE;
        int recv_size = RS232_TEST_BUF_SIZE;
        int len = 0;

        if (rs232_test_hinst == NULL)
        {
            rt_kprintf("the test instance is NULL, please create first.\n");
            return;
        }
        if (argc >= 3)
        {
            send_size = atoi(argv[2]);
        }
        if (argc >= 4)
        {
            recv_size = atoi(argv[3]);
        }
        for (int i=0; i<send_size; i++)
        {
            rs232_test_buf[i] = i;
        }
        rt_kprintf("rs485 send %d datas, then receive max length : %d .\n", send_size, recv_size);
        len = rs232_send_then_recv(rs232_test_hinst, rs232_test_buf, send_size, rs232_test_buf, recv_size);
        if (len == 0)
        {
            rt_kprintf("rs232 receive timeout.\n");
            return;
        }
        rt_kprintf("rs232 received %d datas (hex) : ", len);
        for (int i=0; i<len; i++)
        {
            rt_kprintf("%02X ", rs232_test_buf[i]);
        }
        rt_kprintf("\n");
        return;
    }


    /* ============================================================= */
    /* 11. 未知命令                                                                                                     */
    /* ============================================================= */
    rt_kprintf("error ! unsupported command .\n");
}
MSH_CMD_EXPORT_ALIAS(rs232_test, rs232, test rs232 module functions);

#endif










