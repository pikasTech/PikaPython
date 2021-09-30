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
 * Title:        arm-2d_paving.h
 * Description:  Provides definitions and code templates for generic paving
 *
 * $Date:        20. Jan 2021
 * $Revision:    V 0.5.0
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


#define vec_elts(sz)            (128/sz)

/* Set predicate to true if vector different than color mask */
#define CMP_CL_MSK(sz, colour)   p = ARM_CONNECT2(vcmpneq_m_n_u, sz)(in, colour, p)

#define LOAD_SRC_DIRECT_16(pSource, offset)                                     \
            vld1q_z(pSource, p);                                                \
            pSource += 8;

#define LOAD_SRC_DIRECT_32(pSource, offset)                                     \
            vld1q_z(pSource, p);                                                \
            pSource += 4;

#define LOAD_SRC_X_MIRROR_16(pSource, offset)                                   \
            vldrhq_gather_shifted_offset_z(pSource, offset, p);                 \
            offset = vddupq_x_wb_u16(&curOffsetIdx, 1, p);

#define LOAD_SRC_X_MIRROR_32(pSource, offset)                                   \
            vldrwq_gather_shifted_offset_z(pSource, offset, p);                 \
            offset = vddupq_x_wb_u32(&curOffsetIdx, 1, p);

/* prepare Helium gather load offset */
#define SETUP_MIRROR_COPY_16(srcWidth)                                          \
            uint32_t curOffsetIdx = srcWidth - 1;                               \
            uint16x8_t offset = vddupq_wb_u16(&curOffsetIdx, 1);

#define SETUP_MIRROR_COPY_32(srcWidth)                                          \
            uint32_t curOffsetIdx = srcWidth - 1;                               \
            uint32x4_t offset = vddupq_wb_u32(&curOffsetIdx, 1);

#define __ARM_2D_PAVING_2x2(sz, SRC_OFFSET, DIR, SETUP_MIRROR, LOAD, ...)       \
{                                                                               \
    uint32_t            srcWidth = ptSourceSize->iWidth;                        \
    uint32_t            srcHeight = ptSourceSize->iHeight;                      \
    ARM_PIX_SCLTYP(sz) *pSourceBaseCur;                                         \
    ARM_PIX_SCLTYP(sz) *pTargetBaseCur = pTargetBase;                           \
                                                                                \
                                                                                \
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
                ARM_PIX_SCLTYP(sz)  *pSource = pSourceBaseCur + SRC_OFFSET;     \
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
                    pDst00 += vec_elts(sz);                                     \
                    pDst01 += vec_elts(sz);                                     \
                    pDst10 += vec_elts(sz);                                     \
                    pDst11 += vec_elts(sz);                                     \
                    dstColCnt -= vec_elts(sz);                                  \
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


#define __ARM_2D_PAVING_1x2(sz, SRC_OFFSET, DIR, SETUP_MIRROR, LOAD, ...)       \
{                                                                               \
    uint32_t            srcWidth = ptSourceSize->iWidth;                        \
    uint32_t            srcHeight = ptSourceSize->iHeight;                      \
    ARM_PIX_SCLTYP(sz)  *pSourceBaseCur;                                        \
    ARM_PIX_SCLTYP(sz)  *pTargetBaseCur = pTargetBase;                          \
                                                                                \
                                                                                \
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
            ARM_PIX_SCLTYP(sz)  *pSource = pSourceBaseCur + SRC_OFFSET;         \
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
                pDst00 += vec_elts(sz);                                         \
                pDst10 += vec_elts(sz);                                         \
                dstColCnt -= vec_elts(sz);                                      \
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


#define __ARM_2D_PAVING_2x1(sz, SRC_OFFSET, DIR, SETUP_MIRROR, LOAD, ...)       \
{                                                                               \
    uint32_t             srcWidth = ptSourceSize->iWidth;                       \
    ARM_PIX_SCLTYP(sz)  *pSourceBaseCur;                                        \
    ARM_PIX_SCLTYP(sz)  *pTargetBaseCur = pTargetBase;                          \
    uint32_t             rowCnt = 0;                                            \
                                                                                \
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
        ARM_PIX_SCLTYP(sz)  *pDst = pTargetBaseCur;                             \
        ARM_PIX_SCLTYP(sz)   tilePairColsCnt = tilePairCols;                    \
                                                                                \
        do {                                                                    \
            int                 srcColCnt = srcWidth;                           \
            ARM_PIX_SCLTYP(sz) *pSource = pSourceBaseCur + SRC_OFFSET;          \
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
                pDst00 += vec_elts(sz);                                         \
                pDst01 += vec_elts(sz);                                         \
                srcColCnt -= vec_elts(sz);                                      \
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


#define __ARM_2D_PAVING_1x1(sz, SRC_OFFSET, DIR, SETUP_MIRROR, LOAD, ...)       \
{                                                                               \
    pSource += SRC_OFFSET;                                                      \
                                                                                \
    for (int_fast16_t y = 0; y < ptDstCopySize->iHeight; y++) {                 \
        ARM_PIX_SCLTYP(sz)  *pDst = pTarget;                                    \
        ARM_PIX_SCLTYP(sz)  *pSrc = pSource;                                    \
        uint32_t             srcWidth = ptSrcCopySize->iWidth;                  \
        uint32_t             dstWidth = ptDstCopySize->iWidth;                  \
                                                                                \
        SETUP_MIRROR(srcWidth);                                                 \
                                                                                \
        do {                                                                    \
            mve_pred16_t        p =                                             \
                ARM_CONNECT2(ARM_CONNECT2(vctp, sz), q) (dstWidth);             \
            ARM_PIX_VECTYP(sz)  in;                                             \
                                                                                \
            in = LOAD(pSrc, offset);                                            \
            /* placeholder for color masking */                                 \
            __VA_ARGS__;                                                        \
                                                                                \
            vst1q_p(pDst, in, p);                                               \
                                                                                \
            pDst += vec_elts(sz);                                               \
            dstWidth -= vec_elts(sz);                                           \
        }                                                                       \
        while ((int32_t) dstWidth > 0);                                         \
                                                                                \
        pSource += (iSourceStride * DIR);                                       \
        pTarget += iTargetStride;                                               \
    }                                                                           \
}

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef   __cplusplus
}
#endif

#endif
