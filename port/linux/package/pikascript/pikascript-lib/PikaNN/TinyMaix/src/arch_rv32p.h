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

//RV32 P-extend acceleration
//https://occ.t-head.cn/community/download?id=3916180248689188864
//https://occ.t-head.cn/vendor/cpu/index?id=3900588052540035072&key=download#sticky   //pdf
//https://github.com/T-head-Semi
//We use T-head Xuantie E907 as example, it have its own instructions!!!
#define ENABLE_THEAD_EXT 1

#if TM_MDL_TYPE == TM_MDL_INT8
#if ENABLE_THEAD_EXT
TM_INLINE void tm_dot_prod(mtype_t* sptr, mtype_t* kptr,uint32_t size, sumtype_t* result)
{
    int32_t sum;
    asm  volatile(
    "mv     t6,%[size]          \n"
    "mv     t0,%[sptr]          \n"       
    "mv     t1,%[kptr]          \n"       
    "andi	a7,t6,15	    	\n"
    "srli	t6,t6,0x4	    	\n"
    "li	%[sum],0				\n"
    "beqz	t6,LOOPEND%=\n"
    "LOOP%=:\n"
    "lwia	t2,(t0),4,0		\n"
    "lwia	t3,(t1),4,0		\n"
    "lwia	t4,(t0),4,0		\n"
    "lwia	t5,(t1),4,0		\n"
    "smaqa	%[sum],t2,t3		\n"
    "lwia	t2,(t0),4,0		\n"
    "lwia	t3,(t1),4,0		\n"
    "smaqa	%[sum],t4,t5		\n"
    "lwia	t4,(t0),4,0		\n"
    "lwia	t5,(t1),4,0		\n"
    "smaqa	%[sum],t2,t3		\n"
    "addi	t6,t6,-1		\n"
    "smaqa	%[sum],t4,t5		\n"
    "bnez	t6,LOOP%=		\n"
    "LOOPEND%=:"
    "beqz	a7,OUT%=	\n"
    "REST%=:\n"
    "lbia	t2,(t0),1,0		\n"
    "lbia	t3,(t1),1,0		\n"
    "addi	a7,a7,-1		\n"
    "smaqa	%[sum],t2,t3		\n"
    "bnez	a7,REST%=	\n"
    "OUT%=:\n"
    :[sum]"=r"(sum)
    :[sptr]"r"(sptr),[kptr]"r"(kptr),[size]"r"(size)
    :"t0", "t1", "t2", "t3", "t4", "t5", "t6", "a7"
    );
    //t0,t1: sptr, kptr
    //t2,t3: *sptr0, *kptr0
    //t4,t5: *sptr1, *kptr1
    //t6:    size -> size>>4
    //a7:    size%15
    *result = sum;
    return;
}

TM_INLINE  void tm_dot_prod_pack2(mtype_t* sptr, mtype_t* kptr, uint32_t size, sumtype_t* result)
{ 
    mtype_t* kptra = kptr;
    mtype_t* kptrb = kptr+size;

    int32_t sum0, sum1;
    asm  volatile(
    "mv     t6,%[size]          \n"
    "mv     t0,%[sptr]          \n"       
    "mv     t1,%[kptra]         \n"       
    "mv     a4,%[kptrb]         \n"  
    "andi	a7,t6,15	    	\n"
    "srli	t6,t6,0x4	    	\n"
    "li	%[sum0],0				\n"
    "li	%[sum1],0				\n"
    "beqz	t6,LOOPEND%=\n"
    "LOOP%=:\n"
    "lwia	t2,(t0),4,0		\n"
    "lwia	t3,(t1),4,0		\n"
    "lwia	a5,(a4),4,0		\n"
    "lwia	t4,(t0),4,0		\n"
    "lwia	t5,(t1),4,0		\n"
    "lwia	a6,(a4),4,0		\n"
    "smaqa	%[sum0],t2,t3		\n"
    "smaqa	%[sum1],t2,a5		\n"
    "lwia	t2,(t0),4,0		\n"
    "lwia	t3,(t1),4,0		\n"
    "lwia	a5,(a4),4,0		\n"
    "smaqa	%[sum0],t4,t5		\n"
    "smaqa	%[sum1],t4,a6		\n"
    "lwia	t4,(t0),4,0		\n"
    "lwia	t5,(t1),4,0		\n"
    "lwia	a6,(a4),4,0		\n"
    "smaqa	%[sum0],t2,t3		\n"
    "smaqa	%[sum1],t2,a5		\n"
    "addi	t6,t6,-1		\n"
    "smaqa	%[sum0],t4,t5		\n"
    "smaqa	%[sum1],t4,a6		\n"
    "bnez	t6,LOOP%=		\n"
    "LOOPEND%=:"
    "beqz	a7,OUT%=	\n"
    "REST%=:\n"
    "lbia	t2,(t0),1,0		\n"
    "lbia	t3,(t1),1,0		\n"
    "lbia	a5,(a4),1,0		\n"
    "addi	a7,a7,-1		\n"
    "smaqa	%[sum0],t2,t3		\n"
    "smaqa	%[sum1],t2,a5		\n"
    "bnez	a7,REST%=	\n"
    "OUT%=:\n"
    :[sum0]"=r"(sum0),[sum1]"=r"(sum1)
    :[sptr]"r"(sptr),[kptra]"r"(kptra),[kptrb]"r"(kptrb),[size]"r"(size)
    :"t0", "t1", "t2", "t3", "t4", "t5", "t6", "a7", "a4", "a5", "a6"
    );
    //t0,t1,s9: sptr, kptra, kptrb
    //t2,t3,s10: *sptr0, *kptra0, *kptrb0,
    //t4,t5,s11: *sptr1, *kptra1, *kptrb1
    //t6:    size -> size>>4
    //a7:    size%15

    result[0] = sum0;
    result[1] = sum1;
    return;
}
 
TM_INLINE void tm_dot_prod_3x3x1(mtype_t* sptr, mtype_t* kptr, sumtype_t* result)
{
    *result = sptr[0]*kptr[0] + sptr[1]*kptr[1] + sptr[2]*kptr[2] + \
        sptr[3]*kptr[3] + sptr[4]*kptr[4] + sptr[5]*kptr[5] + \
        sptr[6]*kptr[6] + sptr[7]*kptr[7] + sptr[8]*kptr[8] ;
    return;
}
 
TM_INLINE void tm_dot_prod_gap(mtype_t* sptr, mtype_t* kptr, uint32_t* k_oft, uint32_t size, sumtype_t* result)
{
    sumtype_t sum=0;
    uint32_t i = 0;
    uint32_t cnt = (size>>3)<<3;  //8
    for(; i+8-1 <cnt;){
        sum += sptr[k_oft[i]]*kptr[i]; i++;
        sum += sptr[k_oft[i]]*kptr[i]; i++;
        sum += sptr[k_oft[i]]*kptr[i]; i++;
        sum += sptr[k_oft[i]]*kptr[i]; i++;
        sum += sptr[k_oft[i]]*kptr[i]; i++;
        sum += sptr[k_oft[i]]*kptr[i]; i++;
        sum += sptr[k_oft[i]]*kptr[i]; i++;
        sum += sptr[k_oft[i]]*kptr[i]; i++;
    }
    for(; i <size; i++){
        sum += sptr[k_oft[i]]*kptr[i]; 
    }
    *result = sum;
    return;
}


TM_INLINE void tm_dot_prod_gap_3x3x1(mtype_t* sptr, mtype_t* kptr, uint32_t* k_oft, sumtype_t* result)
{
    *result = sptr[k_oft[0]]*kptr[0] + sptr[k_oft[1]]*kptr[1] + sptr[k_oft[2]]*kptr[2] + \
        sptr[k_oft[3]]*kptr[3] + sptr[k_oft[4]]*kptr[4] + sptr[k_oft[5]]*kptr[5] + \
        sptr[k_oft[6]]*kptr[6] + sptr[k_oft[7]]*kptr[7] + sptr[k_oft[8]]*kptr[8] ;
    return;                  
}

#else
#error "standard RV32P is not implement yet!"
#endif

#else
#error "RV32P opt for FP32 in not implement yet!"
#endif