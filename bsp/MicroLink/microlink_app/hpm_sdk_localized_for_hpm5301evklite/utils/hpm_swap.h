/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _HPM_SWAP_H
#define _HPM_SWAP_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus  */

uint32_t __bswapsi2(uint32_t u);
uint64_t __bswapdi2(uint64_t u);

#ifdef __cplusplus
}
#endif /* __cplusplus  */
#endif
