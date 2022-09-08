/* Copyright 2022 Sipeed Technology Co., Ltd. All Rights Reserved.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "stdlib.h"
#include "stdint.h"
#include "math.h"
#include "tm_port.h"

//ARM SIMD instruction acceleration
//https://www.keil.com/pack/doc/CMSIS/Core/html/group__intrinsic__SIMD__gr.html

//sum = SUM(Ai*Bi)
TM_INLINE void tm_dot_prod(mtype_t* sptr, mtype_t* kptr,uint32_t size, sumtype_t* result)
{
    uint32_t cnt;                  
    int32_t sum = 0;
    cnt = size/4;

#ifdef __UVISION_VERSION //MDK,KEIL
		__asm(
#else
   asm  volatile(
#endif
			"DOT_LOOP%=: \n"
			"subs   %[cnt], %[cnt], #1   \n"
			"ldr    r2, [%[sptr]], #4    \n" 
			"ldr    r3, [%[kptr]], #4    \n" 
			"ror    r4, r2, #8      \n" 
			"ror    r5, r3, #8      \n" 
			"sxtb16 r2, r2          \n" 
			"sxtb16 r3, r3          \n" 
			"sxtb16 r4, r4          \n" 
			"sxtb16 r5, r5          \n" 
			"smlad  %[sum], r2, r3, %[sum]  \n" 
			"smlad  %[sum], r4, r5, %[sum]  \n" 
			"cmp    %[cnt], #0      \n"
			"bgt DOT_LOOP%=         \n"
			:[sptr]"+r"(sptr),[kptr]"+r"(kptr),[sum]"+r"(sum),[cnt]"+r"(cnt)
			:
			:"r2", "r3", "r4", "r5");
    cnt = size % 4;
    while (cnt){
        sum += (int32_t) ((int16_t) *sptr++ * *kptr++);
        cnt--;
    }
		*result = sum;
    return;
}

TM_INLINE  void tm_dot_prod_pack2(mtype_t* sptr, mtype_t* kptr, uint32_t size, sumtype_t* result)
{ 
    uint32_t cnt;                  
    int32_t sum0 = 0;
	int32_t sum1 = 0;
    mtype_t* kptr0 = kptr;
    mtype_t* kptr1 = kptr+size;
    cnt = size/4;

#ifdef __UVISION_VERSION //MDK,KEIL
		__asm(
#else
   asm  volatile(
#endif
			"DOT_LOOP%=: \n"
			"subs   %[cnt], %[cnt], #1   \n"
			"ldr    r2, [%[sptr]], #4    \n" 
			"ldr    r3, [%[kptr0]], #4    \n" 
			"ldr    r6, [%[kptr1]], #4    \n" 
			"ror    r4, r2, #8      \n" 
			"ror    r5, r3, #8      \n" 
			"ror    r7, r6, #8      \n" 
			"sxtb16 r2, r2          \n" 
			"sxtb16 r3, r3          \n" 
			"sxtb16 r4, r4          \n" 
			"sxtb16 r5, r5          \n" 
			"sxtb16 r6, r6          \n"
			"sxtb16 r7, r7          \n"
			"smlad  %[sum0], r2, r3, %[sum0]  \n" 
			"smlad  %[sum0], r4, r5, %[sum0]  \n" 
			"smlad  %[sum1], r2, r6, %[sum1]  \n" 
			"smlad  %[sum1], r4, r7, %[sum1]  \n" 
			"cmp    %[cnt], #0      \n"
			"bgt DOT_LOOP%=         \n"
			:[sptr]"+r"(sptr),[kptr0]"+r"(kptr0),[kptr1]"+r"(kptr1),[sum0]"+r"(sum0),[sum1]"+r"(sum1),[cnt]"+r"(cnt)
			:
			:"r2", "r3", "r4", "r5", "r6", "r7");
			//r2:*sptr  r3:*kptr0  r4:(*sptr)>>8  r5:(*kptr0)>>8  r6:*kptr1  r7:(*kptr1)>>8
    cnt = size % 4;
    while (cnt){
        sum0 += (int32_t) ((int16_t) *sptr * *kptr0++);
		sum1 += (int32_t) ((int16_t) *sptr++ * *kptr1++);
        cnt--;
    }
	result[0] = sum0;
	result[1] = sum1;
    return;
}

TM_INLINE void tm_dot_prod_gap_3x3x1(mtype_t* sptr, mtype_t* kptr, uint32_t* k_oft, sumtype_t* result)
{
    *result = sptr[k_oft[0]]*kptr[0] + sptr[k_oft[1]]*kptr[1] + sptr[k_oft[2]]*kptr[2] + \
        sptr[k_oft[3]]*kptr[3] + sptr[k_oft[4]]*kptr[4] + sptr[k_oft[5]]*kptr[5] + \
        sptr[k_oft[6]]*kptr[6] + sptr[k_oft[7]]*kptr[7] + sptr[k_oft[8]]*kptr[8] ;
    return;                  
}

TM_INLINE void tm_dot_prod_3x3x1(mtype_t* sptr, mtype_t* kptr, sumtype_t* result)
{
    *result = sptr[0]*kptr[0] + sptr[1]*kptr[1] + sptr[2]*kptr[2] + \
        sptr[3]*kptr[3] + sptr[4]*kptr[4] + sptr[5]*kptr[5] + \
        sptr[6]*kptr[6] + sptr[7]*kptr[7] + sptr[8]*kptr[8] ;
    return;
}
