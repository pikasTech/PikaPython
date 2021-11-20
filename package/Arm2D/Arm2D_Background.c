#include "Arm2D_common.h"

void Arm2D_BackGround_init(PikaObj* self) {
    args_setStr(self->attributeList, "color", "white");
}

char* Arm2D_BackGround_getColor(PikaObj* self) {
    return args_getStr(self->attributeList, "color");
}

void Arm2D_BackGround_update(PikaObj* self) {
    void* target_tile = pika_arm2d_window.pfb_tile_now;

    char* color = args_getStr(self->attributeList, "color");
    uint16_t backGroundColor = getColorCode(color);
    arm_2d_rgb16_fill_colour(target_tile, NULL, backGroundColor);
}

void Arm2D_BackGround_setColor(PikaObj* self, char* color) {
    args_setStr(self->attributeList, "color", color);
}
