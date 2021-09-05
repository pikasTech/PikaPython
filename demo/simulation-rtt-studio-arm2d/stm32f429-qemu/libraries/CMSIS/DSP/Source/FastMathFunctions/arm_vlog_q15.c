/* ----------------------------------------------------------------------
 * Project:      CMSIS DSP Library
 * Title:        arm_vlog_q15
 * Description:  Q15 vector log
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

This is a first attempt at implement a log in Q15
without using an interpolation table since there are
already too many tables in CMSIS-DSP.

But the accuracy is not that great for very small values ...

*/

#include "dsp/fast_math_functions.h"

#define LOG_Q15_ACCURACY 15

/* Bit to represent the normalization factor 
   It is Ceiling[Log2[LOG_Q15_ACCURACY]] of the previous value.
   The Log2 algorithm is assuming that the value x is 
   1 <= x < 2.

   But input value could be as small a 2^-LOG_Q15_ACCURACY
   which would give an integer part of -15.
*/
#define LOG_Q15_INTEGER_PART 4

/* 2.0 in Q14 or 0.5 in Q16 */
#define LOQ_Q15_THRESHOLD (1u << LOG_Q15_ACCURACY)
#define LOQ_Q15_Q16_HALF LOQ_Q15_THRESHOLD
#define LOQ_Q15_Q14_HALF (LOQ_Q15_Q16_HALF >> 2)


/* 1.0 / Log2[Exp[1]] in Q15 */
#define LOG_Q15_INVLOG2EXP 0x58b9u

/* Clay Turner algorithm */
static uint16_t arm_scalar_log_q15(uint16_t src)
{
   int i;

   int32_t c = __CLZ(src)-16;
   int16_t normalization=0;

   //printf("x q15 = %08X, c = %d\n",src,c);

   /* 0.5 in q16 */
   uint16_t inc = LOQ_Q15_Q16_HALF;

   /* Will compute y = log2(x) for 1 <= x < 2.0 */
   uint32_t x;

   /* q16 */
   uint16_t y=0;

   /* q4.24 */
   int32_t tmp;


   /* Normalize and convert to q314 format */
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
   //printf("x normalized q14 = %08X\n",x);



   /* Compute the Log2. Result is in Q16 
      because we know 0 <= y < 1.0
   */
   for(i = 0; i < LOG_Q15_ACCURACY ; i++)
   {
      x = ((x*x) + LOQ_Q15_Q14_HALF) >> (LOG_Q15_ACCURACY - 1);
      if (x >= LOQ_Q15_THRESHOLD)
      {
         y += inc ;
         x = x >> 1;
      }
      inc = inc >> 1;
   }

   //printf("Log2 q16 = %04X\n",y);


   /* 
      Convert the Log2 to Log and apply normalization.
      We compute (y - normalisation) * (1 / Log2[e]).
      
   */

   /* q16 */
   tmp = y - ((int32_t)normalization << (LOG_Q15_ACCURACY + 1));
   //printf("Log2 q16 with normalization = %08X\n",tmp);

   /* q12 * q15 -> q27 */
   tmp = (tmp>>LOG_Q15_INTEGER_PART) * (int32_t)LOG_Q15_INVLOG2EXP ;
   //printf("Log10 q27 = %08X\n",tmp);

   /* q4.11 */
   y = tmp >> 16;
   //printf("Log10 q11 = %04X\n",y);
  

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
  @brief         q15 vector of log values.
  @param[in]     pSrc       points to the input vector in q15
  @param[out]    pDst       points to the output vector in q4.11
  @param[in]     blockSize  number of samples in each vector
  @return        none

 */
void arm_vlog_q15(
  const q15_t * pSrc,
        q15_t * pDst,
        uint32_t blockSize)
{
  uint32_t i;
  for(i=0;i < blockSize; i++)
  {
     pDst[i]=arm_scalar_log_q15(pSrc[i]);
  }
}

/**
  @} end of vlog group
 */
