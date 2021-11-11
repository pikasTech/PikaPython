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


#ifndef __ARM_2D_PAVING_H__
#define __ARM_2D_PAVING_H__       

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
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define SETUP_DIRECT_COPY_8(srcWidth)          (void)srcWidth;
#define SETUP_DIRECT_COPY_16(srcWidth)         (void)srcWidth;
#define SETUP_DIRECT_COPY_32(srcWidth)         (void)srcWidth;


/* scalar copy w/o mirroring */
#define LOAD_SRC_DIRECT_8(pSource, offset)     *pSource++;
#define LOAD_SRC_DIRECT_16(pSource, offset)    *pSource++;
#define LOAD_SRC_DIRECT_32(pSource, offset)    *pSource++;

#define LOAD_SRC_X_MIRROR_8(pSource, offset)   pSource[offset]; offset -= 1;
#define LOAD_SRC_X_MIRROR_16(pSource, offset)  pSource[offset]; offset -= 1;
#define LOAD_SRC_X_MIRROR_32(pSource, offset)  pSource[offset]; offset -= 1;

#define SETUP_MIRROR_COPY_8(srcWidth)          uint32_t offset = srcWidth - 1;
#define SETUP_MIRROR_COPY_16(srcWidth)         uint32_t offset = srcWidth - 1;
#define SETUP_MIRROR_COPY_32(srcWidth)         uint32_t offset = srcWidth - 1;


#define INCR_Y_DIR         1
#define DECR_Y_DIR         -1
#define BOTTOM_TO_TOP      INCR_Y_DIR
#define TOP_TO_BOTTOM      DECR_Y_DIR

#ifndef PAVING_OP
    #define PAVING_OP(__DES_ADDR, __SRC)                                        \
            do {                                                                \
                *(__DES_ADDR) = (__SRC);                                        \
            }while(0)
#endif

#define PAVING_DIRECT_START_OFFS(strd, heig)    0
#define PAVING_DIRECT_READ_DIR                  BOTTOM_TO_TOP
#define PAVING_DIRECT_SETUP_COPY(sz)            ARM_CONNECT2(SETUP_DIRECT_COPY_,sz)
#define PAVING_DIRECT_LOAD_PATTERN(sz)          ARM_CONNECT2(LOAD_SRC_DIRECT_,sz)

#define PAVING_X_MIRROR_START_OFFS(strd, heig)  PAVING_DIRECT_START_OFFS(strd, heig)
#define PAVING_X_MIRROR_READ_DIR                PAVING_DIRECT_READ_DIR
#define PAVING_X_MIRROR_SETUP_COPY(sz)          ARM_CONNECT2(SETUP_MIRROR_COPY_,sz)
#define PAVING_X_MIRROR_LOAD_PATTERN(sz)        ARM_CONNECT2(LOAD_SRC_X_MIRROR_,sz)

#define PAVING_Y_MIRROR_START_OFFS(strd, heig)  (strd * (heig - 1))
#define PAVING_Y_MIRROR_READ_DIR                TOP_TO_BOTTOM
#define PAVING_Y_MIRROR_SETUP_COPY(sz)          PAVING_DIRECT_SETUP_COPY(sz)
#define PAVING_Y_MIRROR_LOAD_PATTERN(sz)        PAVING_DIRECT_LOAD_PATTERN(sz)

#define PAVING_XY_MIRROR_START_OFFS(strd, heig) PAVING_Y_MIRROR_START_OFFS(strd, heig)
#define PAVING_XY_MIRROR_READ_DIR               PAVING_Y_MIRROR_READ_DIR
#define PAVING_XY_MIRROR_SETUP_COPY(sz)         PAVING_X_MIRROR_SETUP_COPY(sz)
#define PAVING_XY_MIRROR_LOAD_PATTERN(sz)       PAVING_X_MIRROR_LOAD_PATTERN(sz)



#define __ARM_2D_MEM_FILL_GENERIC(  pSourceBase,                                \
                                    iSourceStride,                              \
                                    ptSourceSize,                               \
                                    pTargetBase,                                \
                                    iTargetStride,                              \
                                    ptTargetSize,                               \
                                    pPavFct,                                    \
                                    ...)                                        \
{                                                                               \
    uint16_t        targetIWidth = ptTargetSize->iWidth;                        \
    uint16_t        sourceIWidth = ptSourceSize->iWidth;                        \
    uint16_t        targetIHeight = ptTargetSize->iHeight;                      \
    uint16_t        sourceIHeight = ptSourceSize->iHeight;                      \
    uint32_t        tilePairRows = (targetIHeight / (2 * sourceIHeight));       \
    uint32_t        tilePairCols = (targetIWidth / (2 * sourceIWidth));         \
    int32_t         residueW = 0;                                               \
    int32_t         residueH = 0;                                               \
    uint32_t        targetOffsetRows = 0;                                       \
    uint32_t        targetOffsetCols = 0;                                       \
                                                                                \
                                                                                \
    /* can we handle vertical pairs of image ? */                               \
    if (tilePairRows >= 1) {                                                    \
        /* target row coordinate after paving */                                \
        targetOffsetRows = tilePairRows * (2 * sourceIHeight);                  \
        residueW = targetIWidth;                                                \
                                                                                \
        /* can we handle horizontal pairs of image ? */                         \
        if (tilePairCols >= 1) {                                                \
            /* target column coordinate after paving */                         \
            targetOffsetCols = tilePairCols * (2 * sourceIWidth);               \
                                                                                \
            /* run 2 x 2 image paving */                                        \
            /*                                                                  \
             *       +=======+=======+=======+=======+=======+=======++.....    \
             *       ||xxxxxx|xxxxxxx||      |       ||      |       ||         \
             *       +-------+-------+-------+-------+-------+-------+-.....    \
             *       ||xxxxxx|xxxxxxx||      |       ||      |       ||         \
             *       +=======+=======+=======+=======+=======+=======++.....    \
             *       ||      |       ||      |       ||      |       ||         \
             *       +-------+-------+-------+-------+-------+-------+-.....    \
             *       ||      |       ||      |       ||      |       ||         \
             *       +=======+=======+=======+=======+=======+=======++.....    \
             */                                                                 \
            pPavFct->pav_2x2(pSourceBase, iSourceStride, ptSourceSize,          \
                                          pTargetBase, iTargetStride,           \
                                          tilePairRows, tilePairCols            \
                                          ,##__VA_ARGS__);                      \
                                                                                \
            residueW = targetIWidth - targetOffsetCols;                         \
        }                                                                       \
                                                                                \
                                                                                \
        if (residueW > ptSourceSize->iWidth) {                                  \
            /* run 1 x 2 image paving */                                        \
            /*                                                                  \
             *       ...+=======+=.....                                         \
             *       ...||xxxxxx||                                              \
             *       ...+-------+-.....                                         \
             *       ...||xxxxxx||                                              \
             *       ...+=======+=.....                                         \
             */                                                                 \
            pPavFct->pav_1x2(   pSourceBase,                                    \
                                iSourceStride,                                  \
                                ptSourceSize,                                   \
                                pTargetBase + targetOffsetCols,                 \
                                iTargetStride,                                  \
                                ptSourceSize->iWidth,                           \
                                tilePairRows, ##__VA_ARGS__);                   \
                                                                                \
            targetOffsetCols += ptSourceSize->iWidth;                           \
            residueW = targetIWidth - targetOffsetCols;                         \
        }                                                                       \
        if (residueW > 0) {                                                     \
            /* run residual 1 x 2 image paving */                               \
            /*                                                                  \
             *       ...+=====                                                  \
             *       ...||xxxx                                                  \
             *       ...+-----                                                  \
             *       ...||xxxx                                                  \
             *       ...+=====                                                  \
             */                                                                 \
            pPavFct->pav_1x2(   pSourceBase, iSourceStride, ptSourceSize,       \
                                pTargetBase + targetOffsetCols,                 \
                                iTargetStride, residueW, tilePairRows           \
                                , ##__VA_ARGS__);                               \
        }                                                                       \
    }                                                                           \
                                                                                \
                                                                                \
    /* less than 2 complete vertical images */                                  \
    targetOffsetCols = 0;                                                       \
    residueH = targetIHeight - targetOffsetRows;                                \
    if (residueH >= ptSourceSize->iHeight) {                                    \
        residueW = targetIWidth;                                                \
                                                                                \
        /* can we handle horizontal pair of image ? */                          \
        if (tilePairCols >= 1) {                                                \
            /* run 2 x 1 image paving */                                        \
            /*                                                                  \
             *       +=======+=======+=======+=======+=======+=======++.....    \
             *       ||xxxxxx|xxxxxxx||      |       ||      |       ||         \
             *       +-------+-------+-------+-------+-------+-------+-.....    \
             *       ..................................................         \
             */                                                                 \
            pPavFct->pav_2x1(   pSourceBase,                                    \
                                iSourceStride,                                  \
                                ptSourceSize,                                   \
                                pTargetBase +                                   \
                                    targetOffsetRows * iTargetStride,           \
                                iTargetStride,                                  \
                                tilePairCols,                                   \
                                ptSourceSize->iHeight                           \
                                , ##__VA_ARGS__);                               \
                                                                                \
            targetOffsetCols = tilePairCols * (2 * sourceIWidth);               \
            residueW = targetIWidth - targetOffsetCols;                         \
        }                                                                       \
                                                                                \
        if (residueW > ptSourceSize->iWidth) {                                  \
            /* run single image paving */                                       \
            /*                                                                  \
             *       +=======+...                                               \
             *       ||xxxxx||...                                               \
             *       +-------+...                                               \
             *       ............                                               \
             */                                                                 \
            pPavFct->pav_1x1(pSourceBase,                                       \
                            iSourceStride,                                      \
                            pTargetBase + targetOffsetRows * iTargetStride +    \
                                targetOffsetCols,                               \
                            iTargetStride,                                      \
                            ptSourceSize,                                       \
                            ptSourceSize                                        \
                            , ##__VA_ARGS__);                                   \
                                                                                \
            targetOffsetCols += ptSourceSize->iWidth;                           \
            residueW = targetIWidth - targetOffsetCols;                         \
        }                                                                       \
                                                                                \
        if (residueW > 0) {                                                     \
            /* run residual single image paving (complete height, */            \
            /* but truncated width) */                                          \
            /*                      */                                          \
            /*       +=====...      */                                          \
            /*       ||xxxx...      */                                          \
            /*       +-----...      */                                          \
            /*       .........      */                                          \
            /*                      */                                          \
            arm_2d_size_t tail;                                                 \
            tail.iWidth = residueW;                                             \
            tail.iHeight = ptSourceSize->iHeight;                               \
                                                                                \
            pPavFct->pav_1x1(   pSourceBase,                                    \
                                iSourceStride,                                  \
                                pTargetBase + targetOffsetRows * iTargetStride +\
                                    targetOffsetCols,                           \
                                iTargetStride,                                  \
                                ptSourceSize,                                   \
                                &tail                                           \
                                , ##__VA_ARGS__);                               \
                                                                                \
        }                                                                       \
        targetOffsetRows += ptSourceSize->iHeight;                              \
    }                                                                           \
                                                                                \
    /* less than 1 complete vertical images */                                  \
    targetOffsetCols = 0;                                                       \
    residueH = ptTargetSize->iHeight - targetOffsetRows;                        \
    if (residueH) {                                                             \
        arm_2d_size_t tail;                                                     \
        tail.iWidth = ptSourceSize->iWidth;                                     \
        tail.iHeight = residueH;                                                \
                                                                                \
        if (tilePairCols >= 1) {                                                \
            /* run vertical truncated 2 x 1 image paving */                     \
            /*                                                                  \
             *       +=======+=======+=======+=======+=======+=======++.....    \
             *       ||xxxxxx|xxxxxxx||      |       ||      |       ||         \
             *       ..................................................         \
             */                                                                 \
            pPavFct->pav_2x1(   pSourceBase,                                    \
                                iSourceStride,                                  \
                                ptSourceSize,                                   \
                                pTargetBase + targetOffsetRows * iTargetStride, \
                                iTargetStride,                                  \
                                tilePairCols,                                   \
                                residueH                                        \
                                , ##__VA_ARGS__);                               \
                                                                                \
            targetOffsetCols = tilePairCols * (2 * sourceIWidth);               \
        }                                                                       \
                                                                                \
        residueW = targetIWidth - targetOffsetCols;                             \
        if (residueW > ptSourceSize->iWidth) {                                  \
            /* run vertical truncated single image copy */                      \
            /*                                                                  \
             *       +=======+...                                               \
             *       ||xxxxx||...                                               \
             *       ............                                               \
             */                                                                 \
                                                                                \
            pPavFct->pav_1x1(   pSourceBase,                                    \
                                iSourceStride,                                  \
                                pTargetBase + targetOffsetRows * iTargetStride +\
                                    targetOffsetCols,                           \
                                iTargetStride,                                  \
                                ptSourceSize,                                   \
                                &tail                                           \
                                , ##__VA_ARGS__);                               \
                                                                                \
            targetOffsetCols += ptSourceSize->iWidth;                           \
            residueW = targetIWidth - targetOffsetCols;                         \
        }                                                                       \
                                                                                \
        if (residueW > 0) {                                                     \
            /* run horizontal & vertical truncated single image copy */         \
            /*                                                                  \
             *       +======..                                                  \
             *       ||xxxxx...                                                 \
             *       ..........                                                 \
             */                                                                 \
            tail.iWidth = residueW;                                             \
                                                                                \
            pPavFct->pav_1x1(   pSourceBase,                                    \
                                iSourceStride,                                  \
                                pTargetBase + targetOffsetRows * iTargetStride +\
                                    targetOffsetCols,                           \
                                iTargetStride,                                  \
                                ptSourceSize,                                   \
                                &tail                                           \
                                , ##__VA_ARGS__);                               \
        }                                                                       \
    }                                                                           \
                                                                                \
}




#define __ARM_2D_PAVING_2x2(sz, SRC_OFFSET, DIR, SETUP_MIRROR, LOAD, ...)       \
{                                                                               \
    uint32_t            srcWidth = ptSourceSize->iWidth;                        \
    uint32_t            srcHeight = ptSourceSize->iHeight;                      \
    ARM_PIX_SCLTYP(sz) *__RESTRICT pSourceBaseCur;                              \
    ARM_PIX_SCLTYP(sz) *__RESTRICT pTargetBaseCur = pTargetBase;                \
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
            ARM_PIX_SCLTYP(sz) *__RESTRICT pDst = pTargetBaseCur;               \
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
                ARM_PIX_SCLTYP(sz)  *__RESTRICT pSource = pSourceBaseCur + SRC_OFFSET;     \
                ARM_PIX_SCLTYP(sz)  *__RESTRICT pDst00 = pDst;                             \
                ARM_PIX_SCLTYP(sz)  *__RESTRICT pDst01 = pDst00 + srcWidth;                \
                ARM_PIX_SCLTYP(sz)  *__RESTRICT pDst10 = pDst + srcHeight * iTargetStride; \
                ARM_PIX_SCLTYP(sz)  *__RESTRICT pDst11 = pDst10 + srcWidth;                \
                                                                                \
                SETUP_MIRROR(srcWidth);                                         \
                                                                                \
                /* duplicate current source line into 2x2 destinations tiles*/  \
                do {                                                            \
                    ARM_PIX_SCLTYP(sz)  in;                                     \
                                                                                \
                    in = LOAD(pSource, offset);                                 \
                    if (true,##__VA_ARGS__) {                                   \
                        PAVING_OP(pDst00++, in);                                \
                        PAVING_OP(pDst01++, in);                                \
                        PAVING_OP(pDst10++, in);                                \
                        PAVING_OP(pDst11++, in);                                \
                    } else {                                                    \
                        pDst00++;pDst01++;pDst10++;pDst11++;                    \
                    }                                                           \
                                                                                \
                } while (--dstColCnt);                                          \
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
    ARM_PIX_SCLTYP(sz)  *__RESTRICT pSourceBaseCur;                             \
    ARM_PIX_SCLTYP(sz)  *__RESTRICT pTargetBaseCur = pTargetBase;               \
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
            ARM_PIX_SCLTYP(sz)  *__RESTRICT pSource = pSourceBaseCur + SRC_OFFSET;         \
            ARM_PIX_SCLTYP(sz)  *__RESTRICT pDst = pTargetBaseCur;                         \
            ARM_PIX_SCLTYP(sz)  *__RESTRICT pDst00 = pDst;                                 \
            ARM_PIX_SCLTYP(sz)  *__RESTRICT pDst10 = pDst + srcHeight * iTargetStride;     \
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
                ARM_PIX_SCLTYP(sz)  in;                                         \
                                                                                \
                in = LOAD(pSource, offset);                                     \
                if (true,##__VA_ARGS__) {                                       \
                     PAVING_OP(pDst00++, in);                                   \
                     PAVING_OP(pDst10++, in);                                   \
                } else {                                                        \
                    pDst00 ++;                                                  \
                    pDst10 ++;                                                  \
                }                                                               \
            }                                                                   \
            while (--dstColCnt);                                                \
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
    ARM_PIX_SCLTYP(sz)  *__RESTRICT pSourceBaseCur;                             \
    ARM_PIX_SCLTYP(sz)  *__RESTRICT pTargetBaseCur = pTargetBase;               \
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
        ARM_PIX_SCLTYP(sz)  *__RESTRICT pDst = pTargetBaseCur;                  \
        ARM_PIX_SCLTYP(sz)   tilePairColsCnt = tilePairCols;                    \
                                                                                \
        do {                                                                    \
            int                 srcColCnt = srcWidth;                           \
            ARM_PIX_SCLTYP(sz) *__RESTRICT pSource = pSourceBaseCur+SRC_OFFSET; \
            ARM_PIX_SCLTYP(sz) *__RESTRICT pDst00 = pDst;                       \
            ARM_PIX_SCLTYP(sz) *__RESTRICT pDst01 = pDst00 + srcColCnt;         \
                                                                                \
            SETUP_MIRROR(srcWidth);                                             \
                                                                                \
            /* duplicate current source line into 4 destinations */             \
            do {                                                                \
                ARM_PIX_SCLTYP(sz)  in;                                         \
                                                                                \
                in = LOAD(pSource, offset);                                     \
                if (true,##__VA_ARGS__) {                                       \
                    PAVING_OP(pDst00++, in);                                    \
                    PAVING_OP(pDst01++, in);                                    \
                } else {                                                        \
                    pDst00 ++;                                                  \
                    pDst01 ++;                                                  \
                }                                                               \
                                                                                \
                srcColCnt -= 1;                                                 \
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
        ARM_PIX_SCLTYP(sz)  *__RESTRICT pDst = pTarget;                         \
        ARM_PIX_SCLTYP(sz)  *__RESTRICT pSrc = pSource;                         \
        uint32_t             srcWidth = ptSrcCopySize->iWidth;                  \
        uint32_t             dstWidth = ptDstCopySize->iWidth;                  \
                                                                                \
        SETUP_MIRROR(srcWidth);                                                 \
                                                                                \
        do {                                                                    \
            ARM_PIX_SCLTYP(sz)  in;                                             \
                                                                                \
            in = LOAD(pSrc, offset);                                            \
            if (true,##__VA_ARGS__) {                                           \
                PAVING_OP(pDst++,in);                                           \
            } else {                                                            \
                pDst++;                                                         \
            }                                                                   \
                                                                                \
            dstWidth -= 1;                                                      \
        }                                                                       \
        while ((int32_t) dstWidth > 0);                                         \
                                                                                \
        pSource += (iSourceStride * DIR);                                       \
        pTarget += iTargetStride;                                               \
    }                                                                           \
}


/*============================ TYPES =========================================*/

typedef void    (arm_2d_c8bit_paving_2x2) (const uint8_t *, int16_t, const arm_2d_size_t *,
                                            uint8_t *, int16_t, uint16_t, uint16_t);

typedef void    (arm_2d_c8bit_paving_1x2) (const uint8_t *, int16_t, const arm_2d_size_t *,
                                            uint8_t *, int16_t, uint32_t, uint16_t);

typedef void    (arm_2d_c8bit_paving_2x1) (const uint8_t *, int16_t,
                                             const arm_2d_size_t *, uint8_t *, int16_t, uint16_t, uint16_t);

typedef void    (arm_2d_c8bit_paving_1x1) (const uint8_t *, int16_t, uint8_t *, int16_t,
                                           const arm_2d_size_t *, const arm_2d_size_t *);


typedef struct arm_2d_c8bit_paving_fct_t {
    arm_2d_c8bit_paving_2x2    *pav_2x2;
    arm_2d_c8bit_paving_1x2    *pav_1x2;
    arm_2d_c8bit_paving_2x1    *pav_2x1;
    arm_2d_c8bit_paving_1x1    *pav_1x1;
} arm_2d_c8bit_paving_fct_t;


typedef void    (arm_2d_rgb16_paving_2x2) (const uint16_t *, int16_t, const arm_2d_size_t *,
                                            uint16_t *, int16_t, uint16_t, uint16_t);

typedef void    (arm_2d_rgb16_paving_1x2) (const uint16_t *, int16_t, const arm_2d_size_t *,
                                            uint16_t *, int16_t, uint32_t, uint16_t);

typedef void    (arm_2d_rgb16_paving_2x1) (const uint16_t *, int16_t,
                                             const arm_2d_size_t *, uint16_t *, int16_t, uint16_t, uint16_t);

typedef void    (arm_2d_rgb16_paving_1x1) (const uint16_t *, int16_t, uint16_t *, int16_t,
                                           const arm_2d_size_t *, const arm_2d_size_t *);


typedef struct arm_2d_rgb16_paving_fct_t {
    arm_2d_rgb16_paving_2x2    *pav_2x2;
    arm_2d_rgb16_paving_1x2    *pav_1x2;
    arm_2d_rgb16_paving_2x1    *pav_2x1;
    arm_2d_rgb16_paving_1x1    *pav_1x1;
} arm_2d_rgb16_paving_fct_t;

typedef void    (arm_2d_rgb32_paving_2x2) (const uint32_t *, int16_t, const arm_2d_size_t *,
                                            uint32_t *, int16_t, uint16_t, uint16_t);

typedef void    (arm_2d_rgb32_paving_1x2) (const uint32_t *, int16_t, const arm_2d_size_t *,
                                            uint32_t *, int16_t, uint32_t, uint16_t);

typedef void    (arm_2d_rgb32_paving_2x1) (const uint32_t *, int16_t,
                                             const arm_2d_size_t *, uint32_t *, int16_t, uint16_t, uint16_t);

typedef void    (arm_2d_rgb32_paving_1x1) (const uint32_t *, int16_t, uint32_t *, int16_t,
                                           const arm_2d_size_t *, const arm_2d_size_t *);


typedef struct arm_2d_rgb32_paving_fct_t {
    arm_2d_rgb32_paving_2x2    *pav_2x2;
    arm_2d_rgb32_paving_1x2    *pav_1x2;
    arm_2d_rgb32_paving_2x1    *pav_2x1;
    arm_2d_rgb32_paving_1x1    *pav_1x1;
} arm_2d_rgb32_paving_fct_t;



typedef void    (arm_2d_c8bit_cl_key_paving_2x2)(   const uint8_t * __RESTRICT, 
                                                    int16_t, 
                                                    const arm_2d_size_t * __RESTRICT,
                                                    uint8_t * __RESTRICT, 
                                                    int16_t, 
                                                    uint16_t, 
                                                    uint16_t, 
                                                    uint8_t);

typedef void    (arm_2d_c8bit_cl_key_paving_1x2) (  const uint8_t *__RESTRICT , 
                                                    int16_t, 
                                                    const arm_2d_size_t *__RESTRICT,
                                                    uint8_t *__RESTRICT, 
                                                    int16_t, 
                                                    uint32_t, 
                                                    uint16_t, 
                                                    uint8_t);

typedef void    (arm_2d_c8bit_cl_key_paving_2x1) (  const uint8_t *__RESTRICT, 
                                                    int16_t,
                                                    const arm_2d_size_t *__RESTRICT, 
                                                    uint8_t *__RESTRICT, 
                                                    int16_t, 
                                                    uint16_t, 
                                                    uint16_t, 
                                                    uint8_t);

typedef void    (arm_2d_c8bit_cl_key_paving_1x1) (  const uint8_t *__RESTRICT, 
                                                    int16_t, 
                                                    uint8_t *__RESTRICT, 
                                                    int16_t,
                                                    const arm_2d_size_t *__RESTRICT, 
                                                    const arm_2d_size_t *__RESTRICT, 
                                                    uint8_t);


typedef struct arm_2d_c8bit_cl_key_paving_fct_t {
    arm_2d_c8bit_cl_key_paving_2x2    *pav_2x2;
    arm_2d_c8bit_cl_key_paving_1x2    *pav_1x2;
    arm_2d_c8bit_cl_key_paving_2x1    *pav_2x1;
    arm_2d_c8bit_cl_key_paving_1x1    *pav_1x1;
} arm_2d_c8bit_cl_key_paving_fct_t;


typedef void    (arm_2d_rgb16_cl_key_paving_2x2)(   const uint16_t * __RESTRICT, 
                                                    int16_t, 
                                                    const arm_2d_size_t * __RESTRICT,
                                                    uint16_t * __RESTRICT, 
                                                    int16_t, 
                                                    uint16_t, 
                                                    uint16_t, 
                                                    uint16_t);

typedef void    (arm_2d_rgb16_cl_key_paving_1x2) (  const uint16_t *__RESTRICT , 
                                                    int16_t, 
                                                    const arm_2d_size_t *__RESTRICT,
                                                    uint16_t *__RESTRICT, 
                                                    int16_t, 
                                                    uint32_t, 
                                                    uint16_t, 
                                                    uint16_t);

typedef void    (arm_2d_rgb16_cl_key_paving_2x1) (  const uint16_t *__RESTRICT, 
                                                    int16_t,
                                                    const arm_2d_size_t *__RESTRICT, 
                                                    uint16_t *__RESTRICT, 
                                                    int16_t, 
                                                    uint16_t, 
                                                    uint16_t, 
                                                    uint16_t);

typedef void    (arm_2d_rgb16_cl_key_paving_1x1) (  const uint16_t *__RESTRICT, 
                                                    int16_t, 
                                                    uint16_t *__RESTRICT, 
                                                    int16_t,
                                                    const arm_2d_size_t *__RESTRICT, 
                                                    const arm_2d_size_t *__RESTRICT, 
                                                    uint16_t);


typedef struct arm_2d_rgb16_cl_key_paving_fct_t {
    arm_2d_rgb16_cl_key_paving_2x2    *pav_2x2;
    arm_2d_rgb16_cl_key_paving_1x2    *pav_1x2;
    arm_2d_rgb16_cl_key_paving_2x1    *pav_2x1;
    arm_2d_rgb16_cl_key_paving_1x1    *pav_1x1;
} arm_2d_rgb16_cl_key_paving_fct_t;



typedef void    (arm_2d_rgb32_cl_key_paving_2x2)(   const uint32_t *__RESTRICT, 
                                                    int16_t, 
                                                    const arm_2d_size_t *__RESTRICT,
                                                    uint32_t *__RESTRICT, 
                                                    int16_t, 
                                                    uint16_t, 
                                                    uint16_t, 
                                                    uint32_t);

typedef void    (arm_2d_rgb32_cl_key_paving_1x2) (  const uint32_t *__RESTRICT, 
                                                    int16_t, 
                                                    const arm_2d_size_t *__RESTRICT,
                                                    uint32_t *__RESTRICT, 
                                                    int16_t, 
                                                    uint32_t, 
                                                    uint16_t, 
                                                    uint32_t);

typedef void    (arm_2d_rgb32_cl_key_paving_2x1) (  const uint32_t *__RESTRICT, 
                                                    int16_t,
                                                    const arm_2d_size_t *__RESTRICT, 
                                                    uint32_t *__RESTRICT, 
                                                    int16_t, 
                                                    uint16_t, 
                                                    uint16_t, 
                                                    uint32_t);

typedef void    (arm_2d_rgb32_cl_key_paving_1x1) (  const uint32_t *__RESTRICT, 
                                                    int16_t, 
                                                    uint32_t *__RESTRICT, 
                                                    int16_t,
                                                    const arm_2d_size_t *__RESTRICT, 
                                                    const arm_2d_size_t *__RESTRICT, 
                                                    uint32_t);


typedef struct arm_2d_rgb32_cl_key_paving_fct_t {
    arm_2d_rgb32_cl_key_paving_2x2    *pav_2x2;
    arm_2d_rgb32_cl_key_paving_1x2    *pav_1x2;
    arm_2d_rgb32_cl_key_paving_2x1    *pav_2x1;
    arm_2d_rgb32_cl_key_paving_1x1    *pav_1x1;
} arm_2d_rgb32_cl_key_paving_fct_t;


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif


#ifdef   __cplusplus
}
#endif

#endif
