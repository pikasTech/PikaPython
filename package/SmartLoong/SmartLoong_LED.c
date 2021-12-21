#include "../../../libraries/ls1c_delay.h"
#include "../../../libraries/ls1c_gpio.h"
#include "../../../libraries/ls1c_public.h"
#include "PikaObj.h"

void SmartLoong_LED___init__(PikaObj* self) {
    obj_setInt(self, "io_number", 52);
}

void SmartLoong_LED_enable(PikaObj* self) {
    int led_gpio = obj_getInt(self, "io_number");
    gpio_init(led_gpio, gpio_mode_output);
}

void SmartLoong_LED_off(PikaObj* self) {
    int led_gpio = obj_getInt(self, "io_number");
    gpio_set(led_gpio, gpio_level_high);
}

void SmartLoong_LED_on(PikaObj* self) {
    int led_gpio = obj_getInt(self, "io_number");
    gpio_set(led_gpio, gpio_level_low);
}

void SmartLoong_LED_setId(PikaObj* self, int id) {
    if (1 == id) {
        obj_setInt(self, "io_number", 52);
    }
}