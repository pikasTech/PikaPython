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

void Arm2D___init__(PikaObj* self) {
    int32_t __Arm2D_platform_Init();
    __Arm2D_platform_Init();

    /* const */
    obj_setInt(self, "CP_MODE_COPY", ARM_2D_CP_MODE_COPY);
    obj_setInt(self, "CP_MODE_FILL", ARM_2D_CP_MODE_FILL);
    obj_setInt(self, "CP_MODE_Y_MIRROR", ARM_2D_CP_MODE_Y_MIRROR);
    obj_setInt(self, "CP_MODE_X_MIRROR", ARM_2D_CP_MODE_X_MIRROR);
    obj_setInt(self, "CP_MODE_XY_MIRROR", ARM_2D_CP_MODE_XY_MIRROR);

    obj_setInt(self, "COLOR_WHITE", GLCD_COLOR_WHITE);
    obj_setInt(self, "COLOR_BLACK", GLCD_COLOR_BLACK);
    obj_setInt(self, "COLOR_RED", GLCD_COLOR_RED);
    obj_setInt(self, "COLOR_BLUE", GLCD_COLOR_BLUE);
    obj_setInt(self, "COLOR_GREEN", GLCD_COLOR_GREEN);

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

int Arm2D_alpha_blending(PikaObj* self,
                         PikaObj* src,
                         PikaObj* des,
                         PikaObj* reg,
                         int alp) {
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

PikaObj* Arm2D_create_location(PikaObj* self, int x, int y) {
    PikaObj* location = newNormalObj(New_Arm2D_Location);
    arm_2d_location_t _location = {
        .iX = x,
        .iY = y,
    };
    obj_setStruct(location, "_self", _location);
    return location;
}

PikaObj* Arm2D_create_region(PikaObj* self, int x, int y, int w, int h) {
    PikaObj* region = newNormalObj(New_Arm2D_Region);
    arm_2d_region_t _region = {
        .tLocation =
            {
                .iX = x,
                .iY = y,
            },
        .tSize =
            {
                .iWidth = w,
                .iHeight = h,
            },
    };
    obj_setStruct(region, "_self", _region);
    return region;
}

int Arm2D_tile_rotation(PikaObj* self,
                        PikaObj* src,
                        PikaObj* des,
                        PikaObj* des_reg,
                        PikaObj* centre,
                        double angle,
                        int mask_color) {
    static arm_2d_op_trans_t op_trans;
    arm_2d_tile_t* _src = obj_getStruct(src, "_self");
    arm_2d_tile_t* _des = obj_getStruct(des, "_self");
    arm_2d_region_t* _des_reg = obj_getStruct(des_reg, "_self");
    arm_2d_location_t* _centre = obj_getStruct(centre, "_self");
    int bIsNewFrame = pika_arm2d_globals.pfb_is_new_frame;
    return arm_2dp_tile_rotation(&op_trans, _src, _des, _des_reg, *_centre,
                                 angle, mask_color);
}
