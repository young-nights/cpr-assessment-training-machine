/**
 * @file    head_oled.c
 * @brief   ST7315 OLED driver (64x48) via software I2C (bit-bang)
 *
 * Hardware: PC10=SCL, PC11=SDA (GPIO output)
 * Display:  ST7315, 64x48 pixels, 8 pages of 64 bytes
 * States:   Dying = black screen, Normal = white with black circle
 *
 * Change Logs:
 * Date           Author      Notes
 * 2026-09-27     coder       the first version
 */
#include "head_sys.h"

/* --- Pin access macros --- */
#define SCL_HIGH()  (OLED_SCL_PORT->BSRR = (1UL << OLED_SCL_PIN))
#define SCL_LOW()   (OLED_SCL_PORT->BRR  = (1UL << OLED_SCL_PIN))
#define SDA_HIGH()  (OLED_SDA_PORT->BSRR = (1UL << OLED_SDA_PIN))
#define SDA_LOW()   (OLED_SDA_PORT->BRR  = (1UL << OLED_SDA_PIN))
#define SDA_READ()  ((OLED_SDA_PORT->IDR >> OLED_SDA_PIN) & 1)

/* --- I2C timing (soft, ~200kHz) --- */
#define I2C_DELAY() Delay_us(2)

/* --- Framebuffer --- */
static uint8_t s_fb[OLED_BUF_SIZE];

/* Forward declaration */
static void set_pixel(int x, int y, uint8_t color);

/* --- ST7315 commands --- */
#define CMD_SWRESET     0x01
#define CMD_SLPOUT      0x11
#define CMD_DISPON      0x29
#define CMD_DISPON_INV  0x29
#define CMD_CASET       0x2A
#define CMD_RASET       0x2B
#define CMD_RAMWR       0x2C
#define CMD_MADCTL      0x36
#define CMD_COLMOD      0x3A
#define CMD_INVON       0x21

/* ====== Soft I2C primitives ====== */

static void i2c_start(void)
{
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
    SDA_LOW();
    I2C_DELAY();
    SCL_HIGH();
    I2C_DELAY();
    SDA_HIGH();
    I2C_DELAY();
}

static uint8_t i2c_write_byte(uint8_t data)
{
    for (int i = 7; i >= 0; i--) {
        if (data & (1 << i))
            SDA_HIGH();
        else
            SDA_LOW();
        I2C_DELAY();
        SCL_HIGH();
        I2C_DELAY();
        SCL_LOW();
    }
    /* ACK bit (ignore) */
    SDA_HIGH();
    I2C_DELAY();
    SCL_HIGH();
    I2C_DELAY();
    SCL_LOW();
    I2C_DELAY();
    return 0;
}

static void oled_send_cmd(uint8_t cmd)
{
    i2c_start();
    i2c_write_byte(OLED_ADDR << 1);     /* Write address */
    i2c_write_byte(0x00);               /* Co=0, D/C#=0 (command) */
    i2c_write_byte(cmd);
    i2c_stop();
}

static void oled_send_data(const uint8_t *data, uint16_t len)
{
    i2c_start();
    i2c_write_byte(OLED_ADDR << 1);
    i2c_write_byte(0x40);               /* Co=0, D/C#=1 (data) */
    for (uint16_t i = 0; i < len; i++) {
        i2c_write_byte(data[i]);
    }
    i2c_stop();
}

/* ====== Framebuffer operations ====== */

void OLED_Clear(void)
{
    for (int i = 0; i < OLED_BUF_SIZE; i++)
        s_fb[i] = 0x00;
}

void OLED_FillWhite(void)
{
    for (int i = 0; i < OLED_BUF_SIZE; i++)
        s_fb[i] = 0xFF;
}

void OLED_DrawCircle(int cx, int cy, int r, uint8_t color)
{
    /* Bresenham circle for ST7315 (vertical addressing per page) */
    int x = r, y = 0, err = 1 - r;
    while (x >= y) {
        /* 8 octants */
        set_pixel(cx + x, cy + y, color);
        set_pixel(cx + y, cy + x, color);
        set_pixel(cx - y, cy + x, color);
        set_pixel(cx - x, cy + y, color);
        set_pixel(cx - x, cy - y, color);
        set_pixel(cx - y, cy - x, color);
        set_pixel(cx + y, cy - x, color);
        set_pixel(cx + x, cy - y, color);
        y++;
        if (err < 0) {
            err += 2 * y + 1;
        } else {
            x--;
            err += 2 * (y - x) + 1;
        }
    }
    /* Filled circle: fill horizontal spans */
    for (int dy = -r; dy <= r; dy++) {
        int dx = (int)(r * r - dy * dy);
        /* Simple integer sqrt approximation */
        int w = 0;
        while ((w + 1) * (w + 1) <= dx) w++;
        for (int i = -w; i <= w; i++) {
            set_pixel(cx + i, cy + dy, color);
        }
    }
}

static void set_pixel(int x, int y, uint8_t color)
{
    if (x < 0 || x >= OLED_WIDTH || y < 0 || y >= OLED_HEIGHT)
        return;
    /* ST7315: 64 wide, 8 pages of 6 rows each */
    int page = y / 6;
    int row_in_page = y % 6;
    int idx = page * OLED_WIDTH + x;
    if (idx >= OLED_BUF_SIZE)
        return;
    if (color)
        s_fb[idx] |= (1 << row_in_page);
    else
        s_fb[idx] &= ~(1 << row_in_page);
}

void OLED_Flush(void)
{
    /* Send framebuffer to OLED via I2C */
    for (int page = 0; page < OLED_PAGES; page++) {
        /* Set page address */
        oled_send_cmd(0xB0 + page);     /* Page address command */
        oled_send_cmd(0x00);            /* Column low nibble */
        oled_send_cmd(0x10);            /* Column high nibble */
        /* Send 64 bytes of page data */
        oled_send_data(&s_fb[page * OLED_WIDTH], OLED_WIDTH);
    }
}

/* ====== Initialization ====== */

void OLED_Init(void)
{
    /* Set SCL/SDA high (idle) */
    SCL_HIGH();
    SDA_HIGH();
    Delay_ms(10);

    /* ST7315 init sequence */
    oled_send_cmd(CMD_SWRESET);
    Delay_ms(10);

    oled_send_cmd(CMD_SLPOUT);          /* Sleep out */
    Delay_ms(10);

    oled_send_cmd(0xB0);                /* Page 0 */
    oled_send_cmd(0x00);                /* Column start low */
    oled_send_cmd(0x10);                /* Column start high */

    oled_send_cmd(0x21);                /* Display inversion ON (ST7315) */
    oled_send_cmd(0x3A);                /* Interface pixel format */
    /* Send parameter for 16-bit color (or 8-bit depending on module) */

    oled_send_cmd(0xBB);                /* VCOM voltage */
    oled_send_cmd(0x3C);                /* Default value */

    oled_send_cmd(CMD_DISPON);          /* Display ON */
    Delay_ms(10);

    /* Clear screen */
    OLED_Clear();
    OLED_Flush();
}

/* ====== Eye state control ====== */

void OLED_SetEyeState(eye_state_et state)
{
    switch (state) {
        case EYE_DYING:
            /* Black screen (pupils dilated, no reflex) */
            OLED_Clear();
            OLED_Flush();
            break;

        case EYE_NORMAL:
            /* White background with black circle (normal pupil) */
            OLED_FillWhite();
            OLED_DrawCircle(OLED_WIDTH / 2, OLED_HEIGHT / 2, 12, 0);  /* Black circle */
            OLED_Flush();
            break;

        default:
            OLED_Clear();
            OLED_Flush();
            break;
    }
}
