/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-11-30     liang       the first version
 */
#include <board.h>
#include "arm2d_config.h"

/* need implement in platform */
int32_t __Arm2D_platform_drawRegin(uint32_t x,
                                   uint32_t y,
                                   uint32_t width,
                                   uint32_t height,
                                   const uint8_t* bitmap){
    return GLCD_DrawBitmap(x, y, width, height, bitmap);
}

/* need implement in platform */
int32_t __Arm2D_platform_Init(){
}
