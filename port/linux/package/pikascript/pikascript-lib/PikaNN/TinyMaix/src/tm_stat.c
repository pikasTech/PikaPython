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
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#if TM_ENABLE_STAT
static const char* mdl_type_str[6] = {
    "int8", "int16", "fp32", "fp16", "fp8 1.4.3", "fp8 1.5.2",
};

static const char* tml_str_tbl[TML_MAXCNT] = {
    "Conv2D",   /*TML_CONV2D  = 0,*/
    "GAP",      /*TML_GAP     = 1,*/
    "FC",       /*TML_FC      = 2,*/
    "Softmax",  /*TML_SOFTMAX = 3,*/
    "Reshape",  /*TML_RESHAPE = 4,*/
    "DWConv2D", /*TML_DWCONV2D= 5,*/
};

static const int tml_headsize_tbl[TML_MAXCNT] = {
    sizeof(tml_conv2d_dw_t), sizeof(tml_gap_t),     sizeof(tml_fc_t),
    sizeof(tml_softmax_t),   sizeof(tml_reshape_t), sizeof(tml_conv2d_dw_t),
};

tm_err_t tm_stat(tm_mdlbin_t* b) {
    printf(
        "================================ model stat "
        "================================\n");
    printf("mdl_type=%d (%s))\n", b->mdl_type, mdl_type_str[b->mdl_type]);
    printf("out_deq=%d \n", b->out_deq);
    printf("input_cnt=%d, output_cnt=%d, layer_cnt=%d\n", b->input_cnt,
           b->output_cnt, b->layer_cnt);
    uint16_t* idim = b->in_dims;
    printf("input %ddims: (%d, %d, %d)\n", idim[0], idim[1], idim[2], idim[3]);
    uint16_t* odim = b->out_dims;
    printf("output %ddims: (%d, %d, %d)\n", odim[0], odim[1], odim[2], odim[3]);
    // printf("model param bin addr: 0x%x\n", (uint32_t)(b->layers_body));
    printf("main buf size %d; sub buf size %d\n", b->buf_size, b->sub_size);

    printf("//Note: PARAM is layer param size, include align padding\r\n\r\n");
    printf("Idx\tLayer\t         outshape\tinoft\toutoft\tPARAM\tMEMOUT OPS\n");
    printf("---\tInput    \t%3d,%3d,%3d\t-   \t0    \t0 \t%ld \t0\n", idim[1],
           idim[2], idim[3], idim[1] * idim[2] * idim[3] * sizeof(mtype_t));
    //      000  Input    -     224,224,3  0x40001234 0x40004000 100000 500000
    //      200000
    // printf("000  Input    -     %3d,%3d,%d  0x%08x   0x%08x     %6d %6d
    // %6d\n",)
    int sum_param = 0;
    int sum_ops = 0;
    uint8_t* layer_body = (uint8_t*)b->layers_body;
    int layer_i;
    for (layer_i = 0; layer_i < b->layer_cnt; layer_i++) {
        tml_head_t* h = (tml_head_t*)(layer_body);
        TM_DBG("body oft = %d\n", (uint32_t)h - (uint32_t)(b));
        TM_DBG(
            "type=%d, is_out=%d, size=%d, in_oft=%d, out_oft=%d, "
            "in_dims=[%d,%d,%d,%d], out_dims=[%d,%d,%d,%d], in_s=%.3f, "
            "in_zp=%d, out_s=%.3f, out_zp=%d\n",
            h->type, h->is_out, h->size, h->in_oft, h->out_oft, h->in_dims[0],
            h->in_dims[1], h->in_dims[2], h->in_dims[3], h->out_dims[0],
            h->out_dims[1], h->out_dims[2], h->out_dims[3], h->in_s,
            (int32_t)(h->in_zp), h->out_s, (int32_t)(h->out_zp));
        if (h->type < TML_MAXCNT) {
            int memout = h->out_dims[1] * h->out_dims[2] * h->out_dims[3];
            sum_param += (h->size - tml_headsize_tbl[h->type]);
            int ops = 0;
            switch (h->type) {
                case TML_CONV2D: {
                    tml_conv2d_dw_t* l = (tml_conv2d_dw_t*)(layer_body);
                    ops = memout * (l->kernel_w) * (l->kernel_h) *
                          (h->in_dims[3]);  // MAC as ops
                    TM_DBG(
                        "Conv2d: kw=%d, kh=%d, sw=%d, sh=%d, dw=%d, dh=%d, "
                        "act=%d, pad=[%d,%d,%d,%d], dmul=%d, ws_oft=%d, "
                        "w_oft=%d, b_oft=%d\n",
                        l->kernel_w, l->kernel_h, l->stride_w, l->stride_h,
                        l->dilation_w, l->dilation_h, l->act, l->pad[0],
                        l->pad[1], l->pad[2], l->pad[3], l->depth_mul,
                        l->ws_oft, l->w_oft, l->b_oft);
                    break;
                }
                case TML_GAP:
                    ops = (h->in_dims[1]) * (h->in_dims[2]) *
                          (h->in_dims[3]);  // SUM as ops
                    break;
                case TML_FC: {
                    tml_fc_t* l = (tml_fc_t*)(layer_body);
                    ops = (h->out_dims[3]) * (h->in_dims[3]);  // MAC as ops
                    TM_DBG("FC: ws_oft=%d, w_oft=%d, b_oft=%d\n", l->ws_oft,
                           l->w_oft, l->b_oft);
                    break;
                }
                case TML_SOFTMAX:
                    ops = 6 * (h->out_dims[3]);  // mixed
                    break;
                case TML_DWCONV2D: {
                    tml_conv2d_dw_t* l = (tml_conv2d_dw_t*)(layer_body);
                    ops = memout * (l->kernel_w) * (l->kernel_h) *
                          1;  // MAC as ops
                    TM_DBG(
                        "DWConv2d: kw=%d, kh=%d, sw=%d, sh=%d, dw=%d, dh=%d, "
                        "act=%d, pad=[%d,%d,%d,%d], dmul=%d, ws_oft=%d, "
                        "w_oft=%d, b_oft=%d\n",
                        l->kernel_w, l->kernel_h, l->stride_w, l->stride_h,
                        l->dilation_w, l->dilation_h, l->act, l->pad[0],
                        l->pad[1], l->pad[2], l->pad[3], l->depth_mul,
                        l->ws_oft, l->w_oft, l->b_oft);
                    break;
                }
                default:
                    ops = 0;
                    break;
            }
            sum_ops += ops;
            printf("%03d\t%s      \t%3d,%3d,%3d\t%d\t%d\t%d\t%ld\t", layer_i,
                   tml_str_tbl[h->type], h->out_dims[1], h->out_dims[2],
                   h->out_dims[3], h->in_oft, h->out_oft,
                   h->size - tml_headsize_tbl[h->type],
                   memout * sizeof(mtype_t));
            printf("%d\r\n", ops);
        } else {
            return TM_ERR_LAYERTYPE;
        }
        layer_body += (h->size);
    }
    printf("\r\nTotal param ~%.1f KB, OPS ~%.2f MOPS, buffer %.1f KB\r\n\r\n",
           sum_param / 1024.0, sum_ops / 1000000.0,
           (b->buf_size + b->sub_size) / 1024.0);
    return TM_OK;
}

#endif
