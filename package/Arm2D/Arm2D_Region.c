#include "Arm2D_common.h"
#include "Arm2D_region.h"

void Arm2D_Region___init__(PikaObj *self){
    arm_2d_region_t _self = {0};
    if (NULL == obj_getStruct(self, "_self")){
        obj_setStruct(self, "_self", _self);
    }
}

PikaObj* Arm2D_Region_intersect(PikaObj *self, PikaObj* in2){
    arm_2d_region_t* _self = obj_getStruct(self, "_self");
    arm_2d_region_t* _in2 = obj_getStruct(in2, "_self");
    arm_2d_region_t _intersect = {0};
    arm_2d_region_intersect(_self, _in2, &_intersect);
    PikaObj* intersect = newNormalObj(New_Arm2D_Region);
    obj_setStruct(intersect, "_self", _intersect);
    return intersect;
}
