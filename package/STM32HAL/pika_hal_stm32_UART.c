#include "pika_hal_stm32_common.h"
#include <ctype.h>
/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 * Copyright (c) 2023-2023, PikaPython Development Team 
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-10-30     SummerGift   first version
 * 2020-03-16     SummerGift   add device close feature
 * 2020-03-20     SummerGift   fix bug caused by ORE
 * 2020-05-02     whj4674672   support stm32h7 uart dma
 * 2020-09-09     forest-rain  support stm32wl uart
 * 2020-10-14     Dozingfiretruck   Porting for stm32wbxx
 * 2023-08-22     lyon              port for PikaPython
 */

#ifndef PIKA_HAL
#include "board.h"
#include "drv_usart.h"
#endif

#ifdef PIKA_HAL
#include "pika_drv_config.h"
#else
#include "drv_config.h"
#endif

// #ifdef RT_USING_SERIAL

#ifndef PIKA_HAL
//#define DRV_DEBUG
#define LOG_TAG             "drv.usart"
#include <drv_log.h>
#endif

#if !defined(BSP_USING_UART1) && !defined(BSP_USING_UART2) && !defined(BSP_USING_UART3) && \
    !defined(BSP_USING_UART4) && !defined(BSP_USING_UART5) && !defined(BSP_USING_UART6) && \
    !defined(BSP_USING_UART7) && !defined(BSP_USING_UART8) && !defined(BSP_USING_LPUART1)
#error "Please define at least one BSP_USING_UARTx"
/* this driver can be disabled at menuconfig -> RT-Thread Components -> Device Drivers */
#endif

#ifdef RT_SERIAL_USING_DMA
static void stm32_dma_config(struct rt_serial_device *serial, rt_ubase_t flag);
#endif

static int rt_hw_usart_init(void);

#if defined(SOC_SERIES_STM32F0) || defined(SOC_SERIES_STM32F1) || defined(SOC_SERIES_STM32L4) || defined(SOC_SERIES_STM32WL) \
    || defined(SOC_SERIES_STM32L0) || defined(SOC_SERIES_STM32G0) || defined(SOC_SERIES_STM32G4) || defined(SOC_SERIES_STM32WB)|| defined(SOC_SERIES_STM32F3)
#define DMA_INSTANCE_TYPE              DMA_Channel_TypeDef
#elif defined(SOC_SERIES_STM32F2) || defined(SOC_SERIES_STM32F4) || defined(SOC_SERIES_STM32F7) \
    || defined(SOC_SERIES_STM32H7) || defined(SOC_SERIES_STM32MP1)
#define DMA_INSTANCE_TYPE              DMA_Stream_TypeDef
#endif /*  defined(SOC_SERIES_STM32F1) || defined(SOC_SERIES_STM32L4) || defined(SOC_SERIES_STM32WL) */

#if defined(SOC_SERIES_STM32F1) || defined(SOC_SERIES_STM32L4) || defined(SOC_SERIES_STM32L5) || defined(SOC_SERIES_STM32WL) \
    || defined(SOC_SERIES_STM32F2) || defined(SOC_SERIES_STM32F4) || defined(SOC_SERIES_STM32L0) || defined(SOC_SERIES_STM32G0) \
    || defined(SOC_SERIES_STM32G4) || defined(SOC_SERIES_STM32WB)|| defined(SOC_SERIES_STM32F3) || defined(SOC_SERIES_STM32U5)
#define UART_INSTANCE_CLEAR_FUNCTION    __HAL_UART_CLEAR_FLAG
#elif defined(SOC_SERIES_STM32F7) || defined(SOC_SERIES_STM32F0) || defined(SOC_SERIES_STM32H7) \
    || defined(SOC_SERIES_STM32MP1)
#define UART_INSTANCE_CLEAR_FUNCTION    __HAL_UART_CLEAR_IT
#endif

#define BAUD_RATE_2400                  2400
#define BAUD_RATE_4800                  4800
#define BAUD_RATE_9600                  9600
#define BAUD_RATE_19200                 19200
#define BAUD_RATE_38400                 38400
#define BAUD_RATE_57600                 57600
#define BAUD_RATE_115200                115200
#define BAUD_RATE_230400                230400
#define BAUD_RATE_460800                460800
#define BAUD_RATE_500000                500000
#define BAUD_RATE_921600                921600
#define BAUD_RATE_2000000               2000000
#define BAUD_RATE_2500000               2500000
#define BAUD_RATE_3000000               3000000

#define DATA_BITS_5                     5
#define DATA_BITS_6                     6
#define DATA_BITS_7                     7
#define DATA_BITS_8                     8
#define DATA_BITS_9                     9

#define STOP_BITS_1                     0
#define STOP_BITS_2                     1
#define STOP_BITS_3                     2
#define STOP_BITS_4                     3

#ifdef _WIN32
#include <windows.h>
#else
#define PARITY_NONE                     0
#define PARITY_ODD                      1
#define PARITY_EVEN                     2
#endif

#define BIT_ORDER_LSB                   0
#define BIT_ORDER_MSB                   1

#define NRZ_NORMAL                      0       /* Non Return to Zero : normal mode */
#define NRZ_INVERTED                    1       /* Non Return to Zero : inverted mode */

#ifndef RT_SERIAL_RB_BUFSZ
#define RT_SERIAL_RB_BUFSZ              64
#endif

#define RT_SERIAL_EVENT_RX_IND          0x01    /* Rx indication */
#define RT_SERIAL_EVENT_TX_DONE         0x02    /* Tx complete   */
#define RT_SERIAL_EVENT_RX_DMADONE      0x03    /* Rx DMA transfer done */
#define RT_SERIAL_EVENT_TX_DMADONE      0x04    /* Tx DMA transfer done */
#define RT_SERIAL_EVENT_RX_TIMEOUT      0x05    /* Rx timeout    */

#define RT_SERIAL_DMA_RX                0x01
#define RT_SERIAL_DMA_TX                0x02

#define RT_SERIAL_RX_INT                0x01
#define RT_SERIAL_TX_INT                0x02

#define RT_SERIAL_ERR_OVERRUN           0x01
#define RT_SERIAL_ERR_FRAMING           0x02
#define RT_SERIAL_ERR_PARITY            0x03

#define RT_SERIAL_TX_DATAQUEUE_SIZE     2048
#define RT_SERIAL_TX_DATAQUEUE_LWM      30

#define RT_SERIAL_FLOWCONTROL_CTSRTS     1
#define RT_SERIAL_FLOWCONTROL_NONE       0

struct serial_configure
{
    rt_uint32_t baud_rate;

    rt_uint32_t data_bits               :4;
    rt_uint32_t stop_bits               :2;
    rt_uint32_t parity                  :2;
    rt_uint32_t bit_order               :1;
    rt_uint32_t invert                  :1;
    rt_uint32_t bufsz                   :16;
    rt_uint32_t flowcontrol             :1;
    rt_uint32_t reserved                :5;
};

struct rt_device_notify
{
    void (*notify)(rt_device_t dev);
    struct rt_device *dev;
};

struct rt_serial_device
{
    struct rt_device          parent;

    const struct rt_uart_ops *ops;
    struct serial_configure   config;

    void *serial_rx;
    void *serial_tx;

    struct rt_device_notify rx_notify;
};
typedef struct rt_serial_device rt_serial_t;

#define UART_RX_DMA_IT_IDLE_FLAG        0x00
#define UART_RX_DMA_IT_HT_FLAG          0x01
#define UART_RX_DMA_IT_TC_FLAG          0x02

/* stm32 config class */
struct stm32_uart_config
{
    const char *name;
    USART_TypeDef *Instance;
    IRQn_Type irq_type;
    struct dma_config *dma_rx;
    struct dma_config *dma_tx;
    uint32_t GPIO_Alternate;
    uint32_t GPIO_TX_Pin;
    GPIO_TypeDef* GPIO_TX_Port;
    uint32_t GPIO_RX_Pin;
    GPIO_TypeDef* GPIO_RX_Port;
};

/* stm32 uart dirver class */
struct stm32_uart
{
    UART_HandleTypeDef handle;
    struct stm32_uart_config *config;
    rt_uint32_t DR_mask;

#ifdef RT_SERIAL_USING_DMA
    struct
    {
        DMA_HandleTypeDef handle;
        rt_size_t remaining_cnt;
    } dma_rx;
    struct
    {
        DMA_HandleTypeDef handle;
    } dma_tx;
#endif
    rt_uint16_t uart_dma_flag;
    struct rt_serial_device serial;
};

void rt_hw_serial_isr(struct rt_serial_device *serial, int event);

enum
{
#ifdef BSP_USING_UART1
    UART1_INDEX,
#endif
#ifdef BSP_USING_UART2
    UART2_INDEX,
#endif
#ifdef BSP_USING_UART3
    UART3_INDEX,
#endif
#ifdef BSP_USING_UART4
    UART4_INDEX,
#endif
#ifdef BSP_USING_UART5
    UART5_INDEX,
#endif
#ifdef BSP_USING_UART6
    UART6_INDEX,
#endif
#ifdef BSP_USING_UART7
    UART7_INDEX,
#endif
#ifdef BSP_USING_UART8
    UART8_INDEX,
#endif
#ifdef BSP_USING_LPUART1
    LPUART1_INDEX,
#endif
};

static struct stm32_uart_config uart_config[] =
{
#ifdef BSP_USING_UART1
    UART1_CONFIG,
#endif
#ifdef BSP_USING_UART2
    UART2_CONFIG,
#endif
#ifdef BSP_USING_UART3
    UART3_CONFIG,
#endif
#ifdef BSP_USING_UART4
    UART4_CONFIG,
#endif
#ifdef BSP_USING_UART5
    UART5_CONFIG,
#endif
#ifdef BSP_USING_UART6
    UART6_CONFIG,
#endif
#ifdef BSP_USING_UART7
    UART7_CONFIG,
#endif
#ifdef BSP_USING_UART8
    UART8_CONFIG,
#endif
#ifdef BSP_USING_LPUART1
    LPUART1_CONFIG,
#endif
};

static struct stm32_uart uart_obj[sizeof(uart_config) / sizeof(uart_config[0])] = {0};

static rt_uint32_t stm32_uart_get_mask(rt_uint32_t word_length, rt_uint32_t parity)
{
    rt_uint32_t mask = 0x00FFU;
    if (word_length == UART_WORDLENGTH_8B)
    {
        if (parity == UART_PARITY_NONE)
        {
            mask = 0x00FFU ;
        }
        else
        {
            mask = 0x007FU ;
        }
    }
#ifdef UART_WORDLENGTH_9B
    else if (word_length == UART_WORDLENGTH_9B)
    {
        if (parity == UART_PARITY_NONE)
        {
            mask = 0x01FFU ;
        }
        else
        {
            mask = 0x00FFU ;
        }
    }
#endif
#ifdef UART_WORDLENGTH_7B
    else if (word_length == UART_WORDLENGTH_7B)
    {
        if (parity == UART_PARITY_NONE)
        {
            mask = 0x007FU ;
        }
        else
        {
            mask = 0x003FU ;
        }
    }
    else
    {
        mask = 0x0000U;
    }
#endif
    return mask;
}

void mp_hal_gpio_clock_enable(GPIO_TypeDef *gpio) {
    #if defined(STM32L476xx) || defined(STM32L496xx)
    if (gpio == GPIOG) {
        // Port G pins 2 thru 15 are powered using VddIO2 on these MCUs.
        HAL_PWREx_EnableVddIO2();
    }
    #endif

    // This logic assumes that all the GPIOx_EN bits are adjacent and ordered in one register

    #if defined(STM32F0) || defined(STM32L1)
    #define AHBxENR AHBENR
    #define AHBxENR_GPIOAEN_Pos RCC_AHBENR_GPIOAEN_Pos
    #elif defined(STM32F4) || defined(STM32F7)
    #define AHBxENR AHB1ENR
    #define AHBxENR_GPIOAEN_Pos RCC_AHB1ENR_GPIOAEN_Pos
    #elif defined(STM32H7)
    #define AHBxENR AHB4ENR
    #define AHBxENR_GPIOAEN_Pos RCC_AHB4ENR_GPIOAEN_Pos
    #elif defined(STM32L0)
    #define AHBxENR IOPENR
    #define AHBxENR_GPIOAEN_Pos RCC_IOPENR_IOPAEN_Pos
    #elif defined(STM32G0)
    #define AHBxENR IOPENR
    #define AHBxENR_GPIOAEN_Pos RCC_IOPENR_GPIOAEN_Pos
    #elif defined(STM32G4) || defined(STM32H5) || defined(STM32L4) || defined(STM32WB) || defined(STM32WL)
    #define AHBxENR AHB2ENR
    #define AHBxENR_GPIOAEN_Pos RCC_AHB2ENR_GPIOAEN_Pos
    #endif

    uint32_t gpio_idx = ((uint32_t)gpio - GPIOA_BASE) / (GPIOB_BASE - GPIOA_BASE);
    RCC->AHBxENR |= 1 << (AHBxENR_GPIOAEN_Pos + gpio_idx);
    volatile uint32_t tmp = RCC->AHBxENR; // Delay after enabling clock
    (void)tmp;
}


static rt_err_t stm32_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
    struct stm32_uart *uart;
    int uart_unit;
    RT_ASSERT(serial != RT_NULL);
    RT_ASSERT(cfg != RT_NULL);

    uart = rt_container_of(serial, struct stm32_uart, serial);
    uart->handle.Instance          = uart->config->Instance;
    uart->handle.Init.BaudRate     = cfg->baud_rate;
    uart->handle.Init.Mode         = UART_MODE_TX_RX;
#ifdef USART_CR1_OVER8
    uart->handle.Init.OverSampling = cfg->baud_rate > 5000000 ? UART_OVERSAMPLING_8 : UART_OVERSAMPLING_16;
#else
    uart->handle.Init.OverSampling = UART_OVERSAMPLING_16;
#endif /* USART_CR1_OVER8 */

    switch (cfg->flowcontrol)
    {
    case RT_SERIAL_FLOWCONTROL_NONE:
        uart->handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
        break;
    case RT_SERIAL_FLOWCONTROL_CTSRTS:
        uart->handle.Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
        break;
    default:
        uart->handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
        break;
    }

    switch (cfg->data_bits)
    {
    case DATA_BITS_8:
        if (cfg->parity == PARITY_ODD || cfg->parity == PARITY_EVEN)
            uart->handle.Init.WordLength = UART_WORDLENGTH_9B;
        else
            uart->handle.Init.WordLength = UART_WORDLENGTH_8B;
        break;
    case DATA_BITS_9:
        uart->handle.Init.WordLength = UART_WORDLENGTH_9B;
        break;
    default:
        uart->handle.Init.WordLength = UART_WORDLENGTH_8B;
        break;
    }

    switch (cfg->stop_bits)
    {
    case STOP_BITS_1:
        uart->handle.Init.StopBits   = UART_STOPBITS_1;
        break;
    case STOP_BITS_2:
        uart->handle.Init.StopBits   = UART_STOPBITS_2;
        break;
    default:
        uart->handle.Init.StopBits   = UART_STOPBITS_1;
        break;
    }

    switch (cfg->parity)
    {
    case PARITY_NONE:
        uart->handle.Init.Parity     = UART_PARITY_NONE;
        break;
    case PARITY_ODD:
        uart->handle.Init.Parity     = UART_PARITY_ODD;
        break;
    case PARITY_EVEN:
        uart->handle.Init.Parity     = UART_PARITY_EVEN;
        break;
    default:
        uart->handle.Init.Parity     = UART_PARITY_NONE;
        break;
    }

#ifdef RT_SERIAL_USING_DMA
    if (!(serial->parent.open_flag & RT_DEVICE_OFLAG_OPEN)) {
        uart->dma_rx.remaining_cnt = cfg->bufsz;
    }
#endif

#ifdef USART1
    if (uart->handle.Instance == USART1){
        __HAL_RCC_USART1_CLK_ENABLE();
        uart_unit = 1;
        if (0 == uart->config->GPIO_TX_Pin && 0 == uart->config->GPIO_RX_Pin){
            uart->config->GPIO_TX_Pin = GPIO_PIN_9;
            uart->config->GPIO_TX_Port = GPIOA;
            uart->config->GPIO_RX_Pin = GPIO_PIN_10;
            uart->config->GPIO_RX_Port = GPIOA;
            pika_platform_printf("WARNING: UART1 GPIO TX/RX Pin not set, use default PA9/PA10\r\n");
        }
    }
#endif

#ifdef USART2
    if (uart->handle.Instance == USART2){
        __HAL_RCC_USART2_CLK_ENABLE();
        uart_unit = 2;
        if (0 == uart->config->GPIO_TX_Pin && 0 == uart->config->GPIO_RX_Pin){
            uart->config->GPIO_TX_Pin = GPIO_PIN_2;
            uart->config->GPIO_TX_Port = GPIOA;
            uart->config->GPIO_RX_Pin = GPIO_PIN_3;
            uart->config->GPIO_RX_Port = GPIOA;
            pika_platform_printf("WARNING: UART2 GPIO TX/RX Pin not set, use default PA2/PA3\r\n");
        }
    }
#endif

#ifdef USART3
    if (uart->handle.Instance == USART3){
        __HAL_RCC_USART3_CLK_ENABLE();
        uart_unit = 3;
        if (0 == uart->config->GPIO_TX_Pin && 0 == uart->config->GPIO_RX_Pin){
            uart->config->GPIO_TX_Pin = GPIO_PIN_10;
            uart->config->GPIO_TX_Port = GPIOB;
            uart->config->GPIO_RX_Pin = GPIO_PIN_11;
            uart->config->GPIO_RX_Port = GPIOB;
            pika_platform_printf("WARNING: UART3 GPIO TX/RX Pin not set, use default PB10/PB11\r\n");
        }
    }
#endif

#ifdef UART4
    if (uart->handle.Instance == UART4){
        __HAL_RCC_UART4_CLK_ENABLE();
        uart_unit = 4;
        if (0 == uart->config->GPIO_TX_Pin && 0 == uart->config->GPIO_RX_Pin){
            uart->config->GPIO_TX_Pin = GPIO_PIN_0;
            uart->config->GPIO_TX_Port = GPIOA;
            uart->config->GPIO_RX_Pin = GPIO_PIN_1;
            uart->config->GPIO_RX_Port = GPIOA;
            pika_platform_printf("WARNING: UART4 GPIO TX/RX Pin not set, use default PA0/PA1\r\n");
        }
    }
#endif

#ifdef USART4
    if (uart->handle.Instance == USART4){
        __HAL_RCC_USART4_CLK_ENABLE();    
        uart_unit = 4;
        if (0 == uart->config->GPIO_TX_Pin && 0 == uart->config->GPIO_RX_Pin){
            uart->config->GPIO_TX_Pin = GPIO_PIN_0;
            uart->config->GPIO_TX_Port = GPIOA;
            uart->config->GPIO_RX_Pin = GPIO_PIN_1;
            uart->config->GPIO_RX_Port = GPIOA;
            pika_platform_printf("WARNING: UART4 GPIO TX/RX Pin not set, use default PA0/PA1\r\n");
        }
    }
#endif

#ifdef UART5
    if (uart->handle.Instance == UART5){
        __HAL_RCC_UART5_CLK_ENABLE();
        uart_unit = 5;
        if (0 == uart->config->GPIO_TX_Pin && 0 == uart->config->GPIO_RX_Pin){
            uart->config->GPIO_TX_Pin = GPIO_PIN_12;
            uart->config->GPIO_TX_Port = GPIOC;
            uart->config->GPIO_RX_Pin = GPIO_PIN_2;
            uart->config->GPIO_RX_Port = GPIOD;
            pika_platform_printf("WARNING: UART5 GPIO TX/RX Pin not set, use default PC12/PD2\r\n");
        }
    }
#endif

#ifdef USART5
    if (uart->handle.Instance == USART5){
        __HAL_RCC_USART5_CLK_ENABLE();  
        uart_unit = 5;
        if (0 == uart->config->GPIO_TX_Pin && 0 == uart->config->GPIO_RX_Pin){
            uart->config->GPIO_TX_Pin = GPIO_PIN_12;
            uart->config->GPIO_TX_Port = GPIOC;
            uart->config->GPIO_RX_Pin = GPIO_PIN_2;
            uart->config->GPIO_RX_Port = GPIOD;
            pika_platform_printf("WARNING: UART5 GPIO TX/RX Pin not set, use default PC12/PD2\r\n");
        }
    }
#endif

#ifdef UART6
    if (uart->handle.Instance == UART6){
        __HAL_RCC_UART6_CLK_ENABLE();  
        uart_unit = 6;
        if (0 == uart->config->GPIO_TX_Pin && 0 == uart->config->GPIO_RX_Pin){
            uart->config->GPIO_TX_Pin = GPIO_PIN_6;
            uart->config->GPIO_TX_Port = GPIOC;
            uart->config->GPIO_RX_Pin = GPIO_PIN_7;
            uart->config->GPIO_RX_Port = GPIOC;
            pika_platform_printf("WARNING: UART6 GPIO TX/RX Pin not set, use default PC6/PC7\r\n");
        }
    }
#endif

#ifdef USART6
    if (uart->handle.Instance == USART6){
        __HAL_RCC_USART6_CLK_ENABLE();   
        uart_unit = 6;
        if (0 == uart->config->GPIO_TX_Pin && 0 == uart->config->GPIO_RX_Pin){
            uart->config->GPIO_TX_Pin = GPIO_PIN_6;
            uart->config->GPIO_TX_Port = GPIOC;
            uart->config->GPIO_RX_Pin = GPIO_PIN_7;
            uart->config->GPIO_RX_Port = GPIOC;
            pika_platform_printf("WARNING: UART6 GPIO TX/RX Pin not set, use default PC6/PC7\r\n");
        }
    }
#endif

#ifdef UART7
    if (uart->handle.Instance == UART7){
        __HAL_RCC_UART7_CLK_ENABLE();    
        uart_unit = 7;
        if (0 == uart->config->GPIO_TX_Pin && 0 == uart->config->GPIO_RX_Pin){
            uart->config->GPIO_TX_Pin = GPIO_PIN_1;
            uart->config->GPIO_TX_Port = GPIOE;
            uart->config->GPIO_RX_Pin = GPIO_PIN_0;
            uart->config->GPIO_RX_Port = GPIOE;
            pika_platform_printf("WARNING: UART7 GPIO TX/RX Pin not set, use default PE1/PE0\r\n");
        }
    }
#endif

#ifdef USART7
    if (uart->handle.Instance == USART7){
        __HAL_RCC_USART7_CLK_ENABLE();    
        uart_unit = 7;
        if(0 == uart->config->GPIO_TX_Pin && 0 == uart->config->GPIO_RX_Pin){
            uart->config->GPIO_TX_Pin = GPIO_PIN_1;
            uart->config->GPIO_TX_Port = GPIOE;
            uart->config->GPIO_RX_Pin = GPIO_PIN_0;
            uart->config->GPIO_RX_Port = GPIOE;
            pika_platform_printf("WARNING: UART7 GPIO TX/RX Pin not set, use default PE1/PE0\r\n");
        }
    }
#endif

#ifdef UART8
    if (uart->handle.Instance == UART8){
        __HAL_RCC_UART8_CLK_ENABLE();   
        uart_unit = 8;
        if(0 == uart->config->GPIO_TX_Pin && 0 == uart->config->GPIO_RX_Pin){
            uart->config->GPIO_TX_Pin = GPIO_PIN_1;
            uart->config->GPIO_TX_Port = GPIOE;
            uart->config->GPIO_RX_Pin = GPIO_PIN_0;
            uart->config->GPIO_RX_Port = GPIOE;
            pika_platform_printf("WARNING: UART8 GPIO TX/RX Pin not set, use default PE1/PE0\r\n");
        }
    }
#endif

#ifdef USART8
    if (uart->handle.Instance == USART8){
        __HAL_RCC_USART8_CLK_ENABLE();    
        uart_unit = 8;
        if(0 == uart->config->GPIO_TX_Pin && 0 == uart->config->GPIO_RX_Pin){
            uart->config->GPIO_TX_Pin = GPIO_PIN_1;
            uart->config->GPIO_TX_Port = GPIOE;
            uart->config->GPIO_RX_Pin = GPIO_PIN_0;
            uart->config->GPIO_RX_Port = GPIOE;
            pika_platform_printf("WARNING: UART8 GPIO TX/RX Pin not set, use default PE1/PE0\r\n");
        }
    }
#endif

#ifdef UART9
    if (uart->handle.Instance == UART9){
        __HAL_RCC_UART9_CLK_ENABLE(); 
        uart_unit = 9;
        if(0 == uart->config->GPIO_TX_Pin && 0 == uart->config->GPIO_RX_Pin){
            uart->config->GPIO_TX_Pin = GPIO_PIN_1;
            uart->config->GPIO_TX_Port = GPIOE;
            uart->config->GPIO_RX_Pin = GPIO_PIN_0;
            uart->config->GPIO_RX_Port = GPIOE;
            pika_platform_printf("WARNING: UART9 GPIO TX/RX Pin not set, use default PE1/PE0\r\n");
        }
    }
#endif

#ifdef USART9
    if (uart->handle.Instance == USART9){
        __HAL_RCC_USART9_CLK_ENABLE();    
        uart_unit = 9;
        if(0 == uart->config->GPIO_TX_Pin && 0 == uart->config->GPIO_RX_Pin){
            uart->config->GPIO_TX_Pin = GPIO_PIN_1;
            uart->config->GPIO_TX_Port = GPIOE;
            uart->config->GPIO_RX_Pin = GPIO_PIN_0;
            uart->config->GPIO_RX_Port = GPIOE;
            pika_platform_printf("WARNING: UART9 GPIO TX/RX Pin not set, use default PE1/PE0\r\n");
        }
    }
#endif

#ifdef UART10
    if (uart->handle.Instance == UART10){
        __HAL_RCC_UART10_CLK_ENABLE();  
        uart_unit = 10;
        if(0 == uart->config->GPIO_TX_Pin && 0 == uart->config->GPIO_RX_Pin){
            uart->config->GPIO_TX_Pin = GPIO_PIN_1;
            uart->config->GPIO_TX_Port = GPIOE;
            uart->config->GPIO_RX_Pin = GPIO_PIN_0;
            uart->config->GPIO_RX_Port = GPIOE;
            pika_platform_printf("WARNING: UART10 GPIO TX/RX Pin not set, use default PE1/PE0\r\n");
        }
    }
#endif

#ifdef USART10
    if (uart->handle.Instance == USART10){
        __HAL_RCC_USART10_CLK_ENABLE();    
        uart_unit = 10;
        if(0 == uart->config->GPIO_TX_Pin && 0 == uart->config->GPIO_RX_Pin){
            uart->config->GPIO_TX_Pin = GPIO_PIN_1;
            uart->config->GPIO_TX_Port = GPIOE;
            uart->config->GPIO_RX_Pin = GPIO_PIN_0;
            uart->config->GPIO_RX_Port = GPIOE;
            pika_platform_printf("WARNING: UART10 GPIO TX/RX Pin not set, use default PE1/PE0\r\n");
        }
    }
#endif

#ifdef LPUART1
    if (uart->handle.Instance == LPUART1){
        __HAL_RCC_LPUART1_CLK_ENABLE();
        uart_unit = 1;
        if(0 == uart->config->GPIO_TX_Pin && 0 == uart->config->GPIO_RX_Pin){
            uart->config->GPIO_TX_Pin = GPIO_PIN_2;
            uart->config->GPIO_TX_Port = GPIOA;
            uart->config->GPIO_RX_Pin = GPIO_PIN_3;
            uart->config->GPIO_RX_Port = GPIOA;
            pika_platform_printf("WARNING: LPUART1 GPIO TX/RX Pin not set, use default PA2/PA3\r\n");
        }
    }
#endif

#ifdef LPUART2
    if (uart->handle.Instance == LPUART2){
        __HAL_RCC_LPUART2_CLK_ENABLE();    
        uart_unit = 2;
        if(0 == uart->config->GPIO_TX_Pin && 0 == uart->config->GPIO_RX_Pin){
            uart->config->GPIO_TX_Pin = GPIO_PIN_2;
            uart->config->GPIO_TX_Port = GPIOA;
            uart->config->GPIO_RX_Pin = GPIO_PIN_3;
            uart->config->GPIO_RX_Port = GPIOA;
            pika_platform_printf("WARNING: LPUART2 GPIO TX/RX Pin not set, use default PA2/PA3\r\n");
        }
    }
#endif
    
    mp_hal_gpio_clock_enable(uart->config->GPIO_TX_Port);
    mp_hal_gpio_clock_enable(uart->config->GPIO_RX_Port);
    /* GPIO INIT */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
#if defined(SOC_SERIES_STM32H7) || defined(SOC_SERIES_STM32F4) || defined(SOC_SERIES_STM32L4)
    GPIO_InitStruct.Alternate = uart->config->GPIO_Alternate;
#endif
    GPIO_InitStruct.Pin = uart->config->GPIO_TX_Pin;
    HAL_GPIO_Init(uart->config->GPIO_TX_Port, &GPIO_InitStruct);    
    GPIO_InitStruct.Pin = uart->config->GPIO_RX_Pin;
    HAL_GPIO_Init(uart->config->GPIO_RX_Port, &GPIO_InitStruct);
    /* END GPIO INIT */

    if (HAL_UART_Init(&uart->handle) != HAL_OK)
    {
        return -RT_ERROR;
    }
    uart->DR_mask = stm32_uart_get_mask(uart->handle.Init.WordLength, uart->handle.Init.Parity);
    
    return RT_EOK;
}

static rt_err_t stm32_control(struct rt_serial_device *serial, int cmd, void *arg)
{
    struct stm32_uart *uart;
#ifdef RT_SERIAL_USING_DMA
    rt_ubase_t ctrl_arg = (rt_ubase_t)arg;
#endif

    RT_ASSERT(serial != RT_NULL);
    uart = rt_container_of(serial, struct stm32_uart, serial);

    switch (cmd)
    {
    /* disable interrupt */
    case RT_DEVICE_CTRL_CLR_INT:
        /* disable rx irq */
        NVIC_DisableIRQ(uart->config->irq_type);
        /* disable interrupt */
        __HAL_UART_DISABLE_IT(&(uart->handle), UART_IT_RXNE);

#ifdef RT_SERIAL_USING_DMA
        /* disable DMA */
        if (ctrl_arg == RT_DEVICE_FLAG_DMA_RX)
        {
            HAL_NVIC_DisableIRQ(uart->config->dma_rx->dma_irq);
            if (HAL_DMA_Abort(&(uart->dma_rx.handle)) != HAL_OK)
            {
                RT_ASSERT(0);
            }

            if (HAL_DMA_DeInit(&(uart->dma_rx.handle)) != HAL_OK)
            {
                RT_ASSERT(0);
            }
        }
        else if(ctrl_arg == RT_DEVICE_FLAG_DMA_TX)
        {
            HAL_NVIC_DisableIRQ(uart->config->dma_tx->dma_irq);
            if (HAL_DMA_DeInit(&(uart->dma_tx.handle)) != HAL_OK)
            {
                RT_ASSERT(0);
            }
        }
#endif
        break;

    /* enable interrupt */
    case RT_DEVICE_CTRL_SET_INT:
        /* enable rx irq */
        HAL_NVIC_SetPriority(uart->config->irq_type, 1, 0);
        HAL_NVIC_EnableIRQ(uart->config->irq_type);
        /* enable interrupt */
        __HAL_UART_ENABLE_IT(&(uart->handle), UART_IT_RXNE);
        break;

#ifdef RT_SERIAL_USING_DMA
    case RT_DEVICE_CTRL_CONFIG:
        stm32_dma_config(serial, ctrl_arg);
        break;
#endif

    case RT_DEVICE_CTRL_CLOSE:
        if (HAL_UART_DeInit(&(uart->handle)) != HAL_OK )
        {
            RT_ASSERT(0)
        }
        break;

    }
    return RT_EOK;
}



static int stm32_putc(struct rt_serial_device *serial, char c)
{
    struct stm32_uart *uart;
    RT_ASSERT(serial != RT_NULL);

    uart = rt_container_of(serial, struct stm32_uart, serial);
    UART_INSTANCE_CLEAR_FUNCTION(&(uart->handle), UART_FLAG_TC);
#if defined(SOC_SERIES_STM32L4) || defined(SOC_SERIES_STM32WL) || defined(SOC_SERIES_STM32F7) || defined(SOC_SERIES_STM32F0) \
    || defined(SOC_SERIES_STM32L0) || defined(SOC_SERIES_STM32G0) || defined(SOC_SERIES_STM32H7) || defined(SOC_SERIES_STM32L5) \
    || defined(SOC_SERIES_STM32G4) || defined(SOC_SERIES_STM32MP1) || defined(SOC_SERIES_STM32WB) || defined(SOC_SERIES_STM32F3)  \
    || defined(SOC_SERIES_STM32U5)
    uart->handle.Instance->TDR = c;
#else
    uart->handle.Instance->DR = c;
#endif
    while (__HAL_UART_GET_FLAG(&(uart->handle), UART_FLAG_TC) == RESET);
    return 1;
}

static int stm32_getc(struct rt_serial_device *serial)
{
    int ch;
    struct stm32_uart *uart;
    RT_ASSERT(serial != RT_NULL);
    uart = rt_container_of(serial, struct stm32_uart, serial);

    ch = -1;
    if (__HAL_UART_GET_FLAG(&(uart->handle), UART_FLAG_RXNE) != RESET)
    {
#if defined(SOC_SERIES_STM32L4) || defined(SOC_SERIES_STM32WL) || defined(SOC_SERIES_STM32F7) || defined(SOC_SERIES_STM32F0) \
    || defined(SOC_SERIES_STM32L0) || defined(SOC_SERIES_STM32G0) || defined(SOC_SERIES_STM32H7) || defined(SOC_SERIES_STM32L5) \
    || defined(SOC_SERIES_STM32G4) || defined(SOC_SERIES_STM32MP1) || defined(SOC_SERIES_STM32WB)|| defined(SOC_SERIES_STM32F3) \
    || defined(SOC_SERIES_STM32U5)
        ch = uart->handle.Instance->RDR & uart->DR_mask;
#else
        ch = uart->handle.Instance->DR & uart->DR_mask;
#endif
    }
    return ch;
}

static rt_ssize_t stm32_dma_transmit(struct rt_serial_device *serial, rt_uint8_t *buf, rt_size_t size, int direction)
{
    struct stm32_uart *uart;
    RT_ASSERT(serial != RT_NULL);
    RT_ASSERT(buf != RT_NULL);
    uart = rt_container_of(serial, struct stm32_uart, serial);

    if (size == 0)
    {
        return 0;
    }

    if (RT_SERIAL_DMA_TX == direction)
    {
        if (HAL_UART_Transmit_DMA(&uart->handle, buf, size) == HAL_OK)
        {
            return size;
        }
        else
        {
            return 0;
        }
    }
    return 0;
}

#ifdef RT_SERIAL_USING_DMA
static void dma_recv_isr(struct rt_serial_device *serial, rt_uint8_t isr_flag)
{
    struct stm32_uart *uart;
    rt_base_t level;
    rt_size_t recv_len, counter;

    RT_ASSERT(serial != RT_NULL);
    uart = rt_container_of(serial, struct stm32_uart, serial);

    level = rt_hw_interrupt_disable();
    recv_len = 0;
    counter = __HAL_DMA_GET_COUNTER(&(uart->dma_rx.handle));

    switch (isr_flag)
    {
    case UART_RX_DMA_IT_IDLE_FLAG:
        if (counter <= uart->dma_rx.remaining_cnt)
            recv_len = uart->dma_rx.remaining_cnt - counter;
        else
            recv_len = serial->config.bufsz + uart->dma_rx.remaining_cnt - counter;
        break;

    case UART_RX_DMA_IT_HT_FLAG:
        if (counter < uart->dma_rx.remaining_cnt)
            recv_len = uart->dma_rx.remaining_cnt - counter;
        break;

    case UART_RX_DMA_IT_TC_FLAG:
        if(counter >= uart->dma_rx.remaining_cnt)
            recv_len = serial->config.bufsz + uart->dma_rx.remaining_cnt - counter;

    default:
        break;
    }

    if (recv_len)
    {
        uart->dma_rx.remaining_cnt = counter;
        rt_hw_serial_isr(serial, RT_SERIAL_EVENT_RX_DMADONE | (recv_len << 8));
    }
    rt_hw_interrupt_enable(level);

}

#endif

/**
 * Uart common interrupt process. This need add to uart ISR.
 *
 * @param serial serial device
 */
static void uart_isr(struct rt_serial_device *serial)
{
    struct stm32_uart *uart;

    RT_ASSERT(serial != RT_NULL);
    uart = rt_container_of(serial, struct stm32_uart, serial);

    /* UART in mode Receiver -------------------------------------------------*/
    if ((__HAL_UART_GET_FLAG(&(uart->handle), UART_FLAG_RXNE) != RESET) &&
            (__HAL_UART_GET_IT_SOURCE(&(uart->handle), UART_IT_RXNE) != RESET))
    {
        rt_hw_serial_isr(serial, RT_SERIAL_EVENT_RX_IND);
    }
#ifdef RT_SERIAL_USING_DMA
    else if ((uart->uart_dma_flag) && (__HAL_UART_GET_FLAG(&(uart->handle), UART_FLAG_IDLE) != RESET)
             && (__HAL_UART_GET_IT_SOURCE(&(uart->handle), UART_IT_IDLE) != RESET))
    {
        dma_recv_isr(serial, UART_RX_DMA_IT_IDLE_FLAG);
        __HAL_UART_CLEAR_IDLEFLAG(&uart->handle);
    }
    else if (__HAL_UART_GET_FLAG(&(uart->handle), UART_FLAG_TC) &&
            (__HAL_UART_GET_IT_SOURCE(&(uart->handle), UART_IT_TC) != RESET))
    {
        if ((serial->parent.open_flag & RT_DEVICE_FLAG_DMA_TX) != 0)
        {
            HAL_UART_IRQHandler(&(uart->handle));
        }
        UART_INSTANCE_CLEAR_FUNCTION(&(uart->handle), UART_FLAG_TC);
    }
#endif
    else
    {
        if (__HAL_UART_GET_FLAG(&(uart->handle), UART_FLAG_ORE) != RESET)
        {
            __HAL_UART_CLEAR_OREFLAG(&uart->handle);
        }
        if (__HAL_UART_GET_FLAG(&(uart->handle), UART_FLAG_NE) != RESET)
        {
            __HAL_UART_CLEAR_NEFLAG(&uart->handle);
        }
        if (__HAL_UART_GET_FLAG(&(uart->handle), UART_FLAG_FE) != RESET)
        {
            __HAL_UART_CLEAR_FEFLAG(&uart->handle);
        }
        if (__HAL_UART_GET_FLAG(&(uart->handle), UART_FLAG_PE) != RESET)
        {
            __HAL_UART_CLEAR_PEFLAG(&uart->handle);
        }
#if !defined(SOC_SERIES_STM32L4) && !defined(SOC_SERIES_STM32WL) && !defined(SOC_SERIES_STM32F7) && !defined(SOC_SERIES_STM32F0) \
    && !defined(SOC_SERIES_STM32L0) && !defined(SOC_SERIES_STM32G0) && !defined(SOC_SERIES_STM32H7) \
    && !defined(SOC_SERIES_STM32G4) && !defined(SOC_SERIES_STM32MP1) && !defined(SOC_SERIES_STM32WB) \
    && !defined(SOC_SERIES_STM32L5) && !defined(SOC_SERIES_STM32U5)
#ifdef SOC_SERIES_STM32F3
        if (__HAL_UART_GET_FLAG(&(uart->handle), UART_FLAG_LBDF) != RESET)
        {
            UART_INSTANCE_CLEAR_FUNCTION(&(uart->handle), UART_FLAG_LBDF);
        }
#else
        if (__HAL_UART_GET_FLAG(&(uart->handle), UART_FLAG_LBD) != RESET)
        {
            UART_INSTANCE_CLEAR_FUNCTION(&(uart->handle), UART_FLAG_LBD);
        }
#endif
#endif
        if (__HAL_UART_GET_FLAG(&(uart->handle), UART_FLAG_CTS) != RESET)
        {
            UART_INSTANCE_CLEAR_FUNCTION(&(uart->handle), UART_FLAG_CTS);
        }
        if (__HAL_UART_GET_FLAG(&(uart->handle), UART_FLAG_TXE) != RESET)
        {
            UART_INSTANCE_CLEAR_FUNCTION(&(uart->handle), UART_FLAG_TXE);
        }
        if (__HAL_UART_GET_FLAG(&(uart->handle), UART_FLAG_RXNE) != RESET)
        {
            UART_INSTANCE_CLEAR_FUNCTION(&(uart->handle), UART_FLAG_RXNE);
        }
    }
}

#ifndef PIKA_HAL

#if defined(BSP_USING_UART1)
void USART1_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    uart_isr(&(uart_obj[UART1_INDEX].serial));

    /* leave interrupt */
    rt_interrupt_leave();
}
#if defined(RT_SERIAL_USING_DMA) && defined(BSP_UART1_RX_USING_DMA)
void UART1_DMA_RX_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    HAL_DMA_IRQHandler(&uart_obj[UART1_INDEX].dma_rx.handle);

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* defined(RT_SERIAL_USING_DMA) && defined(BSP_UART1_RX_USING_DMA) */
#if defined(RT_SERIAL_USING_DMA) && defined(BSP_UART1_TX_USING_DMA)
void UART1_DMA_TX_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    HAL_DMA_IRQHandler(&uart_obj[UART1_INDEX].dma_tx.handle);

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* defined(RT_SERIAL_USING_DMA) && defined(BSP_UART1_TX_USING_DMA) */
#endif /* BSP_USING_UART1 */

#if defined(BSP_USING_UART2)
void USART2_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    uart_isr(&(uart_obj[UART2_INDEX].serial));

    /* leave interrupt */
    rt_interrupt_leave();
}
#if defined(RT_SERIAL_USING_DMA) && defined(BSP_UART2_RX_USING_DMA)
void UART2_DMA_RX_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    HAL_DMA_IRQHandler(&uart_obj[UART2_INDEX].dma_rx.handle);

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* defined(RT_SERIAL_USING_DMA) && defined(BSP_UART2_RX_USING_DMA) */
#if defined(RT_SERIAL_USING_DMA) && defined(BSP_UART2_TX_USING_DMA)
void UART2_DMA_TX_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    HAL_DMA_IRQHandler(&uart_obj[UART2_INDEX].dma_tx.handle);

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* defined(RT_SERIAL_USING_DMA) && defined(BSP_UART2_TX_USING_DMA) */
#endif /* BSP_USING_UART2 */

#if defined(BSP_USING_UART3)
void USART3_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    uart_isr(&(uart_obj[UART3_INDEX].serial));

    /* leave interrupt */
    rt_interrupt_leave();
}
#if defined(RT_SERIAL_USING_DMA) && defined(BSP_UART3_RX_USING_DMA)
void UART3_DMA_RX_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    HAL_DMA_IRQHandler(&uart_obj[UART3_INDEX].dma_rx.handle);

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* defined(BSP_UART_USING_DMA_RX) && defined(BSP_UART3_RX_USING_DMA) */
#if defined(RT_SERIAL_USING_DMA) && defined(BSP_UART3_TX_USING_DMA)
void UART3_DMA_TX_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    HAL_DMA_IRQHandler(&uart_obj[UART3_INDEX].dma_tx.handle);

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* defined(BSP_UART_USING_DMA_TX) && defined(BSP_UART3_TX_USING_DMA) */
#endif /* BSP_USING_UART3*/

#if defined(BSP_USING_UART4)
void UART4_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    uart_isr(&(uart_obj[UART4_INDEX].serial));

    /* leave interrupt */
    rt_interrupt_leave();
}
#if defined(RT_SERIAL_USING_DMA) && defined(BSP_UART4_RX_USING_DMA)
void UART4_DMA_RX_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    HAL_DMA_IRQHandler(&uart_obj[UART4_INDEX].dma_rx.handle);

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* defined(BSP_UART_USING_DMA_RX) && defined(BSP_UART4_RX_USING_DMA) */

#if defined(RT_SERIAL_USING_DMA) && defined(BSP_UART4_TX_USING_DMA)
void UART4_DMA_TX_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    HAL_DMA_IRQHandler(&uart_obj[UART4_INDEX].dma_tx.handle);

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* defined(BSP_UART_USING_DMA_TX) && defined(BSP_UART4_TX_USING_DMA) */
#endif /* BSP_USING_UART4*/

#if defined(BSP_USING_UART5)
void UART5_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    uart_isr(&(uart_obj[UART5_INDEX].serial));

    /* leave interrupt */
    rt_interrupt_leave();
}
#if defined(RT_SERIAL_USING_DMA) && defined(BSP_UART5_RX_USING_DMA)
void UART5_DMA_RX_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    HAL_DMA_IRQHandler(&uart_obj[UART5_INDEX].dma_rx.handle);

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* defined(RT_SERIAL_USING_DMA) && defined(BSP_UART5_RX_USING_DMA) */
#if defined(RT_SERIAL_USING_DMA) && defined(BSP_UART5_TX_USING_DMA)
void UART5_DMA_TX_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    HAL_DMA_IRQHandler(&uart_obj[UART5_INDEX].dma_tx.handle);

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* defined(RT_SERIAL_USING_DMA) && defined(BSP_UART5_TX_USING_DMA) */
#endif /* BSP_USING_UART5*/

#if defined(BSP_USING_UART6)
void USART6_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    uart_isr(&(uart_obj[UART6_INDEX].serial));

    /* leave interrupt */
    rt_interrupt_leave();
}
#if defined(RT_SERIAL_USING_DMA) && defined(BSP_UART6_RX_USING_DMA)
void UART6_DMA_RX_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    HAL_DMA_IRQHandler(&uart_obj[UART6_INDEX].dma_rx.handle);

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* defined(RT_SERIAL_USING_DMA) && defined(BSP_UART6_RX_USING_DMA) */
#if defined(RT_SERIAL_USING_DMA) && defined(BSP_UART6_TX_USING_DMA)
void UART6_DMA_TX_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    HAL_DMA_IRQHandler(&uart_obj[UART6_INDEX].dma_tx.handle);

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* defined(RT_SERIAL_USING_DMA) && defined(BSP_UART6_TX_USING_DMA) */
#endif /* BSP_USING_UART6*/

#if defined(BSP_USING_UART7)
void UART7_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    uart_isr(&(uart_obj[UART7_INDEX].serial));

    /* leave interrupt */
    rt_interrupt_leave();
}
#if defined(RT_SERIAL_USING_DMA) && defined(BSP_UART7_RX_USING_DMA)
void UART7_DMA_RX_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    HAL_DMA_IRQHandler(&uart_obj[UART7_INDEX].dma_rx.handle);

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* defined(RT_SERIAL_USING_DMA) && defined(BSP_UART7_RX_USING_DMA) */
#if defined(RT_SERIAL_USING_DMA) && defined(BSP_UART7_TX_USING_DMA)
void UART7_DMA_TX_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    HAL_DMA_IRQHandler(&uart_obj[UART7_INDEX].dma_tx.handle);

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* defined(RT_SERIAL_USING_DMA) && defined(BSP_UART7_TX_USING_DMA) */
#endif /* BSP_USING_UART7*/

#if defined(BSP_USING_UART8)
void UART8_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    uart_isr(&(uart_obj[UART8_INDEX].serial));

    /* leave interrupt */
    rt_interrupt_leave();
}
#if defined(RT_SERIAL_USING_DMA) && defined(BSP_UART8_RX_USING_DMA)
void UART8_DMA_RX_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    HAL_DMA_IRQHandler(&uart_obj[UART8_INDEX].dma_rx.handle);

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* defined(RT_SERIAL_USING_DMA) && defined(BSP_UART8_RX_USING_DMA) */
#if defined(RT_SERIAL_USING_DMA) && defined(BSP_UART8_TX_USING_DMA)
void UART8_DMA_TX_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    HAL_DMA_IRQHandler(&uart_obj[UART8_INDEX].dma_tx.handle);

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* defined(RT_SERIAL_USING_DMA) && defined(BSP_UART8_TX_USING_DMA) */
#endif /* BSP_USING_UART8*/

#if defined(BSP_USING_LPUART1)
void LPUART1_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    uart_isr(&(uart_obj[LPUART1_INDEX].serial));

    /* leave interrupt */
    rt_interrupt_leave();
}
#if defined(RT_SERIAL_USING_DMA) && defined(BSP_LPUART1_RX_USING_DMA)
void LPUART1_DMA_RX_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    HAL_DMA_IRQHandler(&uart_obj[LPUART1_INDEX].dma_rx.handle);

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* defined(RT_SERIAL_USING_DMA) && defined(BSP_LPUART1_RX_USING_DMA) */
#endif /* BSP_USING_LPUART1*/

#endif

static void stm32_uart_get_dma_config(void)
{
#ifdef BSP_USING_UART1
    uart_obj[UART1_INDEX].uart_dma_flag = 0;
#ifdef BSP_UART1_RX_USING_DMA
    uart_obj[UART1_INDEX].uart_dma_flag |= RT_DEVICE_FLAG_DMA_RX;
    static struct dma_config uart1_dma_rx = UART1_DMA_RX_CONFIG;
    uart_config[UART1_INDEX].dma_rx = &uart1_dma_rx;
#endif
#ifdef BSP_UART1_TX_USING_DMA
    uart_obj[UART1_INDEX].uart_dma_flag |= RT_DEVICE_FLAG_DMA_TX;
    static struct dma_config uart1_dma_tx = UART1_DMA_TX_CONFIG;
    uart_config[UART1_INDEX].dma_tx = &uart1_dma_tx;
#endif
#endif

#ifdef BSP_USING_UART2
    uart_obj[UART2_INDEX].uart_dma_flag = 0;
#ifdef BSP_UART2_RX_USING_DMA
    uart_obj[UART2_INDEX].uart_dma_flag |= RT_DEVICE_FLAG_DMA_RX;
    static struct dma_config uart2_dma_rx = UART2_DMA_RX_CONFIG;
    uart_config[UART2_INDEX].dma_rx = &uart2_dma_rx;
#endif
#ifdef BSP_UART2_TX_USING_DMA
    uart_obj[UART2_INDEX].uart_dma_flag |= RT_DEVICE_FLAG_DMA_TX;
    static struct dma_config uart2_dma_tx = UART2_DMA_TX_CONFIG;
    uart_config[UART2_INDEX].dma_tx = &uart2_dma_tx;
#endif
#endif

#ifdef BSP_USING_UART3
    uart_obj[UART3_INDEX].uart_dma_flag = 0;
#ifdef BSP_UART3_RX_USING_DMA
    uart_obj[UART3_INDEX].uart_dma_flag |= RT_DEVICE_FLAG_DMA_RX;
    static struct dma_config uart3_dma_rx = UART3_DMA_RX_CONFIG;
    uart_config[UART3_INDEX].dma_rx = &uart3_dma_rx;
#endif
#ifdef BSP_UART3_TX_USING_DMA
    uart_obj[UART3_INDEX].uart_dma_flag |= RT_DEVICE_FLAG_DMA_TX;
    static struct dma_config uart3_dma_tx = UART3_DMA_TX_CONFIG;
    uart_config[UART3_INDEX].dma_tx = &uart3_dma_tx;
#endif
#endif

#ifdef BSP_USING_UART4
    uart_obj[UART4_INDEX].uart_dma_flag = 0;
#ifdef BSP_UART4_RX_USING_DMA
    uart_obj[UART4_INDEX].uart_dma_flag |= RT_DEVICE_FLAG_DMA_RX;
    static struct dma_config uart4_dma_rx = UART4_DMA_RX_CONFIG;
    uart_config[UART4_INDEX].dma_rx = &uart4_dma_rx;
#endif
#ifdef BSP_UART4_TX_USING_DMA
    uart_obj[UART4_INDEX].uart_dma_flag |= RT_DEVICE_FLAG_DMA_TX;
    static struct dma_config uart4_dma_tx = UART4_DMA_TX_CONFIG;
    uart_config[UART4_INDEX].dma_tx = &uart4_dma_tx;
#endif
#endif

#ifdef BSP_USING_UART5
    uart_obj[UART5_INDEX].uart_dma_flag = 0;
#ifdef BSP_UART5_RX_USING_DMA
    uart_obj[UART5_INDEX].uart_dma_flag |= RT_DEVICE_FLAG_DMA_RX;
    static struct dma_config uart5_dma_rx = UART5_DMA_RX_CONFIG;
    uart_config[UART5_INDEX].dma_rx = &uart5_dma_rx;
#endif
#ifdef BSP_UART5_TX_USING_DMA
    uart_obj[UART5_INDEX].uart_dma_flag |= RT_DEVICE_FLAG_DMA_TX;
    static struct dma_config uart5_dma_tx = UART5_DMA_TX_CONFIG;
    uart_config[UART5_INDEX].dma_tx = &uart5_dma_tx;
#endif
#endif

#ifdef BSP_USING_UART6
    uart_obj[UART6_INDEX].uart_dma_flag = 0;
#ifdef BSP_UART6_RX_USING_DMA
    uart_obj[UART6_INDEX].uart_dma_flag |= RT_DEVICE_FLAG_DMA_RX;
    static struct dma_config uart6_dma_rx = UART6_DMA_RX_CONFIG;
    uart_config[UART6_INDEX].dma_rx = &uart6_dma_rx;
#endif
#ifdef BSP_UART6_TX_USING_DMA
    uart_obj[UART6_INDEX].uart_dma_flag |= RT_DEVICE_FLAG_DMA_TX;
    static struct dma_config uart6_dma_tx = UART6_DMA_TX_CONFIG;
    uart_config[UART6_INDEX].dma_tx = &uart6_dma_tx;
#endif
#endif

#ifdef BSP_USING_UART7
    uart_obj[UART7_INDEX].uart_dma_flag = 0;
#ifdef BSP_UART7_RX_USING_DMA
    uart_obj[UART7_INDEX].uart_dma_flag |= RT_DEVICE_FLAG_DMA_RX;
    static struct dma_config uart7_dma_rx = UART7_DMA_RX_CONFIG;
    uart_config[UART7_INDEX].dma_rx = &uart7_dma_rx;
#endif
#ifdef BSP_UART7_TX_USING_DMA
    uart_obj[UART7_INDEX].uart_dma_flag |= RT_DEVICE_FLAG_DMA_TX;
    static struct dma_config uart7_dma_tx = UART7_DMA_TX_CONFIG;
    uart_config[UART7_INDEX].dma_tx = &uart7_dma_tx;
#endif
#endif

#ifdef BSP_USING_UART8
    uart_obj[UART8_INDEX].uart_dma_flag = 0;
#ifdef BSP_UART8_RX_USING_DMA
    uart_obj[UART8_INDEX].uart_dma_flag |= RT_DEVICE_FLAG_DMA_RX;
    static struct dma_config uart8_dma_rx = UART8_DMA_RX_CONFIG;
    uart_config[UART8_INDEX].dma_rx = &uart8_dma_rx;
#endif
#ifdef BSP_UART8_TX_USING_DMA
    uart_obj[UART8_INDEX].uart_dma_flag |= RT_DEVICE_FLAG_DMA_TX;
    static struct dma_config uart8_dma_tx = UART8_DMA_TX_CONFIG;
    uart_config[UART8_INDEX].dma_tx = &uart8_dma_tx;
#endif
#endif
}

#ifdef RT_SERIAL_USING_DMA
static void stm32_dma_config(struct rt_serial_device *serial, rt_ubase_t flag)
{
    struct rt_serial_rx_fifo *rx_fifo;
    DMA_HandleTypeDef *DMA_Handle;
    struct dma_config *dma_config;
    struct stm32_uart *uart;

    RT_ASSERT(serial != RT_NULL);
    RT_ASSERT(flag == RT_DEVICE_FLAG_DMA_TX || flag == RT_DEVICE_FLAG_DMA_RX);
    uart = rt_container_of(serial, struct stm32_uart, serial);

    if (RT_DEVICE_FLAG_DMA_RX == flag)
    {
        DMA_Handle = &uart->dma_rx.handle;
        dma_config = uart->config->dma_rx;
    }
    else /* RT_DEVICE_FLAG_DMA_TX == flag */
    {
        DMA_Handle = &uart->dma_tx.handle;
        dma_config = uart->config->dma_tx;
    }
    LOG_D("%s dma config start", uart->config->name);

    {
        rt_uint32_t tmpreg = 0x00U;
#if defined(SOC_SERIES_STM32F1) || defined(SOC_SERIES_STM32F0) || defined(SOC_SERIES_STM32G0) \
    || defined(SOC_SERIES_STM32L0)|| defined(SOC_SERIES_STM32F3) || defined(SOC_SERIES_STM32L1)
        /* enable DMA clock && Delay after an RCC peripheral clock enabling*/
        SET_BIT(RCC->AHBENR, dma_config->dma_rcc);
        tmpreg = READ_BIT(RCC->AHBENR, dma_config->dma_rcc);
#elif defined(SOC_SERIES_STM32F4) || defined(SOC_SERIES_STM32F7) || defined(SOC_SERIES_STM32L4) || defined(SOC_SERIES_STM32WL) \
    || defined(SOC_SERIES_STM32G4)|| defined(SOC_SERIES_STM32H7) || defined(SOC_SERIES_STM32WB)
        /* enable DMA clock && Delay after an RCC peripheral clock enabling*/
        SET_BIT(RCC->AHB1ENR, dma_config->dma_rcc);
        tmpreg = READ_BIT(RCC->AHB1ENR, dma_config->dma_rcc);
#elif defined(SOC_SERIES_STM32MP1)
        /* enable DMA clock && Delay after an RCC peripheral clock enabling*/
        SET_BIT(RCC->MP_AHB2ENSETR, dma_config->dma_rcc);
        tmpreg = READ_BIT(RCC->MP_AHB2ENSETR, dma_config->dma_rcc);
#endif

#if (defined(SOC_SERIES_STM32L4) || defined(SOC_SERIES_STM32WL) || defined(SOC_SERIES_STM32G4) || defined(SOC_SERIES_STM32WB)) && defined(DMAMUX1)
        /* enable DMAMUX clock for L4+ and G4 */
        __HAL_RCC_DMAMUX1_CLK_ENABLE();
#elif defined(SOC_SERIES_STM32MP1)
        __HAL_RCC_DMAMUX_CLK_ENABLE();
#endif

        UNUSED(tmpreg);   /* To avoid compiler warnings */
    }

    if (RT_DEVICE_FLAG_DMA_RX == flag)
    {
        __HAL_LINKDMA(&(uart->handle), hdmarx, uart->dma_rx.handle);
    }
    else if (RT_DEVICE_FLAG_DMA_TX == flag)
    {
        __HAL_LINKDMA(&(uart->handle), hdmatx, uart->dma_tx.handle);
    }

#if defined(SOC_SERIES_STM32F1) || defined(SOC_SERIES_STM32F0) || defined(SOC_SERIES_STM32L0)|| defined(SOC_SERIES_STM32F3) || defined(SOC_SERIES_STM32L1) || defined(SOC_SERIES_STM32U5)
    DMA_Handle->Instance                 = dma_config->Instance;
#elif defined(SOC_SERIES_STM32F4) || defined(SOC_SERIES_STM32F7)
    DMA_Handle->Instance                 = dma_config->Instance;
    DMA_Handle->Init.Channel             = dma_config->channel;
#elif defined(SOC_SERIES_STM32L4) || defined(SOC_SERIES_STM32WL) || defined(SOC_SERIES_STM32G0) || defined(SOC_SERIES_STM32G4) || defined(SOC_SERIES_STM32WB)\
    || defined(SOC_SERIES_STM32H7) || defined(SOC_SERIES_STM32MP1)
    DMA_Handle->Instance                 = dma_config->Instance;
    DMA_Handle->Init.Request             = dma_config->request;
#endif
    DMA_Handle->Init.PeriphInc           = DMA_PINC_DISABLE;
    DMA_Handle->Init.MemInc              = DMA_MINC_ENABLE;
    DMA_Handle->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    DMA_Handle->Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;

    if (RT_DEVICE_FLAG_DMA_RX == flag)
    {
        DMA_Handle->Init.Direction           = DMA_PERIPH_TO_MEMORY;
        DMA_Handle->Init.Mode                = DMA_CIRCULAR;
    }
    else if (RT_DEVICE_FLAG_DMA_TX == flag)
    {
        DMA_Handle->Init.Direction           = DMA_MEMORY_TO_PERIPH;
        DMA_Handle->Init.Mode                = DMA_NORMAL;
    }

    DMA_Handle->Init.Priority            = DMA_PRIORITY_MEDIUM;
#if defined(SOC_SERIES_STM32F4) || defined(SOC_SERIES_STM32F7) || defined(SOC_SERIES_STM32H7) || defined(SOC_SERIES_STM32MP1)
    DMA_Handle->Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
#endif
    if (HAL_DMA_DeInit(DMA_Handle) != HAL_OK)
    {
        RT_ASSERT(0);
    }

    if (HAL_DMA_Init(DMA_Handle) != HAL_OK)
    {
        RT_ASSERT(0);
    }

    /* enable interrupt */
    if (flag == RT_DEVICE_FLAG_DMA_RX)
    {
        rx_fifo = (struct rt_serial_rx_fifo *)serial->serial_rx;
        /* Start DMA transfer */
        if (HAL_UART_Receive_DMA(&(uart->handle), rx_fifo->buffer, serial->config.bufsz) != HAL_OK)
        {
            /* Transfer error in reception process */
            RT_ASSERT(0);
        }
        CLEAR_BIT(uart->handle.Instance->CR3, USART_CR3_EIE);
        __HAL_UART_ENABLE_IT(&(uart->handle), UART_IT_IDLE);
    }

    /* DMA irq should set in DMA TX mode, or HAL_UART_TxCpltCallback function will not be called */
    HAL_NVIC_SetPriority(dma_config->dma_irq, 0, 0);
    HAL_NVIC_EnableIRQ(dma_config->dma_irq);

    HAL_NVIC_SetPriority(uart->config->irq_type, 1, 0);
    HAL_NVIC_EnableIRQ(uart->config->irq_type);

    LOG_D("%s dma %s instance: %x", uart->config->name, flag == RT_DEVICE_FLAG_DMA_RX ? "RX" : "TX", DMA_Handle->Instance);
    LOG_D("%s dma config done", uart->config->name);
}

/**
  * @brief  UART error callbacks
  * @param  huart: UART handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    RT_ASSERT(huart != NULL);
    struct stm32_uart *uart = (struct stm32_uart *)huart;
    LOG_D("%s: %s %d\n", __FUNCTION__, uart->config->name, huart->ErrorCode);
    UNUSED(uart);
}

/**
  * @brief  Rx Transfer completed callback
  * @param  huart: UART handle
  * @note   This example shows a simple way to report end of DMA Rx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    struct stm32_uart *uart;
    RT_ASSERT(huart != NULL);
    uart = (struct stm32_uart *)huart;
    dma_recv_isr(&uart->serial, UART_RX_DMA_IT_TC_FLAG);
}

/**
  * @brief  Rx Half transfer completed callback
  * @param  huart: UART handle
  * @note   This example shows a simple way to report end of DMA Rx Half transfer,
  *         and you can add your own implementation.
  * @retval None
  */
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
    struct stm32_uart *uart;
    RT_ASSERT(huart != NULL);
    uart = (struct stm32_uart *)huart;
    dma_recv_isr(&uart->serial, UART_RX_DMA_IT_HT_FLAG);
}

static void _dma_tx_complete(struct rt_serial_device *serial)
{
    struct stm32_uart *uart;
    rt_size_t trans_total_index;
    rt_base_t level;

    RT_ASSERT(serial != RT_NULL);
    uart = rt_container_of(serial, struct stm32_uart, serial);

    level = rt_hw_interrupt_disable();
    trans_total_index = __HAL_DMA_GET_COUNTER(&(uart->dma_tx.handle));
    rt_hw_interrupt_enable(level);

    if (trans_total_index == 0)
    {
        rt_hw_serial_isr(serial, RT_SERIAL_EVENT_TX_DMADONE);
    }
}

/**
  * @brief  HAL_UART_TxCpltCallback
  * @param  huart: UART handle
  * @note   This callback can be called by two functions, first in UART_EndTransmit_IT when
  *         UART Tx complete and second in UART_DMATransmitCplt function in DMA Circular mode.
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    struct stm32_uart *uart;
    RT_ASSERT(huart != NULL);
    uart = (struct stm32_uart *)huart;
    _dma_tx_complete(&uart->serial);
}
#endif  /* RT_SERIAL_USING_DMA */

struct rt_uart_ops
{
    rt_err_t (*configure)(struct rt_serial_device *serial, struct serial_configure *cfg);
    rt_err_t (*control)(struct rt_serial_device *serial, int cmd, void *arg);

    int (*putc)(struct rt_serial_device *serial, char c);
    int (*getc)(struct rt_serial_device *serial);

    rt_ssize_t (*dma_transmit)(struct rt_serial_device *serial, rt_uint8_t *buf, rt_size_t size, int direction);
};

static const struct rt_uart_ops stm32_uart_ops =
{
    .configure = stm32_configure,
    .control = stm32_control,
    .putc = stm32_putc,
    .getc = stm32_getc,
    .dma_transmit = stm32_dma_transmit
};

static rt_err_t rt_hw_serial_register(struct rt_serial_device *serial,
                               const char              *name,
                               rt_uint32_t              flag,
                               void                    *data)
{
    rt_err_t ret;
    struct rt_device *device;
    RT_ASSERT(serial != RT_NULL);

    device = &(serial->parent);

    device->type        = RT_Device_Class_Char;
    device->rx_indicate = RT_NULL;
    device->tx_complete = RT_NULL;

#ifdef RT_USING_DEVICE_OPS
    // device->ops         = &serial_ops;
    device->ops         = NULL;
#else
    device->init        = rt_serial_init;
    device->open        = rt_serial_open;
    device->close       = rt_serial_close;
    device->read        = rt_serial_read;
    device->write       = rt_serial_write;
    device->control     = rt_serial_control;
#endif
    device->user_data   = data;

    /* register a character device */
    // ret = rt_device_register(device, name, flag);

#ifdef RT_USING_POSIX_STDIO
    /* set fops */
    device->fops        = &_serial_fops;
#endif

    return RT_EOK;
}

/* Default config for serial_configure structure */
#define RT_SERIAL_CONFIG_DEFAULT           \
{                                          \
    BAUD_RATE_115200, /* 115200 bits/s */  \
    DATA_BITS_8,      /* 8 databits */     \
    STOP_BITS_1,      /* 1 stopbit */      \
    PARITY_NONE,      /* No parity  */     \
    BIT_ORDER_LSB,    /* LSB first sent */ \
    NRZ_NORMAL,       /* Normal mode */    \
    RT_SERIAL_RB_BUFSZ, /* Buffer size */  \
    RT_SERIAL_FLOWCONTROL_NONE, /* Off flowcontrol */ \
    0                                      \
}

static void uart_clock_init(void){
#if defined(STM32H7)
    RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit = {0};
    // Configure USART1/6 and USART2/3/4/5/7/8 clock sources
    RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART16 | RCC_PERIPHCLK_USART234578;
    RCC_PeriphClkInit.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
    RCC_PeriphClkInit.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit) != HAL_OK) {
        pika_platform_printf("Error at HAL_RCCEx_PeriphCLKConfig\r\n");
    }
#endif
}

static int stricmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        int diff = tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
        if (diff != 0) {
            return diff;
        }
        s1++;
        s2++;
    }
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

static volatile int uart_inited = 0;
static int rt_hw_usart_init(void)
{
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
    rt_err_t result = 0;
    
    uart_clock_init();

    stm32_uart_get_dma_config();

    for (rt_size_t i = 0; i < sizeof(uart_obj) / sizeof(struct stm32_uart); i++)
    {
        /* init UART object */
        uart_obj[i].config = &uart_config[i];
        uart_obj[i].serial.ops    = &stm32_uart_ops;
        uart_obj[i].serial.config = config;

        /* register UART device */
        result = rt_hw_serial_register(&uart_obj[i].serial, uart_obj[i].config->name,
                                       RT_DEVICE_FLAG_RDWR
                                       | RT_DEVICE_FLAG_INT_RX
                                       | RT_DEVICE_FLAG_INT_TX
                                       | uart_obj[i].uart_dma_flag
                                       , NULL);
        RT_ASSERT(result == RT_EOK);
    }

    return result;
}

typedef struct platform_UART {
    struct stm32_uart* rt_uart;
} platform_UART;

int pika_hal_platform_UART_open(pika_dev* dev, char* name) {
    if (uart_inited == 0){
        rt_hw_usart_init();
        uart_inited = 1;
    }
    for (rt_size_t i = 0; i < sizeof(uart_obj) / sizeof(struct stm32_uart); i++){
        if (0 == stricmp(name, uart_obj[i].config->name)){
            platform_UART* uart = (platform_UART*)pikaMalloc(sizeof(platform_UART));
            if (NULL == uart){
                return -1;
            }
            uart->rt_uart = &uart_obj[i];
            dev->platform_data = uart;
            return 0;
        }
    }
    pika_platform_printf("Error: UART %s not found\r\n", name);
    pika_platform_printf("Available UARTs:\r\n");
    for (rt_size_t i = 0; i < sizeof(uart_obj) / sizeof(struct stm32_uart); i++){
        pika_platform_printf("'%s'\r\n", uart_obj[i].config->name);
    }
    return -1;
}

int pika_hal_platform_UART_close(pika_dev* dev) {
    platform_UART* uart = (platform_UART*)dev->platform_data;
    if(NULL != uart){
        pikaFree(uart, sizeof(platform_UART));
    }
    return 0;
}

int pika_hal_platform_UART_ioctl_config(pika_dev* dev,
                                        pika_hal_UART_config* cfg) {
    return 0;
}

int pika_hal_platform_UART_ioctl_enable(pika_dev* dev) {
    platform_UART* uart = (platform_UART*)dev->platform_data;
    pika_hal_UART_config* cfg = (pika_hal_UART_config*)dev->ioctl_config;
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
    switch(cfg->data_bits){
        case PIKA_HAL_UART_DATA_BITS_8:
            config.data_bits = DATA_BITS_8;
            break;
        case PIKA_HAL_UART_DATA_BITS_7:
            config.data_bits = DATA_BITS_7;
            break;
        case PIKA_HAL_UART_DATA_BITS_6:
            config.data_bits = DATA_BITS_6;
            break;
        case PIKA_HAL_UART_DATA_BITS_5:
            config.data_bits = DATA_BITS_5;
            break;
        default:
            pika_platform_printf("Error: invalid data bits %d\r\n", cfg->data_bits);
            return -1;
    }
    switch(cfg->flow_control){
        case PIKA_HAL_UART_FLOW_CONTROL_NONE:
            config.flowcontrol = RT_SERIAL_FLOWCONTROL_NONE;
            break;
        case PIKA_HAL_UART_FLOW_CONTROL_RTS_CTS:
            config.flowcontrol = RT_SERIAL_FLOWCONTROL_CTSRTS;
            break;
        default:
            pika_platform_printf("Error: invalid flow control %d\r\n", cfg->flow_control);
            return -1;
    }
    switch(cfg->parity){
        case PIKA_HAL_UART_PARITY_NONE:
            config.parity = PARITY_NONE;
            break;
        case PIKA_HAL_UART_PARITY_EVEN:
            config.parity = PARITY_EVEN;
            break;
        case PIKA_HAL_UART_PARITY_ODD:
            config.parity = PARITY_ODD;
            break;
        default:
            pika_platform_printf("Error: invalid parity %d\r\n", cfg->parity);
            return -1;
    }
    switch(cfg->stop_bits){
        case PIKA_HAL_UART_STOP_BITS_1:
            config.stop_bits = STOP_BITS_1;
            break;
        case PIKA_HAL_UART_STOP_BITS_2:
            config.stop_bits = STOP_BITS_2;
            break;
        default:
            pika_platform_printf("Error: invalid stop bits %d\r\n", cfg->stop_bits);
            return -1;
    }
    config.baud_rate = cfg->baudrate;
    stm32_configure(&uart->rt_uart->serial, &config);
    return 0;
}

int pika_hal_platform_UART_ioctl_disable(pika_dev* dev) {
    return -1;
}

int pika_hal_platform_UART_read(pika_dev* dev, void* buf, size_t count) {
    platform_UART* uart = (platform_UART*)dev->platform_data;
    for (size_t i = 0; i < count; i++){
        ((char*)buf)[i] = stm32_getc(&uart->rt_uart->serial);
    }
    return 0;
}

int pika_hal_platform_UART_write(pika_dev* dev, void* buf, size_t count) {
    platform_UART* uart = (platform_UART*)dev->platform_data;
    HAL_UART_Transmit(&uart->rt_uart->handle, buf, count, 0xffff);
    return 0;
}

// #endif /* RT_USING_SERIAL */
