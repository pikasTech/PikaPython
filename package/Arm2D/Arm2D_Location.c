#include "Arm2D_common.h"
#include "Arm2D_Location.h"

void Arm2D_Location___init__(PikaObj *self){
    arm_2d_location_t _self = {0};
    if (NULL == obj_getStruct(self, "_self")){
        obj_setStruct(self, "_self", _self);
    }
}
