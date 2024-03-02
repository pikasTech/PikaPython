#ifndef _PIKA_STDDEVICE_COMMON_H
#define _PIKA_STDDEVICE_COMMON_H
#include "PikaObj.h"
#include "PikaVM.h"
#include "pika_hal.h"
void _PikaStdDevice_setCallback(PikaObj* self,
                                Arg* eventCallback,
                                uintptr_t eventId);

void _PikaStdDevice_GPIO_event_handler(pika_dev* dev,
                                       PIKA_HAL_GPIO_EVENT_SIGNAL signal);
void _PikaStdDevice_TIM_event_handler(pika_dev* dev,
                                      PIKA_HAL_TIM_EVENT_SIGNAL signal);
void _PikaStdDevice_UART_event_handler(pika_dev* dev,
                                       PIKA_HAL_UART_EVENT_SIGNAL signal);

#endif
