#include "Arm2D_Window.h"
#include "Arm2D_Background.h"
#include "Arm2D_ElementList.h"
#include "Arm2D_Tile.h"
#include "Arm2D_common.h"

#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "pikaScript.h"

pika_arm2d_globals_t pika_arm2d_globals;
arm_2d_helper_pfb_t s_tPFBHelper;

int32_t __Arm2D_platform_drawRegin(uint32_t x,
                                   uint32_t y,
                                   uint32_t width,
                                   uint32_t height,
                                   const uint8_t* bitmap);

static void __pfb_render_handler(void* pTarget,
                                 const arm_2d_pfb_t* ptPFB,
                                 bool bIsNewFrame) {
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

static void Arm2D_callback_update(void) {
    PikaObj* self = pika_arm2d_globals.pika_windows_object;
    if (obj_getInt(self, "callback_exist") == 1) {
        PikaObj* __frameBuffer = obj_getPtr(self, "__frameBuffer");
        obj_setStruct(__frameBuffer, "_self", *pika_arm2d_globals.pfb_tile_now);
        obj_setInt(self, "__isNewFrame", pika_arm2d_globals.pfb_is_new_frame);
        /* clang-format off */
        PIKA_PYTHON(
        __callback(__frameBuffer, __isNewFrame)
        )
        /* clang-format on */
        const uint8_t bytes[] = {
            0x0c, 0x00, 0x00, 0x00,/* instruct array size */
            0x10, 0x81, 0x01, 0x00, 0x10, 0x01, 0x0f, 0x00, 0x00, 0x02, 0x1c,
            0x00,
            /* instruct array */
            0x27, 0x00, 0x00, 0x00,/* const pool size */
            0x00, 0x5f, 0x5f, 0x66, 0x72, 0x61, 0x6d, 0x65, 0x42, 0x75, 0x66,
            0x66, 0x65, 0x72, 0x00, 0x5f, 0x5f, 0x69, 0x73, 0x4e, 0x65, 0x77,
            0x46, 0x72, 0x61, 0x6d, 0x65, 0x00, 0x5f, 0x5f, 0x63, 0x61, 0x6c,
            0x6c, 0x62, 0x61, 0x63, 0x6b, 0x00, /* const pool */
        };
        pikaVM_runByteCode(self, (uint8_t*)bytes);
    }
}

static arm_fsm_rt_t pika_pfb_drow_window_hanlder(
    void* pTarget,
    const arm_2d_tile_t* frameBuffer,
    bool isNewFrame) {
    ARM_2D_UNUSED(pTarget);

    pika_arm2d_globals.pfb_tile_now = (arm_2d_tile_t*)frameBuffer;
    pika_arm2d_globals.pfb_is_new_frame = isNewFrame;
    PikaObj* background = pika_arm2d_globals.pika_background_object;
    PikaObj* elems = pika_arm2d_globals.pika_elems_object;
    Arm2D_BackGround_update(background);
    Arm2D_ElementList_update(elems);
    Arm2D_callback_update();
    return arm_fsm_rt_cpl;
}

void pika_arm2d_init(void) {
    arm_irq_safe {
        arm_2d_init();
    }
    //! initialise FPB helper
    if (ARM_2D_HELPER_PFB_INIT(
            &s_tPFBHelper,           //!< FPB Helper object
            ARM2D_LCD_WIDTH,         //!< screen width
            ARM2D_LCD_HEIGHT,        //!< screen height
            uint16_t,                //!< colour date type
            ARM2D_PFB_BLOCK_WIDTH,   //!< PFB block width
            ARM2D_PFB_BLOCK_HEIGHT,  //!< PFB block height
            1,                       //!< number of PFB in the PFB pool
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

void Arm2D_Window___init__(PikaObj* self) {
    obj_setInt(self, "callback_exist", 0);
    PikaObj* __frameBuffer = newNormalObj(New_Arm2D_Tile);
    obj_setPtr(self, "__frameBuffer", __frameBuffer);
    pika_arm2d_globals.pika_windows_object = self;
    pika_arm2d_globals.pika_elems_object = obj_getObj(self, "elems");
    pika_arm2d_globals.pika_background_object = obj_getObj(self, "background");
    pika_arm2d_globals.dirty_region_list = NULL;
}

void Arm2D_Window_addCallBack(PikaObj* self, Arg* callback) {
    obj_setInt(self, "callback_exist", 1);
    obj_setArg(self, "__callback", callback);
}
