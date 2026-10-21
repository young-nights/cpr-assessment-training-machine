/**
 * @file    raster_uart2.c
 * @brief   USART2 Sensor board link (PA2=TX, PA3=RX, 115200bps)
 *
 * Frame formats (compatible with Sensor uart2_protocol.c):
 *   Command  (Sensor -> Raster): 0xAA LEN CMD DATA CHK 0x55, LEN=0x02
 *   Realtime (Raster -> Sensor): 0xAA 0x04 TYPE CNT_H CNT_L DIR CHK 0x55
 *   CHK = sum of all bytes before CHK (8-bit)
 *
 * Change Logs:
 * Date           Author      Notes
 * 2026-10-21     coder       the first version
 */
#include "raster_sys.h"

#define RX_RING_SIZE    64U

static volatile uint8_t s_rx_ring[RX_RING_SIZE];
static volatile uint8_t s_rx_head = 0;
static volatile uint8_t s_rx_tail = 0;

/**
 * @brief  Initialize USART2 (PA2=TX, PA3=RX, 115200bps 8N1, RX interrupt)
 */
void UART2_Init(void)
{
    /* Enable USART2 clock (APB1) */
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    /* BRR = APB1_FREQ / baud = 36000000 / 115200 = 312.5 ~= 313 = 0x139 */
    USART2->BRR = 313;

    /* Enable TX, RX, RX interrupt, then USART */
    USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;
    USART2->CR1 |= USART_CR1_UE;

    /* NVIC: enable USART2 */
    NVIC_ISER(IRQn_USART2 / 32) = (1UL << (IRQn_USART2 % 32));
}

/**
 * @brief  USART2 interrupt: push received byte into ring buffer
 */
void USART2_IRQHandler(void)
{
    if ((USART2->SR & USART_SR_RXNE) && (USART2->CR1 & USART_CR1_RXNEIE)) {
        uint8_t byte = (uint8_t)(USART2->DR & 0xFF);
        uint8_t next = (uint8_t)((s_rx_head + 1U) % RX_RING_SIZE);
        if (next != s_rx_tail) {
            s_rx_ring[s_rx_head] = byte;
            s_rx_head = next;
        }
    }
}

/**
 * @brief  Pop one byte from RX ring buffer
 * @retval 1 if a byte was read, 0 if empty
 */
int UART2_RecvByte(uint8_t *byte)
{
    if (s_rx_tail == s_rx_head) {
        return 0;
    }
    *byte = s_rx_ring[s_rx_tail];
    s_rx_tail = (uint8_t)((s_rx_tail + 1U) % RX_RING_SIZE);
    return 1;
}

/**
 * @brief  Transmit raw bytes on USART2 (blocking)
 */
void UART2_SendFrame(const uint8_t *buf, uint8_t len)
{
    for (uint8_t i = 0; i < len; i++) {
        while (!(USART2->SR & USART_SR_TXE))
            ;
        USART2->DR = buf[i];
    }
    while (!(USART2->SR & USART_SR_TC))
        ;
}

/**
 * @brief  Poll RX ring and parse Sensor command frames
 *
 *         State machine mirrors Sensor-side parser:
 *         WAIT_HEAD -> WAIT_LEN(=0x02) -> RECV_DATA(4 bytes) -> WAIT_TAIL
 *         On valid frame, apply CMD_START / CMD_STOP / CMD_MODE.
 */
void UART2_PollCommand(raster_state_et *state, raster_mode_et *mode)
{
    static uint8_t parse_buf[6];
    static uint8_t parse_idx = 0;
    static uint8_t phase = 0;   /* 0=HEAD 1=LEN 2=DATA 3=TAIL */
    static uint32_t start_tick = 0;
    uint8_t byte;

    /* 50ms incomplete-frame timeout reset */
    if (phase != 0 && (SysTick_GetTick() - start_tick) > 50U) {
        phase = 0;
        parse_idx = 0;
    }

    while (UART2_RecvByte(&byte)) {
        switch (phase) {
            case 0: /* wait head */
                if (byte == FRAME_HEAD) {
                    start_tick = SysTick_GetTick();
                    parse_idx = 0;
                    phase = 1;
                }
                break;

            case 1: /* wait LEN */
                if (byte == FRAME_HEAD) {
                    parse_idx = 0;
                } else if (byte == LEN_CMD) {
                    parse_buf[parse_idx++] = byte;
                    phase = 2;
                } else {
                    phase = 0;
                }
                break;

            case 2: /* receive CMD + DATA + CHK (3 bytes) */
                if (byte == FRAME_HEAD) {
                    parse_idx = 0;
                    phase = 1;
                } else {
                    parse_buf[parse_idx++] = byte;
                    if (parse_idx >= 4U) {  /* LEN+CMD+DATA+CHK */
                        phase = 3;
                    }
                }
                break;

            case 3: /* wait tail */
                if (byte == FRAME_TAIL) {
                    /* CHK: sum of 0xAA + LEN + CMD + DATA */
                    uint8_t calc = (uint8_t)(FRAME_HEAD + parse_buf[0] +
                                             parse_buf[1] + parse_buf[2]);
                    if (calc == parse_buf[3]) {
                        uint8_t cmd  = parse_buf[1];
                        uint8_t data = parse_buf[2];

                        if (cmd == CMD_START) {
                            *state = RASTER_STATE_ACTIVE;
                            Raster_TakePressPulse();
                            Raster_TakeBlowPulse();
                            UART1_Print("[RASTER] CMD START -> ACTIVE\r\n");
                        } else if (cmd == CMD_STOP) {
                            *state = RASTER_STATE_IDLE;
                            UART1_Print("[RASTER] CMD STOP -> IDLE\r\n");
                        } else if (cmd == CMD_MODE) {
                            *mode = (raster_mode_et)data;
                            UART1_Printf("[RASTER] MODE -> 0x%x\r\n", data);
                        }
                    } else {
                        UART1_Printf("[RASTER] CMD CHK error calc=0x%x recv=0x%x\r\n",
                                     calc, parse_buf[3]);
                    }
                }
                phase = 0;
                parse_idx = 0;
                break;

            default:
                phase = 0;
                parse_idx = 0;
                break;
        }
    }
}
