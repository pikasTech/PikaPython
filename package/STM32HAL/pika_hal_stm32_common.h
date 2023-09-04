#ifndef _PIKA_HAL_STM32_COMMON_H_
#define _PIKA_HAL_STM32_COMMON_H_

#include "../PikaStdDevice/pika_hal.h"
#include "pika_adapter_rtt.h"

#ifndef rt_container_of
#define rt_container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))
#endif
    
#define CHIP_FAMILY_STM32
#define SOC_FAMILY_STM32
#define RT_USING_DEVICE_OPS

#if defined(STM32H750xx) || defined(STM32H743xx) || defined(STM32H753xx) || defined(STM32H755xx) || defined(STM32H757xx)
#define SOC_SERIES_STM32H7
#define CHIP_SERIES_STM32H7
#include "stm32h7xx_hal.h"
#elif defined(STM32F103xx) || defined(STM32F101xx) || defined(STM32F102xx)
#define SOC_SERIES_STM32F1
#define CHIP_SERIES_STM32F1
#include "stm32f1xx_hal.h"
#elif defined(STM32G030xx) || defined(STM32G031xx) || defined(STM32G041xx) || defined(STM32G070xx) || defined(STM32G071xx) || defined(STM32G081xx)
#define SOC_SERIES_STM32G0
#define CHIP_SERIES_STM32G0
#include "stm32g0xx_hal.h"
#elif defined(STM32F030xx) || defined(STM32F031xx) || defined(STM32F051xx) || defined(STM32F070xx) || defined(STM32F042xx)
#define SOC_SERIES_STM32F0
#define CHIP_SERIES_STM32F0
#include "stm32f0xx_hal.h"
#elif defined(STM32F401xx) || defined(STM32F405xx) || defined(STM32F407xx) || defined(STM32F411xx)
#define SOC_SERIES_STM32F4
#define CHIP_SERIES_STM32F4
#include "stm32f4xx_hal.h"
#elif defined(STM32U585xx) || defined(STM32U575xx)
#define SOC_SERIES_STM32U5
#define CHIP_SERIES_STM32U5
#include "stm32u5xx_hal.h"
#elif defined(STM32L451xx) || defined(STM32L452xx) || defined(STM32L462xx) || defined(STM32L471xx) || defined(STM32L475xx) || defined(STM32L476xx) || defined(STM32L485xx) || defined(STM32L486xx)
#define SOC_SERIES_STM32L4
#define CHIP_SERIES_STM32L4
#include "stm32l4xx_hal.h"
#else
#error "Unsupported STM32 series"
#endif

/* struct */

struct rt_pin_irq_hdr
{
    rt_base_t        pin;
    rt_uint8_t       mode; /* e.g. PIN_IRQ_MODE_RISING */
    void (*hdr)(void *args);
    void             *args;
};

struct rt_pin_ops
{
    void (*pin_mode)(struct rt_device *device, rt_base_t pin, rt_uint8_t mode);
    void (*pin_write)(struct rt_device *device, rt_base_t pin, rt_uint8_t value);
    rt_int8_t  (*pin_read)(struct rt_device *device, rt_base_t pin);
    rt_err_t (*pin_attach_irq)(struct rt_device *device, rt_base_t pin,
            rt_uint8_t mode, void (*hdr)(void *args), void *args);
    rt_err_t (*pin_detach_irq)(struct rt_device *device, rt_base_t pin);
    rt_err_t (*pin_irq_enable)(struct rt_device *device, rt_base_t pin, rt_uint8_t enabled);
    rt_base_t (*pin_get)(const char *name);
};


#define PIN_NONE                (-1)

#define PIN_LOW                 0x00
#define PIN_HIGH                0x01

#define PIN_MODE_OUTPUT         0x00
#define PIN_MODE_INPUT          0x01
#define PIN_MODE_INPUT_PULLUP   0x02
#define PIN_MODE_INPUT_PULLDOWN 0x03
#define PIN_MODE_OUTPUT_OD      0x04

#define PIN_IRQ_MODE_RISING             0x00
#define PIN_IRQ_MODE_FALLING            0x01
#define PIN_IRQ_MODE_RISING_FALLING     0x02
#define PIN_IRQ_MODE_HIGH_LEVEL         0x03
#define PIN_IRQ_MODE_LOW_LEVEL          0x04

#define PIN_IRQ_DISABLE                 0x00
#define PIN_IRQ_ENABLE                  0x01

#define PIN_IRQ_PIN_NONE                PIN_NONE

#define RT_DEVICE_CTRL_RESUME           0x01            /**< resume device */
#define RT_DEVICE_CTRL_SUSPEND          0x02            /**< suspend device */
#define RT_DEVICE_CTRL_CONFIG           0x03            /**< configure device */
#define RT_DEVICE_CTRL_CLOSE            0x04            /**< close device */
#define RT_DEVICE_CTRL_NOTIFY_SET       0x05            /**< set notify func */
#define RT_DEVICE_CTRL_SET_INT          0x06            /**< set interrupt */
#define RT_DEVICE_CTRL_CLR_INT          0x07            /**< clear interrupt */
#define RT_DEVICE_CTRL_GET_INT          0x08            /**< get interrupt status */
#define RT_DEVICE_CTRL_CONSOLE_OFLAG    0x09            /**< get console open flag */
#define RT_DEVICE_CTRL_MASK             0x1f            /**< mask for contrl commands */

#define RT_DEVICE_FLAG_DEACTIVATE       0x000           /**< device is not not initialized */

#define RT_DEVICE_FLAG_RDONLY           0x001           /**< read only */
#define RT_DEVICE_FLAG_WRONLY           0x002           /**< write only */
#define RT_DEVICE_FLAG_RDWR             0x003           /**< read and write */

#define RT_DEVICE_FLAG_REMOVABLE        0x004           /**< removable device */
#define RT_DEVICE_FLAG_STANDALONE       0x008           /**< standalone device */
#define RT_DEVICE_FLAG_ACTIVATED        0x010           /**< device is activated */
#define RT_DEVICE_FLAG_SUSPENDED        0x020           /**< device is suspended */
#define RT_DEVICE_FLAG_STREAM           0x040           /**< stream mode */

#define RT_DEVICE_FLAG_INT_RX           0x100           /**< INT mode on Rx */
#define RT_DEVICE_FLAG_DMA_RX           0x200           /**< DMA mode on Rx */
#define RT_DEVICE_FLAG_INT_TX           0x400           /**< INT mode on Tx */
#define RT_DEVICE_FLAG_DMA_TX           0x800           /**< DMA mode on Tx */

#define RT_DEVICE_OFLAG_CLOSE           0x000           /**< device is closed */
#define RT_DEVICE_OFLAG_RDONLY          0x001           /**< read only access */
#define RT_DEVICE_OFLAG_WRONLY          0x002           /**< write only access */
#define RT_DEVICE_OFLAG_RDWR            0x003           /**< read and write */
#define RT_DEVICE_OFLAG_OPEN            0x008           /**< device is opened */
#define RT_DEVICE_OFLAG_MASK            0xf0f           /**< mask of open flag */

#ifdef RT_USING_DEVICE_OPS
/**
 * operations set for device object
 */
struct rt_device_ops
{
    /* common device interface */
    rt_err_t  (*init)   (rt_device_t dev);
    rt_err_t  (*open)   (rt_device_t dev, rt_uint16_t oflag);
    rt_err_t  (*close)  (rt_device_t dev);
    rt_ssize_t (*read)  (rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size);
    rt_ssize_t (*write) (rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size);
    rt_err_t  (*control)(rt_device_t dev, int cmd, void *args);
};
#endif /* RT_USING_DEVICE_OPS */

static inline rt_base_t rt_hw_interrupt_disable(void){
    pika_platform_disable_irq_handle();
    return 0;
}

static inline void rt_hw_interrupt_enable(rt_base_t level){
    pika_platform_enable_irq_handle();
}

struct rt_adc_device;
struct rt_adc_ops
{
    rt_err_t (*enabled)(struct rt_adc_device *device, rt_int8_t channel, rt_bool_t enabled);
    rt_err_t (*convert)(struct rt_adc_device *device, rt_int8_t channel, rt_uint32_t *value);
    rt_uint8_t (*get_resolution)(struct rt_adc_device *device);
    rt_int16_t (*get_vref) (struct rt_adc_device *device);
};

struct rt_adc_device
{
    struct rt_device parent;
    const struct rt_adc_ops *ops;
};
typedef struct rt_adc_device *rt_adc_device_t;

static rt_ssize_t _adc_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    rt_err_t result = RT_EOK;
    rt_size_t i;
    struct rt_adc_device *adc = (struct rt_adc_device *)dev;
    rt_uint32_t *value = (rt_uint32_t *)buffer;

    for (i = 0; i < size; i += sizeof(int))
    {
        result = adc->ops->convert(adc, pos + i, value);
        if (result != RT_EOK)
        {
            return 0;
        }
        value++;
    }

    return i;
}

static inline rt_err_t rt_adc_enable(rt_adc_device_t dev, rt_int8_t channel)
{
    rt_err_t result = RT_EOK;

    RT_ASSERT(dev);

    if (dev->ops->enabled != RT_NULL)
    {
        result = dev->ops->enabled(dev, channel, RT_TRUE);
    }
    else
    {
        result = -RT_ENOSYS;
    }

    return result;
}

static inline rt_err_t rt_adc_disable(rt_adc_device_t dev, rt_int8_t channel)
{
    rt_err_t result = RT_EOK;

    RT_ASSERT(dev);

    if (dev->ops->enabled != RT_NULL)
    {
        result = dev->ops->enabled(dev, channel, RT_FALSE);
    }
    else
    {
        result = -RT_ENOSYS;
    }

    return result;
}

static inline rt_uint32_t rt_adc_read(rt_adc_device_t dev, rt_int8_t channel)
{
    rt_uint32_t value;

    RT_ASSERT(dev);

    dev->ops->convert(dev, channel, &value);

    return value;
}


#define PIN_NUM(port, no) (((((port) & 0xFu) << 4) | ((no) & 0xFu)))
#define PIN_PORT(pin) ((uint8_t)(((pin) >> 4) & 0xFu))
#define PIN_NO(pin) ((uint8_t)((pin) & 0xFu))

#if defined(SOC_SERIES_STM32MP1)
#if defined(GPIOZ)
#define gpioz_port_base (175) /* PIN_STPORT_MAX * 16 - 16 */
#define PIN_STPORT(pin) ((pin > gpioz_port_base) ? ((GPIO_TypeDef *)(GPIOZ_BASE )) : ((GPIO_TypeDef *)(GPIOA_BASE + (0x1000u * PIN_PORT(pin)))))
#else
#define PIN_STPORT(pin) ((GPIO_TypeDef *)(GPIOA_BASE + (0x1000u * PIN_PORT(pin))))
#endif /* GPIOZ */
#else
#define PIN_STPORT(pin) ((GPIO_TypeDef *)(GPIOA_BASE + (0x400u * PIN_PORT(pin))))
#endif /* SOC_SERIES_STM32MP1 */

#define PIN_STPIN(pin) ((uint16_t)(1u << PIN_NO(pin)))

/* e.g. PA7 */
static inline rt_base_t _stm32_pin_get(const char *name)
{
    rt_base_t pin = 0;
    int hw_port_num, hw_pin_num = 0;
    int i, name_len;

    name_len = rt_strlen(name);

    if ((name_len < 3) || (name_len >= 5))
    {
        return -RT_EINVAL;
    }
    if ((name[0] != 'P'))
    {
        return -RT_EINVAL;
    }

    if ((name[1] >= 'A') && (name[1] <= 'Z'))
    {
        hw_port_num = (int)(name[1] - 'A');
    }
    else
    {
        return -RT_EINVAL;
    }

    for (i = 2; i < name_len; i++)
    {
        hw_pin_num *= 10;
        hw_pin_num += name[i] - '0';
    }

    pin = PIN_NUM(hw_port_num, hw_pin_num);

    return pin;
}

#define RT_ADC_INTERN_CH_TEMPER     (-1)
#define RT_ADC_INTERN_CH_VREF       (-2)
#define RT_ADC_INTERN_CH_VBAT       (-3)


#define BSP_USING_GPIO
#define RT_USING_PIN

#ifdef ADC1
#define BSP_USING_ADC1
#endif

#ifdef DAC1
#define BSP_USING_DAC1
#endif

#if defined(USART1) || defined(UART1)
#define BSP_USING_UART1
#endif

#if defined(USART2) || defined(UART2)
#define BSP_USING_UART2
#endif

#if defined(USART3) || defined(UART3)
#define BSP_USING_UART3
#endif

#if defined(USART4) || defined(UART4)
#define BSP_USING_UART4
#endif

#if defined(USART5) || defined(UART5)
#define BSP_USING_UART5
#endif

#if defined(USART6) || defined(UART6)
#define BSP_USING_UART6
#endif

#if defined(USART7) || defined(UART7)
#define BSP_USING_UART7
#endif

#if defined(USART8) || defined(UART8)
#define BSP_USING_UART8
#endif


#if defined(BSP_USING_UART1) || defined(BSP_USING_UART2) || defined(BSP_USING_UART3) || \
    defined(BSP_USING_UART4) || defined(BSP_USING_UART5) || defined(BSP_USING_UART6) || \
    defined(BSP_USING_UART7) || defined(BSP_USING_UART8) || defined(BSP_USING_LPUART1)
#define RT_USING_SERIAL
#endif

struct rt_dac_device;
struct rt_dac_ops
{
    rt_err_t (*disabled)(struct rt_dac_device *device, rt_uint32_t channel);
    rt_err_t (*enabled)(struct rt_dac_device *device, rt_uint32_t channel);
    rt_err_t (*convert)(struct rt_dac_device *device, rt_uint32_t channel, rt_uint32_t *value);
    rt_uint8_t (*get_resolution)(struct rt_dac_device *device);
};

struct rt_dac_device
{
    struct rt_device parent;
    const struct rt_dac_ops *ops;
};

typedef struct rt_dac_device *rt_dac_device_t;

#endif

