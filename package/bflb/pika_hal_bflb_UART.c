#include "bflb_uart.h"
#include "bflb_gpio.h"
#include "pika_hal_bflb_common.h"

typedef struct platform_uart_t {
    struct bflb_device_s* device;
    struct bflb_uart_config_s config;
    int uart_id;
} platform_uart_t;

int pika_hal_platform_UART_open(pika_dev* dev, char* name) {
    if (name[0] == 'U' && name[1] == 'A' && name[2] == 'R' && name[3] == 'T') {
        int UART_num = atoi(name + 4);
        platform_uart_t* uart = pikaMalloc(sizeof(platform_uart_t));
        char uart_name[6] = {0};
        snprintf(uart_name, sizeof(uart_name), "uart%d", UART_num);
        uart->device = bflb_device_get_by_name(uart_name);
        uart->uart_id = UART_num;
        dev->platform_data = uart;
        return 0;
    }
    return -1;
}

int pika_hal_platform_UART_close(pika_dev* dev) {
    pikaFree(dev->platform_data, sizeof(platform_uart_t));
    return 0;
}

static void _uart_irq_handler(int irq, void* arg) {
    pika_dev* dev = (pika_dev*)arg;
    pika_debug("uart_irq_handler:%p", dev);
    platform_uart_t* uart = (platform_uart_t*)dev->platform_data;
    pika_hal_UART_config* cfg = (pika_hal_UART_config*)dev->ioctl_config;
    uint32_t intstatus = bflb_uart_get_intstatus(uart->device);
    if (intstatus) {
        pika_debug("uart%d intstatus:0x%02x", uart->uart_id, intstatus);
        pika_debug("event_callback:%p", cfg->event_callback);
        pika_debug("event_callback_filter:%d",
                   cfg->event_callback_filter);
        cfg->event_callback(dev, cfg->event_callback_filter);
        bflb_uart_int_clear(uart->device, intstatus);
    }
}

int pika_hal_platform_UART_ioctl_config(pika_dev* dev,
                                        pika_hal_UART_config* cfg) {
    platform_uart_t* uart = (platform_uart_t*)dev->platform_data;
    uart->config.baudrate = cfg->baudrate;
    uart->config.tx_fifo_threshold = 7;
    uart->config.rx_fifo_threshold = 7;
    switch (cfg->data_bits) {
        case PIKA_HAL_UART_DATA_BITS_5:
            uart->config.data_bits = UART_DATA_BITS_5;
            break;
        case PIKA_HAL_UART_DATA_BITS_6:
            uart->config.data_bits = UART_DATA_BITS_6;
            break;
        case PIKA_HAL_UART_DATA_BITS_7:
            uart->config.data_bits = UART_DATA_BITS_7;
            break;
        case PIKA_HAL_UART_DATA_BITS_8:
            uart->config.data_bits = UART_DATA_BITS_8;
            break;
        default:
            uart->config.data_bits = UART_DATA_BITS_8;
            break;
    }
    switch (cfg->parity) {
        case PIKA_HAL_UART_PARITY_NONE:
            uart->config.parity = UART_PARITY_NONE;
            break;
        case PIKA_HAL_UART_PARITY_ODD:
            uart->config.parity = UART_PARITY_ODD;
            break;
        case PIKA_HAL_UART_PARITY_EVEN:
            uart->config.parity = UART_PARITY_EVEN;
            break;
        default:
            uart->config.parity = UART_PARITY_NONE;
            break;
    }
    switch (cfg->stop_bits) {
        case PIKA_HAL_UART_STOP_BITS_1:
            uart->config.stop_bits = UART_STOP_BITS_1;
            break;
        case PIKA_HAL_UART_STOP_BITS_2:
            uart->config.stop_bits = UART_STOP_BITS_2;
            break;
        case PIKA_HAL_UART_STOP_BITS_1_5:
            uart->config.stop_bits = UART_STOP_BITS_1_5;
            break;
        default:
            uart->config.stop_bits = UART_STOP_BITS_1;
            break;
    }

    switch (cfg->flow_control) {
        case PIKA_HAL_UART_FLOW_CONTROL_NONE:
            uart->config.flow_ctrl = UART_FLOWCTRL_NONE;
            break;
        case PIKA_HAL_UART_FLOW_CONTROL_CTS:
            uart->config.flow_ctrl = UART_FLOWCTRL_CTS;
            break;
        case PIKA_HAL_UART_FLOW_CONTROL_RTS:
            uart->config.flow_ctrl = UART_FLOWCTRL_RTS;
            break;
        case PIKA_HAL_UART_FLOW_CONTROL_RTS_CTS:
            uart->config.flow_ctrl = UART_FLOWCTRL_RTS_CTS;
            break;
        default:
            uart->config.flow_ctrl = UART_FLOWCTRL_NONE;
            break;
    }

    /* support event callback */
    if (NULL != cfg->event_callback &&
        PIKA_HAL_EVENT_CALLBACK_ENA_ENABLE == cfg->event_callback_ena) {
        switch (cfg->event_callback_filter) {
            /* Configure UART to interrupt mode */
            case PIKA_HAL_UART_EVENT_SIGNAL_RX:
                bflb_uart_rxint_mask(uart->device, false);
                bflb_uart_txint_mask(uart->device, true);
                break;
            case PIKA_HAL_UART_EVENT_SIGNAL_TX:
                bflb_uart_txint_mask(uart->device, false);
                bflb_uart_rxint_mask(uart->device, true);
                break;
            default:
                __platform_printf(
                    "Error: not supported event callback filter %d\r\n",
                    cfg->event_callback_filter);
                return -1;
        }
        pika_debug("irq_attach: %d, %p, %p", uart->device->irq_num,
                   _uart_irq_handler, dev);
        bflb_irq_attach(uart->device->irq_num, _uart_irq_handler, dev);
        bflb_irq_enable(uart->device->irq_num);
    }
    return 0;
}

int pika_hal_platform_UART_ioctl_enable(pika_dev* dev) {
    if (!dev->is_enabled) {
        platform_uart_t* uart = (platform_uart_t*)dev->platform_data;
        pika_hal_UART_config* cfg = dev->ioctl_config;
        uint8_t tx_pin = ((platform_gpio_t*)(cfg->TX->platform_data))->pin;
        uint8_t rx_pin = ((platform_gpio_t*)(cfg->RX->platform_data))->pin;
        uint8_t uart_fun_rx = 0;
        uint8_t uart_fun_tx = 0;
        if (uart->uart_id == 0){
            uart_fun_rx = GPIO_UART_FUNC_UART0_RX;
            uart_fun_tx = GPIO_UART_FUNC_UART0_TX;
        }else if (uart->uart_id == 1){
            uart_fun_rx = GPIO_UART_FUNC_UART1_RX;
            uart_fun_tx = GPIO_UART_FUNC_UART1_TX;
        }
        struct bflb_device_s *gpio;
        gpio = bflb_device_get_by_name("gpio");
        bflb_gpio_uart_init(gpio, tx_pin, uart_fun_tx);
        bflb_gpio_uart_init(gpio, rx_pin, uart_fun_rx);
        pika_debug("uart->device->uart_id=%d",uart->uart_id);
        pika_debug("uart->pin_tx=%d",tx_pin);
        pika_debug("uart->pin_rx=%d",rx_pin);
        pika_debug("uart->uart_fun_rx=%d",uart_fun_rx);
        pika_debug("uart->uart_fun_tx=%d",uart_fun_tx);

        pika_debug("uart->config.baudrate=%d",uart->config.baudrate);
        pika_debug("uart->config.data_bits=%d",uart->config.data_bits);
        pika_debug("uart->config.stop_bits=%d",uart->config.stop_bits);
        pika_debug("uart->config.parity=%d",uart->config.parity);
        pika_debug("uart->config.flow_ctrl=%d",uart->config.flow_ctrl);
        pika_debug("uart->config.tx_fifo_threshold=%d",uart->config.tx_fifo_threshold);
        pika_debug("uart->config.rx_fifo_threshold=%d",uart->config.rx_fifo_threshold);
        bflb_uart_init(uart->device, &uart->config);
        return 0;
    }
    return -1;
}

int pika_hal_platform_UART_ioctl_disable(pika_dev* dev) {
    if (dev->is_enabled) {
        platform_uart_t* uart = (platform_uart_t*)dev->platform_data;
        bflb_uart_deinit(uart->device);
        return 0;
    }
    return -1;
}

int pika_hal_platform_UART_write(pika_dev* dev, void* buf, size_t count) {
    platform_uart_t* uart = (platform_uart_t*)dev->platform_data;
    pika_debug("uart->device->uart_id=%d",uart->uart_id);
    pika_debug("uart->count=%d",count);
    pika_debug("uart->write:%s",buf);
    return bflb_uart_put(uart->device, buf, count);
}

int pika_hal_platform_UART_read(pika_dev* dev, void* buf, size_t count) {
    platform_uart_t* uart = (platform_uart_t*)dev->platform_data;
    int ret = bflb_uart_get(uart->device, buf, count);
    pika_debug("uart->device->uart_id=%d",uart->uart_id);
    pika_debug("uart->read:%s", buf);
    pika_debug("uart->count=%d",count);
    return ret;
}
