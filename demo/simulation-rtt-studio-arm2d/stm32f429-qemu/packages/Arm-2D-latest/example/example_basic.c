
#include <rtthread.h>
#include <board.h>

#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "lcd_printf.h"

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

extern int32_t GLCD_DrawBitmap (uint32_t x,
        uint32_t y,
        uint32_t width,
        uint32_t height,
        const uint8_t *bitmap);

static
IMPL_PFB_ON_LOW_LV_RENDERING(__pfb_render_handler)
{
    const arm_2d_tile_t *ptTile = &(ptPFB->tTile);

    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(bIsNewFrame);

    GLCD_DrawBitmap(ptTile->tRegion.tLocation.iX,
                    ptTile->tRegion.tLocation.iY,
                    ptTile->tRegion.tSize.iWidth,
                    ptTile->tRegion.tSize.iHeight,
                    ptTile->pchBuffer);

    arm_2d_helper_pfb_report_rendering_complete(&s_tPFBHelper,
                                                (arm_2d_pfb_t *)ptPFB);
}

static
IMPL_PFB_ON_DRAW(__pfb_draw_handler_t)
{
    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(bIsNewFrame);

    arm_2d_region_t tBox = {
        .tLocation = {50,50},
        .tSize = {200, 100},
    };

    arm_2d_rgb16_fill_colour(ptTile, NULL, GLCD_COLOR_WHITE);

    arm_2d_rgb16_fill_colour(ptTile, &tBox, GLCD_COLOR_BLACK);

    tBox.tLocation.iX -= 10;
    tBox.tLocation.iY -= 10;

    arm_2d_rgb565_fill_colour_with_alpha(
        ptTile,
        &tBox,
        (arm_2d_color_rgb565_t){GLCD_COLOR_BLUE},
        128);

    tBox.tLocation.iX -= 10;
    tBox.tLocation.iY -= 10;
    tBox.tSize.iHeight = 1;
    tBox.tSize.iWidth  = 50;

    arm_2d_rgb16_fill_colour(ptTile, &tBox, GLCD_COLOR_BLUE);

    lcd_text_location(0, 0);
    lcd_printf("Arm-2D\n");

    return arm_fsm_rt_cpl;
}

__OVERRIDE_WEAK
void example_gui_on_refresh_evt_handler(const arm_2d_tile_t *ptFrameBuffer)
{
    ARM_2D_UNUSED(ptFrameBuffer);

    //! print performance info
}

void arm_2d_basic(void)
{
    arm_irq_safe{
        arm_2d_init();
    }

    //! initialise FPB helper
    if (ARM_2D_HELPER_PFB_INIT(
            &s_tPFBHelper,     //!< FPB Helper object
            LCD_WIDTH,               //!< screen width
            LCD_HEIGHT,               //!< screen height
            uint16_t,          //!< colour date type
            100,                //!< PFB block width
            8,                //!< PFB block height
            1,                 //!< number of PFB in the PFB pool
            {
                .evtOnLowLevelRendering = {
                    //! callback for low level rendering
                    .fnHandler = &__pfb_render_handler,
                },
                .evtOnDrawing = {
                    //! callback for drawing GUI
                    .fnHandler = &__pfb_draw_handler_t,
                },
            }
        ) < 0) {
        //! error detected
        assert(false);
    }

    while(arm_fsm_rt_cpl != arm_2d_helper_pfb_task(&s_tPFBHelper, NULL));
}
MSH_CMD_EXPORT(arm_2d_basic, Arm-2D basic example.);
