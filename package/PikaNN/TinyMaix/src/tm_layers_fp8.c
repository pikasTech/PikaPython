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
#include "tinymaix.h"

//It is experimental test for fp8 model, buggy yet, only work for simple mnist model
#if (TM_MDL_TYPE == TM_MDL_FP8_143) || (TM_MDL_TYPE == TM_MDL_FP8_152) 
#if TM_ARCH != TM_ARCH_CPU
    #error "only support CPU simulation now!"
#endif
#include "arch_cpu.h"
#include "float.h"

uint8_t tm_fp32to8(float fp32) 
{
    uint32_t data = *(uint32_t*)&fp32;
    int32_t fp32_s = data>>31;
    int32_t fp32_e = (data>>23)&0x0ff;
    int32_t fp32_m = (data&0x07fffff);
    //printf("fp32 %.6f, s.e.m=%d, %d, %d",fp32, fp32_s, fp32_e, fp32_m);
    uint8_t fp8_s = fp32_s;
    int8_t fp8_e = fp32_e-127+TM_FP8_BIAS;
    uint8_t fp8_m = ((fp32_m>>(22-TM_FP8_MCNT))+1)>>1;
    if(fp8_m>=(1<<TM_FP8_MCNT)) {
        fp8_m = 0;
        fp8_e += 1;
    }
    if(fp8_e > (1<<TM_FP8_ECNT)-1) fp8_e = (1<<TM_FP8_ECNT)-1;
    else if(fp8_e < 0) fp8_e = 0;
    uint8_t fp8 = (fp8_s<<7)|(fp8_e<<TM_FP8_MCNT)|fp8_m;
    //printf("fp8 0x%x, s.e.m=%d, %d, %d",fp8, fp8_s, fp8_e, fp8_m);
    return fp8;
}

float tm_fp8to32(uint8_t fp8) 
{
    uint8_t fp8_s = fp8>>7;
    uint8_t fp8_e = (fp8&0x7f)>>TM_FP8_MCNT;
    uint8_t fp8_m = fp8&((1<<TM_FP8_MCNT)-1);
    //printf("fp8 0x02x, s.e.m=%d, %d, %d",fp8, fp8_s, fp8_e, fp8_m);
    uint32_t fp32_s = fp8_s;
    uint32_t fp32_e = (uint32_t)((int32_t)fp8_e-TM_FP8_BIAS+127);
    uint32_t fp32_m = fp8_m<<(23-TM_FP8_MCNT);

    uint32_t tmp = (fp32_s<<31)|(fp32_e<<23)|fp32_m;
    float fp32 = *(float*)&tmp;
    return fp32;
}

//preprocess data input for fp8 simulation //no weak attr
tm_err_t  tm_preprocess(tm_mdl_t* mdl, tm_pp_t pp_type, tm_mat_t* in, tm_mat_t* out)
{
    tml_head_t* l0h = (tml_head_t*)mdl->b->layers_body;
    sctype_t in_s = l0h->in_s;
    zptype_t in_zp= l0h->in_zp;
    int in_size = in->h*in->w*in->c;
    float tmp = 0;
    switch(pp_type){
    case TMPP_UINT2FP01:
        for(int i=0; i<in_size; i++) {
            tmp= (((uint8_t*)(in->data))[i])/255.0;
            out->data[i] = tm_fp32to8(tmp);
        }
        break;
    case TMPP_UINT2FPN11:
        for(int i=0; i<in_size; i++){
            tmp= ((((uint8_t*)(in->data))[i])-128)/128.0;
            out->data[i] = tm_fp32to8(tmp);
        }
        break;
    default:    //don't do anything
        out->data = in->data;
        break;
    }
    return TM_OK;
}

tm_err_t tml_gap(tm_mat_t* in, tm_mat_t* out, sctype_t in_s, zptype_t in_zp, sctype_t out_s, zptype_t out_zp)
{   TM_DBGT_INIT();
    mtype_t* data;
    for(int c=0; c <out->c; c++){
        sumtype_t sum = 0;
        data = in->data + c;
        for(int y=0; y <in->h; y++){
            for(int x=0; x <in->w; x++){
                sum  += (tm_fp8to32(data));
                data += out->c;
            }
        }
        out->data[c] = tm_fp32to8(sum/((in->h)*(in->w)));
    }
    return TM_OK;
}

tm_err_t tml_fc(tm_mat_t* in, tm_mat_t* out,  wtype_t* w, btype_t* b, \
    sctype_t* ws, sctype_t in_s, zptype_t in_zp, sctype_t out_s, zptype_t out_zp)
{   TM_DBGT_INIT();
    mtype_t* data = in->data;

    for(int c=0; c <out->c; c++){
        sumtype_t sum = 0;
        tm_dot_prod(data, w+c*in->c, in->c, &sum);
        //printf("sum=%.6f\r\n", sum);
        sum += tm_fp8to32(b[c]);   
        //printf("sum+b=%.6f\r\n", sum);
        out->data[c] = tm_fp32to8(sum);
        //printf("sum=0x%02x, %.6f\r\n", out->data[c], tm_fp8to32(out->data[c]));
    }
    return TM_OK;
}

tm_err_t tml_softmax(tm_mat_t* in, tm_mat_t* out, sctype_t in_s, zptype_t in_zp, sctype_t out_s, zptype_t out_zp)
{   TM_DBGT_INIT(); //note we have float size output buf even in INT8/INT16 mode
    mtype_t* din = in->data;
    float*  dout = (float*)(out->data); 
    float   dmax =  -FLT_MAX;
    for(int c=0; c <in->c; c++){
        dout[c] = tm_fp8to32(din[c]);
        if(dout[c] > dmax) dmax = dout[c];
    }
    float sum = 0;
    for(int c=0; c <in->c; c++){
        dout[c] -= dmax;
        dout[c] = (float)exp(dout[c]);
        sum     += dout[c];
        //dout[c] -= 0.000000001;  //prevent 1.0 value (cause 256 overflow)
    }
    for(int c=0; c <in->c; c++){  //int8/int16 <= fp32, so it is ok
        out->data[c] = tm_fp32to8(dout[c]/sum);
    }
    return TM_OK;
}


#endif
