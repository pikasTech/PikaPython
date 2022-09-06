
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
 * Title:        __arm_2d_fill_colour_helium.inc
 * Description:  Helium code template for drawing pattern
 *
 * $Date:        20. Sept 2021
 * $Revision:    V.0.0.1
 *
 * -------------------------------------------------------------------- */

#ifndef __API_COLOUR
#   error You have to define __API_COLOUR before using this c template
#endif

#ifndef __API_ELT_SZ
#   error You have to define the __API_ELT_SZ before using this c template
#endif

#undef ____ARM_2D_FUNC
#undef ___ARM_2D_FUNC
#undef __ARM_2D_FUNC
#define ____ARM_2D_FUNC(__NAME, __COLOUR)  __arm_2d_impl_##__COLOUR##_##__NAME
#define ___ARM_2D_FUNC(__NAME, __COLOUR)   ____ARM_2D_FUNC(__NAME, __COLOUR)
#define __ARM_2D_FUNC(__NAME)   ___ARM_2D_FUNC(__NAME, __API_COLOUR)



__OVERRIDE_WEAK
void __ARM_2D_FUNC(colour_filling)( ARM_PIX_SCLTYP(__API_ELT_SZ) *__RESTRICT  pTarget,
                                    int16_t 	iTargetStride,
                                    arm_2d_size_t *__RESTRICT 	ptCopySize,
                                    ARM_PIX_SCLTYP(__API_ELT_SZ) 	Colour)
{
    ARM_PIX_VECTYP(__API_ELT_SZ)      vColor = ARM_CONNECT2(vdupq_n_u, __API_ELT_SZ)(Colour);

#ifdef USE_MVE_INTRINSICS
    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {
	    int32_t blkCnt = ptCopySize->iWidth;
        ARM_PIX_SCLTYP(__API_ELT_SZ) *pTargetCur = pTarget;

        while (blkCnt > 0) {
            mve_pred16_t    tailPred = ARM_CONNECT2(ARM_CONNECT2(vctp, __API_ELT_SZ), q)(blkCnt);

            vst1q_p(pTargetCur, vColor, tailPred);

            pTargetCur += ARM_PIX_VECELT(__API_ELT_SZ);
            blkCnt -= ARM_PIX_VECELT(__API_ELT_SZ);
        }

        pTarget += iTargetStride;
    }

#else
    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {
		int32_t blkCnt = ptCopySize->iWidth;
        ARM_PIX_SCLTYP(__API_ELT_SZ) *pTargetCur = pTarget;

		__asm volatile(
		    ".p2align 2                                              	\n"
		    "   wlstp." ARM_TO_STRING(__API_ELT_SZ) " lr, %[loopCnt], 1f    \n"
		    "2:                                                      	\n"
		    "   vstrb.u8                %[vColor], [%[pTarget]], #16 	\n"
		    "   letp                    lr, 2b                       	\n"
		    "1:                                                      	\n"

		    : [pTarget] "+r" (pTargetCur)
		    : [loopCnt] "r" (blkCnt), [vColor] "t" (vColor)
		    :"memory", "lr");

		pTarget += iTargetStride;
    }
#endif
}

#undef ____ARM_2D_FUNC
#undef ___ARM_2D_FUNC
#undef __ARM_2D_FUNC
#undef __API_COLOUR
#undef __API_ELT_SZ

