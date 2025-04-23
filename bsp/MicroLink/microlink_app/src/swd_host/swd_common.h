
/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_common.h"

typedef struct {
    uint8_t start;
    uint8_t ap_n_dp;
    uint8_t r_n_w;
    uint8_t addr[2];
    uint8_t parity;
    uint8_t stop;
    uint8_t park;
} host_format_t;

typedef struct {
    uint8_t ap_dp;
    uint8_t rw;
    uint8_t addr;
    host_format_t *format;
} host_ops_t;

ATTR_PLACE_AT(".ahb_sram")  host_format_t host_format_table[] = {
    /* APnDP = 0, RnW = 0 */
    {
        .start = 0x01,
        .stop = 0x00,
        .park = 0x01,
        .ap_n_dp = 0x00,
        .r_n_w = 0x00,
        .addr = {0x00, 0x00},
        .parity = 0x00,
    },

    {
        .start = 0x01,
        .stop = 0x00,
        .park = 0x01,
        .ap_n_dp = 0x00,
        .r_n_w = 0x00,
        .addr = {0x00, 0x01},
        .parity = 0x01,
    },

    {
        .start = 0x01,
        .stop = 0x00,
        .park = 0x01,
        .ap_n_dp = 0x00,
        .r_n_w = 0x00,
        .addr = {0x01, 0x00},
        .parity = 0x01,
    },

    {
        .start = 0x01,
        .stop = 0x00,
        .park = 0x01,
        .ap_n_dp = 0x00,
        .r_n_w = 0x00,
        .addr = {0x01, 0x01},
        .parity = 0x00,
    },

    /* APnDP = 1, RnW = 0 */
    {
        .start = 0x01,
        .stop = 0x00,
        .park = 0x01,
        .ap_n_dp = 0x01,
        .r_n_w = 0x00,
        .addr = {0x00, 0x00},
        .parity = 0x01,
    },

    {
        .start = 0x01,
        .stop = 0x00,
        .park = 0x01,
        .ap_n_dp = 0x01,
        .r_n_w = 0x00,
        .addr = {0x00, 0x01},
        .parity = 0x00,
    },

    {
        .start = 0x01,
        .stop = 0x00,
        .park = 0x01,
        .ap_n_dp = 0x01,
        .r_n_w = 0x00,
        .addr = {0x01, 0x00},
        .parity = 0x00,
    },

    {
        .start = 0x01,
        .stop = 0x00,
        .park = 0x01,
        .ap_n_dp = 0x01,
        .r_n_w = 0x00,
        .addr = {0x01, 0x01},
        .parity = 0x01,
    },

    /* APnDP = 0, RnW = 1 */
    {
        .start = 0x01,
        .stop = 0x00,
        .park = 0x01,
        .ap_n_dp = 0x00,
        .r_n_w = 0x01,
        .addr = {0x00, 0x00},
        .parity = 0x01,
    },

    {
        .start = 0x01,
        .stop = 0x00,
        .park = 0x01,
        .ap_n_dp = 0x00,
        .r_n_w = 0x01,
        .addr = {0x00, 0x01},
        .parity = 0x00,
    },

    {
        .start = 0x01,
        .stop = 0x00,
        .park = 0x01,
        .ap_n_dp = 0x00,
        .r_n_w = 0x01,
        .addr = {0x01, 0x00},
        .parity = 0x00,
    },

    {
        .start = 0x01,
        .stop = 0x00,
        .park = 0x01,
        .ap_n_dp = 0x00,
        .r_n_w = 0x01,
        .addr = {0x01, 0x01},
        .parity = 0x01,
    },

    /* APnDP = 1, RnW = 1 */
    {
        .start = 0x01,
        .stop = 0x00,
        .park = 0x01,
        .ap_n_dp = 0x01,
        .r_n_w = 0x01,
        .addr = {0x00, 0x00},
        .parity = 0x00,
    },

    {
        .start = 0x01,
        .stop = 0x00,
        .park = 0x01,
        .ap_n_dp = 0x01,
        .r_n_w = 0x01,
        .addr = {0x00, 0x01},
        .parity = 0x01,
    },

    {
        .start = 0x01,
        .stop = 0x00,
        .park = 0x01,
        .ap_n_dp = 0x01,
        .r_n_w = 0x01,
        .addr = {0x01, 0x00},
        .parity = 0x01,
    },

    {
        .start = 0x01,
        .stop = 0x00,
        .park = 0x01,
        .ap_n_dp = 0x01,
        .r_n_w = 0x01,
        .addr = {0x01, 0x01},
        .parity = 0x00,
    },
};

ATTR_PLACE_AT(".ahb_sram")  host_ops_t host_ops_table[] = {
     /* APnDP = 0, RnW = 0, ADDR = 0x00 */
    {
        .ap_dp = 0x00,
        .rw = 0x00,
        .addr = 0x00,
        .format = &host_format_table[0],
    },

     /* APnDP = 0, RnW = 0, ADDR = 0x01 */
    {
        .ap_dp = 0x00,
        .rw = 0x00,
        .addr = 0x01,
        .format = &host_format_table[1],
    },

     /* APnDP = 0, RnW = 0, ADDR = 0x10 */
    {
        .ap_dp = 0x00,
        .rw = 0x00,
        .addr = 0x02,
        .format = &host_format_table[2],
    },

     /* APnDP = 0, RnW = 0, ADDR = 0x11 */
    {
        .ap_dp = 0x00,
        .rw = 0x00,
        .addr = 0x03,
        .format = &host_format_table[3],
    },

     /* APnDP = 1, RnW = 0, ADDR = 0x00 */
    {
        .ap_dp = 0x01,
        .rw = 0x00,
        .addr = 0x00,
        .format = &host_format_table[4],
    },

     /* APnDP = 1, RnW = 0, ADDR = 0x01 */
    {
        .ap_dp = 0x01,
        .rw = 0x00,
        .addr = 0x01,
        .format = &host_format_table[5],
    },

     /* APnDP = 1, RnW = 0, ADDR = 0x10 */
    {
        .ap_dp = 0x01,
        .rw = 0x00,
        .addr = 0x02,
        .format = &host_format_table[6],
    },

     /* APnDP = 1, RnW = 0, ADDR = 0x11 */
    {
        .ap_dp = 0x01,
        .rw = 0x00,
        .addr = 0x03,
        .format = &host_format_table[7],
    },

     /* APnDP = 0, RnW = 1, ADDR = 0x00 */
    {
        .ap_dp = 0x00,
        .rw = 0x01,
        .addr = 0x00,
        .format = &host_format_table[8],
    },

     /* APnDP = 0, RnW = 1, ADDR = 0x01 */
    {
        .ap_dp = 0x00,
        .rw = 0x01,
        .addr = 0x01,
        .format = &host_format_table[9],
    },

     /* APnDP = 0, RnW = 1, ADDR = 0x10 */
    {
        .ap_dp = 0x00,
        .rw = 0x01,
        .addr = 0x02,
        .format = &host_format_table[10],
    },

     /* APnDP = 0, RnW = 1, ADDR = 0x11 */
    {
        .ap_dp = 0x00,
        .rw = 0x01,
        .addr = 0x03,
        .format = &host_format_table[11],
    },

     /* APnDP = 1, RnW = 1, ADDR = 0x00 */
    {
        .ap_dp = 0x01,
        .rw = 0x01,
        .addr = 0x00,
        .format = &host_format_table[12],
    },

     /* APnDP = 1, RnW = 1, ADDR = 0x01 */
    {
        .ap_dp = 0x01,
        .rw = 0x01,
        .addr = 0x01,
        .format = &host_format_table[13],
    },

     /* APnDP = 1, RnW = 1, ADDR = 0x10 */
    {
        .ap_dp = 0x01,
        .rw = 0x01,
        .addr = 0x02,
        .format = &host_format_table[14],
    },

     /* APnDP = 1, RnW = 1, ADDR = 0x11 */
    {
        .ap_dp = 0x01,
        .rw = 0x01,
        .addr = 0x03,
        .format = &host_format_table[15],
    },

};
