#include "Arm2d_common.h"

extern const uint8_t c_bmpSun[56 * 57 * sizeof(uint16_t)];
const static arm_2d_tile_t c_tPictureSun = {
    .tRegion =
        {
            .tSize = {.iWidth = 56, .iHeight = 57},
        },
    .tInfo.bIsRoot = true,
    .phwBuffer = (uint16_t*)c_bmpSun,
};

void Arm2D_Star_centra(PikaObj* self, int x, int y) {
    obj_setInt(self, "centreX", x);
    obj_setInt(self, "centreY", y);
}
void Arm2D_Star_speed(PikaObj* self, float speed) {
    obj_setFloat(self, "angleSpeed", speed);
}

void Arm2D_Star_init(PikaObj* self) {
    obj_setInt(self, "posX", 250);
    obj_setInt(self, "posY", 250);
    obj_setInt(self, "centreX", 0);
    obj_setInt(self, "centreY", 0);
    obj_setFloat(self, "angleSpeed", 0.5f);
    obj_setFloat(self, "angle", 0.0f);

    rotate_tile_t rotateTile = {
        .ptTile = &c_tPictureSun,
        .fAngleSpeed = 0.0f,
        .tCentre =
            {
                .iX = 0,
                .iY = 0,
            },
        .tRegion =
            {
                .tLocation =
                    {
                        .iX = 200,
                        .iY = 200,
                    },
                .tSize = c_tPictureSun.tRegion.tSize,
            },
        .fAngle = 0.0,
    };

    Arg* rotateTileArg = New_arg(NULL);
    arg_setContent(rotateTileArg, &rotateTile, sizeof(rotate_tile_t));
    arg_setName(rotateTileArg, "rotateTile");
    arg_setType(rotateTileArg, "rotate_tile_t");
    obj_setArg(self, "rotateTile", rotateTileArg);
    arg_deinit(rotateTileArg);
}

void Arm2D_Star_update(PikaObj* self) {
    int bIsNewFrame = pika_arm2d_window.pfb_is_new_frame;
    void* target_tile = pika_arm2d_window.pfb_tile_now;

    Arg* rotateTileArg = obj_getArg(self, "rotateTile");
    rotate_tile_t* s_tSun = arg_getContent(rotateTileArg);
    s_tSun->tRegion.tLocation.iX = obj_getInt(self, "posX");
    s_tSun->tRegion.tLocation.iY = obj_getInt(self, "posY");
    s_tSun->tCentre.iX = obj_getInt(self, "centreX");
    s_tSun->tCentre.iY = obj_getInt(self, "centreY");
    s_tSun->fAngleSpeed = obj_getFloat(self, "angleSpeed");

    if (bIsNewFrame) {
        s_tSun->fAngle += s_tSun->fAngleSpeed;
    }
    arm_2dp_rgb565_tile_rotation(&(s_tSun->tOP), s_tSun->ptTile, target_tile,
                                 &(s_tSun->tRegion), s_tSun->tCentre,
                                 s_tSun->fAngle, GLCD_COLOR_WHITE);
}
