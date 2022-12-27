#ifndef _PIKA_STDDEVICE_COMMON_H
#define _PIKA_STDDEVICE_COMMON_H
#include "PikaObj.h"
#include "PikaVM.h"
#include "pika_hal.h"
void _PikaStdDevice_setCallBack(PikaObj* self,
                                Arg* eventCallback,
                                uint32_t eventId);

void _PikaStdDevice_event_handler(pika_dev* dev, int signal);

#endif
