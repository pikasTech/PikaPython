#ifndef _PIKA_HAL_UTILS_H
#define _PIKA_HAL_UTILS_H
#include <stdint.h>
#include "pika_hal.h"

/* error handler */
void pika_hal_utils_error_handler(int err,
                                  const char* file,
                                  int line,
                                  const char* func);

/* common utils */
int pika_hal_utils_enable(pika_dev* dev);
int pika_hal_utils_disable(pika_dev* dev);

/* GPIO utils */
int pika_hal_utils_GPIO_config(pika_dev* dev, pika_hal_GPIO_config* cfg);
int pika_hal_utils_GPIO_set_mode(pika_dev* dev,
                                 PIKA_HAL_GPIO_DIR dir,
                                 PIKA_HAL_GPIO_PULL pull);
int pika_hal_utils_GPIO_set_dir(pika_dev* dev, PIKA_HAL_GPIO_DIR dir);
int pika_hal_utils_GPIO_set_pull(pika_dev* dev, PIKA_HAL_GPIO_PULL pull);
int pika_hal_utils_GPIO_write(pika_dev* dev, uint32_t val);
uint32_t pika_hal_utils_GPIO_read(pika_dev* dev);

/* UART utils */
int pika_hal_utils_UART_config(pika_dev* dev, pika_hal_UART_config* cfg);

/* IIC utils */
int pika_hal_utils_IIC_config(pika_dev* dev, pika_hal_IIC_config* cfg);
int pika_hal_utils_IIC_mem_read(pika_dev* dev,
                                uint32_t mem_addr,
                                uint32_t mem_addr_size,
                                uint8_t* data,
                                uint32_t size);
int pika_hal_utils_IIC_mem_write(pika_dev* dev,
                                 uint32_t mem_addr,
                                 uint32_t mem_addr_size,
                                 uint8_t* data,
                                 uint32_t size);

int pika_hal_utils_IIC_set_slave_addr(pika_dev* dev, uint32_t slave_addr);
int pika_hal_utils_IIC_write(pika_dev* dev, uint8_t* data, uint32_t size);
int pika_hal_utils_IIC_read(pika_dev* dev, uint8_t* data, uint32_t size);

/* SPI utils */
int pika_hal_utils_SPI_config(pika_dev* dev, pika_hal_SPI_config* cfg);
int pika_hal_utils_SPI_transfer(pika_dev* dev,
                                uint8_t* txbuff,
                                uint8_t* rxbuff,
                                uint32_t size);

/* ADC utils */
int pika_hal_utils_ADC_config(pika_dev* dev, pika_hal_ADC_config* cfg);
uint32_t pika_hal_utils_ADC_read(pika_dev* dev);

/* DAC utils */
int pika_hal_utils_DAC_config(pika_dev* dev, pika_hal_DAC_config* cfg);
int pika_hal_utils_DAC_write(pika_dev* dev, uint32_t val);

/* PWM utils */
int pika_hal_utils_PWM_config(pika_dev* dev, pika_hal_PWM_config* cfg);
int pika_hal_utils_PWM_set_period(pika_dev* dev, PIKA_HAL_PWM_PERIOD period);
int pika_hal_utils_PWM_set_duty(pika_dev* dev, PIKA_HAL_PWM_DUTY duty);

/* SOFT_SPI utils */
int pika_hal_utils_SOFT_SPI_config(pika_dev* dev,
                                   pika_hal_SOFT_SPI_config* cfg);

/* SOFT_IIC utils */
int pika_hal_utils_SOFT_IIC_config(pika_dev* dev,
                                   pika_hal_SOFT_IIC_config* cfg);

/* WIFI utils */
int pika_hal_utils_WIFI_config(pika_dev* dev, pika_hal_WIFI_config* cfg);

/* SG utils */
int pika_hal_utils_SG_config(pika_dev* dev, pika_hal_SG_config* cfg);
pika_float pika_hal_utils_SG_read(pika_dev* dev);

#endif
