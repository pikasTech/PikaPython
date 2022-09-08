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
 * Title:        #include "arm_2d.h"
 * Description:  Public header file to contain the all avaialble Arm-2D 
 *               interface header files 
 *
 * $Date:        05. Sept 2022
 * $Revision:    V.1.0.7
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
#include "arm_2d_transform.h"

#ifdef   __cplusplus
extern "C" {
#endif

/* suppress some warnings for user applications when using arm-2d.
 */
#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#   pragma diag_suppress 1296,174
#endif

/*!
 * \addtogroup gKernel 1 Kernel
 * @{
 */

/*============================ MACROS ========================================*/

/*  arm-2d version */
#define ARM_2D_VERSION_MAJOR        1                                           //!< Major version
#define ARM_2D_VERSION_MINOR        1                                           //!< Minor version
#define ARM_2D_VERSION_PATCH        0                                           //!< Patch number
#define ARM_2D_VERSION_STR          "preview1"                                  //!< tag

/*!
 * \brief arm-2d version number in decimal
 * 
 */
#define ARM_2D_VERISON              (   ARM_2D_VERSION_MAJOR * 10000ul          \
                                    +   ARM_2D_VERSION_MINOR * 100ul            \
                                        ARM_2D_VERSION_PATCH)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

/*!
 * \brief a type for arm-2d runtime configuration 
 * 
 */
typedef struct {
    /*! if the target region is out of the target tile, return arm_fsm_rt_cpl */
    uint8_t     TREAT_OUT_OF_RANGE_AS_COMPLETE          : 1;
    
    /*! indicate that there is a dedicated thread to run arm_2d_task() in RTOS env */
    uint8_t     HAS_DEDICATED_THREAD_FOR_2D_TASK        : 1;
    uint8_t                                             : 6;
} arm_2d_runtime_feature_t;

/*!
 * \brief a type for arm-2d version
 * 
 */
typedef struct {
    uint8_t     Major;              //!< major version
    uint8_t     Minor;              //!< minor version
    uint8_t     Patch;              //!< patch number
    uint8_t                 : 8;
} arm_2d_version_t;

/*============================ GLOBAL VARIABLES ==============================*/

/*!
 * \brief arm-2d runtime feature configuration
 * 
 */
extern
arm_2d_runtime_feature_t ARM_2D_RUNTIME_FEATURE;

/*!
 * \brief arm-2d version
 * 
 */
extern 
const arm_2d_version_t ARM_2D_VERSION;

/*============================ PROTOTYPES ====================================*/

/*! 
 * \brief initialise arm-2d
 */
extern
void arm_2d_init(void);

/*!
 * \brief set the default frame buffer
 * \param ptFrameBuffer the new frame buffer, 
 * \note  Passing NULL means using no default framebuffer
 * \return arm_2d_tile_t* the address of the old frame buffer
 */
extern 
arm_2d_tile_t *arm_2d_set_default_frame_buffer(
                                        const arm_2d_tile_t *ptFrameBuffer);

/*! 
 * \brief get the default frame buffer
 * \return arm_2d_tile_t* the address of the default frame buffer
 */
extern
arm_2d_tile_t *arm_2d_get_default_frame_buffer(void);

/*! 
 * \brief attach a user param (which could be a pointer) to specified OP
 *
 * \param ptOP the address of the target OP (NULL means using the default OP)
 * 
 * \param pUserParam a user parameter (it can be used as a pointer)
 */
extern
void arm_2d_set_user_param(arm_2d_op_core_t *ptOP, uintptr_t pUserParam);

/*! 
 * \brief wait asynchronous operation complete
 * \param[in] ptOP the address of the target OP (NULL means using the default OP)
 * \retval true sync up with operation
 * \retval false operation is busy
 */
extern
bool arm_2d_op_wait_async(arm_2d_op_core_t *ptOP);

/*! 
   \brief get the status of a specified OP, 
   \details usually, it is used after calling arm_2d_op_wait_async(). 
    E.g.
    \code
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
    \endcode
    \param ptOP the address of the target OP (NULL means using the default OP)
    \return arm_2d_op_status_t the operation status
 */
extern
arm_2d_op_status_t arm_2d_get_op_status(arm_2d_op_core_t *ptOP);

/*! 
 * \brief arm-2d pixel pipeline task entery
 * \note  This function is *TRHEAD-SAFE*
 * \param ptTask the address of an arm-2d task control block
 * \retval arm_fsm_rt_cpl The sub-task FIFO is empty, the caller can wait for a 
 *         semaphore set by arm_2d_notif_sub_task_fifo_task_arrive()
 * \retval arm_fsm_rt_on_going The arm_2d_task yields 
 * \retval arm_fsm_rt_async You shouldn't see this value
 * \retval arm_fsm_rt_wait_for_obj hardware accelerator wants to sync-up with applications.
 * \retval (<0) Serious error is detected.
 */
extern
arm_fsm_rt_t arm_2d_task(arm_2d_task_t *ptTask);

/*! @} */

/*! \note delibrately comment out */
//#if defined(__clang__)
//#   pragma clang diagnostic pop
//#endif

#ifdef   __cplusplus
}
#endif


#endif
