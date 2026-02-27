/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-12-05     18452       the first version
 */
#include "adc128s102cimtx.h"
#include <drv_spi.h>


#define ADC128S102_SPI_NAME     "adc128s_spi1"
#define ADC128S102_SPI_BUS      "spi1"

struct rt_spi_device *adc128s_spi_dev;
static struct rt_mutex  adc128s_spi_lock;

static int adc128s102_spi_init(void)
{

    rt_hw_spi_device_attach(ADC128S102_SPI_BUS, ADC128S102_SPI_NAME, ADC128S_CS_PORT, ADC128S_CS_PIN);
    adc128s_spi_dev = (struct rt_spi_device *)rt_device_find(ADC128S102_SPI_NAME);
    if(adc128s_spi_dev == RT_NULL){
        LOG_E("LOG:%d. adc128s102 spi device is not found!",Record.ulog_cnt++);
    }
    else{
        LOG_I("LOG:%d. adc128s102 spi device is successfully!",Record.ulog_cnt++);
    }

    struct rt_spi_configuration adc128s_spi_cfg;

    adc128s_spi_cfg.data_width = 16;
    adc128s_spi_cfg.max_hz = 21*1000*1000;
    adc128s_spi_cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_1 | RT_SPI_MSB;
    rt_spi_configure(adc128s_spi_dev, &adc128s_spi_cfg);

    rt_mutex_init(&adc128s_spi_lock, "adc128s_mutex", RT_IPC_FLAG_PRIO);
    return RT_EOK;
}
INIT_DEVICE_EXPORT(adc128s102_spi_init);




/* ---------------------------------------------------------
 * 核心读取函数
 * --------------------------------------------------------- */
#define ADC128S102_MAX_CH   8
#define ADC128S102_VREF_MV  5000
rt_err_t adc128s102_read_raw(adc128s_channel_et ch, rt_uint16_t *value)
{
    RT_ASSERT(ch < ADC128S102_MAX_CH);

    rt_uint16_t tx = (ch & 0x07) << 11;   /* 命令帧 0x0800..0xE800 */
    rt_uint16_t rx = 0;

    struct rt_spi_message msg = {
        .send_buf   = &tx,
        .recv_buf   = &rx,
        .length     = 1,            /* 单位=字，1×16 bit */
        .cs_take    = 1,
        .cs_release = 1,
        .next       = RT_NULL
    };

    rt_mutex_take(&adc128s_spi_lock, RT_WAITING_FOREVER);
    rt_spi_transfer_message(adc128s_spi_dev, &msg);
    rt_mutex_release(&adc128s_spi_lock);

    *value = rx & 0x0FFF;                   /* 12 位有效 */
    return RT_EOK;
}




/* 把 12-bit 原始码值转成电压，单位 mV，Vref = 5 000 mV */
static rt_uint16_t adc128s102_raw_to_mv(rt_uint16_t raw)
{
    /* 避免溢出：先乘 5000 再除 4095 */
    return (rt_uint32_t)raw * 5000 / 4095;
}

/* 如果想直接得到浮点伏特，再包一层 */
static float adc128s102_raw_to_volt(rt_uint16_t raw)
{
    return raw * (5.0f / 4095.0f);
}



rt_err_t adc128s102_read_and_print(adc128s_channel_et ch)
{
    rt_uint16_t raw;
    rt_err_t ret = adc128s102_read_raw(ch, &raw);

    if (ret != RT_EOK) {
        rt_kprintf("CH%u read error!\n", ch);
        return ret;
    }

    rt_uint16_t mv = adc128s102_raw_to_mv(raw);
    float       v  = adc128s102_raw_to_volt(raw);

    rt_kprintf("CH%u raw = %u  ->  %u mV  (%.3f V)\n", ch, raw, mv, v);
    return RT_EOK;
}



void adc128s102_thread_entry(void *parameter)
{

    for(;;)
    {
        adc128s102_read_and_print(ADC128S_Channel_5);
        rt_thread_mdelay(500);
    }
}



static rt_thread_t adc128s_Task_Handle = RT_NULL;
int adc128s102_thread_init(void)
{
    adc128s_Task_Handle = rt_thread_create("adc128s102_thread_entry",adc128s102_thread_entry, RT_NULL,1024,9, 100);
    if(adc128s_Task_Handle != RT_NULL){
        LOG_I("LOG:%d. adc128s102_thread_entry is succeed!",Record.ulog_cnt++);
        rt_thread_startup(adc128s_Task_Handle);
    }
    else{
        LOG_E("LOG:%d. adc128s102_thread_entry is failed!",Record.ulog_cnt++);
    }

    return RT_EOK;
}
INIT_ENV_EXPORT(adc128s102_thread_init);




















