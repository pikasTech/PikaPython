#include "Arm2D_Element.h"
#include "Arm2D_common.h"
void Arm2D_Element_update(PikaObj* self) {
    /* need to be override */
    args_setErrorCode(self->list, 1);
    args_setSysOut(self->list, "[error]: update method not be overrided !");
}

void Arm2D_Element_init(PikaObj* self) {
    /* init element info */
    obj_setInt(self, "alpha", 255);
    obj_setInt(self, "x", 0);
    obj_setInt(self, "y", 0);
}

void Arm2D_Element_setAlpha(PikaObj* self, int alpha) {
    obj_setInt(self, "alpha_last", obj_getInt(self, "alpha"));
    obj_setInt(self, "alpha", alpha);
}

void Arm2D_Element_up(PikaObj* self, int y) {
    int y_now = obj_getInt(self, "y");
    obj_setInt(self, "y_last", y_now);
    obj_setInt(self, "y", y_now - y);
}

void Arm2D_Element_down(PikaObj* self, int y) {
    int y_now = obj_getInt(self, "y");
    obj_setInt(self, "y_last", y_now);
    obj_setInt(self, "y", y_now + y);
}

void Arm2D_Element_lift(PikaObj* self, int x) {
    int x_now = obj_getInt(self, "x");
    obj_setInt(self, "x_last", x_now);
    obj_setInt(self, "x", x_now - x);
}

void Arm2D_Element_right(PikaObj* self, int x) {
    int x_now = obj_getInt(self, "x");
    obj_setInt(self, "x_last", x_now);
    obj_setInt(self, "x", x_now + x);
}

void Arm2D_Element_move(PikaObj* self, int x, int y) {
    int x_now = obj_getInt(self, "x");
    int y_now = obj_getInt(self, "y");
    obj_setInt(self, "x_last", x_now);
    obj_setInt(self, "y_last", y_now);
    obj_setInt(self, "x", x);
    obj_setInt(self, "y", y);
}
