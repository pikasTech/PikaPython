/*****************************************************************************
 *   Copyrig;ht(C)2009-2019 by GorgonMeducer<embedded_zhuoran@hotmail.com>    *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/
#include "./trace.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
/*============================ MACROS ========================================*/
#ifndef TRACE_DISPLAY_WIDTH
#   define TRACE_DISPLAY_WIDTH          16
#endif

#undef this
#define this        (*ptThis)

/*============================ MACROFIED FUNCTIONS ===========================*/
#ifndef ASSERT
#   define ASSERT(...)
#endif


/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
static void __trace_init(trace_cfg_t *ptCFG);
static void __trace_string(const char * pchString);
static void __trace_uint32_to_string(uint32_t);
static void __trace_int32_to_string(int32_t);
static void __trace_uint16_to_string(uint16_t);
static void __trace_int16_to_string(int16_t);
static void __trace_uint8_to_string(uint8_t);
static void __trace_int8_to_string(int8_t);
static void __trace_float_to_string(float fValue);
static void __trace_double_to_string(double fValue);
static void __trace_byte_stream(uint8_t *, uint_fast16_t);
static void __trace_hword_stream(uint16_t *, uint_fast16_t);
static void __trace_word_stream(uint32_t *, uint_fast16_t);

/*============================ GLOBAL VARIABLES ==============================*/
const i_trace_t TRACE = {
    .Init =             &__trace_init,
    .ToString = {
        .String =       &__trace_string,
        .UInt32 =       &__trace_uint32_to_string,
        .Int32 =        &__trace_int32_to_string,
        .UInt16 =       &__trace_uint16_to_string,
        .Int16 =        &__trace_int16_to_string,
        .UInt8 =        &__trace_uint8_to_string,
        .Int8 =         &__trace_int8_to_string,
        .Float =        &__trace_float_to_string,
        .Double =       &__trace_double_to_string,
        .Bytes =        &__trace_byte_stream,
        .HWords =       &__trace_hword_stream,
        .Words =        &__trace_word_stream,
    },
};

/*============================ IMPLEMENTATION ================================*/

static void __trace_init(trace_cfg_t *ptCFG)
{
    
}

static void __trace_string(const char * pchString)
{
    printf("%s", pchString);
}

static void __trace_uint32_to_string(uint32_t wValue)
{
    printf("0x%08x", wValue);
}

static void __trace_int32_to_string(int32_t nValue)
{
    printf("%d", nValue);
}

static void __trace_uint16_to_string(uint16_t hwValue)
{
    printf("0x%04x", hwValue);
}

static void __trace_int16_to_string(int16_t iValue)
{
    printf("%d", iValue);
}

static void __trace_uint8_to_string(uint8_t chValue)
{
    printf("0x%02x", chValue);
}

static void __trace_int8_to_string(int8_t cValue)
{
    printf("%d", cValue);
}

static void __trace_float_to_string(float fValue)
{
    printf("%f", fValue);
}

static void __trace_double_to_string(double dfValue)
{
    printf("%f", dfValue);
}

#define __OUTPUT_STREAM(__TYPE, __ADDR, __SIZE, __FORMAT_STR, __BLANK)          \
    do {                                                                        \
        int_fast8_t n = 0;                                                      \
        uint_fast16_t __Size = (__SIZE);                                        \
        __TYPE *pSrc = (__TYPE *)(__ADDR);                                      \
        uint8_t *pchSrc;                                                        \
        while(__Size > (TRACE_DISPLAY_WIDTH / sizeof(__TYPE))) {                \
            for (n = 0; n < (TRACE_DISPLAY_WIDTH / sizeof(__TYPE)); n++) {      \
                printf(__FORMAT_STR, pSrc[n]);                                  \
            }                                                                   \
                                                                                \
            printf("\t");                                                       \
            pchSrc = (uint8_t *)pSrc;                                           \
                                                                                \
            for (n = 0; n < TRACE_DISPLAY_WIDTH; n++) {                         \
                char c = pchSrc[n];                                             \
                if (c >= 127 || c < 32) {                                       \
                    c = '.';                                                    \
                }                                                               \
                printf("%c", c);                                                \
            }                                                                   \
                                                                                \
            printf("\r\n");                                                     \
                                                                                \
            pSrc += (TRACE_DISPLAY_WIDTH / sizeof(__TYPE));                     \
            __Size -= (TRACE_DISPLAY_WIDTH / sizeof(__TYPE));                   \
        }                                                                       \
                                                                                \
        if (__Size > 0) {                                                       \
            for (n = 0; n < __Size; n++) {                                      \
                printf(__FORMAT_STR, pSrc[n]);                                  \
            }                                                                   \
            for (   n = 0;                                                      \
                    n < (TRACE_DISPLAY_WIDTH/sizeof(__TYPE) - __Size);          \
                    n++) {                                                      \
                printf(__BLANK);                                                \
            }                                                                   \
            printf("\t");                                                       \
            pchSrc = (uint8_t *)pSrc;                                           \
            for (n = 0; n < __Size * sizeof(__TYPE); n++) {                     \
                char c = pchSrc[n];                                             \
                if (c > 127 || c < 32) {                                        \
                    c = '.';                                                    \
                }                                                               \
                printf("%c", c);                                                \
            }                                                                   \
                                                                                \
            printf("\r\n");                                                     \
        }                                                                       \
    } while(0)

static void __trace_word_stream(uint32_t *pwStream, uint_fast16_t hwSize)
{
    __OUTPUT_STREAM(uint32_t, pwStream, hwSize, "%08X ", "         ");
}

static void __trace_hword_stream(uint16_t *phwStream, uint_fast16_t hwSize)
{
    __OUTPUT_STREAM(uint16_t, phwStream, hwSize, "%04X ", "     ");
}

static void __trace_byte_stream(uint8_t *pchStream, uint_fast16_t hwSize)
{
    __OUTPUT_STREAM(uint8_t, pchStream, hwSize, "%02X ", "   ");
}

/* EOF */