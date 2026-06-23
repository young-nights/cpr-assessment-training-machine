/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-11-02     Administrator       the first version
 */
#include "bsp_sys.h"

extern void coreless_motor_ctrl(MOTOR_NAME_et name, SWITCH_et status);

#define DBG_TAG "[nRF24L01]"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>


const static struct nrf24_callback g_cb;
rt_sem_t nrf24_send_sem = RT_NULL;
rt_sem_t nrf24_irq_sem = RT_NULL;
rt_mutex_t nrf24_mutex = RT_NULL;

/* Use static allocation instead of malloc to save heap memory */
static struct nRF24L01_STRUCT _nrf24_static;
nrf24_t _nrf24 = &_nrf24_static;


/**
  * @brief  This thread entry is used for nRF24L01
  * @retval void
  */
void nRF24L01_Thread_entry(void* parameter)
{
    /* _nrf24 uses static allocation, no malloc needed */
    rt_memset(_nrf24, 0, sizeof(struct nRF24L01_STRUCT));
    LOG_I("LOG:%d. nrf24 static alloc ok.",Record.ulog_cnt++);


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
    _nrf24->port_api.nRF24L01_IRQ_Pin_Num = GET_PIN(D, 2);


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
    rt_kprintf("running transmitter.\r\n");

    rt_tick_t last_connect_send = 0;
    rt_tick_t last_report_tick = 0;
    uint8_t  connect_retry_cnt = 0;

    for (;;)
    {
        /* ====================== 未连接：持续发送连接请求 ====================== */
        if(Record.nrf_if_connected == 0)
        {
            if (rt_tick_get() - last_connect_send >= 500)   // 每 500ms 发送一次连接请求
            {
                _nrf24->nrf24_ops.nrf24_reset_ce();
                nRF24L01_Set_Role_Mode(_nrf24, ROLE_PTX);

                nrf24l01_order_to_pipe(_nrf24, Order_nRF24L01_ASK_Connect_Control_Panel, NRF24_PIPE_1);

                _nrf24->nrf24_ops.nrf24_set_ce();
                rt_thread_mdelay(5);                    // 给发送一点稳定时间
                _nrf24->nrf24_ops.nrf24_reset_ce();

                last_connect_send = rt_tick_get();
                connect_retry_cnt++;
                LOG_I("Sensor → Mainboard: Send connect request... (retry %d)", connect_retry_cnt);

            }

            // 打开短接收窗口，等待主板 ACK
            _nrf24->nrf24_ops.nrf24_reset_ce();
            nRF24L01_Flush_RX_FIFO(_nrf24);              // 清空旧数据
            nRF24L01_Clear_IRQ_Flags(_nrf24);            // 清除旧中断标志
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
            // 先检查 FIFO 是否已有数据（IRQ 可能在切换前已触发）
            uint8_t precheck_status = nRF24L01_Read_Status_Register(_nrf24);
            if (precheck_status & NRF24BITMASK_RX_DR)
            {
                uint8_t len = nRF24L01_Read_Top_RXFIFO_Width(_nrf24);
                if (len > 0 && len <= 32)
                {
                    uint8_t rec_data[32];
                    nRF24L01_Read_Rx_Payload(_nrf24, rec_data, len);
                    nRF24L01_Clear_IRQ_Flags(_nrf24);

                    cpr_src_type_t src = SRC_UNKNOWN;
                    if (nrf24l01_portocol_get_command(rec_data, len, &src) == CMD_TRUE)
                    {
                        LOG_I("PRX rec_data → `nrf24l01_portocol_get_command()` == ASK_Connect_ACK → connected + LOG (precheck, src=%d)", src);
                        connect_retry_cnt = 0;
                    }
                }
            }

            if (rt_sem_take(nrf24_irq_sem, 200) == RT_EOK)
            {
                _nrf24->nrf24_flags.status = nRF24L01_Read_Status_Register(_nrf24);
                nRF24L01_Clear_IRQ_Flags(_nrf24);

                if (_nrf24->nrf24_flags.status & NRF24BITMASK_RX_DR)
                {
                    uint8_t len = nRF24L01_Read_Top_RXFIFO_Width(_nrf24);
                    if (len > 0 && len <= 32)
                    {
                        uint8_t rec_data[32];
                        nRF24L01_Read_Rx_Payload(_nrf24, rec_data, len);

                        cpr_src_type_t src = SRC_UNKNOWN;
                        if (nrf24l01_portocol_get_command(rec_data, len, &src) == CMD_TRUE)
                        {
                            LOG_I("PRX rec_data → `nrf24l01_portocol_get_command()` == ASK_Connect_ACK → connected + LOG (irq, src=%d)", src);
                            connect_retry_cnt = 0;
                        }
                    }
                }
            }
            _nrf24->nrf24_ops.nrf24_reset_ce();
            nRF24L01_Set_Role_Mode(_nrf24, ROLE_PTX);
        }


        /* ====================== 已连接：正常数据上报 + 偶尔指令处理 ====================== */
        else
        {

            //----------------------------------------------------------------------------------------
            //||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
            //                           TODO: 上报数据和上报指令
            //||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
            //----------------------------------------------------------------------------------------
            if (rt_tick_get() - last_report_tick >= 60)
            {

                // ----------------------------------------------------------------------------------
                if(Record.nrf_rec_start_cmd == 1){  // 主动发送接收到start信号的 ack
                    nrf24l01_send_with_retry(_nrf24, Order_nRF24L01_ACK_Start_Cmd, NRF24_PIPE_1, 5);
                    Record.nrf_rec_start_cmd = 0;
                }
                // ----------------------------------------------------------------------------------
                if(Flag.shoke_ack == 1 && Flag.start == 1){
                    nrf24l01_send_with_retry(_nrf24, Order_nRF24L01_ASK_Shoke_Sensor_Cmd, NRF24_PIPE_1, 1);
                }
                // ----------------------------------------------------------------------------------
                if(Flag.ws2812b_ack == 1 && Flag.start == 1){ // 接收到mainboard主动发送来的指令，就需要循环多发
                    nrf24l01_send_with_retry(_nrf24, Order_nRF24L01_ACK_WS2812_Level_Cmd, NRF24_PIPE_1, 5);
                    Flag.ws2812b_ack = 0;
                }
                // ----------------------------------------------------------------------------------
                if(Flag.motor_ack == 1 && Flag.start == 1){
                    nrf24l01_send_with_retry(_nrf24, Order_nRF24L01_ACK_Motor_Status_Cmd, NRF24_PIPE_1, 5);
                    Flag.motor_ack = 0;
                }
                // ----------------------------------------------------------------------------------
                if(Flag.cc6201_ack == 1 && Flag.start == 1){
                    nrf24l01_send_with_retry(_nrf24, Order_nRF24L01_ASK_CC6201_Cmd, NRF24_PIPE_1, 1);
                }
                // ----------------------------------------------------------------------------------


                last_report_tick = rt_tick_get();
            }

            //----------------------------------------------------------------------------------------
            //||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
            //             已连接后，周期性打开短接收窗口接收主板指令（不需要太频繁）
            //||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
            //----------------------------------------------------------------------------------------
            if (rt_tick_get() % 8 == 0)   // 大约每 160ms（8*20ms）听一次
            {
                _nrf24->nrf24_ops.nrf24_reset_ce();
                nRF24L01_Set_Role_Mode(_nrf24, ROLE_PRX);
                _nrf24->nrf24_ops.nrf24_set_ce();

                if (rt_sem_take(nrf24_irq_sem, 50) == RT_EOK)
                {
                    _nrf24->nrf24_flags.status = nRF24L01_Read_Status_Register(_nrf24);
                    nRF24L01_Clear_IRQ_Flags(_nrf24);

                    if (_nrf24->nrf24_flags.status & NRF24BITMASK_RX_DR)
                    {
                        uint8_t len = nRF24L01_Read_Top_RXFIFO_Width(_nrf24);
                        if (len > 0 && len <= 32)
                        {
                            uint8_t rec_data[32];
                            nRF24L01_Read_Rx_Payload(_nrf24, rec_data, len);

                            cpr_src_type_t src = SRC_UNKNOWN;
                            if (nrf24l01_portocol_get_command(rec_data, len, &src) == CMD_TRUE){}
                        }
                    }
                }
                _nrf24->nrf24_ops.nrf24_reset_ce();
                nRF24L01_Set_Role_Mode(_nrf24, ROLE_PTX);
            }
        }
        rt_thread_mdelay(20);
    }
}




/**
  * @brief  This is a Initialization for nRF24L01
  * @retval int
  */
rt_thread_t nRF24L01_Task_Handle = RT_NULL;
int nRF24L01_Thread_Init(void)
{
    nRF24L01_Task_Handle = rt_thread_create("nRF24L01_Thread_entry", nRF24L01_Thread_entry, RT_NULL, 2048, 22, 100);
    /* 检查是否创建成功,成功就启动线程 */
    if(nRF24L01_Task_Handle != RT_NULL)
    {
        LOG_I("LOG:%d. nRF24L01_Thread_entry is Succeed.",Record.ulog_cnt++);
        rt_thread_startup(nRF24L01_Task_Handle);
    }
    else {
        LOG_E("LOG:%d. nRF24L01_Thread_entry is Failed",Record.ulog_cnt++);
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
