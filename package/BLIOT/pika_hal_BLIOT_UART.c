#include <hosal_uart.h>
#include "../PikaStdDevice/pika_hal.h"
#include "pika_hal_BLIOT_irq_task.h"

extern hosal_uart_dev_t uart_stdio;

int pika_hal_platform_UART_open(pika_dev* dev, char* name) {
    /* Support UART1 */
    if (name[0] == 'U' && name[1] == 'A' && name[2] == 'R' && name[3] == 'T') {
        hosal_uart_dev_t* platform_uart = &uart_stdio;
        dev->platform_data = platform_uart;
        int UART_num = fast_atoi(name + 4);
#if PIKA_DEBUG_ENABLE
        __platform_printf("opening UART%d\r\n", UART_num);
#endif
        return 0;
    }
    return -1;
}

int pika_hal_platform_UART_close(pika_dev* dev) {
    hosal_uart_dev_t* platform_uart = (hosal_uart_dev_t*)dev->platform_data;
    if (platform_uart == &uart_stdio) {
        return 0;
    }
    if (NULL != platform_uart) {
        pikaFree(platform_uart, sizeof(hosal_uart_dev_t));
    }
    return 0;
}

extern volatile _BLIOT_irq g_irq;
int _hosal_uart_irq_handler(void* p_arg) {
    if (g_irq.uart_irq_trigger) {
        return 0;
    }
    g_irq.uart_irq_arg = p_arg;
    g_irq.uart_irq_trigger = 1;
    return 0;
}

int pika_hal_platform_UART_ioctl_config(pika_dev* dev,
                                        pika_hal_UART_config* cfg) {
    hosal_uart_dev_t* platform_uart = (hosal_uart_dev_t*)dev->platform_data;
    if (platform_uart != &uart_stdio) {
        platform_uart->config.baud_rate = cfg->baudrate;
        switch (cfg->data_bits) {
            case PIKA_HAL_UART_DATA_BITS_5:
                platform_uart->config.data_width = HOSAL_DATA_WIDTH_5BIT;
                break;
            case PIKA_HAL_UART_DATA_BITS_6:
                platform_uart->config.data_width = HOSAL_DATA_WIDTH_6BIT;
                break;
            case PIKA_HAL_UART_DATA_BITS_7:
                platform_uart->config.data_width = HOSAL_DATA_WIDTH_7BIT;
                break;
            case PIKA_HAL_UART_DATA_BITS_8:
                platform_uart->config.data_width = HOSAL_DATA_WIDTH_8BIT;
                break;
            default:
                platform_uart->config.data_width = HOSAL_DATA_WIDTH_8BIT;
                break;
        }
        switch (cfg->parity) {
            case PIKA_HAL_UART_PARITY_NONE:
                platform_uart->config.parity = HOSAL_NO_PARITY;
                break;
            case PIKA_HAL_UART_PARITY_ODD:
                platform_uart->config.parity = HOSAL_ODD_PARITY;
                break;
            case PIKA_HAL_UART_PARITY_EVEN:
                platform_uart->config.parity = HOSAL_EVEN_PARITY;
                break;
            default:
                platform_uart->config.parity = HOSAL_NO_PARITY;
                break;
        }
        switch (cfg->stop_bits) {
            case PIKA_HAL_UART_STOP_BITS_1:
                platform_uart->config.stop_bits = HOSAL_STOP_BITS_1;
                break;
            case PIKA_HAL_UART_STOP_BITS_2:
                platform_uart->config.stop_bits = HOSAL_STOP_BITS_2;
                break;
#if !defined(BL808)
            case PIKA_HAL_UART_STOP_BITS_1_5:
                platform_uart->config.stop_bits = HOSAL_STOP_BITS_1_5;
                break;
#endif
            default:
                platform_uart->config.stop_bits = HOSAL_STOP_BITS_1;
                break;
        }
    }

    /* support event callback */
    if (NULL != cfg->event_callback &&
        PIKA_HAL_EVENT_CALLBACK_ENA_ENABLE == cfg->event_callback_ena) {
        hosal_uart_ioctl(platform_uart, HOSAL_UART_MODE_SET,
                         (void*)HOSAL_UART_MODE_INT);
        switch (cfg->event_callback_filter) {
            /* Configure UART to interrupt mode */
            case PIKA_HAL_UART_EVENT_SIGNAL_RX:
#if PIKA_DEBUG_ENABLE
                __platform_printf("Setting UART_RX callback\r\n");
#endif
                hosal_uart_callback_set(platform_uart, HOSAL_UART_RX_CALLBACK,
                                        _hosal_uart_irq_handler, dev);
                break;
            case PIKA_HAL_UART_EVENT_SIGNAL_TX:
#if PIKA_DEBUG_ENABLE
                __platform_printf("Setting UART_TX callback\r\n");
#endif
                hosal_uart_callback_set(platform_uart, HOSAL_UART_TX_CALLBACK,
                                        _hosal_uart_irq_handler, dev);
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
#if PIKA_DEBUG_ENABLE
#endif
    return 0;
}

int pika_hal_platform_UART_ioctl_enable(pika_dev* dev) {
    if (!dev->is_enabled) {
        hosal_uart_dev_t* platform_uart = (hosal_uart_dev_t*)dev->platform_data;
        if (platform_uart == &uart_stdio) {
            return 0;
        }
        hosal_uart_init(platform_uart);
        return 0;
    }
    return -1;
}

int pika_hal_platform_UART_ioctl_disable(pika_dev* dev) {
    if (dev->is_enabled) {
        hosal_uart_dev_t* platform_uart = (hosal_uart_dev_t*)dev->platform_data;
        if (platform_uart == &uart_stdio) {
            return 0;
        }
        hosal_uart_finalize(platform_uart);
        return 0;
    }
    return -1;
}

int pika_hal_platform_UART_write(pika_dev* dev, void* buf, size_t count) {
    hosal_uart_dev_t* platform_uart = (hosal_uart_dev_t*)dev->platform_data;
    return hosal_uart_send(platform_uart, buf, count);
}

int pika_hal_platform_UART_read(pika_dev* dev, void* buf, size_t count) {
    hosal_uart_dev_t* platform_uart = (hosal_uart_dev_t*)dev->platform_data;
    return hosal_uart_receive(platform_uart, buf, count);
}
