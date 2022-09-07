#include "Arm2D.h"
#include "Arm2D_Location.h"
#include "Arm2D_Region.h"
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

PikaObj* Arm2D_get_absolute_location(PikaObj* self, PikaObj* tile) {
    arm_2d_tile_t* _tile = obj_getStruct(tile, "_self");
    arm_2d_location_t _location = {0};
    arm_2d_get_absolute_location(_tile, &_location);
    PikaObj* location = newNormalObj(New_Arm2D_Location);
    obj_setStruct(location, "_self", _location);
    return location;
}

void Arm2D___init__(PikaObj* self) {
    int32_t __Arm2D_platform_Init();
    __Arm2D_platform_Init();

    /* const */
    obj_setInt(self, "CP_MODE_COPY", ARM_2D_CP_MODE_COPY);
    obj_setInt(self, "CP_MODE_FILL", ARM_2D_CP_MODE_FILL);
    obj_setInt(self, "CP_MODE_Y_MIRROR", ARM_2D_CP_MODE_Y_MIRROR);
    obj_setInt(self, "CP_MODE_X_MIRROR", ARM_2D_CP_MODE_X_MIRROR);
    obj_setInt(self, "CP_MODE_XY_MIRROR", ARM_2D_CP_MODE_XY_MIRROR);

    pika_arm2d_init();
}

int Arm2D_tile_copy(PikaObj* self,
                    PikaObj* src,
                    PikaObj* des,
                    PikaObj* des_reg,
                    int mode) {
    arm_2d_tile_t* _src = obj_getStruct(src, "_self");
    arm_2d_tile_t* _des = obj_getStruct(des, "_self");
    arm_2d_region_t* _des_reg = obj_getStruct(des_reg, "_self");
    return arm_2d_tile_copy(_src, _des, _des_reg, mode);
}

int Arm2D_alpha_blending(PikaObj *self, PikaObj* src, PikaObj* des, PikaObj* reg, int alp){
    arm_2d_tile_t* _src = obj_getStruct(src, "_self");
    arm_2d_tile_t* _des = obj_getStruct(des, "_self");
    arm_2d_region_t* _reg = obj_getStruct(reg, "_self");
    return arm_2d_alpha_blending(_src, _des, _reg, alp);
}

int Arm2D_fill_colour(PikaObj* self, PikaObj* tile, PikaObj* reg, int color) {
    arm_2d_tile_t* _tile = obj_getStruct(tile, "_self");
    arm_2d_region_t* _reg = obj_getStruct(reg, "_self");
    return arm_2d_fill_colour(_tile, _reg, color);
}

void Arm2D_update(PikaObj* self) {
    extern arm_2d_helper_pfb_t s_tPFBHelper;
    while (arm_fsm_rt_cpl != arm_2d_helper_pfb_task(&s_tPFBHelper, NULL))
        ;
}
