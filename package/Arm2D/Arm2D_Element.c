#include "Arm2D_common.h"

void Arm2D_Element_update(PikaObj *self)
{
    /* need to be override */
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error]: update method not be overrided !");
}

void Arm2D_Element_init(PikaObj *self)
{
    obj_setInt(self, "posX", 0);
    obj_setInt(self, "posY", 0);
}

void Arm2D_Element_up(PikaObj *self, int y)
{
    obj_setInt(self, "posY", obj_getInt(self, "posY") - y);
}

void Arm2D_Element_down(PikaObj *self, int y)
{
    obj_setInt(self, "posY", obj_getInt(self, "posY") + y);
}

void Arm2D_Element_lift(PikaObj *self, int x)
{
    obj_setInt(self, "posX", obj_getInt(self, "posX") - x);
}

void Arm2D_Element_move(PikaObj *self, int x, int y)
{
    obj_setInt(self, "posX", x);
    obj_setInt(self, "posY", y);
}

void Arm2D_Element_right(PikaObj *self, int x)
{
    obj_setInt(self, "posX", obj_getInt(self, "posX") + x);
}
