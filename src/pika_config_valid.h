/*
 * This file is part of the PikaScript project.
 * http://github.com/pikastech/pikascript
 *
 * MIT License
 *
 * Copyright (c) 2021 lyon 李昂 liang6516@outlook.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef __PIKA_CFG_VALID_H__
#define __PIKA_CFG_VALID_H__

    /* default configuration  */
        #define PIKA_LINE_BUFF_SIZE 128
        #define PIKA_SPRINTF_BUFF_SIZE 256
        #define PIKA_STACK_BUFF_SIZE 256
        #define PIKA_NAME_BUFF_SIZE 32
        #define PIKA_PATH_BUFF_SIZE 64
        #define PIKA_BYTES_DEFAULT_SIZE 64
        #define PIKA_ARG_ALIGN_ENABLE 1
        #define PIKA_METHOD_CACHE_ENABLE 0
        #define PIKA_BUILTIN_LIST_ENABLE 0
        #define PIKA_BUILTIN_DICT_ENABLE 0
        #define PIKA_READ_FILE_BUFF_SIZE 0x10000
        #define PIKA_INIT_STRING_ENABLE 0
        #define PIKA_SYNTEX_ITEM_SLICE_ENABLE 1
        #define PIKA_PLOOC_ENABLE 0
    
    /* optimize options */
        #define PIKA_OPTIMIZE_SIZE 0
        #define PIKA_OPTIMIZE_SPEED 1

    /* syntax support level */
        #define PIKA_SYNTAX_LEVEL_MINIMAL 0
        #define PIKA_SYNTAX_LEVEL_MAXIMAL 1

    /* default optimize */
        #define PIKA_OPTIMIZE PIKA_OPTIMIZE_SIZE
    /* default syntax support level */
        #define PIKA_SYNTAX_LEVEL PIKA_SYNTAX_LEVEL_MAXIMAL

    /* use user config */
    #ifdef PIKA_CONFIG_ENABLE
        #include "pika_config.h"
    #endif

    /* config for size optimize */
    #if PIKA_OPTIMIZE == PIKA_OPTIMIZE_SIZE
        #undef PIKA_METHOD_CACHE_ENABLE
        #define PIKA_METHOD_CACHE_ENABLE 0

    /* config for speed optimize */
    #elif PIKA_OPTIMIZE == PIKA_OPTIMIZE_SPEED
        #undef PIKA_METHOD_CACHE_ENABLE
        #define PIKA_METHOD_CACHE_ENABLE 1
    #endif
    
    /* config for syntax level */
    #if PIKA_SYNTAX_LEVEL == PIKA_SYNTAX_LEVEL_MINIMAL
        #undef PIKA_SYNTEX_ITEM_SLICE_ENABLE
        #define PIKA_SYNTEX_ITEM_SLICE_ENABLE 0
        #undef PIKA_BUILTIN_LIST_ENABLE
        #define PIKA_BUILTIN_LIST_ENABLE 0
        #undef PIKA_BUILTIN_DICT_ENABLE
        #define PIKA_BUILTIN_DICT_ENABLE 0
    #elif PIKA_SYNTAX_LEVEL == PIKA_SYNTAX_LEVEL_MAXIMAL
        #undef PIKA_SYNTEX_ITEM_SLICE_ENABLE
        #define PIKA_SYNTEX_ITEM_SLICE_ENABLE 1
        #undef PIKA_BUILTIN_LIST_ENABLE
        #define PIKA_BUILTIN_LIST_ENABLE 1
        #undef PIKA_BUILTIN_DICT_ENABLE
        #define PIKA_BUILTIN_DICT_ENABLE 1
    #endif

    /* configuration validation */

#endif
