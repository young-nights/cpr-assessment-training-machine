/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-05-12     coder        the first version - OLED eye driver (SSD1306, bit-bang I2C)
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

/* --- Framebuffer: 128x64 = 1024 bytes --- */
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
 * @brief Send a command byte to SSD1306
 */
static void oled_send_cmd(uint8_t cmd)
{
    i2c_send_addr();
    i2c_write_byte(0x00);   /* Co=0, D/C#=0 (command) */
    i2c_write_byte(cmd);
    i2c_stop();
}

/**
 * @brief Send data buffer to SSD1306 (continuous I2C burst)
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
 * @param cy  Center Y (pixel coordinate, 0-63)
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
 * @brief Flush the entire framebuffer to OLED
 */
static void fb_flush(void)
{
    /* Set column address 0-127 */
    oled_send_cmd(SSD1306_CMD_COL_ADDR);
    oled_send_cmd(0);
    oled_send_cmd(127);

    /* Set page address 0-7 */
    oled_send_cmd(SSD1306_CMD_PAGE_ADDR);
    oled_send_cmd(0);
    oled_send_cmd(7);

    /* Send framebuffer in one burst */
    oled_send_data(framebuffer, sizeof(framebuffer));
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
 * @brief Initialize OLED (SSD1306) for eye display
 */
void oled_eye_init(void)
{
    oled_gpio_init();
    rt_thread_mdelay(100);  /* Wait for OLED power-up */

    /* SSD1306 initialization sequence */
    oled_send_cmd(SSD1306_CMD_DISPLAY_OFF);         /* 0xAE */
    oled_send_cmd(SSD1306_CMD_SET_CLK_DIV);         /* 0xD5 */
    oled_send_cmd(0x80);                            /* Default clock */
    oled_send_cmd(SSD1306_CMD_SET_MUX);             /* 0xA8 */
    oled_send_cmd(0x3F);                            /* 64 mux */
    oled_send_cmd(SSD1306_CMD_SET_OFFSET);          /* 0xD3 */
    oled_send_cmd(0x00);                            /* No offset */
    oled_send_cmd(SSD1306_CMD_SET_START_LINE);      /* 0x40 | 0 */
    oled_send_cmd(SSD1306_CMD_CHARGE_PUMP);         /* 0x8D */
    oled_send_cmd(0x14);                            /* Enable charge pump */
    oled_send_cmd(SSD1306_CMD_SEG_REMAP | 0x01);    /* 0xA1: column 127 = SEG0 */
    oled_send_cmd(SSD1306_CMD_COM_SCAN_DIR | 0x08); /* 0xC8: remapped COM */
    oled_send_cmd(SSD1306_CMD_COM_PINS);            /* 0xDA */
    oled_send_cmd(0x12);                            /* Alternative COM config */
    oled_send_cmd(SSD1306_CMD_SET_CONTRAST);        /* 0x81 */
    oled_send_cmd(0xCF);
    oled_send_cmd(SSD1306_CMD_SET_CHARGE);          /* 0xD9 */
    oled_send_cmd(0xF1);
    oled_send_cmd(SSD1306_CMD_SET_VCOM);            /* 0xDB */
    oled_send_cmd(0x40);
    oled_send_cmd(SSD1306_CMD_ENTIRE_ON);           /* 0xA4: output follows RAM */
    oled_send_cmd(SSD1306_CMD_NORMAL);              /* 0xA6: non-inverted */
    oled_send_cmd(SSD1306_CMD_DISPLAY_ON);          /* 0xAF */

    /* Clear screen */
    fb_clear();
    fb_flush();

    LOG_I("OLED eye initialized (SSD1306, I2C bit-bang PC10/PC11)");
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
    int cx = OLED_WIDTH / 2;    /* 64 */
    int cy = OLED_HEIGHT / 2;   /* 32 */
    int radius = 16;            /* Pupil radius in pixels */
    fb_draw_filled_circle(cx, cy, radius, 0);  /* color=0 means black */

    fb_flush();
    LOG_I("OLED eye: normal state (white + pupil)");
}
