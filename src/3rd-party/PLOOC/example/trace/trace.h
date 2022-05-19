/*****************************************************************************
 *   Copyright(C)2009-2019 by GorgonMeducer<embedded_zhuoran@hotmail.com>    *
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


#ifndef __PLOOC_EXAMPLE_TRACE_H__
#define __PLOOC_EXAMPLE_TRACE_H__

/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include <stdbool.h>
#include "plooc.h"

/*============================ MACROS ========================================*/

#if     defined(__clang__)                                                      \
    ||  defined(__IAR_SYSTEMS_ICC__)                                            \
    ||  defined(__ARMCC_VERSION)

#define __TRACE_TOSTRING_1(__OP1)                                               \
            _Generic((__OP1),                                                   \
                    const char *: TRACE.ToString.String,                        \
                          char *: TRACE.ToString.String,                        \
                        int8_t  : TRACE.ToString.Int8,                          \
                        uint8_t : TRACE.ToString.UInt8,                         \
                        int16_t : TRACE.ToString.Int16,                         \
                        uint16_t: TRACE.ToString.UInt16,                        \
                        int32_t : TRACE.ToString.Int32,                         \
                        uint32_t: TRACE.ToString.UInt32,                        \
                        float   : TRACE.ToString.Float,                         \
                        double  : TRACE.ToString.Double                         \
                    )(__OP1)
                    
#define __TRACE_TOSTRING_2(__OP1, __OP2)                                        \
            _Generic((__OP1),                                                   \
                     uint32_t * : TRACE.ToString.Words,                         \
                     uint16_t * : TRACE.ToString.HWords,                        \
                     uint8_t *  : TRACE.ToString.Bytes,                         \
                      int32_t * : TRACE.ToString.Words,                         \
                      int16_t * : TRACE.ToString.HWords,                        \
                      int8_t *  : TRACE.ToString.Bytes                          \
                    )((__OP1), (__OP2))

#else /* for GCC */
#define __TRACE_TOSTRING_1(__OP1)                                               \
            _Generic((__OP1),                                                   \
                    const char *: TRACE.ToString.String,                        \
                          char *: TRACE.ToString.String,                        \
                        int8_t  : TRACE.ToString.Int8,                          \
                        uint8_t : TRACE.ToString.UInt8,                         \
                        int16_t : TRACE.ToString.Int16,                         \
                        uint16_t: TRACE.ToString.UInt16,                        \
                        int32_t : TRACE.ToString.Int32,                         \
                        int     : TRACE.ToString.Int32, /* for stupid gcc */    \
                        uint32_t: TRACE.ToString.UInt32,                        \
                        float   : TRACE.ToString.Float,                         \
                        double  : TRACE.ToString.Double                         \
                    )(__OP1)
                    
#define __TRACE_TOSTRING_2(__OP1, __OP2)                                        \
            _Generic((__OP1),                                                   \
                     uint32_t * : TRACE.ToString.Words,                         \
                     uint16_t * : TRACE.ToString.HWords,                        \
                     uint8_t *  : TRACE.ToString.Bytes,                         \
                      int32_t * : TRACE.ToString.Words,                         \
                      int *     : TRACE.ToString.Words, /* for stupid gcc */    \
                      int16_t * : TRACE.ToString.HWords,                        \
                      int8_t *  : TRACE.ToString.Bytes                          \
                    )((__OP1), (__OP2))
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define TRACE_TOSTR(...)    __PLOOC_EVAL(   __TRACE_TOSTRING_,                  \
                                            __VA_ARGS__)(__VA_ARGS__)

/*============================ TYPES =========================================*/

typedef int trace_low_level_io_write(uintptr_t pStream, uint_fast16_t hwSize);

typedef struct trace_cfg_t {
    trace_low_level_io_write *fnWriteIO;
} trace_cfg_t;

//! \name interface i_byte_queue_t
//! @{
def_interface(i_trace_t)
    void (*Init)     (trace_cfg_t *ptCFG);
    struct {
        void (*Double)  (double dfValue);
        void (*Float)   (float fValue);
        void (*UInt32)  (uint32_t wValue);
        void (*Int32)   (int32_t nValue);
        void (*UInt16)  (uint16_t hwValue);
        void (*Int16)   (int16_t iValue);
        void (*UInt8)   (uint8_t chValue);
        void (*Int8)    (int8_t cValue);
        void (*String)  (const char * pchString);
   
        void (*Words)   (uint32_t *pwStream,  uint_fast16_t hwSize);
        void (*HWords)  (uint16_t *phwStream, uint_fast16_t hwSize);
        void (*Bytes)   (uint8_t  *pchStream, uint_fast16_t hwSize);
    }ToString;
end_def_interface(i_trace_t) /*do not remove this for forward compatibility */
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
extern const i_trace_t TRACE;
/*============================ PROTOTYPES ====================================*/
#endif
/* EOF */
