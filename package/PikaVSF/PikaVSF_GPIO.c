#include "pika_config.h"
#include "PikaObj.h"

#if VSF_HAL_USE_GPIO == ENABLED && GPIO_COUNT > 0
#   define __PIKA_VSF_GPIO      ENABLED
#endif

#if __PIKA_VSF_GPIO == ENABLED
static int __vsf_gpio_pin_get(char *pin) {
    char ch_port, ch_pin;
    int hw_port = 0, hw_pin = 0;

    ch_port = *pin++;
    VSF_ASSERT(ch_port == 'P');

    ch_port = *pin++;
    if ((ch_port >= 'A') && (ch_port <= 'Z')) {
        hw_port = ch_port - 'A';
    } else if ((ch_port >= '0') && (ch_port <= '9')) {
        hw_port = ch_port - '0';
    } else {
        VSF_ASSERT(false);
    }

    if (*pin == '.') {
        pin++;
    }

    while ((ch_pin = *pin++) != NULL) {
        if (!((ch_pin >= '0') && (ch_pin <= '9'))) {
            break;
        }
        hw_pin *= 10;
        hw_pin += ch_pin - '0';
    }
    return vsf_hw_io_mapper_pin(hw_port, hw_pin);
}
#endif

void PikaVSF_GPIO_platformEnable(PikaObj *self) {

}
void PikaVSF_GPIO_platformDisable(PikaObj *self) {

}
void PikaVSF_GPIO_platformHigh(PikaObj *self) {
#if __PIKA_VSF_GPIO == ENABLED
    char *pin = obj_getStr(self, "pin");
    int pin_idx = __vsf_gpio_pin_get(pin);
    vsf_io_mapper_set(&vsf_hw_io_mapper, pin_idx);
#else
    vsf_trace_error("%s: platform has no gpio or gpio is disabled" VSF_TRACE_CFG_LINEEND);
#endif
}
void PikaVSF_GPIO_platformLow(PikaObj *self) {
#if __PIKA_VSF_GPIO == ENABLED
    char *pin = obj_getStr(self, "pin");
    int pin_idx = __vsf_gpio_pin_get(pin);
    vsf_io_mapper_clear(&vsf_hw_io_mapper, pin_idx);
#else
    vsf_trace_error("%s: platform has no gpio or gpio is disabled" VSF_TRACE_CFG_LINEEND);
#endif
}
void PikaVSF_GPIO_platformRead(PikaObj *self) {
#if __PIKA_VSF_GPIO == ENABLED
    char *pin = obj_getStr(self, "pin");
    int pin_idx = __vsf_gpio_pin_get(pin);
    int pin_value = vsf_io_mapper_read(&vsf_hw_io_mapper, pin_idx);
    obj_setInt(self, "readBuff", pin_value);
#else
    vsf_trace_error("%s: platform has no gpio or gpio is disabled" VSF_TRACE_CFG_LINEEND);
#endif
}
void PikaVSF_GPIO_platformSetMode(PikaObj *self) {
#if __PIKA_VSF_GPIO == ENABLED
    char *pin = obj_getStr(self, "pin");
    char *mode = obj_getStr(self, "mode");
    int pin_idx = __vsf_gpio_pin_get(pin);

    if (strEqu(mode, "out")) {
        vsf_io_mapper_set_output(&vsf_hw_io_mapper, pin_idx);
    } else if (strEqu(mode, "in")) {
        vsf_io_mapper_set_input(&vsf_hw_io_mapper, pin_idx);
    }
#else
    vsf_trace_error("%s: platform has no gpio or gpio is disabled" VSF_TRACE_CFG_LINEEND);
#endif
}
