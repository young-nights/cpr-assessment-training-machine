/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-11-02     Administrator       the first version
 */
#include <remote_nrf24l01_driver.h>
#include "bsp_sys.h"
#include "setup_scr_screen.h"

#define DBG_TAG "nRF24"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>



/* 前向声明一下nrf24l01的事件回调句柄 */
const static struct nrf24_callback g_cb;
/* 创建nRF24L01发送数据的二值信号量 */
rt_sem_t nrf24_send_sem = RT_NULL;
/* 创建nRF24L01进入中断的二值信号量 */
rt_sem_t nrf24_irq_sem = RT_NULL;
/* 定义为全局变量 */
nrf24_t _nrf24 = NULL;


/* Remote 内部状态机（保留枚举供后续使用） */
typedef enum {
    REMOTE_NRF_DISCONNECTED = 0,
    REMOTE_NRF_CONNECTING,
    REMOTE_NRF_CONNECTED,
} remote_nrf_state_t;

/**
  * @brief  This thread entry is used for nRF24L01
  * @retval void
  */
void nRF24L01_Thread_entry(void* parameter)
{
    static int connect_retry_cnt;

    /* 0. Allocate memory for the nRF24L01 structure (nrf24_t is a pointer typedef, must use sizeof struct) */
    _nrf24 = malloc(sizeof(struct nRF24L01_STRUCT));
    if (_nrf24 == NULL) {
        LOG_E("LOG:%d. nrf24 malloc error.",Record.ulog_cnt++);
        return;
    }
    rt_memset(_nrf24, 0, sizeof(struct nRF24L01_STRUCT));
    LOG_I("LOG:%d. nrf24 malloc successful.",Record.ulog_cnt++);


    /* 1. 创建二值信号量 */
    nrf24_send_sem = rt_sem_create("nrf24_send", 0, RT_IPC_FLAG_FIFO);
    if(nrf24_send_sem == RT_NULL){
        LOG_E("LOG:%d. Failed to create nrf24l01 send semaphore.",Record.ulog_cnt++);
    }
    else{
        LOG_I("LOG:%d. Succeed to create nrf24l01 send semaphore.",Record.ulog_cnt++);
    }

    nrf24_irq_sem = rt_sem_create("nrf24_irq", 0, RT_IPC_FLAG_FIFO);
    if(nrf24_irq_sem == RT_NULL){
        LOG_E("LOG:%d. Failed to create nrf24l01 irq semaphore.",Record.ulog_cnt++);
    }
    else{
        LOG_I("LOG:%d. Succeed to create nrf24l01 irq semaphore.",Record.ulog_cnt++);
        _nrf24->nrf24_flags.using_irq = RT_TRUE;
    }


    /* 2. 获取中断引脚编号 */
    _nrf24->port_api.nRF24L01_IRQ_Pin_Num = GET_PIN(B, 10);


    /* 3. 初始化SPI */
    nRF24L01_SPI_Init(&_nrf24->port_api);


    /* 4. 把spi底层函数整体拷贝到ops结构体中 */
    _nrf24->nrf24_ops = g_nrf24_func_ops;
    _nrf24->nrf24_cb  = g_cb;

    /* 5. 配置nRF24L01的参数*/
    if(nRF24L01_Param_Config(&_nrf24->nrf24_cfg) != RT_EOK){
        LOG_E("LOG:%d. nrf24 parameter config error.",Record.ulog_cnt++);
    }
    else{
        LOG_I("LOG:%d. nrf24 parameter config successfully.",Record.ulog_cnt++);
    }
    /* 6. 配置启用中断引脚和中断回调函数 */
    if(nRF24L01_IQR_GPIO_Config(&_nrf24->port_api) != RT_EOK){
        LOG_E("LOG:%d. nrf24 irq config error.",Record.ulog_cnt++);
    }
    else{
        LOG_I("LOG:%d. nrf24 irq config successfully.",Record.ulog_cnt++);
    }


    /* 7. 通过回环通信，检测SPI硬件链路是否有误 */
    if (nRF24L01_Check_SPI_Community(_nrf24) != RT_EOK){
        LOG_E("LOG:%d. nRF24L01 check spi hardware false.",Record.ulog_cnt++);
    }
    else{
        LOG_I("LOG:%d. nRF24L01 check spi hardware successful.",Record.ulog_cnt++);
    }

    /* 8~15. 初始化流程（掉电 → 解锁 → 更新寄存器 → 清空 FIFO 等） */
    /* 8. 先进入掉电模式 */
    _nrf24->nrf24_ops.nrf24_reset_ce();
    nRF24L01_Enter_Power_Down_Mode(_nrf24);

    /* 9. 解锁高级扩展功能 */
    nRF24L01_Ensure_RWW_Features_Activated(_nrf24);

    /* 10. 更新寄存器参数 */
    if (nRF24L01_Update_Parameter(_nrf24) != RT_EOK){
        LOG_E("LOG:%d. nRF24L01 update_onchip_config false.",Record.ulog_cnt++);
    }
    else{
        LOG_I("LOG:%d. nRF24L01 update_onchip_config successful.",Record.ulog_cnt++);
    }

    /* 11. 读取寄存器参数 */
    if (nRF24L01_Read_Onchip_Parameter(_nrf24) != RT_EOK){
        LOG_E("LOG:%d. nRF24L01 read parameter false.",Record.ulog_cnt++);
    }
    else{
        LOG_I("LOG:%d. nRF24L01 read parameter successful.",Record.ulog_cnt++);
    }

    /* 12. 清空"发送/接收"队列 */
    nRF24L01_Flush_RX_FIFO(_nrf24);
    nRF24L01_Flush_TX_FIFO(_nrf24);
    /* 13. 清除中断标志*/
    nRF24L01_Clear_IRQ_Flags(_nrf24);
    /* 14. 清除重发计数和丢包计数*/
    nRF24L01_Clear_Observe_TX(_nrf24);
    /* 15. 配置完成，进入上电模式 */
    nRF24L01_Enter_Power_Up_Mode(_nrf24);
    _nrf24->nrf24_ops.nrf24_set_ce();
    LOG_I("LOG:%d. Successfully initialized",Record.ulog_cnt++);

    rt_thread_mdelay(10);
    nRF24L01_Clear_IRQ_Flags(_nrf24);
    while (rt_sem_take(nrf24_irq_sem, 0) == RT_EOK);

    /* ====================== 主循环（状态机） ====================== */
    for(;;)
    {
        /* 尚未连接则持续广播 */
        if(Record.nrf_if_connected == 0)
        {
            static rt_tick_t last_send = 0;

            if(rt_tick_get() - last_send >= 500)
            {
                last_send = rt_tick_get();
                /* ----------  1. PTX 发送  ---------- */
                _nrf24->nrf24_ops.nrf24_reset_ce();
                nRF24L01_Set_Role_Mode(_nrf24, ROLE_PTX);

                nrf24l01_order_to_pipe(_nrf24, Order_nRF24L01_ASK_Connect_Control_Panel, NRF24_PIPE_2);
                _nrf24->nrf24_ops.nrf24_set_ce();
                rt_thread_mdelay(5);
                _nrf24->nrf24_ops.nrf24_reset_ce();

                /* Clear TX IRQ flags from own NO_ACK transmission + drain semaphore */
                nRF24L01_Clear_IRQ_Flags(_nrf24);
                while (rt_sem_take(nrf24_irq_sem, 0) == RT_EOK);

                /* Switch to PRX to listen for ACK */
                nRF24L01_Set_Role_Mode(_nrf24, ROLE_PRX);

                connect_retry_cnt++;
                LOG_I("Remote → Mainboard: Send connect request... (retry %d)", connect_retry_cnt);
            }

            /* ----------  2. Flush + Clear + 切换 PRX  ---------- */
            _nrf24->nrf24_ops.nrf24_reset_ce();
            nRF24L01_Flush_TX_FIFO(_nrf24);
            nRF24L01_Flush_RX_FIFO(_nrf24);
            nRF24L01_Clear_IRQ_Flags(_nrf24);
            nRF24L01_Set_Role_Mode(_nrf24, ROLE_PRX);
            _nrf24->nrf24_ops.nrf24_set_ce();

#if 0
            /* 验证 PRX 模式是否生效 */
            {
                uint8_t cfg_val = nRF24L01_Read_Reg_Data(_nrf24, NRF24REG_CONFIG);
                uint8_t status_val = nRF24L01_Read_Status_Register(_nrf24);
                uint8_t fifo_status = nRF24L01_Read_Reg_Data(_nrf24, NRF24REG_FIFO_STATUS);
                LOG_I("PRX check: CONFIG=0x%02X PRIM_RX=%d STATUS=0x%02X FIFO=0x%02X",
                      cfg_val, cfg_val & 0x01, status_val, fifo_status);
            }
#endif


            /* ----------  3. 预检查 RX_DR（数据可能在切 PRX 前已到达）  ---------- */
            uint8_t pre_status = nRF24L01_Read_Status_Register(_nrf24);
            if(pre_status & NRF24BITMASK_RX_DR)
            {
                uint8_t len = nRF24L01_Read_Top_RXFIFO_Width(_nrf24);
                if(len > 0 && len <= 32)
                {
                    uint8_t rec_data[32];
                    nRF24L01_Read_Rx_Payload(_nrf24, rec_data, len);
                    nRF24L01_Clear_IRQ_Flags(_nrf24);

                    cpr_src_type_t src = SRC_UNKNOWN;
                    if(nrf24l01_portocol_remote_get_command(rec_data, len, &src) == CMD_TRUE)
                    {
                        LOG_I("PRX rec_data → `nrf24l01_portocol_get_command()` == ASK_Connect_ACK → connected + LOG (precheck, src=%d)", src);
                        connect_retry_cnt = 0;
                    }
                }
            }

            /* ----------  4. 等待 IRQ（主板接收到消息后 ACK）  ---------- */
            rt_err_t rx_ok = rt_sem_take(nrf24_irq_sem, 200);
            if(rx_ok == RT_EOK)
            {
                _nrf24->nrf24_flags.status = nRF24L01_Read_Status_Register(_nrf24);
                nRF24L01_Clear_IRQ_Flags(_nrf24);

                if(_nrf24->nrf24_flags.status & NRF24BITMASK_RX_DR)
                {
                    uint8_t len = nRF24L01_Read_Top_RXFIFO_Width(_nrf24);
                    if(len > 0 && len <= 32)
                    {
                        uint8_t rec_data[32];
                        nRF24L01_Read_Rx_Payload(_nrf24, rec_data, len);

                        cpr_src_type_t src = SRC_UNKNOWN;
                        if(nrf24l01_portocol_remote_get_command(rec_data, len, &src) == CMD_TRUE)
                        {
                            LOG_I("PRX rec_data → `nrf24l01_portocol_get_command()` == ASK_Connect_ACK → connected + LOG (irq, src=%d)", src);
                            connect_retry_cnt = 0;
                        }
                    }
                }
            }

            /* ----------  5. 切回 PTX  ---------- */
            _nrf24->nrf24_ops.nrf24_reset_ce();
            nRF24L01_Set_Role_Mode(_nrf24, ROLE_PTX);
        }
        /* 已连接后的业务循环（双向通信） */
        else
        {
            // 1. Handle outgoing command: send START to Mainboard
            if(Record.nrf_send_start == 1) {
                _nrf24->nrf24_ops.nrf24_reset_ce();
                nRF24L01_Set_Role_Mode(_nrf24, ROLE_PTX);
                nrf24l01_order_to_pipe(_nrf24, Order_nRF24L01_SEND_To_Main_Start, NRF24_PIPE_2);
                _nrf24->nrf24_ops.nrf24_set_ce();
                rt_thread_mdelay(5);
                _nrf24->nrf24_ops.nrf24_reset_ce();
                nRF24L01_Set_Role_Mode(_nrf24, ROLE_PRX);
                _nrf24->nrf24_ops.nrf24_set_ce();
                Record.nrf_send_start = 0;
                LOG_I("Remote sent START command to Mainboard");
            }

            // 2. Listen for incoming data from Mainboard (ACKs and status sync)
            rt_err_t rx_result = rt_sem_take(nrf24_irq_sem, 50);
            if(rx_result == RT_EOK) {
                _nrf24->nrf24_flags.status = nRF24L01_Read_Status_Register(_nrf24);
                nRF24L01_Clear_IRQ_Flags(_nrf24);

                if(_nrf24->nrf24_flags.status & NRF24BITMASK_RX_DR) {
                    uint8_t len, rec_data[32];
                    len = nRF24L01_Read_Top_RXFIFO_Width(_nrf24);
                    nRF24L01_Read_Rx_Payload(_nrf24, rec_data, len);

                    cpr_src_type_t src = SRC_UNKNOWN;
                    if(nrf24l01_portocol_remote_get_command(rec_data, len, &src) == CMD_TRUE) {
                        LOG_I("Remote received data from Main in connected state");
                    }
                }
            }

            rt_thread_mdelay(100);
        }

    }
}





void nRF24L01_Decode_Thread_entry(void* parameter)
{

    rt_uint32_t recved = 0;
    rt_err_t nrf_event_result;

    for(;;)
    {
        if(Record.mode_data_in_set == 1){
            nrf24l01_order_to_pipe(_nrf24 ,Order_nRF24L01_ASK_Data_Mode_In, NRF24_PIPE_2);
        }
        else if(Record.mode_data_in_set == 0){
            nrf24l01_order_to_pipe(_nrf24 ,Order_nRF24L01_ASK_Data_Mode_Out, NRF24_PIPE_2);
        }

        //---------------------------------------------------------------------------------------------------
        /* 处理事件集：来自协议解析线程触发的各种命令事件 */
        nrf_event_result = rt_event_recv(   nrf24l01_events,
                                            EVENT_NRF24_ACK_BODY_LED,
                                            RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                                            RT_WAITING_NO,
                                            &recved);

        if(nrf_event_result == RT_EOK)
        {
            if(recved & EVENT_NRF24_ACK_BODY_LED){
                /* 处理圆圈LED颜色更新事件 */
                if(Record.set_press_led == 0)
                {
                    /* 如果索引为0，表示不亮，跳过更新或设置为白色 */
                    /* 这里可以选择跳过，或者将所有圆圈设置为白色 */
                }
                else if(Record.set_press_led >= 1 && Record.set_press_led <= 7)
                {
                    /* 使用接收到的索引和颜色更新对应的圆圈 */
                    /* Record.set_press_led: 1-7 对应 Circle_Name_Body1 到 Circle_Name_Body7 */
                    /* Record.set_press_led_color: 0-3 对应 CIRCLE_COLOR_WHITE/RED/YELLOW/GREEN */
                    update_circle_by_index(&guider_lvgl, Record.set_press_led, Record.set_press_led_color);
                }
            }
        }

        rt_thread_mdelay(50);
    }
}



/**
  * @brief  Handle events from protocol parser and trigger GUI updates
  * @retval void
  */
void nRF24L01_Data_Transmit_Thread_entry(void* parameter)
{
    rt_uint32_t recved = 0;
    rt_err_t result;

    for(;;)
    {
        result = rt_event_recv(nrf24l01_events,
                               EVENT_NRF24_ACK_START_STATUS | EVENT_NRF24_ACK_MODE_SYNC,
                               RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                               RT_WAITING_FOREVER,
                               &recved);

        if(result == RT_EOK)
        {
            if(recved & EVENT_NRF24_ACK_START_STATUS) {
                LOG_I("Mainboard start status updated: %d", Record.main_start_status);
            }
            if(recved & EVENT_NRF24_ACK_MODE_SYNC) {
                LOG_I("Mainboard mode synced: %d", Record.synced_mode);
            }
        }

        rt_thread_mdelay(10);
    }
}






/**
  * @brief  This is a Initialization for nRF24L01
  * @retval int
  */
rt_thread_t nRF24L01_Task_Handle = RT_NULL;
rt_thread_t nRF24L01_Decode_Task_Handle = RT_NULL;
rt_thread_t nRF24L01_Data_Transmit_Task_Handle = RT_NULL;
int nRF24L01_Thread_Init(void)
{
    nRF24L01_Task_Handle = rt_thread_create("nRF24L01_Thread_entry", nRF24L01_Thread_entry, RT_NULL, 4096, 9, 100);
    /* 检查是否创建成功,成功就启动线程 */
    if(nRF24L01_Task_Handle != RT_NULL)
    {
        LOG_I("LOG:%d. nRF24L01_Thread_entry is Succeed.",Record.ulog_cnt++);
        rt_thread_startup(nRF24L01_Task_Handle);
    }
    else {
        LOG_E("LOG:%d. nRF24L01_Thread_entry is Failed",Record.ulog_cnt++);
    }


    nRF24L01_Decode_Task_Handle = rt_thread_create("nRF24L01_Decode_Thread_entry", nRF24L01_Decode_Thread_entry, RT_NULL, 4096, 9, 50);
    /* 检查是否创建成功,成功就启动线程 */
    if(nRF24L01_Decode_Task_Handle != RT_NULL)
    {
        LOG_I("[nRF24L01]nRF24L01_Decode_Thread_entry is Succeed!! \r\n");
        rt_thread_startup(nRF24L01_Decode_Task_Handle);
    }
    else {
        LOG_E("[nRF24L01]nRF24L01_Decode_Thread_entry is Failed \r\n");
    }

    // Data Transmit thread for handling events from Mainboard ACKs
    nRF24L01_Data_Transmit_Task_Handle = rt_thread_create("nRF24L01_DataTx_entry", nRF24L01_Data_Transmit_Thread_entry, RT_NULL, 2048, 10, 50);
    if(nRF24L01_Data_Transmit_Task_Handle != RT_NULL)
    {
        LOG_I("[nRF24L01]nRF24L01_Data_Transmit_Thread_entry is Succeed!! \r\n");
        rt_thread_startup(nRF24L01_Data_Transmit_Task_Handle);
    }
    else {
        LOG_E("[nRF24L01]nRF24L01_Data_Transmit_Thread_entry is Failed \r\n");
    }

    return RT_EOK;
}
INIT_APP_EXPORT(nRF24L01_Thread_Init);




static void nrf24l01_tx_done(nrf24_t nrf24, rt_uint8_t pipe)
{
    /*! Here just want to tell the user when the role is ROLE_PTX
        the pipe have no special meaning except indicating (send) FAILED or OK
        However, it will matter when the role is ROLE_PRX*/
    if(nrf24->nrf24_cfg.config.prim_rx == ROLE_PTX)
    {
        if(pipe == NRF24_PIPE_NONE){
            rt_kprintf("tx_done failed\n");
        }
        else{
            rt_kprintf("tx_done ok\n");
        }
    }

}




static void nrf24l01_rx_ind(nrf24_t nrf24, uint8_t *data, uint8_t len, int pipe)
{
    rt_kprintf("(p%d): ", pipe);
    for (uint8_t i = 0; i < len; i++) {
        rt_kprintf("%02X ", data[i]);
    }
    rt_kprintf("\n");
}



const static struct nrf24_callback g_cb = {
    .nrf24l01_rx_ind = nrf24l01_rx_ind,
    .nrf24l01_tx_done = nrf24l01_tx_done,
};
