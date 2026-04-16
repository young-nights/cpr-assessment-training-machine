/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-11-21     18452       the first version
 */
#ifndef APPLICATIONS_MACBSP_INC_BSP_RS232_DRV_H_
#define APPLICATIONS_MACBSP_INC_BSP_RS232_DRV_H_

#include "bsp_sys.h"


#ifdef __cplusplus
extern "C"
{
#endif


//#define RS232_USING_TEST            //使用测试功能
//#define RS232_USING_SAMPLE_SLAVE    //使用从机示例
//#define RS232_USING_SAMPLE_MASTER   //使用主机示例
#define RS232_USING_DMA_RX          //使用DMA接收
//#define RS232_USING_INT_TX          //使用中断发送
#define RS232_USING_DMA_TX          //使用DMA发送


#ifndef RS232_SW_DLY_US
#define RS232_SW_DLY_US             0    //发送引脚控制切换延时
#endif

#define RS232_TX_COMP_TMO_MAX       (3 * RT_TICK_PER_SECOND)//最大DMA传输完成超时
#define RS232_BYTE_TMO_MIN          2     //最小字节超时
#define RS232_BYTE_TMO_MAX          200   //最大字节超时



// 创建RS458实例(instance)
typedef struct rs232_inst rs232_inst_t;

#ifdef RS485_USING_DEV
#include <rs232_dev.h>
#endif

/*
 * @brief   create rs232 instance dynamically
 * @param   serial      - serial device name
 * @param   baudrate    - serial baud rate
 * @param   parity      - serial parity mode
 * @param   pin         - mode contrle pin
 * @param   level       - send mode level
 * @retval  instance handle
 */
rs232_inst_t * rs232_create(const char *serial, int baudrate, int parity);

/*
 * @brief   destory rs232 instance created dynamically
 * @param   hinst       - instance handle
 * @retval  0 - success, other - error
 */
int rs232_destory(rs232_inst_t * hinst);

/*
 * @brief   config rs232 params
 * @param   hinst       - instance handle
 * @param   baudrate    - baudrate of communication
 * @param   databits    - data bits, 5~8
 * @param   parity      - parity bit, 0~2, 0 - none, 1 - odd, 2 - even
 * @param   stopbits    - stop bits, 0~1, 0 - 1 stop bit, 1 - 2 stop bits
 * @retval  0 - success, other - error
 */
int rs232_config(rs232_inst_t * hinst, int baudrate, int databits, int parity, int stopbits);

/*
 * @brief   set wait datas timeout for receiving
 * @param   hinst       - instance handle
 * @param   tmo_ms      - receive wait timeout, 0--no wait, <0--wait forever, >0--wait timeout, default = 0
 * @retval  0 - success, other - error
 */
int rs232_set_recv_tmo(rs232_inst_t * hinst, int tmo_ms);

/*
 * @brief   set byte interval timeout for receiving
 * @param   hinst       - instance handle
 * @param   tmo_ms      - byte interval timeout, default is calculated from baudrate
 * @retval  0 - success, other - error
 */
int rs232_set_byte_tmo(rs232_inst_t * hinst, int tmo_ms);

/*
 * @brief   open rs232 connect
 * @param   hinst       - instance handle
 * @retval  0 - success, other - error
 */
int rs232_connect(rs232_inst_t * hinst);

/*
 * @brief   close rs232 connect
 * @param   hinst       - instance handle
 * @retval  0 - success, other - error
 */
int rs232_disconn(rs232_inst_t * hinst);

/*
 * @brief   receive datas from rs232
 * @param   hinst       - instance handle
 * @param   buf         - buffer addr
 * @param   size        - maximum length of received datas
 * @retval  >=0 - length of received datas, <0 - error
 */
int rs232_recv(rs232_inst_t * hinst, void *buf, int size);

/*
 * @brief   send datas to rs485
 * @param   hinst       - instance handle
 * @param   buf         - buffer addr
 * @param   size        - length of send datas
 * @retval  >=0 - length of sent datas, <0 - error
 */
int rs232_send(rs232_inst_t * hinst, void *buf, int size);

/*
 * @brief   break rs232 receive wait
 * @param   hinst       - instance handle
 * @retval  0 - success, other - error
 */
int rs232_break_recv(rs232_inst_t * hinst);

/*
 * @brief   send data to rs232 and then receive response data from rs232
 * @param   hinst       - instance handle
 * @param   send_buf    - send buffer addr
 * @param   send_len    - length of send datas
 * @param   recv_buf    - recv buffer addr
 * @param   recv_size   - maximum length of received datas
 * @retval  >=0 - length of received datas, <0 - error
 */
int rs232_send_then_recv(rs232_inst_t * hinst, void *send_buf, int send_len, void *recv_buf, int recv_size);



#ifdef __cplusplus
}

#endif






#endif /* APPLICATIONS_MACBSP_INC_BSP_RS232_DRV_H_ */
