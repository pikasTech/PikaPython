#include "pika_hal_BLIOT_irq_task.h"
#include <FreeRTOS.h>
#include <task.h>

volatile _BLIOT_irq g_irq = {0};

static void _irq_task(void* pvParameters) {
    while (1) {
        vTaskDelay(10);
        if (g_irq.gpio_irq_trigger) {
            g_irq.gpio_irq_trigger = 0;
            pika_dev* dev = (pika_dev*)g_irq.gpio_irq_arg;
            pika_hal_GPIO_config* cfg = dev->ioctl_config;
            cfg->event_callback(dev, cfg->event_callback_filter);
        }
        if (g_irq.uart_irq_trigger) {
            g_irq.uart_irq_trigger = 0;
            pika_dev* dev = (pika_dev*)g_irq.uart_irq_arg;
            pika_hal_UART_config* cfg = dev->ioctl_config;
            cfg->event_callback(dev, cfg->event_callback_filter);
        }
    }
}

static int _BLIOT_irq_init(volatile _BLIOT_irq* irq) {
    irq->task_created = 0;
    irq->gpio_irq_trigger = 0;
    irq->gpio_irq_arg = NULL;
    return 0;
}

int _BLIOT_irq_task_start() {
    /* already started, skip */
    if (1 == g_irq.task_created) {
        return 0;
    }
    _BLIOT_irq_init(&g_irq);
    g_irq.task_created = 1;
#if PIKA_DEBUG_ENABLE
    __platform_printf("Creating _irq_task\r\n");
#endif
    BaseType_t ret =
        xTaskCreate(_irq_task, (char*)"_irq_task", 4096, NULL, 15, NULL);

#if PIKA_DEBUG_ENABLE
    if (pdPASS == ret) {
        __platform_printf("Create _irq_task succeed.\r\n");
        return 0;
    }
#endif
    return -1;
}
