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
#pragma GCC diagnostic ignored "-Wmultichar"
#pragma GCC diagnostic ignored "-Wpointer-sign"
// load model
// mdl: model handle; bin: model bin buf; buf: main buf for middle output; cb:
// layer callback; in: return input mat, include buf addr; //you can ignore it
// if use static buf
tm_err_t __attribute__((weak)) tm_load(tm_mdl_t* mdl,
                                       const uint8_t* bin,
                                       uint8_t* buf,
                                       tm_cb_t cb,
                                       tm_mat_t* in) {
    tm_mdlbin_t* mdl_bin = (tm_mdlbin_t*)bin;
    if (mdl_bin->magic != TM_MDL_MAGIC)
        return TM_ERR_MAGIC;  // FIXME: big-endian not compatible
    if (mdl_bin->mdl_type != TM_MDL_TYPE)
        return TM_ERR_MDLTYPE;
    mdl->b = mdl_bin;
    mdl->cb = (void*)cb;
    if (buf == NULL) {
        mdl->buf = (uint8_t*)tm_malloc(mdl->b->buf_size);
        if (mdl->buf == NULL)
            return TM_ERR_OOM;
        mdl->main_alloc = 1;
    } else {
        mdl->buf = buf;
        mdl->main_alloc = 0;
    }
    if (mdl->b->sub_size > 0) {
        mdl->subbuf = (uint8_t*)tm_malloc(mdl->b->sub_size);
        if (mdl->subbuf == NULL)
            return TM_ERR_OOM;
    } else
        mdl->subbuf = NULL;
    mdl->layer_i = 0;
    mdl->layer_body = mdl->b->layers_body;
    memcpy((void*)in, (void*)mdl->b->in_dims, sizeof(tm_mat_t));
    in->data = (mtype_t*)mdl->buf;  // input at 0 oft
    return TM_OK;
}

// remove model
void __attribute__((weak)) tm_unload(tm_mdl_t* mdl) {
    if (mdl->main_alloc)
        tm_free(mdl->buf);
    if (mdl->subbuf)
        tm_free(mdl->subbuf);
    return;
}

// preprocess data input
tm_err_t __attribute__((weak))
tm_preprocess(tm_mdl_t* mdl, tm_pp_t pp_type, tm_mat_t* in, tm_mat_t* out) {
    tml_head_t* l0h = (tml_head_t*)mdl->b->layers_body;
    sctype_t in_s = l0h->in_s;
    zptype_t in_zp = l0h->in_zp;
    int in_size = in->h * in->w * in->c;
    switch (pp_type) {
#if (TM_MDL_TYPE == TM_MDL_INT8) || (TM_MDL_TYPE == TM_MDL_INT16)
        case TMPP_FP2INT:
            for (int i = 0; i < in_size; i++)
                out->data[i] = (mtype_t)(in->dataf[i] / in_s + in_zp);
            break;
        case TMPP_UINT2INT:
            for (int i = 0; i < in_size; i++)
                out->data[i] = ((mtype_t)(((uint8_t*)(in->data))[i] - 128))
                               << UINT2INT_SHIFT;
            break;
#else
        case TMPP_UINT2FP01:
            for (int i = 0; i < in_size; i++)
                out->data[i] = (((uint8_t*)(in->data))[i]) / 255.0;
            break;
        case TMPP_UINT2FPN11:
            for (int i = 0; i < in_size; i++)
                out->data[i] = ((((uint8_t*)(in->data))[i]) - 128) / 128.0;
            break;
#endif
        default:  // don't do anything
            out->data = in->data;
            break;
    }
    return TM_OK;
}

// run model
// mdl: model handle; in: input mat; out: output mat
tm_err_t __attribute__((weak))
tm_run(tm_mdl_t* mdl, tm_mat_t* in, tm_mat_t* out) {
    tm_mat_t _in, _out;
    tm_err_t res = TM_OK;
    int out_idx = 0;
    memcpy((void*)&_in, (void*)in, sizeof(tm_mat_t));
    mdl->layer_body = mdl->b->layers_body;
    for (mdl->layer_i = 0; mdl->layer_i < mdl->b->layer_cnt; mdl->layer_i++) {
        tml_head_t* h = (tml_head_t*)(mdl->layer_body);
        if (mdl->layer_i > 0) {
            _in.data = mdl->buf + h->in_oft;
            memcpy((void*)&_in, (void*)(h->in_dims), sizeof(uint16_t) * 4);
        }
        _out.data = mdl->buf + h->out_oft;
        memcpy((void*)&_out, (void*)(h->out_dims), sizeof(uint16_t) * 4);
        switch (h->type) {
            case TML_CONV2D:
            case TML_DWCONV2D: {
                tml_conv2d_dw_t* l = (tml_conv2d_dw_t*)(mdl->layer_body);
                res = tml_conv2d_dwconv2d(
                    &_in, &_out, (wtype_t*)(mdl->layer_body + l->w_oft),
                    (btype_t*)(mdl->layer_body + l->b_oft), l->kernel_w,
                    l->kernel_h, l->stride_w, l->stride_h, l->dilation_w,
                    l->dilation_h, l->act, l->pad[0], l->pad[1], l->pad[2],
                    l->pad[3], l->depth_mul,
                    (sctype_t*)(mdl->layer_body + l->ws_oft), h->in_s, h->in_zp,
                    h->out_s, h->out_zp);
                break;
            }
            case TML_GAP: {
                tml_gap_t* l __attribute__((unused)) =
                    (tml_gap_t*)(mdl->layer_body);
                res = tml_gap(&_in, &_out, h->in_s, h->in_zp, h->out_s,
                              h->out_zp);
                break;
            }
            case TML_FC: {
                tml_fc_t* l = (tml_fc_t*)(mdl->layer_body);
                res =
                    tml_fc(&_in, &_out, (wtype_t*)(mdl->layer_body + l->w_oft),
                           (btype_t*)(mdl->layer_body + l->b_oft),
                           (sctype_t*)(mdl->layer_body + l->ws_oft), h->in_s,
                           h->in_zp, h->out_s, h->out_zp);
                break;
            }
            case TML_SOFTMAX: {
                tml_softmax_t* l __attribute__((unused)) =
                    (tml_softmax_t*)(mdl->layer_body);
                res = tml_softmax(&_in, &_out, h->in_s, h->in_zp, h->out_s,
                                  h->out_zp);
                break;
            }
            case TML_RESHAPE: {
                tml_reshape_t* l __attribute__((unused)) =
                    (tml_reshape_t*)(mdl->layer_body);
                res = tml_reshape(&_in, &_out, h->in_s, h->in_zp, h->out_s,
                                  h->out_zp);
                break;
            }
            default:
                res = TM_ERR_LAYERTYPE;
                break;
        }
        if (res != TM_OK)
            return res;
        if (mdl->cb)
            ((tm_cb_t)mdl->cb)(mdl, h);  // layer callback
        if (h->is_out) {
            memcpy((void*)(&out[out_idx]), (void*)(&(h->out_dims)),
                   sizeof(uint16_t) * 4);
            if (mdl->b->out_deq == 0 ||
                TM_MDL_TYPE == TM_MDL_FP32)  // fp32 do not need deq
                out[out_idx].data = (mtype_t*)(TML_GET_OUTPUT(mdl, h));
            else {
                int out_size = h->out_dims[1] * h->out_dims[2] * h->out_dims[3];
                float* outf =
                    (float*)(TM_ALIGN(TML_GET_OUTPUT(mdl, h) + out_size));
                for (int i = 0; i < out_size; i++)  // do dequant
                    outf[i] = TML_DEQUANT(h, (TML_GET_OUTPUT(mdl, h))[i]);
                out[out_idx].dataf = outf;
            }
            out_idx += 1;
        }
        mdl->layer_body += (h->size);
    }
    return TM_OK;
}
