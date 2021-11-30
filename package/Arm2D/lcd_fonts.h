/*
 * Copyright (c) 2009-2021 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#ifndef __LCD_FONTS_H__
#define __LCD_FONTS_H__

/*============================ INCLUDES ======================================*/
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Font definitions */
typedef struct _GLCD_FONT {
        uint16_t width;         ///< Character width
        uint16_t height;        ///< Character height
        uint32_t offset;        ///< Character offset
        uint32_t count;         ///< Character count
  const uint8_t *bitmap;        ///< Characters bitmaps
} const GLCD_FONT;


#ifdef __cplusplus
}
#endif

#endif
