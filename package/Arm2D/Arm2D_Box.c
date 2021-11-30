#include "Arm2D_Box.h"
#include "Arm2D_common.h"
#include "asset_corner_box.h"
#include "dataArgs.h"

void Arm2D_Box_init(PikaObj* self) {
    /* init element info */
    obj_setInt(self, "alpha", 255);
    obj_setInt(self, "x", 0);
    obj_setInt(self, "y", 0);
    /* init box info */
    obj_setInt(self, "weight", 50);
    obj_setInt(self, "height", 50);
    obj_setInt(self, "color", getColorCode("blue"));
}

int __min(int x, int y) {
    return x < y ? x : y;
}

int __max(int x, int y) {
    return x > y ? x : y;
}

void Arm2D_Box_update(PikaObj* self) {
    void* target_tile = pika_arm2d_window.pfb_tile_now;
    bool bIsNewFrame = pika_arm2d_window.pfb_is_new_frame;

    arm_2d_region_t arg2d_regin = {0};
    arg2d_regin.tSize.iHeight = obj_getInt(self, "height");
    arg2d_regin.tSize.iWidth = obj_getInt(self, "weight");
    arg2d_regin.tLocation.iX = obj_getInt(self, "x");
    arg2d_regin.tLocation.iY = obj_getInt(self, "y");
    uint16_t color_code = obj_getInt(self, "color");
    uint8_t alpha = obj_getInt(self, "alpha");
    draw_round_corner_box(target_tile, &arg2d_regin, color_code, alpha, bIsNewFrame);
}

void Arm2D_Box_setColor(PikaObj* self, char* color) {
    obj_setInt(self, "color",  getColorCode(color));
}

void Arm2D_Box_setSize(PikaObj* self, int x, int y) {
    obj_setInt(self, "weight_last", obj_getInt(self, "weight"));
    obj_setInt(self, "height_last", obj_getInt(self, "height"));
    obj_setInt(self, "weight", x);
    obj_setInt(self, "height", y);
}
