/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "Box.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void Box_initMethod(MimiObj *self, Args *args){
    Box_init(self);
}

void Box_moveLeftMethod(MimiObj *self, Args *args){
    int x = args_getInt(args, "x");
    Box_moveLeft(self, x);
}

void Box_moveRightMethod(MimiObj *self, Args *args){
    int x = args_getInt(args, "x");
    Box_moveRight(self, x);
}

void Box_moveToMethod(MimiObj *self, Args *args){
    int x = args_getInt(args, "x");
    int y = args_getInt(args, "y");
    Box_moveTo(self, x, y);
}

void Box_offMethod(MimiObj *self, Args *args){
    Box_off(self);
}

void Box_onMethod(MimiObj *self, Args *args){
    Box_on(self);
}

void Box_setColorMethod(MimiObj *self, Args *args){
    char * color = args_getStr(args, "color");
    Box_setColor(self, color);
}

void Box_setSizeMethod(MimiObj *self, Args *args){
    int sizeX = args_getInt(args, "sizeX");
    int sizeY = args_getInt(args, "sizeY");
    Box_setSize(self, sizeX, sizeY);
}

MimiObj *New_Box(Args *args){
    MimiObj *self = New_TinyObj(args);
    class_defineMethod(self, "init()", Box_initMethod);
    class_defineMethod(self, "moveLeft(x:int)", Box_moveLeftMethod);
    class_defineMethod(self, "moveRight(x:int)", Box_moveRightMethod);
    class_defineMethod(self, "moveTo(x:int,y:int)", Box_moveToMethod);
    class_defineMethod(self, "off()", Box_offMethod);
    class_defineMethod(self, "on()", Box_onMethod);
    class_defineMethod(self, "setColor(color:str)", Box_setColorMethod);
    class_defineMethod(self, "setSize(sizeX:int,sizeY:int)", Box_setSizeMethod);
    return self;
}
