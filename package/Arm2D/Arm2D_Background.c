#include "Arm2D_common.h"

void Arm2D_BackGround_init(PikaObj *self)
{
    obj_setStr(self, "color", "white");
}

char *Arm2D_BackGround_getColor(PikaObj *self)
{
    return obj_getStr(self, "color");
}


void Arm2D_BackGround_update(PikaObj *self, int bIsNewFrame, void * ptTile)
{
    char *color = obj_getStr(self, "color");
    uint16_t backGroundColor = getColorCode(color);
    arm_2d_rgb16_fill_colour(ptTile, NULL, backGroundColor);
}

void Arm2D_BackGround_setColor(PikaObj *self, char *color)
{
    obj_setStr(self, "color", color);
}
