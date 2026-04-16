#include "app_usart.h"
#include "stdio.h"
#include "string.h"


USART_ReceiveDataTypedef    USART1_QueueBuf;


/**
  * @brief  UART1 串口配置函数
  * @param  None
  * @retval UART1-RX   -->  PA4
  *         UART1-TX   -->  PA5
  */
void UART1_Config(void)
{
    /* 复位所有的 UART1 相关的串口寄存器 */
    UART1_DeInit();

    /* 开启 UART1 的时钟 */
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, ENABLE);

  /* UART1 configured as follow:
          - Word Length = 8 Bits
          - One Stop Bit
          - No parity
          - BaudRate = 9600 baud
          - Tx and Rx enabled
          - UART1 Clock disabled
  */
    UART1_Init(9600, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE); 

    /* 使能UART1接收中断，中断向量号为18 */
    UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);

    /* 使能 UART1 */
    UART1_Cmd(ENABLE);
    

    /* 初始化，带上拉输入，不带中断 */
    GPIO_Init(GPIOD, GPIO_PIN_6, GPIO_MODE_IN_PU_NO_IT);
    /* 初始化，推挽输出，快速 */
    GPIO_Init(GPIOD, GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST);

}






/**
  * @brief  UART1 串口发送数据
  * @param  *buffer   要发送的数据
  *         size      要发送的数组的大小
  * @retval None
  */
void USART1_SendData(u8 * buffer , u16 size )
{
    u16 i = 0;

    for ( i = 0; i < size; i++ )
    {
        UART1_SendData8( *buffer );

        while ( UART1_GetFlagStatus( UART1_FLAG_TC ) == RESET );

        buffer++;
    }
}








/**
  * @brief  UART1 串口环形队列元素数据初始化
  * @param  *Uart_Device_Rx   接受数据结构体类型
  *         Length            数据长度
  * @retval None
  */
void    UART1_ReceiveValueInit(USART_ReceiveDataTypedef* Uart_Device_Rx, uint16_t Length)
{
    Uart_Device_Rx->receive_last = Uart_Device_Rx->Receive_Buffer;
    Uart_Device_Rx->get_last = Uart_Device_Rx->Receive_Buffer;
    Uart_Device_Rx->receive_length = Length;
    Uart_Device_Rx->receive_signal_flag = 0;
}






/**
  * @brief  UART1 串口接收函数
  * @param  *Uart_Device_Rx   接受数据结构体类型
  *         Data              数据内容
  * @retval None
  */
void UART1_Receive(USART_ReceiveDataTypedef* Uart_Device_Rx, uint8_t Data)
{
    if (!Uart_Device_Rx->receive_full_flag)
    {
        *(Uart_Device_Rx->receive_last) = Data; 
        Uart_Device_Rx->receive_last++;
        Uart_Device_Rx->receive_signal_flag = 1;

        if (Uart_Device_Rx->receive_last >= Uart_Device_Rx->Receive_Buffer + Uart_Device_Rx->receive_length)
        {
            Uart_Device_Rx->receive_last = Uart_Device_Rx->Receive_Buffer;
        }
        if (Uart_Device_Rx->receive_last == Uart_Device_Rx->get_last)
        {
            Uart_Device_Rx->receive_full_flag = 1;
        }
    }
}




/**
  * @brief  UART1 串口获取一个字节数据函数
  * @param  *Uart_Device_Rx   接受数据结构体类型
  * @retval None
  */
uint8_t UART1_GetByte(USART_ReceiveDataTypedef* Uart_Device_Rx)
{
    uint8_t data = 0;

    if (!Uart_Device_Rx->receive_signal_flag)
    {
        return 0;
    }

    data = *(Uart_Device_Rx->get_last);
    Uart_Device_Rx->get_last++;
    Uart_Device_Rx->receive_full_flag = 0;

    if (Uart_Device_Rx->get_last >= Uart_Device_Rx->Receive_Buffer + Uart_Device_Rx->receive_length)
    {
        Uart_Device_Rx->get_last = Uart_Device_Rx->Receive_Buffer;
    }

    if (Uart_Device_Rx->get_last == Uart_Device_Rx->receive_last)
    {
        Uart_Device_Rx->receive_signal_flag = 0;
    }
    return  data;
}








