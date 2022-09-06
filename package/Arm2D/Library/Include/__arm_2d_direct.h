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
 * Title:        __arm_2d_direct.h
 * Description:  header files for internal users or professional developers
 *
 * $Date:        29. April 2022
 * $Revision:    V.1.0.2
 *
 * Target Processor:  Cortex-M cores
 *
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_DIRECT_H__
#define __ARM_2D_DIRECT_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wpadded"
#elif __IS_COMPILER_ARM_COMPILER_5__
#   pragma diag_suppress 174,177,188,68,513,144,64
#elif __IS_COMPILER_IAR__
#   pragma diag_suppress=Pe301
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wunused-value"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/


void __arm_2d_impl_c8bit_1x1_paving(const uint8_t * __restrict pSource,
                    int16_t iSourceStride,
                    uint8_t * __restrict pTarget,
                    int16_t iTargetStride,
                    const arm_2d_size_t *
                    __restrict ptSrcCopySize,
                    const arm_2d_size_t *
                    __restrict ptDstCopySize);

void __arm_2d_impl_c8bit_1x1_paving_x_mirror(const uint8_t * __restrict pSource,
                         int16_t iSourceStride,
                         uint8_t * __restrict pTarget,
                         int16_t iTargetStride,
                         const arm_2d_size_t *
                         __restrict ptSrcCopySize,
                         const arm_2d_size_t *
                         __restrict ptDstCopySize);

void __arm_2d_impl_c8bit_1x1_paving_xy_mirror(const uint8_t *
                          __restrict pSource,
                          int16_t iSourceStride,
                          uint8_t * __restrict pTarget,
                          int16_t iTargetStride,
                          const arm_2d_size_t *
                          __restrict ptSrcCopySize,
                          const arm_2d_size_t *
                          __restrict ptDstCopySize);

void __arm_2d_impl_c8bit_1x1_paving_y_mirror(const uint8_t * __restrict pSource,
                         int16_t iSourceStride,
                         uint8_t * __restrict pTarget,
                         int16_t iTargetStride,
                         const arm_2d_size_t *
                         __restrict ptSrcCopySize,
                         const arm_2d_size_t *
                         __restrict ptDstCopySize);

void __arm_2d_impl_c8bit_1x2_paving(const uint8_t * __restrict pSourceBase,
                    int16_t iSourceStride,
                    const arm_2d_size_t *
                    __restrict ptSourceSize,
                    uint8_t * __restrict pTargetBase,
                    int16_t iTargetStride, uint32_t destWidth,
                    uint16_t tilePairRows);

void __arm_2d_impl_c8bit_1x2_paving_x_mirror(const uint8_t *
                         __restrict pSourceBase,
                         int16_t iSourceStride,
                         const arm_2d_size_t *
                         __restrict ptSourceSize,
                         uint8_t * __restrict pTargetBase,
                         int16_t iTargetStride,
                         uint32_t destWidth,
                         uint16_t tilePairRows);

void __arm_2d_impl_c8bit_1x2_paving_xy_mirror(const uint8_t *
                          __restrict pSourceBase,
                          int16_t iSourceStride,
                          const arm_2d_size_t *
                          __restrict ptSourceSize,
                          uint8_t * __restrict pTargetBase,
                          int16_t iTargetStride,
                          uint32_t destWidth,
                          uint16_t tilePairRows);

void __arm_2d_impl_c8bit_1x2_paving_y_mirror(const uint8_t *
                         __restrict pSourceBase,
                         int16_t iSourceStride,
                         const arm_2d_size_t *
                         __restrict ptSourceSize,
                         uint8_t * __restrict pTargetBase,
                         int16_t iTargetStride,
                         uint32_t destWidth,
                         uint16_t tilePairRows);

void __arm_2d_impl_c8bit_2x1_paving(const uint8_t * __restrict pSourceBase,
                    int16_t iSourceStride,
                    const arm_2d_size_t *
                    __restrict ptSourceSize,
                    uint8_t * __restrict pTargetBase,
                    int16_t iTargetStride,
                    uint16_t tilePairCols, uint16_t destHeight);

void __arm_2d_impl_c8bit_2x1_paving_x_mirror(const uint8_t *
                         __restrict pSourceBase,
                         int16_t iSourceStride,
                         const arm_2d_size_t *
                         __restrict ptSourceSize,
                         uint8_t * __restrict pTargetBase,
                         int16_t iTargetStride,
                         uint16_t tilePairCols,
                         uint16_t destHeight);

void __arm_2d_impl_c8bit_2x1_paving_xy_mirror(const uint8_t *
                          __restrict pSourceBase,
                          int16_t iSourceStride,
                          const arm_2d_size_t *
                          __restrict ptSourceSize,
                          uint8_t * __restrict pTargetBase,
                          int16_t iTargetStride,
                          uint16_t tilePairCols,
                          uint16_t destHeight);

void __arm_2d_impl_c8bit_2x1_paving_y_mirror(const uint8_t *
                         __restrict pSourceBase,
                         int16_t iSourceStride,
                         const arm_2d_size_t *
                         __restrict ptSourceSize,
                         uint8_t * __restrict pTargetBase,
                         int16_t iTargetStride,
                         uint16_t tilePairCols,
                         uint16_t destHeight);

void __arm_2d_impl_c8bit_2x2_paving(const uint8_t * __restrict pSourceBase,
                    int16_t iSourceStride,
                    const arm_2d_size_t *
                    __restrict ptSourceSize,
                    uint8_t * __restrict pTargetBase,
                    int16_t iTargetStride,
                    uint16_t tilePairRows,
                    uint16_t tilePairCols);

void __arm_2d_impl_c8bit_2x2_paving_x_mirror(const uint8_t *
                         __restrict pSourceBase,
                         int16_t iSourceStride,
                         const arm_2d_size_t *
                         __restrict ptSourceSize,
                         uint8_t * __restrict pTargetBase,
                         int16_t iTargetStride,
                         uint16_t tilePairRows,
                         uint16_t tilePairCols);

void __arm_2d_impl_c8bit_2x2_paving_xy_mirror(const uint8_t *
                          __restrict pSourceBase,
                          int16_t iSourceStride,
                          const arm_2d_size_t *
                          __restrict ptSourceSize,
                          uint8_t * __restrict pTargetBase,
                          int16_t iTargetStride,
                          uint16_t tilePairRows,
                          uint16_t tilePairCols);

void __arm_2d_impl_c8bit_2x2_paving_y_mirror(const uint8_t *
                         __restrict pSourceBase,
                         int16_t iSourceStride,
                         const arm_2d_size_t *
                         __restrict ptSourceSize,
                         uint8_t * __restrict pTargetBase,
                         int16_t iTargetStride,
                         uint16_t tilePairRows,
                         uint16_t tilePairCols);

void __arm_2d_impl_c8bit_cl_key_1x1_paving(const uint8_t * __restrict pSource,
                       int16_t iSourceStride,
                       uint8_t * __restrict pTarget,
                       int16_t iTargetStride,
                       const arm_2d_size_t *
                       __restrict ptSrcCopySize,
                       const arm_2d_size_t *
                       __restrict ptDstCopySize,
                       uint8_t Colour);

void __arm_2d_impl_c8bit_cl_key_1x1_paving_x_mirror(const uint8_t *
                            __restrict pSource,
                            int16_t iSourceStride,
                            uint8_t *
                            __restrict pTarget,
                            int16_t iTargetStride,
                            const arm_2d_size_t *
                            __restrict ptSrcCopySize,
                            const arm_2d_size_t *
                            __restrict ptDstCopySize,
                            uint8_t Colour);

void __arm_2d_impl_c8bit_cl_key_1x1_paving_xy_mirror(const uint8_t *
                             __restrict pSource,
                             int16_t iSourceStride,
                             uint8_t *
                             __restrict pTarget,
                             int16_t iTargetStride,
                             const arm_2d_size_t *
                             __restrict ptSrcCopySize,
                             const arm_2d_size_t *
                             __restrict ptDstCopySize,
                             uint8_t Colour);

void __arm_2d_impl_c8bit_cl_key_1x1_paving_y_mirror(const uint8_t *
                            __restrict pSource,
                            int16_t iSourceStride,
                            uint8_t *
                            __restrict pTarget,
                            int16_t iTargetStride,
                            const arm_2d_size_t *
                            __restrict ptSrcCopySize,
                            const arm_2d_size_t *
                            __restrict ptDstCopySize,
                            uint8_t Colour);

void __arm_2d_impl_c8bit_cl_key_1x2_paving(const uint8_t *
                       __restrict pSourceBase,
                       int16_t iSourceStride,
                       const arm_2d_size_t *
                       __restrict ptSourceSize,
                       uint8_t * __restrict pTargetBase,
                       int16_t iTargetStride,
                       uint32_t destWidth,
                       uint16_t tilePairRows,
                       uint8_t Colour);

void __arm_2d_impl_c8bit_cl_key_1x2_paving_x_mirror(const uint8_t *
                            __restrict pSourceBase,
                            int16_t iSourceStride,
                            const arm_2d_size_t *
                            __restrict ptSourceSize,
                            uint8_t *
                            __restrict pTargetBase,
                            int16_t iTargetStride,
                            uint32_t destWidth,
                            uint16_t tilePairRows,
                            uint8_t Colour);

void __arm_2d_impl_c8bit_cl_key_1x2_paving_xy_mirror(const uint8_t *
                             __restrict pSourceBase,
                             int16_t iSourceStride,
                             const arm_2d_size_t *
                             __restrict ptSourceSize,
                             uint8_t *
                             __restrict pTargetBase,
                             int16_t iTargetStride,
                             uint32_t destWidth,
                             uint16_t tilePairRows,
                             uint8_t Colour);

void __arm_2d_impl_c8bit_cl_key_1x2_paving_y_mirror(const uint8_t *
                            __restrict pSourceBase,
                            int16_t iSourceStride,
                            const arm_2d_size_t *
                            __restrict ptSourceSize,
                            uint8_t *
                            __restrict pTargetBase,
                            int16_t iTargetStride,
                            uint32_t destWidth,
                            uint16_t tilePairRows,
                            uint8_t Colour);

void __arm_2d_impl_c8bit_cl_key_2x1_paving(const uint8_t *
                       __restrict pSourceBase,
                       int16_t iSourceStride,
                       const arm_2d_size_t *
                       __restrict ptSourceSize,
                       uint8_t * __restrict pTargetBase,
                       int16_t iTargetStride,
                       uint16_t tilePairCols,
                       uint16_t destHeight, uint8_t Colour);

void __arm_2d_impl_c8bit_cl_key_2x1_paving_x_mirror(const uint8_t *
                            __restrict pSourceBase,
                            int16_t iSourceStride,
                            const arm_2d_size_t *
                            __restrict ptSourceSize,
                            uint8_t *
                            __restrict pTargetBase,
                            int16_t iTargetStride,
                            uint16_t tilePairCols,
                            uint16_t destHeight,
                            uint8_t Colour);

void __arm_2d_impl_c8bit_cl_key_2x1_paving_xy_mirror(const uint8_t *
                             __restrict pSourceBase,
                             int16_t iSourceStride,
                             const arm_2d_size_t *
                             __restrict ptSourceSize,
                             uint8_t *
                             __restrict pTargetBase,
                             int16_t iTargetStride,
                             uint16_t tilePairCols,
                             uint16_t destHeight,
                             uint8_t Colour);

void __arm_2d_impl_c8bit_cl_key_2x1_paving_y_mirror(const uint8_t *
                            __restrict pSourceBase,
                            int16_t iSourceStride,
                            const arm_2d_size_t *
                            __restrict ptSourceSize,
                            uint8_t *
                            __restrict pTargetBase,
                            int16_t iTargetStride,
                            uint16_t tilePairCols,
                            uint16_t destHeight,
                            uint8_t Colour);

void __arm_2d_impl_c8bit_cl_key_2x2_paving(const uint8_t *
                       __restrict pSourceBase,
                       int16_t iSourceStride,
                       const arm_2d_size_t *
                       __restrict ptSourceSize,
                       uint8_t * __restrict pTargetBase,
                       int16_t iTargetStride,
                       uint16_t tilePairRows,
                       uint16_t tilePairCols,
                       uint8_t Colour);

void __arm_2d_impl_c8bit_cl_key_2x2_paving_x_mirror(const uint8_t *
                            __restrict pSourceBase,
                            int16_t iSourceStride,
                            const arm_2d_size_t *
                            __restrict ptSourceSize,
                            uint8_t *
                            __restrict pTargetBase,
                            int16_t iTargetStride,
                            uint16_t tilePairRows,
                            uint16_t tilePairCols,
                            uint8_t Colour);

void __arm_2d_impl_c8bit_cl_key_2x2_paving_xy_mirror(const uint8_t *
                             __restrict pSourceBase,
                             int16_t iSourceStride,
                             const arm_2d_size_t *
                             __restrict ptSourceSize,
                             uint8_t *
                             __restrict pTargetBase,
                             int16_t iTargetStride,
                             uint16_t tilePairRows,
                             uint16_t tilePairCols,
                             uint8_t Colour);

void __arm_2d_impl_c8bit_cl_key_2x2_paving_y_mirror(const uint8_t *
                            __restrict pSourceBase,
                            int16_t iSourceStride,
                            const arm_2d_size_t *
                            __restrict ptSourceSize,
                            uint8_t *
                            __restrict pTargetBase,
                            int16_t iTargetStride,
                            uint16_t tilePairRows,
                            uint16_t tilePairCols,
                            uint8_t Colour);

void __arm_2d_impl_c8bit_cl_key_copy(uint8_t * __restrict pSource,
                     int16_t iSourceStride,
                     uint8_t * __restrict pTarget,
                     int16_t iTargetStride,
                     arm_2d_size_t * __restrict ptCopySize,
                     uint8_t Colour);

void __arm_2d_impl_c8bit_cl_key_copy_mirror(uint8_t * __restrict pSource,
                        int16_t iSourceStride,
                        uint8_t * __restrict pTarget,
                        int16_t iTargetStride,
                        arm_2d_size_t *
                        __restrict ptCopySize,
                        uint32_t wMode, uint8_t Colour);

void __arm_2d_impl_c8bit_cl_key_copy_x_mirror(uint8_t * __restrict pSource,
                          int16_t iSourceStride,
                          uint8_t * __restrict pTarget,
                          int16_t iTargetStride,
                          arm_2d_size_t *
                          __restrict ptCopySize,
                          uint8_t Colour);

void __arm_2d_impl_c8bit_cl_key_copy_xy_mirror(uint8_t * __restrict pSource,
                           int16_t iSourceStride,
                           uint8_t * __restrict pTarget,
                           int16_t iTargetStride,
                           arm_2d_size_t *
                           __restrict ptCopySize,
                           uint8_t Colour);

void __arm_2d_impl_c8bit_cl_key_copy_y_mirror(uint8_t * __restrict pSource,
                          int16_t iSourceStride,
                          uint8_t * __restrict pTarget,
                          int16_t iTargetStride,
                          arm_2d_size_t *
                          __restrict ptCopySize,
                          uint8_t Colour);

void __arm_2d_impl_c8bit_cl_key_fill(uint8_t * __restrict pSourceBase,
                     int16_t iSourceStride,
                     arm_2d_size_t * __restrict ptSourceSize,
                     uint8_t * __restrict pTargetBase,
                     int16_t iTargetStride,
                     arm_2d_size_t * __restrict ptTargetSize,
                     uint8_t Colour);

void __arm_2d_impl_c8bit_cl_key_fill_mirror(uint8_t * __restrict pSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *
                        __restrict ptSourceSize,
                        uint8_t * __restrict pTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *
                        __restrict ptTargetSize,
                        uint32_t wMode, uint8_t Colour);

void __arm_2d_impl_c8bit_cl_key_fill_x_mirror(uint8_t * __restrict pSourceBase,
                          int16_t iSourceStride,
                          arm_2d_size_t *
                          __restrict ptSourceSize,
                          uint8_t * __restrict pTargetBase,
                          int16_t iTargetStride,
                          arm_2d_size_t *
                          __restrict ptTargetSize,
                          uint8_t Colour);

void __arm_2d_impl_c8bit_cl_key_fill_xy_mirror(uint8_t * __restrict pSourceBase,
                           int16_t iSourceStride,
                           arm_2d_size_t *
                           __restrict ptSourceSize,
                           uint8_t * __restrict pTargetBase,
                           int16_t iTargetStride,
                           arm_2d_size_t *
                           __restrict ptTargetSize,
                           uint8_t Colour);

void __arm_2d_impl_c8bit_cl_key_fill_y_mirror(uint8_t * __restrict pSourceBase,
                          int16_t iSourceStride,
                          arm_2d_size_t *
                          __restrict ptSourceSize,
                          uint8_t * __restrict pTargetBase,
                          int16_t iTargetStride,
                          arm_2d_size_t *
                          __restrict ptTargetSize,
                          uint8_t Colour);

void __arm_2d_impl_c8bit_colour_filling(uint8_t * __restrict pTarget,
                    int16_t iTargetStride,
                    arm_2d_size_t * __restrict ptCopySize,
                    uint8_t Colour);

void __arm_2d_impl_c8bit_copy(uint8_t * __restrict pSource,
                  int16_t iSourceStride,
                  uint8_t * __restrict pTarget,
                  int16_t iTargetStride,
                  arm_2d_size_t * __restrict ptCopySize);

void __arm_2d_impl_c8bit_copy_mirror(uint8_t * __restrict pSource,
                     int16_t iSourceStride,
                     uint8_t * __restrict pTarget,
                     int16_t iTargetStride,
                     arm_2d_size_t * __restrict ptCopySize,
                     uint32_t wMode);

void __arm_2d_impl_c8bit_copy_x_mirror(uint8_t * __restrict pSource,
                       int16_t iSourceStride,
                       uint8_t * __restrict pTarget,
                       int16_t iTargetStride,
                       arm_2d_size_t * __restrict ptCopySize);

void __arm_2d_impl_c8bit_copy_xy_mirror(uint8_t * __restrict pSource,
                    int16_t iSourceStride,
                    uint8_t * __restrict pTarget,
                    int16_t iTargetStride,
                    arm_2d_size_t * __restrict ptCopySize);

void __arm_2d_impl_c8bit_copy_y_mirror(uint8_t * __restrict pSource,
                       int16_t iSourceStride,
                       uint8_t * __restrict pTarget,
                       int16_t iTargetStride,
                       arm_2d_size_t * __restrict ptCopySize);

void __arm_2d_impl_c8bit_draw_pattern(uint8_t * __restrict pchSourceBase,
                      int32_t iOffset, int16_t iSourceStride,
                      uint8_t * __restrict pTargetBase,
                      int16_t iTargetStride,
                      arm_2d_size_t * __restrict ptCopySize,
                      uint8_t chMode, uint8_t ForeColour,
                      uint8_t BackColour);

void __arm_2d_impl_c8bit_draw_pattern_bg_comp(uint8_t *
                          __restrict pchSourceBase,
                          int32_t iOffset,
                          int16_t iSourceStride,
                          uint8_t * __restrict pTargetBase,
                          int16_t iTargetStride,
                          arm_2d_size_t *
                          __restrict ptCopySize,
                          uint8_t BackColour);

void __arm_2d_impl_c8bit_draw_pattern_bg_fg(uint8_t * __restrict pchSourceBase,
                        int32_t iOffset,
                        int16_t iSourceStride,
                        uint8_t * __restrict pTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *
                        __restrict ptCopySize,
                        uint8_t ForeColour,
                        uint8_t BackColour);

void __arm_2d_impl_c8bit_draw_pattern_bg_only(uint8_t *
                          __restrict pchSourceBase,
                          int32_t iOffset,
                          int16_t iSourceStride,
                          uint8_t * __restrict pTargetBase,
                          int16_t iTargetStride,
                          arm_2d_size_t *
                          __restrict ptCopySize,
                          uint8_t BackColour);

void __arm_2d_impl_c8bit_draw_pattern_fg_only(uint8_t *
                          __restrict pchSourceBase,
                          int32_t iOffset,
                          int16_t iSourceStride,
                          uint8_t * __restrict pTargetBase,
                          int16_t iTargetStride,
                          arm_2d_size_t *
                          __restrict ptCopySize,
                          uint8_t ForeColour);

void __arm_2d_impl_c8bit_draw_pattern_no_bg_comp(uint8_t *
                         __restrict pchSourceBase,
                         int32_t iOffset,
                         int16_t iSourceStride,
                         uint8_t *
                         __restrict pTargetBase,
                         int16_t iTargetStride,
                         arm_2d_size_t *
                         __restrict ptCopySize);

void __arm_2d_impl_c8bit_fill(uint8_t * __restrict pSourceBase,
                  int16_t iSourceStride,
                  arm_2d_size_t * __restrict ptSourceSize,
                  uint8_t * __restrict pTargetBase,
                  int16_t iTargetStride,
                  arm_2d_size_t * __restrict ptTargetSize);

void __arm_2d_impl_c8bit_fill_mirror(uint8_t * __restrict pSourceBase,
                     int16_t iSourceStride,
                     arm_2d_size_t * __restrict ptSourceSize,
                     uint8_t * __restrict pTargetBase,
                     int16_t iTargetStride,
                     arm_2d_size_t * __restrict ptTargetSize,
                     uint32_t wMode);

void __arm_2d_impl_c8bit_fill_x_mirror(uint8_t * __restrict pSourceBase,
                       int16_t iSourceStride,
                       arm_2d_size_t * __restrict ptSourceSize,
                       uint8_t * __restrict pTargetBase,
                       int16_t iTargetStride,
                       arm_2d_size_t * __restrict ptTargetSize);

void __arm_2d_impl_c8bit_fill_xy_mirror(uint8_t * __restrict pSourceBase,
                    int16_t iSourceStride,
                    arm_2d_size_t * __restrict ptSourceSize,
                    uint8_t * __restrict pTargetBase,
                    int16_t iTargetStride,
                    arm_2d_size_t *
                    __restrict ptTargetSize);

void __arm_2d_impl_c8bit_fill_y_mirror(uint8_t * __restrict pSourceBase,
                       int16_t iSourceStride,
                       arm_2d_size_t * __restrict ptSourceSize,
                       uint8_t * __restrict pTargetBase,
                       int16_t iTargetStride,
                       arm_2d_size_t * __restrict ptTargetSize);

void __arm_2d_impl_cccn888_1h_des_msk_copy(uint32_t * __restrict pSourceBase,
                       int16_t iSourceStride,
                       uint32_t * __restrict pTargetBase,
                       int16_t iTargetStride,
                       uint8_t *
                       __restrict ptTargetMaskBase,
                       int16_t iTargetMaskStride,
                       arm_2d_size_t *
                       __restrict ptTargetMaskSize,
                       arm_2d_size_t *
                       __restrict ptCopySize);

void __arm_2d_impl_cccn888_1h_des_msk_copy_mirror(uint32_t *
                          __restrict pSourceBase,
                          int16_t iSourceStride,
                          uint32_t *
                          __restrict pTargetBase,
                          int16_t iTargetStride,
                          uint8_t *
                          __restrict ptTargetMaskBase,
                          int16_t iTargetMaskStride,
                          arm_2d_size_t *
                          __restrict ptTargetMaskSize,
                          arm_2d_size_t *
                          __restrict ptCopySize,
                          uint32_t wMode);

void __arm_2d_impl_cccn888_1h_des_msk_copy_x_mirror(uint32_t *
                            __restrict pSourceBase,
                            int16_t iSourceStride,
                            uint32_t *
                            __restrict pTargetBase,
                            int16_t iTargetStride,
                            uint8_t *
                            __restrict ptTargetMaskBase,
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *
                            __restrict ptTargetMaskSize,
                            arm_2d_size_t *
                            __restrict ptCopySize);

void __arm_2d_impl_cccn888_1h_des_msk_copy_xy_mirror(uint32_t *
                             __restrict pSourceBase,
                             int16_t iSourceStride,
                             uint32_t *
                             __restrict pTargetBase,
                             int16_t iTargetStride,
                             uint8_t *
                             __restrict
                             ptTargetMaskBase,
                             int16_t iTargetMaskStride,
                             arm_2d_size_t *
                             __restrict
                             ptTargetMaskSize,
                             arm_2d_size_t *
                             __restrict ptCopySize);

void __arm_2d_impl_cccn888_1h_des_msk_copy_y_mirror(uint32_t *
                            __restrict pSourceBase,
                            int16_t iSourceStride,
                            uint32_t *
                            __restrict pTargetBase,
                            int16_t iTargetStride,
                            uint8_t *
                            __restrict ptTargetMaskBase,
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *
                            __restrict ptTargetMaskSize,
                            arm_2d_size_t *
                            __restrict ptCopySize);

void __arm_2d_impl_cccn888_1h_des_msk_fill(uint32_t * __restrict ptSourceBase,
                       int16_t iSourceStride,
                       arm_2d_size_t *
                       __restrict ptSourceSize,
                       uint32_t * __restrict ptTargetBase,
                       int16_t iTargetStride,
                       arm_2d_size_t *
                       __restrict ptTargetSize,
                       uint8_t *
                       __restrict ptTargetMaskBase,
                       int16_t iTargetMaskStride,
                       arm_2d_size_t *
                       __restrict ptTargetMaskSize);

void __arm_2d_impl_cccn888_1h_des_msk_fill_mirror(uint32_t *
                          __restrict ptSourceBase,
                          int16_t iSourceStride,
                          arm_2d_size_t *
                          __restrict ptSourceSize,
                          uint32_t *
                          __restrict ptTargetBase,
                          int16_t iTargetStride,
                          arm_2d_size_t *
                          __restrict ptTargetSize,
                          uint8_t *
                          __restrict ptTargetMaskBase,
                          int16_t iTargetMaskStride,
                          arm_2d_size_t *
                          __restrict ptTargetMaskSize,
                          uint32_t wMode);

void __arm_2d_impl_cccn888_1h_des_msk_fill_x_mirror(uint32_t *
                            __restrict ptSourceBase,
                            int16_t iSourceStride,
                            arm_2d_size_t *
                            __restrict ptSourceSize,
                            uint32_t *
                            __restrict ptTargetBase,
                            int16_t iTargetStride,
                            arm_2d_size_t *
                            __restrict ptTargetSize,
                            uint8_t *
                            __restrict ptTargetMaskBase,
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *
                            __restrict
                            ptTargetMaskSize);

void __arm_2d_impl_cccn888_1h_des_msk_fill_xy_mirror(uint32_t *
                             __restrict ptSourceBase,
                             int16_t iSourceStride,
                             arm_2d_size_t *
                             __restrict ptSourceSize,
                             uint32_t *
                             __restrict ptTargetBase,
                             int16_t iTargetStride,
                             arm_2d_size_t *
                             __restrict ptTargetSize,
                             uint8_t *
                             __restrict
                             ptTargetMaskBase,
                             int16_t iTargetMaskStride,
                             arm_2d_size_t *
                             __restrict
                             ptTargetMaskSize);

void __arm_2d_impl_cccn888_1h_des_msk_fill_y_mirror(uint32_t *
                            __restrict ptSourceBase,
                            int16_t iSourceStride,
                            arm_2d_size_t *
                            __restrict ptSourceSize,
                            uint32_t *
                            __restrict ptTargetBase,
                            int16_t iTargetStride,
                            arm_2d_size_t *
                            __restrict ptTargetSize,
                            uint8_t *
                            __restrict ptTargetMaskBase,
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *
                            __restrict
                            ptTargetMaskSize);

void __arm_2d_impl_cccn888_alpha_blending(uint32_t * __restrict pSourceBase,
                      int16_t iSourceStride,
                      uint32_t * __restrict pTargetBase,
                      int16_t iTargetStride,
                      arm_2d_size_t * __restrict ptCopySize,
                      uint_fast16_t chRatio);

void __arm_2d_impl_cccn888_alpha_blending(uint32_t * pwSourceBase,
                      int16_t iSourceStride,
                      uint32_t * pwTargetBase,
                      int16_t iTargetStride,
                      arm_2d_size_t * ptCopySize,
                      uint_fast16_t chRatio);

void __arm_2d_impl_cccn888_alpha_blending_colour_keying(uint32_t *
                            __restrict pSourceBase,
                            int16_t iSourceStride,
                            uint32_t *
                            __restrict pTargetBase,
                            int16_t iTargetStride,
                            arm_2d_size_t *
                            __restrict ptCopySize,
                            uint_fast16_t chRatio,
                            uint32_t Colour);

void __arm_2d_impl_cccn888_colour_filling_channel_mask(uint32_t *
                               __restrict pTarget,
                               int16_t iTargetStride,
                               uint32_t *
                               __restrict pwAlpha,
                               int16_t iAlphaStride,
                               arm_2d_size_t *
                               __restrict ptCopySize,
                               uint32_t Colour);

void __arm_2d_impl_cccn888_colour_filling_channel_mask_opacity(uint32_t *
                                   __restrict
                                   pTarget,
                                   int16_t
                                   iTargetStride,
                                   uint32_t *
                                   __restrict
                                   pwAlpha,
                                   int16_t
                                   iAlphaStride,
                                   arm_2d_size_t *
                                   __restrict
                                   ptCopySize,
                                   uint32_t Colour,
                                   uint_fast16_t hwOpacity);

void __arm_2d_impl_cccn888_colour_filling_mask(uint32_t * __restrict pTarget,
                           int16_t iTargetStride,
                           uint8_t * __restrict pchAlpha,
                           int16_t iAlphaStride,
                           arm_2d_size_t *
                           __restrict ptCopySize,
                           uint32_t Colour);

void __arm_2d_impl_cccn888_colour_filling_mask_opacity(uint32_t *
                               __restrict pTarget,
                               int16_t iTargetStride,
                               uint8_t *
                               __restrict pchAlpha,
                               int16_t iAlphaStride,
                               arm_2d_size_t *
                               __restrict ptCopySize,
                               uint32_t Colour,
                               uint_fast16_t hwOpacity);

void __arm_2d_impl_cccn888_colour_filling_with_opacity(uint32_t *
                               __restrict pTargetBase,
                               int16_t iTargetStride,
                               arm_2d_size_t *
                               __restrict ptCopySize,
                               uint32_t Colour,
                               uint_fast16_t hwRatio);

void __arm_2d_impl_cccn888_des_chn_msk_copy(uint32_t * __restrict pSourceBase,
                        int16_t iSourceStride,
                        uint32_t * __restrict pTargetBase,
                        int16_t iTargetStride,
                        uint32_t *
                        __restrict ptTargetMaskBase,
                        int16_t iTargetMaskStride,
                        arm_2d_size_t *
                        __restrict ptTargetMaskSize,
                        arm_2d_size_t *
                        __restrict ptCopySize);

void __arm_2d_impl_cccn888_des_chn_msk_copy_mirror(uint32_t *
                           __restrict pSourceBase,
                           int16_t iSourceStride,
                           uint32_t *
                           __restrict pTargetBase,
                           int16_t iTargetStride,
                           uint32_t *
                           __restrict ptTargetMaskBase,
                           int16_t iTargetMaskStride,
                           arm_2d_size_t *
                           __restrict ptTargetMaskSize,
                           arm_2d_size_t *
                           __restrict ptCopySize,
                           uint32_t wMode);

void __arm_2d_impl_cccn888_des_chn_msk_copy_x_mirror(uint32_t *
                             __restrict pSourceBase,
                             int16_t iSourceStride,
                             uint32_t *
                             __restrict pTargetBase,
                             int16_t iTargetStride,
                             uint32_t *
                             __restrict
                             ptTargetMaskBase,
                             int16_t iTargetMaskStride,
                             arm_2d_size_t *
                             __restrict
                             ptTargetMaskSize,
                             arm_2d_size_t *
                             __restrict ptCopySize);

void __arm_2d_impl_cccn888_des_chn_msk_copy_xy_mirror(uint32_t *
                              __restrict pSourceBase,
                              int16_t iSourceStride,
                              uint32_t *
                              __restrict pTargetBase,
                              int16_t iTargetStride,
                              uint32_t *
                              __restrict
                              ptTargetMaskBase,
                              int16_t iTargetMaskStride,
                              arm_2d_size_t *
                              __restrict
                              ptTargetMaskSize,
                              arm_2d_size_t *
                              __restrict ptCopySize);

void __arm_2d_impl_cccn888_des_chn_msk_copy_y_mirror(uint32_t *
                             __restrict pSourceBase,
                             int16_t iSourceStride,
                             uint32_t *
                             __restrict pTargetBase,
                             int16_t iTargetStride,
                             uint32_t *
                             __restrict
                             ptTargetMaskBase,
                             int16_t iTargetMaskStride,
                             arm_2d_size_t *
                             __restrict
                             ptTargetMaskSize,
                             arm_2d_size_t *
                             __restrict ptCopySize);

void __arm_2d_impl_cccn888_des_chn_msk_fill(uint32_t * __restrict ptSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *
                        __restrict ptSourceSize,
                        uint32_t * __restrict ptTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *
                        __restrict ptTargetSize,
                        uint32_t *
                        __restrict ptTargetMaskBase,
                        int16_t iTargetMaskStride,
                        arm_2d_size_t *
                        __restrict ptTargetMaskSize);

void __arm_2d_impl_cccn888_des_chn_msk_fill_mirror(uint32_t *
                           __restrict ptSourceBase,
                           int16_t iSourceStride,
                           arm_2d_size_t *
                           __restrict ptSourceSize,
                           uint32_t *
                           __restrict ptTargetBase,
                           int16_t iTargetStride,
                           arm_2d_size_t *
                           __restrict ptTargetSize,
                           uint32_t *
                           __restrict ptTargetMaskBase,
                           int16_t iTargetMaskStride,
                           arm_2d_size_t *
                           __restrict ptTargetMaskSize,
                           uint32_t wMode);

void __arm_2d_impl_cccn888_des_chn_msk_fill_x_mirror(uint32_t *
                             __restrict ptSourceBase,
                             int16_t iSourceStride,
                             arm_2d_size_t *
                             __restrict ptSourceSize,
                             uint32_t *
                             __restrict ptTargetBase,
                             int16_t iTargetStride,
                             arm_2d_size_t *
                             __restrict ptTargetSize,
                             uint32_t *
                             __restrict
                             ptTargetMaskBase,
                             int16_t iTargetMaskStride,
                             arm_2d_size_t *
                             __restrict
                             ptTargetMaskSize);

void __arm_2d_impl_cccn888_des_chn_msk_fill_xy_mirror(uint32_t *
                              __restrict ptSourceBase,
                              int16_t iSourceStride,
                              arm_2d_size_t *
                              __restrict ptSourceSize,
                              uint32_t *
                              __restrict ptTargetBase,
                              int16_t iTargetStride,
                              arm_2d_size_t *
                              __restrict ptTargetSize,
                              uint32_t *
                              __restrict
                              ptTargetMaskBase,
                              int16_t iTargetMaskStride,
                              arm_2d_size_t *
                              __restrict
                              ptTargetMaskSize);

void __arm_2d_impl_cccn888_des_chn_msk_fill_y_mirror(uint32_t *
                             __restrict ptSourceBase,
                             int16_t iSourceStride,
                             arm_2d_size_t *
                             __restrict ptSourceSize,
                             uint32_t *
                             __restrict ptTargetBase,
                             int16_t iTargetStride,
                             arm_2d_size_t *
                             __restrict ptTargetSize,
                             uint32_t *
                             __restrict
                             ptTargetMaskBase,
                             int16_t iTargetMaskStride,
                             arm_2d_size_t *
                             __restrict
                             ptTargetMaskSize);

void __arm_2d_impl_cccn888_des_msk_copy(uint32_t * __restrict pSourceBase,
                    int16_t iSourceStride,
                    uint32_t * __restrict pTargetBase,
                    int16_t iTargetStride,
                    uint8_t * __restrict ptTargetMaskBase,
                    int16_t iTargetMaskStride,
                    arm_2d_size_t *
                    __restrict ptTargetMaskSize,
                    arm_2d_size_t * __restrict ptCopySize);

void __arm_2d_impl_cccn888_des_msk_copy_mirror(uint32_t *
                           __restrict pSourceBase,
                           int16_t iSourceStride,
                           uint32_t *
                           __restrict pTargetBase,
                           int16_t iTargetStride,
                           uint8_t *
                           __restrict ptTargetMaskBase,
                           int16_t iTargetMaskStride,
                           arm_2d_size_t *
                           __restrict ptTargetMaskSize,
                           arm_2d_size_t *
                           __restrict ptCopySize,
                           uint32_t wMode);

void __arm_2d_impl_cccn888_des_msk_copy_x_mirror(uint32_t *
                         __restrict pSourceBase,
                         int16_t iSourceStride,
                         uint32_t *
                         __restrict pTargetBase,
                         int16_t iTargetStride,
                         uint8_t *
                         __restrict ptTargetMaskBase,
                         int16_t iTargetMaskStride,
                         arm_2d_size_t *
                         __restrict ptTargetMaskSize,
                         arm_2d_size_t *
                         __restrict ptCopySize);

void __arm_2d_impl_cccn888_des_msk_copy_xy_mirror(uint32_t *
                          __restrict pSourceBase,
                          int16_t iSourceStride,
                          uint32_t *
                          __restrict pTargetBase,
                          int16_t iTargetStride,
                          uint8_t *
                          __restrict ptTargetMaskBase,
                          int16_t iTargetMaskStride,
                          arm_2d_size_t *
                          __restrict ptTargetMaskSize,
                          arm_2d_size_t *
                          __restrict ptCopySize);

void __arm_2d_impl_cccn888_des_msk_copy_y_mirror(uint32_t *
                         __restrict pSourceBase,
                         int16_t iSourceStride,
                         uint32_t *
                         __restrict pTargetBase,
                         int16_t iTargetStride,
                         uint8_t *
                         __restrict ptTargetMaskBase,
                         int16_t iTargetMaskStride,
                         arm_2d_size_t *
                         __restrict ptTargetMaskSize,
                         arm_2d_size_t *
                         __restrict ptCopySize);

void __arm_2d_impl_cccn888_des_msk_fill(uint32_t * __restrict ptSourceBase,
                    int16_t iSourceStride,
                    arm_2d_size_t * __restrict ptSourceSize,
                    uint32_t * __restrict ptTargetBase,
                    int16_t iTargetStride,
                    arm_2d_size_t * __restrict ptTargetSize,
                    uint8_t * __restrict ptTargetMaskBase,
                    int16_t iTargetMaskStride,
                    arm_2d_size_t *
                    __restrict ptTargetMaskSize);

void __arm_2d_impl_cccn888_des_msk_fill_mirror(uint32_t *
                           __restrict ptSourceBase,
                           int16_t iSourceStride,
                           arm_2d_size_t *
                           __restrict ptSourceSize,
                           uint32_t *
                           __restrict ptTargetBase,
                           int16_t iTargetStride,
                           arm_2d_size_t *
                           __restrict ptTargetSize,
                           uint8_t *
                           __restrict ptTargetMaskBase,
                           int16_t iTargetMaskStride,
                           arm_2d_size_t *
                           __restrict ptTargetMaskSize,
                           uint32_t wMode);

void __arm_2d_impl_cccn888_des_msk_fill_x_mirror(uint32_t *
                         __restrict ptSourceBase,
                         int16_t iSourceStride,
                         arm_2d_size_t *
                         __restrict ptSourceSize,
                         uint32_t *
                         __restrict ptTargetBase,
                         int16_t iTargetStride,
                         arm_2d_size_t *
                         __restrict ptTargetSize,
                         uint8_t *
                         __restrict ptTargetMaskBase,
                         int16_t iTargetMaskStride,
                         arm_2d_size_t *
                         __restrict ptTargetMaskSize);

void __arm_2d_impl_cccn888_des_msk_fill_xy_mirror(uint32_t *
                          __restrict ptSourceBase,
                          int16_t iSourceStride,
                          arm_2d_size_t *
                          __restrict ptSourceSize,
                          uint32_t *
                          __restrict ptTargetBase,
                          int16_t iTargetStride,
                          arm_2d_size_t *
                          __restrict ptTargetSize,
                          uint8_t *
                          __restrict ptTargetMaskBase,
                          int16_t iTargetMaskStride,
                          arm_2d_size_t *
                          __restrict ptTargetMaskSize);

void __arm_2d_impl_cccn888_des_msk_fill_y_mirror(uint32_t *
                         __restrict ptSourceBase,
                         int16_t iSourceStride,
                         arm_2d_size_t *
                         __restrict ptSourceSize,
                         uint32_t *
                         __restrict ptTargetBase,
                         int16_t iTargetStride,
                         arm_2d_size_t *
                         __restrict ptTargetSize,
                         uint8_t *
                         __restrict ptTargetMaskBase,
                         int16_t iTargetMaskStride,
                         arm_2d_size_t *
                         __restrict ptTargetMaskSize);

void __arm_2d_impl_cccn888_masks_copy(uint32_t * __restrict pSourceBase,
                      int16_t iSourceStride,
                      uint8_t * __restrict ptSourceMaskBase,
                      int16_t iSourceMaskStride,
                      arm_2d_size_t *
                      __restrict ptSourceMaskSize,
                      uint32_t * __restrict pTargetBase,
                      int16_t iTargetStride,
                      uint8_t * __restrict ptTargetMaskBase,
                      int16_t iTargetMaskStride,
                      arm_2d_size_t *
                      __restrict ptTargetMaskSize,
                      arm_2d_size_t * __restrict ptCopySize);

void __arm_2d_impl_cccn888_masks_copy_mirror(uint32_t * __restrict pSourceBase,
                         int16_t iSourceStride,
                         uint8_t *
                         __restrict ptSourceMaskBase,
                         int16_t iSourceMaskStride,
                         arm_2d_size_t *
                         __restrict ptSourceMaskSize,
                         uint32_t * __restrict pTargetBase,
                         int16_t iTargetStride,
                         uint8_t *
                         __restrict ptTargetMaskBase,
                         int16_t iTargetMaskStride,
                         arm_2d_size_t *
                         __restrict ptTargetMaskSize,
                         arm_2d_size_t *
                         __restrict ptCopySize,
                         uint32_t wMode);

void __arm_2d_impl_cccn888_masks_copy_x_mirror(uint32_t *
                           __restrict pSourceBase,
                           int16_t iSourceStride,
                           uint8_t *
                           __restrict ptSourceMaskBase,
                           int16_t iSourceMaskStride,
                           arm_2d_size_t *
                           __restrict ptSourceMaskSize,
                           uint32_t *
                           __restrict pTargetBase,
                           int16_t iTargetStride,
                           uint8_t *
                           __restrict ptTargetMaskBase,
                           int16_t iTargetMaskStride,
                           arm_2d_size_t *
                           __restrict ptTargetMaskSize,
                           arm_2d_size_t *
                           __restrict ptCopySize);

void __arm_2d_impl_cccn888_masks_copy_xy_mirror(uint32_t *
                        __restrict pSourceBase,
                        int16_t iSourceStride,
                        uint8_t *
                        __restrict ptSourceMaskBase,
                        int16_t iSourceMaskStride,
                        arm_2d_size_t *
                        __restrict ptSourceMaskSize,
                        uint32_t *
                        __restrict pTargetBase,
                        int16_t iTargetStride,
                        uint8_t *
                        __restrict ptTargetMaskBase,
                        int16_t iTargetMaskStride,
                        arm_2d_size_t *
                        __restrict ptTargetMaskSize,
                        arm_2d_size_t *
                        __restrict ptCopySize);

void __arm_2d_impl_cccn888_masks_copy_y_mirror(uint32_t *
                           __restrict pSourceBase,
                           int16_t iSourceStride,
                           uint8_t *
                           __restrict ptSourceMaskBase,
                           int16_t iSourceMaskStride,
                           arm_2d_size_t *
                           __restrict ptSourceMaskSize,
                           uint32_t *
                           __restrict pTargetBase,
                           int16_t iTargetStride,
                           uint8_t *
                           __restrict ptTargetMaskBase,
                           int16_t iTargetMaskStride,
                           arm_2d_size_t *
                           __restrict ptTargetMaskSize,
                           arm_2d_size_t *
                           __restrict ptCopySize);

void __arm_2d_impl_cccn888_masks_fill(uint32_t * __restrict ptSourceBase,
                      int16_t iSourceStride,
                      arm_2d_size_t * __restrict ptSourceSize,
                      uint8_t * __restrict ptSourceMaskBase,
                      int16_t iSourceMaskStride,
                      arm_2d_size_t *
                      __restrict ptSourceMaskSize,
                      uint32_t * __restrict ptTargetBase,
                      int16_t iTargetStride,
                      arm_2d_size_t * __restrict ptTargetSize,
                      uint8_t * __restrict ptTargetMaskBase,
                      int16_t iTargetMaskStride,
                      arm_2d_size_t *
                      __restrict ptTargetMaskSize);

void __arm_2d_impl_cccn888_masks_fill_mirror(uint32_t * __restrict ptSourceBase,
                         int16_t iSourceStride,
                         arm_2d_size_t *
                         __restrict ptSourceSize,
                         uint8_t *
                         __restrict ptSourceMaskBase,
                         int16_t iSourceMaskStride,
                         arm_2d_size_t *
                         __restrict ptSourceMaskSize,
                         uint32_t * __restrict ptTargetBase,
                         int16_t iTargetStride,
                         arm_2d_size_t *
                         __restrict ptTargetSize,
                         uint8_t *
                         __restrict ptTargetMaskBase,
                         int16_t iTargetMaskStride,
                         arm_2d_size_t *
                         __restrict ptTargetMaskSize,
                         uint32_t wMode);

void __arm_2d_impl_cccn888_masks_fill_x_mirror(uint32_t *
                           __restrict ptSourceBase,
                           int16_t iSourceStride,
                           arm_2d_size_t *
                           __restrict ptSourceSize,
                           uint8_t *
                           __restrict ptSourceMaskBase,
                           int16_t iSourceMaskStride,
                           arm_2d_size_t *
                           __restrict ptSourceMaskSize,
                           uint32_t *
                           __restrict ptTargetBase,
                           int16_t iTargetStride,
                           arm_2d_size_t *
                           __restrict ptTargetSize,
                           uint8_t *
                           __restrict ptTargetMaskBase,
                           int16_t iTargetMaskStride,
                           arm_2d_size_t *
                           __restrict ptTargetMaskSize);

void __arm_2d_impl_cccn888_masks_fill_xy_mirror(uint32_t *
                        __restrict ptSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *
                        __restrict ptSourceSize,
                        uint8_t *
                        __restrict ptSourceMaskBase,
                        int16_t iSourceMaskStride,
                        arm_2d_size_t *
                        __restrict ptSourceMaskSize,
                        uint32_t *
                        __restrict ptTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *
                        __restrict ptTargetSize,
                        uint8_t *
                        __restrict ptTargetMaskBase,
                        int16_t iTargetMaskStride,
                        arm_2d_size_t *
                        __restrict ptTargetMaskSize);

void __arm_2d_impl_cccn888_masks_fill_y_mirror(uint32_t *
                           __restrict ptSourceBase,
                           int16_t iSourceStride,
                           arm_2d_size_t *
                           __restrict ptSourceSize,
                           uint8_t *
                           __restrict ptSourceMaskBase,
                           int16_t iSourceMaskStride,
                           arm_2d_size_t *
                           __restrict ptSourceMaskSize,
                           uint32_t *
                           __restrict ptTargetBase,
                           int16_t iTargetStride,
                           arm_2d_size_t *
                           __restrict ptTargetSize,
                           uint8_t *
                           __restrict ptTargetMaskBase,
                           int16_t iTargetMaskStride,
                           arm_2d_size_t *
                           __restrict ptTargetMaskSize);

void __arm_2d_impl_cccn888_src_chn_msk_1h_des_msk_copy(uint32_t *
                               __restrict pSourceBase,
                               int16_t iSourceStride,
                               uint32_t *
                               __restrict
                               ptSourceMaskBase,
                               int16_t
                               iSourceMaskStride,
                               arm_2d_size_t *
                               __restrict
                               ptSourceMaskSize,
                               uint32_t *
                               __restrict pTargetBase,
                               int16_t iTargetStride,
                               uint8_t *
                               __restrict
                               ptTargetMaskBase,
                               int16_t
                               iTargetMaskStride,
                               arm_2d_size_t *
                               __restrict
                               ptTargetMaskSize,
                               arm_2d_size_t *
                               __restrict ptCopySize);

void __arm_2d_impl_cccn888_src_chn_msk_1h_des_msk_copy_mirror(uint32_t *
                                  __restrict
                                  pSourceBase,
                                  int16_t
                                  iSourceStride,
                                  uint32_t *
                                  __restrict
                                  ptSourceMaskBase,
                                  int16_t
                                  iSourceMaskStride,
                                  arm_2d_size_t *
                                  __restrict
                                  ptSourceMaskSize,
                                  uint32_t *
                                  __restrict
                                  pTargetBase,
                                  int16_t
                                  iTargetStride,
                                  uint8_t *
                                  __restrict
                                  ptTargetMaskBase,
                                  int16_t
                                  iTargetMaskStride,
                                  arm_2d_size_t *
                                  __restrict
                                  ptTargetMaskSize,
                                  arm_2d_size_t *
                                  __restrict
                                  ptCopySize,
                                  uint32_t wMode);

void __arm_2d_impl_cccn888_src_chn_msk_1h_des_msk_copy_x_mirror(uint32_t *
                                __restrict
                                pSourceBase,
                                int16_t
                                iSourceStride,
                                uint32_t *
                                __restrict
                                ptSourceMaskBase,
                                int16_t
                                iSourceMaskStride,
                                arm_2d_size_t *
                                __restrict
                                ptSourceMaskSize,
                                uint32_t *
                                __restrict
                                pTargetBase,
                                int16_t
                                iTargetStride,
                                uint8_t *
                                __restrict
                                ptTargetMaskBase,
                                int16_t
                                iTargetMaskStride,
                                arm_2d_size_t *
                                __restrict
                                ptTargetMaskSize,
                                arm_2d_size_t *
                                __restrict
                                ptCopySize);

void __arm_2d_impl_cccn888_src_chn_msk_1h_des_msk_copy_xy_mirror(uint32_t *
                                 __restrict
                                 pSourceBase,
                                 int16_t
                                 iSourceStride,
                                 uint32_t *
                                 __restrict
                                 ptSourceMaskBase,
                                 int16_t
                                 iSourceMaskStride,
                                 arm_2d_size_t *
                                 __restrict
                                 ptSourceMaskSize,
                                 uint32_t *
                                 __restrict
                                 pTargetBase,
                                 int16_t
                                 iTargetStride,
                                 uint8_t *
                                 __restrict
                                 ptTargetMaskBase,
                                 int16_t
                                 iTargetMaskStride,
                                 arm_2d_size_t *
                                 __restrict
                                 ptTargetMaskSize,
                                 arm_2d_size_t *
                                 __restrict
                                 ptCopySize);

void __arm_2d_impl_cccn888_src_chn_msk_1h_des_msk_copy_y_mirror(uint32_t *
                                __restrict
                                pSourceBase,
                                int16_t
                                iSourceStride,
                                uint32_t *
                                __restrict
                                ptSourceMaskBase,
                                int16_t
                                iSourceMaskStride,
                                arm_2d_size_t *
                                __restrict
                                ptSourceMaskSize,
                                uint32_t *
                                __restrict
                                pTargetBase,
                                int16_t
                                iTargetStride,
                                uint8_t *
                                __restrict
                                ptTargetMaskBase,
                                int16_t
                                iTargetMaskStride,
                                arm_2d_size_t *
                                __restrict
                                ptTargetMaskSize,
                                arm_2d_size_t *
                                __restrict
                                ptCopySize);

void __arm_2d_impl_cccn888_src_chn_msk_1h_des_msk_fill(uint32_t *
                               __restrict ptSourceBase,
                               int16_t iSourceStride,
                               arm_2d_size_t *
                               __restrict ptSourceSize,
                               uint32_t *
                               __restrict
                               ptSourceMaskBase,
                               int16_t
                               iSourceMaskStride,
                               arm_2d_size_t *
                               __restrict
                               ptSourceMaskSize,
                               uint32_t *
                               __restrict ptTargetBase,
                               int16_t iTargetStride,
                               arm_2d_size_t *
                               __restrict ptTargetSize,
                               uint8_t *
                               __restrict
                               ptTargetMaskBase,
                               int16_t
                               iTargetMaskStride,
                               arm_2d_size_t *
                               __restrict
                               ptTargetMaskSize);

void __arm_2d_impl_cccn888_src_chn_msk_1h_des_msk_fill_mirror(uint32_t *
                                  __restrict
                                  ptSourceBase,
                                  int16_t
                                  iSourceStride,
                                  arm_2d_size_t *
                                  __restrict
                                  ptSourceSize,
                                  uint32_t *
                                  __restrict
                                  ptSourceMaskBase,
                                  int16_t
                                  iSourceMaskStride,
                                  arm_2d_size_t *
                                  __restrict
                                  ptSourceMaskSize,
                                  uint32_t *
                                  __restrict
                                  ptTargetBase,
                                  int16_t
                                  iTargetStride,
                                  arm_2d_size_t *
                                  __restrict
                                  ptTargetSize,
                                  uint8_t *
                                  __restrict
                                  ptTargetMaskBase,
                                  int16_t
                                  iTargetMaskStride,
                                  arm_2d_size_t *
                                  __restrict
                                  ptTargetMaskSize,
                                  uint32_t wMode);

void __arm_2d_impl_cccn888_src_chn_msk_1h_des_msk_fill_x_mirror(uint32_t *
                                __restrict
                                ptSourceBase,
                                int16_t
                                iSourceStride,
                                arm_2d_size_t *
                                __restrict
                                ptSourceSize,
                                uint32_t *
                                __restrict
                                ptSourceMaskBase,
                                int16_t
                                iSourceMaskStride,
                                arm_2d_size_t *
                                __restrict
                                ptSourceMaskSize,
                                uint32_t *
                                __restrict
                                ptTargetBase,
                                int16_t
                                iTargetStride,
                                arm_2d_size_t *
                                __restrict
                                ptTargetSize,
                                uint8_t *
                                __restrict
                                ptTargetMaskBase,
                                int16_t
                                iTargetMaskStride,
                                arm_2d_size_t *
                                __restrict
                                ptTargetMaskSize);

void __arm_2d_impl_cccn888_src_chn_msk_1h_des_msk_fill_xy_mirror(uint32_t *
                                 __restrict
                                 ptSourceBase,
                                 int16_t
                                 iSourceStride,
                                 arm_2d_size_t *
                                 __restrict
                                 ptSourceSize,
                                 uint32_t *
                                 __restrict
                                 ptSourceMaskBase,
                                 int16_t
                                 iSourceMaskStride,
                                 arm_2d_size_t *
                                 __restrict
                                 ptSourceMaskSize,
                                 uint32_t *
                                 __restrict
                                 ptTargetBase,
                                 int16_t
                                 iTargetStride,
                                 arm_2d_size_t *
                                 __restrict
                                 ptTargetSize,
                                 uint8_t *
                                 __restrict
                                 ptTargetMaskBase,
                                 int16_t
                                 iTargetMaskStride,
                                 arm_2d_size_t *
                                 __restrict
                                 ptTargetMaskSize);

void __arm_2d_impl_cccn888_src_chn_msk_1h_des_msk_fill_y_mirror(uint32_t *
                                __restrict
                                ptSourceBase,
                                int16_t
                                iSourceStride,
                                arm_2d_size_t *
                                __restrict
                                ptSourceSize,
                                uint32_t *
                                __restrict
                                ptSourceMaskBase,
                                int16_t
                                iSourceMaskStride,
                                arm_2d_size_t *
                                __restrict
                                ptSourceMaskSize,
                                uint32_t *
                                __restrict
                                ptTargetBase,
                                int16_t
                                iTargetStride,
                                arm_2d_size_t *
                                __restrict
                                ptTargetSize,
                                uint8_t *
                                __restrict
                                ptTargetMaskBase,
                                int16_t
                                iTargetMaskStride,
                                arm_2d_size_t *
                                __restrict
                                ptTargetMaskSize);

void __arm_2d_impl_cccn888_src_chn_msk_copy(uint32_t * __restrict pSourceBase,
                        int16_t iSourceStride,
                        uint32_t *
                        __restrict ptSourceMaskBase,
                        int16_t iSourceMaskStride,
                        arm_2d_size_t *
                        __restrict ptSourceMaskSize,
                        uint32_t * __restrict pTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *
                        __restrict ptCopySize);

void __arm_2d_impl_cccn888_src_chn_msk_copy_mirror(uint32_t *
                           __restrict pSourceBase,
                           int16_t iSourceStride,
                           uint32_t *
                           __restrict ptSourceMaskBase,
                           int16_t iSourceMaskStride,
                           arm_2d_size_t *
                           __restrict ptSourceMaskSize,
                           uint32_t *
                           __restrict pTargetBase,
                           int16_t iTargetStride,
                           arm_2d_size_t *
                           __restrict ptCopySize,
                           uint32_t wMode);

void __arm_2d_impl_cccn888_src_chn_msk_copy_x_mirror(uint32_t *
                             __restrict pSourceBase,
                             int16_t iSourceStride,
                             uint32_t *
                             __restrict
                             ptSourceMaskBase,
                             int16_t iSourceMaskStride,
                             arm_2d_size_t *
                             __restrict
                             ptSourceMaskSize,
                             uint32_t *
                             __restrict pTargetBase,
                             int16_t iTargetStride,
                             arm_2d_size_t *
                             __restrict ptCopySize);

void __arm_2d_impl_cccn888_src_chn_msk_copy_xy_mirror(uint32_t *
                              __restrict pSourceBase,
                              int16_t iSourceStride,
                              uint32_t *
                              __restrict
                              ptSourceMaskBase,
                              int16_t iSourceMaskStride,
                              arm_2d_size_t *
                              __restrict
                              ptSourceMaskSize,
                              uint32_t *
                              __restrict pTargetBase,
                              int16_t iTargetStride,
                              arm_2d_size_t *
                              __restrict ptCopySize);

void __arm_2d_impl_cccn888_src_chn_msk_copy_y_mirror(uint32_t *
                             __restrict pSourceBase,
                             int16_t iSourceStride,
                             uint32_t *
                             __restrict
                             ptSourceMaskBase,
                             int16_t iSourceMaskStride,
                             arm_2d_size_t *
                             __restrict
                             ptSourceMaskSize,
                             uint32_t *
                             __restrict pTargetBase,
                             int16_t iTargetStride,
                             arm_2d_size_t *
                             __restrict ptCopySize);

void __arm_2d_impl_cccn888_src_chn_msk_des_chn_msk_copy(uint32_t *
                            __restrict pSourceBase,
                            int16_t iSourceStride,
                            uint32_t *
                            __restrict
                            ptSourceMaskBase,
                            int16_t
                            iSourceMaskStride,
                            arm_2d_size_t *
                            __restrict
                            ptSourceMaskSize,
                            uint32_t *
                            __restrict pTargetBase,
                            int16_t iTargetStride,
                            uint32_t *
                            __restrict
                            ptTargetMaskBase,
                            int16_t
                            iTargetMaskStride,
                            arm_2d_size_t *
                            __restrict
                            ptTargetMaskSize,
                            arm_2d_size_t *
                            __restrict ptCopySize);

void __arm_2d_impl_cccn888_src_chn_msk_des_chn_msk_copy_mirror(uint32_t *
                                   __restrict
                                   pSourceBase,
                                   int16_t
                                   iSourceStride,
                                   uint32_t *
                                   __restrict
                                   ptSourceMaskBase,
                                   int16_t
                                   iSourceMaskStride,
                                   arm_2d_size_t *
                                   __restrict
                                   ptSourceMaskSize,
                                   uint32_t *
                                   __restrict
                                   pTargetBase,
                                   int16_t
                                   iTargetStride,
                                   uint32_t *
                                   __restrict
                                   ptTargetMaskBase,
                                   int16_t
                                   iTargetMaskStride,
                                   arm_2d_size_t *
                                   __restrict
                                   ptTargetMaskSize,
                                   arm_2d_size_t *
                                   __restrict
                                   ptCopySize,
                                   uint32_t wMode);

void __arm_2d_impl_cccn888_src_chn_msk_des_chn_msk_copy_x_mirror(uint32_t *
                                 __restrict
                                 pSourceBase,
                                 int16_t
                                 iSourceStride,
                                 uint32_t *
                                 __restrict
                                 ptSourceMaskBase,
                                 int16_t
                                 iSourceMaskStride,
                                 arm_2d_size_t *
                                 __restrict
                                 ptSourceMaskSize,
                                 uint32_t *
                                 __restrict
                                 pTargetBase,
                                 int16_t
                                 iTargetStride,
                                 uint32_t *
                                 __restrict
                                 ptTargetMaskBase,
                                 int16_t
                                 iTargetMaskStride,
                                 arm_2d_size_t *
                                 __restrict
                                 ptTargetMaskSize,
                                 arm_2d_size_t *
                                 __restrict
                                 ptCopySize);

void __arm_2d_impl_cccn888_src_chn_msk_des_chn_msk_copy_xy_mirror(uint32_t *
                                  __restrict
                                  pSourceBase,
                                  int16_t
                                  iSourceStride,
                                  uint32_t *
                                  __restrict
                                  ptSourceMaskBase,
                                  int16_t
                                  iSourceMaskStride,
                                  arm_2d_size_t
                                  *
                                  __restrict
                                  ptSourceMaskSize,
                                  uint32_t *
                                  __restrict
                                  pTargetBase,
                                  int16_t
                                  iTargetStride,
                                  uint32_t *
                                  __restrict
                                  ptTargetMaskBase,
                                  int16_t
                                  iTargetMaskStride,
                                  arm_2d_size_t
                                  *
                                  __restrict
                                  ptTargetMaskSize,
                                  arm_2d_size_t
                                  *
                                  __restrict
                                  ptCopySize);

void __arm_2d_impl_cccn888_src_chn_msk_des_chn_msk_copy_y_mirror(uint32_t *
                                 __restrict
                                 pSourceBase,
                                 int16_t
                                 iSourceStride,
                                 uint32_t *
                                 __restrict
                                 ptSourceMaskBase,
                                 int16_t
                                 iSourceMaskStride,
                                 arm_2d_size_t *
                                 __restrict
                                 ptSourceMaskSize,
                                 uint32_t *
                                 __restrict
                                 pTargetBase,
                                 int16_t
                                 iTargetStride,
                                 uint32_t *
                                 __restrict
                                 ptTargetMaskBase,
                                 int16_t
                                 iTargetMaskStride,
                                 arm_2d_size_t *
                                 __restrict
                                 ptTargetMaskSize,
                                 arm_2d_size_t *
                                 __restrict
                                 ptCopySize);

void __arm_2d_impl_cccn888_src_chn_msk_des_chn_msk_fill(uint32_t *
                            __restrict ptSourceBase,
                            int16_t iSourceStride,
                            arm_2d_size_t *
                            __restrict ptSourceSize,
                            uint32_t *
                            __restrict
                            ptSourceMaskBase,
                            int16_t
                            iSourceMaskStride,
                            arm_2d_size_t *
                            __restrict
                            ptSourceMaskSize,
                            uint32_t *
                            __restrict ptTargetBase,
                            int16_t iTargetStride,
                            arm_2d_size_t *
                            __restrict ptTargetSize,
                            uint32_t *
                            __restrict
                            ptTargetMaskBase,
                            int16_t
                            iTargetMaskStride,
                            arm_2d_size_t *
                            __restrict
                            ptTargetMaskSize);

void __arm_2d_impl_cccn888_src_chn_msk_des_chn_msk_fill_mirror(uint32_t *
                                   __restrict
                                   ptSourceBase,
                                   int16_t
                                   iSourceStride,
                                   arm_2d_size_t *
                                   __restrict
                                   ptSourceSize,
                                   uint32_t *
                                   __restrict
                                   ptSourceMaskBase,
                                   int16_t
                                   iSourceMaskStride,
                                   arm_2d_size_t *
                                   __restrict
                                   ptSourceMaskSize,
                                   uint32_t *
                                   __restrict
                                   ptTargetBase,
                                   int16_t
                                   iTargetStride,
                                   arm_2d_size_t *
                                   __restrict
                                   ptTargetSize,
                                   uint32_t *
                                   __restrict
                                   ptTargetMaskBase,
                                   int16_t
                                   iTargetMaskStride,
                                   arm_2d_size_t *
                                   __restrict
                                   ptTargetMaskSize,
                                   uint32_t wMode);

void __arm_2d_impl_cccn888_src_chn_msk_des_chn_msk_fill_x_mirror(uint32_t *
                                 __restrict
                                 ptSourceBase,
                                 int16_t
                                 iSourceStride,
                                 arm_2d_size_t *
                                 __restrict
                                 ptSourceSize,
                                 uint32_t *
                                 __restrict
                                 ptSourceMaskBase,
                                 int16_t
                                 iSourceMaskStride,
                                 arm_2d_size_t *
                                 __restrict
                                 ptSourceMaskSize,
                                 uint32_t *
                                 __restrict
                                 ptTargetBase,
                                 int16_t
                                 iTargetStride,
                                 arm_2d_size_t *
                                 __restrict
                                 ptTargetSize,
                                 uint32_t *
                                 __restrict
                                 ptTargetMaskBase,
                                 int16_t
                                 iTargetMaskStride,
                                 arm_2d_size_t *
                                 __restrict
                                 ptTargetMaskSize);

void __arm_2d_impl_cccn888_src_chn_msk_des_chn_msk_fill_xy_mirror(uint32_t *
                                  __restrict
                                  ptSourceBase,
                                  int16_t
                                  iSourceStride,
                                  arm_2d_size_t
                                  *
                                  __restrict
                                  ptSourceSize,
                                  uint32_t *
                                  __restrict
                                  ptSourceMaskBase,
                                  int16_t
                                  iSourceMaskStride,
                                  arm_2d_size_t
                                  *
                                  __restrict
                                  ptSourceMaskSize,
                                  uint32_t *
                                  __restrict
                                  ptTargetBase,
                                  int16_t
                                  iTargetStride,
                                  arm_2d_size_t
                                  *
                                  __restrict
                                  ptTargetSize,
                                  uint32_t *
                                  __restrict
                                  ptTargetMaskBase,
                                  int16_t
                                  iTargetMaskStride,
                                  arm_2d_size_t
                                  *
                                  __restrict
                                  ptTargetMaskSize);

void __arm_2d_impl_cccn888_src_chn_msk_des_chn_msk_fill_y_mirror(uint32_t *
                                 __restrict
                                 ptSourceBase,
                                 int16_t
                                 iSourceStride,
                                 arm_2d_size_t *
                                 __restrict
                                 ptSourceSize,
                                 uint32_t *
                                 __restrict
                                 ptSourceMaskBase,
                                 int16_t
                                 iSourceMaskStride,
                                 arm_2d_size_t *
                                 __restrict
                                 ptSourceMaskSize,
                                 uint32_t *
                                 __restrict
                                 ptTargetBase,
                                 int16_t
                                 iTargetStride,
                                 arm_2d_size_t *
                                 __restrict
                                 ptTargetSize,
                                 uint32_t *
                                 __restrict
                                 ptTargetMaskBase,
                                 int16_t
                                 iTargetMaskStride,
                                 arm_2d_size_t *
                                 __restrict
                                 ptTargetMaskSize);

void __arm_2d_impl_cccn888_src_chn_msk_des_msk_copy(uint32_t *
                            __restrict pSourceBase,
                            int16_t iSourceStride,
                            uint32_t *
                            __restrict ptSourceMaskBase,
                            int16_t iSourceMaskStride,
                            arm_2d_size_t *
                            __restrict ptSourceMaskSize,
                            uint32_t *
                            __restrict pTargetBase,
                            int16_t iTargetStride,
                            uint8_t *
                            __restrict ptTargetMaskBase,
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *
                            __restrict ptTargetMaskSize,
                            arm_2d_size_t *
                            __restrict ptCopySize);

void __arm_2d_impl_cccn888_src_chn_msk_des_msk_copy_mirror(uint32_t *
                               __restrict
                               pSourceBase,
                               int16_t
                               iSourceStride,
                               uint32_t *
                               __restrict
                               ptSourceMaskBase,
                               int16_t
                               iSourceMaskStride,
                               arm_2d_size_t *
                               __restrict
                               ptSourceMaskSize,
                               uint32_t *
                               __restrict
                               pTargetBase,
                               int16_t
                               iTargetStride,
                               uint8_t *
                               __restrict
                               ptTargetMaskBase,
                               int16_t
                               iTargetMaskStride,
                               arm_2d_size_t *
                               __restrict
                               ptTargetMaskSize,
                               arm_2d_size_t *
                               __restrict
                               ptCopySize,
                               uint32_t wMode);

void __arm_2d_impl_cccn888_src_chn_msk_des_msk_copy_x_mirror(uint32_t *
                                 __restrict
                                 pSourceBase,
                                 int16_t
                                 iSourceStride,
                                 uint32_t *
                                 __restrict
                                 ptSourceMaskBase,
                                 int16_t
                                 iSourceMaskStride,
                                 arm_2d_size_t *
                                 __restrict
                                 ptSourceMaskSize,
                                 uint32_t *
                                 __restrict
                                 pTargetBase,
                                 int16_t
                                 iTargetStride,
                                 uint8_t *
                                 __restrict
                                 ptTargetMaskBase,
                                 int16_t
                                 iTargetMaskStride,
                                 arm_2d_size_t *
                                 __restrict
                                 ptTargetMaskSize,
                                 arm_2d_size_t *
                                 __restrict
                                 ptCopySize);

void __arm_2d_impl_cccn888_src_chn_msk_des_msk_copy_xy_mirror(uint32_t *
                                  __restrict
                                  pSourceBase,
                                  int16_t
                                  iSourceStride,
                                  uint32_t *
                                  __restrict
                                  ptSourceMaskBase,
                                  int16_t
                                  iSourceMaskStride,
                                  arm_2d_size_t *
                                  __restrict
                                  ptSourceMaskSize,
                                  uint32_t *
                                  __restrict
                                  pTargetBase,
                                  int16_t
                                  iTargetStride,
                                  uint8_t *
                                  __restrict
                                  ptTargetMaskBase,
                                  int16_t
                                  iTargetMaskStride,
                                  arm_2d_size_t *
                                  __restrict
                                  ptTargetMaskSize,
                                  arm_2d_size_t *
                                  __restrict
                                  ptCopySize);

void __arm_2d_impl_cccn888_src_chn_msk_des_msk_copy_y_mirror(uint32_t *
                                 __restrict
                                 pSourceBase,
                                 int16_t
                                 iSourceStride,
                                 uint32_t *
                                 __restrict
                                 ptSourceMaskBase,
                                 int16_t
                                 iSourceMaskStride,
                                 arm_2d_size_t *
                                 __restrict
                                 ptSourceMaskSize,
                                 uint32_t *
                                 __restrict
                                 pTargetBase,
                                 int16_t
                                 iTargetStride,
                                 uint8_t *
                                 __restrict
                                 ptTargetMaskBase,
                                 int16_t
                                 iTargetMaskStride,
                                 arm_2d_size_t *
                                 __restrict
                                 ptTargetMaskSize,
                                 arm_2d_size_t *
                                 __restrict
                                 ptCopySize);

void __arm_2d_impl_cccn888_src_chn_msk_des_msk_fill(uint32_t *
                            __restrict ptSourceBase,
                            int16_t iSourceStride,
                            arm_2d_size_t *
                            __restrict ptSourceSize,
                            uint32_t *
                            __restrict ptSourceMaskBase,
                            int16_t iSourceMaskStride,
                            arm_2d_size_t *
                            __restrict ptSourceMaskSize,
                            uint32_t *
                            __restrict ptTargetBase,
                            int16_t iTargetStride,
                            arm_2d_size_t *
                            __restrict ptTargetSize,
                            uint8_t *
                            __restrict ptTargetMaskBase,
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *
                            __restrict
                            ptTargetMaskSize);

void __arm_2d_impl_cccn888_src_chn_msk_des_msk_fill_mirror(uint32_t *
                               __restrict
                               ptSourceBase,
                               int16_t
                               iSourceStride,
                               arm_2d_size_t *
                               __restrict
                               ptSourceSize,
                               uint32_t *
                               __restrict
                               ptSourceMaskBase,
                               int16_t
                               iSourceMaskStride,
                               arm_2d_size_t *
                               __restrict
                               ptSourceMaskSize,
                               uint32_t *
                               __restrict
                               ptTargetBase,
                               int16_t
                               iTargetStride,
                               arm_2d_size_t *
                               __restrict
                               ptTargetSize,
                               uint8_t *
                               __restrict
                               ptTargetMaskBase,
                               int16_t
                               iTargetMaskStride,
                               arm_2d_size_t *
                               __restrict
                               ptTargetMaskSize,
                               uint32_t wMode);

void __arm_2d_impl_cccn888_src_chn_msk_des_msk_fill_x_mirror(uint32_t *
                                                             __restrict
                                                             ptSourceBase,
                                                             int16_t
                                                             iSourceStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptSourceSize,
                                                             uint32_t *
                                                             __restrict
                                                             ptSourceMaskBase,
                                                             int16_t
                                                             iSourceMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptSourceMaskSize,
                                                             uint32_t *
                                                             __restrict
                                                             ptTargetBase,
                                                             int16_t
                                                             iTargetStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptTargetSize,
                                                             uint8_t *
                                                             __restrict
                                                             ptTargetMaskBase,
                                                             int16_t
                                                             iTargetMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptTargetMaskSize);

void __arm_2d_impl_cccn888_src_chn_msk_des_msk_fill_xy_mirror(uint32_t *
                                                              __restrict
                                                              ptSourceBase,
                                                              int16_t
                                                              iSourceStride,
                                                              arm_2d_size_t *
                                                              __restrict
                                                              ptSourceSize,
                                                              uint32_t *
                                                              __restrict
                                                              ptSourceMaskBase,
                                                              int16_t
                                                              iSourceMaskStride,
                                                              arm_2d_size_t *
                                                              __restrict
                                                              ptSourceMaskSize,
                                                              uint32_t *
                                                              __restrict
                                                              ptTargetBase,
                                                              int16_t
                                                              iTargetStride,
                                                              arm_2d_size_t *
                                                              __restrict
                                                              ptTargetSize,
                                                              uint8_t *
                                                              __restrict
                                                              ptTargetMaskBase,
                                                              int16_t
                                                              iTargetMaskStride,
                                                              arm_2d_size_t *
                                                              __restrict
                                                              ptTargetMaskSize);

void __arm_2d_impl_cccn888_src_chn_msk_des_msk_fill_y_mirror(uint32_t *
                                                             __restrict
                                                             ptSourceBase,
                                                             int16_t
                                                             iSourceStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptSourceSize,
                                                             uint32_t *
                                                             __restrict
                                                             ptSourceMaskBase,
                                                             int16_t
                                                             iSourceMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptSourceMaskSize,
                                                             uint32_t *
                                                             __restrict
                                                             ptTargetBase,
                                                             int16_t
                                                             iTargetStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptTargetSize,
                                                             uint8_t *
                                                             __restrict
                                                             ptTargetMaskBase,
                                                             int16_t
                                                             iTargetMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptTargetMaskSize);

void __arm_2d_impl_cccn888_src_chn_msk_fill(uint32_t * __restrict ptSourceBase,
                                            int16_t iSourceStride,
                                            arm_2d_size_t *
                                            __restrict ptSourceSize,
                                            uint32_t *
                                            __restrict ptSourceMaskBase,
                                            int16_t iSourceMaskStride,
                                            arm_2d_size_t *
                                            __restrict ptSourceMaskSize,
                                            uint32_t * __restrict ptTargetBase,
                                            int16_t iTargetStride,
                                            arm_2d_size_t *
                                            __restrict ptTargetSize);

void __arm_2d_impl_cccn888_src_chn_msk_fill_mirror(uint32_t *
                                                   __restrict ptSourceBase,
                                                   int16_t iSourceStride,
                                                   arm_2d_size_t *
                                                   __restrict ptSourceSize,
                                                   uint32_t *
                                                   __restrict ptSourceMaskBase,
                                                   int16_t iSourceMaskStride,
                                                   arm_2d_size_t *
                                                   __restrict ptSourceMaskSize,
                                                   uint32_t *
                                                   __restrict ptTargetBase,
                                                   int16_t iTargetStride,
                                                   arm_2d_size_t *
                                                   __restrict ptTargetSize,
                                                   uint32_t wMode);

void __arm_2d_impl_cccn888_src_chn_msk_fill_x_mirror(uint32_t *
                                                     __restrict ptSourceBase,
                                                     int16_t iSourceStride,
                                                     arm_2d_size_t *
                                                     __restrict ptSourceSize,
                                                     uint32_t *
                                                     __restrict
                                                     ptSourceMaskBase,
                                                     int16_t iSourceMaskStride,
                                                     arm_2d_size_t *
                                                     __restrict
                                                     ptSourceMaskSize,
                                                     uint32_t *
                                                     __restrict ptTargetBase,
                                                     int16_t iTargetStride,
                                                     arm_2d_size_t *
                                                     __restrict ptTargetSize);

void __arm_2d_impl_cccn888_src_chn_msk_fill_xy_mirror(uint32_t *
                                                      __restrict ptSourceBase,
                                                      int16_t iSourceStride,
                                                      arm_2d_size_t *
                                                      __restrict ptSourceSize,
                                                      uint32_t *
                                                      __restrict
                                                      ptSourceMaskBase,
                                                      int16_t iSourceMaskStride,
                                                      arm_2d_size_t *
                                                      __restrict
                                                      ptSourceMaskSize,
                                                      uint32_t *
                                                      __restrict ptTargetBase,
                                                      int16_t iTargetStride,
                                                      arm_2d_size_t *
                                                      __restrict ptTargetSize);

void __arm_2d_impl_cccn888_src_chn_msk_fill_y_mirror(uint32_t *
                                                     __restrict ptSourceBase,
                                                     int16_t iSourceStride,
                                                     arm_2d_size_t *
                                                     __restrict ptSourceSize,
                                                     uint32_t *
                                                     __restrict
                                                     ptSourceMaskBase,
                                                     int16_t iSourceMaskStride,
                                                     arm_2d_size_t *
                                                     __restrict
                                                     ptSourceMaskSize,
                                                     uint32_t *
                                                     __restrict ptTargetBase,
                                                     int16_t iTargetStride,
                                                     arm_2d_size_t *
                                                     __restrict ptTargetSize);

void __arm_2d_impl_cccn888_src_msk_1h_des_msk_copy(uint32_t *
                                                   __restrict pSourceBase,
                                                   int16_t iSourceStride,
                                                   uint8_t *
                                                   __restrict ptSourceMaskBase,
                                                   int16_t iSourceMaskStride,
                                                   arm_2d_size_t *
                                                   __restrict ptSourceMaskSize,
                                                   uint32_t *
                                                   __restrict pTargetBase,
                                                   int16_t iTargetStride,
                                                   uint8_t *
                                                   __restrict ptTargetMaskBase,
                                                   int16_t iTargetMaskStride,
                                                   arm_2d_size_t *
                                                   __restrict ptTargetMaskSize,
                                                   arm_2d_size_t *
                                                   __restrict ptCopySize);

void __arm_2d_impl_cccn888_src_msk_1h_des_msk_copy_mirror(uint32_t *
                                                          __restrict
                                                          pSourceBase,
                                                          int16_t iSourceStride,
                                                          uint8_t *
                                                          __restrict
                                                          ptSourceMaskBase,
                                                          int16_t
                                                          iSourceMaskStride,
                                                          arm_2d_size_t *
                                                          __restrict
                                                          ptSourceMaskSize,
                                                          uint32_t *
                                                          __restrict
                                                          pTargetBase,
                                                          int16_t iTargetStride,
                                                          uint8_t *
                                                          __restrict
                                                          ptTargetMaskBase,
                                                          int16_t
                                                          iTargetMaskStride,
                                                          arm_2d_size_t *
                                                          __restrict
                                                          ptTargetMaskSize,
                                                          arm_2d_size_t *
                                                          __restrict ptCopySize,
                                                          uint32_t wMode);

void __arm_2d_impl_cccn888_src_msk_1h_des_msk_copy_x_mirror(uint32_t *
                                                            __restrict
                                                            pSourceBase,
                                                            int16_t
                                                            iSourceStride,
                                                            uint8_t *
                                                            __restrict
                                                            ptSourceMaskBase,
                                                            int16_t
                                                            iSourceMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceMaskSize,
                                                            uint32_t *
                                                            __restrict
                                                            pTargetBase,
                                                            int16_t
                                                            iTargetStride,
                                                            uint8_t *
                                                            __restrict
                                                            ptTargetMaskBase,
                                                            int16_t
                                                            iTargetMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetMaskSize,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptCopySize);

void __arm_2d_impl_cccn888_src_msk_1h_des_msk_copy_xy_mirror(uint32_t *
                                                             __restrict
                                                             pSourceBase,
                                                             int16_t
                                                             iSourceStride,
                                                             uint8_t *
                                                             __restrict
                                                             ptSourceMaskBase,
                                                             int16_t
                                                             iSourceMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptSourceMaskSize,
                                                             uint32_t *
                                                             __restrict
                                                             pTargetBase,
                                                             int16_t
                                                             iTargetStride,
                                                             uint8_t *
                                                             __restrict
                                                             ptTargetMaskBase,
                                                             int16_t
                                                             iTargetMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptTargetMaskSize,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptCopySize);

void __arm_2d_impl_cccn888_src_msk_1h_des_msk_copy_y_mirror(uint32_t *
                                                            __restrict
                                                            pSourceBase,
                                                            int16_t
                                                            iSourceStride,
                                                            uint8_t *
                                                            __restrict
                                                            ptSourceMaskBase,
                                                            int16_t
                                                            iSourceMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceMaskSize,
                                                            uint32_t *
                                                            __restrict
                                                            pTargetBase,
                                                            int16_t
                                                            iTargetStride,
                                                            uint8_t *
                                                            __restrict
                                                            ptTargetMaskBase,
                                                            int16_t
                                                            iTargetMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetMaskSize,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptCopySize);

void __arm_2d_impl_cccn888_src_msk_1h_des_msk_fill(uint32_t *
                                                   __restrict ptSourceBase,
                                                   int16_t iSourceStride,
                                                   arm_2d_size_t *
                                                   __restrict ptSourceSize,
                                                   uint8_t *
                                                   __restrict ptSourceMaskBase,
                                                   int16_t iSourceMaskStride,
                                                   arm_2d_size_t *
                                                   __restrict ptSourceMaskSize,
                                                   uint32_t *
                                                   __restrict ptTargetBase,
                                                   int16_t iTargetStride,
                                                   arm_2d_size_t *
                                                   __restrict ptTargetSize,
                                                   uint8_t *
                                                   __restrict ptTargetMaskBase,
                                                   int16_t iTargetMaskStride,
                                                   arm_2d_size_t *
                                                   __restrict ptTargetMaskSize);

void __arm_2d_impl_cccn888_src_msk_1h_des_msk_fill_mirror(uint32_t *
                                                          __restrict
                                                          ptSourceBase,
                                                          int16_t iSourceStride,
                                                          arm_2d_size_t *
                                                          __restrict
                                                          ptSourceSize,
                                                          uint8_t *
                                                          __restrict
                                                          ptSourceMaskBase,
                                                          int16_t
                                                          iSourceMaskStride,
                                                          arm_2d_size_t *
                                                          __restrict
                                                          ptSourceMaskSize,
                                                          uint32_t *
                                                          __restrict
                                                          ptTargetBase,
                                                          int16_t iTargetStride,
                                                          arm_2d_size_t *
                                                          __restrict
                                                          ptTargetSize,
                                                          uint8_t *
                                                          __restrict
                                                          ptTargetMaskBase,
                                                          int16_t
                                                          iTargetMaskStride,
                                                          arm_2d_size_t *
                                                          __restrict
                                                          ptTargetMaskSize,
                                                          uint32_t wMode);

void __arm_2d_impl_cccn888_src_msk_1h_des_msk_fill_x_mirror(uint32_t *
                                                            __restrict
                                                            ptSourceBase,
                                                            int16_t
                                                            iSourceStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceSize,
                                                            uint8_t *
                                                            __restrict
                                                            ptSourceMaskBase,
                                                            int16_t
                                                            iSourceMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceMaskSize,
                                                            uint32_t *
                                                            __restrict
                                                            ptTargetBase,
                                                            int16_t
                                                            iTargetStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetSize,
                                                            uint8_t *
                                                            __restrict
                                                            ptTargetMaskBase,
                                                            int16_t
                                                            iTargetMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetMaskSize);

void __arm_2d_impl_cccn888_src_msk_1h_des_msk_fill_xy_mirror(uint32_t *
                                                             __restrict
                                                             ptSourceBase,
                                                             int16_t
                                                             iSourceStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptSourceSize,
                                                             uint8_t *
                                                             __restrict
                                                             ptSourceMaskBase,
                                                             int16_t
                                                             iSourceMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptSourceMaskSize,
                                                             uint32_t *
                                                             __restrict
                                                             ptTargetBase,
                                                             int16_t
                                                             iTargetStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptTargetSize,
                                                             uint8_t *
                                                             __restrict
                                                             ptTargetMaskBase,
                                                             int16_t
                                                             iTargetMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptTargetMaskSize);

void __arm_2d_impl_cccn888_src_msk_1h_des_msk_fill_y_mirror(uint32_t *
                                                            __restrict
                                                            ptSourceBase,
                                                            int16_t
                                                            iSourceStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceSize,
                                                            uint8_t *
                                                            __restrict
                                                            ptSourceMaskBase,
                                                            int16_t
                                                            iSourceMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceMaskSize,
                                                            uint32_t *
                                                            __restrict
                                                            ptTargetBase,
                                                            int16_t
                                                            iTargetStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetSize,
                                                            uint8_t *
                                                            __restrict
                                                            ptTargetMaskBase,
                                                            int16_t
                                                            iTargetMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetMaskSize);

void __arm_2d_impl_cccn888_src_msk_copy(uint32_t * __restrict pSourceBase,
                                        int16_t iSourceStride,
                                        uint8_t * __restrict ptSourceMaskBase,
                                        int16_t iSourceMaskStride,
                                        arm_2d_size_t *
                                        __restrict ptSourceMaskSize,
                                        uint32_t * __restrict pTargetBase,
                                        int16_t iTargetStride,
                                        arm_2d_size_t * __restrict ptCopySize);

void __arm_2d_impl_cccn888_src_msk_copy_mirror(uint32_t *
                                               __restrict pSourceBase,
                                               int16_t iSourceStride,
                                               uint8_t *
                                               __restrict ptSourceMaskBase,
                                               int16_t iSourceMaskStride,
                                               arm_2d_size_t *
                                               __restrict ptSourceMaskSize,
                                               uint32_t *
                                               __restrict pTargetBase,
                                               int16_t iTargetStride,
                                               arm_2d_size_t *
                                               __restrict ptCopySize,
                                               uint32_t wMode);

void __arm_2d_impl_cccn888_src_msk_copy_x_mirror(uint32_t *
                                                 __restrict pSourceBase,
                                                 int16_t iSourceStride,
                                                 uint8_t *
                                                 __restrict ptSourceMaskBase,
                                                 int16_t iSourceMaskStride,
                                                 arm_2d_size_t *
                                                 __restrict ptSourceMaskSize,
                                                 uint32_t *
                                                 __restrict pTargetBase,
                                                 int16_t iTargetStride,
                                                 arm_2d_size_t *
                                                 __restrict ptCopySize);

void __arm_2d_impl_cccn888_src_msk_copy_xy_mirror(uint32_t *
                                                  __restrict pSourceBase,
                                                  int16_t iSourceStride,
                                                  uint8_t *
                                                  __restrict ptSourceMaskBase,
                                                  int16_t iSourceMaskStride,
                                                  arm_2d_size_t *
                                                  __restrict ptSourceMaskSize,
                                                  uint32_t *
                                                  __restrict pTargetBase,
                                                  int16_t iTargetStride,
                                                  arm_2d_size_t *
                                                  __restrict ptCopySize);

void __arm_2d_impl_cccn888_src_msk_copy_y_mirror(uint32_t *
                                                 __restrict pSourceBase,
                                                 int16_t iSourceStride,
                                                 uint8_t *
                                                 __restrict ptSourceMaskBase,
                                                 int16_t iSourceMaskStride,
                                                 arm_2d_size_t *
                                                 __restrict ptSourceMaskSize,
                                                 uint32_t *
                                                 __restrict pTargetBase,
                                                 int16_t iTargetStride,
                                                 arm_2d_size_t *
                                                 __restrict ptCopySize);

void __arm_2d_impl_cccn888_src_msk_des_chn_msk_copy(uint32_t *
                                                    __restrict pSourceBase,
                                                    int16_t iSourceStride,
                                                    uint8_t *
                                                    __restrict ptSourceMaskBase,
                                                    int16_t iSourceMaskStride,
                                                    arm_2d_size_t *
                                                    __restrict ptSourceMaskSize,
                                                    uint32_t *
                                                    __restrict pTargetBase,
                                                    int16_t iTargetStride,
                                                    uint32_t *
                                                    __restrict ptTargetMaskBase,
                                                    int16_t iTargetMaskStride,
                                                    arm_2d_size_t *
                                                    __restrict ptTargetMaskSize,
                                                    arm_2d_size_t *
                                                    __restrict ptCopySize);

void __arm_2d_impl_cccn888_src_msk_des_chn_msk_copy_mirror(uint32_t *
                                                           __restrict
                                                           pSourceBase,
                                                           int16_t
                                                           iSourceStride,
                                                           uint8_t *
                                                           __restrict
                                                           ptSourceMaskBase,
                                                           int16_t
                                                           iSourceMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptSourceMaskSize,
                                                           uint32_t *
                                                           __restrict
                                                           pTargetBase,
                                                           int16_t
                                                           iTargetStride,
                                                           uint32_t *
                                                           __restrict
                                                           ptTargetMaskBase,
                                                           int16_t
                                                           iTargetMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptTargetMaskSize,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptCopySize,
                                                           uint32_t wMode);

void __arm_2d_impl_cccn888_src_msk_des_chn_msk_copy_x_mirror(uint32_t *
                                                             __restrict
                                                             pSourceBase,
                                                             int16_t
                                                             iSourceStride,
                                                             uint8_t *
                                                             __restrict
                                                             ptSourceMaskBase,
                                                             int16_t
                                                             iSourceMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptSourceMaskSize,
                                                             uint32_t *
                                                             __restrict
                                                             pTargetBase,
                                                             int16_t
                                                             iTargetStride,
                                                             uint32_t *
                                                             __restrict
                                                             ptTargetMaskBase,
                                                             int16_t
                                                             iTargetMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptTargetMaskSize,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptCopySize);

void __arm_2d_impl_cccn888_src_msk_des_chn_msk_copy_xy_mirror(uint32_t *
                                                              __restrict
                                                              pSourceBase,
                                                              int16_t
                                                              iSourceStride,
                                                              uint8_t *
                                                              __restrict
                                                              ptSourceMaskBase,
                                                              int16_t
                                                              iSourceMaskStride,
                                                              arm_2d_size_t *
                                                              __restrict
                                                              ptSourceMaskSize,
                                                              uint32_t *
                                                              __restrict
                                                              pTargetBase,
                                                              int16_t
                                                              iTargetStride,
                                                              uint32_t *
                                                              __restrict
                                                              ptTargetMaskBase,
                                                              int16_t
                                                              iTargetMaskStride,
                                                              arm_2d_size_t *
                                                              __restrict
                                                              ptTargetMaskSize,
                                                              arm_2d_size_t *
                                                              __restrict
                                                              ptCopySize);

void __arm_2d_impl_cccn888_src_msk_des_chn_msk_copy_y_mirror(uint32_t *
                                                             __restrict
                                                             pSourceBase,
                                                             int16_t
                                                             iSourceStride,
                                                             uint8_t *
                                                             __restrict
                                                             ptSourceMaskBase,
                                                             int16_t
                                                             iSourceMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptSourceMaskSize,
                                                             uint32_t *
                                                             __restrict
                                                             pTargetBase,
                                                             int16_t
                                                             iTargetStride,
                                                             uint32_t *
                                                             __restrict
                                                             ptTargetMaskBase,
                                                             int16_t
                                                             iTargetMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptTargetMaskSize,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptCopySize);

void __arm_2d_impl_cccn888_src_msk_des_chn_msk_fill(uint32_t *
                                                    __restrict ptSourceBase,
                                                    int16_t iSourceStride,
                                                    arm_2d_size_t *
                                                    __restrict ptSourceSize,
                                                    uint8_t *
                                                    __restrict ptSourceMaskBase,
                                                    int16_t iSourceMaskStride,
                                                    arm_2d_size_t *
                                                    __restrict ptSourceMaskSize,
                                                    uint32_t *
                                                    __restrict ptTargetBase,
                                                    int16_t iTargetStride,
                                                    arm_2d_size_t *
                                                    __restrict ptTargetSize,
                                                    uint32_t *
                                                    __restrict ptTargetMaskBase,
                                                    int16_t iTargetMaskStride,
                                                    arm_2d_size_t *
                                                    __restrict
                                                    ptTargetMaskSize);

void __arm_2d_impl_cccn888_src_msk_des_chn_msk_fill_mirror(uint32_t *
                                                           __restrict
                                                           ptSourceBase,
                                                           int16_t
                                                           iSourceStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptSourceSize,
                                                           uint8_t *
                                                           __restrict
                                                           ptSourceMaskBase,
                                                           int16_t
                                                           iSourceMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptSourceMaskSize,
                                                           uint32_t *
                                                           __restrict
                                                           ptTargetBase,
                                                           int16_t
                                                           iTargetStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptTargetSize,
                                                           uint32_t *
                                                           __restrict
                                                           ptTargetMaskBase,
                                                           int16_t
                                                           iTargetMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptTargetMaskSize,
                                                           uint32_t wMode);

void __arm_2d_impl_cccn888_src_msk_des_chn_msk_fill_x_mirror(uint32_t *
                                                             __restrict
                                                             ptSourceBase,
                                                             int16_t
                                                             iSourceStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptSourceSize,
                                                             uint8_t *
                                                             __restrict
                                                             ptSourceMaskBase,
                                                             int16_t
                                                             iSourceMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptSourceMaskSize,
                                                             uint32_t *
                                                             __restrict
                                                             ptTargetBase,
                                                             int16_t
                                                             iTargetStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptTargetSize,
                                                             uint32_t *
                                                             __restrict
                                                             ptTargetMaskBase,
                                                             int16_t
                                                             iTargetMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptTargetMaskSize);

void __arm_2d_impl_cccn888_src_msk_des_chn_msk_fill_xy_mirror(uint32_t *
                                                              __restrict
                                                              ptSourceBase,
                                                              int16_t
                                                              iSourceStride,
                                                              arm_2d_size_t *
                                                              __restrict
                                                              ptSourceSize,
                                                              uint8_t *
                                                              __restrict
                                                              ptSourceMaskBase,
                                                              int16_t
                                                              iSourceMaskStride,
                                                              arm_2d_size_t *
                                                              __restrict
                                                              ptSourceMaskSize,
                                                              uint32_t *
                                                              __restrict
                                                              ptTargetBase,
                                                              int16_t
                                                              iTargetStride,
                                                              arm_2d_size_t *
                                                              __restrict
                                                              ptTargetSize,
                                                              uint32_t *
                                                              __restrict
                                                              ptTargetMaskBase,
                                                              int16_t
                                                              iTargetMaskStride,
                                                              arm_2d_size_t *
                                                              __restrict
                                                              ptTargetMaskSize);

void __arm_2d_impl_cccn888_src_msk_des_chn_msk_fill_y_mirror(uint32_t *
                                                             __restrict
                                                             ptSourceBase,
                                                             int16_t
                                                             iSourceStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptSourceSize,
                                                             uint8_t *
                                                             __restrict
                                                             ptSourceMaskBase,
                                                             int16_t
                                                             iSourceMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptSourceMaskSize,
                                                             uint32_t *
                                                             __restrict
                                                             ptTargetBase,
                                                             int16_t
                                                             iTargetStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptTargetSize,
                                                             uint32_t *
                                                             __restrict
                                                             ptTargetMaskBase,
                                                             int16_t
                                                             iTargetMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptTargetMaskSize);

void __arm_2d_impl_cccn888_src_msk_fill(uint32_t * __restrict ptSourceBase,
                                        int16_t iSourceStride,
                                        arm_2d_size_t * __restrict ptSourceSize,
                                        uint8_t * __restrict ptSourceMaskBase,
                                        int16_t iSourceMaskStride,
                                        arm_2d_size_t *
                                        __restrict ptSourceMaskSize,
                                        uint32_t * __restrict ptTargetBase,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *
                                        __restrict ptTargetSize);

void __arm_2d_impl_cccn888_src_msk_fill_mirror(uint32_t *
                                               __restrict ptSourceBase,
                                               int16_t iSourceStride,
                                               arm_2d_size_t *
                                               __restrict ptSourceSize,
                                               uint8_t *
                                               __restrict ptSourceMaskBase,
                                               int16_t iSourceMaskStride,
                                               arm_2d_size_t *
                                               __restrict ptSourceMaskSize,
                                               uint32_t *
                                               __restrict ptTargetBase,
                                               int16_t iTargetStride,
                                               arm_2d_size_t *
                                               __restrict ptTargetSize,
                                               uint32_t wMode);

void __arm_2d_impl_cccn888_src_msk_fill_x_mirror(uint32_t *
                                                 __restrict ptSourceBase,
                                                 int16_t iSourceStride,
                                                 arm_2d_size_t *
                                                 __restrict ptSourceSize,
                                                 uint8_t *
                                                 __restrict ptSourceMaskBase,
                                                 int16_t iSourceMaskStride,
                                                 arm_2d_size_t *
                                                 __restrict ptSourceMaskSize,
                                                 uint32_t *
                                                 __restrict ptTargetBase,
                                                 int16_t iTargetStride,
                                                 arm_2d_size_t *
                                                 __restrict ptTargetSize);

void __arm_2d_impl_cccn888_src_msk_fill_xy_mirror(  uint32_t *
                                                    __restrict ptSourceBase,
                                                    int16_t iSourceStride,
                                                    arm_2d_size_t *
                                                    __restrict ptSourceSize,
                                                    uint8_t *
                                                    __restrict ptSourceMaskBase,
                                                    int16_t iSourceMaskStride,
                                                    arm_2d_size_t *
                                                    __restrict ptSourceMaskSize,
                                                    uint32_t *
                                                    __restrict ptTargetBase,
                                                    int16_t iTargetStride,
                                                    arm_2d_size_t *
                                                    __restrict ptTargetSize);

void __arm_2d_impl_cccn888_src_msk_fill_y_mirror(   uint32_t *
                                                    __restrict ptSourceBase,
                                                    int16_t iSourceStride,
                                                    arm_2d_size_t *
                                                    __restrict ptSourceSize,
                                                    uint8_t *
                                                    __restrict ptSourceMaskBase,
                                                    int16_t iSourceMaskStride,
                                                    arm_2d_size_t *
                                                    __restrict ptSourceMaskSize,
                                                    uint32_t *
                                                    __restrict ptTargetBase,
                                                    int16_t iTargetStride,
                                                    arm_2d_size_t *
                                                    __restrict ptTargetSize);

void __arm_2d_impl_gray8_to_rgb565( uint8_t *__RESTRICT pchSourceBase,
                                    int16_t iSourceStride,
                                    uint16_t *__RESTRICT phwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize);

void __arm_2d_impl_cccn888_to_rgb565(uint32_t * __restrict pwSourceBase,
                                     int16_t iSourceStride,
                                     uint16_t * __restrict phwTargetBase,
                                     int16_t iTargetStride,
                                     arm_2d_size_t * __restrict ptCopySize);

void __arm_2d_impl_cccn888_transform(__arm_2d_param_copy_orig_t * ptParam,
                                     __arm_2d_transform_info_t * ptInfo);

void __arm_2d_impl_cccn888_transform_with_opacity(
                                           __arm_2d_param_copy_orig_t * ptParam,
                                           __arm_2d_transform_info_t * ptInfo,
                                           uint_fast16_t hwRatio);
                       
void __arm_2d_impl_cccn888_transform_with_src_mask(
                                        __arm_2d_param_copy_orig_msk_t * ptParam,
                                        __arm_2d_transform_info_t * ptInfo);

void __arm_2d_impl_cccn888_transform_with_src_mask_and_opacity(
                                        __arm_2d_param_copy_orig_msk_t * ptParam,
                                        __arm_2d_transform_info_t * ptInfo,
                                        uint_fast16_t hwRatio);

void __arm_2d_impl_cccn888_transform_with_src_chn_mask(
                                        __arm_2d_param_copy_orig_msk_t * ptParam,
                                        __arm_2d_transform_info_t * ptInfo);

void __arm_2d_impl_cccn888_transform_with_src_chn_mask_and_opacity(
                                        __arm_2d_param_copy_orig_msk_t * ptParam,
                                        __arm_2d_transform_info_t * ptInfo,
                                        uint_fast16_t hwRatio);

void __arm_2d_impl_gray8_1h_des_msk_copy(uint8_t * __restrict pSourceBase,
                                         int16_t iSourceStride,
                                         uint8_t * __restrict pTargetBase,
                                         int16_t iTargetStride,
                                         uint8_t * __restrict ptTargetMaskBase,
                                         int16_t iTargetMaskStride,
                                         arm_2d_size_t *
                                         __restrict ptTargetMaskSize,
                                         arm_2d_size_t * __restrict ptCopySize);

void __arm_2d_impl_gray8_1h_des_msk_copy_mirror(uint8_t *
                                                __restrict pSourceBase,
                                                int16_t iSourceStride,
                                                uint8_t *
                                                __restrict pTargetBase,
                                                int16_t iTargetStride,
                                                uint8_t *
                                                __restrict ptTargetMaskBase,
                                                int16_t iTargetMaskStride,
                                                arm_2d_size_t *
                                                __restrict ptTargetMaskSize,
                                                arm_2d_size_t *
                                                __restrict ptCopySize,
                                                uint32_t wMode);

void __arm_2d_impl_gray8_1h_des_msk_copy_x_mirror(uint8_t *
                                                  __restrict pSourceBase,
                                                  int16_t iSourceStride,
                                                  uint8_t *
                                                  __restrict pTargetBase,
                                                  int16_t iTargetStride,
                                                  uint8_t *
                                                  __restrict ptTargetMaskBase,
                                                  int16_t iTargetMaskStride,
                                                  arm_2d_size_t *
                                                  __restrict ptTargetMaskSize,
                                                  arm_2d_size_t *
                                                  __restrict ptCopySize);

void __arm_2d_impl_gray8_1h_des_msk_copy_xy_mirror(uint8_t *
                                                   __restrict pSourceBase,
                                                   int16_t iSourceStride,
                                                   uint8_t *
                                                   __restrict pTargetBase,
                                                   int16_t iTargetStride,
                                                   uint8_t *
                                                   __restrict ptTargetMaskBase,
                                                   int16_t iTargetMaskStride,
                                                   arm_2d_size_t *
                                                   __restrict ptTargetMaskSize,
                                                   arm_2d_size_t *
                                                   __restrict ptCopySize);

void __arm_2d_impl_gray8_1h_des_msk_copy_y_mirror(uint8_t *
                                                  __restrict pSourceBase,
                                                  int16_t iSourceStride,
                                                  uint8_t *
                                                  __restrict pTargetBase,
                                                  int16_t iTargetStride,
                                                  uint8_t *
                                                  __restrict ptTargetMaskBase,
                                                  int16_t iTargetMaskStride,
                                                  arm_2d_size_t *
                                                  __restrict ptTargetMaskSize,
                                                  arm_2d_size_t *
                                                  __restrict ptCopySize);

void __arm_2d_impl_gray8_1h_des_msk_fill(uint8_t * __restrict ptSourceBase,
                                         int16_t iSourceStride,
                                         arm_2d_size_t *
                                         __restrict ptSourceSize,
                                         uint8_t * __restrict ptTargetBase,
                                         int16_t iTargetStride,
                                         arm_2d_size_t *
                                         __restrict ptTargetSize,
                                         uint8_t * __restrict ptTargetMaskBase,
                                         int16_t iTargetMaskStride,
                                         arm_2d_size_t *
                                         __restrict ptTargetMaskSize);

void __arm_2d_impl_gray8_1h_des_msk_fill_mirror(uint8_t *
                                                __restrict ptSourceBase,
                                                int16_t iSourceStride,
                                                arm_2d_size_t *
                                                __restrict ptSourceSize,
                                                uint8_t *
                                                __restrict ptTargetBase,
                                                int16_t iTargetStride,
                                                arm_2d_size_t *
                                                __restrict ptTargetSize,
                                                uint8_t *
                                                __restrict ptTargetMaskBase,
                                                int16_t iTargetMaskStride,
                                                arm_2d_size_t *
                                                __restrict ptTargetMaskSize,
                                                uint32_t wMode);

void __arm_2d_impl_gray8_1h_des_msk_fill_x_mirror(uint8_t *
                                                  __restrict ptSourceBase,
                                                  int16_t iSourceStride,
                                                  arm_2d_size_t *
                                                  __restrict ptSourceSize,
                                                  uint8_t *
                                                  __restrict ptTargetBase,
                                                  int16_t iTargetStride,
                                                  arm_2d_size_t *
                                                  __restrict ptTargetSize,
                                                  uint8_t *
                                                  __restrict ptTargetMaskBase,
                                                  int16_t iTargetMaskStride,
                                                  arm_2d_size_t *
                                                  __restrict ptTargetMaskSize);

void __arm_2d_impl_gray8_1h_des_msk_fill_xy_mirror(uint8_t *
                                                   __restrict ptSourceBase,
                                                   int16_t iSourceStride,
                                                   arm_2d_size_t *
                                                   __restrict ptSourceSize,
                                                   uint8_t *
                                                   __restrict ptTargetBase,
                                                   int16_t iTargetStride,
                                                   arm_2d_size_t *
                                                   __restrict ptTargetSize,
                                                   uint8_t *
                                                   __restrict ptTargetMaskBase,
                                                   int16_t iTargetMaskStride,
                                                   arm_2d_size_t *
                                                   __restrict ptTargetMaskSize);

void __arm_2d_impl_gray8_1h_des_msk_fill_y_mirror(uint8_t *
                                                  __restrict ptSourceBase,
                                                  int16_t iSourceStride,
                                                  arm_2d_size_t *
                                                  __restrict ptSourceSize,
                                                  uint8_t *
                                                  __restrict ptTargetBase,
                                                  int16_t iTargetStride,
                                                  arm_2d_size_t *
                                                  __restrict ptTargetSize,
                                                  uint8_t *
                                                  __restrict ptTargetMaskBase,
                                                  int16_t iTargetMaskStride,
                                                  arm_2d_size_t *
                                                  __restrict ptTargetMaskSize);

void __arm_2d_impl_gray8_alpha_blending(uint8_t * __restrict pSourceBase,
                                        int16_t iSourceStride,
                                        uint8_t * __restrict pTargetBase,
                                        int16_t iTargetStride,
                                        arm_2d_size_t * __restrict ptCopySize,
                                        uint_fast16_t hwRatio);

void __arm_2d_impl_gray8_alpha_blending_colour_keying(uint8_t *
                                                      __restrict pSourceBase,
                                                      int16_t iSourceStride,
                                                      uint8_t *
                                                      __restrict pTargetBase,
                                                      int16_t iTargetStride,
                                                      arm_2d_size_t *
                                                      __restrict ptCopySize,
                                                      uint_fast16_t hwRatio,
                                                      uint8_t Colour);

void __arm_2d_impl_gray8_colour_filling_channel_mask(uint8_t *
                                                     __restrict pTarget,
                                                     int16_t iTargetStride,
                                                     uint32_t *
                                                     __restrict pwAlpha,
                                                     int16_t iAlphaStride,
                                                     arm_2d_size_t *
                                                     __restrict ptCopySize,
                                                     uint8_t Colour);

void __arm_2d_impl_gray8_colour_filling_channel_mask_opacity(uint8_t *
                                                     __restrict pTarget,
                                                     int16_t
                                                     iTargetStride,
                                                     uint32_t *
                                                     __restrict pwAlpha,
                                                     int16_t
                                                     iAlphaStride,
                                                     arm_2d_size_t *
                                                     __restrict
                                                     ptCopySize,
                                                     uint8_t Colour,
                                                     uint_fast16_t hwOpacity);

void __arm_2d_impl_gray8_colour_filling_mask(uint8_t * __restrict pTarget,
                                             int16_t iTargetStride,
                                             uint8_t * __restrict pchAlpha,
                                             int16_t iAlphaStride,
                                             arm_2d_size_t *
                                             __restrict ptCopySize,
                                             uint8_t Colour);

void __arm_2d_impl_gray8_colour_filling_mask_opacity(uint8_t *
                                                     __restrict pTarget,
                                                     int16_t iTargetStride,
                                                     uint8_t *
                                                     __restrict pchAlpha,
                                                     int16_t iAlphaStride,
                                                     arm_2d_size_t *
                                                     __restrict ptCopySize,
                                                     uint8_t Colour,
                                                     uint_fast16_t hwOpacity);

void __arm_2d_impl_gray8_colour_filling_with_opacity(uint8_t *
                                                     __restrict pTargetBase,
                                                     int16_t iTargetStride,
                                                     arm_2d_size_t *
                                                     __restrict ptCopySize,
                                                     uint8_t Colour,
                                                     uint_fast16_t hwRatio);

void __arm_2d_impl_gray8_des_chn_msk_copy(uint8_t * __restrict pSourceBase,
                                          int16_t iSourceStride,
                                          uint8_t * __restrict pTargetBase,
                                          int16_t iTargetStride,
                                          uint32_t *
                                          __restrict ptTargetMaskBase,
                                          int16_t iTargetMaskStride,
                                          arm_2d_size_t *
                                          __restrict ptTargetMaskSize,
                                          arm_2d_size_t *
                                          __restrict ptCopySize);

void __arm_2d_impl_gray8_des_chn_msk_copy_mirror(uint8_t *
                                                 __restrict pSourceBase,
                                                 int16_t iSourceStride,
                                                 uint8_t *
                                                 __restrict pTargetBase,
                                                 int16_t iTargetStride,
                                                 uint32_t *
                                                 __restrict ptTargetMaskBase,
                                                 int16_t iTargetMaskStride,
                                                 arm_2d_size_t *
                                                 __restrict ptTargetMaskSize,
                                                 arm_2d_size_t *
                                                 __restrict ptCopySize,
                                                 uint32_t wMode);

void __arm_2d_impl_gray8_des_chn_msk_copy_x_mirror(uint8_t *
                                                   __restrict pSourceBase,
                                                   int16_t iSourceStride,
                                                   uint8_t *
                                                   __restrict pTargetBase,
                                                   int16_t iTargetStride,
                                                   uint32_t *
                                                   __restrict ptTargetMaskBase,
                                                   int16_t iTargetMaskStride,
                                                   arm_2d_size_t *
                                                   __restrict ptTargetMaskSize,
                                                   arm_2d_size_t *
                                                   __restrict ptCopySize);

void __arm_2d_impl_gray8_des_chn_msk_copy_xy_mirror(uint8_t *
                                                    __restrict pSourceBase,
                                                    int16_t iSourceStride,
                                                    uint8_t *
                                                    __restrict pTargetBase,
                                                    int16_t iTargetStride,
                                                    uint32_t *
                                                    __restrict ptTargetMaskBase,
                                                    int16_t iTargetMaskStride,
                                                    arm_2d_size_t *
                                                    __restrict ptTargetMaskSize,
                                                    arm_2d_size_t *
                                                    __restrict ptCopySize);

void __arm_2d_impl_gray8_des_chn_msk_copy_y_mirror(uint8_t *
                                                   __restrict pSourceBase,
                                                   int16_t iSourceStride,
                                                   uint8_t *
                                                   __restrict pTargetBase,
                                                   int16_t iTargetStride,
                                                   uint32_t *
                                                   __restrict ptTargetMaskBase,
                                                   int16_t iTargetMaskStride,
                                                   arm_2d_size_t *
                                                   __restrict ptTargetMaskSize,
                                                   arm_2d_size_t *
                                                   __restrict ptCopySize);

void __arm_2d_impl_gray8_des_chn_msk_fill(uint8_t * __restrict ptSourceBase,
                                          int16_t iSourceStride,
                                          arm_2d_size_t *
                                          __restrict ptSourceSize,
                                          uint8_t * __restrict ptTargetBase,
                                          int16_t iTargetStride,
                                          arm_2d_size_t *
                                          __restrict ptTargetSize,
                                          uint32_t *
                                          __restrict ptTargetMaskBase,
                                          int16_t iTargetMaskStride,
                                          arm_2d_size_t *
                                          __restrict ptTargetMaskSize);

void __arm_2d_impl_gray8_des_chn_msk_fill_mirror(uint8_t *
                                                 __restrict ptSourceBase,
                                                 int16_t iSourceStride,
                                                 arm_2d_size_t *
                                                 __restrict ptSourceSize,
                                                 uint8_t *
                                                 __restrict ptTargetBase,
                                                 int16_t iTargetStride,
                                                 arm_2d_size_t *
                                                 __restrict ptTargetSize,
                                                 uint32_t *
                                                 __restrict ptTargetMaskBase,
                                                 int16_t iTargetMaskStride,
                                                 arm_2d_size_t *
                                                 __restrict ptTargetMaskSize,
                                                 uint32_t wMode);

void __arm_2d_impl_gray8_des_chn_msk_fill_x_mirror(uint8_t *
                                                   __restrict ptSourceBase,
                                                   int16_t iSourceStride,
                                                   arm_2d_size_t *
                                                   __restrict ptSourceSize,
                                                   uint8_t *
                                                   __restrict ptTargetBase,
                                                   int16_t iTargetStride,
                                                   arm_2d_size_t *
                                                   __restrict ptTargetSize,
                                                   uint32_t *
                                                   __restrict ptTargetMaskBase,
                                                   int16_t iTargetMaskStride,
                                                   arm_2d_size_t *
                                                   __restrict ptTargetMaskSize);

void __arm_2d_impl_gray8_des_chn_msk_fill_xy_mirror(uint8_t *
                                                    __restrict ptSourceBase,
                                                    int16_t iSourceStride,
                                                    arm_2d_size_t *
                                                    __restrict ptSourceSize,
                                                    uint8_t *
                                                    __restrict ptTargetBase,
                                                    int16_t iTargetStride,
                                                    arm_2d_size_t *
                                                    __restrict ptTargetSize,
                                                    uint32_t *
                                                    __restrict ptTargetMaskBase,
                                                    int16_t iTargetMaskStride,
                                                    arm_2d_size_t *
                                                    __restrict
                                                    ptTargetMaskSize);

void __arm_2d_impl_gray8_des_chn_msk_fill_y_mirror(uint8_t *
                                                   __restrict ptSourceBase,
                                                   int16_t iSourceStride,
                                                   arm_2d_size_t *
                                                   __restrict ptSourceSize,
                                                   uint8_t *
                                                   __restrict ptTargetBase,
                                                   int16_t iTargetStride,
                                                   arm_2d_size_t *
                                                   __restrict ptTargetSize,
                                                   uint32_t *
                                                   __restrict ptTargetMaskBase,
                                                   int16_t iTargetMaskStride,
                                                   arm_2d_size_t *
                                                   __restrict ptTargetMaskSize);

void __arm_2d_impl_gray8_des_msk_copy(uint8_t * __restrict pSourceBase,
                                      int16_t iSourceStride,
                                      uint8_t * __restrict pTargetBase,
                                      int16_t iTargetStride,
                                      uint8_t * __restrict ptTargetMaskBase,
                                      int16_t iTargetMaskStride,
                                      arm_2d_size_t *
                                      __restrict ptTargetMaskSize,
                                      arm_2d_size_t * __restrict ptCopySize);

void __arm_2d_impl_gray8_des_msk_copy_mirror(uint8_t * __restrict pSourceBase,
                                             int16_t iSourceStride,
                                             uint8_t * __restrict pTargetBase,
                                             int16_t iTargetStride,
                                             uint8_t *
                                             __restrict ptTargetMaskBase,
                                             int16_t iTargetMaskStride,
                                             arm_2d_size_t *
                                             __restrict ptTargetMaskSize,
                                             arm_2d_size_t *
                                             __restrict ptCopySize,
                                             uint32_t wMode);

void __arm_2d_impl_gray8_des_msk_copy_x_mirror(uint8_t * __restrict pSourceBase,
                                               int16_t iSourceStride,
                                               uint8_t * __restrict pTargetBase,
                                               int16_t iTargetStride,
                                               uint8_t *
                                               __restrict ptTargetMaskBase,
                                               int16_t iTargetMaskStride,
                                               arm_2d_size_t *
                                               __restrict ptTargetMaskSize,
                                               arm_2d_size_t *
                                               __restrict ptCopySize);

void __arm_2d_impl_gray8_des_msk_copy_xy_mirror(uint8_t *
                                                __restrict pSourceBase,
                                                int16_t iSourceStride,
                                                uint8_t *
                                                __restrict pTargetBase,
                                                int16_t iTargetStride,
                                                uint8_t *
                                                __restrict ptTargetMaskBase,
                                                int16_t iTargetMaskStride,
                                                arm_2d_size_t *
                                                __restrict ptTargetMaskSize,
                                                arm_2d_size_t *
                                                __restrict ptCopySize);

void __arm_2d_impl_gray8_des_msk_copy_y_mirror(uint8_t * __restrict pSourceBase,
                                               int16_t iSourceStride,
                                               uint8_t * __restrict pTargetBase,
                                               int16_t iTargetStride,
                                               uint8_t *
                                               __restrict ptTargetMaskBase,
                                               int16_t iTargetMaskStride,
                                               arm_2d_size_t *
                                               __restrict ptTargetMaskSize,
                                               arm_2d_size_t *
                                               __restrict ptCopySize);

void __arm_2d_impl_gray8_des_msk_fill(uint8_t * __restrict ptSourceBase,
                                      int16_t iSourceStride,
                                      arm_2d_size_t * __restrict ptSourceSize,
                                      uint8_t * __restrict ptTargetBase,
                                      int16_t iTargetStride,
                                      arm_2d_size_t * __restrict ptTargetSize,
                                      uint8_t * __restrict ptTargetMaskBase,
                                      int16_t iTargetMaskStride,
                                      arm_2d_size_t *
                                      __restrict ptTargetMaskSize);

void __arm_2d_impl_gray8_des_msk_fill_mirror(uint8_t * __restrict ptSourceBase,
                                             int16_t iSourceStride,
                                             arm_2d_size_t *
                                             __restrict ptSourceSize,
                                             uint8_t * __restrict ptTargetBase,
                                             int16_t iTargetStride,
                                             arm_2d_size_t *
                                             __restrict ptTargetSize,
                                             uint8_t *
                                             __restrict ptTargetMaskBase,
                                             int16_t iTargetMaskStride,
                                             arm_2d_size_t *
                                             __restrict ptTargetMaskSize,
                                             uint32_t wMode);

void __arm_2d_impl_gray8_des_msk_fill_x_mirror(uint8_t *
                                               __restrict ptSourceBase,
                                               int16_t iSourceStride,
                                               arm_2d_size_t *
                                               __restrict ptSourceSize,
                                               uint8_t *
                                               __restrict ptTargetBase,
                                               int16_t iTargetStride,
                                               arm_2d_size_t *
                                               __restrict ptTargetSize,
                                               uint8_t *
                                               __restrict ptTargetMaskBase,
                                               int16_t iTargetMaskStride,
                                               arm_2d_size_t *
                                               __restrict ptTargetMaskSize);

void __arm_2d_impl_gray8_des_msk_fill_xy_mirror(uint8_t *
                                                __restrict ptSourceBase,
                                                int16_t iSourceStride,
                                                arm_2d_size_t *
                                                __restrict ptSourceSize,
                                                uint8_t *
                                                __restrict ptTargetBase,
                                                int16_t iTargetStride,
                                                arm_2d_size_t *
                                                __restrict ptTargetSize,
                                                uint8_t *
                                                __restrict ptTargetMaskBase,
                                                int16_t iTargetMaskStride,
                                                arm_2d_size_t *
                                                __restrict ptTargetMaskSize);

void __arm_2d_impl_gray8_des_msk_fill_y_mirror(uint8_t *
                                               __restrict ptSourceBase,
                                               int16_t iSourceStride,
                                               arm_2d_size_t *
                                               __restrict ptSourceSize,
                                               uint8_t *
                                               __restrict ptTargetBase,
                                               int16_t iTargetStride,
                                               arm_2d_size_t *
                                               __restrict ptTargetSize,
                                               uint8_t *
                                               __restrict ptTargetMaskBase,
                                               int16_t iTargetMaskStride,
                                               arm_2d_size_t *
                                               __restrict ptTargetMaskSize);

void __arm_2d_impl_gray8_masks_copy(uint8_t * __restrict pSourceBase,
                                    int16_t iSourceStride,
                                    uint8_t * __restrict ptSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __restrict ptSourceMaskSize,
                                    uint8_t * __restrict pTargetBase,
                                    int16_t iTargetStride,
                                    uint8_t * __restrict ptTargetMaskBase,
                                    int16_t iTargetMaskStride,
                                    arm_2d_size_t * __restrict ptTargetMaskSize,
                                    arm_2d_size_t * __restrict ptCopySize);

void __arm_2d_impl_gray8_masks_copy_mirror(uint8_t * __restrict pSourceBase,
                                           int16_t iSourceStride,
                                           uint8_t *
                                           __restrict ptSourceMaskBase,
                                           int16_t iSourceMaskStride,
                                           arm_2d_size_t *
                                           __restrict ptSourceMaskSize,
                                           uint8_t * __restrict pTargetBase,
                                           int16_t iTargetStride,
                                           uint8_t *
                                           __restrict ptTargetMaskBase,
                                           int16_t iTargetMaskStride,
                                           arm_2d_size_t *
                                           __restrict ptTargetMaskSize,
                                           arm_2d_size_t *
                                           __restrict ptCopySize,
                                           uint32_t wMode);

void __arm_2d_impl_gray8_masks_copy_x_mirror(uint8_t * __restrict pSourceBase,
                                             int16_t iSourceStride,
                                             uint8_t *
                                             __restrict ptSourceMaskBase,
                                             int16_t iSourceMaskStride,
                                             arm_2d_size_t *
                                             __restrict ptSourceMaskSize,
                                             uint8_t * __restrict pTargetBase,
                                             int16_t iTargetStride,
                                             uint8_t *
                                             __restrict ptTargetMaskBase,
                                             int16_t iTargetMaskStride,
                                             arm_2d_size_t *
                                             __restrict ptTargetMaskSize,
                                             arm_2d_size_t *
                                             __restrict ptCopySize);

void __arm_2d_impl_gray8_masks_copy_xy_mirror(uint8_t * __restrict pSourceBase,
                                              int16_t iSourceStride,
                                              uint8_t *
                                              __restrict ptSourceMaskBase,
                                              int16_t iSourceMaskStride,
                                              arm_2d_size_t *
                                              __restrict ptSourceMaskSize,
                                              uint8_t * __restrict pTargetBase,
                                              int16_t iTargetStride,
                                              uint8_t *
                                              __restrict ptTargetMaskBase,
                                              int16_t iTargetMaskStride,
                                              arm_2d_size_t *
                                              __restrict ptTargetMaskSize,
                                              arm_2d_size_t *
                                              __restrict ptCopySize);

void __arm_2d_impl_gray8_masks_copy_y_mirror(uint8_t * __restrict pSourceBase,
                                             int16_t iSourceStride,
                                             uint8_t *
                                             __restrict ptSourceMaskBase,
                                             int16_t iSourceMaskStride,
                                             arm_2d_size_t *
                                             __restrict ptSourceMaskSize,
                                             uint8_t * __restrict pTargetBase,
                                             int16_t iTargetStride,
                                             uint8_t *
                                             __restrict ptTargetMaskBase,
                                             int16_t iTargetMaskStride,
                                             arm_2d_size_t *
                                             __restrict ptTargetMaskSize,
                                             arm_2d_size_t *
                                             __restrict ptCopySize);

void __arm_2d_impl_gray8_masks_fill(uint8_t * __restrict ptSourceBase,
                                    int16_t iSourceStride,
                                    arm_2d_size_t * __restrict ptSourceSize,
                                    uint8_t * __restrict ptSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __restrict ptSourceMaskSize,
                                    uint8_t * __restrict ptTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t * __restrict ptTargetSize,
                                    uint8_t * __restrict ptTargetMaskBase,
                                    int16_t iTargetMaskStride,
                                    arm_2d_size_t *
                                    __restrict ptTargetMaskSize);

void __arm_2d_impl_gray8_masks_fill_mirror(uint8_t * __restrict ptSourceBase,
                                           int16_t iSourceStride,
                                           arm_2d_size_t *
                                           __restrict ptSourceSize,
                                           uint8_t *
                                           __restrict ptSourceMaskBase,
                                           int16_t iSourceMaskStride,
                                           arm_2d_size_t *
                                           __restrict ptSourceMaskSize,
                                           uint8_t * __restrict ptTargetBase,
                                           int16_t iTargetStride,
                                           arm_2d_size_t *
                                           __restrict ptTargetSize,
                                           uint8_t *
                                           __restrict ptTargetMaskBase,
                                           int16_t iTargetMaskStride,
                                           arm_2d_size_t *
                                           __restrict ptTargetMaskSize,
                                           uint32_t wMode);

void __arm_2d_impl_gray8_masks_fill_x_mirror(uint8_t * __restrict ptSourceBase,
                                             int16_t iSourceStride,
                                             arm_2d_size_t *
                                             __restrict ptSourceSize,
                                             uint8_t *
                                             __restrict ptSourceMaskBase,
                                             int16_t iSourceMaskStride,
                                             arm_2d_size_t *
                                             __restrict ptSourceMaskSize,
                                             uint8_t * __restrict ptTargetBase,
                                             int16_t iTargetStride,
                                             arm_2d_size_t *
                                             __restrict ptTargetSize,
                                             uint8_t *
                                             __restrict ptTargetMaskBase,
                                             int16_t iTargetMaskStride,
                                             arm_2d_size_t *
                                             __restrict ptTargetMaskSize);

void __arm_2d_impl_gray8_masks_fill_xy_mirror(uint8_t * __restrict ptSourceBase,
                                              int16_t iSourceStride,
                                              arm_2d_size_t *
                                              __restrict ptSourceSize,
                                              uint8_t *
                                              __restrict ptSourceMaskBase,
                                              int16_t iSourceMaskStride,
                                              arm_2d_size_t *
                                              __restrict ptSourceMaskSize,
                                              uint8_t * __restrict ptTargetBase,
                                              int16_t iTargetStride,
                                              arm_2d_size_t *
                                              __restrict ptTargetSize,
                                              uint8_t *
                                              __restrict ptTargetMaskBase,
                                              int16_t iTargetMaskStride,
                                              arm_2d_size_t *
                                              __restrict ptTargetMaskSize);

void __arm_2d_impl_gray8_masks_fill_y_mirror(uint8_t * __restrict ptSourceBase,
                                             int16_t iSourceStride,
                                             arm_2d_size_t *
                                             __restrict ptSourceSize,
                                             uint8_t *
                                             __restrict ptSourceMaskBase,
                                             int16_t iSourceMaskStride,
                                             arm_2d_size_t *
                                             __restrict ptSourceMaskSize,
                                             uint8_t * __restrict ptTargetBase,
                                             int16_t iTargetStride,
                                             arm_2d_size_t *
                                             __restrict ptTargetSize,
                                             uint8_t *
                                             __restrict ptTargetMaskBase,
                                             int16_t iTargetMaskStride,
                                             arm_2d_size_t *
                                             __restrict ptTargetMaskSize);

void __arm_2d_impl_gray8_src_chn_msk_1h_des_msk_copy(uint8_t *
                                                     __restrict pSourceBase,
                                                     int16_t iSourceStride,
                                                     uint32_t *
                                                     __restrict
                                                     ptSourceMaskBase,
                                                     int16_t iSourceMaskStride,
                                                     arm_2d_size_t *
                                                     __restrict
                                                     ptSourceMaskSize,
                                                     uint8_t *
                                                     __restrict pTargetBase,
                                                     int16_t iTargetStride,
                                                     uint8_t *
                                                     __restrict
                                                     ptTargetMaskBase,
                                                     int16_t iTargetMaskStride,
                                                     arm_2d_size_t *
                                                     __restrict
                                                     ptTargetMaskSize,
                                                     arm_2d_size_t *
                                                     __restrict ptCopySize);

void __arm_2d_impl_gray8_src_chn_msk_1h_des_msk_copy_mirror(uint8_t *
                                                            __restrict
                                                            pSourceBase,
                                                            int16_t
                                                            iSourceStride,
                                                            uint32_t *
                                                            __restrict
                                                            ptSourceMaskBase,
                                                            int16_t
                                                            iSourceMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceMaskSize,
                                                            uint8_t *
                                                            __restrict
                                                            pTargetBase,
                                                            int16_t
                                                            iTargetStride,
                                                            uint8_t *
                                                            __restrict
                                                            ptTargetMaskBase,
                                                            int16_t
                                                            iTargetMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetMaskSize,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptCopySize,
                                                            uint32_t wMode);

void __arm_2d_impl_gray8_src_chn_msk_1h_des_msk_copy_x_mirror(uint8_t *
                                                              __restrict
                                                              pSourceBase,
                                                              int16_t
                                                              iSourceStride,
                                                              uint32_t *
                                                              __restrict
                                                              ptSourceMaskBase,
                                                              int16_t
                                                              iSourceMaskStride,
                                                              arm_2d_size_t *
                                                              __restrict
                                                              ptSourceMaskSize,
                                                              uint8_t *
                                                              __restrict
                                                              pTargetBase,
                                                              int16_t
                                                              iTargetStride,
                                                              uint8_t *
                                                              __restrict
                                                              ptTargetMaskBase,
                                                              int16_t
                                                              iTargetMaskStride,
                                                              arm_2d_size_t *
                                                              __restrict
                                                              ptTargetMaskSize,
                                                              arm_2d_size_t *
                                                              __restrict
                                                              ptCopySize);

void __arm_2d_impl_gray8_src_chn_msk_1h_des_msk_copy_xy_mirror(uint8_t *
                                                               __restrict
                                                               pSourceBase,
                                                               int16_t
                                                               iSourceStride,
                                                               uint32_t *
                                                               __restrict
                                                               ptSourceMaskBase,
                                                               int16_t
                                                               iSourceMaskStride,
                                                               arm_2d_size_t *
                                                               __restrict
                                                               ptSourceMaskSize,
                                                               uint8_t *
                                                               __restrict
                                                               pTargetBase,
                                                               int16_t
                                                               iTargetStride,
                                                               uint8_t *
                                                               __restrict
                                                               ptTargetMaskBase,
                                                               int16_t
                                                               iTargetMaskStride,
                                                               arm_2d_size_t *
                                                               __restrict
                                                               ptTargetMaskSize,
                                                               arm_2d_size_t *
                                                               __restrict
                                                               ptCopySize);

void __arm_2d_impl_gray8_src_chn_msk_1h_des_msk_copy_y_mirror(uint8_t *
                                                              __restrict
                                                              pSourceBase,
                                                              int16_t
                                                              iSourceStride,
                                                              uint32_t *
                                                              __restrict
                                                              ptSourceMaskBase,
                                                              int16_t
                                                              iSourceMaskStride,
                                                              arm_2d_size_t *
                                                              __restrict
                                                              ptSourceMaskSize,
                                                              uint8_t *
                                                              __restrict
                                                              pTargetBase,
                                                              int16_t
                                                              iTargetStride,
                                                              uint8_t *
                                                              __restrict
                                                              ptTargetMaskBase,
                                                              int16_t
                                                              iTargetMaskStride,
                                                              arm_2d_size_t *
                                                              __restrict
                                                              ptTargetMaskSize,
                                                              arm_2d_size_t *
                                                              __restrict
                                                              ptCopySize);

void __arm_2d_impl_gray8_src_chn_msk_1h_des_msk_fill(uint8_t *
                                                     __restrict ptSourceBase,
                                                     int16_t iSourceStride,
                                                     arm_2d_size_t *
                                                     __restrict ptSourceSize,
                                                     uint32_t *
                                                     __restrict
                                                     ptSourceMaskBase,
                                                     int16_t iSourceMaskStride,
                                                     arm_2d_size_t *
                                                     __restrict
                                                     ptSourceMaskSize,
                                                     uint8_t *
                                                     __restrict ptTargetBase,
                                                     int16_t iTargetStride,
                                                     arm_2d_size_t *
                                                     __restrict ptTargetSize,
                                                     uint8_t *
                                                     __restrict
                                                     ptTargetMaskBase,
                                                     int16_t iTargetMaskStride,
                                                     arm_2d_size_t *
                                                     __restrict
                                                     ptTargetMaskSize);

void __arm_2d_impl_gray8_src_chn_msk_1h_des_msk_fill_mirror(uint8_t *
                                                            __restrict
                                                            ptSourceBase,
                                                            int16_t
                                                            iSourceStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceSize,
                                                            uint32_t *
                                                            __restrict
                                                            ptSourceMaskBase,
                                                            int16_t
                                                            iSourceMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceMaskSize,
                                                            uint8_t *
                                                            __restrict
                                                            ptTargetBase,
                                                            int16_t
                                                            iTargetStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetSize,
                                                            uint8_t *
                                                            __restrict
                                                            ptTargetMaskBase,
                                                            int16_t
                                                            iTargetMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetMaskSize,
                                                            uint32_t wMode);

void __arm_2d_impl_gray8_src_chn_msk_1h_des_msk_fill_x_mirror(uint8_t *
                                                              __restrict
                                                              ptSourceBase,
                                                              int16_t
                                                              iSourceStride,
                                                              arm_2d_size_t *
                                                              __restrict
                                                              ptSourceSize,
                                                              uint32_t *
                                                              __restrict
                                                              ptSourceMaskBase,
                                                              int16_t
                                                              iSourceMaskStride,
                                                              arm_2d_size_t *
                                                              __restrict
                                                              ptSourceMaskSize,
                                                              uint8_t *
                                                              __restrict
                                                              ptTargetBase,
                                                              int16_t
                                                              iTargetStride,
                                                              arm_2d_size_t *
                                                              __restrict
                                                              ptTargetSize,
                                                              uint8_t *
                                                              __restrict
                                                              ptTargetMaskBase,
                                                              int16_t
                                                              iTargetMaskStride,
                                                              arm_2d_size_t *
                                                              __restrict
                                                              ptTargetMaskSize);

void __arm_2d_impl_gray8_src_chn_msk_1h_des_msk_fill_xy_mirror(uint8_t *
                                                               __restrict
                                                               ptSourceBase,
                                                               int16_t
                                                               iSourceStride,
                                                               arm_2d_size_t *
                                                               __restrict
                                                               ptSourceSize,
                                                               uint32_t *
                                                               __restrict
                                                               ptSourceMaskBase,
                                                               int16_t
                                                               iSourceMaskStride,
                                                               arm_2d_size_t *
                                                               __restrict
                                                               ptSourceMaskSize,
                                                               uint8_t *
                                                               __restrict
                                                               ptTargetBase,
                                                               int16_t
                                                               iTargetStride,
                                                               arm_2d_size_t *
                                                               __restrict
                                                               ptTargetSize,
                                                               uint8_t *
                                                               __restrict
                                                               ptTargetMaskBase,
                                                               int16_t
                                                               iTargetMaskStride,
                                                               arm_2d_size_t *
                                                               __restrict
                                                               ptTargetMaskSize);

void __arm_2d_impl_gray8_src_chn_msk_1h_des_msk_fill_y_mirror(uint8_t *
                                                              __restrict
                                                              ptSourceBase,
                                                              int16_t
                                                              iSourceStride,
                                                              arm_2d_size_t *
                                                              __restrict
                                                              ptSourceSize,
                                                              uint32_t *
                                                              __restrict
                                                              ptSourceMaskBase,
                                                              int16_t
                                                              iSourceMaskStride,
                                                              arm_2d_size_t *
                                                              __restrict
                                                              ptSourceMaskSize,
                                                              uint8_t *
                                                              __restrict
                                                              ptTargetBase,
                                                              int16_t
                                                              iTargetStride,
                                                              arm_2d_size_t *
                                                              __restrict
                                                              ptTargetSize,
                                                              uint8_t *
                                                              __restrict
                                                              ptTargetMaskBase,
                                                              int16_t
                                                              iTargetMaskStride,
                                                              arm_2d_size_t *
                                                              __restrict
                                                              ptTargetMaskSize);

void __arm_2d_impl_gray8_src_chn_msk_copy(uint8_t * __restrict pSourceBase,
                                          int16_t iSourceStride,
                                          uint32_t *
                                          __restrict ptSourceMaskBase,
                                          int16_t iSourceMaskStride,
                                          arm_2d_size_t *
                                          __restrict ptSourceMaskSize,
                                          uint8_t * __restrict pTargetBase,
                                          int16_t iTargetStride,
                                          arm_2d_size_t *
                                          __restrict ptCopySize);

void __arm_2d_impl_gray8_src_chn_msk_copy_mirror(uint8_t *
                                                 __restrict pSourceBase,
                                                 int16_t iSourceStride,
                                                 uint32_t *
                                                 __restrict ptSourceMaskBase,
                                                 int16_t iSourceMaskStride,
                                                 arm_2d_size_t *
                                                 __restrict ptSourceMaskSize,
                                                 uint8_t *
                                                 __restrict pTargetBase,
                                                 int16_t iTargetStride,
                                                 arm_2d_size_t *
                                                 __restrict ptCopySize,
                                                 uint32_t wMode);

void __arm_2d_impl_gray8_src_chn_msk_copy_x_mirror(uint8_t *
                                                   __restrict pSourceBase,
                                                   int16_t iSourceStride,
                                                   uint32_t *
                                                   __restrict ptSourceMaskBase,
                                                   int16_t iSourceMaskStride,
                                                   arm_2d_size_t *
                                                   __restrict ptSourceMaskSize,
                                                   uint8_t *
                                                   __restrict pTargetBase,
                                                   int16_t iTargetStride,
                                                   arm_2d_size_t *
                                                   __restrict ptCopySize);

void __arm_2d_impl_gray8_src_chn_msk_copy_xy_mirror(uint8_t *
                                                    __restrict pSourceBase,
                                                    int16_t iSourceStride,
                                                    uint32_t *
                                                    __restrict ptSourceMaskBase,
                                                    int16_t iSourceMaskStride,
                                                    arm_2d_size_t *
                                                    __restrict ptSourceMaskSize,
                                                    uint8_t *
                                                    __restrict pTargetBase,
                                                    int16_t iTargetStride,
                                                    arm_2d_size_t *
                                                    __restrict ptCopySize);

void __arm_2d_impl_gray8_src_chn_msk_copy_y_mirror(uint8_t *
                                                   __restrict pSourceBase,
                                                   int16_t iSourceStride,
                                                   uint32_t *
                                                   __restrict ptSourceMaskBase,
                                                   int16_t iSourceMaskStride,
                                                   arm_2d_size_t *
                                                   __restrict ptSourceMaskSize,
                                                   uint8_t *
                                                   __restrict pTargetBase,
                                                   int16_t iTargetStride,
                                                   arm_2d_size_t *
                                                   __restrict ptCopySize);

void __arm_2d_impl_gray8_src_chn_msk_des_chn_msk_copy(uint8_t *
                                                      __restrict pSourceBase,
                                                      int16_t iSourceStride,
                                                      uint32_t *
                                                      __restrict
                                                      ptSourceMaskBase,
                                                      int16_t iSourceMaskStride,
                                                      arm_2d_size_t *
                                                      __restrict
                                                      ptSourceMaskSize,
                                                      uint8_t *
                                                      __restrict pTargetBase,
                                                      int16_t iTargetStride,
                                                      uint32_t *
                                                      __restrict
                                                      ptTargetMaskBase,
                                                      int16_t iTargetMaskStride,
                                                      arm_2d_size_t *
                                                      __restrict
                                                      ptTargetMaskSize,
                                                      arm_2d_size_t *
                                                      __restrict ptCopySize);

void __arm_2d_impl_gray8_src_chn_msk_des_chn_msk_copy_mirror(uint8_t *
                                                             __restrict
                                                             pSourceBase,
                                                             int16_t
                                                             iSourceStride,
                                                             uint32_t *
                                                             __restrict
                                                             ptSourceMaskBase,
                                                             int16_t
                                                             iSourceMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptSourceMaskSize,
                                                             uint8_t *
                                                             __restrict
                                                             pTargetBase,
                                                             int16_t
                                                             iTargetStride,
                                                             uint32_t *
                                                             __restrict
                                                             ptTargetMaskBase,
                                                             int16_t
                                                             iTargetMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptTargetMaskSize,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptCopySize,
                                                             uint32_t wMode);

void __arm_2d_impl_gray8_src_chn_msk_des_chn_msk_copy_x_mirror(uint8_t *
                                                               __restrict
                                                               pSourceBase,
                                                               int16_t
                                                               iSourceStride,
                                                               uint32_t *
                                                               __restrict
                                                               ptSourceMaskBase,
                                                               int16_t
                                                               iSourceMaskStride,
                                                               arm_2d_size_t *
                                                               __restrict
                                                               ptSourceMaskSize,
                                                               uint8_t *
                                                               __restrict
                                                               pTargetBase,
                                                               int16_t
                                                               iTargetStride,
                                                               uint32_t *
                                                               __restrict
                                                               ptTargetMaskBase,
                                                               int16_t
                                                               iTargetMaskStride,
                                                               arm_2d_size_t *
                                                               __restrict
                                                               ptTargetMaskSize,
                                                               arm_2d_size_t *
                                                               __restrict
                                                               ptCopySize);

void __arm_2d_impl_gray8_src_chn_msk_des_chn_msk_copy_xy_mirror(uint8_t *
                                                            __restrict
                                                            pSourceBase,
                                                            int16_t
                                                            iSourceStride,
                                                            uint32_t *
                                                            __restrict
                                                            ptSourceMaskBase,
                                                            int16_t
                                                            iSourceMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceMaskSize,
                                                            uint8_t *
                                                            __restrict
                                                            pTargetBase,
                                                            int16_t
                                                            iTargetStride,
                                                            uint32_t *
                                                            __restrict
                                                            ptTargetMaskBase,
                                                            int16_t
                                                            iTargetMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetMaskSize,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptCopySize);

void __arm_2d_impl_gray8_src_chn_msk_des_chn_msk_copy_y_mirror(uint8_t *
                                                               __restrict
                                                               pSourceBase,
                                                               int16_t
                                                               iSourceStride,
                                                               uint32_t *
                                                               __restrict
                                                               ptSourceMaskBase,
                                                               int16_t
                                                               iSourceMaskStride,
                                                               arm_2d_size_t *
                                                               __restrict
                                                               ptSourceMaskSize,
                                                               uint8_t *
                                                               __restrict
                                                               pTargetBase,
                                                               int16_t
                                                               iTargetStride,
                                                               uint32_t *
                                                               __restrict
                                                               ptTargetMaskBase,
                                                               int16_t
                                                               iTargetMaskStride,
                                                               arm_2d_size_t *
                                                               __restrict
                                                               ptTargetMaskSize,
                                                               arm_2d_size_t *
                                                               __restrict
                                                               ptCopySize);

void __arm_2d_impl_gray8_src_chn_msk_des_chn_msk_fill(uint8_t *
                                                      __restrict ptSourceBase,
                                                      int16_t iSourceStride,
                                                      arm_2d_size_t *
                                                      __restrict ptSourceSize,
                                                      uint32_t *
                                                      __restrict
                                                      ptSourceMaskBase,
                                                      int16_t iSourceMaskStride,
                                                      arm_2d_size_t *
                                                      __restrict
                                                      ptSourceMaskSize,
                                                      uint8_t *
                                                      __restrict ptTargetBase,
                                                      int16_t iTargetStride,
                                                      arm_2d_size_t *
                                                      __restrict ptTargetSize,
                                                      uint32_t *
                                                      __restrict
                                                      ptTargetMaskBase,
                                                      int16_t iTargetMaskStride,
                                                      arm_2d_size_t *
                                                      __restrict
                                                      ptTargetMaskSize);

void __arm_2d_impl_gray8_src_chn_msk_des_chn_msk_fill_mirror(uint8_t *
                                                             __restrict
                                                             ptSourceBase,
                                                             int16_t
                                                             iSourceStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptSourceSize,
                                                             uint32_t *
                                                             __restrict
                                                             ptSourceMaskBase,
                                                             int16_t
                                                             iSourceMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptSourceMaskSize,
                                                             uint8_t *
                                                             __restrict
                                                             ptTargetBase,
                                                             int16_t
                                                             iTargetStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptTargetSize,
                                                             uint32_t *
                                                             __restrict
                                                             ptTargetMaskBase,
                                                             int16_t
                                                             iTargetMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptTargetMaskSize,
                                                             uint32_t wMode);

void __arm_2d_impl_gray8_src_chn_msk_des_chn_msk_fill_x_mirror(uint8_t *
                                                               __restrict
                                                               ptSourceBase,
                                                               int16_t
                                                               iSourceStride,
                                                               arm_2d_size_t *
                                                               __restrict
                                                               ptSourceSize,
                                                               uint32_t *
                                                               __restrict
                                                               ptSourceMaskBase,
                                                               int16_t
                                                               iSourceMaskStride,
                                                               arm_2d_size_t *
                                                               __restrict
                                                               ptSourceMaskSize,
                                                               uint8_t *
                                                               __restrict
                                                               ptTargetBase,
                                                               int16_t
                                                               iTargetStride,
                                                               arm_2d_size_t *
                                                               __restrict
                                                               ptTargetSize,
                                                               uint32_t *
                                                               __restrict
                                                               ptTargetMaskBase,
                                                               int16_t
                                                               iTargetMaskStride,
                                                               arm_2d_size_t *
                                                               __restrict
                                                               ptTargetMaskSize);

void __arm_2d_impl_gray8_src_chn_msk_des_chn_msk_fill_xy_mirror(uint8_t *
                                                            __restrict
                                                            ptSourceBase,
                                                            int16_t
                                                            iSourceStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceSize,
                                                            uint32_t *
                                                            __restrict
                                                            ptSourceMaskBase,
                                                            int16_t
                                                            iSourceMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceMaskSize,
                                                            uint8_t *
                                                            __restrict
                                                            ptTargetBase,
                                                            int16_t
                                                            iTargetStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetSize,
                                                            uint32_t *
                                                            __restrict
                                                            ptTargetMaskBase,
                                                            int16_t
                                                            iTargetMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetMaskSize);

void __arm_2d_impl_gray8_src_chn_msk_des_chn_msk_fill_y_mirror(uint8_t *
                                                               __restrict
                                                               ptSourceBase,
                                                               int16_t
                                                               iSourceStride,
                                                               arm_2d_size_t *
                                                               __restrict
                                                               ptSourceSize,
                                                               uint32_t *
                                                               __restrict
                                                               ptSourceMaskBase,
                                                               int16_t
                                                               iSourceMaskStride,
                                                               arm_2d_size_t *
                                                               __restrict
                                                               ptSourceMaskSize,
                                                               uint8_t *
                                                               __restrict
                                                               ptTargetBase,
                                                               int16_t
                                                               iTargetStride,
                                                               arm_2d_size_t *
                                                               __restrict
                                                               ptTargetSize,
                                                               uint32_t *
                                                               __restrict
                                                               ptTargetMaskBase,
                                                               int16_t
                                                               iTargetMaskStride,
                                                               arm_2d_size_t *
                                                               __restrict
                                                               ptTargetMaskSize);

void __arm_2d_impl_gray8_src_chn_msk_des_msk_copy(uint8_t *
                                                  __restrict pSourceBase,
                                                  int16_t iSourceStride,
                                                  uint32_t *
                                                  __restrict ptSourceMaskBase,
                                                  int16_t iSourceMaskStride,
                                                  arm_2d_size_t *
                                                  __restrict ptSourceMaskSize,
                                                  uint8_t *
                                                  __restrict pTargetBase,
                                                  int16_t iTargetStride,
                                                  uint8_t *
                                                  __restrict ptTargetMaskBase,
                                                  int16_t iTargetMaskStride,
                                                  arm_2d_size_t *
                                                  __restrict ptTargetMaskSize,
                                                  arm_2d_size_t *
                                                  __restrict ptCopySize);

void __arm_2d_impl_gray8_src_chn_msk_des_msk_copy_mirror(uint8_t *
                                                         __restrict pSourceBase,
                                                         int16_t iSourceStride,
                                                         uint32_t *
                                                         __restrict
                                                         ptSourceMaskBase,
                                                         int16_t
                                                         iSourceMaskStride,
                                                         arm_2d_size_t *
                                                         __restrict
                                                         ptSourceMaskSize,
                                                         uint8_t *
                                                         __restrict pTargetBase,
                                                         int16_t iTargetStride,
                                                         uint8_t *
                                                         __restrict
                                                         ptTargetMaskBase,
                                                         int16_t
                                                         iTargetMaskStride,
                                                         arm_2d_size_t *
                                                         __restrict
                                                         ptTargetMaskSize,
                                                         arm_2d_size_t *
                                                         __restrict ptCopySize,
                                                         uint32_t wMode);

void __arm_2d_impl_gray8_src_chn_msk_des_msk_copy_x_mirror(uint8_t *
                                                           __restrict
                                                           pSourceBase,
                                                           int16_t
                                                           iSourceStride,
                                                           uint32_t *
                                                           __restrict
                                                           ptSourceMaskBase,
                                                           int16_t
                                                           iSourceMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptSourceMaskSize,
                                                           uint8_t *
                                                           __restrict
                                                           pTargetBase,
                                                           int16_t
                                                           iTargetStride,
                                                           uint8_t *
                                                           __restrict
                                                           ptTargetMaskBase,
                                                           int16_t
                                                           iTargetMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptTargetMaskSize,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptCopySize);

void __arm_2d_impl_gray8_src_chn_msk_des_msk_copy_xy_mirror(uint8_t *
                                                            __restrict
                                                            pSourceBase,
                                                            int16_t
                                                            iSourceStride,
                                                            uint32_t *
                                                            __restrict
                                                            ptSourceMaskBase,
                                                            int16_t
                                                            iSourceMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceMaskSize,
                                                            uint8_t *
                                                            __restrict
                                                            pTargetBase,
                                                            int16_t
                                                            iTargetStride,
                                                            uint8_t *
                                                            __restrict
                                                            ptTargetMaskBase,
                                                            int16_t
                                                            iTargetMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetMaskSize,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptCopySize);

void __arm_2d_impl_gray8_src_chn_msk_des_msk_copy_y_mirror(uint8_t *
                                                           __restrict
                                                           pSourceBase,
                                                           int16_t
                                                           iSourceStride,
                                                           uint32_t *
                                                           __restrict
                                                           ptSourceMaskBase,
                                                           int16_t
                                                           iSourceMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptSourceMaskSize,
                                                           uint8_t *
                                                           __restrict
                                                           pTargetBase,
                                                           int16_t
                                                           iTargetStride,
                                                           uint8_t *
                                                           __restrict
                                                           ptTargetMaskBase,
                                                           int16_t
                                                           iTargetMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptTargetMaskSize,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptCopySize);

void __arm_2d_impl_gray8_src_chn_msk_des_msk_fill(uint8_t *
                                                  __restrict ptSourceBase,
                                                  int16_t iSourceStride,
                                                  arm_2d_size_t *
                                                  __restrict ptSourceSize,
                                                  uint32_t *
                                                  __restrict ptSourceMaskBase,
                                                  int16_t iSourceMaskStride,
                                                  arm_2d_size_t *
                                                  __restrict ptSourceMaskSize,
                                                  uint8_t *
                                                  __restrict ptTargetBase,
                                                  int16_t iTargetStride,
                                                  arm_2d_size_t *
                                                  __restrict ptTargetSize,
                                                  uint8_t *
                                                  __restrict ptTargetMaskBase,
                                                  int16_t iTargetMaskStride,
                                                  arm_2d_size_t *
                                                  __restrict ptTargetMaskSize);

void __arm_2d_impl_gray8_src_chn_msk_des_msk_fill_mirror(uint8_t *
                                                         __restrict
                                                         ptSourceBase,
                                                         int16_t iSourceStride,
                                                         arm_2d_size_t *
                                                         __restrict
                                                         ptSourceSize,
                                                         uint32_t *
                                                         __restrict
                                                         ptSourceMaskBase,
                                                         int16_t
                                                         iSourceMaskStride,
                                                         arm_2d_size_t *
                                                         __restrict
                                                         ptSourceMaskSize,
                                                         uint8_t *
                                                         __restrict
                                                         ptTargetBase,
                                                         int16_t iTargetStride,
                                                         arm_2d_size_t *
                                                         __restrict
                                                         ptTargetSize,
                                                         uint8_t *
                                                         __restrict
                                                         ptTargetMaskBase,
                                                         int16_t
                                                         iTargetMaskStride,
                                                         arm_2d_size_t *
                                                         __restrict
                                                         ptTargetMaskSize,
                                                         uint32_t wMode);

void __arm_2d_impl_gray8_src_chn_msk_des_msk_fill_x_mirror(uint8_t *
                                                           __restrict
                                                           ptSourceBase,
                                                           int16_t
                                                           iSourceStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptSourceSize,
                                                           uint32_t *
                                                           __restrict
                                                           ptSourceMaskBase,
                                                           int16_t
                                                           iSourceMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptSourceMaskSize,
                                                           uint8_t *
                                                           __restrict
                                                           ptTargetBase,
                                                           int16_t
                                                           iTargetStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptTargetSize,
                                                           uint8_t *
                                                           __restrict
                                                           ptTargetMaskBase,
                                                           int16_t
                                                           iTargetMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptTargetMaskSize);

void __arm_2d_impl_gray8_src_chn_msk_des_msk_fill_xy_mirror(uint8_t *
                                                            __restrict
                                                            ptSourceBase,
                                                            int16_t
                                                            iSourceStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceSize,
                                                            uint32_t *
                                                            __restrict
                                                            ptSourceMaskBase,
                                                            int16_t
                                                            iSourceMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceMaskSize,
                                                            uint8_t *
                                                            __restrict
                                                            ptTargetBase,
                                                            int16_t
                                                            iTargetStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetSize,
                                                            uint8_t *
                                                            __restrict
                                                            ptTargetMaskBase,
                                                            int16_t
                                                            iTargetMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetMaskSize);

void __arm_2d_impl_gray8_src_chn_msk_des_msk_fill_y_mirror(uint8_t *
                                                           __restrict
                                                           ptSourceBase,
                                                           int16_t
                                                           iSourceStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptSourceSize,
                                                           uint32_t *
                                                           __restrict
                                                           ptSourceMaskBase,
                                                           int16_t
                                                           iSourceMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptSourceMaskSize,
                                                           uint8_t *
                                                           __restrict
                                                           ptTargetBase,
                                                           int16_t
                                                           iTargetStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptTargetSize,
                                                           uint8_t *
                                                           __restrict
                                                           ptTargetMaskBase,
                                                           int16_t
                                                           iTargetMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptTargetMaskSize);

void __arm_2d_impl_gray8_src_chn_msk_fill(uint8_t * __restrict ptSourceBase,
                                          int16_t iSourceStride,
                                          arm_2d_size_t *
                                          __restrict ptSourceSize,
                                          uint32_t *
                                          __restrict ptSourceMaskBase,
                                          int16_t iSourceMaskStride,
                                          arm_2d_size_t *
                                          __restrict ptSourceMaskSize,
                                          uint8_t * __restrict ptTargetBase,
                                          int16_t iTargetStride,
                                          arm_2d_size_t *
                                          __restrict ptTargetSize);

void __arm_2d_impl_gray8_src_chn_msk_fill_mirror(uint8_t *
                                                 __restrict ptSourceBase,
                                                 int16_t iSourceStride,
                                                 arm_2d_size_t *
                                                 __restrict ptSourceSize,
                                                 uint32_t *
                                                 __restrict ptSourceMaskBase,
                                                 int16_t iSourceMaskStride,
                                                 arm_2d_size_t *
                                                 __restrict ptSourceMaskSize,
                                                 uint8_t *
                                                 __restrict ptTargetBase,
                                                 int16_t iTargetStride,
                                                 arm_2d_size_t *
                                                 __restrict ptTargetSize,
                                                 uint32_t wMode);

void __arm_2d_impl_gray8_src_chn_msk_fill_x_mirror(uint8_t *
                                                   __restrict ptSourceBase,
                                                   int16_t iSourceStride,
                                                   arm_2d_size_t *
                                                   __restrict ptSourceSize,
                                                   uint32_t *
                                                   __restrict ptSourceMaskBase,
                                                   int16_t iSourceMaskStride,
                                                   arm_2d_size_t *
                                                   __restrict ptSourceMaskSize,
                                                   uint8_t *
                                                   __restrict ptTargetBase,
                                                   int16_t iTargetStride,
                                                   arm_2d_size_t *
                                                   __restrict ptTargetSize);

void __arm_2d_impl_gray8_src_chn_msk_fill_xy_mirror(uint8_t *
                                                    __restrict ptSourceBase,
                                                    int16_t iSourceStride,
                                                    arm_2d_size_t *
                                                    __restrict ptSourceSize,
                                                    uint32_t *
                                                    __restrict ptSourceMaskBase,
                                                    int16_t iSourceMaskStride,
                                                    arm_2d_size_t *
                                                    __restrict ptSourceMaskSize,
                                                    uint8_t *
                                                    __restrict ptTargetBase,
                                                    int16_t iTargetStride,
                                                    arm_2d_size_t *
                                                    __restrict ptTargetSize);

void __arm_2d_impl_gray8_src_chn_msk_fill_y_mirror(uint8_t *
                                                   __restrict ptSourceBase,
                                                   int16_t iSourceStride,
                                                   arm_2d_size_t *
                                                   __restrict ptSourceSize,
                                                   uint32_t *
                                                   __restrict ptSourceMaskBase,
                                                   int16_t iSourceMaskStride,
                                                   arm_2d_size_t *
                                                   __restrict ptSourceMaskSize,
                                                   uint8_t *
                                                   __restrict ptTargetBase,
                                                   int16_t iTargetStride,
                                                   arm_2d_size_t *
                                                   __restrict ptTargetSize);

void __arm_2d_impl_gray8_src_msk_1h_des_msk_copy(uint8_t *
                                                 __restrict pSourceBase,
                                                 int16_t iSourceStride,
                                                 uint8_t *
                                                 __restrict ptSourceMaskBase,
                                                 int16_t iSourceMaskStride,
                                                 arm_2d_size_t *
                                                 __restrict ptSourceMaskSize,
                                                 uint8_t *
                                                 __restrict pTargetBase,
                                                 int16_t iTargetStride,
                                                 uint8_t *
                                                 __restrict ptTargetMaskBase,
                                                 int16_t iTargetMaskStride,
                                                 arm_2d_size_t *
                                                 __restrict ptTargetMaskSize,
                                                 arm_2d_size_t *
                                                 __restrict ptCopySize);

void __arm_2d_impl_gray8_src_msk_1h_des_msk_copy_mirror(uint8_t *
                                                        __restrict pSourceBase,
                                                        int16_t iSourceStride,
                                                        uint8_t *
                                                        __restrict
                                                        ptSourceMaskBase,
                                                        int16_t
                                                        iSourceMaskStride,
                                                        arm_2d_size_t *
                                                        __restrict
                                                        ptSourceMaskSize,
                                                        uint8_t *
                                                        __restrict pTargetBase,
                                                        int16_t iTargetStride,
                                                        uint8_t *
                                                        __restrict
                                                        ptTargetMaskBase,
                                                        int16_t
                                                        iTargetMaskStride,
                                                        arm_2d_size_t *
                                                        __restrict
                                                        ptTargetMaskSize,
                                                        arm_2d_size_t *
                                                        __restrict ptCopySize,
                                                        uint32_t wMode);

void __arm_2d_impl_gray8_src_msk_1h_des_msk_copy_x_mirror(uint8_t *
                                                          __restrict
                                                          pSourceBase,
                                                          int16_t iSourceStride,
                                                          uint8_t *
                                                          __restrict
                                                          ptSourceMaskBase,
                                                          int16_t
                                                          iSourceMaskStride,
                                                          arm_2d_size_t *
                                                          __restrict
                                                          ptSourceMaskSize,
                                                          uint8_t *
                                                          __restrict
                                                          pTargetBase,
                                                          int16_t iTargetStride,
                                                          uint8_t *
                                                          __restrict
                                                          ptTargetMaskBase,
                                                          int16_t
                                                          iTargetMaskStride,
                                                          arm_2d_size_t *
                                                          __restrict
                                                          ptTargetMaskSize,
                                                          arm_2d_size_t *
                                                          __restrict
                                                          ptCopySize);

void __arm_2d_impl_gray8_src_msk_1h_des_msk_copy_xy_mirror(uint8_t *
                                                           __restrict
                                                           pSourceBase,
                                                           int16_t
                                                           iSourceStride,
                                                           uint8_t *
                                                           __restrict
                                                           ptSourceMaskBase,
                                                           int16_t
                                                           iSourceMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptSourceMaskSize,
                                                           uint8_t *
                                                           __restrict
                                                           pTargetBase,
                                                           int16_t
                                                           iTargetStride,
                                                           uint8_t *
                                                           __restrict
                                                           ptTargetMaskBase,
                                                           int16_t
                                                           iTargetMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptTargetMaskSize,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptCopySize);

void __arm_2d_impl_gray8_src_msk_1h_des_msk_copy_y_mirror(uint8_t *
                                                          __restrict
                                                          pSourceBase,
                                                          int16_t iSourceStride,
                                                          uint8_t *
                                                          __restrict
                                                          ptSourceMaskBase,
                                                          int16_t
                                                          iSourceMaskStride,
                                                          arm_2d_size_t *
                                                          __restrict
                                                          ptSourceMaskSize,
                                                          uint8_t *
                                                          __restrict
                                                          pTargetBase,
                                                          int16_t iTargetStride,
                                                          uint8_t *
                                                          __restrict
                                                          ptTargetMaskBase,
                                                          int16_t
                                                          iTargetMaskStride,
                                                          arm_2d_size_t *
                                                          __restrict
                                                          ptTargetMaskSize,
                                                          arm_2d_size_t *
                                                          __restrict
                                                          ptCopySize);

void __arm_2d_impl_gray8_src_msk_1h_des_msk_fill(uint8_t *
                                                 __restrict ptSourceBase,
                                                 int16_t iSourceStride,
                                                 arm_2d_size_t *
                                                 __restrict ptSourceSize,
                                                 uint8_t *
                                                 __restrict ptSourceMaskBase,
                                                 int16_t iSourceMaskStride,
                                                 arm_2d_size_t *
                                                 __restrict ptSourceMaskSize,
                                                 uint8_t *
                                                 __restrict ptTargetBase,
                                                 int16_t iTargetStride,
                                                 arm_2d_size_t *
                                                 __restrict ptTargetSize,
                                                 uint8_t *
                                                 __restrict ptTargetMaskBase,
                                                 int16_t iTargetMaskStride,
                                                 arm_2d_size_t *
                                                 __restrict ptTargetMaskSize);

void __arm_2d_impl_gray8_src_msk_1h_des_msk_fill_mirror(uint8_t *
                                                        __restrict ptSourceBase,
                                                        int16_t iSourceStride,
                                                        arm_2d_size_t *
                                                        __restrict ptSourceSize,
                                                        uint8_t *
                                                        __restrict
                                                        ptSourceMaskBase,
                                                        int16_t
                                                        iSourceMaskStride,
                                                        arm_2d_size_t *
                                                        __restrict
                                                        ptSourceMaskSize,
                                                        uint8_t *
                                                        __restrict ptTargetBase,
                                                        int16_t iTargetStride,
                                                        arm_2d_size_t *
                                                        __restrict ptTargetSize,
                                                        uint8_t *
                                                        __restrict
                                                        ptTargetMaskBase,
                                                        int16_t
                                                        iTargetMaskStride,
                                                        arm_2d_size_t *
                                                        __restrict
                                                        ptTargetMaskSize,
                                                        uint32_t wMode);

void __arm_2d_impl_gray8_src_msk_1h_des_msk_fill_x_mirror(uint8_t *
                                                          __restrict
                                                          ptSourceBase,
                                                          int16_t iSourceStride,
                                                          arm_2d_size_t *
                                                          __restrict
                                                          ptSourceSize,
                                                          uint8_t *
                                                          __restrict
                                                          ptSourceMaskBase,
                                                          int16_t
                                                          iSourceMaskStride,
                                                          arm_2d_size_t *
                                                          __restrict
                                                          ptSourceMaskSize,
                                                          uint8_t *
                                                          __restrict
                                                          ptTargetBase,
                                                          int16_t iTargetStride,
                                                          arm_2d_size_t *
                                                          __restrict
                                                          ptTargetSize,
                                                          uint8_t *
                                                          __restrict
                                                          ptTargetMaskBase,
                                                          int16_t
                                                          iTargetMaskStride,
                                                          arm_2d_size_t *
                                                          __restrict
                                                          ptTargetMaskSize);

void __arm_2d_impl_gray8_src_msk_1h_des_msk_fill_xy_mirror(uint8_t *
                                                           __restrict
                                                           ptSourceBase,
                                                           int16_t
                                                           iSourceStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptSourceSize,
                                                           uint8_t *
                                                           __restrict
                                                           ptSourceMaskBase,
                                                           int16_t
                                                           iSourceMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptSourceMaskSize,
                                                           uint8_t *
                                                           __restrict
                                                           ptTargetBase,
                                                           int16_t
                                                           iTargetStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptTargetSize,
                                                           uint8_t *
                                                           __restrict
                                                           ptTargetMaskBase,
                                                           int16_t
                                                           iTargetMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptTargetMaskSize);

void __arm_2d_impl_gray8_src_msk_1h_des_msk_fill_y_mirror(uint8_t *
                                                          __restrict
                                                          ptSourceBase,
                                                          int16_t iSourceStride,
                                                          arm_2d_size_t *
                                                          __restrict
                                                          ptSourceSize,
                                                          uint8_t *
                                                          __restrict
                                                          ptSourceMaskBase,
                                                          int16_t
                                                          iSourceMaskStride,
                                                          arm_2d_size_t *
                                                          __restrict
                                                          ptSourceMaskSize,
                                                          uint8_t *
                                                          __restrict
                                                          ptTargetBase,
                                                          int16_t iTargetStride,
                                                          arm_2d_size_t *
                                                          __restrict
                                                          ptTargetSize,
                                                          uint8_t *
                                                          __restrict
                                                          ptTargetMaskBase,
                                                          int16_t
                                                          iTargetMaskStride,
                                                          arm_2d_size_t *
                                                          __restrict
                                                          ptTargetMaskSize);

void __arm_2d_impl_gray8_src_msk_copy(uint8_t * __restrict pSourceBase,
                                      int16_t iSourceStride,
                                      uint8_t * __restrict ptSourceMaskBase,
                                      int16_t iSourceMaskStride,
                                      arm_2d_size_t *
                                      __restrict ptSourceMaskSize,
                                      uint8_t * __restrict pTargetBase,
                                      int16_t iTargetStride,
                                      arm_2d_size_t * __restrict ptCopySize);

void __arm_2d_impl_gray8_src_msk_copy_mirror(uint8_t * __restrict pSourceBase,
                                             int16_t iSourceStride,
                                             uint8_t *
                                             __restrict ptSourceMaskBase,
                                             int16_t iSourceMaskStride,
                                             arm_2d_size_t *
                                             __restrict ptSourceMaskSize,
                                             uint8_t * __restrict pTargetBase,
                                             int16_t iTargetStride,
                                             arm_2d_size_t *
                                             __restrict ptCopySize,
                                             uint32_t wMode);

void __arm_2d_impl_gray8_src_msk_copy_x_mirror(uint8_t * __restrict pSourceBase,
                                               int16_t iSourceStride,
                                               uint8_t *
                                               __restrict ptSourceMaskBase,
                                               int16_t iSourceMaskStride,
                                               arm_2d_size_t *
                                               __restrict ptSourceMaskSize,
                                               uint8_t * __restrict pTargetBase,
                                               int16_t iTargetStride,
                                               arm_2d_size_t *
                                               __restrict ptCopySize);

void __arm_2d_impl_gray8_src_msk_copy_xy_mirror(uint8_t *
                                                __restrict pSourceBase,
                                                int16_t iSourceStride,
                                                uint8_t *
                                                __restrict ptSourceMaskBase,
                                                int16_t iSourceMaskStride,
                                                arm_2d_size_t *
                                                __restrict ptSourceMaskSize,
                                                uint8_t *
                                                __restrict pTargetBase,
                                                int16_t iTargetStride,
                                                arm_2d_size_t *
                                                __restrict ptCopySize);

void __arm_2d_impl_gray8_src_msk_copy_y_mirror(uint8_t * __restrict pSourceBase,
                                               int16_t iSourceStride,
                                               uint8_t *
                                               __restrict ptSourceMaskBase,
                                               int16_t iSourceMaskStride,
                                               arm_2d_size_t *
                                               __restrict ptSourceMaskSize,
                                               uint8_t * __restrict pTargetBase,
                                               int16_t iTargetStride,
                                               arm_2d_size_t *
                                               __restrict ptCopySize);

void __arm_2d_impl_gray8_src_msk_des_chn_msk_copy(uint8_t *
                                                  __restrict pSourceBase,
                                                  int16_t iSourceStride,
                                                  uint8_t *
                                                  __restrict ptSourceMaskBase,
                                                  int16_t iSourceMaskStride,
                                                  arm_2d_size_t *
                                                  __restrict ptSourceMaskSize,
                                                  uint8_t *
                                                  __restrict pTargetBase,
                                                  int16_t iTargetStride,
                                                  uint32_t *
                                                  __restrict ptTargetMaskBase,
                                                  int16_t iTargetMaskStride,
                                                  arm_2d_size_t *
                                                  __restrict ptTargetMaskSize,
                                                  arm_2d_size_t *
                                                  __restrict ptCopySize);

void __arm_2d_impl_gray8_src_msk_des_chn_msk_copy_mirror(uint8_t *
                                                         __restrict pSourceBase,
                                                         int16_t iSourceStride,
                                                         uint8_t *
                                                         __restrict
                                                         ptSourceMaskBase,
                                                         int16_t
                                                         iSourceMaskStride,
                                                         arm_2d_size_t *
                                                         __restrict
                                                         ptSourceMaskSize,
                                                         uint8_t *
                                                         __restrict pTargetBase,
                                                         int16_t iTargetStride,
                                                         uint32_t *
                                                         __restrict
                                                         ptTargetMaskBase,
                                                         int16_t
                                                         iTargetMaskStride,
                                                         arm_2d_size_t *
                                                         __restrict
                                                         ptTargetMaskSize,
                                                         arm_2d_size_t *
                                                         __restrict ptCopySize,
                                                         uint32_t wMode);

void __arm_2d_impl_gray8_src_msk_des_chn_msk_copy_x_mirror(uint8_t *
                                                           __restrict
                                                           pSourceBase,
                                                           int16_t
                                                           iSourceStride,
                                                           uint8_t *
                                                           __restrict
                                                           ptSourceMaskBase,
                                                           int16_t
                                                           iSourceMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptSourceMaskSize,
                                                           uint8_t *
                                                           __restrict
                                                           pTargetBase,
                                                           int16_t
                                                           iTargetStride,
                                                           uint32_t *
                                                           __restrict
                                                           ptTargetMaskBase,
                                                           int16_t
                                                           iTargetMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptTargetMaskSize,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptCopySize);

void __arm_2d_impl_gray8_src_msk_des_chn_msk_copy_xy_mirror(uint8_t *
                                                            __restrict
                                                            pSourceBase,
                                                            int16_t
                                                            iSourceStride,
                                                            uint8_t *
                                                            __restrict
                                                            ptSourceMaskBase,
                                                            int16_t
                                                            iSourceMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceMaskSize,
                                                            uint8_t *
                                                            __restrict
                                                            pTargetBase,
                                                            int16_t
                                                            iTargetStride,
                                                            uint32_t *
                                                            __restrict
                                                            ptTargetMaskBase,
                                                            int16_t
                                                            iTargetMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetMaskSize,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptCopySize);

void __arm_2d_impl_gray8_src_msk_des_chn_msk_copy_y_mirror(uint8_t *
                                                           __restrict
                                                           pSourceBase,
                                                           int16_t
                                                           iSourceStride,
                                                           uint8_t *
                                                           __restrict
                                                           ptSourceMaskBase,
                                                           int16_t
                                                           iSourceMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptSourceMaskSize,
                                                           uint8_t *
                                                           __restrict
                                                           pTargetBase,
                                                           int16_t
                                                           iTargetStride,
                                                           uint32_t *
                                                           __restrict
                                                           ptTargetMaskBase,
                                                           int16_t
                                                           iTargetMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptTargetMaskSize,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptCopySize);

void __arm_2d_impl_gray8_src_msk_des_chn_msk_fill(uint8_t *
                                                  __restrict ptSourceBase,
                                                  int16_t iSourceStride,
                                                  arm_2d_size_t *
                                                  __restrict ptSourceSize,
                                                  uint8_t *
                                                  __restrict ptSourceMaskBase,
                                                  int16_t iSourceMaskStride,
                                                  arm_2d_size_t *
                                                  __restrict ptSourceMaskSize,
                                                  uint8_t *
                                                  __restrict ptTargetBase,
                                                  int16_t iTargetStride,
                                                  arm_2d_size_t *
                                                  __restrict ptTargetSize,
                                                  uint32_t *
                                                  __restrict ptTargetMaskBase,
                                                  int16_t iTargetMaskStride,
                                                  arm_2d_size_t *
                                                  __restrict ptTargetMaskSize);

void __arm_2d_impl_gray8_src_msk_des_chn_msk_fill_mirror(uint8_t *
                                                         __restrict
                                                         ptSourceBase,
                                                         int16_t iSourceStride,
                                                         arm_2d_size_t *
                                                         __restrict
                                                         ptSourceSize,
                                                         uint8_t *
                                                         __restrict
                                                         ptSourceMaskBase,
                                                         int16_t
                                                         iSourceMaskStride,
                                                         arm_2d_size_t *
                                                         __restrict
                                                         ptSourceMaskSize,
                                                         uint8_t *
                                                         __restrict
                                                         ptTargetBase,
                                                         int16_t iTargetStride,
                                                         arm_2d_size_t *
                                                         __restrict
                                                         ptTargetSize,
                                                         uint32_t *
                                                         __restrict
                                                         ptTargetMaskBase,
                                                         int16_t
                                                         iTargetMaskStride,
                                                         arm_2d_size_t *
                                                         __restrict
                                                         ptTargetMaskSize,
                                                         uint32_t wMode);

void __arm_2d_impl_gray8_src_msk_des_chn_msk_fill_x_mirror(uint8_t *
                                                           __restrict
                                                           ptSourceBase,
                                                           int16_t
                                                           iSourceStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptSourceSize,
                                                           uint8_t *
                                                           __restrict
                                                           ptSourceMaskBase,
                                                           int16_t
                                                           iSourceMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptSourceMaskSize,
                                                           uint8_t *
                                                           __restrict
                                                           ptTargetBase,
                                                           int16_t
                                                           iTargetStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptTargetSize,
                                                           uint32_t *
                                                           __restrict
                                                           ptTargetMaskBase,
                                                           int16_t
                                                           iTargetMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptTargetMaskSize);

void __arm_2d_impl_gray8_src_msk_des_chn_msk_fill_xy_mirror(uint8_t *
                                                            __restrict
                                                            ptSourceBase,
                                                            int16_t
                                                            iSourceStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceSize,
                                                            uint8_t *
                                                            __restrict
                                                            ptSourceMaskBase,
                                                            int16_t
                                                            iSourceMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceMaskSize,
                                                            uint8_t *
                                                            __restrict
                                                            ptTargetBase,
                                                            int16_t
                                                            iTargetStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetSize,
                                                            uint32_t *
                                                            __restrict
                                                            ptTargetMaskBase,
                                                            int16_t
                                                            iTargetMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetMaskSize);

void __arm_2d_impl_gray8_src_msk_des_chn_msk_fill_y_mirror(uint8_t *
                                                           __restrict
                                                           ptSourceBase,
                                                           int16_t
                                                           iSourceStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptSourceSize,
                                                           uint8_t *
                                                           __restrict
                                                           ptSourceMaskBase,
                                                           int16_t
                                                           iSourceMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptSourceMaskSize,
                                                           uint8_t *
                                                           __restrict
                                                           ptTargetBase,
                                                           int16_t
                                                           iTargetStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptTargetSize,
                                                           uint32_t *
                                                           __restrict
                                                           ptTargetMaskBase,
                                                           int16_t
                                                           iTargetMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptTargetMaskSize);

void __arm_2d_impl_gray8_src_msk_fill(uint8_t * __restrict ptSourceBase,
                                      int16_t iSourceStride,
                                      arm_2d_size_t * __restrict ptSourceSize,
                                      uint8_t * __restrict ptSourceMaskBase,
                                      int16_t iSourceMaskStride,
                                      arm_2d_size_t *
                                      __restrict ptSourceMaskSize,
                                      uint8_t * __restrict ptTargetBase,
                                      int16_t iTargetStride,
                                      arm_2d_size_t * __restrict ptTargetSize);

void __arm_2d_impl_gray8_src_msk_fill_mirror(uint8_t * __restrict ptSourceBase,
                                             int16_t iSourceStride,
                                             arm_2d_size_t *
                                             __restrict ptSourceSize,
                                             uint8_t *
                                             __restrict ptSourceMaskBase,
                                             int16_t iSourceMaskStride,
                                             arm_2d_size_t *
                                             __restrict ptSourceMaskSize,
                                             uint8_t * __restrict ptTargetBase,
                                             int16_t iTargetStride,
                                             arm_2d_size_t *
                                             __restrict ptTargetSize,
                                             uint32_t wMode);

void __arm_2d_impl_gray8_src_msk_fill_x_mirror(uint8_t *
                                               __restrict ptSourceBase,
                                               int16_t iSourceStride,
                                               arm_2d_size_t *
                                               __restrict ptSourceSize,
                                               uint8_t *
                                               __restrict ptSourceMaskBase,
                                               int16_t iSourceMaskStride,
                                               arm_2d_size_t *
                                               __restrict ptSourceMaskSize,
                                               uint8_t *
                                               __restrict ptTargetBase,
                                               int16_t iTargetStride,
                                               arm_2d_size_t *
                                               __restrict ptTargetSize);

void __arm_2d_impl_gray8_src_msk_fill_xy_mirror(uint8_t *
                                                __restrict ptSourceBase,
                                                int16_t iSourceStride,
                                                arm_2d_size_t *
                                                __restrict ptSourceSize,
                                                uint8_t *
                                                __restrict ptSourceMaskBase,
                                                int16_t iSourceMaskStride,
                                                arm_2d_size_t *
                                                __restrict ptSourceMaskSize,
                                                uint8_t *
                                                __restrict ptTargetBase,
                                                int16_t iTargetStride,
                                                arm_2d_size_t *
                                                __restrict ptTargetSize);

void __arm_2d_impl_gray8_src_msk_fill_y_mirror(uint8_t *
                                               __restrict ptSourceBase,
                                               int16_t iSourceStride,
                                               arm_2d_size_t *
                                               __restrict ptSourceSize,
                                               uint8_t *
                                               __restrict ptSourceMaskBase,
                                               int16_t iSourceMaskStride,
                                               arm_2d_size_t *
                                               __restrict ptSourceMaskSize,
                                               uint8_t *
                                               __restrict ptTargetBase,
                                               int16_t iTargetStride,
                                               arm_2d_size_t *
                                               __restrict ptTargetSize);

void __arm_2d_impl_gray8_transform( __arm_2d_param_copy_orig_t * ptParam,
                                    __arm_2d_transform_info_t * ptInfo);

void __arm_2d_impl_gray8_transform_with_opacity(
                                         __arm_2d_param_copy_orig_t * ptParam,
                                         __arm_2d_transform_info_t * ptInfo,
                                         uint_fast16_t hwRatio);

void __arm_2d_impl_gray8_transform_with_src_mask(
                                        __arm_2d_param_copy_orig_msk_t * ptParam,
                                        __arm_2d_transform_info_t * ptInfo);

void __arm_2d_impl_gray8_transform_with_src_mask_and_opacity(
                                        __arm_2d_param_copy_orig_msk_t * ptParam,
                                        __arm_2d_transform_info_t * ptInfo,
                                        uint_fast16_t hwRatio);

void __arm_2d_impl_gray8_transform_with_src_chn_mask(
                                        __arm_2d_param_copy_orig_msk_t * ptParam,
                                        __arm_2d_transform_info_t * ptInfo);

void __arm_2d_impl_gray8_transform_with_src_chn_mask_and_opacity(
                                        __arm_2d_param_copy_orig_msk_t * ptParam,
                                        __arm_2d_transform_info_t * ptInfo,
                                        uint_fast16_t hwRatio);

void __arm_2d_impl_rgb16_1x1_paving(const uint16_t * __restrict pSource,
                                    int16_t iSourceStride,
                                    uint16_t * __restrict pTarget,
                                    int16_t iTargetStride,
                                    const arm_2d_size_t *
                                    __restrict ptSrcCopySize,
                                    const arm_2d_size_t *
                                    __restrict ptDstCopySize);

void __arm_2d_impl_rgb16_1x1_paving_x_mirror(const uint16_t *
                                             __restrict pSource,
                                             int16_t iSourceStride,
                                             uint16_t * __restrict pTarget,
                                             int16_t iTargetStride,
                                             const arm_2d_size_t *
                                             __restrict ptSrcCopySize,
                                             const arm_2d_size_t *
                                             __restrict ptDstCopySize);

void __arm_2d_impl_rgb16_1x1_paving_xy_mirror(const uint16_t *
                                              __restrict pSource,
                                              int16_t iSourceStride,
                                              uint16_t * __restrict pTarget,
                                              int16_t iTargetStride,
                                              const arm_2d_size_t *
                                              __restrict ptSrcCopySize,
                                              const arm_2d_size_t *
                                              __restrict ptDstCopySize);

void __arm_2d_impl_rgb16_1x1_paving_y_mirror(const uint16_t *
                                             __restrict pSource,
                                             int16_t iSourceStride,
                                             uint16_t * __restrict pTarget,
                                             int16_t iTargetStride,
                                             const arm_2d_size_t *
                                             __restrict ptSrcCopySize,
                                             const arm_2d_size_t *
                                             __restrict ptDstCopySize);

void __arm_2d_impl_rgb16_1x2_paving(const uint16_t * __restrict pSourceBase,
                                    int16_t iSourceStride,
                                    const arm_2d_size_t *
                                    __restrict ptSourceSize,
                                    uint16_t * __restrict pTargetBase,
                                    int16_t iTargetStride, uint32_t destWidth,
                                    uint16_t tilePairRows);

void __arm_2d_impl_rgb16_1x2_paving_x_mirror(const uint16_t *
                                             __restrict pSourceBase,
                                             int16_t iSourceStride,
                                             const arm_2d_size_t *
                                             __restrict ptSourceSize,
                                             uint16_t * __restrict pTargetBase,
                                             int16_t iTargetStride,
                                             uint32_t destWidth,
                                             uint16_t tilePairRows);

void __arm_2d_impl_rgb16_1x2_paving_xy_mirror(const uint16_t *
                                              __restrict pSourceBase,
                                              int16_t iSourceStride,
                                              const arm_2d_size_t *
                                              __restrict ptSourceSize,
                                              uint16_t * __restrict pTargetBase,
                                              int16_t iTargetStride,
                                              uint32_t destWidth,
                                              uint16_t tilePairRows);

void __arm_2d_impl_rgb16_1x2_paving_y_mirror(const uint16_t *
                                             __restrict pSourceBase,
                                             int16_t iSourceStride,
                                             const arm_2d_size_t *
                                             __restrict ptSourceSize,
                                             uint16_t * __restrict pTargetBase,
                                             int16_t iTargetStride,
                                             uint32_t destWidth,
                                             uint16_t tilePairRows);

void __arm_2d_impl_rgb16_2x1_paving(const uint16_t * __restrict pSourceBase,
                                    int16_t iSourceStride,
                                    const arm_2d_size_t *
                                    __restrict ptSourceSize,
                                    uint16_t * __restrict pTargetBase,
                                    int16_t iTargetStride,
                                    uint16_t tilePairCols, uint16_t destHeight);

void __arm_2d_impl_rgb16_2x1_paving_x_mirror(const uint16_t *
                                             __restrict pSourceBase,
                                             int16_t iSourceStride,
                                             const arm_2d_size_t *
                                             __restrict ptSourceSize,
                                             uint16_t * __restrict pTargetBase,
                                             int16_t iTargetStride,
                                             uint16_t tilePairCols,
                                             uint16_t destHeight);

void __arm_2d_impl_rgb16_2x1_paving_xy_mirror(const uint16_t *
                                              __restrict pSourceBase,
                                              int16_t iSourceStride,
                                              const arm_2d_size_t *
                                              __restrict ptSourceSize,
                                              uint16_t * __restrict pTargetBase,
                                              int16_t iTargetStride,
                                              uint16_t tilePairCols,
                                              uint16_t destHeight);

void __arm_2d_impl_rgb16_2x1_paving_y_mirror(const uint16_t *
                                             __restrict pSourceBase,
                                             int16_t iSourceStride,
                                             const arm_2d_size_t *
                                             __restrict ptSourceSize,
                                             uint16_t * __restrict pTargetBase,
                                             int16_t iTargetStride,
                                             uint16_t tilePairCols,
                                             uint16_t destHeight);

void __arm_2d_impl_rgb16_2x2_paving(const uint16_t * __restrict pSourceBase,
                                    int16_t iSourceStride,
                                    const arm_2d_size_t *
                                    __restrict ptSourceSize,
                                    uint16_t * __restrict pTargetBase,
                                    int16_t iTargetStride,
                                    uint16_t tilePairRows,
                                    uint16_t tilePairCols);

void __arm_2d_impl_rgb16_2x2_paving_x_mirror(const uint16_t *
                                             __restrict pSourceBase,
                                             int16_t iSourceStride,
                                             const arm_2d_size_t *
                                             __restrict ptSourceSize,
                                             uint16_t * __restrict pTargetBase,
                                             int16_t iTargetStride,
                                             uint16_t tilePairRows,
                                             uint16_t tilePairCols);

void __arm_2d_impl_rgb16_2x2_paving_xy_mirror(const uint16_t *
                                              __restrict pSourceBase,
                                              int16_t iSourceStride,
                                              const arm_2d_size_t *
                                              __restrict ptSourceSize,
                                              uint16_t * __restrict pTargetBase,
                                              int16_t iTargetStride,
                                              uint16_t tilePairRows,
                                              uint16_t tilePairCols);

void __arm_2d_impl_rgb16_2x2_paving_y_mirror(const uint16_t *
                                             __restrict pSourceBase,
                                             int16_t iSourceStride,
                                             const arm_2d_size_t *
                                             __restrict ptSourceSize,
                                             uint16_t * __restrict pTargetBase,
                                             int16_t iTargetStride,
                                             uint16_t tilePairRows,
                                             uint16_t tilePairCols);

void __arm_2d_impl_rgb16_cl_key_1x1_paving(const uint16_t * __restrict pSource,
                                           int16_t iSourceStride,
                                           uint16_t * __restrict pTarget,
                                           int16_t iTargetStride,
                                           const arm_2d_size_t *
                                           __restrict ptSrcCopySize,
                                           const arm_2d_size_t *
                                           __restrict ptDstCopySize,
                                           uint16_t Colour);

void __arm_2d_impl_rgb16_cl_key_1x1_paving_x_mirror(const uint16_t *
                                                    __restrict pSource,
                                                    int16_t iSourceStride,
                                                    uint16_t *
                                                    __restrict pTarget,
                                                    int16_t iTargetStride,
                                                    const arm_2d_size_t *
                                                    __restrict ptSrcCopySize,
                                                    const arm_2d_size_t *
                                                    __restrict ptDstCopySize,
                                                    uint16_t Colour);

void __arm_2d_impl_rgb16_cl_key_1x1_paving_xy_mirror(const uint16_t *
                                                     __restrict pSource,
                                                     int16_t iSourceStride,
                                                     uint16_t *
                                                     __restrict pTarget,
                                                     int16_t iTargetStride,
                                                     const arm_2d_size_t *
                                                     __restrict ptSrcCopySize,
                                                     const arm_2d_size_t *
                                                     __restrict ptDstCopySize,
                                                     uint16_t Colour);

void __arm_2d_impl_rgb16_cl_key_1x1_paving_y_mirror(const uint16_t *
                                                    __restrict pSource,
                                                    int16_t iSourceStride,
                                                    uint16_t *
                                                    __restrict pTarget,
                                                    int16_t iTargetStride,
                                                    const arm_2d_size_t *
                                                    __restrict ptSrcCopySize,
                                                    const arm_2d_size_t *
                                                    __restrict ptDstCopySize,
                                                    uint16_t Colour);

void __arm_2d_impl_rgb16_cl_key_1x2_paving(const uint16_t *
                                           __restrict pSourceBase,
                                           int16_t iSourceStride,
                                           const arm_2d_size_t *
                                           __restrict ptSourceSize,
                                           uint16_t * __restrict pTargetBase,
                                           int16_t iTargetStride,
                                           uint32_t destWidth,
                                           uint16_t tilePairRows,
                                           uint16_t Colour);

void __arm_2d_impl_rgb16_cl_key_1x2_paving_x_mirror(const uint16_t *
                                                    __restrict pSourceBase,
                                                    int16_t iSourceStride,
                                                    const arm_2d_size_t *
                                                    __restrict ptSourceSize,
                                                    uint16_t *
                                                    __restrict pTargetBase,
                                                    int16_t iTargetStride,
                                                    uint32_t destWidth,
                                                    uint16_t tilePairRows,
                                                    uint16_t Colour);

void __arm_2d_impl_rgb16_cl_key_1x2_paving_xy_mirror(const uint16_t *
                                                     __restrict pSourceBase,
                                                     int16_t iSourceStride,
                                                     const arm_2d_size_t *
                                                     __restrict ptSourceSize,
                                                     uint16_t *
                                                     __restrict pTargetBase,
                                                     int16_t iTargetStride,
                                                     uint32_t destWidth,
                                                     uint16_t tilePairRows,
                                                     uint16_t Colour);

void __arm_2d_impl_rgb16_cl_key_1x2_paving_y_mirror(const uint16_t *
                                                    __restrict pSourceBase,
                                                    int16_t iSourceStride,
                                                    const arm_2d_size_t *
                                                    __restrict ptSourceSize,
                                                    uint16_t *
                                                    __restrict pTargetBase,
                                                    int16_t iTargetStride,
                                                    uint32_t destWidth,
                                                    uint16_t tilePairRows,
                                                    uint16_t Colour);

void __arm_2d_impl_rgb16_cl_key_2x1_paving(const uint16_t *
                                           __restrict pSourceBase,
                                           int16_t iSourceStride,
                                           const arm_2d_size_t *
                                           __restrict ptSourceSize,
                                           uint16_t * __restrict pTargetBase,
                                           int16_t iTargetStride,
                                           uint16_t tilePairCols,
                                           uint16_t destHeight,
                                           uint16_t Colour);

void __arm_2d_impl_rgb16_cl_key_2x1_paving_x_mirror(const uint16_t *
                                                    __restrict pSourceBase,
                                                    int16_t iSourceStride,
                                                    const arm_2d_size_t *
                                                    __restrict ptSourceSize,
                                                    uint16_t *
                                                    __restrict pTargetBase,
                                                    int16_t iTargetStride,
                                                    uint16_t tilePairCols,
                                                    uint16_t destHeight,
                                                    uint16_t Colour);

void __arm_2d_impl_rgb16_cl_key_2x1_paving_xy_mirror(const uint16_t *
                                                     __restrict pSourceBase,
                                                     int16_t iSourceStride,
                                                     const arm_2d_size_t *
                                                     __restrict ptSourceSize,
                                                     uint16_t *
                                                     __restrict pTargetBase,
                                                     int16_t iTargetStride,
                                                     uint16_t tilePairCols,
                                                     uint16_t destHeight,
                                                     uint16_t Colour);

void __arm_2d_impl_rgb16_cl_key_2x1_paving_y_mirror(const uint16_t *
                                                    __restrict pSourceBase,
                                                    int16_t iSourceStride,
                                                    const arm_2d_size_t *
                                                    __restrict ptSourceSize,
                                                    uint16_t *
                                                    __restrict pTargetBase,
                                                    int16_t iTargetStride,
                                                    uint16_t tilePairCols,
                                                    uint16_t destHeight,
                                                    uint16_t Colour);

void __arm_2d_impl_rgb16_cl_key_2x2_paving(const uint16_t *
                                           __restrict pSourceBase,
                                           int16_t iSourceStride,
                                           const arm_2d_size_t *
                                           __restrict ptSourceSize,
                                           uint16_t * __restrict pTargetBase,
                                           int16_t iTargetStride,
                                           uint16_t tilePairRows,
                                           uint16_t tilePairCols,
                                           uint16_t Colour);

void __arm_2d_impl_rgb16_cl_key_2x2_paving_x_mirror(const uint16_t *
                                                    __restrict pSourceBase,
                                                    int16_t iSourceStride,
                                                    const arm_2d_size_t *
                                                    __restrict ptSourceSize,
                                                    uint16_t *
                                                    __restrict pTargetBase,
                                                    int16_t iTargetStride,
                                                    uint16_t tilePairRows,
                                                    uint16_t tilePairCols,
                                                    uint16_t Colour);

void __arm_2d_impl_rgb16_cl_key_2x2_paving_xy_mirror(const uint16_t *
                                                     __restrict pSourceBase,
                                                     int16_t iSourceStride,
                                                     const arm_2d_size_t *
                                                     __restrict ptSourceSize,
                                                     uint16_t *
                                                     __restrict pTargetBase,
                                                     int16_t iTargetStride,
                                                     uint16_t tilePairRows,
                                                     uint16_t tilePairCols,
                                                     uint16_t Colour);

void __arm_2d_impl_rgb16_cl_key_2x2_paving_y_mirror(const uint16_t *
                                                    __restrict pSourceBase,
                                                    int16_t iSourceStride,
                                                    const arm_2d_size_t *
                                                    __restrict ptSourceSize,
                                                    uint16_t *
                                                    __restrict pTargetBase,
                                                    int16_t iTargetStride,
                                                    uint16_t tilePairRows,
                                                    uint16_t tilePairCols,
                                                    uint16_t Colour);

void __arm_2d_impl_rgb16_cl_key_copy(uint16_t * __restrict pSource,
                                     int16_t iSourceStride,
                                     uint16_t * __restrict pTarget,
                                     int16_t iTargetStride,
                                     arm_2d_size_t * __restrict ptCopySize,
                                     uint16_t Colour);

void __arm_2d_impl_rgb16_cl_key_copy_mirror(uint16_t * __restrict pSource,
                                            int16_t iSourceStride,
                                            uint16_t * __restrict pTarget,
                                            int16_t iTargetStride,
                                            arm_2d_size_t *
                                            __restrict ptCopySize,
                                            uint32_t wMode, uint16_t Colour);

void __arm_2d_impl_rgb16_cl_key_copy_x_mirror(uint16_t * __restrict pSource,
                                              int16_t iSourceStride,
                                              uint16_t * __restrict pTarget,
                                              int16_t iTargetStride,
                                              arm_2d_size_t *
                                              __restrict ptCopySize,
                                              uint16_t Colour);

void __arm_2d_impl_rgb16_cl_key_copy_xy_mirror(uint16_t * __restrict pSource,
                                               int16_t iSourceStride,
                                               uint16_t * __restrict pTarget,
                                               int16_t iTargetStride,
                                               arm_2d_size_t *
                                               __restrict ptCopySize,
                                               uint16_t Colour);

void __arm_2d_impl_rgb16_cl_key_copy_y_mirror(uint16_t * __restrict pSource,
                                              int16_t iSourceStride,
                                              uint16_t * __restrict pTarget,
                                              int16_t iTargetStride,
                                              arm_2d_size_t *
                                              __restrict ptCopySize,
                                              uint16_t Colour);

void __arm_2d_impl_rgb16_cl_key_fill(uint16_t * __restrict pSourceBase,
                                     int16_t iSourceStride,
                                     arm_2d_size_t * __restrict ptSourceSize,
                                     uint16_t * __restrict pTargetBase,
                                     int16_t iTargetStride,
                                     arm_2d_size_t * __restrict ptTargetSize,
                                     uint16_t Colour);

void __arm_2d_impl_rgb16_cl_key_fill_mirror(uint16_t * __restrict pSourceBase,
                                            int16_t iSourceStride,
                                            arm_2d_size_t *
                                            __restrict ptSourceSize,
                                            uint16_t * __restrict pTargetBase,
                                            int16_t iTargetStride,
                                            arm_2d_size_t *
                                            __restrict ptTargetSize,
                                            uint32_t wMode, uint16_t Colour);

void __arm_2d_impl_rgb16_cl_key_fill_x_mirror(uint16_t * __restrict pSourceBase,
                                              int16_t iSourceStride,
                                              arm_2d_size_t *
                                              __restrict ptSourceSize,
                                              uint16_t * __restrict pTargetBase,
                                              int16_t iTargetStride,
                                              arm_2d_size_t *
                                              __restrict ptTargetSize,
                                              uint16_t Colour);

void __arm_2d_impl_rgb16_cl_key_fill_xy_mirror(uint16_t *
                                               __restrict pSourceBase,
                                               int16_t iSourceStride,
                                               arm_2d_size_t *
                                               __restrict ptSourceSize,
                                               uint16_t *
                                               __restrict pTargetBase,
                                               int16_t iTargetStride,
                                               arm_2d_size_t *
                                               __restrict ptTargetSize,
                                               uint16_t Colour);

void __arm_2d_impl_rgb16_cl_key_fill_y_mirror(uint16_t * __restrict pSourceBase,
                                              int16_t iSourceStride,
                                              arm_2d_size_t *
                                              __restrict ptSourceSize,
                                              uint16_t * __restrict pTargetBase,
                                              int16_t iTargetStride,
                                              arm_2d_size_t *
                                              __restrict ptTargetSize,
                                              uint16_t Colour);

void __arm_2d_impl_rgb16_colour_filling(uint16_t * __restrict pTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t * __restrict ptCopySize,
                                        uint16_t Colour);

void __arm_2d_impl_rgb16_copy(uint16_t * __restrict pSource,
                              int16_t iSourceStride,
                              uint16_t * __restrict pTarget,
                              int16_t iTargetStride,
                              arm_2d_size_t * __restrict ptCopySize);

void __arm_2d_impl_rgb16_copy(uint16_t * phwSource, int16_t iSourceStride,
                              uint16_t * phwTarget, int16_t iTargetStride,
                              arm_2d_size_t * ptCopySize);

void __arm_2d_impl_rgb16_copy_mirror(uint16_t * __restrict pSource,
                                     int16_t iSourceStride,
                                     uint16_t * __restrict pTarget,
                                     int16_t iTargetStride,
                                     arm_2d_size_t * __restrict ptCopySize,
                                     uint32_t wMode);

void __arm_2d_impl_rgb16_copy_x_mirror(uint16_t * __restrict pSource,
                                       int16_t iSourceStride,
                                       uint16_t * __restrict pTarget,
                                       int16_t iTargetStride,
                                       arm_2d_size_t * __restrict ptCopySize);

void __arm_2d_impl_rgb16_copy_xy_mirror(uint16_t * __restrict pSource,
                                        int16_t iSourceStride,
                                        uint16_t * __restrict pTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t * __restrict ptCopySize);

void __arm_2d_impl_rgb16_copy_y_mirror(uint16_t * __restrict pSource,
                                       int16_t iSourceStride,
                                       uint16_t * __restrict pTarget,
                                       int16_t iTargetStride,
                                       arm_2d_size_t * __restrict ptCopySize);

void __arm_2d_impl_rgb16_draw_pattern(uint8_t * __restrict pchSourceBase,
                                      int32_t iOffset, int16_t iSourceStride,
                                      uint16_t * __restrict pTargetBase,
                                      int16_t iTargetStride,
                                      arm_2d_size_t * __restrict ptCopySize,
                                      uint8_t chMode, uint16_t ForeColour,
                                      uint16_t BackColour);

void __arm_2d_impl_rgb16_draw_pattern_bg_comp(uint8_t *
                                              __restrict pchSourceBase,
                                              int32_t iOffset,
                                              int16_t iSourceStride,
                                              uint16_t * __restrict pTargetBase,
                                              int16_t iTargetStride,
                                              arm_2d_size_t *
                                              __restrict ptCopySize,
                                              uint16_t BackColour);

void __arm_2d_impl_rgb16_draw_pattern_bg_fg(uint8_t * __restrict pchSourceBase,
                                            int32_t iOffset,
                                            int16_t iSourceStride,
                                            uint16_t * __restrict pTargetBase,
                                            int16_t iTargetStride,
                                            arm_2d_size_t *
                                            __restrict ptCopySize,
                                            uint16_t ForeColour,
                                            uint16_t BackColour);

void __arm_2d_impl_rgb16_draw_pattern_bg_only(uint8_t *
                                              __restrict pchSourceBase,
                                              int32_t iOffset,
                                              int16_t iSourceStride,
                                              uint16_t * __restrict pTargetBase,
                                              int16_t iTargetStride,
                                              arm_2d_size_t *
                                              __restrict ptCopySize,
                                              uint16_t BackColour);

void __arm_2d_impl_rgb16_draw_pattern_fg_only(uint8_t *
                                              __restrict pchSourceBase,
                                              int32_t iOffset,
                                              int16_t iSourceStride,
                                              uint16_t * __restrict pTargetBase,
                                              int16_t iTargetStride,
                                              arm_2d_size_t *
                                              __restrict ptCopySize,
                                              uint16_t ForeColour);

void __arm_2d_impl_rgb16_draw_pattern_no_bg_comp(uint8_t *
                                                 __restrict pchSourceBase,
                                                 int32_t iOffset,
                                                 int16_t iSourceStride,
                                                 uint16_t *
                                                 __restrict pTargetBase,
                                                 int16_t iTargetStride,
                                                 arm_2d_size_t *
                                                 __restrict ptCopySize);

void __arm_2d_impl_rgb16_fill(uint16_t * __restrict pSourceBase,
                              int16_t iSourceStride,
                              arm_2d_size_t * __restrict ptSourceSize,
                              uint16_t * __restrict pTargetBase,
                              int16_t iTargetStride,
                              arm_2d_size_t * __restrict ptTargetSize);

void __arm_2d_impl_rgb16_fill_mirror(uint16_t * __restrict pSourceBase,
                                     int16_t iSourceStride,
                                     arm_2d_size_t * __restrict ptSourceSize,
                                     uint16_t * __restrict pTargetBase,
                                     int16_t iTargetStride,
                                     arm_2d_size_t * __restrict ptTargetSize,
                                     uint32_t wMode);

void __arm_2d_impl_rgb16_fill_x_mirror(uint16_t * __restrict pSourceBase,
                                       int16_t iSourceStride,
                                       arm_2d_size_t * __restrict ptSourceSize,
                                       uint16_t * __restrict pTargetBase,
                                       int16_t iTargetStride,
                                       arm_2d_size_t * __restrict ptTargetSize);

void __arm_2d_impl_rgb16_fill_xy_mirror(uint16_t * __restrict pSourceBase,
                                        int16_t iSourceStride,
                                        arm_2d_size_t * __restrict ptSourceSize,
                                        uint16_t * __restrict pTargetBase,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *
                                        __restrict ptTargetSize);

void __arm_2d_impl_rgb16_fill_y_mirror(uint16_t * __restrict pSourceBase,
                                       int16_t iSourceStride,
                                       arm_2d_size_t * __restrict ptSourceSize,
                                       uint16_t * __restrict pTargetBase,
                                       int16_t iTargetStride,
                                       arm_2d_size_t * __restrict ptTargetSize);

void __arm_2d_impl_rgb32_1x1_paving(const uint32_t * __restrict pSource,
                                    int16_t iSourceStride,
                                    uint32_t * __restrict pTarget,
                                    int16_t iTargetStride,
                                    const arm_2d_size_t *
                                    __restrict ptSrcCopySize,
                                    const arm_2d_size_t *
                                    __restrict ptDstCopySize);

void __arm_2d_impl_rgb32_1x1_paving_x_mirror(const uint32_t *
                                             __restrict pSource,
                                             int16_t iSourceStride,
                                             uint32_t * __restrict pTarget,
                                             int16_t iTargetStride,
                                             const arm_2d_size_t *
                                             __restrict ptSrcCopySize,
                                             const arm_2d_size_t *
                                             __restrict ptDstCopySize);

void __arm_2d_impl_rgb32_1x1_paving_xy_mirror(const uint32_t *
                                              __restrict pSource,
                                              int16_t iSourceStride,
                                              uint32_t * __restrict pTarget,
                                              int16_t iTargetStride,
                                              const arm_2d_size_t *
                                              __restrict ptSrcCopySize,
                                              const arm_2d_size_t *
                                              __restrict ptDstCopySize);

void __arm_2d_impl_rgb32_1x1_paving_y_mirror(const uint32_t *
                                             __restrict pSource,
                                             int16_t iSourceStride,
                                             uint32_t * __restrict pTarget,
                                             int16_t iTargetStride,
                                             const arm_2d_size_t *
                                             __restrict ptSrcCopySize,
                                             const arm_2d_size_t *
                                             __restrict ptDstCopySize);

void __arm_2d_impl_rgb32_1x2_paving(const uint32_t * __restrict pSourceBase,
                                    int16_t iSourceStride,
                                    const arm_2d_size_t *
                                    __restrict ptSourceSize,
                                    uint32_t * __restrict pTargetBase,
                                    int16_t iTargetStride, uint32_t destWidth,
                                    uint16_t tilePairRows);

void __arm_2d_impl_rgb32_1x2_paving_x_mirror(const uint32_t *
                                             __restrict pSourceBase,
                                             int16_t iSourceStride,
                                             const arm_2d_size_t *
                                             __restrict ptSourceSize,
                                             uint32_t * __restrict pTargetBase,
                                             int16_t iTargetStride,
                                             uint32_t destWidth,
                                             uint16_t tilePairRows);

void __arm_2d_impl_rgb32_1x2_paving_xy_mirror(const uint32_t *
                                              __restrict pSourceBase,
                                              int16_t iSourceStride,
                                              const arm_2d_size_t *
                                              __restrict ptSourceSize,
                                              uint32_t * __restrict pTargetBase,
                                              int16_t iTargetStride,
                                              uint32_t destWidth,
                                              uint16_t tilePairRows);

void __arm_2d_impl_rgb32_1x2_paving_y_mirror(const uint32_t *
                                             __restrict pSourceBase,
                                             int16_t iSourceStride,
                                             const arm_2d_size_t *
                                             __restrict ptSourceSize,
                                             uint32_t * __restrict pTargetBase,
                                             int16_t iTargetStride,
                                             uint32_t destWidth,
                                             uint16_t tilePairRows);

void __arm_2d_impl_rgb32_2x1_paving(const uint32_t * __restrict pSourceBase,
                                    int16_t iSourceStride,
                                    const arm_2d_size_t *
                                    __restrict ptSourceSize,
                                    uint32_t * __restrict pTargetBase,
                                    int16_t iTargetStride,
                                    uint16_t tilePairCols, uint16_t destHeight);

void __arm_2d_impl_rgb32_2x1_paving_x_mirror(const uint32_t *
                                             __restrict pSourceBase,
                                             int16_t iSourceStride,
                                             const arm_2d_size_t *
                                             __restrict ptSourceSize,
                                             uint32_t * __restrict pTargetBase,
                                             int16_t iTargetStride,
                                             uint16_t tilePairCols,
                                             uint16_t destHeight);

void __arm_2d_impl_rgb32_2x1_paving_xy_mirror(const uint32_t *
                                              __restrict pSourceBase,
                                              int16_t iSourceStride,
                                              const arm_2d_size_t *
                                              __restrict ptSourceSize,
                                              uint32_t * __restrict pTargetBase,
                                              int16_t iTargetStride,
                                              uint16_t tilePairCols,
                                              uint16_t destHeight);

void __arm_2d_impl_rgb32_2x1_paving_y_mirror(const uint32_t *
                                             __restrict pSourceBase,
                                             int16_t iSourceStride,
                                             const arm_2d_size_t *
                                             __restrict ptSourceSize,
                                             uint32_t * __restrict pTargetBase,
                                             int16_t iTargetStride,
                                             uint16_t tilePairCols,
                                             uint16_t destHeight);

void __arm_2d_impl_rgb32_2x2_paving(const uint32_t * __restrict pSourceBase,
                                    int16_t iSourceStride,
                                    const arm_2d_size_t *
                                    __restrict ptSourceSize,
                                    uint32_t * __restrict pTargetBase,
                                    int16_t iTargetStride,
                                    uint16_t tilePairRows,
                                    uint16_t tilePairCols);

void __arm_2d_impl_rgb32_2x2_paving_x_mirror(const uint32_t *
                                             __restrict pSourceBase,
                                             int16_t iSourceStride,
                                             const arm_2d_size_t *
                                             __restrict ptSourceSize,
                                             uint32_t * __restrict pTargetBase,
                                             int16_t iTargetStride,
                                             uint16_t tilePairRows,
                                             uint16_t tilePairCols);

void __arm_2d_impl_rgb32_2x2_paving_xy_mirror(const uint32_t *
                                              __restrict pSourceBase,
                                              int16_t iSourceStride,
                                              const arm_2d_size_t *
                                              __restrict ptSourceSize,
                                              uint32_t * __restrict pTargetBase,
                                              int16_t iTargetStride,
                                              uint16_t tilePairRows,
                                              uint16_t tilePairCols);

void __arm_2d_impl_rgb32_2x2_paving_y_mirror(const uint32_t *
                                             __restrict pSourceBase,
                                             int16_t iSourceStride,
                                             const arm_2d_size_t *
                                             __restrict ptSourceSize,
                                             uint32_t * __restrict pTargetBase,
                                             int16_t iTargetStride,
                                             uint16_t tilePairRows,
                                             uint16_t tilePairCols);

void __arm_2d_impl_rgb32_cl_key_1x1_paving(const uint32_t * __restrict pSource,
                                           int16_t iSourceStride,
                                           uint32_t * __restrict pTarget,
                                           int16_t iTargetStride,
                                           const arm_2d_size_t *
                                           __restrict ptSrcCopySize,
                                           const arm_2d_size_t *
                                           __restrict ptDstCopySize,
                                           uint32_t Colour);

void __arm_2d_impl_rgb32_cl_key_1x1_paving_x_mirror(const uint32_t *
                                                    __restrict pSource,
                                                    int16_t iSourceStride,
                                                    uint32_t *
                                                    __restrict pTarget,
                                                    int16_t iTargetStride,
                                                    const arm_2d_size_t *
                                                    __restrict ptSrcCopySize,
                                                    const arm_2d_size_t *
                                                    __restrict ptDstCopySize,
                                                    uint32_t Colour);

void __arm_2d_impl_rgb32_cl_key_1x1_paving_xy_mirror(const uint32_t *
                                                     __restrict pSource,
                                                     int16_t iSourceStride,
                                                     uint32_t *
                                                     __restrict pTarget,
                                                     int16_t iTargetStride,
                                                     const arm_2d_size_t *
                                                     __restrict ptSrcCopySize,
                                                     const arm_2d_size_t *
                                                     __restrict ptDstCopySize,
                                                     uint32_t Colour);

void __arm_2d_impl_rgb32_cl_key_1x1_paving_y_mirror(const uint32_t *
                                                    __restrict pSource,
                                                    int16_t iSourceStride,
                                                    uint32_t *
                                                    __restrict pTarget,
                                                    int16_t iTargetStride,
                                                    const arm_2d_size_t *
                                                    __restrict ptSrcCopySize,
                                                    const arm_2d_size_t *
                                                    __restrict ptDstCopySize,
                                                    uint32_t Colour);

void __arm_2d_impl_rgb32_cl_key_1x2_paving(const uint32_t *
                                           __restrict pSourceBase,
                                           int16_t iSourceStride,
                                           const arm_2d_size_t *
                                           __restrict ptSourceSize,
                                           uint32_t * __restrict pTargetBase,
                                           int16_t iTargetStride,
                                           uint32_t destWidth,
                                           uint16_t tilePairRows,
                                           uint32_t Colour);

void __arm_2d_impl_rgb32_cl_key_1x2_paving_x_mirror(const uint32_t *
                                                    __restrict pSourceBase,
                                                    int16_t iSourceStride,
                                                    const arm_2d_size_t *
                                                    __restrict ptSourceSize,
                                                    uint32_t *
                                                    __restrict pTargetBase,
                                                    int16_t iTargetStride,
                                                    uint32_t destWidth,
                                                    uint16_t tilePairRows,
                                                    uint32_t Colour);

void __arm_2d_impl_rgb32_cl_key_1x2_paving_xy_mirror(const uint32_t *
                                                     __restrict pSourceBase,
                                                     int16_t iSourceStride,
                                                     const arm_2d_size_t *
                                                     __restrict ptSourceSize,
                                                     uint32_t *
                                                     __restrict pTargetBase,
                                                     int16_t iTargetStride,
                                                     uint32_t destWidth,
                                                     uint16_t tilePairRows,
                                                     uint32_t Colour);

void __arm_2d_impl_rgb32_cl_key_1x2_paving_y_mirror(const uint32_t *
                                                    __restrict pSourceBase,
                                                    int16_t iSourceStride,
                                                    const arm_2d_size_t *
                                                    __restrict ptSourceSize,
                                                    uint32_t *
                                                    __restrict pTargetBase,
                                                    int16_t iTargetStride,
                                                    uint32_t destWidth,
                                                    uint16_t tilePairRows,
                                                    uint32_t Colour);

void __arm_2d_impl_rgb32_cl_key_2x1_paving(const uint32_t *
                                           __restrict pSourceBase,
                                           int16_t iSourceStride,
                                           const arm_2d_size_t *
                                           __restrict ptSourceSize,
                                           uint32_t * __restrict pTargetBase,
                                           int16_t iTargetStride,
                                           uint16_t tilePairCols,
                                           uint16_t destHeight,
                                           uint32_t Colour);

void __arm_2d_impl_rgb32_cl_key_2x1_paving_x_mirror(const uint32_t *
                                                    __restrict pSourceBase,
                                                    int16_t iSourceStride,
                                                    const arm_2d_size_t *
                                                    __restrict ptSourceSize,
                                                    uint32_t *
                                                    __restrict pTargetBase,
                                                    int16_t iTargetStride,
                                                    uint16_t tilePairCols,
                                                    uint16_t destHeight,
                                                    uint32_t Colour);

void __arm_2d_impl_rgb32_cl_key_2x1_paving_xy_mirror(const uint32_t *
                                                     __restrict pSourceBase,
                                                     int16_t iSourceStride,
                                                     const arm_2d_size_t *
                                                     __restrict ptSourceSize,
                                                     uint32_t *
                                                     __restrict pTargetBase,
                                                     int16_t iTargetStride,
                                                     uint16_t tilePairCols,
                                                     uint16_t destHeight,
                                                     uint32_t Colour);

void __arm_2d_impl_rgb32_cl_key_2x1_paving_y_mirror(const uint32_t *
                                                    __restrict pSourceBase,
                                                    int16_t iSourceStride,
                                                    const arm_2d_size_t *
                                                    __restrict ptSourceSize,
                                                    uint32_t *
                                                    __restrict pTargetBase,
                                                    int16_t iTargetStride,
                                                    uint16_t tilePairCols,
                                                    uint16_t destHeight,
                                                    uint32_t Colour);

void __arm_2d_impl_rgb32_cl_key_2x2_paving(const uint32_t *
                                           __restrict pSourceBase,
                                           int16_t iSourceStride,
                                           const arm_2d_size_t *
                                           __restrict ptSourceSize,
                                           uint32_t * __restrict pTargetBase,
                                           int16_t iTargetStride,
                                           uint16_t tilePairRows,
                                           uint16_t tilePairCols,
                                           uint32_t Colour);

void __arm_2d_impl_rgb32_cl_key_2x2_paving_x_mirror(const uint32_t *
                                                    __restrict pSourceBase,
                                                    int16_t iSourceStride,
                                                    const arm_2d_size_t *
                                                    __restrict ptSourceSize,
                                                    uint32_t *
                                                    __restrict pTargetBase,
                                                    int16_t iTargetStride,
                                                    uint16_t tilePairRows,
                                                    uint16_t tilePairCols,
                                                    uint32_t Colour);

void __arm_2d_impl_rgb32_cl_key_2x2_paving_xy_mirror(const uint32_t *
                                                     __restrict pSourceBase,
                                                     int16_t iSourceStride,
                                                     const arm_2d_size_t *
                                                     __restrict ptSourceSize,
                                                     uint32_t *
                                                     __restrict pTargetBase,
                                                     int16_t iTargetStride,
                                                     uint16_t tilePairRows,
                                                     uint16_t tilePairCols,
                                                     uint32_t Colour);

void __arm_2d_impl_rgb32_cl_key_2x2_paving_y_mirror(const uint32_t *
                                                    __restrict pSourceBase,
                                                    int16_t iSourceStride,
                                                    const arm_2d_size_t *
                                                    __restrict ptSourceSize,
                                                    uint32_t *
                                                    __restrict pTargetBase,
                                                    int16_t iTargetStride,
                                                    uint16_t tilePairRows,
                                                    uint16_t tilePairCols,
                                                    uint32_t Colour);


void __arm_2d_impl_rgb32_cl_key_copy(uint32_t * __restrict pSource,
                                     int16_t iSourceStride,
                                     uint32_t * __restrict pTarget,
                                     int16_t iTargetStride,
                                     arm_2d_size_t * __restrict ptCopySize,
                                     uint32_t Colour);

void __arm_2d_impl_rgb32_cl_key_copy_mirror(uint32_t * __restrict pSource,
                                            int16_t iSourceStride,
                                            uint32_t * __restrict pTarget,
                                            int16_t iTargetStride,
                                            arm_2d_size_t *
                                            __restrict ptCopySize,
                                            uint32_t wMode, uint32_t Colour);

void __arm_2d_impl_rgb32_cl_key_copy_x_mirror(uint32_t * __restrict pSource,
                                              int16_t iSourceStride,
                                              uint32_t * __restrict pTarget,
                                              int16_t iTargetStride,
                                              arm_2d_size_t *
                                              __restrict ptCopySize,
                                              uint32_t Colour);

void __arm_2d_impl_rgb32_cl_key_copy_xy_mirror(uint32_t * __restrict pSource,
                                               int16_t iSourceStride,
                                               uint32_t * __restrict pTarget,
                                               int16_t iTargetStride,
                                               arm_2d_size_t *
                                               __restrict ptCopySize,
                                               uint32_t Colour);

void __arm_2d_impl_rgb32_cl_key_copy_y_mirror(uint32_t * __restrict pSource,
                                              int16_t iSourceStride,
                                              uint32_t * __restrict pTarget,
                                              int16_t iTargetStride,
                                              arm_2d_size_t *
                                              __restrict ptCopySize,
                                              uint32_t Colour);

void __arm_2d_impl_rgb32_cl_key_fill(uint32_t * __restrict pSourceBase,
                                     int16_t iSourceStride,
                                     arm_2d_size_t * __restrict ptSourceSize,
                                     uint32_t * __restrict pTargetBase,
                                     int16_t iTargetStride,
                                     arm_2d_size_t * __restrict ptTargetSize,
                                     uint32_t Colour);

void __arm_2d_impl_rgb32_cl_key_fill_mirror(uint32_t * __restrict pSourceBase,
                                            int16_t iSourceStride,
                                            arm_2d_size_t *
                                            __restrict ptSourceSize,
                                            uint32_t * __restrict pTargetBase,
                                            int16_t iTargetStride,
                                            arm_2d_size_t *
                                            __restrict ptTargetSize,
                                            uint32_t wMode, uint32_t Colour);

void __arm_2d_impl_rgb32_cl_key_fill_x_mirror(uint32_t * __restrict pSourceBase,
                                              int16_t iSourceStride,
                                              arm_2d_size_t *
                                              __restrict ptSourceSize,
                                              uint32_t * __restrict pTargetBase,
                                              int16_t iTargetStride,
                                              arm_2d_size_t *
                                              __restrict ptTargetSize,
                                              uint32_t Colour);

void __arm_2d_impl_rgb32_cl_key_fill_xy_mirror(uint32_t *
                                               __restrict pSourceBase,
                                               int16_t iSourceStride,
                                               arm_2d_size_t *
                                               __restrict ptSourceSize,
                                               uint32_t *
                                               __restrict pTargetBase,
                                               int16_t iTargetStride,
                                               arm_2d_size_t *
                                               __restrict ptTargetSize,
                                               uint32_t Colour);

void __arm_2d_impl_rgb32_cl_key_fill_y_mirror(uint32_t * __restrict pSourceBase,
                                              int16_t iSourceStride,
                                              arm_2d_size_t *
                                              __restrict ptSourceSize,
                                              uint32_t * __restrict pTargetBase,
                                              int16_t iTargetStride,
                                              arm_2d_size_t *
                                              __restrict ptTargetSize,
                                              uint32_t Colour);

void __arm_2d_impl_rgb32_colour_filling(uint32_t * __restrict pTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t * __restrict ptCopySize,
                                        uint32_t Colour);

void __arm_2d_impl_rgb32_copy(uint32_t * __restrict pSource,
                              int16_t iSourceStride,
                              uint32_t * __restrict pTarget,
                              int16_t iTargetStride,
                              arm_2d_size_t * __restrict ptCopySize);

void __arm_2d_impl_rgb32_copy(uint32_t * pwSource, int16_t iSourceStride,
                              uint32_t * pwTarget, int16_t iTargetStride,
                              arm_2d_size_t * ptCopySize);

void __arm_2d_impl_rgb32_copy_mirror(uint32_t * __restrict pSource,
                                     int16_t iSourceStride,
                                     uint32_t * __restrict pTarget,
                                     int16_t iTargetStride,
                                     arm_2d_size_t * __restrict ptCopySize,
                                     uint32_t wMode);

void __arm_2d_impl_rgb32_copy_x_mirror(uint32_t * __restrict pSource,
                                       int16_t iSourceStride,
                                       uint32_t * __restrict pTarget,
                                       int16_t iTargetStride,
                                       arm_2d_size_t * __restrict ptCopySize);

void __arm_2d_impl_rgb32_copy_xy_mirror(uint32_t * __restrict pSource,
                                        int16_t iSourceStride,
                                        uint32_t * __restrict pTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t * __restrict ptCopySize);

void __arm_2d_impl_rgb32_copy_y_mirror(uint32_t * __restrict pSource,
                                       int16_t iSourceStride,
                                       uint32_t * __restrict pTarget,
                                       int16_t iTargetStride,
                                       arm_2d_size_t * __restrict ptCopySize);

void __arm_2d_impl_rgb32_draw_pattern(uint8_t * __restrict pchSourceBase,
                                      int32_t iOffset, int16_t iSourceStride,
                                      uint32_t * __restrict pTargetBase,
                                      int16_t iTargetStride,
                                      arm_2d_size_t * __restrict ptCopySize,
                                      uint8_t chMode, uint32_t ForeColour,
                                      uint32_t BackColour);

void __arm_2d_impl_rgb32_draw_pattern_bg_comp(uint8_t *
                                              __restrict pchSourceBase,
                                              int32_t iOffset,
                                              int16_t iSourceStride,
                                              uint32_t * __restrict pTargetBase,
                                              int16_t iTargetStride,
                                              arm_2d_size_t *
                                              __restrict ptCopySize,
                                              uint32_t BackColour);

void __arm_2d_impl_rgb32_draw_pattern_bg_comp(uint8_t *
                                              __restrict pchSourceBase,
                                              int32_t iOffset,
                                              int16_t iSourceStride,
                                              uint32_t * __restrict pTargetBase,
                                              int16_t iTargetStride,
                                              arm_2d_size_t *
                                              __restrict ptCopySize,
                                              uint32_t hwBackColour);

void __arm_2d_impl_rgb32_draw_pattern_bg_fg(uint8_t * __restrict pchSourceBase,
                                            int32_t iOffset,
                                            int16_t iSourceStride,
                                            uint32_t * __restrict pTargetBase,
                                            int16_t iTargetStride,
                                            arm_2d_size_t *
                                            __restrict ptCopySize,
                                            uint32_t ForeColour,
                                            uint32_t BackColour);

void __arm_2d_impl_rgb32_draw_pattern_bg_fg(uint8_t * __restrict pchSourceBase,
                                            int32_t iOffset,
                                            int16_t iSourceStride,
                                            uint32_t * __restrict pTargetBase,
                                            int16_t iTargetStride,
                                            arm_2d_size_t *
                                            __restrict ptCopySize,
                                            uint32_t hwForeColour,
                                            uint32_t hwBackColour);

void __arm_2d_impl_rgb32_draw_pattern_bg_only(uint8_t *
                                              __restrict pchSourceBase,
                                              int32_t iOffset,
                                              int16_t iSourceStride,
                                              uint32_t * __restrict pTargetBase,
                                              int16_t iTargetStride,
                                              arm_2d_size_t *
                                              __restrict ptCopySize,
                                              uint32_t BackColour);

void __arm_2d_impl_rgb32_draw_pattern_bg_only(uint8_t *
                                              __restrict pchSourceBase,
                                              int32_t iOffset,
                                              int16_t iSourceStride,
                                              uint32_t * __restrict pTargetBase,
                                              int16_t iTargetStride,
                                              arm_2d_size_t *
                                              __restrict ptCopySize,
                                              uint32_t hwBackColour);

void __arm_2d_impl_rgb32_draw_pattern_fg_only(uint8_t *
                                              __restrict pchSourceBase,
                                              int32_t iOffset,
                                              int16_t iSourceStride,
                                              uint32_t * __restrict pTargetBase,
                                              int16_t iTargetStride,
                                              arm_2d_size_t *
                                              __restrict ptCopySize,
                                              uint32_t ForeColour);

void __arm_2d_impl_rgb32_draw_pattern_fg_only(uint8_t *
                                              __restrict pchSourceBase,
                                              int32_t iOffset,
                                              int16_t iSourceStride,
                                              uint32_t * __restrict pTargetBase,
                                              int16_t iTargetStride,
                                              arm_2d_size_t *
                                              __restrict ptCopySize,
                                              uint32_t hwForeColour);

void __arm_2d_impl_rgb32_draw_pattern_no_bg_comp(uint8_t *
                                                 __restrict pchSourceBase,
                                                 int32_t iOffset,
                                                 int16_t iSourceStride,
                                                 uint32_t *
                                                 __restrict pTargetBase,
                                                 int16_t iTargetStride,
                                                 arm_2d_size_t *
                                                 __restrict ptCopySize);

void __arm_2d_impl_rgb32_fill(uint32_t * __restrict pSourceBase,
                              int16_t iSourceStride,
                              arm_2d_size_t * __restrict ptSourceSize,
                              uint32_t * __restrict pTargetBase,
                              int16_t iTargetStride,
                              arm_2d_size_t * __restrict ptTargetSize);

void __arm_2d_impl_rgb32_fill_mirror(uint32_t * __restrict pSourceBase,
                                     int16_t iSourceStride,
                                     arm_2d_size_t * __restrict ptSourceSize,
                                     uint32_t * __restrict pTargetBase,
                                     int16_t iTargetStride,
                                     arm_2d_size_t * __restrict ptTargetSize,
                                     uint32_t wMode);

void __arm_2d_impl_rgb32_fill_x_mirror(uint32_t * __restrict pSourceBase,
                                       int16_t iSourceStride,
                                       arm_2d_size_t * __restrict ptSourceSize,
                                       uint32_t * __restrict pTargetBase,
                                       int16_t iTargetStride,
                                       arm_2d_size_t * __restrict ptTargetSize);

void __arm_2d_impl_rgb32_fill_xy_mirror(uint32_t * __restrict pSourceBase,
                                        int16_t iSourceStride,
                                        arm_2d_size_t * __restrict ptSourceSize,
                                        uint32_t * __restrict pTargetBase,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *
                                        __restrict ptTargetSize);

void __arm_2d_impl_rgb32_fill_y_mirror(uint32_t * __restrict pSourceBase,
                                       int16_t iSourceStride,
                                       arm_2d_size_t * __restrict ptSourceSize,
                                       uint32_t * __restrict pTargetBase,
                                       int16_t iTargetStride,
                                       arm_2d_size_t * __restrict ptTargetSize);

void __arm_2d_impl_rgb565_1h_des_msk_copy(uint16_t * __restrict pSourceBase,
                                          int16_t iSourceStride,
                                          uint16_t * __restrict pTargetBase,
                                          int16_t iTargetStride,
                                          uint8_t * __restrict ptTargetMaskBase,
                                          int16_t iTargetMaskStride,
                                          arm_2d_size_t *
                                          __restrict ptTargetMaskSize,
                                          arm_2d_size_t *
                                          __restrict ptCopySize);

void __arm_2d_impl_rgb565_1h_des_msk_copy_mirror(uint16_t *
                                                 __restrict pSourceBase,
                                                 int16_t iSourceStride,
                                                 uint16_t *
                                                 __restrict pTargetBase,
                                                 int16_t iTargetStride,
                                                 uint8_t *
                                                 __restrict ptTargetMaskBase,
                                                 int16_t iTargetMaskStride,
                                                 arm_2d_size_t *
                                                 __restrict ptTargetMaskSize,
                                                 arm_2d_size_t *
                                                 __restrict ptCopySize,
                                                 uint32_t wMode);

void __arm_2d_impl_rgb565_1h_des_msk_copy_x_mirror(uint16_t *
                                                   __restrict pSourceBase,
                                                   int16_t iSourceStride,
                                                   uint16_t *
                                                   __restrict pTargetBase,
                                                   int16_t iTargetStride,
                                                   uint8_t *
                                                   __restrict ptTargetMaskBase,
                                                   int16_t iTargetMaskStride,
                                                   arm_2d_size_t *
                                                   __restrict ptTargetMaskSize,
                                                   arm_2d_size_t *
                                                   __restrict ptCopySize);

void __arm_2d_impl_rgb565_1h_des_msk_copy_xy_mirror(uint16_t *
                                                    __restrict pSourceBase,
                                                    int16_t iSourceStride,
                                                    uint16_t *
                                                    __restrict pTargetBase,
                                                    int16_t iTargetStride,
                                                    uint8_t *
                                                    __restrict ptTargetMaskBase,
                                                    int16_t iTargetMaskStride,
                                                    arm_2d_size_t *
                                                    __restrict ptTargetMaskSize,
                                                    arm_2d_size_t *
                                                    __restrict ptCopySize);

void __arm_2d_impl_rgb565_1h_des_msk_copy_y_mirror(uint16_t *
                                                   __restrict pSourceBase,
                                                   int16_t iSourceStride,
                                                   uint16_t *
                                                   __restrict pTargetBase,
                                                   int16_t iTargetStride,
                                                   uint8_t *
                                                   __restrict ptTargetMaskBase,
                                                   int16_t iTargetMaskStride,
                                                   arm_2d_size_t *
                                                   __restrict ptTargetMaskSize,
                                                   arm_2d_size_t *
                                                   __restrict ptCopySize);

void __arm_2d_impl_rgb565_1h_des_msk_fill(uint16_t * __restrict ptSourceBase,
                                          int16_t iSourceStride,
                                          arm_2d_size_t *
                                          __restrict ptSourceSize,
                                          uint16_t * __restrict ptTargetBase,
                                          int16_t iTargetStride,
                                          arm_2d_size_t *
                                          __restrict ptTargetSize,
                                          uint8_t * __restrict ptTargetMaskBase,
                                          int16_t iTargetMaskStride,
                                          arm_2d_size_t *
                                          __restrict ptTargetMaskSize);

void __arm_2d_impl_rgb565_1h_des_msk_fill_mirror(uint16_t *
                                                 __restrict ptSourceBase,
                                                 int16_t iSourceStride,
                                                 arm_2d_size_t *
                                                 __restrict ptSourceSize,
                                                 uint16_t *
                                                 __restrict ptTargetBase,
                                                 int16_t iTargetStride,
                                                 arm_2d_size_t *
                                                 __restrict ptTargetSize,
                                                 uint8_t *
                                                 __restrict ptTargetMaskBase,
                                                 int16_t iTargetMaskStride,
                                                 arm_2d_size_t *
                                                 __restrict ptTargetMaskSize,
                                                 uint32_t wMode);

void __arm_2d_impl_rgb565_1h_des_msk_fill_x_mirror(uint16_t *
                                                   __restrict ptSourceBase,
                                                   int16_t iSourceStride,
                                                   arm_2d_size_t *
                                                   __restrict ptSourceSize,
                                                   uint16_t *
                                                   __restrict ptTargetBase,
                                                   int16_t iTargetStride,
                                                   arm_2d_size_t *
                                                   __restrict ptTargetSize,
                                                   uint8_t *
                                                   __restrict ptTargetMaskBase,
                                                   int16_t iTargetMaskStride,
                                                   arm_2d_size_t *
                                                   __restrict ptTargetMaskSize);

void __arm_2d_impl_rgb565_1h_des_msk_fill_xy_mirror(uint16_t *
                                                    __restrict ptSourceBase,
                                                    int16_t iSourceStride,
                                                    arm_2d_size_t *
                                                    __restrict ptSourceSize,
                                                    uint16_t *
                                                    __restrict ptTargetBase,
                                                    int16_t iTargetStride,
                                                    arm_2d_size_t *
                                                    __restrict ptTargetSize,
                                                    uint8_t *
                                                    __restrict ptTargetMaskBase,
                                                    int16_t iTargetMaskStride,
                                                    arm_2d_size_t *
                                                    __restrict
                                                    ptTargetMaskSize);

void __arm_2d_impl_rgb565_1h_des_msk_fill_y_mirror(uint16_t *
                                                   __restrict ptSourceBase,
                                                   int16_t iSourceStride,
                                                   arm_2d_size_t *
                                                   __restrict ptSourceSize,
                                                   uint16_t *
                                                   __restrict ptTargetBase,
                                                   int16_t iTargetStride,
                                                   arm_2d_size_t *
                                                   __restrict ptTargetSize,
                                                   uint8_t *
                                                   __restrict ptTargetMaskBase,
                                                   int16_t iTargetMaskStride,
                                                   arm_2d_size_t *
                                                   __restrict ptTargetMaskSize);

void __arm_2d_impl_rgb565_alpha_blending(uint16_t * __restrict phwSourceBase,
                                         int16_t iSourceStride,
                                         uint16_t * __restrict phwTargetBase,
                                         int16_t iTargetStride,
                                         arm_2d_size_t * __restrict ptCopySize,
                                         uint_fast16_t hwRatio);

void __arm_2d_impl_rgb565_alpha_blending(uint16_t * phwSourceBase,
                                         int16_t iSourceStride,
                                         uint16_t * phwTargetBase,
                                         int16_t iTargetStride,
                                         arm_2d_size_t * ptCopySize,
                                         uint_fast16_t hwRatio);

void __arm_2d_impl_rgb565_alpha_blending_colour_keying(uint16_t *
                                                       __restrict pSourceBase,
                                                       int16_t iSourceStride,
                                                       uint16_t *
                                                       __restrict pTargetBase,
                                                       int16_t iTargetStride,
                                                       arm_2d_size_t *
                                                       __restrict ptCopySize,
                                                       uint_fast16_t hwRatio,
                                                       uint16_t hwColour);

void __arm_2d_impl_rgb565_colour_filling_channel_mask(uint16_t *
                                                      __restrict pTarget,
                                                      int16_t iTargetStride,
                                                      uint32_t *
                                                      __restrict pwAlpha,
                                                      int16_t iAlphaStride,
                                                      arm_2d_size_t *
                                                      __restrict ptCopySize,
                                                      uint16_t Colour);

void __arm_2d_impl_rgb565_colour_filling_channel_mask_opacity(uint16_t *
                                                      __restrict
                                                      pTarget,
                                                      int16_t
                                                      iTargetStride,
                                                      uint32_t *
                                                      __restrict
                                                      pwAlpha,
                                                      int16_t
                                                      iAlphaStride,
                                                      arm_2d_size_t *
                                                      __restrict
                                                      ptCopySize,
                                                      uint16_t Colour,
                                                      uint_fast16_t hwOpacity);

void __arm_2d_impl_rgb565_colour_filling_mask(uint16_t * __restrict pTarget,
                                              int16_t iTargetStride,
                                              uint8_t * __restrict pchAlpha,
                                              int16_t iAlphaStride,
                                              arm_2d_size_t *
                                              __restrict ptCopySize,
                                              uint16_t Colour);

void __arm_2d_impl_rgb565_colour_filling_mask_opacity(uint16_t *
                                                      __restrict pTarget,
                                                      int16_t iTargetStride,
                                                      uint8_t *
                                                      __restrict pchAlpha,
                                                      int16_t iAlphaStride,
                                                      arm_2d_size_t *
                                                      __restrict ptCopySize,
                                                      uint16_t Colour,
                                                      uint_fast16_t hwOpacity);

void __arm_2d_impl_rgb565_colour_filling_with_opacity(uint16_t *
                                                      __restrict pTargetBase,
                                                      int16_t iTargetStride,
                                                      arm_2d_size_t *
                                                      __restrict ptCopySize,
                                                      uint16_t Colour,
                                                      uint_fast16_t hwRatio);

void __arm_2d_impl_rgb565_des_chn_msk_copy(uint16_t * __restrict pSourceBase,
                                           int16_t iSourceStride,
                                           uint16_t * __restrict pTargetBase,
                                           int16_t iTargetStride,
                                           uint32_t *
                                           __restrict ptTargetMaskBase,
                                           int16_t iTargetMaskStride,
                                           arm_2d_size_t *
                                           __restrict ptTargetMaskSize,
                                           arm_2d_size_t *
                                           __restrict ptCopySize);

void __arm_2d_impl_rgb565_des_chn_msk_copy_mirror(uint16_t *
                                                  __restrict pSourceBase,
                                                  int16_t iSourceStride,
                                                  uint16_t *
                                                  __restrict pTargetBase,
                                                  int16_t iTargetStride,
                                                  uint32_t *
                                                  __restrict ptTargetMaskBase,
                                                  int16_t iTargetMaskStride,
                                                  arm_2d_size_t *
                                                  __restrict ptTargetMaskSize,
                                                  arm_2d_size_t *
                                                  __restrict ptCopySize,
                                                  uint32_t wMode);

void __arm_2d_impl_rgb565_des_chn_msk_copy_x_mirror(uint16_t *
                                                    __restrict pSourceBase,
                                                    int16_t iSourceStride,
                                                    uint16_t *
                                                    __restrict pTargetBase,
                                                    int16_t iTargetStride,
                                                    uint32_t *
                                                    __restrict ptTargetMaskBase,
                                                    int16_t iTargetMaskStride,
                                                    arm_2d_size_t *
                                                    __restrict ptTargetMaskSize,
                                                    arm_2d_size_t *
                                                    __restrict ptCopySize);

void __arm_2d_impl_rgb565_des_chn_msk_copy_xy_mirror(uint16_t *
                                                     __restrict pSourceBase,
                                                     int16_t iSourceStride,
                                                     uint16_t *
                                                     __restrict pTargetBase,
                                                     int16_t iTargetStride,
                                                     uint32_t *
                                                     __restrict
                                                     ptTargetMaskBase,
                                                     int16_t iTargetMaskStride,
                                                     arm_2d_size_t *
                                                     __restrict
                                                     ptTargetMaskSize,
                                                     arm_2d_size_t *
                                                     __restrict ptCopySize);

void __arm_2d_impl_rgb565_des_chn_msk_copy_y_mirror(uint16_t *
                                                    __restrict pSourceBase,
                                                    int16_t iSourceStride,
                                                    uint16_t *
                                                    __restrict pTargetBase,
                                                    int16_t iTargetStride,
                                                    uint32_t *
                                                    __restrict ptTargetMaskBase,
                                                    int16_t iTargetMaskStride,
                                                    arm_2d_size_t *
                                                    __restrict ptTargetMaskSize,
                                                    arm_2d_size_t *
                                                    __restrict ptCopySize);

void __arm_2d_impl_rgb565_des_chn_msk_fill(uint16_t * __restrict ptSourceBase,
                                           int16_t iSourceStride,
                                           arm_2d_size_t *
                                           __restrict ptSourceSize,
                                           uint16_t * __restrict ptTargetBase,
                                           int16_t iTargetStride,
                                           arm_2d_size_t *
                                           __restrict ptTargetSize,
                                           uint32_t *
                                           __restrict ptTargetMaskBase,
                                           int16_t iTargetMaskStride,
                                           arm_2d_size_t *
                                           __restrict ptTargetMaskSize);

void __arm_2d_impl_rgb565_des_chn_msk_fill_mirror(uint16_t *
                                                  __restrict ptSourceBase,
                                                  int16_t iSourceStride,
                                                  arm_2d_size_t *
                                                  __restrict ptSourceSize,
                                                  uint16_t *
                                                  __restrict ptTargetBase,
                                                  int16_t iTargetStride,
                                                  arm_2d_size_t *
                                                  __restrict ptTargetSize,
                                                  uint32_t *
                                                  __restrict ptTargetMaskBase,
                                                  int16_t iTargetMaskStride,
                                                  arm_2d_size_t *
                                                  __restrict ptTargetMaskSize,
                                                  uint32_t wMode);

void __arm_2d_impl_rgb565_des_chn_msk_fill_x_mirror(uint16_t *
                                                    __restrict ptSourceBase,
                                                    int16_t iSourceStride,
                                                    arm_2d_size_t *
                                                    __restrict ptSourceSize,
                                                    uint16_t *
                                                    __restrict ptTargetBase,
                                                    int16_t iTargetStride,
                                                    arm_2d_size_t *
                                                    __restrict ptTargetSize,
                                                    uint32_t *
                                                    __restrict ptTargetMaskBase,
                                                    int16_t iTargetMaskStride,
                                                    arm_2d_size_t *
                                                    __restrict
                                                    ptTargetMaskSize);

void __arm_2d_impl_rgb565_des_chn_msk_fill_xy_mirror(uint16_t *
                                                     __restrict ptSourceBase,
                                                     int16_t iSourceStride,
                                                     arm_2d_size_t *
                                                     __restrict ptSourceSize,
                                                     uint16_t *
                                                     __restrict ptTargetBase,
                                                     int16_t iTargetStride,
                                                     arm_2d_size_t *
                                                     __restrict ptTargetSize,
                                                     uint32_t *
                                                     __restrict
                                                     ptTargetMaskBase,
                                                     int16_t iTargetMaskStride,
                                                     arm_2d_size_t *
                                                     __restrict
                                                     ptTargetMaskSize);

void __arm_2d_impl_rgb565_des_chn_msk_fill_y_mirror(uint16_t *
                                                    __restrict ptSourceBase,
                                                    int16_t iSourceStride,
                                                    arm_2d_size_t *
                                                    __restrict ptSourceSize,
                                                    uint16_t *
                                                    __restrict ptTargetBase,
                                                    int16_t iTargetStride,
                                                    arm_2d_size_t *
                                                    __restrict ptTargetSize,
                                                    uint32_t *
                                                    __restrict ptTargetMaskBase,
                                                    int16_t iTargetMaskStride,
                                                    arm_2d_size_t *
                                                    __restrict
                                                    ptTargetMaskSize);

void __arm_2d_impl_rgb565_des_msk_copy(uint16_t * __restrict pSourceBase,
                                       int16_t iSourceStride,
                                       uint16_t * __restrict pTargetBase,
                                       int16_t iTargetStride,
                                       uint8_t * __restrict ptTargetMaskBase,
                                       int16_t iTargetMaskStride,
                                       arm_2d_size_t *
                                       __restrict ptTargetMaskSize,
                                       arm_2d_size_t * __restrict ptCopySize);

void __arm_2d_impl_rgb565_des_msk_copy_mirror(uint16_t * __restrict pSourceBase,
                                              int16_t iSourceStride,
                                              uint16_t * __restrict pTargetBase,
                                              int16_t iTargetStride,
                                              uint8_t *
                                              __restrict ptTargetMaskBase,
                                              int16_t iTargetMaskStride,
                                              arm_2d_size_t *
                                              __restrict ptTargetMaskSize,
                                              arm_2d_size_t *
                                              __restrict ptCopySize,
                                              uint32_t wMode);

void __arm_2d_impl_rgb565_des_msk_copy_x_mirror(uint16_t *
                                                __restrict pSourceBase,
                                                int16_t iSourceStride,
                                                uint16_t *
                                                __restrict pTargetBase,
                                                int16_t iTargetStride,
                                                uint8_t *
                                                __restrict ptTargetMaskBase,
                                                int16_t iTargetMaskStride,
                                                arm_2d_size_t *
                                                __restrict ptTargetMaskSize,
                                                arm_2d_size_t *
                                                __restrict ptCopySize);

void __arm_2d_impl_rgb565_des_msk_copy_xy_mirror(uint16_t *
                                                 __restrict pSourceBase,
                                                 int16_t iSourceStride,
                                                 uint16_t *
                                                 __restrict pTargetBase,
                                                 int16_t iTargetStride,
                                                 uint8_t *
                                                 __restrict ptTargetMaskBase,
                                                 int16_t iTargetMaskStride,
                                                 arm_2d_size_t *
                                                 __restrict ptTargetMaskSize,
                                                 arm_2d_size_t *
                                                 __restrict ptCopySize);

void __arm_2d_impl_rgb565_des_msk_copy_y_mirror(uint16_t *
                                                __restrict pSourceBase,
                                                int16_t iSourceStride,
                                                uint16_t *
                                                __restrict pTargetBase,
                                                int16_t iTargetStride,
                                                uint8_t *
                                                __restrict ptTargetMaskBase,
                                                int16_t iTargetMaskStride,
                                                arm_2d_size_t *
                                                __restrict ptTargetMaskSize,
                                                arm_2d_size_t *
                                                __restrict ptCopySize);

void __arm_2d_impl_rgb565_des_msk_fill(uint16_t * __restrict ptSourceBase,
                                       int16_t iSourceStride,
                                       arm_2d_size_t * __restrict ptSourceSize,
                                       uint16_t * __restrict ptTargetBase,
                                       int16_t iTargetStride,
                                       arm_2d_size_t * __restrict ptTargetSize,
                                       uint8_t * __restrict ptTargetMaskBase,
                                       int16_t iTargetMaskStride,
                                       arm_2d_size_t *
                                       __restrict ptTargetMaskSize);

void __arm_2d_impl_rgb565_des_msk_fill_mirror(uint16_t *
                                              __restrict ptSourceBase,
                                              int16_t iSourceStride,
                                              arm_2d_size_t *
                                              __restrict ptSourceSize,
                                              uint16_t *
                                              __restrict ptTargetBase,
                                              int16_t iTargetStride,
                                              arm_2d_size_t *
                                              __restrict ptTargetSize,
                                              uint8_t *
                                              __restrict ptTargetMaskBase,
                                              int16_t iTargetMaskStride,
                                              arm_2d_size_t *
                                              __restrict ptTargetMaskSize,
                                              uint32_t wMode);

void __arm_2d_impl_rgb565_des_msk_fill_x_mirror(uint16_t *
                                                __restrict ptSourceBase,
                                                int16_t iSourceStride,
                                                arm_2d_size_t *
                                                __restrict ptSourceSize,
                                                uint16_t *
                                                __restrict ptTargetBase,
                                                int16_t iTargetStride,
                                                arm_2d_size_t *
                                                __restrict ptTargetSize,
                                                uint8_t *
                                                __restrict ptTargetMaskBase,
                                                int16_t iTargetMaskStride,
                                                arm_2d_size_t *
                                                __restrict ptTargetMaskSize);

void __arm_2d_impl_rgb565_des_msk_fill_xy_mirror(uint16_t *
                                                 __restrict ptSourceBase,
                                                 int16_t iSourceStride,
                                                 arm_2d_size_t *
                                                 __restrict ptSourceSize,
                                                 uint16_t *
                                                 __restrict ptTargetBase,
                                                 int16_t iTargetStride,
                                                 arm_2d_size_t *
                                                 __restrict ptTargetSize,
                                                 uint8_t *
                                                 __restrict ptTargetMaskBase,
                                                 int16_t iTargetMaskStride,
                                                 arm_2d_size_t *
                                                 __restrict ptTargetMaskSize);

void __arm_2d_impl_rgb565_des_msk_fill_y_mirror(uint16_t *
                                                __restrict ptSourceBase,
                                                int16_t iSourceStride,
                                                arm_2d_size_t *
                                                __restrict ptSourceSize,
                                                uint16_t *
                                                __restrict ptTargetBase,
                                                int16_t iTargetStride,
                                                arm_2d_size_t *
                                                __restrict ptTargetSize,
                                                uint8_t *
                                                __restrict ptTargetMaskBase,
                                                int16_t iTargetMaskStride,
                                                arm_2d_size_t *
                                                __restrict ptTargetMaskSize);

void __arm_2d_impl_rgb565_masks_copy(uint16_t * __restrict pSourceBase,
                                     int16_t iSourceStride,
                                     uint8_t * __restrict ptSourceMaskBase,
                                     int16_t iSourceMaskStride,
                                     arm_2d_size_t *
                                     __restrict ptSourceMaskSize,
                                     uint16_t * __restrict pTargetBase,
                                     int16_t iTargetStride,
                                     uint8_t * __restrict ptTargetMaskBase,
                                     int16_t iTargetMaskStride,
                                     arm_2d_size_t *
                                     __restrict ptTargetMaskSize,
                                     arm_2d_size_t * __restrict ptCopySize);

void __arm_2d_impl_rgb565_masks_copy_mirror(uint16_t * __restrict pSourceBase,
                                            int16_t iSourceStride,
                                            uint8_t *
                                            __restrict ptSourceMaskBase,
                                            int16_t iSourceMaskStride,
                                            arm_2d_size_t *
                                            __restrict ptSourceMaskSize,
                                            uint16_t * __restrict pTargetBase,
                                            int16_t iTargetStride,
                                            uint8_t *
                                            __restrict ptTargetMaskBase,
                                            int16_t iTargetMaskStride,
                                            arm_2d_size_t *
                                            __restrict ptTargetMaskSize,
                                            arm_2d_size_t *
                                            __restrict ptCopySize,
                                            uint32_t wMode);

void __arm_2d_impl_rgb565_masks_copy_x_mirror(uint16_t * __restrict pSourceBase,
                                              int16_t iSourceStride,
                                              uint8_t *
                                              __restrict ptSourceMaskBase,
                                              int16_t iSourceMaskStride,
                                              arm_2d_size_t *
                                              __restrict ptSourceMaskSize,
                                              uint16_t * __restrict pTargetBase,
                                              int16_t iTargetStride,
                                              uint8_t *
                                              __restrict ptTargetMaskBase,
                                              int16_t iTargetMaskStride,
                                              arm_2d_size_t *
                                              __restrict ptTargetMaskSize,
                                              arm_2d_size_t *
                                              __restrict ptCopySize);

void __arm_2d_impl_rgb565_masks_copy_xy_mirror(uint16_t *
                                               __restrict pSourceBase,
                                               int16_t iSourceStride,
                                               uint8_t *
                                               __restrict ptSourceMaskBase,
                                               int16_t iSourceMaskStride,
                                               arm_2d_size_t *
                                               __restrict ptSourceMaskSize,
                                               uint16_t *
                                               __restrict pTargetBase,
                                               int16_t iTargetStride,
                                               uint8_t *
                                               __restrict ptTargetMaskBase,
                                               int16_t iTargetMaskStride,
                                               arm_2d_size_t *
                                               __restrict ptTargetMaskSize,
                                               arm_2d_size_t *
                                               __restrict ptCopySize);

void __arm_2d_impl_rgb565_masks_copy_y_mirror(uint16_t * __restrict pSourceBase,
                                              int16_t iSourceStride,
                                              uint8_t *
                                              __restrict ptSourceMaskBase,
                                              int16_t iSourceMaskStride,
                                              arm_2d_size_t *
                                              __restrict ptSourceMaskSize,
                                              uint16_t * __restrict pTargetBase,
                                              int16_t iTargetStride,
                                              uint8_t *
                                              __restrict ptTargetMaskBase,
                                              int16_t iTargetMaskStride,
                                              arm_2d_size_t *
                                              __restrict ptTargetMaskSize,
                                              arm_2d_size_t *
                                              __restrict ptCopySize);

void __arm_2d_impl_rgb565_masks_fill(uint16_t * __restrict ptSourceBase,
                                     int16_t iSourceStride,
                                     arm_2d_size_t * __restrict ptSourceSize,
                                     uint8_t * __restrict ptSourceMaskBase,
                                     int16_t iSourceMaskStride,
                                     arm_2d_size_t *
                                     __restrict ptSourceMaskSize,
                                     uint16_t * __restrict ptTargetBase,
                                     int16_t iTargetStride,
                                     arm_2d_size_t * __restrict ptTargetSize,
                                     uint8_t * __restrict ptTargetMaskBase,
                                     int16_t iTargetMaskStride,
                                     arm_2d_size_t *
                                     __restrict ptTargetMaskSize);

void __arm_2d_impl_rgb565_masks_fill_mirror(uint16_t * __restrict ptSourceBase,
                                            int16_t iSourceStride,
                                            arm_2d_size_t *
                                            __restrict ptSourceSize,
                                            uint8_t *
                                            __restrict ptSourceMaskBase,
                                            int16_t iSourceMaskStride,
                                            arm_2d_size_t *
                                            __restrict ptSourceMaskSize,
                                            uint16_t * __restrict ptTargetBase,
                                            int16_t iTargetStride,
                                            arm_2d_size_t *
                                            __restrict ptTargetSize,
                                            uint8_t *
                                            __restrict ptTargetMaskBase,
                                            int16_t iTargetMaskStride,
                                            arm_2d_size_t *
                                            __restrict ptTargetMaskSize,
                                            uint32_t wMode);

void __arm_2d_impl_rgb565_masks_fill_x_mirror(uint16_t *
                                              __restrict ptSourceBase,
                                              int16_t iSourceStride,
                                              arm_2d_size_t *
                                              __restrict ptSourceSize,
                                              uint8_t *
                                              __restrict ptSourceMaskBase,
                                              int16_t iSourceMaskStride,
                                              arm_2d_size_t *
                                              __restrict ptSourceMaskSize,
                                              uint16_t *
                                              __restrict ptTargetBase,
                                              int16_t iTargetStride,
                                              arm_2d_size_t *
                                              __restrict ptTargetSize,
                                              uint8_t *
                                              __restrict ptTargetMaskBase,
                                              int16_t iTargetMaskStride,
                                              arm_2d_size_t *
                                              __restrict ptTargetMaskSize);

void __arm_2d_impl_rgb565_masks_fill_xy_mirror(uint16_t *
                                               __restrict ptSourceBase,
                                               int16_t iSourceStride,
                                               arm_2d_size_t *
                                               __restrict ptSourceSize,
                                               uint8_t *
                                               __restrict ptSourceMaskBase,
                                               int16_t iSourceMaskStride,
                                               arm_2d_size_t *
                                               __restrict ptSourceMaskSize,
                                               uint16_t *
                                               __restrict ptTargetBase,
                                               int16_t iTargetStride,
                                               arm_2d_size_t *
                                               __restrict ptTargetSize,
                                               uint8_t *
                                               __restrict ptTargetMaskBase,
                                               int16_t iTargetMaskStride,
                                               arm_2d_size_t *
                                               __restrict ptTargetMaskSize);

void __arm_2d_impl_rgb565_masks_fill_y_mirror(uint16_t *
                                              __restrict ptSourceBase,
                                              int16_t iSourceStride,
                                              arm_2d_size_t *
                                              __restrict ptSourceSize,
                                              uint8_t *
                                              __restrict ptSourceMaskBase,
                                              int16_t iSourceMaskStride,
                                              arm_2d_size_t *
                                              __restrict ptSourceMaskSize,
                                              uint16_t *
                                              __restrict ptTargetBase,
                                              int16_t iTargetStride,
                                              arm_2d_size_t *
                                              __restrict ptTargetSize,
                                              uint8_t *
                                              __restrict ptTargetMaskBase,
                                              int16_t iTargetMaskStride,
                                              arm_2d_size_t *
                                              __restrict ptTargetMaskSize);

void __arm_2d_impl_rgb565_src_chn_msk_1h_des_msk_copy(uint16_t *
                                                      __restrict pSourceBase,
                                                      int16_t iSourceStride,
                                                      uint32_t *
                                                      __restrict
                                                      ptSourceMaskBase,
                                                      int16_t iSourceMaskStride,
                                                      arm_2d_size_t *
                                                      __restrict
                                                      ptSourceMaskSize,
                                                      uint16_t *
                                                      __restrict pTargetBase,
                                                      int16_t iTargetStride,
                                                      uint8_t *
                                                      __restrict
                                                      ptTargetMaskBase,
                                                      int16_t iTargetMaskStride,
                                                      arm_2d_size_t *
                                                      __restrict
                                                      ptTargetMaskSize,
                                                      arm_2d_size_t *
                                                      __restrict ptCopySize);

void __arm_2d_impl_rgb565_src_chn_msk_1h_des_msk_copy_mirror(uint16_t *
                                                             __restrict
                                                             pSourceBase,
                                                             int16_t
                                                             iSourceStride,
                                                             uint32_t *
                                                             __restrict
                                                             ptSourceMaskBase,
                                                             int16_t
                                                             iSourceMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptSourceMaskSize,
                                                             uint16_t *
                                                             __restrict
                                                             pTargetBase,
                                                             int16_t
                                                             iTargetStride,
                                                             uint8_t *
                                                             __restrict
                                                             ptTargetMaskBase,
                                                             int16_t
                                                             iTargetMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptTargetMaskSize,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptCopySize,
                                                             uint32_t wMode);

void __arm_2d_impl_rgb565_src_chn_msk_1h_des_msk_copy_x_mirror(uint16_t *
                                                           __restrict
                                                           pSourceBase,
                                                           int16_t
                                                           iSourceStride,
                                                           uint32_t *
                                                           __restrict
                                                           ptSourceMaskBase,
                                                           int16_t
                                                           iSourceMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptSourceMaskSize,
                                                           uint16_t *
                                                           __restrict
                                                           pTargetBase,
                                                           int16_t
                                                           iTargetStride,
                                                           uint8_t *
                                                           __restrict
                                                           ptTargetMaskBase,
                                                           int16_t
                                                           iTargetMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptTargetMaskSize,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptCopySize);

void __arm_2d_impl_rgb565_src_chn_msk_1h_des_msk_copy_xy_mirror(uint16_t *
                                                            __restrict
                                                            pSourceBase,
                                                            int16_t
                                                            iSourceStride,
                                                            uint32_t *
                                                            __restrict
                                                            ptSourceMaskBase,
                                                            int16_t
                                                            iSourceMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceMaskSize,
                                                            uint16_t *
                                                            __restrict
                                                            pTargetBase,
                                                            int16_t
                                                            iTargetStride,
                                                            uint8_t *
                                                            __restrict
                                                            ptTargetMaskBase,
                                                            int16_t
                                                            iTargetMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetMaskSize,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptCopySize);

void __arm_2d_impl_rgb565_src_chn_msk_1h_des_msk_copy_y_mirror(uint16_t *
                                                           __restrict
                                                           pSourceBase,
                                                           int16_t
                                                           iSourceStride,
                                                           uint32_t *
                                                           __restrict
                                                           ptSourceMaskBase,
                                                           int16_t
                                                           iSourceMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptSourceMaskSize,
                                                           uint16_t *
                                                           __restrict
                                                           pTargetBase,
                                                           int16_t
                                                           iTargetStride,
                                                           uint8_t *
                                                           __restrict
                                                           ptTargetMaskBase,
                                                           int16_t
                                                           iTargetMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptTargetMaskSize,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptCopySize);

void __arm_2d_impl_rgb565_src_chn_msk_1h_des_msk_fill(uint16_t *
                                                      __restrict ptSourceBase,
                                                      int16_t iSourceStride,
                                                      arm_2d_size_t *
                                                      __restrict ptSourceSize,
                                                      uint32_t *
                                                      __restrict
                                                      ptSourceMaskBase,
                                                      int16_t iSourceMaskStride,
                                                      arm_2d_size_t *
                                                      __restrict
                                                      ptSourceMaskSize,
                                                      uint16_t *
                                                      __restrict ptTargetBase,
                                                      int16_t iTargetStride,
                                                      arm_2d_size_t *
                                                      __restrict ptTargetSize,
                                                      uint8_t *
                                                      __restrict
                                                      ptTargetMaskBase,
                                                      int16_t iTargetMaskStride,
                                                      arm_2d_size_t *
                                                      __restrict
                                                      ptTargetMaskSize);

void __arm_2d_impl_rgb565_src_chn_msk_1h_des_msk_fill_mirror(uint16_t *
                                                             __restrict
                                                             ptSourceBase,
                                                             int16_t
                                                             iSourceStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptSourceSize,
                                                             uint32_t *
                                                             __restrict
                                                             ptSourceMaskBase,
                                                             int16_t
                                                             iSourceMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptSourceMaskSize,
                                                             uint16_t *
                                                             __restrict
                                                             ptTargetBase,
                                                             int16_t
                                                             iTargetStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptTargetSize,
                                                             uint8_t *
                                                             __restrict
                                                             ptTargetMaskBase,
                                                             int16_t
                                                             iTargetMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptTargetMaskSize,
                                                             uint32_t wMode);

void __arm_2d_impl_rgb565_src_chn_msk_1h_des_msk_fill_x_mirror(uint16_t *
                                                           __restrict
                                                           ptSourceBase,
                                                           int16_t
                                                           iSourceStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptSourceSize,
                                                           uint32_t *
                                                           __restrict
                                                           ptSourceMaskBase,
                                                           int16_t
                                                           iSourceMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptSourceMaskSize,
                                                           uint16_t *
                                                           __restrict
                                                           ptTargetBase,
                                                           int16_t
                                                           iTargetStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptTargetSize,
                                                           uint8_t *
                                                           __restrict
                                                           ptTargetMaskBase,
                                                           int16_t
                                                           iTargetMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptTargetMaskSize);

void __arm_2d_impl_rgb565_src_chn_msk_1h_des_msk_fill_xy_mirror(uint16_t *
                                                            __restrict
                                                            ptSourceBase,
                                                            int16_t
                                                            iSourceStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceSize,
                                                            uint32_t *
                                                            __restrict
                                                            ptSourceMaskBase,
                                                            int16_t
                                                            iSourceMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceMaskSize,
                                                            uint16_t *
                                                            __restrict
                                                            ptTargetBase,
                                                            int16_t
                                                            iTargetStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetSize,
                                                            uint8_t *
                                                            __restrict
                                                            ptTargetMaskBase,
                                                            int16_t
                                                            iTargetMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetMaskSize);

void __arm_2d_impl_rgb565_src_chn_msk_1h_des_msk_fill_y_mirror(uint16_t *
                                                               __restrict
                                                               ptSourceBase,
                                                               int16_t
                                                               iSourceStride,
                                                               arm_2d_size_t *
                                                               __restrict
                                                               ptSourceSize,
                                                               uint32_t *
                                                               __restrict
                                                               ptSourceMaskBase,
                                                               int16_t
                                                               iSourceMaskStride,
                                                               arm_2d_size_t *
                                                               __restrict
                                                               ptSourceMaskSize,
                                                               uint16_t *
                                                               __restrict
                                                               ptTargetBase,
                                                               int16_t
                                                               iTargetStride,
                                                               arm_2d_size_t *
                                                               __restrict
                                                               ptTargetSize,
                                                               uint8_t *
                                                               __restrict
                                                               ptTargetMaskBase,
                                                               int16_t
                                                               iTargetMaskStride,
                                                               arm_2d_size_t *
                                                               __restrict
                                                               ptTargetMaskSize);

void __arm_2d_impl_rgb565_src_chn_msk_copy(uint16_t * __restrict pSourceBase,
                                               int16_t iSourceStride,
                                               uint32_t *
                                               __restrict ptSourceMaskBase,
                                               int16_t iSourceMaskStride,
                                               arm_2d_size_t *
                                               __restrict ptSourceMaskSize,
                                               uint16_t * __restrict pTargetBase,
                                               int16_t iTargetStride,
                                               arm_2d_size_t *
                                               __restrict ptCopySize);

void __arm_2d_impl_rgb565_src_chn_msk_copy_mirror(uint16_t *
                                                  __restrict pSourceBase,
                                                  int16_t iSourceStride,
                                                  uint32_t *
                                                  __restrict ptSourceMaskBase,
                                                  int16_t iSourceMaskStride,
                                                  arm_2d_size_t *
                                                  __restrict ptSourceMaskSize,
                                                  uint16_t *
                                                  __restrict pTargetBase,
                                                  int16_t iTargetStride,
                                                  arm_2d_size_t *
                                                  __restrict ptCopySize,
                                                  uint32_t wMode);

void __arm_2d_impl_rgb565_src_chn_msk_copy_x_mirror(uint16_t *
                                                    __restrict pSourceBase,
                                                    int16_t iSourceStride,
                                                    uint32_t *
                                                    __restrict ptSourceMaskBase,
                                                    int16_t iSourceMaskStride,
                                                    arm_2d_size_t *
                                                    __restrict ptSourceMaskSize,
                                                    uint16_t *
                                                    __restrict pTargetBase,
                                                    int16_t iTargetStride,
                                                    arm_2d_size_t *
                                                    __restrict ptCopySize);

void __arm_2d_impl_rgb565_src_chn_msk_copy_xy_mirror(uint16_t *
                                                     __restrict pSourceBase,
                                                     int16_t iSourceStride,
                                                     uint32_t *
                                                     __restrict
                                                     ptSourceMaskBase,
                                                     int16_t iSourceMaskStride,
                                                     arm_2d_size_t *
                                                     __restrict
                                                     ptSourceMaskSize,
                                                     uint16_t *
                                                     __restrict pTargetBase,
                                                     int16_t iTargetStride,
                                                     arm_2d_size_t *
                                                     __restrict ptCopySize);

void __arm_2d_impl_rgb565_src_chn_msk_copy_y_mirror(uint16_t *
                                                    __restrict pSourceBase,
                                                    int16_t iSourceStride,
                                                    uint32_t *
                                                    __restrict ptSourceMaskBase,
                                                    int16_t iSourceMaskStride,
                                                    arm_2d_size_t *
                                                    __restrict ptSourceMaskSize,
                                                    uint16_t *
                                                    __restrict pTargetBase,
                                                    int16_t iTargetStride,
                                                    arm_2d_size_t *
                                                    __restrict ptCopySize);

void __arm_2d_impl_rgb565_src_chn_msk_des_chn_msk_copy(uint16_t *
                                                       __restrict pSourceBase,
                                                       int16_t iSourceStride,
                                                       uint32_t *
                                                       __restrict
                                                       ptSourceMaskBase,
                                                       int16_t
                                                       iSourceMaskStride,
                                                       arm_2d_size_t *
                                                       __restrict
                                                       ptSourceMaskSize,
                                                       uint16_t *
                                                       __restrict pTargetBase,
                                                       int16_t iTargetStride,
                                                       uint32_t *
                                                       __restrict
                                                       ptTargetMaskBase,
                                                       int16_t
                                                       iTargetMaskStride,
                                                       arm_2d_size_t *
                                                       __restrict
                                                       ptTargetMaskSize,
                                                       arm_2d_size_t *
                                                       __restrict ptCopySize);

void __arm_2d_impl_rgb565_src_chn_msk_des_chn_msk_copy_mirror(uint16_t *
                                                              __restrict
                                                              pSourceBase,
                                                              int16_t
                                                              iSourceStride,
                                                              uint32_t *
                                                              __restrict
                                                              ptSourceMaskBase,
                                                              int16_t
                                                              iSourceMaskStride,
                                                              arm_2d_size_t *
                                                              __restrict
                                                              ptSourceMaskSize,
                                                              uint16_t *
                                                              __restrict
                                                              pTargetBase,
                                                              int16_t
                                                              iTargetStride,
                                                              uint32_t *
                                                              __restrict
                                                              ptTargetMaskBase,
                                                              int16_t
                                                              iTargetMaskStride,
                                                              arm_2d_size_t *
                                                              __restrict
                                                              ptTargetMaskSize,
                                                              arm_2d_size_t *
                                                              __restrict
                                                              ptCopySize,
                                                              uint32_t wMode);

void __arm_2d_impl_rgb565_src_chn_msk_des_chn_msk_copy_x_mirror(uint16_t *
                                                            __restrict
                                                            pSourceBase,
                                                            int16_t
                                                            iSourceStride,
                                                            uint32_t *
                                                            __restrict
                                                            ptSourceMaskBase,
                                                            int16_t
                                                            iSourceMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceMaskSize,
                                                            uint16_t *
                                                            __restrict
                                                            pTargetBase,
                                                            int16_t
                                                            iTargetStride,
                                                            uint32_t *
                                                            __restrict
                                                            ptTargetMaskBase,
                                                            int16_t
                                                            iTargetMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetMaskSize,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptCopySize);

void __arm_2d_impl_rgb565_src_chn_msk_des_chn_msk_copy_xy_mirror(uint16_t *
                                                             __restrict
                                                             pSourceBase,
                                                             int16_t
                                                             iSourceStride,
                                                             uint32_t *
                                                             __restrict
                                                             ptSourceMaskBase,
                                                             int16_t
                                                             iSourceMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptSourceMaskSize,
                                                             uint16_t *
                                                             __restrict
                                                             pTargetBase,
                                                             int16_t
                                                             iTargetStride,
                                                             uint32_t *
                                                             __restrict
                                                             ptTargetMaskBase,
                                                             int16_t
                                                             iTargetMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptTargetMaskSize,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptCopySize);

void __arm_2d_impl_rgb565_src_chn_msk_des_chn_msk_copy_y_mirror(uint16_t *
                                                            __restrict
                                                            pSourceBase,
                                                            int16_t
                                                            iSourceStride,
                                                            uint32_t *
                                                            __restrict
                                                            ptSourceMaskBase,
                                                            int16_t
                                                            iSourceMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceMaskSize,
                                                            uint16_t *
                                                            __restrict
                                                            pTargetBase,
                                                            int16_t
                                                            iTargetStride,
                                                            uint32_t *
                                                            __restrict
                                                            ptTargetMaskBase,
                                                            int16_t
                                                            iTargetMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetMaskSize,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptCopySize);

void __arm_2d_impl_rgb565_src_chn_msk_des_chn_msk_fill(uint16_t *
                                                       __restrict ptSourceBase,
                                                       int16_t iSourceStride,
                                                       arm_2d_size_t *
                                                       __restrict ptSourceSize,
                                                       uint32_t *
                                                       __restrict
                                                       ptSourceMaskBase,
                                                       int16_t
                                                       iSourceMaskStride,
                                                       arm_2d_size_t *
                                                       __restrict
                                                       ptSourceMaskSize,
                                                       uint16_t *
                                                       __restrict ptTargetBase,
                                                       int16_t iTargetStride,
                                                       arm_2d_size_t *
                                                       __restrict ptTargetSize,
                                                       uint32_t *
                                                       __restrict
                                                       ptTargetMaskBase,
                                                       int16_t
                                                       iTargetMaskStride,
                                                       arm_2d_size_t *
                                                       __restrict
                                                       ptTargetMaskSize);

void __arm_2d_impl_rgb565_src_chn_msk_des_chn_msk_fill_mirror(uint16_t *
                                                              __restrict
                                                              ptSourceBase,
                                                              int16_t
                                                              iSourceStride,
                                                              arm_2d_size_t *
                                                              __restrict
                                                              ptSourceSize,
                                                              uint32_t *
                                                              __restrict
                                                              ptSourceMaskBase,
                                                              int16_t
                                                              iSourceMaskStride,
                                                              arm_2d_size_t *
                                                              __restrict
                                                              ptSourceMaskSize,
                                                              uint16_t *
                                                              __restrict
                                                              ptTargetBase,
                                                              int16_t
                                                              iTargetStride,
                                                              arm_2d_size_t *
                                                              __restrict
                                                              ptTargetSize,
                                                              uint32_t *
                                                              __restrict
                                                              ptTargetMaskBase,
                                                              int16_t
                                                              iTargetMaskStride,
                                                              arm_2d_size_t *
                                                              __restrict
                                                              ptTargetMaskSize,
                                                              uint32_t wMode);

void __arm_2d_impl_rgb565_src_chn_msk_des_chn_msk_fill_x_mirror(uint16_t *
                                                            __restrict
                                                            ptSourceBase,
                                                            int16_t
                                                            iSourceStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceSize,
                                                            uint32_t *
                                                            __restrict
                                                            ptSourceMaskBase,
                                                            int16_t
                                                            iSourceMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceMaskSize,
                                                            uint16_t *
                                                            __restrict
                                                            ptTargetBase,
                                                            int16_t
                                                            iTargetStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetSize,
                                                            uint32_t *
                                                            __restrict
                                                            ptTargetMaskBase,
                                                            int16_t
                                                            iTargetMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetMaskSize);

void __arm_2d_impl_rgb565_src_chn_msk_des_chn_msk_fill_xy_mirror(
                                                             uint16_t *
                                                             __restrict
                                                             ptSourceBase,
                                                             int16_t
                                                             iSourceStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptSourceSize,
                                                             uint32_t *
                                                             __restrict
                                                             ptSourceMaskBase,
                                                             int16_t
                                                             iSourceMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptSourceMaskSize,
                                                             uint16_t *
                                                             __restrict
                                                             ptTargetBase,
                                                             int16_t
                                                             iTargetStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptTargetSize,
                                                             uint32_t *
                                                             __restrict
                                                             ptTargetMaskBase,
                                                             int16_t
                                                             iTargetMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptTargetMaskSize);

void __arm_2d_impl_rgb565_src_chn_msk_des_chn_msk_fill_y_mirror(uint16_t *
                                                                __restrict
                                                                ptSourceBase,
                                                                int16_t
                                                                iSourceStride,
                                                                arm_2d_size_t *
                                                                __restrict
                                                                ptSourceSize,
                                                                uint32_t *
                                                                __restrict
                                                                ptSourceMaskBase,
                                                                int16_t
                                                                iSourceMaskStride,
                                                                arm_2d_size_t *
                                                                __restrict
                                                                ptSourceMaskSize,
                                                                uint16_t *
                                                                __restrict
                                                                ptTargetBase,
                                                                int16_t
                                                                iTargetStride,
                                                                arm_2d_size_t *
                                                                __restrict
                                                                ptTargetSize,
                                                                uint32_t *
                                                                __restrict
                                                                ptTargetMaskBase,
                                                                int16_t
                                                                iTargetMaskStride,
                                                                arm_2d_size_t *
                                                                __restrict
                                                                ptTargetMaskSize);

void __arm_2d_impl_rgb565_src_chn_msk_des_msk_copy(uint16_t *
                                                   __restrict pSourceBase,
                                                   int16_t iSourceStride,
                                                   uint32_t *
                                                   __restrict ptSourceMaskBase,
                                                   int16_t iSourceMaskStride,
                                                   arm_2d_size_t *
                                                   __restrict ptSourceMaskSize,
                                                   uint16_t *
                                                   __restrict pTargetBase,
                                                   int16_t iTargetStride,
                                                   uint8_t *
                                                   __restrict ptTargetMaskBase,
                                                   int16_t iTargetMaskStride,
                                                   arm_2d_size_t *
                                                   __restrict ptTargetMaskSize,
                                                   arm_2d_size_t *
                                                   __restrict ptCopySize);

void __arm_2d_impl_rgb565_src_chn_msk_des_msk_copy_mirror(uint16_t *
                                                          __restrict
                                                          pSourceBase,
                                                          int16_t iSourceStride,
                                                          uint32_t *
                                                          __restrict
                                                          ptSourceMaskBase,
                                                          int16_t
                                                          iSourceMaskStride,
                                                          arm_2d_size_t *
                                                          __restrict
                                                          ptSourceMaskSize,
                                                          uint16_t *
                                                          __restrict
                                                          pTargetBase,
                                                          int16_t iTargetStride,
                                                          uint8_t *
                                                          __restrict
                                                          ptTargetMaskBase,
                                                          int16_t
                                                          iTargetMaskStride,
                                                          arm_2d_size_t *
                                                          __restrict
                                                          ptTargetMaskSize,
                                                          arm_2d_size_t *
                                                          __restrict ptCopySize,
                                                          uint32_t wMode);

void __arm_2d_impl_rgb565_src_chn_msk_des_msk_copy_x_mirror(uint16_t *
                                                            __restrict
                                                            pSourceBase,
                                                            int16_t
                                                            iSourceStride,
                                                            uint32_t *
                                                            __restrict
                                                            ptSourceMaskBase,
                                                            int16_t
                                                            iSourceMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceMaskSize,
                                                            uint16_t *
                                                            __restrict
                                                            pTargetBase,
                                                            int16_t
                                                            iTargetStride,
                                                            uint8_t *
                                                            __restrict
                                                            ptTargetMaskBase,
                                                            int16_t
                                                            iTargetMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetMaskSize,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptCopySize);

void __arm_2d_impl_rgb565_src_chn_msk_des_msk_copy_xy_mirror(uint16_t *
                                                             __restrict
                                                             pSourceBase,
                                                             int16_t
                                                             iSourceStride,
                                                             uint32_t *
                                                             __restrict
                                                             ptSourceMaskBase,
                                                             int16_t
                                                             iSourceMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptSourceMaskSize,
                                                             uint16_t *
                                                             __restrict
                                                             pTargetBase,
                                                             int16_t
                                                             iTargetStride,
                                                             uint8_t *
                                                             __restrict
                                                             ptTargetMaskBase,
                                                             int16_t
                                                             iTargetMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptTargetMaskSize,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptCopySize);

void __arm_2d_impl_rgb565_src_chn_msk_des_msk_copy_y_mirror(uint16_t *
                                                            __restrict
                                                            pSourceBase,
                                                            int16_t
                                                            iSourceStride,
                                                            uint32_t *
                                                            __restrict
                                                            ptSourceMaskBase,
                                                            int16_t
                                                            iSourceMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceMaskSize,
                                                            uint16_t *
                                                            __restrict
                                                            pTargetBase,
                                                            int16_t
                                                            iTargetStride,
                                                            uint8_t *
                                                            __restrict
                                                            ptTargetMaskBase,
                                                            int16_t
                                                            iTargetMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetMaskSize,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptCopySize);

void __arm_2d_impl_rgb565_src_chn_msk_des_msk_fill(uint16_t *
                                                   __restrict ptSourceBase,
                                                   int16_t iSourceStride,
                                                   arm_2d_size_t *
                                                   __restrict ptSourceSize,
                                                   uint32_t *
                                                   __restrict ptSourceMaskBase,
                                                   int16_t iSourceMaskStride,
                                                   arm_2d_size_t *
                                                   __restrict ptSourceMaskSize,
                                                   uint16_t *
                                                   __restrict ptTargetBase,
                                                   int16_t iTargetStride,
                                                   arm_2d_size_t *
                                                   __restrict ptTargetSize,
                                                   uint8_t *
                                                   __restrict ptTargetMaskBase,
                                                   int16_t iTargetMaskStride,
                                                   arm_2d_size_t *
                                                   __restrict ptTargetMaskSize);

void __arm_2d_impl_rgb565_src_chn_msk_des_msk_fill_mirror(uint16_t *
                                                          __restrict
                                                          ptSourceBase,
                                                          int16_t iSourceStride,
                                                          arm_2d_size_t *
                                                          __restrict
                                                          ptSourceSize,
                                                          uint32_t *
                                                          __restrict
                                                          ptSourceMaskBase,
                                                          int16_t
                                                          iSourceMaskStride,
                                                          arm_2d_size_t *
                                                          __restrict
                                                          ptSourceMaskSize,
                                                          uint16_t *
                                                          __restrict
                                                          ptTargetBase,
                                                          int16_t iTargetStride,
                                                          arm_2d_size_t *
                                                          __restrict
                                                          ptTargetSize,
                                                          uint8_t *
                                                          __restrict
                                                          ptTargetMaskBase,
                                                          int16_t
                                                          iTargetMaskStride,
                                                          arm_2d_size_t *
                                                          __restrict
                                                          ptTargetMaskSize,
                                                          uint32_t wMode);

void __arm_2d_impl_rgb565_src_chn_msk_des_msk_fill_x_mirror(uint16_t *
                                                            __restrict
                                                            ptSourceBase,
                                                            int16_t
                                                            iSourceStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceSize,
                                                            uint32_t *
                                                            __restrict
                                                            ptSourceMaskBase,
                                                            int16_t
                                                            iSourceMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceMaskSize,
                                                            uint16_t *
                                                            __restrict
                                                            ptTargetBase,
                                                            int16_t
                                                            iTargetStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetSize,
                                                            uint8_t *
                                                            __restrict
                                                            ptTargetMaskBase,
                                                            int16_t
                                                            iTargetMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetMaskSize);

void __arm_2d_impl_rgb565_src_chn_msk_des_msk_fill_xy_mirror(uint16_t *
                                                             __restrict
                                                             ptSourceBase,
                                                             int16_t
                                                             iSourceStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptSourceSize,
                                                             uint32_t *
                                                             __restrict
                                                             ptSourceMaskBase,
                                                             int16_t
                                                             iSourceMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptSourceMaskSize,
                                                             uint16_t *
                                                             __restrict
                                                             ptTargetBase,
                                                             int16_t
                                                             iTargetStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptTargetSize,
                                                             uint8_t *
                                                             __restrict
                                                             ptTargetMaskBase,
                                                             int16_t
                                                             iTargetMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptTargetMaskSize);

void __arm_2d_impl_rgb565_src_chn_msk_des_msk_fill_y_mirror(uint16_t *
                                                            __restrict
                                                            ptSourceBase,
                                                            int16_t
                                                            iSourceStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceSize,
                                                            uint32_t *
                                                            __restrict
                                                            ptSourceMaskBase,
                                                            int16_t
                                                            iSourceMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceMaskSize,
                                                            uint16_t *
                                                            __restrict
                                                            ptTargetBase,
                                                            int16_t
                                                            iTargetStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetSize,
                                                            uint8_t *
                                                            __restrict
                                                            ptTargetMaskBase,
                                                            int16_t
                                                            iTargetMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetMaskSize);

void __arm_2d_impl_rgb565_src_chn_msk_fill(uint16_t * __restrict ptSourceBase,
                                           int16_t iSourceStride,
                                           arm_2d_size_t *
                                           __restrict ptSourceSize,
                                           uint32_t *
                                           __restrict ptSourceMaskBase,
                                           int16_t iSourceMaskStride,
                                           arm_2d_size_t *
                                           __restrict ptSourceMaskSize,
                                           uint16_t * __restrict ptTargetBase,
                                           int16_t iTargetStride,
                                           arm_2d_size_t *
                                           __restrict ptTargetSize);

void __arm_2d_impl_rgb565_src_chn_msk_fill_mirror(uint16_t *
                                                  __restrict ptSourceBase,
                                                  int16_t iSourceStride,
                                                  arm_2d_size_t *
                                                  __restrict ptSourceSize,
                                                  uint32_t *
                                                  __restrict ptSourceMaskBase,
                                                  int16_t iSourceMaskStride,
                                                  arm_2d_size_t *
                                                  __restrict ptSourceMaskSize,
                                                  uint16_t *
                                                  __restrict ptTargetBase,
                                                  int16_t iTargetStride,
                                                  arm_2d_size_t *
                                                  __restrict ptTargetSize,
                                                  uint32_t wMode);

void __arm_2d_impl_rgb565_src_chn_msk_fill_x_mirror(uint16_t *
                                                    __restrict ptSourceBase,
                                                    int16_t iSourceStride,
                                                    arm_2d_size_t *
                                                    __restrict ptSourceSize,
                                                    uint32_t *
                                                    __restrict ptSourceMaskBase,
                                                    int16_t iSourceMaskStride,
                                                    arm_2d_size_t *
                                                    __restrict ptSourceMaskSize,
                                                    uint16_t *
                                                    __restrict ptTargetBase,
                                                    int16_t iTargetStride,
                                                    arm_2d_size_t *
                                                    __restrict ptTargetSize);

void __arm_2d_impl_rgb565_src_chn_msk_fill_xy_mirror(uint16_t *
                                                     __restrict ptSourceBase,
                                                     int16_t iSourceStride,
                                                     arm_2d_size_t *
                                                     __restrict ptSourceSize,
                                                     uint32_t *
                                                     __restrict
                                                     ptSourceMaskBase,
                                                     int16_t iSourceMaskStride,
                                                     arm_2d_size_t *
                                                     __restrict
                                                     ptSourceMaskSize,
                                                     uint16_t *
                                                     __restrict ptTargetBase,
                                                     int16_t iTargetStride,
                                                     arm_2d_size_t *
                                                     __restrict ptTargetSize);

void __arm_2d_impl_rgb565_src_chn_msk_fill_y_mirror(uint16_t *
                                                    __restrict ptSourceBase,
                                                    int16_t iSourceStride,
                                                    arm_2d_size_t *
                                                    __restrict ptSourceSize,
                                                    uint32_t *
                                                    __restrict ptSourceMaskBase,
                                                    int16_t iSourceMaskStride,
                                                    arm_2d_size_t *
                                                    __restrict ptSourceMaskSize,
                                                    uint16_t *
                                                    __restrict ptTargetBase,
                                                    int16_t iTargetStride,
                                                    arm_2d_size_t *
                                                    __restrict ptTargetSize);

void __arm_2d_impl_rgb565_src_msk_1h_des_msk_copy(uint16_t *
                                                  __restrict pSourceBase,
                                                  int16_t iSourceStride,
                                                  uint8_t *
                                                  __restrict ptSourceMaskBase,
                                                  int16_t iSourceMaskStride,
                                                  arm_2d_size_t *
                                                  __restrict ptSourceMaskSize,
                                                  uint16_t *
                                                  __restrict pTargetBase,
                                                  int16_t iTargetStride,
                                                  uint8_t *
                                                  __restrict ptTargetMaskBase,
                                                  int16_t iTargetMaskStride,
                                                  arm_2d_size_t *
                                                  __restrict ptTargetMaskSize,
                                                  arm_2d_size_t *
                                                  __restrict ptCopySize);

void __arm_2d_impl_rgb565_src_msk_1h_des_msk_copy_mirror(uint16_t *
                                                         __restrict pSourceBase,
                                                         int16_t iSourceStride,
                                                         uint8_t *
                                                         __restrict
                                                         ptSourceMaskBase,
                                                         int16_t
                                                         iSourceMaskStride,
                                                         arm_2d_size_t *
                                                         __restrict
                                                         ptSourceMaskSize,
                                                         uint16_t *
                                                         __restrict pTargetBase,
                                                         int16_t iTargetStride,
                                                         uint8_t *
                                                         __restrict
                                                         ptTargetMaskBase,
                                                         int16_t
                                                         iTargetMaskStride,
                                                         arm_2d_size_t *
                                                         __restrict
                                                         ptTargetMaskSize,
                                                         arm_2d_size_t *
                                                         __restrict ptCopySize,
                                                         uint32_t wMode);

void __arm_2d_impl_rgb565_src_msk_1h_des_msk_copy_x_mirror(uint16_t *
                                                           __restrict
                                                           pSourceBase,
                                                           int16_t
                                                           iSourceStride,
                                                           uint8_t *
                                                           __restrict
                                                           ptSourceMaskBase,
                                                           int16_t
                                                           iSourceMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptSourceMaskSize,
                                                           uint16_t *
                                                           __restrict
                                                           pTargetBase,
                                                           int16_t
                                                           iTargetStride,
                                                           uint8_t *
                                                           __restrict
                                                           ptTargetMaskBase,
                                                           int16_t
                                                           iTargetMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptTargetMaskSize,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptCopySize);

void __arm_2d_impl_rgb565_src_msk_1h_des_msk_copy_xy_mirror(uint16_t *
                                                            __restrict
                                                            pSourceBase,
                                                            int16_t
                                                            iSourceStride,
                                                            uint8_t *
                                                            __restrict
                                                            ptSourceMaskBase,
                                                            int16_t
                                                            iSourceMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceMaskSize,
                                                            uint16_t *
                                                            __restrict
                                                            pTargetBase,
                                                            int16_t
                                                            iTargetStride,
                                                            uint8_t *
                                                            __restrict
                                                            ptTargetMaskBase,
                                                            int16_t
                                                            iTargetMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetMaskSize,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptCopySize);

void __arm_2d_impl_rgb565_src_msk_1h_des_msk_copy_y_mirror(uint16_t *
                                                           __restrict
                                                           pSourceBase,
                                                           int16_t
                                                           iSourceStride,
                                                           uint8_t *
                                                           __restrict
                                                           ptSourceMaskBase,
                                                           int16_t
                                                           iSourceMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptSourceMaskSize,
                                                           uint16_t *
                                                           __restrict
                                                           pTargetBase,
                                                           int16_t
                                                           iTargetStride,
                                                           uint8_t *
                                                           __restrict
                                                           ptTargetMaskBase,
                                                           int16_t
                                                           iTargetMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptTargetMaskSize,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptCopySize);

void __arm_2d_impl_rgb565_src_msk_1h_des_msk_fill(uint16_t *
                                                  __restrict ptSourceBase,
                                                  int16_t iSourceStride,
                                                  arm_2d_size_t *
                                                  __restrict ptSourceSize,
                                                  uint8_t *
                                                  __restrict ptSourceMaskBase,
                                                  int16_t iSourceMaskStride,
                                                  arm_2d_size_t *
                                                  __restrict ptSourceMaskSize,
                                                  uint16_t *
                                                  __restrict ptTargetBase,
                                                  int16_t iTargetStride,
                                                  arm_2d_size_t *
                                                  __restrict ptTargetSize,
                                                  uint8_t *
                                                  __restrict ptTargetMaskBase,
                                                  int16_t iTargetMaskStride,
                                                  arm_2d_size_t *
                                                  __restrict ptTargetMaskSize);

void __arm_2d_impl_rgb565_src_msk_1h_des_msk_fill_mirror(uint16_t *
                                                         __restrict
                                                         ptSourceBase,
                                                         int16_t iSourceStride,
                                                         arm_2d_size_t *
                                                         __restrict
                                                         ptSourceSize,
                                                         uint8_t *
                                                         __restrict
                                                         ptSourceMaskBase,
                                                         int16_t
                                                         iSourceMaskStride,
                                                         arm_2d_size_t *
                                                         __restrict
                                                         ptSourceMaskSize,
                                                         uint16_t *
                                                         __restrict
                                                         ptTargetBase,
                                                         int16_t iTargetStride,
                                                         arm_2d_size_t *
                                                         __restrict
                                                         ptTargetSize,
                                                         uint8_t *
                                                         __restrict
                                                         ptTargetMaskBase,
                                                         int16_t
                                                         iTargetMaskStride,
                                                         arm_2d_size_t *
                                                         __restrict
                                                         ptTargetMaskSize,
                                                         uint32_t wMode);

void __arm_2d_impl_rgb565_src_msk_1h_des_msk_fill_x_mirror(uint16_t *
                                                           __restrict
                                                           ptSourceBase,
                                                           int16_t
                                                           iSourceStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptSourceSize,
                                                           uint8_t *
                                                           __restrict
                                                           ptSourceMaskBase,
                                                           int16_t
                                                           iSourceMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptSourceMaskSize,
                                                           uint16_t *
                                                           __restrict
                                                           ptTargetBase,
                                                           int16_t
                                                           iTargetStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptTargetSize,
                                                           uint8_t *
                                                           __restrict
                                                           ptTargetMaskBase,
                                                           int16_t
                                                           iTargetMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptTargetMaskSize);

void __arm_2d_impl_rgb565_src_msk_1h_des_msk_fill_xy_mirror(uint16_t *
                                                            __restrict
                                                            ptSourceBase,
                                                            int16_t
                                                            iSourceStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceSize,
                                                            uint8_t *
                                                            __restrict
                                                            ptSourceMaskBase,
                                                            int16_t
                                                            iSourceMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceMaskSize,
                                                            uint16_t *
                                                            __restrict
                                                            ptTargetBase,
                                                            int16_t
                                                            iTargetStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetSize,
                                                            uint8_t *
                                                            __restrict
                                                            ptTargetMaskBase,
                                                            int16_t
                                                            iTargetMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetMaskSize);

void __arm_2d_impl_rgb565_src_msk_1h_des_msk_fill_y_mirror(uint16_t *
                                                           __restrict
                                                           ptSourceBase,
                                                           int16_t
                                                           iSourceStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptSourceSize,
                                                           uint8_t *
                                                           __restrict
                                                           ptSourceMaskBase,
                                                           int16_t
                                                           iSourceMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptSourceMaskSize,
                                                           uint16_t *
                                                           __restrict
                                                           ptTargetBase,
                                                           int16_t
                                                           iTargetStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptTargetSize,
                                                           uint8_t *
                                                           __restrict
                                                           ptTargetMaskBase,
                                                           int16_t
                                                           iTargetMaskStride,
                                                           arm_2d_size_t *
                                                           __restrict
                                                           ptTargetMaskSize);

void __arm_2d_impl_rgb565_src_msk_copy(uint16_t * __restrict pSourceBase,
                                       int16_t iSourceStride,
                                       uint8_t * __restrict ptSourceMaskBase,
                                       int16_t iSourceMaskStride,
                                       arm_2d_size_t *
                                       __restrict ptSourceMaskSize,
                                       uint16_t * __restrict pTargetBase,
                                       int16_t iTargetStride,
                                       arm_2d_size_t * __restrict ptCopySize);

void __arm_2d_impl_rgb565_src_msk_copy_mirror(uint16_t * __restrict pSourceBase,
                                              int16_t iSourceStride,
                                              uint8_t *
                                              __restrict ptSourceMaskBase,
                                              int16_t iSourceMaskStride,
                                              arm_2d_size_t *
                                              __restrict ptSourceMaskSize,
                                              uint16_t * __restrict pTargetBase,
                                              int16_t iTargetStride,
                                              arm_2d_size_t *
                                              __restrict ptCopySize,
                                              uint32_t wMode);

void __arm_2d_impl_rgb565_src_msk_copy_x_mirror(uint16_t *
                                                __restrict pSourceBase,
                                                int16_t iSourceStride,
                                                uint8_t *
                                                __restrict ptSourceMaskBase,
                                                int16_t iSourceMaskStride,
                                                arm_2d_size_t *
                                                __restrict ptSourceMaskSize,
                                                uint16_t *
                                                __restrict pTargetBase,
                                                int16_t iTargetStride,
                                                arm_2d_size_t *
                                                __restrict ptCopySize);

void __arm_2d_impl_rgb565_src_msk_copy_xy_mirror(uint16_t *
                                                 __restrict pSourceBase,
                                                 int16_t iSourceStride,
                                                 uint8_t *
                                                 __restrict ptSourceMaskBase,
                                                 int16_t iSourceMaskStride,
                                                 arm_2d_size_t *
                                                 __restrict ptSourceMaskSize,
                                                 uint16_t *
                                                 __restrict pTargetBase,
                                                 int16_t iTargetStride,
                                                 arm_2d_size_t *
                                                 __restrict ptCopySize);

void __arm_2d_impl_rgb565_src_msk_copy_y_mirror(uint16_t *
                                                __restrict pSourceBase,
                                                int16_t iSourceStride,
                                                uint8_t *
                                                __restrict ptSourceMaskBase,
                                                int16_t iSourceMaskStride,
                                                arm_2d_size_t *
                                                __restrict ptSourceMaskSize,
                                                uint16_t *
                                                __restrict pTargetBase,
                                                int16_t iTargetStride,
                                                arm_2d_size_t *
                                                __restrict ptCopySize);

void __arm_2d_impl_rgb565_src_msk_des_chn_msk_copy(uint16_t *
                                                   __restrict pSourceBase,
                                                   int16_t iSourceStride,
                                                   uint8_t *
                                                   __restrict ptSourceMaskBase,
                                                   int16_t iSourceMaskStride,
                                                   arm_2d_size_t *
                                                   __restrict ptSourceMaskSize,
                                                   uint16_t *
                                                   __restrict pTargetBase,
                                                   int16_t iTargetStride,
                                                   uint32_t *
                                                   __restrict ptTargetMaskBase,
                                                   int16_t iTargetMaskStride,
                                                   arm_2d_size_t *
                                                   __restrict ptTargetMaskSize,
                                                   arm_2d_size_t *
                                                   __restrict ptCopySize);

void __arm_2d_impl_rgb565_src_msk_des_chn_msk_copy_mirror(uint16_t *
                                                          __restrict
                                                          pSourceBase,
                                                          int16_t iSourceStride,
                                                          uint8_t *
                                                          __restrict
                                                          ptSourceMaskBase,
                                                          int16_t
                                                          iSourceMaskStride,
                                                          arm_2d_size_t *
                                                          __restrict
                                                          ptSourceMaskSize,
                                                          uint16_t *
                                                          __restrict
                                                          pTargetBase,
                                                          int16_t iTargetStride,
                                                          uint32_t *
                                                          __restrict
                                                          ptTargetMaskBase,
                                                          int16_t
                                                          iTargetMaskStride,
                                                          arm_2d_size_t *
                                                          __restrict
                                                          ptTargetMaskSize,
                                                          arm_2d_size_t *
                                                          __restrict ptCopySize,
                                                          uint32_t wMode);

void __arm_2d_impl_rgb565_src_msk_des_chn_msk_copy_x_mirror(uint16_t *
                                                            __restrict
                                                            pSourceBase,
                                                            int16_t
                                                            iSourceStride,
                                                            uint8_t *
                                                            __restrict
                                                            ptSourceMaskBase,
                                                            int16_t
                                                            iSourceMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceMaskSize,
                                                            uint16_t *
                                                            __restrict
                                                            pTargetBase,
                                                            int16_t
                                                            iTargetStride,
                                                            uint32_t *
                                                            __restrict
                                                            ptTargetMaskBase,
                                                            int16_t
                                                            iTargetMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetMaskSize,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptCopySize);

void __arm_2d_impl_rgb565_src_msk_des_chn_msk_copy_xy_mirror(uint16_t *
                                                             __restrict
                                                             pSourceBase,
                                                             int16_t
                                                             iSourceStride,
                                                             uint8_t *
                                                             __restrict
                                                             ptSourceMaskBase,
                                                             int16_t
                                                             iSourceMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptSourceMaskSize,
                                                             uint16_t *
                                                             __restrict
                                                             pTargetBase,
                                                             int16_t
                                                             iTargetStride,
                                                             uint32_t *
                                                             __restrict
                                                             ptTargetMaskBase,
                                                             int16_t
                                                             iTargetMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptTargetMaskSize,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptCopySize);

void __arm_2d_impl_rgb565_src_msk_des_chn_msk_copy_y_mirror(uint16_t *
                                                            __restrict
                                                            pSourceBase,
                                                            int16_t
                                                            iSourceStride,
                                                            uint8_t *
                                                            __restrict
                                                            ptSourceMaskBase,
                                                            int16_t
                                                            iSourceMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceMaskSize,
                                                            uint16_t *
                                                            __restrict
                                                            pTargetBase,
                                                            int16_t
                                                            iTargetStride,
                                                            uint32_t *
                                                            __restrict
                                                            ptTargetMaskBase,
                                                            int16_t
                                                            iTargetMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetMaskSize,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptCopySize);

void __arm_2d_impl_rgb565_src_msk_des_chn_msk_fill(uint16_t *
                                                   __restrict ptSourceBase,
                                                   int16_t iSourceStride,
                                                   arm_2d_size_t *
                                                   __restrict ptSourceSize,
                                                   uint8_t *
                                                   __restrict ptSourceMaskBase,
                                                   int16_t iSourceMaskStride,
                                                   arm_2d_size_t *
                                                   __restrict ptSourceMaskSize,
                                                   uint16_t *
                                                   __restrict ptTargetBase,
                                                   int16_t iTargetStride,
                                                   arm_2d_size_t *
                                                   __restrict ptTargetSize,
                                                   uint32_t *
                                                   __restrict ptTargetMaskBase,
                                                   int16_t iTargetMaskStride,
                                                   arm_2d_size_t *
                                                   __restrict ptTargetMaskSize);

void __arm_2d_impl_rgb565_src_msk_des_chn_msk_fill_mirror(uint16_t *
                                                          __restrict
                                                          ptSourceBase,
                                                          int16_t iSourceStride,
                                                          arm_2d_size_t *
                                                          __restrict
                                                          ptSourceSize,
                                                          uint8_t *
                                                          __restrict
                                                          ptSourceMaskBase,
                                                          int16_t
                                                          iSourceMaskStride,
                                                          arm_2d_size_t *
                                                          __restrict
                                                          ptSourceMaskSize,
                                                          uint16_t *
                                                          __restrict
                                                          ptTargetBase,
                                                          int16_t iTargetStride,
                                                          arm_2d_size_t *
                                                          __restrict
                                                          ptTargetSize,
                                                          uint32_t *
                                                          __restrict
                                                          ptTargetMaskBase,
                                                          int16_t
                                                          iTargetMaskStride,
                                                          arm_2d_size_t *
                                                          __restrict
                                                          ptTargetMaskSize,
                                                          uint32_t wMode);

void __arm_2d_impl_rgb565_src_msk_des_chn_msk_fill_x_mirror(uint16_t *
                                                            __restrict
                                                            ptSourceBase,
                                                            int16_t
                                                            iSourceStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceSize,
                                                            uint8_t *
                                                            __restrict
                                                            ptSourceMaskBase,
                                                            int16_t
                                                            iSourceMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceMaskSize,
                                                            uint16_t *
                                                            __restrict
                                                            ptTargetBase,
                                                            int16_t
                                                            iTargetStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetSize,
                                                            uint32_t *
                                                            __restrict
                                                            ptTargetMaskBase,
                                                            int16_t
                                                            iTargetMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetMaskSize);

void __arm_2d_impl_rgb565_src_msk_des_chn_msk_fill_xy_mirror(uint16_t *
                                                             __restrict
                                                             ptSourceBase,
                                                             int16_t
                                                             iSourceStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptSourceSize,
                                                             uint8_t *
                                                             __restrict
                                                             ptSourceMaskBase,
                                                             int16_t
                                                             iSourceMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptSourceMaskSize,
                                                             uint16_t *
                                                             __restrict
                                                             ptTargetBase,
                                                             int16_t
                                                             iTargetStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptTargetSize,
                                                             uint32_t *
                                                             __restrict
                                                             ptTargetMaskBase,
                                                             int16_t
                                                             iTargetMaskStride,
                                                             arm_2d_size_t *
                                                             __restrict
                                                             ptTargetMaskSize);

void __arm_2d_impl_rgb565_src_msk_des_chn_msk_fill_y_mirror(uint16_t *
                                                            __restrict
                                                            ptSourceBase,
                                                            int16_t
                                                            iSourceStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceSize,
                                                            uint8_t *
                                                            __restrict
                                                            ptSourceMaskBase,
                                                            int16_t
                                                            iSourceMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptSourceMaskSize,
                                                            uint16_t *
                                                            __restrict
                                                            ptTargetBase,
                                                            int16_t
                                                            iTargetStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetSize,
                                                            uint32_t *
                                                            __restrict
                                                            ptTargetMaskBase,
                                                            int16_t
                                                            iTargetMaskStride,
                                                            arm_2d_size_t *
                                                            __restrict
                                                            ptTargetMaskSize);

void __arm_2d_impl_rgb565_src_msk_fill(uint16_t * __restrict ptSourceBase,
                                       int16_t iSourceStride,
                                       arm_2d_size_t * __restrict ptSourceSize,
                                       uint8_t * __restrict ptSourceMaskBase,
                                       int16_t iSourceMaskStride,
                                       arm_2d_size_t *
                                       __restrict ptSourceMaskSize,
                                       uint16_t * __restrict ptTargetBase,
                                       int16_t iTargetStride,
                                       arm_2d_size_t * __restrict ptTargetSize);

void __arm_2d_impl_rgb565_src_msk_fill_mirror(uint16_t *
                                              __restrict ptSourceBase,
                                              int16_t iSourceStride,
                                              arm_2d_size_t *
                                              __restrict ptSourceSize,
                                              uint8_t *
                                              __restrict ptSourceMaskBase,
                                              int16_t iSourceMaskStride,
                                              arm_2d_size_t *
                                              __restrict ptSourceMaskSize,
                                              uint16_t *
                                              __restrict ptTargetBase,
                                              int16_t iTargetStride,
                                              arm_2d_size_t *
                                              __restrict ptTargetSize,
                                              uint32_t wMode);

void __arm_2d_impl_rgb565_src_msk_fill_x_mirror(uint16_t *
                                                __restrict ptSourceBase,
                                                int16_t iSourceStride,
                                                arm_2d_size_t *
                                                __restrict ptSourceSize,
                                                uint8_t *
                                                __restrict ptSourceMaskBase,
                                                int16_t iSourceMaskStride,
                                                arm_2d_size_t *
                                                __restrict ptSourceMaskSize,
                                                uint16_t *
                                                __restrict ptTargetBase,
                                                int16_t iTargetStride,
                                                arm_2d_size_t *
                                                __restrict ptTargetSize);

void __arm_2d_impl_rgb565_src_msk_fill_xy_mirror(uint16_t *
                                                 __restrict ptSourceBase,
                                                 int16_t iSourceStride,
                                                 arm_2d_size_t *
                                                 __restrict ptSourceSize,
                                                 uint8_t *
                                                 __restrict ptSourceMaskBase,
                                                 int16_t iSourceMaskStride,
                                                 arm_2d_size_t *
                                                 __restrict ptSourceMaskSize,
                                                 uint16_t *
                                                 __restrict ptTargetBase,
                                                 int16_t iTargetStride,
                                                 arm_2d_size_t *
                                                 __restrict ptTargetSize);

void __arm_2d_impl_rgb565_src_msk_fill_y_mirror(uint16_t *
                                                __restrict ptSourceBase,
                                                int16_t iSourceStride,
                                                arm_2d_size_t *
                                                __restrict ptSourceSize,
                                                uint8_t *
                                                __restrict ptSourceMaskBase,
                                                int16_t iSourceMaskStride,
                                                arm_2d_size_t *
                                                __restrict ptSourceMaskSize,
                                                uint16_t *
                                                __restrict ptTargetBase,
                                                int16_t iTargetStride,
                                                arm_2d_size_t *
                                                __restrict ptTargetSize);

void __arm_2d_impl_rgb565_to_gray8( uint16_t *__RESTRICT phwSourceBase,
                                    int16_t iSourceStride,
                                    uint8_t *__RESTRICT pchTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize);

void __arm_2d_impl_cccn888_to_gray8(uint32_t *__RESTRICT pwSource,
                                    int16_t iSourceStride,
                                    uint8_t *__RESTRICT pchwTarget,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize);

void __arm_2d_impl_gray8_to_cccn888(uint8_t *__RESTRICT pchSourceBase,
                                    int16_t iSourceStride,
                                    uint32_t *__RESTRICT pwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize);

void __arm_2d_impl_rgb565_to_cccn888(uint16_t * __restrict phwSourceBase,
                                     int16_t iSourceStride,
                                     uint32_t * __restrict pwTargetBase,
                                     int16_t iTargetStride,
                                     arm_2d_size_t * __restrict ptCopySize);

void __arm_2d_impl_rgb565_transform(__arm_2d_param_copy_orig_t * ptParam,
                                    __arm_2d_transform_info_t * ptInfo);

void __arm_2d_impl_rgb565_transform_with_opacity(
                                        __arm_2d_param_copy_orig_t * ptParam,
                                        __arm_2d_transform_info_t * ptInfo,
                                        uint_fast16_t hwRatio);
                      
void __arm_2d_impl_rgb565_transform_with_src_mask(
                                        __arm_2d_param_copy_orig_msk_t * ptParam,
                                        __arm_2d_transform_info_t * ptInfo);

void __arm_2d_impl_rgb565_transform_with_src_mask_and_opacity(
                                        __arm_2d_param_copy_orig_msk_t * ptParam,
                                        __arm_2d_transform_info_t * ptInfo,
                                        uint_fast16_t hwRatio);

void __arm_2d_impl_rgb565_transform_with_src_chn_mask(
                                        __arm_2d_param_copy_orig_msk_t * ptParam,
                                        __arm_2d_transform_info_t * ptInfo);

void __arm_2d_impl_rgb565_transform_with_src_chn_mask_and_opacity(
                                        __arm_2d_param_copy_orig_msk_t * ptParam,
                                        __arm_2d_transform_info_t * ptInfo,
                                        uint_fast16_t hwRatio);

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif __IS_COMPILER_ARM_COMPILER_5__
#   pragma diag_warning 174,177,188,68,513,144,64
#elif __IS_COMPILER_IAR__
#   pragma diag_warning=pe111
#endif

#ifdef   __cplusplus
}
#endif

#endif
