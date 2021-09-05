#include "MimiObj.h"
#include <stdio.h>
void Box_init(MimiObj *self)
{
    obj_setInt(self, "isOn", 0);
    obj_setInt(self, "x", 50);
    obj_setInt(self, "y", 50);
    obj_setInt(self, "sizeX", 150);
    obj_setInt(self, "sizeY", 75);
}
void Box_moveLeft(MimiObj *self, int x)
{
    obj_setInt(self, "x", obj_getInt(self, "x") - x);
}
void Box_moveRight(MimiObj *self, int x)
{    
    obj_setInt(self, "x", obj_getInt(self, "x") + x);
}
void Box_moveTo(MimiObj *self, int x, int y)
{    
    obj_setInt(self, "x", x);
    obj_setInt(self, "y", y);
}    
void Box_off(MimiObj *self)
{    
    obj_setInt(self, "isOn", 0);
}
void Box_on(MimiObj *self)
{    
    obj_setInt(self, "isOn", 1);
}
void Box_setColor(MimiObj *self, char * color)
{   
}
void Box_setSize(MimiObj *self, int sizeX, int sizeY)
{
    obj_setInt(self, "sizeX", sizeX);
    obj_setInt(self, "sizeY", sizeY);
}
