#include "Arm2D_Box.h"
#include "Arm2D_common.h"
#include "asset_corner_box.h"

void Arm2D_Box_init(PikaObj* self) {
    obj_setInt(self, "posX", 0);
    obj_setInt(self, "posY", 0);
    obj_setInt(self, "sizeX", 50);
    obj_setInt(self, "sizeY", 50);
    obj_setInt(self, "alpha", 128);
    obj_setStr(self, "color", "blue");
}

void Arm2D_Box_update(PikaObj* self) {
    void* target_tile = pika_arm2d_window.pfb_tile_now;
    bool bIsNewFrame = pika_arm2d_window.pfb_is_new_frame;
    int posX = args_getInt(self, "posX");
    int posY = args_getInt(self, "posY");
    int sizeX = args_getInt(self, "sizeX");
    int sizeY = args_getInt(self, "sizeY");
    int alpha = args_getInt(self, "alpha");
    char* color = args_getStr(self, "color");
    arm_2d_region_t tBox = {
        .tLocation = {posX, posY},
        .tSize = {sizeX, sizeY},
    };
    draw_round_corner_box(target_tile, &tBox, getColorCode(color), alpha ,bIsNewFrame);
}

void Arm2D_Box_setColor(PikaObj* self, char* color) {
    args_setStr(self, "color", color);
}

void Arm2D_Box_setSize(PikaObj* self, int x, int y) {
    args_setInt(self, "sizeX", x);
    args_setInt(self, "sizeY", y);
}
