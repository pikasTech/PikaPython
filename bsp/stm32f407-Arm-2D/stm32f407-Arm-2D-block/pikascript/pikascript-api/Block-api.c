/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "Block.h"
#include "SysObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void Block_initMethod(MimiObj *self, Args *args){
    Block_init(self);
}

void Block_moveLeftMethod(MimiObj *self, Args *args){
    int x = args_getInt(args, "x");
    Block_moveLeft(self, x);
}

void Block_moveRightMethod(MimiObj *self, Args *args){
    int x = args_getInt(args, "x");
    Block_moveRight(self, x);
}

void Block_moveToMethod(MimiObj *self, Args *args){
    int x = args_getInt(args, "x");
    int y = args_getInt(args, "y");
    Block_moveTo(self, x, y);
}

void Block_offMethod(MimiObj *self, Args *args){
    Block_off(self);
}

void Block_onMethod(MimiObj *self, Args *args){
    Block_on(self);
}

void Block_setColorMethod(MimiObj *self, Args *args){
    char * color = args_getStr(args, "color");
    Block_setColor(self, color);
}

void Block_setSizeMethod(MimiObj *self, Args *args){
    int sizeX = args_getInt(args, "sizeX");
    int sizeY = args_getInt(args, "sizeY");
    Block_setSize(self, sizeX, sizeY);
}

MimiObj *New_Block(Args *args){
    MimiObj *self = New_SysObj(args);
    class_defineMethod(self, "init()", Block_initMethod);
    class_defineMethod(self, "moveLeft(x:int)", Block_moveLeftMethod);
    class_defineMethod(self, "moveRight(x:int)", Block_moveRightMethod);
    class_defineMethod(self, "moveTo(x:int,y:int)", Block_moveToMethod);
    class_defineMethod(self, "off()", Block_offMethod);
    class_defineMethod(self, "on()", Block_onMethod);
    class_defineMethod(self, "setColor(color:str)", Block_setColorMethod);
    class_defineMethod(self, "setSize(sizeX:int,sizeY:int)", Block_setSizeMethod);
    return self;
}
