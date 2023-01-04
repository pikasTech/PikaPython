#include "../PikaStdDevice/pika_hal.h"

typedef struct {
    uint8_t task_created;
    uint8_t gpio_irq_trigger;
    void* gpio_irq_arg;

    uint8_t uart_irq_trigger;
    void* uart_irq_arg;
} _BLIOT_irq;

int _BLIOT_irq_task_start();
