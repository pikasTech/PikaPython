#include "../../../libraries/ls1c_delay.h"
#include "../../../libraries/ls1c_gpio.h"
#include "../../../libraries/ls1c_public.h"
#include "PikaObj.h"

void SmartLoong_KEY___init__(PikaObj* self) {
    obj_setInt(self, "io_number", 85);
}

void SmartLoong_KEY_enable(PikaObj* self) {
    int led_gpio = obj_getInt(self, "io_number");
    gpio_init(led_gpio, gpio_mode_input);
}

int SmartLoong_KEY_read(PikaObj* self) {
    int led_gpio = obj_getInt(self, "io_number");
    return gpio_get(led_gpio);
}

void SmartLoong_KEY_setId(PikaObj* self, int id) {
    if (1 == id) {
        obj_setInt(self, "io_number", 85);
    }
}