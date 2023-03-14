#include <bl_gpio.h>
#include <hosal_gpio.h>
#include "../PikaStdDevice/pika_hal.h"
#include "pika_hal_BLIOT_irq_task.h"

int pika_hal_platform_GPIO_open(pika_dev* dev, char* name) {
    dev->platform_data = pikaMalloc(sizeof(hosal_gpio_dev_t));
    memset(dev->platform_data, 0, sizeof(hosal_gpio_dev_t));
    hosal_gpio_dev_t* platform_gpio = (hosal_gpio_dev_t*)dev->platform_data;
    /* Support P1/P2 ... */
    platform_gpio->port = fast_atoi(name + 1);
    return 0;
}

int pika_hal_platform_GPIO_close(pika_dev* dev) {
    pikaFree(dev->platform_data, sizeof(hosal_gpio_dev_t));
    return 0;
}

int pika_hal_platform_GPIO_read(pika_dev* dev, void* buf, size_t count) {
    uint8_t value = 0;
    hosal_gpio_dev_t* platform_gpio = (hosal_gpio_dev_t*)dev->platform_data;
    hosal_gpio_input_get(platform_gpio, &value);
    uint32_t value_in = value;
    memcpy(buf, &value_in, sizeof(value_in));
#if PIKA_DEBUG_ENABLE
    __platform_printf("GPIO read port %d to %d\r\n", platform_gpio->port,
                      value_in);
#endif
    return 0;
}

int pika_hal_platform_GPIO_write(pika_dev* dev, void* buf, size_t count) {
    hosal_gpio_dev_t* platform_gpio = (hosal_gpio_dev_t*)dev->platform_data;
    uint32_t value_out = 0;
    memcpy(&value_out, buf, sizeof(value_out));
#if PIKA_DEBUG_ENABLE
    __platform_printf("GPIO write port %d to %d\r\n", platform_gpio->port,
                      value_out);
#endif
    hosal_gpio_output_set(platform_gpio, value_out);
    return 0;
}

int pika_hal_platform_GPIO_ioctl_enable(pika_dev* dev) {
#if PIKA_DEBUG_ENABLE
    __platform_printf("in pika_hal_platform_GPIO_ioctl_enable\r\n");
#endif
    hosal_gpio_dev_t* platform_gpio = (hosal_gpio_dev_t*)dev->platform_data;
#if PIKA_DEBUG_ENABLE
    __platform_printf("GPIO enable port %d, config:%d\r\n", platform_gpio->port,
                      platform_gpio->config);
#endif
    hosal_gpio_init(platform_gpio);
    return 0;
}

int pika_hal_platform_GPIO_ioctl_disable(pika_dev* dev) {
    hosal_gpio_dev_t* platform_gpio = (hosal_gpio_dev_t*)dev->platform_data;
    hosal_gpio_finalize(platform_gpio);
    return 0;
}

extern volatile _BLIOT_irq g_irq;
void _hosal_gpio_irq_handler(void* arg) {
    if (g_irq.gpio_irq_trigger) {
        return;
    }
    g_irq.gpio_irq_arg = arg;
    g_irq.gpio_irq_trigger = 1;
}

int pika_hal_platform_GPIO_ioctl_config(pika_dev* dev,
                                        pika_hal_GPIO_config* cfg) {
    hosal_gpio_dev_t* platform_gpio = (hosal_gpio_dev_t*)dev->platform_data;
    switch (cfg->dir) {
        case PIKA_HAL_GPIO_DIR_OUT:
            // printf("GPIO set port %d to output\r\n", platform_gpio->port);
            switch (cfg->pull) {
                case PIKA_HAL_GPIO_PULL_NONE:
                    platform_gpio->config = OUTPUT_OPEN_DRAIN_NO_PULL;
                    break;
                case PIKA_HAL_GPIO_PULL_UP:
                    platform_gpio->config = OUTPUT_OPEN_DRAIN_PULL_UP;
                    break;
                case PIKA_HAL_GPIO_PULL_DOWN:
                    platform_gpio->config = OUTPUT_PUSH_PULL;
                    break;
                default:
                    break;
            }
            break;
        case PIKA_HAL_GPIO_DIR_IN:
#if PIKA_DEBUG_ENABLE
            __platform_printf("GPIO set port %d to input\r\n",
                              platform_gpio->port);
#endif
            switch (cfg->pull) {
                case PIKA_HAL_GPIO_PULL_NONE:
                    platform_gpio->config = INPUT_HIGH_IMPEDANCE;
                    break;

                case PIKA_HAL_GPIO_PULL_UP:
                    platform_gpio->config = INPUT_PULL_UP;
                    break;

                case PIKA_HAL_GPIO_PULL_DOWN:
                    /* not supported */
                    return -1;
                default:
                    return -1;
            }
            break;
        default:
#if PIKA_DEBUG_ENABLE
            __platform_printf("GPIO set port %d to unknown\r\n",
                              platform_gpio->port);
#endif
            return -1;
    }

    /* support event callback */
    if (NULL != cfg->event_callback &&
        PIKA_HAL_EVENT_CALLBACK_ENA_ENABLE == cfg->event_callback_ena) {
        switch (cfg->event_callback_filter) {
            case PIKA_HAL_GPIO_EVENT_SIGNAL_RISING:
                hosal_gpio_irq_set(dev->platform_data, HOSAL_IRQ_TRIG_POS_PULSE,
                                   _hosal_gpio_irq_handler, dev);
                break;
            case PIKA_HAL_GPIO_EVENT_SIGNAL_FALLING:
                hosal_gpio_irq_set(dev->platform_data, HOSAL_IRQ_TRIG_NEG_PULSE,
                                   _hosal_gpio_irq_handler, dev);
                break;

            default:
                __platform_printf(
                    "Error: not supported event callback filter %d\r\n",
                    cfg->event_callback_filter);
                return -1;
        }
        /* start irq task thread */
        _BLIOT_irq_task_start();
    }
    return 0;
}
