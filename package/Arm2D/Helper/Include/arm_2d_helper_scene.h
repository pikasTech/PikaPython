/*
 * Copyright (C) 2022 Arm Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* ----------------------------------------------------------------------
 * Project:      Arm-2D Library
 * Title:        #include "arm_2d_helper_scene.h"
 * Description:  Public header file for the scene service
 *
 * $Date:        29. Aug 2022
 * $Revision:    V.1.3.4
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_HELPER_SCENE_H__
#define __ARM_2D_HELPER_SCENE_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d_helper_pfb.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#   pragma clang diagnostic ignored "-Wpadded"
#endif

/*!
 * \addtogroup gHelper 7 Helper Services
 * @{
 */


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

/*!
 * \brief scene switching mode
 */
typedef enum {

    /* valid switching visual effects begin */
    ARM_2D_SCENE_SWITCH_MODE_NONE           = 0,                                //!< no switching visual effect
    ARM_2D_SCENE_SWITCH_MODE_USER           = 1,                                //!< user defined switching visual effect
    ARM_2D_SCENE_SWITCH_MODE_FADE_WHITE     = 2,                                //!< fade in fade out (white)
    ARM_2D_SCENE_SWITCH_MODE_FADE_BLACK     = 3,                                //!< fade in fade out (black)
    ARM_2D_SCENE_SWITCH_MODE_SLIDE_LEFT     = 4,                                //!< slide left
    ARM_2D_SCENE_SWITCH_MODE_SLIDE_RIGHT,                                       //!< slide right
    ARM_2D_SCENE_SWITCH_MODE_SLIDE_UP,                                          //!< slide up
    ARM_2D_SCENE_SWITCH_MODE_SLIDE_DOWN,                                        //!< slide down
    ARM_2D_SCENE_SWITCH_MODE_ERASE_LEFT     = 8,                                //!< erase to the right
    ARM_2D_SCENE_SWITCH_MODE_ERASE_RIGHT,                                       //!< erase to the left
    ARM_2D_SCENE_SWITCH_MODE_ERASE_UP,                                          //!< erase to the top
    ARM_2D_SCENE_SWITCH_MODE_ERASE_DOWN,                                        //!< erase to the bottom
    
    /* valid switching visual effects end */
    __ARM_2D_SCENE_SWITCH_MODE_VALID,                                           //!< For internal user only
    

    
    ARM_2D_SCENE_SWITCH_MODE_IGNORE_OLD_BG          = _BV(8),                   //!< ignore the background of the old scene
    ARM_2D_SCENE_SWITCH_MODE_IGNORE_OLD_SCEBE       = _BV(9),                   //!< ignore the old scene
    ARM_2D_SCENE_SWITCH_MODE_IGNORE_NEW_BG          = _BV(10),                  //!< ignore the background of the new scene
    ARM_2D_SCENE_SWITCH_MODE_IGNORE_NEW_SCEBE       = _BV(11),                  //!< ignore the new scene
    
    ARM_2D_SCENE_SWITCH_MODE_DEFAULT_BG_WHITE       = 0 << 12,                  //!< use white as default background
    ARM_2D_SCENE_SWITCH_MODE_DEFAULT_BG_BLACK       = 1 << 12,                  //!< use black as default background
    ARM_2D_SCENE_SWITCH_MODE_DEFAULT_BG_USER        = 2 << 12,                  //!< use user defined default background

    __ARM_2D_SCENE_SWTICH_MODE_IGNORE_msk           = 0x0F << 8,                //!< For internal user only
    __ARM_2D_SCENE_SWTICH_MODE_IGNORE_pos           = 8,                        //!< For internal user only
    __ARM_2D_SCENE_SWTICH_MODE_DEFAULT_BG_msk       = 3 << 12,                  //!< For internal user only
    __ARM_2D_SCENE_SWTICH_MODE_DEFAULT_BG_pos       = 12,                       //!< For internal user only

} arm_2d_scene_player_switch_mode_t;

typedef union __arm_2d_helper_scene_switch_t {
    struct {
        uint8_t chMode;                                                         //!< the switch visual effect
        uint8_t bIgnoreOldSceneBG       : 1;                                    //!< when set, ignore the background of the old scene
        uint8_t bIgnoreOldScene         : 1;                                    //!< when set, ignore the old scene
        uint8_t bIgnoreNewSceneBG       : 1;                                    //!< when set, ignore the background of the new scene
        uint8_t bIgnoreNewScene         : 1;                                    //!< when set, ignore the new scene
        uint8_t u2DefaultBG             : 2;                                    //!< the default background
        uint8_t                         : 2;
    } Feature;
    uint16_t hwSetting;                                                         //!< the setting value
}__arm_2d_helper_scene_switch_t;



typedef struct arm_2d_scene_player_t arm_2d_scene_player_t;

/*!
 * \brief a class for describing scenes which are the combination of a
 *        background and a foreground with a dirty-region-list support
 * 
 */
typedef struct arm_2d_scene_t arm_2d_scene_t;
struct arm_2d_scene_t {
    arm_2d_scene_t *ptNext;                                                     //!< next scene
    arm_2d_scene_player_t *ptPlayer;                                            //!< points to the host scene player
    arm_2d_region_list_item_t       *ptDirtyRegion;                             //!< dirty region list for the foreground 
    arm_2d_helper_draw_handler_t    *fnBackground;                              //!< the function pointer for the background 
    arm_2d_helper_draw_handler_t    *fnScene;                                   //!< the function pointer for the foreground
    void (*fnOnBGStart)(arm_2d_scene_t *ptThis);                                //!< on-start-drawing-background event handler
    void (*fnOnBGComplete)(arm_2d_scene_t *ptThis);                             //!< on-complete-drawing-background event handler
    void (*fnOnFrameStart)(arm_2d_scene_t *ptThis);                             //!< on-frame-start event handler
    void (*fnOnFrameCPL)(arm_2d_scene_t *ptThis);                               //!< on-frame-complete event handler

    /*!
     * \note We use fnDepose to free the resources
     */
    void (*fnDepose)(arm_2d_scene_t *ptThis);                                   //!< on-scene-depose event handler
    struct {
        uint8_t bOnSwitchingIgnoreBG    : 1;                                    //!< ignore background during switching period
        uint8_t bOnSwitchingIgnoreScene : 1;                                    //!< ignore forground during switching period
    };
};

/*!
 * \brief a class to manage scenes
 * 
 */
struct arm_2d_scene_player_t {
    inherit(arm_2d_helper_pfb_t);                                               //!< inherit from arm_2d_helper_pfb_t
    
    ARM_PRIVATE(
        struct {
            arm_2d_scene_t *ptHead;                                             //!< points to the head of the FIFO
            arm_2d_scene_t *ptTail;                                             //!< points to the tail of the FIFO
        } SceneFIFO;                                                            //!< Scene FIFO
        
        struct {
            uint8_t bNextSceneReq   : 1;                                        //!< a flag to request switching-to-the next-scene
            uint8_t bSwitchCPL      : 1;                                        //!< indication of scene switching completion
            uint8_t                 : 6;
            uint8_t chState;                                                    //!< the state of the FSM used by runtime.
        } Runtime;                                                              //!< scene player runtime
        
        struct {
            union {
                uint8_t chState;
                struct {
                    uint8_t chState;
                    uint8_t chOpacity;
                    bool bIsFadeBlack;
                }Fade;
                struct {
                    uint8_t chState;
                    arm_2d_tile_t tSceneWindow;
                    arm_2d_tile_t tTemp;
                    int16_t iOffset;
                }Erase;
                struct {
                    uint8_t chState;
                    arm_2d_tile_t tSceneWindow;
                    int16_t iOffset;
                }Slide;
            };
            __arm_2d_helper_scene_switch_t tConfig;                             //!< the switching configuration
            uint16_t hwPeriod;                                                  //!< the switching should finish in specified millisecond
            int64_t lTimeStamp;
        }Switch;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*!
 * \brief flush the scene FIFO
 * 
 * \param[in] ptThis the target scene player
 */
extern
ARM_NONNULL(1)
void arm_2d_scene_player_flush_fifo(arm_2d_scene_player_t *ptThis);

/*!
 * \brief append a set of scenes to a scene player
 *
 * \param[in] ptThis the target scene player
 * \param[in] ptScenes a scene array
 * \param[in] hwCount the number of scenes in the array
 */
extern
ARM_NONNULL(1)
void arm_2d_scene_player_append_scenes(arm_2d_scene_player_t *ptThis, 
                                            arm_2d_scene_t *ptScenes,
                                            int_fast16_t hwCount);

/*!
 * \brief request switching to the next scene safely
 * 
 * \param[in] ptThis the target scene player
 * 
 * \note Once received a request, the scene player will only switch to the
 *       next scene at the end of a frame. 
 */
extern
ARM_NONNULL(1)
void arm_2d_scene_player_switch_to_next_scene(arm_2d_scene_player_t *ptThis);

/*!
 * \brief configure the scene switching mode
 *
 * \param[in] ptThis the target scene player
 * \param[in] hwSettings a combination of valid settings defined in
 *            arm_2d_scene_player_switch_mode_t
 */
extern
ARM_NONNULL(1)
void arm_2d_scene_player_set_switching_mode(arm_2d_scene_player_t *ptThis,
                                            uint_fast16_t hwSettings);
                                            
/*!
 * \brief read the current scene switching mode
 *
 * \param[in] ptThis the target scene player
 * \return uint16_t the current setting value for the scene switching mode
 */
extern
ARM_NONNULL(1)
uint16_t arm_2d_scene_player_get_switching_mode(arm_2d_scene_player_t *ptThis);

/*!
 * \brief configure the scene switching period
 *
 * \param[in] ptThis the target scene player
 * \param[in] hwMS period in millisecond
 */
extern
ARM_NONNULL(1)
void arm_2d_scene_player_set_switching_period(  arm_2d_scene_player_t *ptThis,
                                                uint_fast16_t hwMS);

/*!
 * \brief the scene player task function
 * 
 * \param[in] ptThis the target scene player
 * 
 * \note the event sequence of a scene:
 *       1. when fnBackground is valid
 *           - invoke fnOnBGStart when it is valid
 *           - invoke fnBackground
 *           - invoke fnOnBGComplete when it is valid
 *       2. invoke fnOnFrameStart when it is valid
 *       3. invoke fnScene
 *       4. invoke fnOnFrameCPL when it is valid
 *       5. Check bNextSceneReq
 *           - false (0), go back to step 2
 *           - true, invoke fnDepose when it is valid and switch to the next scene
 *          
 */
extern
ARM_NONNULL(1)
arm_fsm_rt_t arm_2d_scene_player_task(arm_2d_scene_player_t *ptThis);

/*! @} */

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
