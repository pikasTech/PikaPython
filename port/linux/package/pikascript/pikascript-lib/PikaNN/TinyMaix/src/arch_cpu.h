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
#include "float.h"
#include "tinymaix.h"

#if (TM_MDL_TYPE != TM_MDL_FP8_143) && (TM_MDL_TYPE != TM_MDL_FP8_152)
//sum = SUM(Ai*Bi)
TM_INLINE void tm_dot_prod(mtype_t* sptr, mtype_t* kptr,uint32_t size, sumtype_t* result)
{
    sumtype_t sum=0;
    uint32_t i = 0;
    uint32_t cnt = (size>>3)<<3;  //8
    for(; i+8-1 <cnt; ){
        sum += sptr[i]*kptr[i];i++;
        sum += sptr[i]*kptr[i];i++;
        sum += sptr[i]*kptr[i];i++;
        sum += sptr[i]*kptr[i];i++;
        sum += sptr[i]*kptr[i];i++;
        sum += sptr[i]*kptr[i];i++;
        sum += sptr[i]*kptr[i];i++;
        sum += sptr[i]*kptr[i];i++;
    }
    for(; i <size; i++){
        sum += sptr[i]*kptr[i]; 
    }
    *result = sum;
    return;
}

TM_INLINE  void tm_dot_prod_pack2(mtype_t* sptr, mtype_t* kptr, uint32_t size, sumtype_t* result)
{ 
    sumtype_t sum0 = 0;
    sumtype_t sum1 = 0;
    mtype_t* kptr0 = kptr;
    mtype_t* kptr1 = kptr+size;

    uint32_t i = 0;
    uint32_t cnt = (size>>3)<<3;  //8
    for(; i+8-1 <cnt; ){
        sum0 += sptr[i]*kptr0[i]; sum1 += sptr[i]*kptr1[i]; i++;
        sum0 += sptr[i]*kptr0[i]; sum1 += sptr[i]*kptr1[i]; i++;
        sum0 += sptr[i]*kptr0[i]; sum1 += sptr[i]*kptr1[i]; i++;
        sum0 += sptr[i]*kptr0[i]; sum1 += sptr[i]*kptr1[i]; i++;
        sum0 += sptr[i]*kptr0[i]; sum1 += sptr[i]*kptr1[i]; i++;
        sum0 += sptr[i]*kptr0[i]; sum1 += sptr[i]*kptr1[i]; i++;
        sum0 += sptr[i]*kptr0[i]; sum1 += sptr[i]*kptr1[i]; i++;
        sum0 += sptr[i]*kptr0[i]; sum1 += sptr[i]*kptr1[i]; i++;
    }
    for(; i <size; i++){
        sum0 += sptr[i]*kptr0[i]; 
        sum1 += sptr[i]*kptr1[i]; 
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



#else
/*************************** FP8 SIMULATION **********************************/
#define SUMSCALE 1.0

TM_INLINE void tm_dot_prod(mtype_t* sptr, mtype_t* kptr,uint32_t size, sumtype_t* result)
{
    sumtype_t sum=0;
    for(int i=0; i <size; i++){
        float _s = tm_fp8to32(sptr[i]);
        float _k = tm_fp8to32(kptr[i]);
        sum += _s*_k;
        //printf("%.3f*%.3f+",_s,_k);
    }
    //printf("\r\n");
    *result = sum;
    return;
}

TM_INLINE void l_postprocess_sum(sumtype_t sum, btype_t b, int act, mtype_t* outp, \
    sctype_t scale, sctype_t out_s, zptype_t out_zp)
{   //printf("sum=%.6f,", sum);
    sum += tm_fp8to32(b); //printf("%.6f,", sum);
    switch(act){    //activation func
    case TM_ACT_RELU:
        sum = sum>0?sum:0;
        break;
    case TM_ACT_RELU6:
        sum = sum>0?sum:0;
        sum = sum>6?6:sum;
        break;
    default:
        break;
    }
    //printf("%.6f,", sum);
    *outp = tm_fp32to8(sum);
    //printf("  %02x,%.6f\r\n", *outp, tm_fp8to32(*outp));
    return;
}

#endif


