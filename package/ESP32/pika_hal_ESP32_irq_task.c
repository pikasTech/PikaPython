#include "pika_hal_ESP32_irq_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

static QueueHandle_t gpio_evt_queue = NULL;

void IRAM_ATTR _ESP32_gpio_isr_handler(void *arg)
{
    pika_dev *dev = (pika_dev *)arg;
    xQueueSendFromISR(gpio_evt_queue, dev, 0);
}

static void _ESP32_gpio_task(void *arg)
{
    pika_dev dev = {0};
    gpio_evt_queue = xQueueCreate(10, sizeof(pika_dev));
    for (;;)
    {
        if (xQueueReceive(gpio_evt_queue, &dev, portMAX_DELAY))
        {
            pika_hal_GPIO_config *cfg = dev.ioctl_config;
            cfg->event_callback(&dev, cfg->event_callback_filter);
            printf("event_callback: %p\n", cfg->event_callback);
            // printf("event_callback_filter: %d\n", cfg->event_callback_filter);
        }
    }
}

int _ESP32_irq_start(void)
{
    static uint8_t task_created = 0;
    if (task_created == 1)
    {
        return 0;
    }
    task_created = 1;

    BaseType_t ret = xTaskCreate(_ESP32_gpio_task, (char *)"_irq_task", 4096, NULL, 12, NULL);

    if (pdPASS != ret)
    {
        return 1;
    }
    return 0;
}