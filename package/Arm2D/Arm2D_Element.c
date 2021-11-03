#include "Arm2D_Element.h"
#include "Arm2D_common.h"
void Arm2D_Element_update(PikaObj* self) {
    /* need to be override */
    args_setErrorCode(self, 1);
    args_setSysOut(self, "[error]: update method not be overrided !");
}

void Arm2D_Element_init(PikaObj* self) {
    args_setInt(self, "posX", 0);
    args_setInt(self, "posY", 0);
    args_setInt(self, "alpha", 255);
}

void Arm2D_Element_setAlpha(PikaObj* self, int alpha) {
    args_setInt(self, "alpha", alpha);
}

void Arm2D_Element_up(PikaObj* self, int y) {
    args_setInt(self, "posY", obj_getInt(self, "posY") - y);
}

void Arm2D_Element_down(PikaObj* self, int y) {
    args_setInt(self, "posY", obj_getInt(self, "posY") + y);
}

void Arm2D_Element_lift(PikaObj* self, int x) {
    args_setInt(self, "posX", obj_getInt(self, "posX") - x);
}

void Arm2D_Element_move(PikaObj* self, int x, int y) {
    args_setInt(self, "posX", x);
    args_setInt(self, "posY", y);
}

void Arm2D_Element_right(PikaObj* self, int x) {
    args_setInt(self, "posX", obj_getInt(self, "posX") + x);
}
