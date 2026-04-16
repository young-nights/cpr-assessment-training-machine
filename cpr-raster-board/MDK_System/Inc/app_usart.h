#ifndef __USART_H
#define	__USART_H
#include "app_sys.h"



#define  UART_DATALENGTH 64

typedef struct
{
    u8 Receive_Buffer[UART_DATALENGTH];//数据缓冲区
    u8 *receive_last;//接收末尾
    u8 *get_last;//获取末尾
    u8 receive_signal_flag;//有收到数据标志位
    u8 receive_full_flag;//接收满标志位
    u8 receive_length;//接收长度
} USART_ReceiveDataTypedef; //定义队列数据类型
extern USART_ReceiveDataTypedef    USART1_QueueBuf;






void    UART1_Config(void);
void    USART1_SendData(u8* buffer, u16 size);
void    UART1_ReceiveValueInit(USART_ReceiveDataTypedef* Uart_Device_Rx, uint16_t Length);
void    UART1_Receive(USART_ReceiveDataTypedef* Uart_Device_Rx, uint8_t Data);
uint8_t UART1_GetByte(USART_ReceiveDataTypedef* Uart_Device_Rx);





#endif /*__USART_H*/
