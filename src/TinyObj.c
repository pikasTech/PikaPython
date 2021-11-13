/*
  Author: lyon
  Tencent QQ: 645275593
*/

#include "PikaObj.h"

void _UpdateHandle(PikaObj* self) {
    // override the handle function here
}

void _beforDinit(PikaObj* self) {
    /* override in user code */
}

PikaObj* New_TinyObj(Args* args) {
    PikaObj* self = New_PikaObj();
    return self;
}
