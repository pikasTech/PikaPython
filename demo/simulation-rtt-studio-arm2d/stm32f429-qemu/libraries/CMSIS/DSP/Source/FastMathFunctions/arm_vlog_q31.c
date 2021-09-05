/* ----------------------------------------------------------------------
 * Project:      CMSIS DSP Library
 * Title:        arm_vlog_q31
 * Description:  Q31 vector log
 *
 * $Date:        19 July 2021
 * $Revision:    V1.10.0
 *
 * Target Processor: Cortex-M and Cortex-A cores
 * -------------------------------------------------------------------- */
/*
 * Copyright (C) 2010-2021 ARM Limited or its affiliates. All rights reserved.
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

/*

This is a first attempt at implement a log in Q31
without using an interpolation table since there are
already too many tables in CMSIS-DSP.

But the accuracy is not that great for very small values ...

*/

#include "dsp/fast_math_functions.h"

#define LOG_Q31_ACCURACY 31

/* Bit to represent the normalization factor 
   It is Ceiling[Log2[LOG_Q31_ACCURACY]] of the previous value.
   The Log2 algorithm is assuming that the value x is 
   1 <= x < 2.

   But input value could be as small a 2^-LOG_Q31_ACCURACY
   which would give an integer part of -31.
*/
#define LOG_Q31_INTEGER_PART 5

/* 2.0 in Q30 or 0.5 in Q32 */
#define LOQ_Q31_THRESHOLD (1u << LOG_Q31_ACCURACY)
#define LOQ_Q31_Q32_HALF LOQ_Q31_THRESHOLD
#define LOQ_Q31_Q30_HALF (LOQ_Q31_Q32_HALF >> 2)


/* 1.0 / Log2[Exp[1]] in Q31 */
#define LOG_Q31_INVLOG2EXP 0x58b90bfbuL

/* Clay Turner algorithm */
static uint32_t arm_scalar_log_q31(uint32_t src)
{
   int i;
   
   int32_t c = __CLZ(src);
   int32_t normalization=0;

   //printf("x q31 = %08X\n",src);

   /* 0.5 in q32 */
   uint32_t inc = LOQ_Q31_Q32_HALF;

   /* Will compute y = log2(x) for 1 <= x < 2.0 */
   uint64_t x;

   /* q32 */
   uint32_t y=0;

   /* q5.58 */
   int64_t tmp;


   /* Normalize and convert to q30 format */
   x = src;
   if ((c-1) < 0)
   {
     x = x >> (1-c);
   }
   else
   {
     x = x << (c-1);
   }
   normalization = c;
   //printf("normalization = %d\n",normalization);
   //printf("x normalized q30 = %08llX\n",x);



   /* Compute the Log2. Result is in Q32 
      because we know 0 <= y < 1.0
   */
   for(i = 0; i < LOG_Q31_ACCURACY ; i++)
   {
      x = ((x*x) + LOQ_Q31_Q30_HALF) >> (LOG_Q31_ACCURACY - 1);
      if (x >= LOQ_Q31_THRESHOLD)
      {
         y += inc ;
         x = x >> 1;
      }
      inc = inc >> 1;
   }

   //printf("Log2 q32 = %08X\n",y);


   /* 
      Convert the Log2 to Log and apply normalization.
      We compute (y - normalisation) * (1 / Log2[e]).
      
   */

   /* q32 */
   tmp = y - ((int64_t)normalization << (LOG_Q31_ACCURACY + 1));
   //printf("Log2 q32 with normalization = %016llX\n",tmp);

   /* q27 * q31 -> q58 */
   tmp = (tmp>>LOG_Q31_INTEGER_PART) * (int64_t)LOG_Q31_INVLOG2EXP ;
   //printf("Log10 q58 = %016llX\n",tmp);

   /* q5.26 */
   y = tmp >> 32;
   //printf("Log10 q25 = %08X\n",y);
  

   return(y);

}

/**
  @ingroup groupFastMath
 */

/**
  @addtogroup vlog
  @{
 */

/**
  @brief         q31 vector of log values.
  @param[in]     pSrc       points to the input vector in q31
  @param[out]    pDst       points to the output vector q5.26
  @param[in]     blockSize  number of samples in each vector
  @return        none

 */
void arm_vlog_q31(
  const q31_t * pSrc,
        q31_t * pDst,
        uint32_t blockSize)
{
  uint32_t i;
  for(i=0;i < blockSize; i++)
  {
     pDst[i]=arm_scalar_log_q31(pSrc[i]);
  }
 
}

/**
  @} end of vlog group
 */
