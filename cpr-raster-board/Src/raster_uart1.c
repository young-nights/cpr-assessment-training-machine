/**
 * @file    raster_uart1.c
 * @brief   USART1 debug console (PA9=TX, PA10=RX, 115200bps)
 *
 * Change Logs:
 * Date           Author      Notes
 * 2026-10-21     coder       the first version
 */
#include "raster_sys.h"
#include <stdarg.h>

/**
 * @brief  Initialize USART1 (PA9=TX, PA10=RX, 115200bps 8N1)
 */
void UART1_Init(void)
{
    /* Enable USART1 clock */
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    /* BRR = APB2_FREQ / baud = 72000000 / 115200 = 625 = 0x271 */
    USART1->BRR = 625;

    /* Enable TX and RX, then USART */
    USART1->CR1 = USART_CR1_TE | USART_CR1_RE;
    USART1->CR1 |= USART_CR1_UE;
}

/**
 * @brief  Send a single character
 */
static void uart1_putc(char c)
{
    while (!(USART1->SR & USART_SR_TXE))
        ;
    USART1->DR = (uint8_t)c;
}

/**
 * @brief  Send a null-terminated string
 */
void UART1_Print(const char *str)
{
    while (*str) {
        uart1_putc(*str++);
    }
}

/**
 * @brief  Simple printf implementation (supports %d, %u, %x, %s, %c)
 */
void UART1_Printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    char buf[16];
    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 'd': {
                    int val = va_arg(args, int);
                    if (val < 0) {
                        uart1_putc('-');
                        val = -val;
                    }
                    int i = 0;
                    if (val == 0) buf[i++] = '0';
                    while (val > 0) {
                        buf[i++] = (char)('0' + (val % 10));
                        val /= 10;
                    }
                    while (i > 0) uart1_putc(buf[--i]);
                    break;
                }
                case 'u': {
                    unsigned val = va_arg(args, unsigned);
                    int i = 0;
                    if (val == 0) buf[i++] = '0';
                    while (val > 0) {
                        buf[i++] = (char)('0' + (val % 10));
                        val /= 10;
                    }
                    while (i > 0) uart1_putc(buf[--i]);
                    break;
                }
                case 'x': {
                    unsigned val = va_arg(args, unsigned);
                    int i = 0;
                    if (val == 0) buf[i++] = '0';
                    while (val > 0) {
                        int d = (int)(val & 0xF);
                        buf[i++] = (char)((d < 10) ? ('0' + d) : ('a' + d - 10));
                        val >>= 4;
                    }
                    while (i > 0) uart1_putc(buf[--i]);
                    break;
                }
                case 's': {
                    const char *s = va_arg(args, const char *);
                    while (*s) uart1_putc(*s++);
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    uart1_putc(c);
                    break;
                }
                default:
                    uart1_putc('%');
                    uart1_putc(*fmt);
                    break;
            }
        } else {
            uart1_putc(*fmt);
        }
        fmt++;
    }

    va_end(args);
}
