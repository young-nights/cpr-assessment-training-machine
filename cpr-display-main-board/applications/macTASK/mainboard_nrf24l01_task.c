/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-09-02     Administrator       the first version
 */
#include "bsp_sys.h"

/* 前向声明一下nrf24l01的事件回调句柄 */
const static struct nrf24_callback g_cb;
/* 创建nRF24L01发送数据的二值信号量 */
rt_sem_t nrf24_send_sem = RT_NULL;
/* 创建nRF24L01进入中断的二值信号量 */
rt_sem_t nrf24_irq_sem = RT_NULL;
/* nRF24L01 操作互斥锁，防止 Thread_entry 和 Decode_entry 并发访问 */
rt_mutex_t nrf24_mutex = RT_NULL;
/* 定义为全局变量 */
nrf24_t _nrf24 = NULL;
/**
  * @brief  This thread entry is used for nRF24L01
  * @retval void
  */
void nRF24L01_Thread_entry(void* parameter)
{

    /* 0. 给nrf24开创一个实际空间 */
    _nrf24 = malloc(sizeof(struct nRF24L01_STRUCT));
    if (_nrf24 == NULL) {
        LOG_E("LOG:%d. nrf24 malloc error.",Record.ulog_cnt++);
        return;
    }
    LOG_I("LOG:%d. nrf24 malloc successful.",Record.ulog_cnt++);


    /* 1. 创建二值信号量 */
    nrf24_send_sem = rt_sem_create("nrf24_send", 0, RT_IPC_FLAG_FIFO);
    if(nrf24_send_sem == RT_NULL){
        LOG_E("Failed to create nrf24l01 send semaphore.");
    }
    else{
        LOG_I("Succeed to create nrf24l01 send semaphore.");
    }

    nrf24_irq_sem = rt_sem_create("nrf24_irq", 0, RT_IPC_FLAG_FIFO);
    if(nrf24_irq_sem == RT_NULL){
        LOG_E("Failed to create nrf24l01 irq semaphore.");
    }
    else{
        LOG_I("Succeed to create nrf24l01 irq semaphore.");
        _nrf24->nrf24_flags.using_irq = RT_TRUE;
    }

    /* 创建互斥锁保护 nRF24L01 硬件访问 */
    nrf24_mutex = rt_mutex_create("nrf24_mux", RT_IPC_FLAG_PRIO);
    if(nrf24_mutex == RT_NULL){
        LOG_E("Failed to create nrf24 mutex.");
    }


    /* 2. 获取中断引脚编号 */
    _nrf24->port_api.nRF24L01_IRQ_Pin_Num = GET_PIN(D, 9);


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
    rt_kprintf("----------------------------------\r\n");
    rt_kprintf("nrf24l01 running receiver.\r\n");


    for(;;)
    {
        // 1. 如果使用IRQ中断，则获取信号量等待释放
        if(_nrf24->nrf24_flags.using_irq == RT_TRUE){
            rt_sem_take(nrf24_irq_sem, RT_WAITING_FOREVER);
        }

        /* 锁定互斥锁，防止 Decode_entry 同时操作 nRF24L01 */
        if(nrf24_mutex) rt_mutex_take(nrf24_mutex, RT_WAITING_FOREVER);

        // 2. 读取status状态标志，并清除中断触发标志位
        _nrf24->nrf24_flags.status = nRF24L01_Read_Status_Register(_nrf24);
         nRF24L01_Clear_Status_Register(_nrf24, NRF24BITMASK_RX_DR | NRF24BITMASK_TX_DS | NRF24BITMASK_MAX_RT);



         // 4. 角色 = 接收端（PRX）
         if(_nrf24->nrf24_cfg.config.prim_rx == ROLE_PRX)
         {

             // 分析哪条信道接收的数据 -----------------------------------------------------------------
             uint8_t pipe = ((_nrf24->nrf24_flags.status & NRF24BITMASK_RX_P_NO) >> 1) ;

             // 根据 Pipe 自动识别来源（Pipe1 = Sensor）-----------------------------------------------------
                 cpr_src_type_t src = SRC_UNKNOWN;
                 if(pipe == 1) {
                     src = SRC_FROM_SENSOR;
                 } else if(pipe == 2) {
                     src = SRC_FROM_REMOTE;
                 }

             // ------------------------------------------------------------------------------------
             if(src != SRC_UNKNOWN) {
                 uint8_t data_buf[32];
                 uint8_t length = nRF24L01_Read_Top_RXFIFO_Width(_nrf24);
                 rt_kprintf("\n----------------------\n");
                 LOG_I("Receive length = %d from %s (Pipe%d)", length,
                       (src==SRC_FROM_SENSOR)?"Sensor":"Remote", pipe);

                 nRF24L01_Read_Rx_Payload(_nrf24, data_buf, length);

                 // 使用我们修改后的统一解析函数
                 if(nrf24l01_portocol_get_command(data_buf, length, &src) == CMD_TRUE) {
                     LOG_I("Protocol parse succeed from %s",
                           (src==SRC_FROM_SENSOR)?"Sensor":"Remote");
                 } else {
                     LOG_W("Protocol parse failed");
                 }
             }
         }

        /* 释放互斥锁 */
        if(nrf24_mutex) rt_mutex_release(nrf24_mutex);

        rt_thread_mdelay(100);
    }
}




/**
  * @brief  This thread entry is used for nRF24L01
  * @retval void
  */
void nRF24L01_Decode_entry(void* parameter)
{

    for(;;)
    {
        //============================================================
        // 1. 处理 Sensor 的连接请求 ACK（重试3次，覆盖Sensor接收窗口）
        //============================================================
        if (Record.sensor_connect_pending)
        {
            Record.sensor_connect_pending = 0;

            if(nrf24_mutex) rt_mutex_take(nrf24_mutex, RT_WAITING_FOREVER);

            for(int retry = 0; retry < 3; retry++)
            {
                _nrf24->nrf24_ops.nrf24_reset_ce();
                nRF24L01_Set_Role_Mode(_nrf24, ROLE_PTX);
                nRF24L01_Flush_TX_FIFO(_nrf24);

                nrf24l01_order_to_pipe(Order_nRF24L01_ACK_Connect_Control_Panel, NRF24_PIPE_1);

                _nrf24->nrf24_ops.nrf24_set_ce();
                rt_thread_mdelay(10);           // CE 稳定 10ms 确保发送完成
                _nrf24->nrf24_ops.nrf24_reset_ce();
                /* ===== 每次重试后诊断 ===== */
                uint8_t diag_status = nRF24L01_Read_Reg_Data(_nrf24, NRF24REG_STATUS);
                uint8_t diag_observe = nRF24L01_Read_Reg_Data(_nrf24, NRF24REG_OBSERVE_TX);
                uint8_t diag_plos = (diag_observe >> 4) & 0x0F;  // 丢包计数
                uint8_t diag_arc  = diag_observe & 0x0F;          // 重发计数
                uint8_t diag_fifo = nRF24L01_Read_Reg_Data(_nrf24, NRF24REG_FIFO_STATUS);
                uint8_t diag_cfg  = nRF24L01_Read_Reg_Data(_nrf24, NRF24REG_CONFIG);

                /* 读取芯片实际 TX_ADDR */
                uint8_t diag_txaddr[5] = {0};
                uint8_t diag_cmd = NRF24CMD_R_REG | NRF24REG_TX_ADDR;
                _nrf24->nrf24_ops.nrf24_send_then_recv(&_nrf24->port_api, &diag_cmd, 1, diag_txaddr, 5);

                /* 读取芯片实际 RX_ADDR_P1 (Sensor 地址) */
                uint8_t diag_rxaddr_p1[5] = {0};
                diag_cmd = NRF24CMD_R_REG | NRF24REG_RX_ADDR_P1;
                _nrf24->nrf24_ops.nrf24_send_then_recv(&_nrf24->port_api, &diag_cmd, 1, diag_rxaddr_p1, 5);

                LOG_I("DIAG retry %d: STATUS=0x%02X(MAX_RT=%d TX_DS=%d RX_DR=%d) OBSERVE=0x%02X(PLOS=%d ARC=%d) FIFO=0x%02X",
                      retry, diag_status,
                      (diag_status>>4)&1, (diag_status>>5)&1, (diag_status>>6)&1,
                      diag_observe, diag_plos, diag_arc, diag_fifo);
                LOG_I("DIAG retry %d: CONFIG=0x%02X PRIM_RX=%d TX_ADDR=%02X%02X%02X%02X%02X RX_P1=%02X%02X%02X%02X%02X",
                      retry, diag_cfg, diag_cfg & 1,
                      diag_txaddr[0], diag_txaddr[1], diag_txaddr[2], diag_txaddr[3], diag_txaddr[4],
                      diag_rxaddr_p1[0], diag_rxaddr_p1[1], diag_rxaddr_p1[2], diag_rxaddr_p1[3], diag_rxaddr_p1[4]);
                /* ===== 诊断结束 ===== */
                rt_thread_mdelay(50);           // 重试间隔 50ms
            }

            nRF24L01_Set_Role_Mode(_nrf24, ROLE_PRX);
            nRF24L01_Flush_RX_FIFO(_nrf24);
            nRF24L01_Clear_IRQ_Flags(_nrf24);
            _nrf24->nrf24_ops.nrf24_set_ce();

            LOG_I("Sent ACK_Connect to Sensor via Pipe1 (3 retries)");
            if(nrf24_mutex) rt_mutex_release(nrf24_mutex);
        }

        //============================================================
        // 2. 处理 Remote 的连接请求 ACK（重试3次）
        //============================================================
        if (Record.remote_connect_pending)
        {
            Record.remote_connect_pending = 0;

            if(nrf24_mutex) rt_mutex_take(nrf24_mutex, RT_WAITING_FOREVER);

            for(int retry = 0; retry < 3; retry++)
            {
                _nrf24->nrf24_ops.nrf24_reset_ce();
                nRF24L01_Set_Role_Mode(_nrf24, ROLE_PTX);
                nRF24L01_Flush_TX_FIFO(_nrf24);

                nrf24l01_order_to_pipe(Order_nRF24L01_ACK_Connect_Control_Panel, NRF24_PIPE_2);

                _nrf24->nrf24_ops.nrf24_set_ce();
                rt_thread_mdelay(10);
                _nrf24->nrf24_ops.nrf24_reset_ce();

                rt_thread_mdelay(50);
            }

            nRF24L01_Set_Role_Mode(_nrf24, ROLE_PRX);
            nRF24L01_Flush_RX_FIFO(_nrf24);
            nRF24L01_Clear_IRQ_Flags(_nrf24);
            _nrf24->nrf24_ops.nrf24_set_ce();

            LOG_I("Sent ACK_Connect to Remote via Pipe2 (3 retries)");
            if(nrf24_mutex) rt_mutex_release(nrf24_mutex);
        }

        rt_thread_mdelay(50);
    }
}



/**
  * @brief  This thread entry is used for nRF24L01 data transmit
  * @retval void
  */
char send_step_nums = 0;
void nRF24L01_Data_Transmit_Thread_entry(void* parameter)
{


    for(;;)
    {
        if(MySysCfg.start_status == 1)
        {
            // --------------------------------------------------------------------
            /* 发送指令-step1：如果按下了开始按键，就给sensor板发送开始指令 */
            if(send_step_nums == 0 && Record.sensor_start_cmd_ack == 0)
            {
                if(nrf24l01_send_with_retry(_nrf24, Order_nRF24L01_SEND_To_Sensor_Start, NRF24_PIPE_1, 1) == RT_EOK) {
                    LOG_I("step1：mainboard发送开始指令到sensor成功!");
                } else {
                    LOG_E("step1：mainboard发送开始指令到sensor失败!");
                }

                // Also notify Remote that CPR has started
                if(Record.remote_connected == 1) {
                    if(nrf24l01_send_with_retry(_nrf24, Order_nRF24L01_SEND_To_Remote_Start_Status, NRF24_PIPE_2, 1) == RT_EOK) {
                        LOG_I("step1：mainboard发送开始状态到Remote成功!");
                    } else {
                        LOG_E("step1：mainboard发送开始状态到Remote失败!");
                    }
                }
            }
            // --------------------------------------------------------------------
            /* 发送指令-step2：控制ws2812b的灯光亮度，开始后默认亮度1 */
            if(send_step_nums == 1 && Record.sensor_wsrgb_cmd_ack == 0)
            {
                MySysCfg.eyes_rgb_level = 1;
                if(nrf24l01_send_with_retry(_nrf24, Order_nRF24L01_SEND_To_Sensor_WS2812_Level, NRF24_PIPE_1, 1) == RT_EOK) {
                    LOG_I("step2：mainboard发送灯光亮度1指令到sensor成功");
                } else {
                    LOG_E("step2：mainboard发送灯光亮度1指令到sensor失败");
                }
            }
            // --------------------------------------------------------------------
            /* 发送指令-step3：初始状态把sensor的电机震动设置为不响应（即无脉博），sensor板收到按压数据后再自主脉动 */
            if(send_step_nums == 2 && Record.sensor_motor_cmd_ack == 0)
            {
                MySysCfg.motor_work_sta = 0;
                if(nrf24l01_send_with_retry(_nrf24, Order_nRF24L01_SEND_To_Sensor_Motor_Status, NRF24_PIPE_1, 1) == RT_EOK) {
                    LOG_I("step3：mainboard发送电机初始状态指令到sensor成功");
                } else {
                    LOG_E("step3：mainboard发送电机初始状态指令到sensor失败");
                }
            }

            // --------------------------------------------------------------------
            /* 发送回应shoke_cmd指令 */
            if(Record.shoke_cmd_ack == 1)
            {
                if(nrf24l01_send_with_retry(_nrf24, Order_nRF24L01_ACK_Shoke_Sensor_Cmd, NRF24_PIPE_1, 1) == RT_EOK) {
                    LOG_I("正常回应压电陶瓷片的指令");
                } else {
                    LOG_E("错误回应压电陶瓷片的指令");
                }
                Record.shoke_cmd_ack = 0;
            }
            // --------------------------------------------------------------------
            /* 发送回应cc6201指令 */
            if(Record.cc6201_cmd_ack == 1)
            {
                if(nrf24l01_send_with_retry(_nrf24, Order_nRF24L01_ACK_CC6201_State_Cmd, NRF24_PIPE_1, 1) == RT_EOK) {
                    LOG_I("正常回应CC6201状态设置的指令");
                } else {
                    LOG_E("错误回应CC6201状态设置的指令");
                }
                Record.cc6201_cmd_ack = 0;
            }


        }
        // 一个急救流程完成状态
        else {
            MySysCfg.eyes_rgb_level = 1;
            nrf24l01_send_with_retry(_nrf24, Order_nRF24L01_SEND_To_Sensor_WS2812_Level, NRF24_PIPE_1, 5);

            // Notify Remote that CPR has stopped / completed
            if(Record.remote_connected == 1) {
                nrf24l01_send_with_retry(_nrf24, Order_nRF24L01_SEND_To_Remote_Start_Status, NRF24_PIPE_2, 1);
            }
        }



        rt_thread_mdelay(50);
    }
}



/**
  * @brief  This is a Initialization for nRF24L01
  * @retval int
  */
rt_thread_t nRF24L01_Task_Handle = RT_NULL;
rt_thread_t nRF24L01_Decode_Handle = RT_NULL;
rt_thread_t nRF24L01_Data_Transmit_Task_Handle = RT_NULL;
int nRF24L01_Thread_Init(void)
{
    nRF24L01_Task_Handle = rt_thread_create("nRF24L01_Thread_entry", nRF24L01_Thread_entry, RT_NULL, 4096, 22, 50);
    if(nRF24L01_Task_Handle != RT_NULL)
    {
        LOG_I("nRF24L01_Thread_entry is Succeed!! \r\n");
    }
    else {
        LOG_E("nRF24L01_Thread_entry is Failed \r\n");
    }

    //----------------------------------------------------------------------------------------------------------------
    nRF24L01_Decode_Handle = rt_thread_create("nRF24L01_Decode_entry", nRF24L01_Decode_entry, RT_NULL, 1024, 21, 50);
    if(nRF24L01_Decode_Handle != RT_NULL)
    {
        LOG_I("nRF24L01_Decode_entry is Succeed!! \r\n");
    }
    else {
        LOG_E("nRF24L01_Decode_entry is Failed \r\n");
    }
    //---------------------------------------------------------------------------------------------------------------------------
    nRF24L01_Data_Transmit_Task_Handle = rt_thread_create("nRF24L01_Data_Transmit_Thread_entry", nRF24L01_Data_Transmit_Thread_entry, RT_NULL, 2048, 23, 50);
    if(nRF24L01_Data_Transmit_Task_Handle != RT_NULL)
    {
        LOG_I("[nRF24L01]nRF24L01_Data_Transmit_Thread_entry is Succeed!! \r\n");
    }
    else {
        LOG_E("[nRF24L01]nRF24L01_Data_Transmit_Thread_entry is Failed \r\n");
    }

    return RT_EOK;
}











static void nrf24l01_tx_done(nrf24_t nrf24, rt_uint8_t pipe)
{
    /*! Here just want to tell the user when the role is ROLE_PTX
        the pipe have no special meaning except indicating (send) FAILED or OK
        However, it will matter when the role is ROLE_PRX*/

    if(nrf24->nrf24_cfg.config.prim_rx == ROLE_PTX)
    {
        if(pipe == NRF24_PIPE_NONE){
            rt_kprintf("tx_done failed");
        }
        else{
            rt_kprintf("tx_done ok");
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








