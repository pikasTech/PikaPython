#include "app-arm2d.h"

#include <rtthread.h>
#include <board.h>

#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "lcd_printf.h"
#include "pikaScript.h"
extern PikaObj *pikaMain;

static arm_2d_helper_pfb_t s_tPFBHelper;

extern int32_t GLCD_DrawBitmap(uint32_t x,
                               uint32_t y,
                               uint32_t width,
                               uint32_t height,
                               const uint8_t *bitmap);

static IMPL_PFB_ON_LOW_LV_RENDERING(__pfb_render_handler)
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

static IMPL_PFB_ON_DRAW(__pfb_draw_handler_t)
{
    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(bIsNewFrame);

    obj_setPtr(pikaMain, "ptTile", (void *)ptTile);
    obj_setInt(pikaMain, "bIsNewFrame", bIsNewFrame);
    obj_run(pikaMain, "screen.update(ptTile, bIsNewFrame)");

    return arm_fsm_rt_cpl;
}

__OVERRIDE_WEAK
void example_gui_on_refresh_evt_handler(const arm_2d_tile_t *ptFrameBuffer)
{
    ARM_2D_UNUSED(ptFrameBuffer);

    //! print performance info
}

void arm_2d_app_init(void)
{
    arm_irq_safe
    {
        arm_2d_init();
    }

    //! initialise FPB helper
    if (ARM_2D_HELPER_PFB_INIT(
            &s_tPFBHelper, //!< FPB Helper object
            LCD_WIDTH,     //!< screen width
            LCD_HEIGHT,    //!< screen height
            uint16_t,      //!< colour date type
            LCD_WIDTH,     //!< PFB block width
            80,            //!< PFB block height
            1,             //!< number of PFB in the PFB pool
            {
                .evtOnLowLevelRendering = {
                    //! callback for low level rendering
                    .fnHandler = &__pfb_render_handler,
                },
                .evtOnDrawing = {
                    //! callback for drawing GUI
                    .fnHandler = &__pfb_draw_handler_t,
                },
            }) < 0)
    {
        //! error detected
        assert(false);
    }
}

void arm_2d_app_update()
{
    while (arm_fsm_rt_cpl != arm_2d_helper_pfb_task(&s_tPFBHelper, NULL))
        ;
}
