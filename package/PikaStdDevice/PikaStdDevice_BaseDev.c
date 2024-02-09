#include "PikaStdDevice_BaseDev.h"
#include "PikaStdDevice_common.h"

#if !PIKASCRIPT_VERSION_REQUIRE_MINIMUN(1, 10, 4)
#error "This library requires PikaScript version 1.10.4 or higher"
#endif

PikaEventListener* g_pika_device_event_listener;

void PikaStdDevice_BaseDev_addEventCallback(PikaObj* self, Arg* eventCallBack) {
#if PIKA_EVENT_ENABLE
    /* init event_listener for the first time */
    if (NULL == g_pika_device_event_listener) {
        pika_eventListener_init(&g_pika_device_event_listener);
    }
    if (PIKA_RES_OK != obj_runNativeMethod(self, "platformGetEventId", NULL)) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "Error: Method %s no found.", "platformGetEventId");
    }
    uintptr_t eventId = obj_getInt(self, "eventId");
    pika_eventListener_registEventCallback(g_pika_device_event_listener,
                                           eventId, eventCallBack);
#else
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] PIKA_EVENT_ENABLE is disabled.");
#endif
}

void PikaStdDevice_BaseDev_addEventCallBack(PikaObj* self, Arg* eventCallBack) {
    pika_platform_printf("Warning: Method %s is deprecated, please use %s.\r\n",
                         "addEventCallBack", "addEventCallback");
    PikaStdDevice_BaseDev_addEventCallback(self, eventCallBack);
}

void PikaStdDevice_BaseDev_platformGetEventId(PikaObj* self) {
    ABSTRACT_METHOD_NEED_OVERRIDE_ERROR();
}
