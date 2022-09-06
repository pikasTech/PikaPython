/*
 * Copyright (c) 2009-2022 Arm Limited. All rights reserved.
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
 * Title:        #include "arm_2d_dis_adapters.h"
 * Description:  Public header file to include all display adapter header files
 *
 * $Date:        12. Aug 2022
 * $Revision:    V.1.0.0
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_DISP_ADAPTERS_H__
#define __ARM_2D_DISP_ADAPTERS_H__

/*============================ INCLUDES ======================================*/

#ifdef __RTE_ACCELERATION_ARM_2D_HELPER_DISP_ADAPTER0__
#   include "arm_2d_disp_adapter_0.h"
#endif

#ifdef __RTE_ACCELERATION_ARM_2D_HELPER_DISP_ADAPTER1__
#   include "arm_2d_disp_adapter_1.h"
#endif

#ifdef __RTE_ACCELERATION_ARM_2D_HELPER_DISP_ADAPTER2__
#   include "arm_2d_disp_adapter_2.h"
#endif

#ifdef __RTE_ACCELERATION_ARM_2D_HELPER_DISP_ADAPTER3__
#   include "arm_2d_disp_adapter_3.h"
#endif

#ifdef __RTE_ACCELERATION_ARM_2D_HELPER_DISP_ADAPTER4__
#   include "arm_2d_disp_adapter_4.h"
#endif

#ifdef __RTE_ACCELERATION_ARM_2D_HELPER_DISP_ADAPTER5__
#   include "arm_2d_disp_adapter_5.h"
#endif

#ifdef __RTE_ACCELERATION_ARM_2D_HELPER_DISP_ADAPTER6__
#   include "arm_2d_disp_adapter_6.h"
#endif

#ifdef __RTE_ACCELERATION_ARM_2D_HELPER_DISP_ADAPTER7__
#   include "arm_2d_disp_adapter_7.h"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/


#endif
