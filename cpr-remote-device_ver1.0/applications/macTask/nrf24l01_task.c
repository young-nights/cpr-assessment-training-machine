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
#include "bsp_nrf24l01_driver.h"

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
/**
  * @brief  This thread entry is used for nRF24L01
  * @retval void
  */
void nRF24L01_Thread_entry(void* parameter)
{

    /* 0. 给nrf24开创一个实际空间 */
    _nrf24 = malloc(sizeof(nrf24_t));
    if (_nrf24 == NULL) {
        LOG_E("LOG:%d. nrf24 malloc error.",Record.ulog_cnt++);
    }
    else{
        LOG_I("LOG:%d. nrf24 malloc successful.",Record.ulog_cnt++);
    }


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

    for(;;)
    {
        /* 尚未连接则持续广播 */
        if(Record.nrf_if_connected == 0){
            /* ----------  1. PTX 发送  ---------- */
            _nrf24->nrf24_ops.nrf24_reset_ce();
            nRF24L01_Set_Role_Mode(_nrf24, ROLE_PTX);
            nrf24l01_order_to_pipe(_nrf24, Order_nRF24L01_ASK_Connect_Control_Panel,NRF24_PIPE_2);
            _nrf24->nrf24_ops.nrf24_set_ce();
            rt_thread_mdelay(1);
            _nrf24->nrf24_ops.nrf24_reset_ce();

            /* ----------  2. 立即进入 PRX 接收窗口  ---------- */
            nRF24L01_Set_Role_Mode(_nrf24, ROLE_PRX);          /* 切 PRX */
            _nrf24->nrf24_ops.nrf24_set_ce();                  /* 开始监听 */

            /* ----------  3. 限时等待 IRQ（ACK 或独立回包）  ---------- */
            rt_err_t  rx_ok = rt_sem_take(nrf24_irq_sem, 100);

            /* ----------  4. 处理本次 IRQ  ---------- */
            if(rx_ok == RT_EOK)
            {
                /* 读 STATUS 并清中断 */
                _nrf24->nrf24_flags.status = nRF24L01_Read_Status_Register(_nrf24);
                nRF24L01_Clear_IRQ_Flags(_nrf24);
                /* 4.1 收到数据（ACK-Payload 或独立包） */
                if(_nrf24->nrf24_flags.status & NRF24BITMASK_RX_DR)
                {
                    uint8_t len, rec_data[32];
                    len = nRF24L01_Read_Top_RXFIFO_Width(_nrf24);
                    nRF24L01_Read_Rx_Payload(_nrf24, rec_data, len);

                    if(nrf24l01_portocol_get_command(rec_data, len) == CMD_TRUE)
                    {
                        LOG_I("Protocol parse succeed.");
                    }
                    else
                    {
                        LOG_W("Protocol parse failed.");
                    }

                    uint8_t pipe = (_nrf24->nrf24_flags.status & NRF24BITMASK_RX_P_NO) >> 1;
                    if(_nrf24->nrf24_cb.nrf24l01_rx_ind){
                        _nrf24->nrf24_cb.nrf24l01_rx_ind(_nrf24, rec_data, len, pipe);
                    }
                }

                /* 4.2 发送成功 */
                if(_nrf24->nrf24_flags.status & NRF24BITMASK_TX_DS)
                {
                    rt_thread_mdelay(100);
                }

                /* 4.3 重发超限 */
                 if(_nrf24->nrf24_flags.status & NRF24BITMASK_MAX_RT)
                 {
                     nRF24L01_Flush_TX_FIFO(_nrf24);
                     LOG_W("TX timeout.");
                 }
            }
            else
            {
                // 超时重新请求
                LOG_W("RX window timeout.");
                _nrf24->nrf24_ops.nrf24_reset_ce();
                nRF24L01_Set_Role_Mode(_nrf24, ROLE_PTX);
                nrf24l01_order_to_pipe(_nrf24, Order_nRF24L01_ASK_Connect_Control_Panel,NRF24_PIPE_2);
                _nrf24->nrf24_ops.nrf24_set_ce();
                rt_thread_mdelay(1);
                _nrf24->nrf24_ops.nrf24_reset_ce();

            }
            /* ----------  5. 窗口结束，切回 PTX  ---------- */
            _nrf24->nrf24_ops.nrf24_reset_ce();
            nRF24L01_Set_Role_Mode(_nrf24, ROLE_PTX);
        }
        /* 已连接后的业务循环（示例：每 400 ms 心跳） */
        else
        {
            /* 这里放正常双向通信代码 */
            rt_thread_mdelay(400);
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
rt_thread_t nRF24L01_Decode_Task_Handle = RT_NULL;
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
