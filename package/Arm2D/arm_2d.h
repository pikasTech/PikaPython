/*
 * Copyright (C) 2020 Arm Limited or its affiliates. All rights reserved.
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
 * Title:        #include "arm_2d.h"
 * Description:  Public header file to contain the all avaialble Arm-2D 
 *               interface header files 
 *
 * $Date:        01. December 2020
 * $Revision:    V.0.5.0
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */
 
#ifndef __ARM_2D_H__
#define __ARM_2D_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d_types.h"
#include "arm_2d_op.h"
#include "arm_2d_tile.h"
#include "arm_2d_draw.h"
#include "arm_2d_conversion.h"
#include "arm_2d_alpha_blending.h"
#include "arm_2d_rotation.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct {
    uint8_t     TREAT_OUT_OF_RANGE_AS_COMPLETE          : 1;
    uint8_t     HAS_DEDICATED_THREAD_FOR_2D_TASK        : 1;
    uint8_t                                             : 6;
} arm_2d_runtime_feature_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern
arm_2d_runtime_feature_t ARM_2D_RUNTIME_FEATURE;

/*============================ PROTOTYPES ====================================*/

#if defined(__ARM_2D_HAS_ASYNC__) && __ARM_2D_HAS_ASYNC__
/*! \brief initialise the whole arm-2d service
 *! \param none
 *! \return none
 */
extern
void __arm_2d_async_init(void);
#endif

#if defined(__ARM_2D_HAS_HELIUM__) && __ARM_2D_HAS_HELIUM__
/*! \brief initialise the helium service
 *! \param none
 *! \return none
 */
extern
void __arm_2d_helium_init(void);
#endif

#if defined(__ARM_2D_HAS_CDE__) && __ARM_2D_HAS_CDE__
/*! \brief initialise the cde service
 *! \param none
 *! \return none
 */
extern
void __arm_2d_cde_init(void);
#endif



#if defined(__ARM_2D_HAS_HW_ACC__) && __ARM_2D_HAS_HW_ACC__
/*! \brief initialise the hardware accelerator adapter
 *! \param none
 *! \return none
 */
extern
void __arm_2d_acc_init(void);
#endif


/*! \brief initialise the whole arm-2d service
 *! \param none
 *! \return none
 */
extern
void __arm_2d_init(void);

/*! \brief set the default frame buffer
 *! \param ptFramebuffer the new frame buffer, if NULL is given, no default
 *!       frame buffer will be used
 *! \return the address of the old frame buffer
 */
extern 
arm_2d_tile_t *arm_2d_set_default_frame_buffer(
                                        const arm_2d_tile_t *ptFrameBuffer);


/*! \brief get the default frame buffer
 *! \return the address of the default frame buffer
 */
extern
arm_2d_tile_t *arm_2d_get_default_frame_buffer(void);

/*! \brief attach a user param (which could be a pointer) to specified OP
 *! \param ptOP the address of the target OP (NULL means using the default OP)
 *! \param pUserParam a user param (it can be used as a pointer)
 */
extern
void arm_2d_set_user_param(arm_2d_op_core_t *ptOP, uintptr_t pUserParam);


/*! \brief sync up with operation 
 *! \retval true sync up with operation
 *! \retval false operation is busy
 */
extern
bool arm_2d_op_wait_async(arm_2d_op_core_t *ptOP);

/*! \brief get the status of a specified OP, usually, it is used after calling
 *!        arm_2d_op_wait_async(). 
 *!        E.g.
 
            //! wait for previous operation complete
            do {
                arm_2d_op_wait_async();
                arm_2d_op_status_t tStatus = arm_2d_get_op_status();
                if (tStatus.bIOError) {
                    //! error detected
                    ...
                } else if (tStatus.bOpCpl) {
                    break;
                }
            } while(true);
 *!
 *! \param ptOP the address of the target OP (NULL means using the default OP)
 *! \return the status
 */
extern
arm_2d_op_status_t arm_2d_get_op_status(arm_2d_op_core_t *ptOP);

 /*! \brief arm-2d pixel pipeline task entery
  *! \note  This function is *TRHEAD-SAFE*
  *! \param none
  *! \retval arm_fsm_rt_cpl The sub-task FIFO is empty, the caller, i.e. the host
  *!            RTOS thread can block itself by waiting for a semaphore which is
  *!            set by arm_2d_notif_sub_task_fifo_task_arrive()
  *! \retval arm_fsm_rt_on_going The arm_2d_task issued one sub-task without 
  *!            problem and it yields. 
  *! \retval arm_fsm_rt_async You shouldn't see this value
  *! \retval arm_fsm_rt_wait_for_obj some algorithm or hardware accelerator wants
  *!            to sync-up with applications.
  *! \retval (<0) Serious error is detected.
  */
extern
arm_fsm_rt_t arm_2d_task(arm_2d_task_t *ptTask);

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif


#endif
