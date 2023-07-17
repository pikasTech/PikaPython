#include <stdint.h>
#include "BaseObj.h"
#include "dataStrs.h"
#include "driver/uart.h"
#include "pika_hal.h"
#include "pika_hal_ESP32_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern volatile PikaMemInfo g_PikaMemInfo;
typedef struct platform_data_UART {
    uart_port_t uartPort;
    uart_config_t uart_conf;
    QueueHandle_t uart_queue;
    gpio_num_t tx_port;
    gpio_num_t rx_port;
    gpio_num_t rts_port;
    gpio_num_t cts_port;
    PIKA_BOOL event_thread_started;
} platform_data_UART;

#define EVENT_TRHEAD_STACK_SIZE 32 * 1024

static SemaphoreHandle_t g_event_lock = NULL;

int pika_hal_platform_UART_open(pika_dev* dev, char* name) {
    /* UARTX */
    if (name[0] != 'U' || name[1] != 'A' || name[2] != 'R' || name[3] != 'T') {
        return -1;
    }
    platform_data_UART* uart = pikaMalloc(sizeof(platform_data_UART));
    memset(uart, 0, sizeof(platform_data_UART));

    uart->uartPort = fast_atoi(name + 4);
    if (uart->uartPort < 0 || uart->uartPort >= UART_NUM_MAX) {
        pikaFree(uart, sizeof(platform_data_UART));
        return -1;
    }
    dev->platform_data = uart;
    return 0;
}

int pika_hal_platform_UART_close(pika_dev* dev) {
    platform_data_UART* uart = (platform_data_UART*)dev->platform_data;
    if (NULL == uart) {
        return -1;
    }
    pikaFree(uart, sizeof(platform_data_UART));
    dev->platform_data = NULL;
    return 0;
}

int pika_hal_platform_UART_read(pika_dev* dev, void* buf, size_t count) {
    platform_data_UART* uart = (platform_data_UART*)dev->platform_data;
    if (NULL == uart) {
        return -1;
    }
    return uart_read_bytes(uart->uartPort, buf, count, 100);
}

int pika_hal_platform_UART_write(pika_dev* dev, void* buf, size_t count) {
    platform_data_UART* uart = (platform_data_UART*)dev->platform_data;
    if (NULL == uart) {
        return -1;
    }
    return uart_write_bytes(uart->uartPort, buf, count);
}

static void uart_event_task(void* pvParameters) {
    pika_debug("uart_event_task start: pvParameters: %p", pvParameters);
    pika_dev* dev = (pika_dev*)pvParameters;
    platform_data_UART* uart = (platform_data_UART*)dev->platform_data;
    pika_hal_UART_config* cfg = (pika_hal_UART_config*)dev->ioctl_config;
    if (NULL == uart) {
        pika_platform_printf("Error: uart_event_task: uart is NULL\r\n");
        vTaskDelete(NULL);
        return;
    }
    uart_event_t event;
    for (;;) {
        // Waiting for UART event.
        if (xQueueReceive(uart->uart_queue, (void*)&event,
                          (TickType_t)portMAX_DELAY)) {
            pika_debug("UART%d: event.type: %d", uart->uartPort,
                       event.type);
            switch (event.type) {
                // Event of UART receving data
                /*We'd better handler data event fast, there would be much more
                data events than other types of events. If we take too much time
                on data event, the queue might be full.*/
                case UART_DATA:
                    pika_assert(g_event_lock != NULL);
                    xSemaphoreTake(g_event_lock, portMAX_DELAY);
                    cfg->event_callback(dev, event.type);
                    xSemaphoreGive(g_event_lock);
                    break;
                // Others
                default:
                    pika_platform_printf("[INFO] uart event type: %d\r\n", event.type);
                    break;
            }
        }
    }
    g_PikaMemInfo.heapUsed -= EVENT_TRHEAD_STACK_SIZE;
    vTaskDelete(NULL);
}

int pika_hal_platform_UART_ioctl_enable(pika_dev* dev) {
    platform_data_UART* uart = (platform_data_UART*)dev->platform_data;
    pika_debug("UART%d_enable: platform_data: %p", uart->uartPort, uart);
    pika_hal_UART_config* cfg = (pika_hal_UART_config*)dev->ioctl_config;
    if (NULL == uart || NULL == cfg) {
        return -1;
    }
    uart_driver_install(uart->uartPort, 1024, 1024, 20, &uart->uart_queue, 0);
    pika_debug(
        "UART%d: baudrate:%d, data_bits:%d, parity:%d, stop_bits: %d",
        uart->uartPort, uart->uart_conf.baud_rate, uart->uart_conf.data_bits,
        uart->uart_conf.parity, uart->uart_conf.stop_bits);
    uart_param_config(uart->uartPort, &uart->uart_conf);
    pika_debug("UART%d: tx:%d, rx:%d, rts:%d, cts:%d", uart->uartPort,
               uart->tx_port, uart->rx_port, uart->rts_port, uart->cts_port);
    uart_set_pin(uart->uartPort, uart->tx_port, uart->rx_port, uart->rts_port,
                 uart->cts_port);
    return 0;
}

int pika_hal_platform_UART_ioctl_disable(pika_dev* dev) {
    platform_data_UART* uart = (platform_data_UART*)dev->platform_data;
    if (NULL == uart) {
        return -1;
    }
    uart_driver_delete(uart->uartPort);
    return 0;
}

int pika_hal_platform_UART_ioctl_config(pika_dev* dev,
                                        pika_hal_UART_config* cfg) {
    platform_data_UART* uart = (platform_data_UART*)dev->platform_data;
    pika_debug("UART%d_config: platform_data: %p", uart->uartPort, uart);
    if (NULL == uart || NULL == cfg) {
        pika_platform_printf("Error: uart config error, uart:%p, cfg:%p\r\n",
                             uart, cfg);
        return -1;
    }
    uart->uart_conf.baud_rate = cfg->baudrate;
    pika_debug("UART%d: set baudrate to %d", uart->uartPort,
               uart->uart_conf.baud_rate);
    switch (cfg->data_bits) {
        case PIKA_HAL_UART_DATA_BITS_5:
            uart->uart_conf.data_bits = UART_DATA_5_BITS;
            break;
        case PIKA_HAL_UART_DATA_BITS_6:
            uart->uart_conf.data_bits = UART_DATA_6_BITS;
            break;
        case PIKA_HAL_UART_DATA_BITS_7:
            uart->uart_conf.data_bits = UART_DATA_7_BITS;
            break;
        case PIKA_HAL_UART_DATA_BITS_8:
            uart->uart_conf.data_bits = UART_DATA_8_BITS;
            break;
        default:
            uart->uart_conf.data_bits = UART_DATA_8_BITS;
            break;
    }
    pika_debug("UART%d: set data_bites to %d", uart->uartPort,
               uart->uart_conf.data_bits);
    switch (cfg->parity) {
        case PIKA_HAL_UART_PARITY_NONE:
            uart->uart_conf.parity = UART_PARITY_DISABLE;
            break;
        case PIKA_HAL_UART_PARITY_ODD:
            uart->uart_conf.parity = UART_PARITY_ODD;
            break;
        case PIKA_HAL_UART_PARITY_EVEN:
            uart->uart_conf.parity = UART_PARITY_EVEN;
            break;
        default:
            uart->uart_conf.parity = UART_PARITY_DISABLE;
            break;
    }
    pika_debug("UART%d: set parity to %d", uart->uartPort,
               uart->uart_conf.parity);
    switch (cfg->stop_bits) {
        case PIKA_HAL_UART_STOP_BITS_1:
            uart->uart_conf.stop_bits = UART_STOP_BITS_1;
            break;
        case PIKA_HAL_UART_STOP_BITS_1_5:
            uart->uart_conf.stop_bits = UART_STOP_BITS_1_5;
            break;
        case PIKA_HAL_UART_STOP_BITS_2:
            uart->uart_conf.stop_bits = UART_STOP_BITS_2;
            break;
        default:
            uart->uart_conf.stop_bits = UART_STOP_BITS_1;
            break;
    }
    pika_debug("UART%d: set stop_bits to %d", uart->uartPort,
               uart->uart_conf.stop_bits);
    switch (cfg->flow_control) {
        case PIKA_HAL_UART_FLOW_CONTROL_NONE:
            uart->uart_conf.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
            break;
        case PIKA_HAL_UART_FLOW_CONTROL_RTS:
            uart->uart_conf.flow_ctrl = UART_HW_FLOWCTRL_RTS;
            break;
        case PIKA_HAL_UART_FLOW_CONTROL_CTS:
            uart->uart_conf.flow_ctrl = UART_HW_FLOWCTRL_CTS;
            break;
        case PIKA_HAL_UART_FLOW_CONTROL_RTS_CTS:
            uart->uart_conf.flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS;
            break;
        default:
            uart->uart_conf.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
            break;
    }
    uart->uart_conf.source_clk = UART_SCLK_DEFAULT;
    pika_debug("UART%d: set flow_control to %d", uart->uartPort,
               uart->uart_conf.flow_ctrl);

    if (NULL == cfg->TX || NULL == cfg->RX) {
        pika_platform_printf("Error: uart config error, TX:%p, RX:%p\r\n",
                             cfg->TX, cfg->RX);
        return -1;
    }
    uart->tx_port = ((platform_data_GPIO*)(cfg->TX->platform_data))->gpioPort;
    uart->rx_port = ((platform_data_GPIO*)(cfg->RX->platform_data))->gpioPort;
    if (NULL != cfg->RTS && NULL != cfg->CTS) {
        uart->rts_port =
            ((platform_data_GPIO*)(cfg->RTS->platform_data))->gpioPort;
        uart->cts_port =
            ((platform_data_GPIO*)(cfg->CTS->platform_data))->gpioPort;
    } else {
        uart->rts_port = UART_PIN_NO_CHANGE;
        uart->cts_port = UART_PIN_NO_CHANGE;
    }

    if (dev->is_enabled){
        pika_debug("UART%d: uart is enabled, reconfig", uart->uartPort);
        uart_param_config(uart->uartPort, &uart->uart_conf);
    }

    /* support event callback */
    if (dev->is_enabled == PIKA_TRUE && NULL != cfg->event_callback &&
        PIKA_HAL_EVENT_CALLBACK_ENA_ENABLE == cfg->event_callback_ena) {
        // hosal_uart_ioctl(platform_uart, HOSAL_UART_MODE_SET,
        //                  (void*)HOSAL_UART_MODE_INT);
        switch (cfg->event_callback_filter) {
            /* Configure UART to interrupt mode */
            case PIKA_HAL_UART_EVENT_SIGNAL_RX:
                pika_debug("Setting UART_RX callback");
                uart_enable_rx_intr(uart->uartPort);
                break;
            case PIKA_HAL_UART_EVENT_SIGNAL_TX:
                pika_debug("Setting UART_TX callback");
                uart_enable_tx_intr(uart->uartPort, 1, 0);
                break;
            default:
                __platform_printf(
                    "Error: not supported event callback filter %d\r\n",
                    cfg->event_callback_filter);
                return -1;
        }
        if (uart->event_thread_started == PIKA_FALSE){
            /* start irq task thread */
            pika_debug("Starting uart event task:%p", dev);
            if (NULL == g_event_lock){
                g_event_lock = xSemaphoreCreateMutex();
            }
            g_PikaMemInfo.heapUsed += EVENT_TRHEAD_STACK_SIZE;
            xTaskCreate(uart_event_task, "uart_event_task", EVENT_TRHEAD_STACK_SIZE, dev, 12, NULL);
            uart->event_thread_started = PIKA_TRUE;
        }
    }
    return 0;
}
