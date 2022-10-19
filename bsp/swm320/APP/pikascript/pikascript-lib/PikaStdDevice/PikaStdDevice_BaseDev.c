#include "PikaStdDevice_BaseDev.h"
#include "BaseObj.h"
#include "PikaObj.h"
#include "dataStrs.h"

#if !(PIKASCRIPT_VERSION_NUM >= PIKASCRIPT_VERSION_TO_NUM(1, 10, 4))
#error "require pikascript kernal version >= v1.10.4"
#endif

PikaEventListener* g_pika_device_event_listener;

void PikaStdDevice_BaseDev_addEventCallBack(PikaObj* self, Arg* eventCallBack) {
#if PIKA_EVENT_ENABLE
    obj_setArg(self, "eventCallBack", eventCallBack);
    /* init event_listener for the first time */
    if (NULL == g_pika_device_event_listener) {
        pks_eventLisener_init(&g_pika_device_event_listener);
    }
    if (PIKA_RES_OK != obj_runNativeMethod(self, "platformGetEventId", NULL)) {
        obj_setErrorCode(self, 1);
        __platform_printf("Error: Method %s no found.\r\n",
                          "platformGetEventId");
    }
    uint32_t eventId = obj_getInt(self, "eventId");
    pks_eventLicener_registEvent(g_pika_device_event_listener, eventId, self);
#else
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] PIKA_EVENT_ENABLE is disabled.");
#endif
}

void PikaStdDevice_BaseDev_platformGetEventId(PikaObj* self) {
    ABSTRACT_METHOD_NEED_OVERRIDE_ERROR();
}
