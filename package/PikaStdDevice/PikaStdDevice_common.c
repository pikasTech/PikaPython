#include "PikaStdDevice_common.h"

extern PikaEventListener* g_pika_device_event_listener;
void _PikaStdDevice_GPIO_event_handler(pika_dev* dev,
                                       PIKA_HAL_GPIO_EVENT_SIGNAL signal) {
    pika_eventListener_sendSignal(g_pika_device_event_listener, (uintptr_t)dev,
                                  signal);
}

void _PikaStdDevice_TIM_event_handler(pika_dev* dev,
                                      PIKA_HAL_TIM_EVENT_SIGNAL signal) {
    pika_eventListener_sendSignal(g_pika_device_event_listener, (uintptr_t)dev,
                                  signal);
}

void _PikaStdDevice_UART_event_handler(pika_dev* dev,
                                       PIKA_HAL_UART_EVENT_SIGNAL signal) {
    pika_eventListener_sendSignal(g_pika_device_event_listener, (uintptr_t)dev,
                                  signal);
}

void _PikaStdDevice_setCallback(PikaObj* self,
                                Arg* eventCallback,
                                uintptr_t eventId) {
    if (NULL == g_pika_device_event_listener) {
        pika_eventListener_init(&g_pika_device_event_listener);
    }
    pika_eventListener_registEventCallback(g_pika_device_event_listener,
                                           eventId, eventCallback);
}

extern volatile PikaObj* __pikaMain;
PikaObj* PikaStdDevice_Time(PikaObj* self) {
    PikaObj* time = obj_getPtr((PikaObj*)__pikaMain, "time");
    if (NULL == time) {
        obj_setErrorCode(self, -1);
        obj_setSysOut(self, "Error: please install and import 'time' module");
        return NULL;
    }
    obj_refcntInc(time);
    return time;
}
