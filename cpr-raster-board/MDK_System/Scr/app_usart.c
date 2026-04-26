#include "app_usart.h"
#include "stdio.h"
#include "string.h"


USART_ReceiveDataTypedef    USART1_QueueBuf;


/**
  * @brief  UART1 �������ú���
  * @param  None
  * @retval UART1-RX   -->  PA4
  *         UART1-TX   -->  PA5
  */
void UART1_Config(void)
{
    /* ��λ���е� UART1 ��صĴ��ڼĴ��� */
    UART1_DeInit();

    /* ���� UART1 ��ʱ�� */
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, ENABLE);

  /* UART1 configured as follow:
          - Word Length = 8 Bits
          - One Stop Bit
          - No parity
          - BaudRate = 115200 baud
          - Tx and Rx enabled
          - UART1 Clock disabled
  */
    UART1_Init(115200, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE); 

    /* ʹ��UART1�����жϣ��ж�������Ϊ18 */
    UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);

    /* ʹ�� UART1 */
    UART1_Cmd(ENABLE);
    

    /* ��ʼ�������������룬�����ж� */
    GPIO_Init(GPIOD, GPIO_PIN_6, GPIO_MODE_IN_PU_NO_IT);
    /* ��ʼ����������������� */
    GPIO_Init(GPIOD, GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST);

}






/**
  * @brief  UART1 ���ڷ�������
  * @param  *buffer   Ҫ���͵�����
  *         size      Ҫ���͵�����Ĵ�С
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
  * @brief  UART1 ���ڻ��ζ���Ԫ�����ݳ�ʼ��
  * @param  *Uart_Device_Rx   �������ݽṹ������
  *         Length            ���ݳ���
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
  * @brief  UART1 ���ڽ��պ���
  * @param  *Uart_Device_Rx   �������ݽṹ������
  *         Data              ��������
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
  * @brief  UART1 ���ڻ�ȡһ���ֽ����ݺ���
  * @param  *Uart_Device_Rx   �������ݽṹ������
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








