
/*
 * Copyright (C) 2010-2021 Arm Limited or its affiliates. All rights reserved.
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
 * Title:        __arm_2d_fill_colour.inc
 * Description:  c code template for drawing pattern
 *
 * $Date:        14. April 2020
 * $Revision:    V.1.0.0
 *
 * -------------------------------------------------------------------- */


#ifndef __API_COLOUR
#   error You have to define __API_COLOUR before using this c template
#endif
#ifndef __API_INT_TYPE
#   error You have to define the __API_INT_TYPE before using this c template
#endif

#undef ____ARM_2D_FUNC
#undef ___ARM_2D_FUNC
#undef __ARM_2D_FUNC
#define ____ARM_2D_FUNC(__NAME, __COLOUR)  __arm_2d_impl_##__COLOUR##_##__NAME
#define ___ARM_2D_FUNC(__NAME, __COLOUR)   ____ARM_2D_FUNC(__NAME, __COLOUR) 
#define __ARM_2D_FUNC(__NAME)   ___ARM_2D_FUNC(__NAME, __API_COLOUR)


__WEAK
void __ARM_2D_FUNC(colour_filling)( __API_INT_TYPE *__RESTRICT pTarget,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    __API_INT_TYPE Colour)
{
    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {
        for (int_fast16_t x = 0; x < ptCopySize->iWidth; x++){
            pTarget[x] = Colour;
        }
        pTarget += iTargetStride;
    }
}

#undef ____ARM_2D_FUNC
#undef ___ARM_2D_FUNC
#undef __ARM_2D_FUNC
#undef __API_COLOUR
#undef __API_INT_TYPE
