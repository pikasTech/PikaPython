#include "Arm2D.h"
#include "Arm2D_Region.h"
#include "Arm2D_Location.h"
#include "Arm2D_common.h"


int Arm2D_is_point_inside_region(PikaObj* self,
                                 PikaObj* region,
                                 PikaObj* location) {
    arm_2d_region_t* _region = obj_getStruct(region, "_self");
    arm_2d_location_t* _location = obj_getStruct(location, "_self");
    return arm_2d_is_point_inside_region(_region, _location);
}

int Arm2D_is_root_tile(PikaObj* self, PikaObj* tile) {
    arm_2d_tile_t* _tile = obj_getStruct(tile, "_self");
    return arm_2d_is_root_tile(_tile);
}

PikaObj* Arm2D_get_absolute_location(PikaObj *self, PikaObj* tile){
    arm_2d_tile_t* _tile = obj_getStruct(tile, "_self");
    arm_2d_location_t _location = {0};
    arm_2d_get_absolute_location(_tile, &_location);
    PikaObj* location = newNormalObj(New_Arm2D_Location);
    obj_setStruct(location, "_self", _location);
    return location;
}
