#ifndef __ARM_2D__COMMON_H
#define __ARM_2D__COMMON_H

#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "lcd_printf.h"
#include "pikaScript.h"
#include "app-arm2d.h"
#include "Arm2D_Box.h"
#include "dataStrs.h"
#include "BaseObj.h"

uint16_t getColorCode(char *colorName);
typedef struct
{
    arm_2d_op_rotate_t tOP;
    const arm_2d_tile_t *ptTile;
    float fAngle;
    float fAngleSpeed;
    arm_2d_location_t tCentre;
    arm_2d_region_t tRegion;
} rotate_tile_t;

#endif