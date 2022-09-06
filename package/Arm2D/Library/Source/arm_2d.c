/*
 * Copyright (C) 2010-2022 Arm Limited or its affiliates. All rights reserved.
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
 * Title:        arm-2d.c
 * Description:  Tables for pixel pipeline OPs
 *
 * $Date:        11. Aug 2022
 * $Revision:    V.1.0.2
 *
 * Target Processor:  Cortex-M cores
 *
 * -------------------------------------------------------------------- */


/*============================ INCLUDES ======================================*/
#define __ARM_2D_IMPL__

#include "arm_2d.h"
#include "__arm_2d_impl.h"


#define __ARM_2D_COMPILATION_UNIT
#include "../Source/__arm_2d_core.c"

#define __ARM_2D_COMPILATION_UNIT
#include "../Source/__arm_2d_tile.c"


#ifdef   __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if defined(__ARM_2D_HAS_CDE__) && !__ARM_2D_HAS_CDE__
#   define __arm_2d_cde_init()
#endif

#if defined(__ARM_2D_HAS_HELIUM__) && !__ARM_2D_HAS_HELIUM__
#   define __arm_2d_helium_init()
#endif

#if defined(__ARM_2D_HAS_HW_ACC__) && !__ARM_2D_HAS_HW_ACC__
#   define __arm_2d_acc_init()
#endif

#if defined(__ARM_2D_HAS_ASYNC__) && !__ARM_2D_HAS_ASYNC__
#   define __arm_2d_async_init(...)
#endif

#ifndef __ARM_2D_CFG_DEFAULT_SUB_TASK_POOL_SIZE__
#   define __ARM_2D_CFG_DEFAULT_SUB_TASK_POOL_SIZE__          4
#endif
#if __ARM_2D_CFG_DEFAULT_SUB_TASK_POOL_SIZE__ < 4
#   warning The __ARM_2D_CFG_DEFAULT_SUB_TASK_POOL_SIZE__ should be larger than or\
 equal to 4, set it to the default value 4.
#   undef __ARM_2D_CFG_DEFAULT_SUB_TASK_POOL_SIZE__
#   define __ARM_2D_CFG_DEFAULT_SUB_TASK_POOL_SIZE__          4
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#if defined(__ARM_2D_HAS_ASYNC__) && __ARM_2D_HAS_ASYNC__
/*! 
 * \brief initialise the arm-2d pipeline
 * \param ptSubTasks an array of __arm_2d_sub_task_t objects
 * \param hwCount the number of items in the array
 * \return arm_2d_err_t error code
 */
extern
arm_2d_err_t __arm_2d_async_init(   __arm_2d_sub_task_t *ptSubTasks, 
                                    uint_fast16_t hwCount);
#endif

#if defined(__ARM_2D_HAS_HELIUM__) && __ARM_2D_HAS_HELIUM__
/*! 
 * \brief initialise the helium acceleration
 */
extern
void __arm_2d_helium_init(void);
#endif

#if defined(__ARM_2D_HAS_CDE__) && __ARM_2D_HAS_CDE__
/*! 
 * \brief initialise the cde service
 */
extern
void __arm_2d_cde_init(void);
#endif



#if defined(__ARM_2D_HAS_HW_ACC__) && __ARM_2D_HAS_HW_ACC__
/*! 
 * \brief initialise the hardware accelerator adapter
 */
extern
void __arm_2d_acc_init(void);
#endif


/*! 
 *  \brief initialise the arm-2d core service
 */
extern
void __arm_2d_init(void);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

/*! 
 * \brief initialise arm-2d
 */
void arm_2d_init(void)
{
    __arm_2d_init();      
    
    do {
        static __arm_2d_sub_task_t 
            s_tDefaultTaskPool[__ARM_2D_CFG_DEFAULT_SUB_TASK_POOL_SIZE__];

        ARM_2D_UNUSED(s_tDefaultTaskPool);

        __arm_2d_async_init(s_tDefaultTaskPool, dimof(s_tDefaultTaskPool));
    } while(0);

    __arm_2d_helium_init();                                             
    __arm_2d_cde_init();                                                
    __arm_2d_acc_init();                                                
}

#ifdef   __cplusplus
}
#endif
