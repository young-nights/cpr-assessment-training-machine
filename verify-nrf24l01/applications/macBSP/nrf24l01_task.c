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
#include <rtdbg.h>
#include "bsp_nrf24l01_driver.h"

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


    /* 2. 获取中断引脚编号 */
    _nrf24->port_api.nRF24L01_IRQ_Pin_Num = GET_PIN(C, 7);


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
            rt_kprintf("RT_WAITING_FOREVER\n");
            rt_sem_take(nrf24_irq_sem, RT_WAITING_FOREVER);
        }

        // 2. 读取status状态标志，并清除中断触发标志位
        _nrf24->nrf24_flags.status = nRF24L01_Read_Status_Register(_nrf24);
        nRF24L01_Clear_Status_Register(_nrf24, NRF24BITMASK_RX_DR | NRF24BITMASK_TX_DS | NRF24BITMASK_MAX_RT);


        // 3. 角色 = 发送端（PTX）
        if(_nrf24->nrf24_cfg.config.prim_rx == ROLE_PTX)
        {
            // 3.1 读取status寄存器的 NRF24BITMASK_MAX_RT位，如果为1，说明达到最大重发次数，发送失败
            if(_nrf24->nrf24_flags.status & NRF24BITMASK_MAX_RT){
                nRF24L01_Flush_TX_FIFO(_nrf24);
                nRF24L01_Clear_Status_Register(_nrf24, NRF24BITMASK_MAX_RT);
                if(_nrf24->nrf24_cb.nrf24l01_tx_done){
                    _nrf24->nrf24_cb.nrf24l01_tx_done(_nrf24, NRF24_PIPE_NONE);
                }
            }

            // 3.2 分析哪条信道接收的数据
            uint8_t pipe = (_nrf24->nrf24_flags.status & NRF24BITMASK_RX_P_NO) >> 1;
            if(pipe == 0x07){
                LOG_I("RX FIFO Empty.\n");
            }
            else if(pipe == 0x06){
                LOG_I("Not used.\n");
            }
            else{
                LOG_I("Data pipe number(p%d).\n",pipe);
            }


            /* 3.3 收到 ACK 带载荷（PTX 也能收） */
            if(_nrf24->nrf24_flags.status & NRF24BITMASK_RX_DR){
                uint8_t rec_data[32];
                uint8_t len = nRF24L01_Read_Top_RXFIFO_Width(_nrf24);
                LOG_I("ACK Receive length = %d. \n",len);
                nRF24L01_Read_Rx_Payload(_nrf24, rec_data, len);
                if(nrf24l01_portocol_get_command(rec_data,len) == CMD_TRUE){
                    LOG_I("ACK Protocol parse succeed.\n");
                }
                else{
                    LOG_W("ACK Protocol parse failed.\n");
                }

                if(_nrf24->nrf24_cb.nrf24l01_rx_ind){
                    _nrf24->nrf24_cb.nrf24l01_rx_ind(_nrf24, rec_data, len, pipe);
                }
            }

            /* 3.4 发送完成 */
            if(_nrf24->nrf24_flags.status & NRF24BITMASK_TX_DS){
                if(_nrf24->nrf24_cb.nrf24l01_tx_done){
                    _nrf24->nrf24_cb.nrf24l01_tx_done(_nrf24, pipe);
                }
            }
        }


        // 4. 角色 = 接收端（PRX）
        if(_nrf24->nrf24_cfg.config.prim_rx == ROLE_PRX)
        {
            rt_kprintf("in\n");
            // 4.1 分析哪条信道接收的数据
            uint8_t pipe = (_nrf24->nrf24_flags.status & NRF24BITMASK_RX_P_NO) >> 1;
            if(pipe == 0x07){
                LOG_I("RX FIFO Empty.\n");
            }
            else if(pipe == 0x06){
                LOG_I("Not used.\n");
            }
            else{
                LOG_I("Data pipe number(p%d).\n",pipe);
            }

            if(pipe < 5){
                uint8_t data_buf[32];
                uint8_t length = nRF24L01_Read_Top_RXFIFO_Width(_nrf24);
                LOG_I("Receive length = %d. \n",length);
                nRF24L01_Read_Rx_Payload(_nrf24, data_buf, length);

                if(nrf24l01_portocol_get_command(data_buf,length) == CMD_TRUE){
                    LOG_I("Protocol parse succeed.\n");
                }
                else{
                    LOG_W("Protocol parse failed.\n");
                }

                if(_nrf24->nrf24_cb.nrf24l01_rx_ind){
                    _nrf24->nrf24_cb.nrf24l01_rx_ind(_nrf24, data_buf, length, pipe);
                }

            }
        }

        rt_thread_mdelay(500);
    }
}





void nRF24L01_Decode_Thread_entry(void* parameter)
{

    for(;;)
    {
        /* 主循环 PTX 段末尾（发完询问包之后） */
        if (Record.nRF24_tx_pending) {
            rt_kprintf("now ptx pening\n");
            Record.nRF24_tx_pending = 0;              /* 先清标志，防止重入 */

            _nrf24->nrf24_ops.nrf24_reset_ce();
            nRF24L01_Set_Role_Mode(_nrf24, ROLE_PTX);

            nrf24l01_order_to_pipe(Order_nRF24L01_ACK_Connect_Control_Panel, NRF24_PIPE_2);

            _nrf24->nrf24_ops.nrf24_set_ce();
            rt_thread_mdelay(1);             /* 1 ms 足够发完 */
            _nrf24->nrf24_ops.nrf24_reset_ce();
            nRF24L01_Set_Role_Mode(_nrf24, ROLE_PRX);
            _nrf24->nrf24_ops.nrf24_set_ce();
            /* 后面继续正常 PRX 窗口即可 */
        }
        rt_thread_mdelay(200);
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
    nRF24L01_Task_Handle = rt_thread_create("nRF24L01_Thread_entry", nRF24L01_Thread_entry, RT_NULL, 4096, 9, 50);
    /* 检查是否创建成功,成功就启动线程 */
    if(nRF24L01_Task_Handle != RT_NULL)
    {
        LOG_I("[nRF24L01]nRF24L01_Thread_entry is Succeed!! \r\n");
        rt_thread_startup(nRF24L01_Task_Handle);
    }
    else {
        LOG_E("[nRF24L01]nRF24L01_Thread_entry is Failed \r\n");
    }



    nRF24L01_Decode_Task_Handle = rt_thread_create("nRF24L01_Decode_Thread_entry", nRF24L01_Decode_Thread_entry, RT_NULL, 4096, 8, 50);
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
            rt_kprintf("tx_done failed");
        }
        else{
            rt_kprintf("tx_done ok");
        }
    }
}




static void nrf24l01_rx_ind(nrf24_t nrf24, uint8_t *data, uint8_t len, int pipe)
{
    /*! Don't need to care the pipe if the role is ROLE_PTX */
    rt_kprintf("(p%d): ", pipe);
    rt_kprintf((char *)data);
}



const static struct nrf24_callback g_cb = {
    .nrf24l01_rx_ind = nrf24l01_rx_ind,
    .nrf24l01_tx_done = nrf24l01_tx_done,
};








