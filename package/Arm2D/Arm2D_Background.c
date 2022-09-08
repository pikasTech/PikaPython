#include "Arm2D_common.h"

void Arm2D_BackGround___init__(PikaObj* self) {
    obj_setStr(self, "color", "white");
}

int Arm2D_BackGround_getColor(PikaObj* self) {
    return obj_getInt(self, "color");
}

void Arm2D_BackGround_update(PikaObj* self) {
    void* target_tile = pika_arm2d_globals.pfb_tile_now;

    int color = obj_getInt(self, "color");
    uint16_t backGroundColor = color;
    arm_2d_rgb16_fill_colour(target_tile, NULL, backGroundColor);
}

void Arm2D_BackGround_setColor(PikaObj* self, int color) {
    obj_setInt(self, "color", color);
}
