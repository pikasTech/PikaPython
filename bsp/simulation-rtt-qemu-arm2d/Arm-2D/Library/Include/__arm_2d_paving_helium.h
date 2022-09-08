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
 * Title:        arm-2d_paving_helium.h
 * Description:  Provides definitions and code templates for generic paving
 *
 * $Date:        17. Sep 2021
 * $Revision:    V 0.6.0
 *
 * Target Processor:  Cortex-M cores
 *
 * -------------------------------------------------------------------- */


#ifndef __ARM_2D_PAVING_HELIUM_H__
#define __ARM_2D_PAVING_HELIUM_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"
#include "__arm_2d_paving.h"

#ifdef   __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/


#undef LOAD_SRC_DIRECT_16
#undef LOAD_SRC_DIRECT_32
#undef LOAD_SRC_X_MIRROR_16
#undef LOAD_SRC_X_MIRROR_32
#undef SETUP_MIRROR_COPY_16
#undef SETUP_MIRROR_COPY_32
#undef __ARM_2D_PAVING_2x2
#undef __ARM_2D_PAVING_1x2
#undef __ARM_2D_PAVING_2x1
#undef __ARM_2D_PAVING_1x1



/* Set predicate to true if vector different than color mask */
#define CMP_CL_MSK(sz, colour)   p = vcmpneq_m(in, colour, p)


#undef LOAD_SRC_DIRECT_8
#undef LOAD_SRC_DIRECT_16
#undef LOAD_SRC_DIRECT_32
#undef LOAD_SRC_X_MIRROR_8
#undef LOAD_SRC_X_MIRROR_16
#undef LOAD_SRC_X_MIRROR_32
#undef SETUP_MIRROR_COPY_8
#undef SETUP_MIRROR_COPY_16
#undef SETUP_MIRROR_COPY_32


#define LOAD_SRC_DIRECT_8(pSource, offset)                                      \
            vld1q_z(pSource, p);                                                \
            pSource += 16;

#define LOAD_SRC_DIRECT_16(pSource, offset)                                     \
            vld1q_z(pSource, p);                                                \
            pSource += 8;

#define LOAD_SRC_DIRECT_32(pSource, offset)                                     \
            vld1q_z(pSource, p);                                                \
            pSource += 4;

#define LOAD_SRC_X_MIRROR_8(pSource, offset)                                    \
            vldrbq_gather_offset_z(pSource, offset, p);                         \
            offset = vddupq_x_wb_u8(&curOffsetIdx, 1, p);

#define LOAD_SRC_X_MIRROR_16(pSource, offset)                                   \
            vldrhq_gather_shifted_offset_z(pSource, offset, p);                 \
            offset = vddupq_x_wb_u16(&curOffsetIdx, 1, p);

#define LOAD_SRC_X_MIRROR_32(pSource, offset)                                   \
            vldrwq_gather_shifted_offset_z(pSource, offset, p);                 \
            offset = vddupq_x_wb_u32(&curOffsetIdx, 1, p);

/* prepare Helium gather load offset */
#define SETUP_MIRROR_COPY_8(srcWidth)                                           \
            uint32_t curOffsetIdx = srcWidth - 1;                               \
            uint8x16_t offset = vddupq_wb_u8(&curOffsetIdx, 1);

#define SETUP_MIRROR_COPY_16(srcWidth)                                          \
            uint32_t curOffsetIdx = srcWidth - 1;                               \
            uint16x8_t offset = vddupq_wb_u16(&curOffsetIdx, 1);

#define SETUP_MIRROR_COPY_32(srcWidth)                                          \
            uint32_t curOffsetIdx = srcWidth - 1;                               \
            uint32x4_t offset = vddupq_wb_u32(&curOffsetIdx, 1);


/* Macros intercepting X or XY mirroring */
#define IS_PAVING_DIRECT_LOAD_PATTERN(sz)            (0)
#define IS_PAVING_X_MIRROR_LOAD_PATTERN(sz)          ((sz == 8)? 1: 0)
#define IS_PAVING_Y_MIRROR_LOAD_PATTERN(sz)          (0)
#define IS_PAVING_XY_MIRROR_LOAD_PATTERN(sz)         ((sz == 8)? 1: 0)


/* handle c8bit X or XY 2x2 mirroring when tile width > 256 */
/* uses 8-bit widened load allowing width up to 65K */
#define __ARM_2D_PAVING_2x2_8BIT_X_MIRROR_FIXUP(SRC_OFFSET, DIR,                \
                                                      SETUP_MIRROR, LOAD, ...)  \
{                                                                               \
    /* row iteration */                                                         \
    /* handle pair of source image row and fill in the column direction */      \
    do {                                                                        \
        uint32_t        rowCnt = 0;                                             \
                                                                                \
        pSourceBaseCur = pSourceBase;                                           \
                                                                                \
        /* single source row loop */                                            \
        do {                                                                    \
            uint8_t            *pDst = (uint8_t*)pTargetBaseCur;                \
            uint32_t            tilePairColsCnt = tilePairCols;                 \
                                                                                \
            /* column loop */                                                   \
            /* duplicate current source row into 2 x 2 destination across */    \
            /*destination columns */                                            \
            /*                                                                  \
             *  +-------+                                                       \
             *  |xxxxxxx|                                                       \
             *  |  src  |                                                       \
             *  +-------+                                                       \
             *                                                                  \
             *            <--------------->                                     \
             *                                                                  \
             *           ||       |       ||       |       ||                   \
             *       *   +========+=======+========+=======+                    \
             *       |   ||xxxxxxx|xxxxxxx||xxxxxxx|xxxxxxx||...                \
             *       |   ||       |       ||       |       ||...                \
             *       |   +--------+-------+--------+-------+-...                \
             *       |   ||xxxxxxx|xxxxxxx||xxxxxxx|xxxxxxx||...                \
             *       |   ||       |       ||       |       ||...                \
             *       *   +========+=======+========+=======+                    \
             *           ||       |       ||       |       ||                   \
             */                                                                 \
            do {                                                                \
                int32_t              dstColCnt = srcWidth;                      \
                const uint8_t       *pSource = (const uint8_t*)pSourceBaseCur   \
                                                              + SRC_OFFSET;     \
                uint8_t             *pDst00 = (uint8_t*)pDst;                   \
                uint8_t             *pDst01 = (uint8_t*)pDst00 + srcWidth;      \
                uint8_t             *pDst10 = (uint8_t*)pDst + srcHeight *      \
                                                                 iTargetStride; \
                uint8_t             *pDst11 = (uint8_t*)pDst10 + srcWidth;      \
                                                                                \
                uint32_t            curOffsetIdx = srcWidth - 1;                \
                uint16x8_t          offset = vddupq_wb_u16(&curOffsetIdx, 1);   \
                                                                                \
                /* duplicate current source line into 2x2 destinations */       \
                do {                                                            \
                    mve_pred16_t    p = vctp16q(dstColCnt);                     \
                    uint8x16_t      in;                                         \
                    in = vldrbq_gather_offset_z_u16(pSource, offset, p);        \
                    offset = vddupq_x_wb_u16(&curOffsetIdx, 1, p);              \
                                                                                \
                    /* placeholder for color masking */                         \
                    __VA_ARGS__;                                                \
                                                                                \
                    vstrbq_p_u16(pDst00, in, p);                                \
                    vstrbq_p_u16(pDst01, in, p);                                \
                    vstrbq_p_u16(pDst10, in, p);                                \
                    vstrbq_p_u16(pDst11, in, p);                                \
                                                                                \
                    pDst00 += 8;                                                \
                    pDst01 += 8;                                                \
                    pDst10 += 8;                                                \
                    pDst11 += 8;                                                \
                    dstColCnt -= 8;                                             \
                }                                                               \
                while (dstColCnt > 0);                                          \
                                                                                \
                pDst += 2 * srcWidth;                                           \
                tilePairColsCnt--;                                              \
            }                                                                   \
            while (tilePairColsCnt != 0);                                       \
                                                                                \
            rowCnt ++;                                                          \
            pTargetBaseCur += iTargetStride;                                    \
            pSourceBaseCur += (iSourceStride * DIR);                            \
        }                                                                       \
        while (rowCnt < ptSourceSize->iHeight);                                 \
                                                                                \
        pTargetBaseCur += srcHeight * iTargetStride;                            \
        tilePairRows--;                                                         \
    }                                                                           \
    while (tilePairRows != 0);                                                  \
}



#define __ARM_2D_PAVING_2x2(sz, SRC_OFFSET, DIR, SETUP_MIRROR, LOAD, ...)       \
{                                                                               \
    uint32_t            srcWidth = ptSourceSize->iWidth;                        \
    uint32_t            srcHeight = ptSourceSize->iHeight;                      \
    const ARM_PIX_SCLTYP(sz) *pSourceBaseCur;                                   \
    ARM_PIX_SCLTYP(sz) *pTargetBaseCur = pTargetBase;                           \
                                                                                \
    if ((sz == 8) && (srcWidth >= 256) && IS_##LOAD) {                          \
        /* special case for 8-bit and X & XY mirror  */                         \
        /* width does not fit in 8-bit, need widening */                        \
        /* will be optimized away for all other cases */                        \
        /* not executed unconditionally as slower */                            \
        __ARM_2D_PAVING_2x2_8BIT_X_MIRROR_FIXUP(SRC_OFFSET, DIR,                \
                            SETUP_MIRROR, LOAD, __VA_ARGS__)                    \
    } else                                                                      \
    /* row iteration */                                                         \
    /* handle pair of source image row and fill in the column direction */      \
    do {                                                                        \
        uint32_t        rowCnt = 0;                                             \
                                                                                \
        pSourceBaseCur = pSourceBase;                                           \
                                                                                \
        /* single source row loop */                                            \
        do {                                                                    \
            ARM_PIX_SCLTYP(sz) *pDst = pTargetBaseCur;                          \
            uint32_t            tilePairColsCnt = tilePairCols;                 \
                                                                                \
            /* column loop */                                                   \
            /* duplicate current source row into 2 x 2 destination across */    \
            /*destination columns */                                            \
            /*                                                                  \
             *  +-------+                                                       \
             *  |xxxxxxx|                                                       \
             *  |  src  |                                                       \
             *  +-------+                                                       \
             *                                                                  \
             *            <--------------->                                     \
             *                                                                  \
             *           ||       |       ||       |       ||                   \
             *       *   +========+=======+========+=======+                    \
             *       |   ||xxxxxxx|xxxxxxx||xxxxxxx|xxxxxxx||...                \
             *       |   ||       |       ||       |       ||...                \
             *       |   +--------+-------+--------+-------+-...                \
             *       |   ||xxxxxxx|xxxxxxx||xxxxxxx|xxxxxxx||...                \
             *       |   ||       |       ||       |       ||...                \
             *       *   +========+=======+========+=======+                    \
             *           ||       |       ||       |       ||                   \
             */                                                                 \
            do {                                                                \
                int32_t              dstColCnt = srcWidth;                      \
                const ARM_PIX_SCLTYP(sz)  *pSource = pSourceBaseCur             \
                                                              + SRC_OFFSET;     \
                ARM_PIX_SCLTYP(sz)  *pDst00 = pDst;                             \
                ARM_PIX_SCLTYP(sz)  *pDst01 = pDst00 + srcWidth;                \
                ARM_PIX_SCLTYP(sz)  *pDst10 = pDst + srcHeight * iTargetStride; \
                ARM_PIX_SCLTYP(sz)  *pDst11 = pDst10 + srcWidth;                \
                                                                                \
                SETUP_MIRROR(srcWidth);                                         \
                                                                                \
                /* duplicate current source line into 2x2 destinations */       \
                do {                                                            \
                    mve_pred16_t        p =                                     \
                        ARM_CONNECT2(ARM_CONNECT2(vctp, sz),q)(dstColCnt);      \
                    ARM_PIX_VECTYP(sz)  in;                                     \
                                                                                \
                    in = LOAD(pSource, offset);                                 \
                    /* placeholder for color masking */                         \
                    __VA_ARGS__;                                                \
                                                                                \
                    vst1q_p(pDst00, in, p);                                     \
                    vst1q_p(pDst01, in, p);                                     \
                    vst1q_p(pDst10, in, p);                                     \
                    vst1q_p(pDst11, in, p);                                     \
                                                                                \
                    pDst00 += ARM_PIX_VECELT(sz);                               \
                    pDst01 += ARM_PIX_VECELT(sz);                               \
                    pDst10 += ARM_PIX_VECELT(sz);                               \
                    pDst11 += ARM_PIX_VECELT(sz);                               \
                    dstColCnt -= ARM_PIX_VECELT(sz);                            \
                }                                                               \
                while (dstColCnt > 0);                                          \
                                                                                \
                pDst += 2 * srcWidth;                                           \
                tilePairColsCnt--;                                              \
            }                                                                   \
            while (tilePairColsCnt != 0);                                       \
                                                                                \
            rowCnt ++;                                                          \
            pTargetBaseCur += iTargetStride;                                    \
            pSourceBaseCur += (iSourceStride * DIR);                            \
        }                                                                       \
        while (rowCnt < ptSourceSize->iHeight);                                 \
                                                                                \
        pTargetBaseCur += srcHeight * iTargetStride;                            \
        tilePairRows--;                                                         \
    }                                                                           \
    while (tilePairRows != 0);                                                  \
}


/* handle c8bit X or XY 1x2 mirroring when tile width > 256 */
/* uses 8-bit widened load allowing width up to 65K */
#define __ARM_2D_PAVING_1x2_8BIT_X_MIRROR_FIXUP(SRC_OFFSET, DIR,                \
                                                SETUP_MIRROR, LOAD, ...)        \
{                                                                               \
    /* row iteration */                                                         \
    /* handle pair of source image row and fill in the column direction */      \
    do {                                                                        \
        uint32_t        rowCnt = 0;                                             \
                                                                                \
        pSourceBaseCur = pSourceBase;                                           \
                                                                                \
        /* single source row loop */                                            \
        do {                                                                    \
            int32_t              dstColCnt = destWidth;                         \
            const uint8_t       *pSource = (const uint8_t*)pSourceBaseCur       \
                                                              + SRC_OFFSET;     \
            uint8_t            *pDst = (uint8_t*)pTargetBaseCur;                \
            uint8_t            *pDst00 = pDst;                                  \
            uint8_t            *pDst10 = pDst + srcHeight * iTargetStride;      \
                                                                                \
            /* column loop */                                                   \
            /* duplicate current source row into 1 x 2 destination across */    \
            /* destination columns */                                           \
            /*                                                                  \
             *  +-------+                                                       \
             *  |xxxxxxx|                                                       \
             *  |  src  |                                                       \
             *  +-------+                                                       \
             *                                                                  \
             *           ..||       ||                                          \
             *       *   ==+========+=...                                       \
             *       |   ..||xxxxxxx||...                                       \
             *       |   ..||       ||...                                       \
             *       |   ..+--------+-...                                       \
             *       |   ..||xxxxxxx||...                                       \
             *       |   ..||       ||...                                       \
             *       *   ==+========+=...                                       \
             *           ..||       ||...                                       \
             */                                                                 \
            uint32_t            curOffsetIdx = srcWidth - 1;                    \
            uint16x8_t          offset = vddupq_wb_u16(&curOffsetIdx, 1);       \
                                                                                \
            /* duplicate current source line into 2x2 destinations */           \
            do {                                                                \
                mve_pred16_t    p = vctp16q(dstColCnt);                         \
                uint8x16_t      in;                                             \
                in = vldrbq_gather_offset_z_u16(pSource, offset, p);            \
                offset = vddupq_x_wb_u16(&curOffsetIdx, 1, p);                  \
                                                                                \
                /* placeholder for color masking */                             \
                __VA_ARGS__;                                                    \
                                                                                \
                vstrbq_p_u16(pDst00, in, p);                                    \
                vstrbq_p_u16(pDst10, in, p);                                    \
                                                                                \
                pDst00 += 8;                                                    \
                pDst10 += 8;                                                    \
                dstColCnt -= 8;                                                 \
            }                                                                   \
            while (dstColCnt > 0);                                              \
                                                                                \
            rowCnt ++;                                                          \
            pTargetBaseCur += iTargetStride;                                    \
            pSourceBaseCur += (iSourceStride * DIR);                            \
        }                                                                       \
        while (rowCnt < ptSourceSize->iHeight);                                 \
                                                                                \
        pTargetBaseCur += srcHeight * iTargetStride;                            \
        tilePairRows--;                                                         \
    }                                                                           \
    while (tilePairRows != 0);                                                  \
}


#define __ARM_2D_PAVING_1x2(sz, SRC_OFFSET, DIR, SETUP_MIRROR, LOAD, ...)       \
{                                                                               \
    uint32_t            srcWidth = ptSourceSize->iWidth;                        \
    uint32_t            srcHeight = ptSourceSize->iHeight;                      \
    const ARM_PIX_SCLTYP(sz) *pSourceBaseCur;                                   \
    ARM_PIX_SCLTYP(sz) *pTargetBaseCur = pTargetBase;                           \
                                                                                \
    if ((sz == 8) && (srcWidth >= 256) && IS_##LOAD) {                          \
        /* special case for 8-bit and X & XY mirror  */                         \
        /* width does not fit in 8-bit, need widening */                        \
        /* will be optimized away for all other cases */                        \
        /* not executed unconditionally as slower */                            \
        __ARM_2D_PAVING_1x2_8BIT_X_MIRROR_FIXUP(SRC_OFFSET, DIR,                \
                            SETUP_MIRROR, LOAD, __VA_ARGS__)                    \
    } else                                                                      \
    /* row iteration */                                                         \
    /* handle pair of source image row and fill in the column direction */      \
    do {                                                                        \
        uint32_t        rowCnt = 0;                                             \
                                                                                \
        pSourceBaseCur = pSourceBase;                                           \
                                                                                \
        /* single source row loop */                                            \
        do {                                                                    \
            int32_t              dstColCnt = destWidth;                         \
            const ARM_PIX_SCLTYP(sz)  *pSource = pSourceBaseCur + SRC_OFFSET;   \
            ARM_PIX_SCLTYP(sz)  *pDst = pTargetBaseCur;                         \
            ARM_PIX_SCLTYP(sz)  *pDst00 = pDst;                                 \
            ARM_PIX_SCLTYP(sz)  *pDst10 = pDst + srcHeight * iTargetStride;     \
                                                                                \
            /* column loop */                                                   \
            /* duplicate current source row into 1 x 2 destination across */    \
            /* destination columns */                                           \
            /*                                                                  \
             *  +-------+                                                       \
             *  |xxxxxxx|                                                       \
             *  |  src  |                                                       \
             *  +-------+                                                       \
             *                                                                  \
             *           ..||       ||                                          \
             *       *   ==+========+=...                                       \
             *       |   ..||xxxxxxx||...                                       \
             *       |   ..||       ||...                                       \
             *       |   ..+--------+-...                                       \
             *       |   ..||xxxxxxx||...                                       \
             *       |   ..||       ||...                                       \
             *       *   ==+========+=...                                       \
             *           ..||       ||...                                       \
             */                                                                 \
            SETUP_MIRROR(srcWidth);                                             \
                                                                                \
            /* duplicate current source line into 2x2 destinations */           \
            do {                                                                \
                mve_pred16_t        p =                                         \
                    ARM_CONNECT2(ARM_CONNECT2(vctp, sz),q)(dstColCnt);          \
                ARM_PIX_VECTYP(sz)  in;                                         \
                                                                                \
                in = LOAD(pSource, offset);                                     \
                /* placeholder for color masking */                             \
                __VA_ARGS__;                                                    \
                                                                                \
                vst1q_p(pDst00, in, p);                                         \
                vst1q_p(pDst10, in, p);                                         \
                                                                                \
                pDst00 += ARM_PIX_VECELT(sz);                                   \
                pDst10 += ARM_PIX_VECELT(sz);                                   \
                dstColCnt -= ARM_PIX_VECELT(sz);                                \
            }                                                                   \
            while (dstColCnt > 0);                                              \
                                                                                \
            rowCnt ++;                                                          \
            pTargetBaseCur += iTargetStride;                                    \
            pSourceBaseCur += (iSourceStride * DIR);                            \
        }                                                                       \
        while (rowCnt < ptSourceSize->iHeight);                                 \
                                                                                \
        pTargetBaseCur += srcHeight * iTargetStride;                            \
        tilePairRows--;                                                         \
    }                                                                           \
    while (tilePairRows != 0);                                                  \
}

/* handle c8bit X or XY 2x1 mirroring when tile width > 256 */
/* uses 8-bit widened load allowing width up to 65K */
#define __ARM_2D_PAVING_2x1_8BIT_X_MIRROR_FIXUP(SRC_OFFSET, DIR,                \
                                                 SETUP_MIRROR, LOAD, ...)       \
{                                                                               \
                                                                                \
    /* row iteration */                                                         \
    /* handle pair of source image row and fill in the column direction */      \
    /*                                                                          \
     *  +-------+                                                               \
     *  | src   |                                                               \
     *  +-------+                                                               \
     *                                                                          \
     *                             tilePairCols                                 \
     *                  <---------------+--------........>                      \
     *                                                                          \
     *                  +=======+=======++=======+=======++......               \
     *                  ||      |       ||       |       ||                     \
     *                  +=======+=======++=======+=======++                     \
     *                  ........                                                \
     */                                                                         \
                                                                                \
    pSourceBaseCur = pSourceBase;                                               \
                                                                                \
    /* copy 2 x 2 source image block */                                         \
    do {                                                                        \
        uint8_t            *pDst = (uint8_t*)pTargetBaseCur;                    \
        int16_t   tilePairColsCnt = tilePairCols;                               \
                                                                                \
        do {                                                                    \
            int                 srcColCnt = srcWidth;                           \
            const uint8_t       *pSource = (const uint8_t*)pSourceBaseCur       \
                                                              + SRC_OFFSET;     \
            uint8_t *pDst00 = pDst;                                             \
            uint8_t *pDst01 = pDst00 + srcColCnt;                               \
                                                                                \
             uint32_t            curOffsetIdx = srcWidth - 1;                   \
            uint16x8_t          offset = vddupq_wb_u16(&curOffsetIdx, 1);       \
                                                                                \
            /* duplicate current source line into 4 destinations */             \
            do {                                                                \
                mve_pred16_t    p = vctp16q(srcColCnt);                         \
                uint8x16_t      in;                                             \
                in = vldrbq_gather_offset_z_u16(pSource, offset, p);            \
                offset = vddupq_x_wb_u16(&curOffsetIdx, 1, p);                  \
                /* placeholder for color masking */                             \
                __VA_ARGS__;                                                    \
                                                                                \
                vstrbq_p_u16(pDst00, in, p);                                    \
                vstrbq_p_u16(pDst01, in, p);                                    \
                                                                                \
                pDst00 +=  8;                                                   \
                pDst01 +=  8;                                                   \
                srcColCnt -=  8;                                                \
            }                                                                   \
            while ((int32_t) srcColCnt > 0);                                    \
                                                                                \
            pDst += 2 * srcWidth;                                               \
            tilePairColsCnt--;                                                  \
        }                                                                       \
        while (tilePairColsCnt != 0);                                           \
                                                                                \
        rowCnt++;                                                               \
        pTargetBaseCur += iTargetStride;                                        \
        pSourceBaseCur += (iSourceStride * DIR);                                \
    }                                                                           \
    while (rowCnt < destHeight);                                                \
}


#define __ARM_2D_PAVING_2x1(sz, SRC_OFFSET, DIR, SETUP_MIRROR, LOAD, ...)       \
{                                                                               \
    uint32_t             srcWidth = ptSourceSize->iWidth;                       \
    const ARM_PIX_SCLTYP(sz)  *pSourceBaseCur;                                  \
    ARM_PIX_SCLTYP(sz)  *pTargetBaseCur = pTargetBase;                          \
    uint32_t             rowCnt = 0;                                            \
                                                                                \
    if ((sz == 8) && (srcWidth >= 256) && IS_##LOAD) {                          \
        /* special case for 8-bit and X & XY mirror  */                         \
        /* width does not fit in 8-bit, need widening */                        \
        /* will be optimized away for all other cases */                        \
        /* not executed unconditionally as slower */                            \
        __ARM_2D_PAVING_2x1_8BIT_X_MIRROR_FIXUP(SRC_OFFSET, DIR,                \
                            SETUP_MIRROR, LOAD, __VA_ARGS__)                    \
    } else {                                                                    \
    /* row iteration */                                                         \
    /* handle pair of source image row and fill in the column direction */      \
    /*                                                                          \
     *  +-------+                                                               \
     *  | src   |                                                               \
     *  +-------+                                                               \
     *                                                                          \
     *                             tilePairCols                                 \
     *                  <---------------+--------........>                      \
     *                                                                          \
     *                  +=======+=======++=======+=======++......               \
     *                  ||      |       ||       |       ||                     \
     *                  +=======+=======++=======+=======++                     \
     *                  ........                                                \
     */                                                                         \
                                                                                \
    pSourceBaseCur = pSourceBase;                                               \
                                                                                \
    /* copy 2 x 2 source image block */                                         \
    do {                                                                        \
        ARM_PIX_SCLTYP(sz)  *pDst = pTargetBaseCur;                             \
        ARM_PIX_SCLTYP(sz)   tilePairColsCnt = tilePairCols;                    \
                                                                                \
        do {                                                                    \
            int                 srcColCnt = srcWidth;                           \
            const ARM_PIX_SCLTYP(sz) *pSource = pSourceBaseCur + SRC_OFFSET;    \
            ARM_PIX_SCLTYP(sz) *pDst00 = pDst;                                  \
            ARM_PIX_SCLTYP(sz) *pDst01 = pDst00 + srcColCnt;                    \
                                                                                \
            SETUP_MIRROR(srcWidth);                                             \
                                                                                \
            /* duplicate current source line into 4 destinations */             \
            do {                                                                \
                mve_pred16_t        p =                                         \
                    ARM_CONNECT2(ARM_CONNECT2(vctp, sz),q)(srcColCnt);          \
                ARM_PIX_VECTYP(sz)  in;                                         \
                                                                                \
                in = LOAD(pSource, offset);                                     \
                /* placeholder for color masking */                             \
                __VA_ARGS__;                                                    \
                                                                                \
                vst1q_p(pDst00, in, p);                                         \
                vst1q_p(pDst01, in, p);                                         \
                                                                                \
                pDst00 += ARM_PIX_VECELT(sz);                                   \
                pDst01 += ARM_PIX_VECELT(sz);                                   \
                srcColCnt -= ARM_PIX_VECELT(sz);                                \
            }                                                                   \
            while ((int32_t) srcColCnt > 0);                                    \
                                                                                \
            pDst += 2 * srcWidth;                                               \
            tilePairColsCnt--;                                                  \
        }                                                                       \
        while (tilePairColsCnt != 0);                                           \
                                                                                \
        rowCnt++;                                                               \
        pTargetBaseCur += iTargetStride;                                        \
        pSourceBaseCur += (iSourceStride * DIR);                                \
    }                                                                           \
    while (rowCnt < destHeight);                                                \
    }                                                                           \
}


/* handle c8bit X or XY 1x1 mirroring when tile width > 256 */
/* uses 8-bit widened load allowing width up to 65K */
#define __ARM_2D_PAVING_1x1_8BIT_X_MIRROR_FIXUP(SRC_OFFSET, DIR,                \
                                                 SETUP_MIRROR, LOAD, ...)       \
{                                                                               \
    pSource += SRC_OFFSET;                                                      \
                                                                                \
    for (int_fast16_t y = 0; y < ptDstCopySize->iHeight; y++) {                 \
        uint8_t         *pDst = (uint8_t  *)pTarget;                            \
        const uint8_t   *pSrc = (const uint8_t*)pSource;                        \
        uint32_t         dstWidth = ptDstCopySize->iWidth;                      \
        uint32_t         curOffsetIdx = srcWidth - 1;                           \
        uint16x8_t       offset = vddupq_wb_u16(&curOffsetIdx, 1);              \
                                                                                \
        do {                                                                    \
            mve_pred16_t    p = vctp16q(dstWidth);                              \
            uint8x16_t      in;                                                 \
            in = vldrbq_gather_offset_z_u16(pSrc, offset, p);                   \
            offset = vddupq_x_wb_u16(&curOffsetIdx, 1, p);                      \
            /* placeholder for color masking */                                 \
            __VA_ARGS__;                                                        \
                                                                                \
            vstrbq_p_u16(pDst, in, p);                                          \
                                                                                \
            pDst += 8;                                                          \
            dstWidth -= 8;                                                      \
        }                                                                       \
        while ((int32_t) dstWidth > 0);                                         \
                                                                                \
        pSource += (iSourceStride * DIR);                                       \
        pTarget += iTargetStride;                                               \
    }                                                                           \
}


#define __ARM_2D_PAVING_1x1(sz, SRC_OFFSET, DIR, SETUP_MIRROR, LOAD, ...)       \
{                                                                               \
    uint32_t             srcWidth = ptSrcCopySize->iWidth;                      \
    if ((sz == 8) && (srcWidth >= 256) && IS_##LOAD) {                          \
        /* special case for 8-bit and X & XY mirror  */                         \
        /* width does not fit in 8-bit, need widening */                        \
        /* will be optimized away for all other cases */                        \
        /* not executed unconditionally as slower */                            \
        __ARM_2D_PAVING_1x1_8BIT_X_MIRROR_FIXUP(SRC_OFFSET, DIR,                \
                            SETUP_MIRROR, LOAD, __VA_ARGS__)                    \
    } else {                                                                    \
        pSource += SRC_OFFSET;                                                  \
                                                                                \
        for (int_fast16_t y = 0; y < ptDstCopySize->iHeight; y++) {             \
            ARM_PIX_SCLTYP(sz)  *pDst = pTarget;                                \
            ARM_PIX_SCLTYP(sz)  *pSrc = pSource;                                \
            uint32_t             dstWidth = ptDstCopySize->iWidth;              \
                                                                                \
            SETUP_MIRROR(srcWidth);                                             \
                                                                                \
            do {                                                                \
                mve_pred16_t        p =                                         \
                    ARM_CONNECT2(ARM_CONNECT2(vctp, sz), q) (dstWidth);         \
                ARM_PIX_VECTYP(sz)  in;                                         \
                                                                                \
                in = LOAD(pSrc, offset);                                        \
                /* placeholder for color masking */                             \
                __VA_ARGS__;                                                    \
                                                                                \
                vst1q_p(pDst, in, p);                                           \
                                                                                \
                pDst += ARM_PIX_VECELT(sz);                                     \
                dstWidth -= ARM_PIX_VECELT(sz);                                 \
            }                                                                   \
            while ((int32_t) dstWidth > 0);                                     \
                                                                                \
            pSource += (iSourceStride * DIR);                                   \
            pTarget += iTargetStride;                                           \
        }                                                                       \
    }                                                                           \
}

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef   __cplusplus
}
#endif

#endif
