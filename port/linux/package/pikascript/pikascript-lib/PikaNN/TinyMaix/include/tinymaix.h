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

#ifndef __TINYMAIX_H
#define __TINYMAIX_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define  TM_MDL_INT8    0
#define  TM_MDL_INT16   1
#define  TM_MDL_FP32    2
#define  TM_MDL_FP16    3
#define  TM_MDL_FP8_143 4 //experimental
#define  TM_MDL_FP8_152 5 //experimental
#include "tm_port.h"

/******************************* MARCO ************************************/
#define TM_MDL_MAGIC 'XIAM'     //mdl magic sign
#define TM_ALIGN_SIZE   (8)     //8 byte align
#define TM_ALIGN(addr)  ((((size_t)(addr))+(TM_ALIGN_SIZE-1))/TM_ALIGN_SIZE*TM_ALIGN_SIZE)
#define TM_MATP(mat,y,x,ch) ((mat)->data + ((y)*(mat)->w + (x))*(mat)->c + (ch))
                                //HWC
#if   TM_MDL_TYPE == TM_MDL_INT8
    typedef int8_t  mtype_t;    //mat data type
    typedef int8_t  wtype_t;    //weight data type
    typedef int32_t btype_t;    //bias data type
    typedef int32_t sumtype_t;  //sum data type 
    typedef int32_t zptype_t;   //zeropoint data type 
    #define UINT2INT_SHIFT (0)
#elif TM_MDL_TYPE == TM_MDL_INT16
    typedef int16_t mtype_t;    //mat data type
    typedef int16_t wtype_t;    //weight data type
    typedef int32_t btype_t;    //bias data type
    typedef int32_t sumtype_t;  //sum data type 
    typedef int32_t zptype_t;   //zeropoint data type
    #define UINT2INT_SHIFT (8)
#elif TM_MDL_TYPE == TM_MDL_FP32
    typedef float   mtype_t;    //mat data type
    typedef float   wtype_t;    //weight data type
    typedef float   btype_t;    //bias data type
    typedef float   sumtype_t;  //sum data type 
    typedef float   zptype_t;   //zeropoint data type 
#elif TM_MDL_TYPE == TM_MDL_FP16
    #if TM_ARCH != TM_ARCH_RV64V
        #error "only support RV64V's float16!"
    #endif
    #include <riscv_vector.h>
    typedef float16_t mtype_t;    //mat data type
    typedef float16_t wtype_t;    //weight data type
    typedef float16_t btype_t;    //bias data type
    typedef float16_t sumtype_t;  //sum data type 
    typedef float16_t zptype_t;   //zeropoint data type
#elif (TM_MDL_TYPE == TM_MDL_FP8_143) || (TM_MDL_TYPE == TM_MDL_FP8_152)
    #if TM_ARCH != TM_ARCH_CPU
        #error "only support CPU simulation now!"
    #endif
    typedef uint8_t mtype_t;    //mat data type
    typedef uint8_t wtype_t;    //weight data type
    typedef uint8_t btype_t;    //bias data type
    typedef float sumtype_t;    //sum data type 
    typedef float zptype_t;     //zeropoint data type
#else 
    #error "Not support this MDL_TYPE!"
#endif

#if TM_MDL_TYPE == TM_MDL_FP8_143
    #define TM_FP8_SCNT (1)
    #define TM_FP8_ECNT (4)
    #define TM_FP8_MCNT (3)
    #define TM_FP8_BIAS (9)
#elif TM_MDL_TYPE == TM_MDL_FP8_152
    #define TM_FP8_SCNT (1)
    #define TM_FP8_ECNT (5)
    #define TM_FP8_MCNT (2)
    #define TM_FP8_BIAS (15)
#endif

typedef float sctype_t;
#define TM_FASTSCALE_SHIFT (8)

/******************************* ENUM ************************************/
typedef enum{
    TM_OK = 0,
    TM_ERR= 1,
    TM_ERR_MAGIC     = 2,
    TM_ERR_UNSUPPORT = 3,
    TM_ERR_OOM       = 4,
    TM_ERR_LAYERTYPE = 5,
    TM_ERR_DIMS      = 6,
    TM_ERR_TODO      = 7,
    TM_ERR_MDLTYPE   = 8,
    TM_ERR_KSIZE     = 9,
}tm_err_t;

typedef enum{
    TML_CONV2D    = 0,
    TML_GAP       = 1,
    TML_FC        = 2,
    TML_SOFTMAX   = 3,
    TML_RESHAPE   = 4,
    TML_DWCONV2D  = 5,
    TML_MAXCNT    ,
}tm_layer_type_t;

typedef enum{
    TM_PAD_VALID  = 0,
    TM_PAD_SAME   = 1,
}tm_pad_type_t;

typedef enum{
    TM_ACT_NONE   = 0,
    TM_ACT_RELU   = 1,
    TM_ACT_RELU1  = 2,
    TM_ACT_RELU6  = 3,
    TM_ACT_TANH   = 4,
    TM_ACT_SIGNBIT= 5,
    TM_ACT_MAXCNT ,
}tm_act_type_t;


typedef enum {
    TMPP_NONE      = 0,
    TMPP_FP2INT    = 1,  //user own fp buf -> int input buf
    TMPP_UINT2INT  = 2,  //int8: cvt in place; int16: can't cvt in place
    TMPP_UINT2FP01 = 3,  // u8/255.0
    TMPP_UINT2FPN11= 4,  // (u8-128)/128 
    TMPP_UINT2DTYPE= 5,  //uint8 to fp16,fp8
    TMPP_MAXCNT,
}tm_pp_t;

/******************************* STRUCT ************************************/
//mdlbin in flash
typedef struct{
    uint32_t magic;         //"MAIX"
    uint8_t  mdl_type;      //0 int8, 1 int16, 2 fp32,
    uint8_t  out_deq;       //0 don't dequant out; 1 dequant out
    uint16_t input_cnt;     //only support 1 yet
    uint16_t output_cnt;    //only support 1 yet
    uint16_t layer_cnt;     
    uint32_t buf_size;      //main buf size for middle result
    uint32_t sub_size;      //sub buf size for middle result
    uint16_t in_dims[4];    //0:dims; 1:dim0; 2:dim1; 3:dim2
    uint16_t out_dims[4];
    uint8_t  reserve[28];   //reserve for future
    uint8_t  layers_body[0];//oft 64 here
}tm_mdlbin_t;

//mdl meta data in ram
typedef struct{
    tm_mdlbin_t* b;         //bin
    void*    cb;            //Layer callback
    uint8_t* buf;           //main buf addr
    uint8_t* subbuf;        //sub buf addr
    uint16_t main_alloc;    //is main buf alloc or static
    uint16_t layer_i;       //current layer index
    uint8_t* layer_body;    //current layer body addr
}tm_mdl_t;

//dims==3, hwc
//dims==2, 1wc
//dims==1, 11c
typedef struct{
    uint16_t dims;
    uint16_t h;
    uint16_t w;
    uint16_t c;
    union {
        mtype_t* data;
        float*   dataf;
    };
}tm_mat_t;

/******************************* LAYER STRUCT ************************************/
typedef struct{             //48byte
    uint16_t type;          //layer type
    uint16_t is_out;        //is output
    uint32_t size;          //8 byte align size for this layer
    uint32_t in_oft;        //input  oft in main buf
    uint32_t out_oft;       //output oft in main buf
    uint16_t in_dims[4];    //0:dims; 1:dim0; 2:dim1; 3:dim2
    uint16_t out_dims[4];
                            //following unit not used in fp32 mode
    sctype_t in_s;          //input scale, 
    zptype_t in_zp;         //input zeropoint
    sctype_t out_s;         //output scale
    zptype_t out_zp;        //output zeropoint
    //note: real = scale*(q-zeropoint)
}tml_head_t;

typedef struct{
    tml_head_t h;

    uint8_t  kernel_w;
    uint8_t  kernel_h;
    uint8_t  stride_w;
    uint8_t  stride_h;
    
    uint8_t  dilation_w;
    uint8_t  dilation_h;
    uint16_t  act;          //0 none, 1 relu, 2 relu1, 3 relu6, 4 tanh, 5 sign_bit
    
    uint8_t  pad[4];        //top,bottom,left,right

    uint32_t depth_mul;     //depth_multiplier: if conv2d,=0; else: >=1
    uint32_t reserve;       //for 8byte align
    
    uint32_t ws_oft;        //weight scale oft from this layer start 
                            //skip bias scale: bias_scale = weight_scale*in_scale
    uint32_t w_oft;         //weight oft from this layer start
    uint32_t b_oft;         //bias oft from this layer start 
    //note: bias[c] = bias[c] + (-out_zp)*sum(w[c*chi*maxk:(c+1)*chi*maxk])
    //      fused in advance (when convert model)
}tml_conv2d_dw_t;  //compatible with conv2d and dwconv2d

typedef struct{
    tml_head_t h;
}tml_gap_t;

typedef struct{
    tml_head_t h;

    uint32_t ws_oft;        //weight scale oft from this layer start 
    uint32_t w_oft;         //weight oft from this layer start
    uint32_t b_oft;         //bias oft from this layer start
    uint32_t reserve;       //for 8byte align
}tml_fc_t;

typedef struct{
    tml_head_t h;
}tml_softmax_t;

typedef struct{
    tml_head_t h;
}tml_reshape_t;

typedef struct{
    tml_head_t h;

    uint8_t  kernel_w;
    uint8_t  kernel_h;
    uint8_t  stride_w;
    uint8_t  stride_h;
    
    uint8_t  dilation_w;
    uint8_t  dilation_h;
    uint16_t  act;          //0 none, 1 relu, 2 relu1, 3 relu6, 4 tanh, 5 sign_bit
    
    uint8_t  pad[4];        //top,bottom,left,right


    
    uint32_t ws_oft;        //weight scale oft from this layer start 
                            //skip bias scale: bias_scale = weight_scale*in_scale
    uint32_t w_oft;         //weight oft from this layer start
    uint32_t b_oft;         //bias oft from this layer start 
    //note: bias[c] = bias[c] + (-out_zp)*sum(w[c*chi*maxk:(c+1)*chi*maxk])
    //      fused in advance (when convert model)
}tml_dwconv2d_t;



/******************************* TYPE ************************************/
typedef tm_err_t (*tml_stat_t)(tml_head_t* layer, tm_mat_t* in, tm_mat_t* out);
typedef tm_err_t (*tm_cb_t)(tm_mdl_t* mdl, tml_head_t* lh);


/******************************* GLOBAL VARIABLE ************************************/


/******************************* MODEL FUCNTION ************************************/
tm_err_t tm_load  (tm_mdl_t* mdl, const uint8_t* bin, uint8_t*buf, tm_cb_t cb, tm_mat_t* in);   //load model
void     tm_unload(tm_mdl_t* mdl);                                      //remove model
tm_err_t tm_preprocess(tm_mdl_t* mdl, tm_pp_t pp_type, tm_mat_t* in, tm_mat_t* out);            //preprocess input data
tm_err_t tm_run   (tm_mdl_t* mdl, tm_mat_t* in, tm_mat_t* out);         //run model


/******************************* LAYER FUCNTION ************************************/
tm_err_t tml_conv2d_dwconv2d(tm_mat_t* in, tm_mat_t* out, wtype_t* w, btype_t* b, \
    int kw, int kh, int sx, int sy, int dx, int dy, int act, \
    int pad_top, int pad_bottom, int pad_left, int pad_right, int dmul, \
    sctype_t* ws, sctype_t in_s, zptype_t in_zp, sctype_t out_s, zptype_t out_zp);
tm_err_t tml_gap(tm_mat_t* in, tm_mat_t* out, sctype_t in_s, zptype_t in_zp, sctype_t out_s, zptype_t out_zp);
tm_err_t tml_fc(tm_mat_t* in, tm_mat_t* out,  wtype_t* w, btype_t* b, \
    sctype_t* ws, sctype_t in_s, zptype_t in_zp, sctype_t out_s, zptype_t out_zp);
tm_err_t tml_softmax(tm_mat_t* in, tm_mat_t* out, sctype_t in_s, zptype_t in_zp, sctype_t out_s, zptype_t out_zp);
tm_err_t tml_reshape(tm_mat_t* in, tm_mat_t* out, sctype_t in_s, zptype_t in_zp, sctype_t out_s, zptype_t out_zp);


/******************************* STAT FUCNTION ************************************/
#if TM_ENABLE_STAT
tm_err_t tm_stat(tm_mdlbin_t* mdl);                    //stat model
#endif

/******************************* UTILS FUCNTION ************************************/
uint8_t __attribute__((weak)) tm_fp32to8(float fp32);
float __attribute__((weak)) tm_fp8to32(uint8_t fp8);


/******************************* UTILS  ************************************/
#define TML_GET_INPUT(mdl,lh)   ((mtype_t*)((mdl)->buf + (lh)->in_oft))
#define TML_GET_OUTPUT(mdl,lh)  ((mtype_t*)((mdl)->buf + (lh)->out_oft))
#if (TM_MDL_TYPE == TM_MDL_INT8)||(TM_MDL_TYPE == TM_MDL_INT16)
    #define TML_DEQUANT(lh, x)       (((sumtype_t)(x)-((lh)->out_zp))*((lh)->out_s))
#elif (TM_MDL_TYPE == TM_MDL_FP8_143) || (TM_MDL_TYPE == TM_MDL_FP8_152)
    #define TML_DEQUANT(lh, x)       (tm_fp8to32(x))
#else   //FP32,FP16
    #define TML_DEQUANT(lh, x)       ((float)(x))
#endif

#endif 
