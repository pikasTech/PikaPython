#include "TemplateDevice_GPIO.h"
#include "../PikaStdDevice/pika_hal.h"

const uint32_t GPIO_PA8_EVENT_ID = 0x08;
void TemplateDevice_GPIO_platformGetEventId(PikaObj* self) {
    char* pin = obj_getStr(self, "pin");
    if (strEqu(pin, "PA8")) {
        obj_setInt(self, "eventId", GPIO_PA8_EVENT_ID);
    }
}

extern PikaEventListener* g_pika_device_event_listener;
#define EVENT_SIGNAL_IO_RISING_EDGE 0x01
#define EVENT_SIGNAL_IO_FALLING_EDGE 0x02
#define GPIO_PA8_EVENT_ID 0x08

void TemplateDevice_GPIO_eventTest(PikaObj* self) {
    pika_eventListener_sendSignal(g_pika_device_event_listener,
                                 GPIO_PA8_EVENT_ID,
                                 EVENT_SIGNAL_IO_FALLING_EDGE);
    pika_eventListener_sendSignal(g_pika_device_event_listener,
                                 GPIO_PA8_EVENT_ID,
                                 EVENT_SIGNAL_IO_RISING_EDGE);
}

int pika_hal_platform_GPIO_open(pika_dev* dev, char* name) {
    return 0;
}

int pika_hal_platform_GPIO_close(pika_dev* dev) {
    return 0;
}

int pika_hal_platform_GPIO_ioctl_config(pika_dev* dev,
                                        pika_hal_GPIO_config* cfg) {
    return 0;
}

int pika_hal_platform_GPIO_ioctl_enable(pika_dev* dev) {
    return 0;
}

int pika_hal_platform_GPIO_ioctl_disable(pika_dev* dev) {
    return 0;
}
