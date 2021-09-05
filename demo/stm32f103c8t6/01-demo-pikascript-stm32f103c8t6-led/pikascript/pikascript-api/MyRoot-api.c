/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "MyRoot.h"
#include "BaseObj.h"
#include "LED.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

PikaObj *New_MyRoot(Args *args){
    PikaObj *self = New_BaseObj(args);
    obj_import(self, "LED", New_LED);
    obj_newObj(self, "led", "LED");
    return self;
}
