/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-05-12     coder        the first version - OLED eye driver (SSD1306, bit-bang I2C)
 * 2026-05-12     coder        migrated from SSD1306 (128x64) to ST7315 (64x48)
 */

#include "bsp_oled_eye.h"

#define DBG_TAG "[OLED_EYE]"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

/* --- Pin definitions (PC10 = SDA, PC11 = SCL) --- */
#define OLED_SDA_PORT       GPIOC
#define OLED_SDA_PIN        GPIO_PIN_10
#define OLED_SCL_PORT       GPIOC
#define OLED_SCL_PIN        GPIO_PIN_11

/* --- Bit-bang I2C timing (short delay for ~100kHz) --- */
#define I2C_DELAY()         do { volatile uint32_t _d = 20; while(_d--); } while(0)

/* --- GPIO helpers --- */
#define SDA_HIGH()          HAL_GPIO_WritePin(OLED_SDA_PORT, OLED_SDA_PIN, GPIO_PIN_SET)
#define SDA_LOW()           HAL_GPIO_WritePin(OLED_SDA_PORT, OLED_SDA_PIN, GPIO_PIN_RESET)
#define SCL_HIGH()          HAL_GPIO_WritePin(OLED_SCL_PORT, OLED_SCL_PIN, GPIO_PIN_SET)
#define SCL_LOW()           HAL_GPIO_WritePin(OLED_SCL_PORT, OLED_SCL_PIN, GPIO_PIN_RESET)
#define SDA_READ()          HAL_GPIO_ReadPin(OLED_SDA_PORT, OLED_SDA_PIN)

/* --- Framebuffer: 64x6 = 384 bytes (ST7315 64x48) --- */
static uint8_t framebuffer[OLED_WIDTH * OLED_PAGES];

/* ================================================================
 *  Bit-bang I2C primitives
 * ================================================================ */

/**
 * @brief Configure SDA as output
 */
static inline void sda_output(void)
{
    GPIO_InitTypeDef gpio = {0};
    gpio.Pin   = OLED_SDA_PIN;
    gpio.Mode  = GPIO_MODE_OUTPUT_OD;
    gpio.Pull  = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(OLED_SDA_PORT, &gpio);
}

/**
 * @brief Configure SDA as input (for ACK read)
 */
static inline void sda_input(void)
{
    GPIO_InitTypeDef gpio = {0};
    gpio.Pin   = OLED_SDA_PIN;
    gpio.Mode  = GPIO_MODE_INPUT;
    gpio.Pull  = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(OLED_SDA_PORT, &gpio);
}

static void i2c_start(void)
{
    sda_output();
    SDA_HIGH();
    SCL_HIGH();
    I2C_DELAY();
    SDA_LOW();
    I2C_DELAY();
    SCL_LOW();
    I2C_DELAY();
}

static void i2c_stop(void)
{
    sda_output();
    SDA_LOW();
    I2C_DELAY();
    SCL_HIGH();
    I2C_DELAY();
    SDA_HIGH();
    I2C_DELAY();
}

/**
 * @brief Send one byte over I2C, return ACK status
 * @return 0 = ACK, 1 = NACK
 */
static uint8_t i2c_write_byte(uint8_t data)
{
    uint8_t i;
    sda_output();

    for (i = 0; i < 8; i++)
    {
        if (data & 0x80)
            SDA_HIGH();
        else
            SDA_LOW();
        data <<= 1;
        I2C_DELAY();
        SCL_HIGH();
        I2C_DELAY();
        SCL_LOW();
    }

    /* Read ACK */
    sda_input();
    I2C_DELAY();
    SCL_HIGH();
    I2C_DELAY();
    uint8_t ack = SDA_READ();
    SCL_LOW();
    sda_output();

    return ack;
}

/**
 * @brief Send I2C start + address byte
 */
static void i2c_send_addr(void)
{
    i2c_start();
    i2c_write_byte((OLED_I2C_ADDR << 1) | 0x00);  /* Write mode */
}

/**
 * @brief Send a command byte to ST7315
 */
static void oled_send_cmd(uint8_t cmd)
{
    i2c_send_addr();
    i2c_write_byte(0x00);   /* Co=0, D/C#=0 (command) */
    i2c_write_byte(cmd);
    i2c_stop();
}

/**
 * @brief Send data buffer to ST7315 (continuous I2C burst)
 */
static void oled_send_data(const uint8_t *data, uint16_t len)
{
    i2c_send_addr();
    i2c_write_byte(0x40);   /* Co=0, D/C#=1 (data) */
    for (uint16_t i = 0; i < len; i++)
    {
        i2c_write_byte(data[i]);
    }
    i2c_stop();
}

/* ================================================================
 *  Framebuffer operations
 * ================================================================ */

/**
 * @brief Clear the entire framebuffer
 */
static void fb_clear(void)
{
    memset(framebuffer, 0x00, sizeof(framebuffer));
}

/**
 * @brief Set all framebuffer bits to 1 (white)
 */
static void fb_fill_white(void)
{
    memset(framebuffer, 0xFF, sizeof(framebuffer));
}

/**
 * @brief Draw a filled circle in the framebuffer
 * @param cx  Center X
 * @param cy  Center Y (pixel coordinate, 0-47)
 * @param r   Radius in pixels
 * @param color 0=black (clear bits), 1=white (set bits)
 */
static void fb_draw_filled_circle(int cx, int cy, int r, uint8_t color)
{
    for (int y = -r; y <= r; y++)
    {
        for (int x = -r; x <= r; x++)
        {
            if (x * x + y * y <= r * r)
            {
                int px = cx + x;
                int py = cy + y;
                if (px >= 0 && px < OLED_WIDTH && py >= 0 && py < OLED_HEIGHT)
                {
                    uint16_t byte_idx = (py / 8) * OLED_WIDTH + px;
                    uint8_t bit_mask = 1 << (py % 8);
                    if (color)
                        framebuffer[byte_idx] |= bit_mask;
                    else
                        framebuffer[byte_idx] &= ~bit_mask;
                }
            }
        }
    }
}

/**
 * @brief Flush the entire framebuffer to ST7315
 *
 * ST7315 requires page-by-page addressing with column offset 32.
 * Cannot send all 384 bytes in one burst.
 */
static void fb_flush(void)
{
    for (uint8_t page = 0; page < OLED_PAGES; page++)
    {
        /* Set page address */
        oled_send_cmd(0xB0 + page);

        /* Set column address (offset 32 for 64x48 mapping) */
        oled_send_cmd(0x10 | ((32) >> 4));    /* Higher nibble */
        oled_send_cmd(0x00 | ((32) & 0x0F));  /* Lower nibble */

        /* Send one page of data (64 bytes) */
        oled_send_data(&framebuffer[page * OLED_WIDTH], OLED_WIDTH);
    }
}

/* ================================================================
 *  GPIO init for I2C pins
 * ================================================================ */

/**
 * @brief Initialize PC10 (SDA) and PC11 (SCL) as open-drain outputs
 */
static void oled_gpio_init(void)
{
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef gpio = {0};
    gpio.Mode  = GPIO_MODE_OUTPUT_OD;
    gpio.Pull  = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;

    gpio.Pin = OLED_SDA_PIN;
    HAL_GPIO_Init(OLED_SDA_PORT, &gpio);

    gpio.Pin = OLED_SCL_PIN;
    HAL_GPIO_Init(OLED_SCL_PORT, &gpio);

    /* Idle state: both high */
    SDA_HIGH();
    SCL_HIGH();
}

/* ================================================================
 *  Public API
 * ================================================================ */

/**
 * @brief Initialize OLED (ST7315) for eye display
 */
void oled_eye_init(void)
{
    oled_gpio_init();
    rt_thread_mdelay(100);  /* Wait for OLED power-up */

    /* ST7315 initialization sequence */
    oled_send_cmd(0xAE);    /* display off */
    oled_send_cmd(0x00);    /* set lower column address */
    oled_send_cmd(0x12);    /* set higher column address */
    oled_send_cmd(0x40);    /* set display start line */
    oled_send_cmd(0xB0);    /* set page address */
    oled_send_cmd(0x81);    /* contrast control */
    oled_send_cmd(0xFF);    /* max contrast */
    oled_send_cmd(0xA1);    /* set segment remap */
    oled_send_cmd(0xA6);    /* normal (non-inverted) */
    oled_send_cmd(0xA8);    /* multiplex ratio */
    oled_send_cmd(0x2F);    /* duty = 1/48 */
    oled_send_cmd(0xC8);    /* COM scan direction */
    oled_send_cmd(0xD3);    /* set display offset */
    oled_send_cmd(0x00);
    oled_send_cmd(0xD5);    /* set osc division */
    oled_send_cmd(0x80);
    oled_send_cmd(0xD9);    /* set pre-charge period */
    oled_send_cmd(0x21);    /* ST7315 uses 0x21 (SSD1306 uses 0xF1) */
    oled_send_cmd(0xDA);    /* set COM pins */
    oled_send_cmd(0x12);
    oled_send_cmd(0xDB);    /* set VCOMH */
    oled_send_cmd(0x40);
    oled_send_cmd(0x8D);    /* set charge pump enable */
    oled_send_cmd(0x14);
    oled_send_cmd(0xAF);    /* display ON */

    /* Clear screen */
    fb_clear();
    fb_flush();

    LOG_I("OLED eye initialized (ST7315, 64x48, I2C bit-bang PC10/PC11)");
}

/**
 * @brief Dying state: entire screen black (eyes off)
 */
void oled_eye_dying(void)
{
    fb_clear();
    fb_flush();
    LOG_I("OLED eye: dying state (black screen)");
}

/**
 * @brief Normal state: white background with black pupil circle in center
 */
void oled_eye_normal(void)
{
    /* Fill entire screen white */
    fb_fill_white();

    /* Draw black filled circle in center (pupil) */
    int cx = 32;            /* Center X for 64-pixel width */
    int cy = 24;            /* Center Y for 48-pixel height */
    int radius = 10;        /* Pupil radius, fits 64x48 */
    fb_draw_filled_circle(cx, cy, radius, 0);  /* color=0 means black */

    fb_flush();
    LOG_I("OLED eye: normal state (white + pupil)");
}

/* ================================================================
 *  MSH test command
 * ================================================================ */

/**
 * @brief OLED display test routine
 *
 * Test sequence:
 *   1. Init hardware
 *   2. Fill screen white
 *   3. Fill screen black
 *   4. Draw filled circle (center)
 *   5. Draw circles at 4 corners
 *   6. Show normal eye state
 *   7. Show dying eye state (clear)
 *
 * Usage: type "oled_test" in MSH console
 */
static int oled_test(int argc, char **argv)
{
    RT_UNUSED(argc);
    RT_UNUSED(argv);

    rt_kprintf("[OLED] === Test Start ===\n");

    /* Step 1: Init */
    rt_kprintf("[OLED] Initializing...\n");
    oled_eye_init();
    rt_thread_mdelay(500);

    /* Step 2: Full white */
    rt_kprintf("[OLED] Fill white\n");
    fb_fill_white();
    fb_flush();
    rt_thread_mdelay(1000);

    /* Step 3: Full black */
    rt_kprintf("[OLED] Fill black\n");
    fb_clear();
    fb_flush();
    rt_thread_mdelay(1000);

    /* Step 4: Center filled circle */
    rt_kprintf("[OLED] Draw center circle\n");
    fb_clear();
    fb_draw_filled_circle(32, 24, 12, 1);
    fb_flush();
    rt_thread_mdelay(1500);

    /* Step 5: 4-corner circles */
    rt_kprintf("[OLED] Draw corner circles\n");
    fb_clear();
    fb_draw_filled_circle(8,  8,  5, 1);   /* top-left     */
    fb_draw_filled_circle(55, 8,  5, 1);   /* top-right    */
    fb_draw_filled_circle(8,  39, 5, 1);   /* bottom-left  */
    fb_draw_filled_circle(55, 39, 5, 1);   /* bottom-right */
    fb_flush();
    rt_thread_mdelay(1500);

    /* Step 6: Normal eye state */
    rt_kprintf("[OLED] Normal eye state\n");
    oled_eye_normal();
    rt_thread_mdelay(2000);

    /* Step 7: Dying eye state */
    rt_kprintf("[OLED] Dying eye state\n");
    oled_eye_dying();
    rt_thread_mdelay(1000);

    rt_kprintf("[OLED] === Test Done ===\n");
    return 0;
}

MSH_CMD_EXPORT(oled_test, OLED ST7315 display test);
