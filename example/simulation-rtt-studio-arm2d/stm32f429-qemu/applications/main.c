/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-08-19     RT-Thread    first version
 */

#include <rtthread.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>
#include <stdio.h>

#include "board.h"
#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "lcd_printf.h"
#include "PikaMain.h"

/* GLCD RGB color definitions */
#define GLCD_COLOR_BLACK        0x0000  /*   0,   0,   0 */
#define GLCD_COLOR_NAVY         0x000F  /*   0,   0, 128 */
#define GLCD_COLOR_DARK_GREEN   0x03E0  /*   0, 128,   0 */
#define GLCD_COLOR_DARK_CYAN    0x03EF  /*   0, 128, 128 */
#define GLCD_COLOR_MAROON       0x7800  /* 128,   0,   0 */
#define GLCD_COLOR_PURPLE       0x780F  /* 128,   0, 128 */
#define GLCD_COLOR_OLIVE        0x7BE0  /* 128, 128,   0 */
#define GLCD_COLOR_LIGHT_GREY   0xC618  /* 192, 192, 192 */
#define GLCD_COLOR_DARK_GREY    0x7BEF  /* 128, 128, 128 */
#define GLCD_COLOR_BLUE         0x001F  /*   0,   0, 255 */
#define GLCD_COLOR_GREEN        0x07E0  /*   0, 255,   0 */
#define GLCD_COLOR_CYAN         0x07FF  /*   0, 255, 255 */
#define GLCD_COLOR_RED          0xF800  /* 255,   0,   0 */
#define GLCD_COLOR_MAGENTA      0xF81F  /* 255,   0, 255 */
#define GLCD_COLOR_YELLOW       0xFFE0  /* 255, 255, 0   */
#define GLCD_COLOR_WHITE        0xFFFF  /* 255, 255, 255 */

static arm_2d_helper_pfb_t s_tPFBHelper;
MimiObj *pikaMain;

extern int32_t GLCD_DrawBitmap(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const uint8_t *bitmap);

static
IMPL_PFB_ON_LOW_LV_RENDERING(__pfb_render_handler)
{
    const arm_2d_tile_t *ptTile = &(ptPFB->tTile);

    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(bIsNewFrame);

    GLCD_DrawBitmap(ptTile->tRegion.tLocation.iX, ptTile->tRegion.tLocation.iY, ptTile->tRegion.tSize.iWidth,
            ptTile->tRegion.tSize.iHeight, ptTile->pchBuffer);

    arm_2d_helper_pfb_report_rendering_complete(&s_tPFBHelper, (arm_2d_pfb_t *) ptPFB);
}

static IMPL_PFB_ON_DRAW(__pfb_draw_background_handler)
{
    ARM_2D_UNUSED(pTarget);
    //! 背景填充白色
    arm_2d_rgb16_fill_colour(ptTile, NULL, GLCD_COLOR_WHITE);
    return arm_fsm_rt_cpl;
}

static IMPL_PFB_ON_DRAW(__pfb_draw_handler)
{
    ARM_2D_UNUSED(pTarget);
    int x = obj_getInt(pikaMain, "box.x");
    int y = obj_getInt(pikaMain, "box.y");
    int sizeX = obj_getInt(pikaMain, "box.sizeX");
    int sizeY = obj_getInt(pikaMain, "box.sizeY");
    int isOn = obj_getInt(pikaMain, "box.isOn");
    arm_2d_region_t tBox = { .tLocation = { x, y }, .tSize = { sizeX, sizeY }, };
    //! 背景填充白色
    arm_2d_rgb16_fill_colour(ptTile, NULL, GLCD_COLOR_WHITE);
    if (isOn)
    {
        arm_2d_rgb16_fill_colour(ptTile, &tBox, GLCD_COLOR_BLUE);
    }
    return arm_fsm_rt_cpl;
}

void draw_main()
{
    while (arm_fsm_rt_cpl != arm_2d_helper_pfb_task(&s_tPFBHelper, NULL))
        ;
}

char inputBuff[256] = { 0 };
int main(void)
{
    printf("Hello RT-Thread!\r\n");
    /* Arm-2D init */
    arm_2d_init();

    /* Arg-2D-helper init */
    //! initialise FPB helper
    if (ARM_2D_HELPER_PFB_INIT(
            &s_tPFBHelper,                    //!< FPB Helper object
            LCD_WIDTH,//!< screen width
            LCD_HEIGHT,//!< screen height
            uint16_t, //!< colour date type
            LCD_WIDTH ,//!< PFB Box width
            40 ,      //!< PFB Box height
            1,        //!< number of PFB in the PFB pool
            {
                .evtOnLowLevelRendering =
                {
                    //! callback for low level rendering
                    .fnHandler = &__pfb_render_handler,
                },
                .evtOnDrawing =
                {
                    //! callback for drawing GUI
                    .fnHandler = &__pfb_draw_background_handler,
                },
            }
    ) < 0)
    {
        //! error detected
        assert(false);
    }

    /* PikaScript init */
    pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* user init */
    while (arm_fsm_rt_cpl != arm_2d_helper_pfb_task(&s_tPFBHelper, NULL))
        ;
    ARM_2D_HELPER_PFB_UPDATE_ON_DRAW_HANDLER(&s_tPFBHelper, &__pfb_draw_handler);
    printf("\r\n");
    printf("==============================================\r\n");
    printf("* Arm-2D and PikaScript in STM32f407         *\r\n");
    printf("* https://github.com/ARM-software/EndpointAI *\r\n");
    printf("* https://github.com/mimilib/pikascript      *\r\n");
    printf("* Have fun!                                  *\r\n");
    printf("==============================================\r\n");

    printf("Pika script Initing...\r\n");
    obj_run(pikaMain, "   box.init()                            ");
    printf("hello Arm-2D and PikaScript!\r\n");
    obj_run(pikaMain, "   box.on()                              ");

    draw_main();
    printf(">>> ");

//    while (1)
//    {
//        fgets(inputBuff, sizeof(inputBuff), stdin);
//        printf("%s\r\n", inputBuff);
//        /* run mimiScript and get res */
//
//        Args *resArgs = obj_runDirect(pikaMain, inputBuff);
//
//        /* get system output of mimiScript*/
//        char *sysOut = args_getStr(resArgs, "sysOut");
//
//        if (NULL != sysOut)
//        {
//            /* print32_t out the system output */
//            printf(sysOut);
//            printf("\r\n");
//        }
//        printf(">>> ");
//
//        /* deinit the res */
//        args_deinit(resArgs);
//        draw_main();
//    }
    return RT_EOK;
}
