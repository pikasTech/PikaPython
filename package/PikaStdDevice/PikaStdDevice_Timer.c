#include "PikaStdDevice_Timer.h"
#include "pika_hal.h"
#include "PikaStdDevice_common.h"

extern PikaEventListener* g_pika_device_event_listener;

static pika_dev* _get_dev(PikaObj* self) {
    pika_dev* dev = obj_getPtr(self, "pika_dev");
    if (NULL != dev) {
        return dev;
    }
    char* name = obj_getStr(self, "name");
    if (!strEqu(name, "none")) {
        dev = pika_hal_open(PIKA_HAL_TIM, name);
        if (NULL == dev) {
            __platform_printf("Error: open TIM '%s' failed.\r\n", name);
        }
        obj_setPtr(self, "pika_dev", dev);
        return dev;
    }
    int id = obj_getInt(self, "id");
    if (id != -1) {
        char id_str[32] = {0};
        sprintf(id_str, "TIM%d", id);
        dev = pika_hal_open(PIKA_HAL_TIM, id_str);
        if (NULL == dev) {
            __platform_printf("Error: open TIM '%s' failed.\r\n", id_str);
        }
        obj_setPtr(self, "pika_dev", dev);
        return dev;
    } else {
        /* id == -1 means a SOFT timer */
        dev = pika_hal_open(PIKA_HAL_SOFT_TIM, "SOFT_TIM");
        if (NULL == dev) {
            __platform_printf("Error: open SOFT_TIM failed.\r\n");
        }
        obj_setPtr(self, "pika_dev", dev);
        return dev;
    }
}

void PikaStdDevice_Timer___init__(PikaObj* self) {
    obj_setInt(self, "id", -1);
    obj_setStr(self, "name", "none");
    obj_setInt(self, "period", PIKA_HAL_TIM_PERIOD_1S);
    obj_setInt(self, "SIGNAL_TIMEOUT", PIKA_HAL_TIM_EVENT_SIGNAL_TIMEOUT);
    obj_setInt(self, "mode", PIKA_HAL_TIM_MODE_CONTINUOUS);
    obj_setInt(self, "SIGNAL_ANY", PIKA_HAL_TIM_EVENT_SIGNAL_ANY);
}

void PikaStdDevice_Timer_disable(PikaObj* self) {
    pika_dev* dev = _get_dev(self);
    if (NULL == dev) {
        __platform_printf("Error: open TIM '%d' failed.\r\n",
                          (int)obj_getInt(self, "id"));
        return;
    }
    pika_hal_ioctl(dev, PIKA_HAL_IOCTL_DISABLE);
}

void PikaStdDevice_Timer_enable(PikaObj* self) {
    pika_dev* dev = _get_dev(self);
    if (NULL == dev) {
        __platform_printf("Error: open TIM '%d' failed.\r\n",
                          (int)obj_getInt(self, "id"));
        return;
    }
    pika_hal_TIM_config cfg = {0};
    cfg.period = obj_getInt(self, "period");
    if (NULL != obj_getArg(self, "callback")) {
        PikaStdDevice_Timer_setCallback(self, obj_getArg(self, "callback"),
                                        obj_getInt(self, "callback_filter"));
    }
    int err = -1;
    err = pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, &cfg);
    if (err != 0) {
        __platform_printf("Error: config TIM '%d' failed.\r\n",
                          (int)obj_getInt(self, "id"));
        return;
    }
    err = pika_hal_ioctl(dev, PIKA_HAL_IOCTL_ENABLE);
    if (err != 0) {
        __platform_printf("Error: enable TIM '%d' failed.\r\n",
                          (int)obj_getInt(self, "id"));
        return;
    }
}

void PikaStdDevice_Timer_setCallback(PikaObj* self, Arg* callback, int filter) {
#if PIKA_EVENT_ENABLE
    if (NULL != obj_getPtr(self, "pika_dev")) {
        pika_dev* dev = _get_dev(self);
        _PikaStdDevice_setCallback(self, callback, (uintptr_t)dev);
        /* regist event to pika_hal */
        pika_hal_TIM_config cfg_cb = {0};
        cfg_cb.event_callback = _PikaStdDevice_TIM_event_handler;
        cfg_cb.event_callback_filter = filter;
        cfg_cb.event_callback_ena = PIKA_HAL_EVENT_CALLBACK_ENA_ENABLE;
        pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, &cfg_cb);
    } else {
        obj_setArg(self, "callback", callback);
        obj_setInt(self, "callback_filter", filter);
    }
#else
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] PIKA_EVENT_ENABLE is disabled.");
#endif
}

void PikaStdDevice_Timer_setPeriod(PikaObj* self, int period_ms) {
    obj_setInt(self, "period", period_ms * PIKA_HAL_TIM_PERIOD_1MS);
    pika_hal_TIM_config cfg = {0};
    cfg.period = obj_getInt(self, "period");
    pika_hal_ioctl(_get_dev(self), PIKA_HAL_IOCTL_CONFIG, &cfg);
}

void PikaStdDevice_Timer_close(PikaObj* self) {
    pika_hal_close(_get_dev(self));
    obj_setPtr(self, "pika_dev", NULL);
}

void PikaStdDevice_Timer_setId(PikaObj* self, int id) {
    obj_setInt(self, "id", id);
}

void PikaStdDevice_Timer_setName(PikaObj* self, char* name) {
    obj_setStr(self, "name", name);
}

int PikaStdDevice_Timer_getId(PikaObj* self) {
    return obj_getInt(self, "id");
}

char* PikaStdDevice_Timer_getName(PikaObj* self) {
    return obj_getStr(self, "name");
}

void PikaStdDevice_Timer_setMode(PikaObj* self, char* mode) {
    if (strEqu(mode, "oneshot")) {
        obj_setInt(self, "mode", PIKA_HAL_TIM_MODE_ONESHOT);
    } else if (strEqu(mode, "continuous")) {
        obj_setInt(self, "mode", PIKA_HAL_TIM_MODE_CONTINUOUS);
    } else {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] mode is not supported.");
    }
    pika_dev* dev = _get_dev(self);
    pika_hal_TIM_config cfg = {0};
    cfg.mode = obj_getInt(self, "mode");
    pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, &cfg);
}
