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
// It is O1 implement
// warning: O1 code bloat much to get better performance, experimental now
/*
pwconv
dwconv
    3x3：tm_dot_prod_gap_3x3x1
    none 3x3:
mdwconv
conv
*/

#include "PikaObj.h"
#include "float.h"
#include "math.h"
#include "tinymaix.h"

#if TM_OPT_LEVEL == TM_OPT1

#if TM_ARCH == TM_ARCH_CPU
#include "arch_cpu.h"
#elif TM_ARCH == TM_ARCH_ARM_SIMD
#include "arch_arm_simd.h"
#elif TM_ARCH == TM_ARCH_ARM_NEON
#include "arch_arm_neon.h"
#elif TM_ARCH == TM_ARCH_ARM_MVEI
#include "arch_arm_mvei.h"
#elif TM_ARCH == TM_ARCH_RV32P
#include "arch_rv32p.h"
#elif TM_ARCH == TM_ARCH_RV64V
#include "arch_rv64v.h"
#else
#error "UNSUPPORT ARCH!"
#endif

TM_PERF_REG(t_sbuf);
TM_PERF_REG(t_dotp);
TM_PERF_REG(t_post);
TM_PERF_REG(t_valid);
TM_PERF_REG(t_pad);
TM_PERF_REG(t_conv);
TM_PERF_REG(t_pwconv);
TM_PERF_REG(t_dwconv);

#define BATCH_SIZE 2  // batch sum size
/*************************** TML_CONV2D **********************************/
static uint32_t k_oft[TM_MAX_KSIZE];
static mtype_t sbuf[TM_MAX_KCSIZE];
#if (TM_MDL_TYPE == TM_MDL_FP32) || (TM_MDL_TYPE == TM_MDL_FP16)
#define SUMSCALE (1.0)
static sctype_t outscale;
TM_INLINE void l_postprocess_sum(sumtype_t sum,
                                 btype_t b,
                                 int act,
                                 mtype_t* outp,
                                 sctype_t scale,
                                 sctype_t out_s,
                                 zptype_t out_zp) {
    sum += b;
    switch (act) {  // activation func
        case TM_ACT_RELU:
        case TM_ACT_RELU6:  // treat relu6 as relu in float mode //speed up
            sum = sum > 0 ? sum : 0;
            break;
        //    sum = sum>0?sum:0;
        //    sum = sum>6?6:sum;
        //    break;
        default:
            break;
    }
    *outp = (mtype_t)sum;
    return;
}

#elif (TM_MDL_TYPE == TM_MDL_INT8) || (TM_MDL_TYPE == TM_MDL_INT16)

#if TM_FASTSCALE
static int32_t sumscale[TM_MAX_CSIZE];
static int32_t outscale;
#else
static float sumscale[TM_MAX_CSIZE];
static sctype_t outscale;
#endif
#define SUMSCALE (sumscale[c])

#if !TM_FASTSCALE
TM_INLINE void l_postprocess_sum(sumtype_t sum,
                                 btype_t b,
                                 int act,
                                 mtype_t* outp,
                                 sctype_t scale,
                                 sctype_t out_s,
                                 zptype_t out_zp)
#else
TM_INLINE void l_postprocess_sum(sumtype_t sum,
                                 btype_t b,
                                 int act,
                                 mtype_t* outp,
                                 int32_t scale,
                                 int32_t out_s,
                                 zptype_t out_zp)
#endif
{
    sum += b;
#if !TM_FASTSCALE
    float sumf = sum * scale;
#else
    sumtype_t sumf = (sum << TM_FASTSCALE_SHIFT) / scale;
#endif
    switch (act) {  // activation func
        case TM_ACT_RELU:
            sumf = sumf > 0 ? sumf : 0;
            break;
        case TM_ACT_RELU6:
            sumf = sumf > 0 ? sumf : 0;
#if (!TM_FASTSCALE)
            sumf = sumf > 6 ? 6 : sumf;
#else
            sumf = sumf > (6 << TM_FASTSCALE_SHIFT) ? (6 << TM_FASTSCALE_SHIFT)
                                                    : sumf;
#endif
            break;
        default:
            break;
    }
#if !TM_FASTSCALE
    *outp = (mtype_t)(sumf / out_s +
                      out_zp);  //(mtype_t)((int)(sumf/out_s) + out_zp)
                                ////(mtype_t)((int)(sumf/out_s +0.5) + out_zp)
#else
    *outp = (mtype_t)(((sumf * out_s) >>
                       (TM_FASTSCALE_SHIFT + TM_FASTSCALE_SHIFT)) +
                      out_zp);
#endif
    return;
}
#endif

// 1x1 pw conv
TM_INLINE tm_err_t l_tml_pwconv2d(tm_mat_t* in,
                                  tm_mat_t* out,
                                  wtype_t* w,
                                  btype_t* b,
                                  int kw,
                                  int kh,
                                  int sx,
                                  int sy,
                                  int dx,
                                  int dy,
                                  int act,
                                  int pad_top,
                                  int pad_bottom,
                                  int pad_left,
                                  int pad_right,
                                  int dmul,
                                  sctype_t* ws,
                                  sctype_t in_s,
                                  zptype_t in_zp,
                                  sctype_t out_s,
                                  zptype_t out_zp) {
    // int pad_flag = (pad_top != 0 ||pad_bottom != 0 ||pad_left != 0
    // ||pad_right != 0); int maxk = kw*kh;
    int chi = in->c;
    // int cho  = out->c;
    sumtype_t sum;
    mtype_t* outp = out->data;
    sumtype_t sums[BATCH_SIZE];

    for (int y = 0; y < out->h; y++) {
        for (int x = 0; x < out->w; x++) {
            mtype_t* sptr = (mtype_t*)TM_MATP(in, sy * y, sx * x, 0);
            wtype_t* kptr = (wtype_t*)w;

            int c = 0;
            for (; c < out->c - BATCH_SIZE + 1;) {
                tm_dot_prod_pack2(sptr, kptr, chi, sums);
                l_postprocess_sum(sums[0], b[c], act, outp, SUMSCALE, outscale,
                                  out_zp);
                c++;
                outp++;
                l_postprocess_sum(sums[1], b[c], act, outp, SUMSCALE, outscale,
                                  out_zp);
                c++;
                outp++;
                kptr += chi * BATCH_SIZE;  //*2;
            }
            for (; c < out->c; c++) {
                tm_dot_prod(sptr, kptr, chi,
                            &sum);  // size=maxk*chi //pw maxk==1
                l_postprocess_sum(sum, b[c], act, outp, SUMSCALE, outscale,
                                  out_zp);
                outp++;
                kptr += chi;
            }
        }
    }
    return TM_OK;
}

// normal conv
TM_INLINE tm_err_t l_tml_conv2d(tm_mat_t* in,
                                tm_mat_t* out,
                                wtype_t* w,
                                btype_t* b,
                                int kw,
                                int kh,
                                int sx,
                                int sy,
                                int dx,
                                int dy,
                                int act,
                                int pad_top,
                                int pad_bottom,
                                int pad_left,
                                int pad_right,
                                int dmul,
                                sctype_t* ws,
                                sctype_t in_s,
                                zptype_t in_zp,
                                sctype_t out_s,
                                zptype_t out_zp) {
    // int pad_flag = (pad_top != 0 ||pad_bottom != 0 ||pad_left != 0
    // ||pad_right != 0);
    int maxk = kw * kh;
    int chi = in->c;
    // int cho  = out->c;
    // sumtype_t sum;
    mtype_t* outp = out->data;

    int oft = 0;
    int idx = 0;
    for (int y = 0; y < kh; y++) {  // gen k_oft table
        for (int x = 0; x < kw; x++) {
            k_oft[idx] = oft;
            idx += 1;
            oft += chi;
        }
        oft += (in->w - kw) * chi;
    }
    int slow_flag = 0;  // same pad part is slow

    for (int y = 0; y < out->h; y++) {
        int src_y0 = sy * y - pad_top;
        for (int x = 0; x < out->w; x++) {
            int src_x0 = sx * x - pad_left;
            sumtype_t sum;
            slow_flag = ((src_y0 < 0) + (src_x0 < 0) + (src_y0 + kh > in->h) +
                         (src_x0 + kw > in->w));
            if (!slow_flag) {  // valid or same valid part
                mtype_t* sptr_base =
                    (mtype_t*)TM_MATP(in, src_y0, src_x0, 0);  //?c/dmul:0
                mtype_t* sptr = sptr_base;  //= (mtype_t*)TM_MATP(in, src_y0,
                                            // src_x0, 0); //sbuf 不变
                uint32_t sidx = 0;          // sbuf:cho,chi,maxk //dw:chi==1;
                for (int cc = 0; cc < chi; cc++) {
                    for (int k = 0; k < maxk; k++) {
                        sbuf[sidx + k] = sptr[k_oft[k]];
                    }
                    sidx += maxk;
                    sptr += 1;
                }
            } else {  // same pad part
                int _ky0 = src_y0 < 0 ? -src_y0 : 0;
                int _kx0 = src_x0 < 0 ? -src_x0 : 0;
                int _ky1 = in->h - src_y0 > kh ? kh : in->h - src_y0;
                int _kx1 = in->w - src_x0 > kw ? kw : in->w - src_x0;
                uint32_t sidx = 0;  // sbuf:cho,chi,maxk //dw:chi==1;
                // uint32_t s_step = (_ky1-_ky0)*(_kx1-_kx0);
                mtype_t* sptr_base = (mtype_t*)TM_MATP(in, src_y0, src_x0, 0);
                mtype_t* sptr = sptr_base;
#if TM_MDL_TYPE == TM_MDL_INT8
                memset(sbuf, in_zp, chi * maxk);  // do padding
#elif (TM_MDL_TYPE == TM_MDL_FP32) || (TM_MDL_TYPE == TM_MDL_FP16)
                memset(sbuf, 0, chi * maxk * sizeof(mtype_t));
#else
#error "unsupport mdl type"
#endif
                for (int cc = 0; cc < chi; cc++) {
                    for (int _ky = _ky0; _ky < _ky1; _ky++) {
                        for (int _kx = _kx0; _kx < _kx1; _kx++) {
                            int k = _ky * kw + _kx;
                            sbuf[sidx + k] = sptr[k_oft[k]];
                        }
                    }
                    sidx += maxk;
                    sptr += 1;
                }
            }
            mtype_t* sptr = sbuf;  // sbuf prepare ok~
            sumtype_t sums[BATCH_SIZE];
            int c = 0;
            wtype_t* kptr = (wtype_t*)w;
            for (; c < out->c - BATCH_SIZE + 1;) {
                tm_dot_prod_pack2(sptr, kptr, maxk * chi, sums);
                l_postprocess_sum(sums[0], b[c], act, outp, SUMSCALE, outscale,
                                  out_zp);
                c++;
                outp++;
                l_postprocess_sum(sums[1], b[c], act, outp, SUMSCALE, outscale,
                                  out_zp);
                c++;
                outp++;
                kptr += chi * maxk * BATCH_SIZE;
            }
            for (; c < out->c; c++) {
                tm_dot_prod(sptr, kptr, maxk * chi, &sum);
                l_postprocess_sum(sum, b[c], act, outp, SUMSCALE, outscale,
                                  out_zp);
                outp++;
                kptr += chi * maxk;
            }
        }
    }
    return TM_OK;
}

// dwconv
TM_INLINE tm_err_t l_tml_dwconv2d(tm_mat_t* in,
                                  tm_mat_t* out,
                                  wtype_t* w,
                                  btype_t* b,
                                  int kw,
                                  int kh,
                                  int sx,
                                  int sy,
                                  int dx,
                                  int dy,
                                  int act,
                                  int pad_top,
                                  int pad_bottom,
                                  int pad_left,
                                  int pad_right,
                                  int dmul,
                                  sctype_t* ws,
                                  sctype_t in_s,
                                  zptype_t in_zp,
                                  sctype_t out_s,
                                  zptype_t out_zp) {
    // int pad_flag = (pad_top != 0 ||pad_bottom != 0 ||pad_left != 0
    // ||pad_right != 0);
    int maxk = kw * kh;
    int chi = in->c;
    int cho = out->c;
    // sumtype_t sum;
    mtype_t* outp = out->data;

    int oft = 0;
    int idx = 0;
    for (int y = 0; y < kh; y++) {  // gen k_oft table
        for (int x = 0; x < kw; x++) {
            k_oft[idx] = oft;
            idx += 1;
            oft += chi;
        }
        oft += (in->w - kw) * chi;
    }
    int slow_flag = 0;  // same pad part is slow

    for (int y = 0; y < out->h; y++) {
        int src_y0 = sy * y - pad_top;
        for (int x = 0; x < out->w; x++) {
            int src_x0 = sx * x - pad_left;
            sumtype_t sum;
            slow_flag = ((src_y0 < 0) + (src_x0 < 0) + (src_y0 + kh > in->h) +
                         (src_x0 + kw > in->w));
            if (!slow_flag) {  // valid or same valid part
                mtype_t* sptr_base =
                    (mtype_t*)TM_MATP(in, src_y0, src_x0, 0);  //?c/dmul:0
                mtype_t* sptr = sptr_base;  //= (mtype_t*)TM_MATP(in, src_y0,
                                            // src_x0, 0); //sbuf 不变
                if (maxk == 9) {
                    for (int c = 0; c < cho; c++) {
                        wtype_t* kptr = (wtype_t*)w + c * 9;
                        tm_dot_prod_gap_3x3x1(sptr, kptr, k_oft, &sum);
                        l_postprocess_sum(sum, b[c], act, outp, SUMSCALE,
                                          outscale, out_zp);
                        outp++;
                        sptr += 1;
                    }
                } else {
                    uint32_t sidx = 0;
                    for (int c = 0; c < cho; c++) {
                        for (int k = 0; k < maxk; k++) {
                            sbuf[sidx + k] = sptr[k_oft[k]];
                        }
                        sidx += maxk;
                        sptr += 1;
                    }
                    sptr = sbuf;  // sbuf prepare ok~
                    for (int c = 0; c < out->c; c++) {
                        sum = 0;
                        wtype_t* kptr = (wtype_t*)w + c * maxk;
                        tm_dot_prod(sptr, kptr, maxk, &sum);
                        // sum = sptr[0]*kptr[0] + sptr[1]*kptr[1] +
                        // sptr[2]*kptr[2] + sptr[3]*kptr[3] + sptr[4]*kptr[4] +
                        // sptr[5]*kptr[5] + sptr[6]*kptr[6] + sptr[7]*kptr[7] +
                        // sptr[8]*kptr[8] ;
                        l_postprocess_sum(sum, b[c], act, outp, SUMSCALE,
                                          outscale, out_zp);
                        outp++;
                        sptr += maxk;  // dwconv need move step
                    }
                }
            } else {  // same pad part
                int _ky0 = src_y0 < 0 ? -src_y0 : 0;
                int _kx0 = src_x0 < 0 ? -src_x0 : 0;
                int _ky1 = in->h - src_y0 > kh ? kh : in->h - src_y0;
                int _kx1 = in->w - src_x0 > kw ? kw : in->w - src_x0;
                uint32_t sidx = 0;  // sbuf:cho,chi,maxk //dw:chi==1;
                // uint32_t s_step = (_ky1-_ky0)*(_kx1-_kx0);
                mtype_t* sptr_base = (mtype_t*)TM_MATP(in, src_y0, src_x0, 0);
                mtype_t* sptr = sptr_base;
#if TM_MDL_TYPE == TM_MDL_INT8
                memset(sbuf, in_zp, cho * maxk);  // do padding
#elif (TM_MDL_TYPE == TM_MDL_FP32) || (TM_MDL_TYPE == TM_MDL_FP16)
                memset(sbuf, 0, cho * maxk * sizeof(mtype_t));
#else
#error "unsupport mdl type"
#endif
                for (int cc = 0; cc < cho; cc++) {
                    for (int _ky = _ky0; _ky < _ky1; _ky++) {
                        for (int _kx = _kx0; _kx < _kx1; _kx++) {
                            int k = _ky * kw + _kx;
                            sbuf[sidx + k] = sptr[k_oft[k]];
                        }
                    }
                    sidx += maxk;
                    sptr += 1;
                }
                sptr = sbuf;  // sbuf prepare ok~
                if (maxk == 9) {
                    for (int c = 0; c < cho; c++) {
                        wtype_t* kptr = (wtype_t*)w + c * 9;
                        tm_dot_prod_3x3x1(sptr, kptr, &sum);
                        l_postprocess_sum(sum, b[c], act, outp, SUMSCALE,
                                          outscale, out_zp);
                        outp++;
                        sptr += maxk;
                    }
                } else {
                    for (int c = 0; c < out->c; c++) {
                        wtype_t* kptr = (wtype_t*)w + c * maxk;
                        tm_dot_prod(sptr, kptr, maxk, &sum);
                        l_postprocess_sum(sum, b[c], act, outp, SUMSCALE,
                                          outscale, out_zp);
                        outp++;
                        sptr += maxk;  // dwconv need move step
                    }
                }
            }
        }
    }
    return TM_OK;
}

// dwconv
TM_INLINE tm_err_t l_tml_dwconv2d_3x3_part(tm_mat_t* in,
                                           tm_mat_t* out,
                                           wtype_t* w,
                                           btype_t* b,
                                           int kw,
                                           int kh,
                                           int sx,
                                           int sy,
                                           int dx,
                                           int dy,
                                           int act,
                                           int pad_top,
                                           int pad_bottom,
                                           int pad_left,
                                           int pad_right,
                                           int dmul,
                                           sctype_t* ws,
                                           sctype_t in_s,
                                           zptype_t in_zp,
                                           sctype_t out_s,
                                           zptype_t out_zp,
                                           int x0,
                                           int x1,
                                           int y0,
                                           int y1) {
    // int pad_flag = (pad_top != 0 ||pad_bottom != 0 ||pad_left != 0
    // ||pad_right != 0);
    int maxk = kw * kh;
    int chi = in->c;
    int cho = out->c;
    // sumtype_t sum;
    mtype_t* outp = out->data;

    int oft = 0;
    int idx = 0;
    for (int y = 0; y < kh; y++) {  // gen k_oft table
        for (int x = 0; x < kw; x++) {
            k_oft[idx] = oft;
            idx += 1;
            oft += chi;
        }
        oft += (in->w - kw) * chi;
    }
    int slow_flag = 0;  // same pad part is slow

    for (int y = 0; y < out->h; y++) {
        int src_y0 = sy * y - pad_top;
        for (int x = 0; x < out->w; x++) {
            int src_x0 = sx * x - pad_left;
            sumtype_t sum;
            slow_flag = ((src_y0 < 0) + (src_x0 < 0) + (src_y0 + kh > in->h) +
                         (src_x0 + kw > in->w));
            if (!slow_flag) {  // valid or same valid part
                mtype_t* sptr_base =
                    (mtype_t*)TM_MATP(in, src_y0, src_x0, 0);  //?c/dmul:0
                mtype_t* sptr = sptr_base;  //= (mtype_t*)TM_MATP(in, src_y0,
                                            // src_x0, 0); //sbuf 不变
                for (int c = 0; c < cho; c++) {
                    wtype_t* kptr = (wtype_t*)w + c * 9;
                    tm_dot_prod_gap_3x3x1(sptr, kptr, k_oft, &sum);
                    l_postprocess_sum(sum, b[c], act, outp, SUMSCALE, outscale,
                                      out_zp);
                    outp++;
                    sptr += 1;
                }
            } else {  // same pad part
                int _ky0 = src_y0 < 0 ? -src_y0 : 0;
                int _kx0 = src_x0 < 0 ? -src_x0 : 0;
                int _ky1 = in->h - src_y0 > kh ? kh : in->h - src_y0;
                int _kx1 = in->w - src_x0 > kw ? kw : in->w - src_x0;
                uint32_t sidx = 0;  // sbuf:cho,chi,maxk //dw:chi==1;
                // uint32_t s_step = (_ky1-_ky0)*(_kx1-_kx0);
                mtype_t* sptr_base = (mtype_t*)TM_MATP(in, src_y0, src_x0, 0);
                mtype_t* sptr = sptr_base;
#if TM_MDL_TYPE == TM_MDL_INT8
                memset(sbuf, in_zp, cho * maxk);  // do padding
#elif (TM_MDL_TYPE == TM_MDL_FP32) || (TM_MDL_TYPE == TM_MDL_FP16)
                memset(sbuf, 0, cho * maxk * sizeof(mtype_t));
#else
#error "unsupport mdl type"
#endif
                for (int cc = 0; cc < cho; cc++) {
                    for (int _ky = _ky0; _ky < _ky1; _ky++) {
                        for (int _kx = _kx0; _kx < _kx1; _kx++) {
                            int k = _ky * kw + _kx;
                            sbuf[sidx + k] = sptr[k_oft[k]];
                        }
                    }
                    sidx += maxk;
                    sptr += 1;
                }
                sptr = sbuf;  // sbuf prepare ok~
                outp = out->data + (y * out->w + x) * cho;
                for (int c = 0; c < cho; c++) {
                    wtype_t* kptr = (wtype_t*)w + c * 9;
                    tm_dot_prod_3x3x1(sptr, kptr, &sum);
                    l_postprocess_sum(sum, b[c], act, outp, SUMSCALE, outscale,
                                      out_zp);
                    outp++;
                    sptr += maxk;
                }
            }
        }
    }
    return TM_OK;
}

#define CONV_BLK_W (4)
#define CONV_BLK_H (4)
#define CONV_BLK_STEPX (CONV_BLK_W - 3 + 1)
#define CONV_BLK_STEPY (CONV_BLK_H - 3 + 1)
static mtype_t dw_sbuf[CONV_BLK_W * CONV_BLK_H * TM_MAX_CSIZE];
static uint32_t dw_koft[CONV_BLK_W * CONV_BLK_H];

// 40ms->27ms
TM_INLINE tm_err_t l_tml_dwconv2d_3x3_nostride(tm_mat_t* in,
                                               tm_mat_t* out,
                                               wtype_t* w,
                                               btype_t* b,
                                               int kw,
                                               int kh,
                                               int sx,
                                               int sy,
                                               int dx,
                                               int dy,
                                               int act,
                                               int pad_top,
                                               int pad_bottom,
                                               int pad_left,
                                               int pad_right,
                                               int dmul,
                                               sctype_t* ws,
                                               sctype_t in_s,
                                               zptype_t in_zp,
                                               sctype_t out_s,
                                               zptype_t out_zp) {
    // int pad_flag = (pad_top != 0 ||pad_bottom != 0 ||pad_left != 0
    // ||pad_right != 0);
    int maxk = kw * kh;
    int chi = in->c;
    int cho = out->c;
    // sumtype_t sum;
    mtype_t* outp = out->data;

    int oft = 0;
    int idx = 0;
    for (int y = 0; y < kh; y++) {  // gen k_oft table
        for (int x = 0; x < kw; x++) {
            k_oft[idx] = oft;
            idx += 1;
            oft += chi;
        }
        oft += (in->w - kw) * chi;
    }
    oft = 0;
    idx = 0;
    for (int y = 0; y < CONV_BLK_H; y++) {  // gen k_oft table
        for (int x = 0; x < CONV_BLK_W; x++) {
            dw_koft[idx] = oft;
            idx += 1;
            oft += chi;
        }
        oft += (in->w - CONV_BLK_W) * chi;
    }
    int slow_flag = 0;  // same pad part is slow
    sumtype_t sum0, sum1, sum2, sum3;

    uint32_t maxk_blk = CONV_BLK_H * CONV_BLK_W;
    for (int y = 0; y + CONV_BLK_STEPY <= out->h; y += CONV_BLK_STEPY) {
        int src_y0 = sy * y - pad_top;
        for (int x = 0; x + CONV_BLK_STEPX <= out->w; x += CONV_BLK_STEPX) {
            int src_x0 = sx * x - pad_left;
            // sumtype_t sum;
            slow_flag =
                ((src_y0 < 0) + (src_x0 < 0) + (src_y0 + CONV_BLK_H > in->h) +
                 (src_x0 + CONV_BLK_W > in->w));
            if (!slow_flag) {  // valid or same valid part
                mtype_t* sptr_base =
                    (mtype_t*)TM_MATP(in, src_y0, src_x0, 0);  //?c/dmul:0
                mtype_t* sptr = sptr_base;  //= (mtype_t*)TM_MATP(in, src_y0,
                                            // src_x0, 0); //sbuf 不变
                // uint32_t sidx=0;    //sbuf:cho,chi,maxk //dw:chi==1;
                outp = out->data + (y * out->w + x) * cho;
                for (int c = 0; c < cho; c++) {
                    wtype_t* kptr = (wtype_t*)w + c * maxk;
                    sum0 = sptr[dw_koft[0]] * kptr[0] +
                           sptr[dw_koft[1]] * kptr[1] +
                           sptr[dw_koft[2]] * kptr[2] +
                           sptr[dw_koft[4]] * kptr[3] +
                           sptr[dw_koft[5]] * kptr[4] +
                           sptr[dw_koft[6]] * kptr[5] +
                           sptr[dw_koft[8]] * kptr[6] +
                           sptr[dw_koft[9]] * kptr[7] +
                           sptr[dw_koft[10]] * kptr[8];
                    sum1 = sptr[dw_koft[1]] * kptr[0] +
                           sptr[dw_koft[2]] * kptr[1] +
                           sptr[dw_koft[3]] * kptr[2] +
                           sptr[dw_koft[5]] * kptr[3] +
                           sptr[dw_koft[6]] * kptr[4] +
                           sptr[dw_koft[7]] * kptr[5] +
                           sptr[dw_koft[9]] * kptr[6] +
                           sptr[dw_koft[10]] * kptr[7] +
                           sptr[dw_koft[11]] * kptr[8];
                    sum2 = sptr[dw_koft[4]] * kptr[0] +
                           sptr[dw_koft[5]] * kptr[1] +
                           sptr[dw_koft[6]] * kptr[2] +
                           sptr[dw_koft[8]] * kptr[3] +
                           sptr[dw_koft[9]] * kptr[4] +
                           sptr[dw_koft[10]] * kptr[5] +
                           sptr[dw_koft[12]] * kptr[6] +
                           sptr[dw_koft[13]] * kptr[7] +
                           sptr[dw_koft[14]] * kptr[8];
                    sum3 = sptr[dw_koft[5]] * kptr[0] +
                           sptr[dw_koft[6]] * kptr[1] +
                           sptr[dw_koft[7]] * kptr[2] +
                           sptr[dw_koft[9]] * kptr[3] +
                           sptr[dw_koft[10]] * kptr[4] +
                           sptr[dw_koft[11]] * kptr[5] +
                           sptr[dw_koft[13]] * kptr[6] +
                           sptr[dw_koft[14]] * kptr[7] +
                           sptr[dw_koft[15]] * kptr[8];
                    l_postprocess_sum(sum0, b[c], act, outp + 0 * cho, SUMSCALE,
                                      outscale, out_zp);
                    l_postprocess_sum(sum1, b[c], act, outp + 1 * cho, SUMSCALE,
                                      outscale, out_zp);
                    l_postprocess_sum(sum2, b[c], act,
                                      outp + (out->w + 0) * cho, SUMSCALE,
                                      outscale, out_zp);
                    l_postprocess_sum(sum3, b[c], act,
                                      outp + (out->w + 1) * cho, SUMSCALE,
                                      outscale, out_zp);
                    outp++;
                    sptr++;
                }
            } else {  // same pad part
                int _ky0 = src_y0 < 0 ? -src_y0 : 0;
                int _kx0 = src_x0 < 0 ? -src_x0 : 0;
                int _ky1 =
                    in->h - src_y0 > CONV_BLK_H ? CONV_BLK_H : in->h - src_y0;
                int _kx1 =
                    in->w - src_x0 > CONV_BLK_W ? CONV_BLK_W : in->w - src_x0;
                uint32_t sidx = 0;  // sbuf:cho,chi,maxk //dw:chi==1;
                // uint32_t s_step = (_ky1-_ky0)*(_kx1-_kx0);
                mtype_t* sptr_base = (mtype_t*)TM_MATP(in, src_y0, src_x0, 0);
                mtype_t* sptr = sptr_base;
#if TM_MDL_TYPE == TM_MDL_INT8
                memset(dw_sbuf, in_zp, cho * maxk_blk);  // do padding
#elif (TM_MDL_TYPE == TM_MDL_FP32) || (TM_MDL_TYPE == TM_MDL_FP16)
                memset(dw_sbuf, 0, cho * maxk_blk * sizeof(mtype_t));
#else
#error "unsupport mdl type"
#endif
                for (int cc = 0; cc < cho; cc++) {
                    for (int _ky = _ky0; _ky < _ky1; _ky++) {
                        for (int _kx = _kx0; _kx < _kx1; _kx++) {
                            int k = _ky * CONV_BLK_W + _kx;
                            dw_sbuf[sidx + k] = sptr[dw_koft[k]];
                        }
                    }
                    sidx += maxk_blk;
                    sptr++;
                }
                sptr = dw_sbuf;  // sbuf prepare ok~
                outp = out->data + (y * out->w + x) * cho;
                sumtype_t sum0, sum1, sum2, sum3;
                for (int c = 0; c < out->c; c++) {
                    wtype_t* kptr = (wtype_t*)w + c * maxk;
                    // tm_dot_prod(sptr, kptr, maxk, &sum);TM_PERF_ADD(t_dotp);
                    // l_postprocess_sum(sum, b[c], act, outp+0*cho, SUMSCALE,
                    // outscale, out_zp);
                    sum0 = sptr[0] * kptr[0] + sptr[1] * kptr[1] +
                           sptr[2] * kptr[2] + sptr[4] * kptr[3] +
                           sptr[5] * kptr[4] + sptr[6] * kptr[5] +
                           sptr[8] * kptr[6] + sptr[9] * kptr[7] +
                           sptr[10] * kptr[8];
                    sum1 = sptr[1] * kptr[0] + sptr[2] * kptr[1] +
                           sptr[3] * kptr[2] + sptr[5] * kptr[3] +
                           sptr[6] * kptr[4] + sptr[7] * kptr[5] +
                           sptr[9] * kptr[6] + sptr[10] * kptr[7] +
                           sptr[11] * kptr[8];
                    sum2 = sptr[4] * kptr[0] + sptr[5] * kptr[1] +
                           sptr[6] * kptr[2] + sptr[8] * kptr[3] +
                           sptr[9] * kptr[4] + sptr[10] * kptr[5] +
                           sptr[12] * kptr[6] + sptr[13] * kptr[7] +
                           sptr[14] * kptr[8];
                    sum3 = sptr[5] * kptr[0] + sptr[6] * kptr[1] +
                           sptr[7] * kptr[2] + sptr[9] * kptr[3] +
                           sptr[10] * kptr[4] + sptr[11] * kptr[5] +
                           sptr[13] * kptr[6] + sptr[14] * kptr[7] +
                           sptr[15] * kptr[8];
                    l_postprocess_sum(sum0, b[c], act, outp + 0 * cho, SUMSCALE,
                                      outscale, out_zp);
                    l_postprocess_sum(sum1, b[c], act, outp + 1 * cho, SUMSCALE,
                                      outscale, out_zp);
                    l_postprocess_sum(sum2, b[c], act,
                                      outp + (out->w + 0) * cho, SUMSCALE,
                                      outscale, out_zp);
                    l_postprocess_sum(sum3, b[c], act,
                                      outp + (out->w + 1) * cho, SUMSCALE,
                                      outscale, out_zp);
                    // printf("==%.1f,%.1f,%.1f,%.1f\r\n", out->data[0],
                    // out->data[1], out->data[2], out->data[3]);
                    sptr += maxk_blk;  // dwconv need move step
                    outp++;
                }
            }
        }
        // x loop end
    }

    if (out->w % CONV_BLK_W != 0 || out->h % CONV_BLK_H != 0) {  // cal rest
                                                                 // part
        // x = [out->w-out->w%CONV_BLK_W, out->w), y = [0,
        // out->h/CONV_BLK_H*CONV_BLK_H) x = [0, out->w), y =
        // [out->h-out->h%CONV_BLK_H, out->h/CONV_BLK_H*CONV_BLK_H)
        l_tml_dwconv2d_3x3_part(in, out, w, b, kw, kh, sx, sy, dx, dy, act,
                                pad_top, pad_bottom, pad_left, pad_right, dmul,
                                ws, in_s, in_zp, out_s, out_zp,
                                out->w - out->w % CONV_BLK_W, out->w, 0,
                                out->h / CONV_BLK_H * CONV_BLK_H);
        l_tml_dwconv2d_3x3_part(in, out, w, b, kw, kh, sx, sy, dx, dy, act,
                                pad_top, pad_bottom, pad_left, pad_right, dmul,
                                ws, in_s, in_zp, out_s, out_zp, 0, out->w,
                                out->h - out->h % CONV_BLK_H,
                                out->h / CONV_BLK_H * CONV_BLK_H);
    }

    // TODO: rest
    return TM_OK;
}

// dmul>1 dwconv //not opt yet
TM_INLINE tm_err_t l_tml_mdwconv2d(tm_mat_t* in,
                                   tm_mat_t* out,
                                   wtype_t* w,
                                   btype_t* b,
                                   int kw,
                                   int kh,
                                   int sx,
                                   int sy,
                                   int dx,
                                   int dy,
                                   int act,
                                   int pad_top,
                                   int pad_bottom,
                                   int pad_left,
                                   int pad_right,
                                   int dmul,
                                   sctype_t* ws,
                                   sctype_t in_s,
                                   zptype_t in_zp,
                                   sctype_t out_s,
                                   zptype_t out_zp) {
    // int pad_flag = (pad_top != 0 ||pad_bottom != 0 ||pad_left != 0
    // ||pad_right != 0);
    int maxk = kw * kh;
    int chi = in->c;
    int cho = out->c;
    // sumtype_t sum;
    mtype_t* outp = out->data;

    int oft = 0;
    int idx = 0;
    for (int y = 0; y < kh; y++) {  // gen k_oft table
        for (int x = 0; x < kw; x++) {
            k_oft[idx] = oft;
            idx += 1;
            oft += chi;
        }
        oft += (in->w - kw) * chi;
    }
    int slow_flag = 0;  // same pad part is slow

    for (int y = 0; y < out->h; y++) {
        int src_y0 = sy * y - pad_top;
        for (int x = 0; x < out->w; x++) {
            int src_x0 = sx * x - pad_left;
            sumtype_t sum;
            slow_flag = ((src_y0 < 0) + (src_x0 < 0) + (src_y0 + kh > in->h) +
                         (src_x0 + kw > in->w));
            if (!slow_flag) {  // valid or same valid part
                mtype_t* sptr_base =
                    (mtype_t*)TM_MATP(in, src_y0, src_x0, 0);  //?c/dmul:0
                mtype_t* sptr = sptr_base;  //= (mtype_t*)TM_MATP(in, src_y0,
                                            // src_x0, 0); //sbuf 不变
                uint32_t sidx = 0;          // sbuf:cho,chi,maxk //dw:chi==1;
                for (int cc = 0; cc < cho; cc++) {
                    for (int k = 0; k < maxk; k++) {
                        sbuf[sidx + k] = sptr[k_oft[k]];
                    }
                    sidx += maxk;
                    sptr = sptr_base + (cc + 1) / dmul;
                }
            } else {  // same pad part
                int _ky0 = src_y0 < 0 ? -src_y0 : 0;
                int _kx0 = src_x0 < 0 ? -src_x0 : 0;
                int _ky1 = in->h - src_y0 > kh ? kh : in->h - src_y0;
                int _kx1 = in->w - src_x0 > kw ? kw : in->w - src_x0;
                uint32_t sidx = 0;  // sbuf:cho,chi,maxk //dw:chi==1;
                // uint32_t s_step = (_ky1-_ky0)*(_kx1-_kx0);
                mtype_t* sptr_base = (mtype_t*)TM_MATP(in, src_y0, src_x0, 0);
                mtype_t* sptr = sptr_base;
#if TM_MDL_TYPE == TM_MDL_INT8
                memset(sbuf, in_zp, cho * maxk);  // do padding
#elif (TM_MDL_TYPE == TM_MDL_FP32) || (TM_MDL_TYPE == TM_MDL_FP16)
                memset(sbuf, 0, cho * maxk * sizeof(mtype_t));
#else
#error "unsupport mdl type"
#endif
                for (int cc = 0; cc < cho; cc++) {
                    for (int _ky = _ky0; _ky < _ky1; _ky++) {
                        for (int _kx = _kx0; _kx < _kx1; _kx++) {
                            int k = _ky * kw + _kx;
                            sbuf[sidx + k] = sptr[k_oft[k]];
                        }
                    }
                    sidx += maxk;
                    sptr = sptr_base + (cc + 1) / dmul;
                }
            }
            mtype_t* sptr = sbuf;  // sbuf prepare ok~
            for (int c = 0; c < out->c; c++) {
                sum = 0;
                wtype_t* kptr = (wtype_t*)w + c * maxk;
                tm_dot_prod(sptr, kptr, maxk, &sum);
                l_postprocess_sum(sum, b[c], act, outp, SUMSCALE, outscale,
                                  out_zp);
                outp++;
                sptr += maxk;  // dwconv need move step
            }
        }
    }
    return TM_OK;
}

tm_err_t tml_conv2d_dwconv2d(tm_mat_t* in,
                             tm_mat_t* out,
                             wtype_t* w,
                             btype_t* b,
                             int kw,
                             int kh,
                             int sx,
                             int sy,
                             int dx,
                             int dy,
                             int act,
                             int pad_top,
                             int pad_bottom,
                             int pad_left,
                             int pad_right,
                             int dmul,
                             sctype_t* ws,
                             sctype_t in_s,
                             zptype_t in_zp,
                             sctype_t out_s,
                             zptype_t out_zp)  // kernel: (cho, chi, h, w)
{
    TM_PERF_INIT(t_conv);
    TM_PERF_INIT(t_pwconv);
    TM_PERF_INIT(t_dwconv);
    int pad_flag =
        (pad_top != 0 || pad_bottom != 0 || pad_left != 0 || pad_right != 0);
    if (dx != 1 || dy != 1)
        return TM_ERR_TODO;
    if (act >= TM_ACT_MAXCNT)
        return TM_ERR_UNSUPPORT;
    int maxk = kw * kh;
    if (maxk > TM_MAX_KSIZE)
        return TM_ERR_KSIZE;
    if (maxk == 1 && (pad_flag || dmul))
        return TM_ERR_UNSUPPORT;  // assume no pad or dwconv when pwconv

#if (TM_MDL_TYPE == TM_MDL_INT8) || (TM_MDL_TYPE == TM_MDL_INT16)
#if TM_FASTSCALE
    outscale = (1 << TM_FASTSCALE_SHIFT) / out_s;
    for (int c = 0; c < out->c; c++)
        sumscale[c] = 1.0 / ws[c] / in_s;
#else
    outscale = out_s;
    for (int c = 0; c < out->c; c++)
        sumscale[c] = ws[c] * in_s;
#endif
#else
    outscale = out_s;
#endif

    if (maxk == 1) {
        TM_PERF_START(t_pwconv);  // pw conv
        l_tml_pwconv2d(in, out, w, b, kw, kh, sx, sy, dx, dy, act, pad_top,
                       pad_bottom, pad_left, pad_right, dmul, ws, in_s, in_zp,
                       out_s, out_zp);
        TM_PERF_ADD(t_pwconv);
    } else if (dmul == 0) {
        TM_PERF_START(t_conv);  // normal conv
        l_tml_conv2d(in, out, w, b, kw, kh, sx, sy, dx, dy, act, pad_top,
                     pad_bottom, pad_left, pad_right, dmul, ws, in_s, in_zp,
                     out_s, out_zp);
        TM_PERF_ADD(t_conv);
    } else if (dmul == 1) {
        TM_PERF_START(t_dwconv);   // dw conv
        if (kh == 3 && kw == 3) {  // opt for 3x3 dwconv
            if (sx == 1 && sy == 1) {
                l_tml_dwconv2d_3x3_nostride(in, out, w, b, kw, kh, sx, sy, dx,
                                            dy, act, pad_top, pad_bottom,
                                            pad_left, pad_right, dmul, ws, in_s,
                                            in_zp, out_s, out_zp);
            } else {
                l_tml_dwconv2d_3x3_part(in, out, w, b, kw, kh, sx, sy, dx, dy,
                                        act, pad_top, pad_bottom, pad_left,
                                        pad_right, dmul, ws, in_s, in_zp, out_s,
                                        out_zp, 0, out->w, 0, out->h);
            }
        } else {
            l_tml_dwconv2d(in, out, w, b, kw, kh, sx, sy, dx, dy, act, pad_top,
                           pad_bottom, pad_left, pad_right, dmul, ws, in_s,
                           in_zp, out_s, out_zp);
        }
        TM_PERF_ADD(t_dwconv);
    } else {  // dmul>1 dwconv, rare
        l_tml_mdwconv2d(in, out, w, b, kw, kh, sx, sy, dx, dy, act, pad_top,
                        pad_bottom, pad_left, pad_right, dmul, ws, in_s, in_zp,
                        out_s, out_zp);
    }

    return TM_OK;
}

/*************************** TML_GAP **********************************/
tm_err_t __attribute__((weak)) tml_gap(tm_mat_t* in,
                                       tm_mat_t* out,
                                       sctype_t in_s,
                                       zptype_t in_zp,
                                       sctype_t out_s,
                                       zptype_t out_zp) {
    TM_DBGT_INIT();
    mtype_t* data;
    for (int c = 0; c < out->c; c++) {
        sumtype_t sum = 0;
        data = in->data + c;
        for (int y = 0; y < in->h; y++) {
            for (int x = 0; x < in->w; x++) {
                sum += ((sumtype_t)(*data));
                data += out->c;
            }
        }
#if TM_MDL_TYPE == TM_MDL_INT8 || TM_MDL_TYPE == TM_MDL_INT16
        sumtype_t temp = ((in->h) * (in->w));
        if (temp != 0) {
            out->data[c] = (mtype_t)((sum / temp - in_zp) * in_s / out_s +
                                     out_zp);  // requant
        }
#elif TM_MDL_TYPE == TM_MDL_FP32 || TM_MDL_TYPE == TM_MDL_FP16
        out->data[c] = (mtype_t)(sum / ((in->h) * (in->w)));
//#else //#elif TM_MDL_TYPE == TM_MDL_FP8_143 || TM_MDL_TYPE == TM_MDL_FP8_152
#endif
    }
    return TM_OK;
}

/*************************** TML_FC **********************************/
tm_err_t __attribute__((weak)) tml_fc(tm_mat_t* in,
                                      tm_mat_t* out,
                                      wtype_t* w,
                                      btype_t* b,
                                      sctype_t* ws,
                                      sctype_t in_s,
                                      zptype_t in_zp,
                                      sctype_t out_s,
                                      zptype_t out_zp) {
    TM_DBGT_INIT();
    mtype_t* data = in->data;
    for (int c = 0; c < out->c; c++) {
        sumtype_t sum = 0;
        tm_dot_prod(data, w + c * in->c, in->c, &sum);
        sum += b[c];  // fuse with zp
#if TM_MDL_TYPE == TM_MDL_INT8 || TM_MDL_TYPE == TM_MDL_INT16
        out->data[c] =
            (mtype_t)(sum * in_s * ws[0] / out_s + out_zp);  // requant
#else
        out->data[c] = (mtype_t)(sum);
#endif
    }
    return TM_OK;
}

/*************************** TML_SOFTMAX **********************************/
tm_err_t __attribute__((weak)) tml_softmax(tm_mat_t* in,
                                           tm_mat_t* out,
                                           sctype_t in_s,
                                           zptype_t in_zp,
                                           sctype_t out_s,
                                           zptype_t out_zp) {
    TM_DBGT_INIT();  // note we have float size output buf even in INT8/INT16
                     // mode
    mtype_t* din = in->data;
    float* dout = (float*)(out->data);
    float dmax = -FLT_MAX;
    for (int c = 0; c < in->c; c++) {
#if TM_MDL_TYPE == TM_MDL_INT8 || TM_MDL_TYPE == TM_MDL_INT16
        dout[c] = (float)((sumtype_t)din[c] - in_zp) * in_s;
#else
        dout[c] = din[c];
#endif
        if (dout[c] > dmax)
            dmax = dout[c];
    }
    float sum = 0;
    for (int c = 0; c < in->c; c++) {
        dout[c] -= dmax;
        dout[c] = (float)exp(dout[c]);
        sum += dout[c];
        dout[c] -= 0.000001;  // prevent 1.0 value (cause 256 overflow)
    }
    for (int c = 0; c < in->c; c++) {  // int8/int16 <= fp32, so it is ok
#if TM_MDL_TYPE == TM_MDL_INT8 || TM_MDL_TYPE == TM_MDL_INT16
        out->data[c] = (mtype_t)(dout[c] / sum / out_s + out_zp);  // requant
#else
        out->data[c] = (mtype_t)(dout[c] / sum);
#endif
    }
    return TM_OK;
}

/*************************** TML_RESHAPE **********************************/
tm_err_t __attribute__((weak)) tml_reshape(tm_mat_t* in,
                                           tm_mat_t* out,
                                           sctype_t in_s,
                                           zptype_t in_zp,
                                           sctype_t out_s,
                                           zptype_t out_zp) {
    // in fact do nothing... out shape
    return TM_OK;
}

#endif
