#include "Arm2D_Window.h"
#include "Arm2D_Background.h"
#include "Arm2D_ElementList.h"
#include "Arm2D_common.h"

#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "pikaScript.h"

pika_arm2d_window_t pika_arm2d_window;
static arm_2d_helper_pfb_t s_tPFBHelper;

static IMPL_PFB_ON_LOW_LV_RENDERING(__pfb_render_handler) {
    const arm_2d_tile_t* pfb_tile = &(ptPFB->tTile);

    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(bIsNewFrame);

    __Arm2D_platform_drawRegin(
        pfb_tile->tRegion.tLocation.iX, pfb_tile->tRegion.tLocation.iY,
        pfb_tile->tRegion.tSize.iWidth, pfb_tile->tRegion.tSize.iHeight,
        pfb_tile->pchBuffer);

    arm_2d_helper_pfb_report_rendering_complete(&s_tPFBHelper,
                                                (arm_2d_pfb_t*)ptPFB);
}

static IMPL_PFB_ON_DRAW(pika_pfb_drow_window_hanlder) {
    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(bIsNewFrame);

    pika_arm2d_window.pfb_tile_now = (arm_2d_tile_t*)ptTile;
    pika_arm2d_window.pfb_is_new_frame = bIsNewFrame;
    PikaObj* background = pika_arm2d_window.pika_background_object;
    PikaObj* elems = pika_arm2d_window.pika_elems_object;
    Arm2D_BackGround_update(background);
    Arm2D_ElementList_update(elems);
    return arm_fsm_rt_cpl;
}

void pika_arm2d_init(void) {
    arm_irq_safe { arm_2d_init(); }
    //! initialise FPB helper
    if (ARM_2D_HELPER_PFB_INIT(
            &s_tPFBHelper,    //!< FPB Helper object
            ARM2D_LCD_WIDTH,        //!< screen width
            ARM2D_LCD_HEIGHT,       //!< screen height
            uint16_t,         //!< colour date type
            ARM2D_PFB_BLOCK_WIDTH,    //!< PFB block width
            ARM2D_PFB_BLOCK_HEIGHT,  //!< PFB block height
            1,                //!< number of PFB in the PFB pool
            {
                .evtOnLowLevelRendering =
                    {
                        //! callback for low level rendering
                        .fnHandler = &__pfb_render_handler,
                    },
                .evtOnDrawing =
                    {
                        //! callback for drawing GUI
                        .fnHandler = &pika_pfb_drow_window_hanlder,
                    },
            }) < 0) {
        //! error detected
        assert(false);
    }
}

void Arm2D_Window_init(PikaObj* self) {
    __Arm2D_platform_Init();
    obj_run(self, "background.init()");
    pika_arm2d_init();
    pika_arm2d_window.pika_windows_object = self;
    pika_arm2d_window.pika_elems_object = obj_getObj(self, "elems", 0);
    pika_arm2d_window.pika_background_object =
        obj_getObj(self, "background", 0);
    pika_arm2d_window.dirty_region_list = NULL;
}

void Arm2D_Window_update(PikaObj* self) {
    while (arm_fsm_rt_cpl != arm_2d_helper_pfb_task(&s_tPFBHelper, NULL))
        ;
}
