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

/* clang-format off */

/*
 * Don't modify the "pika_config_valid.h" file!
 * If you want to change the config, create "pika_config.h",
 * then #define PIKA_CONFIG_ENABLE in the Compiler Options.
 * To see more:
 * https://pikadoc.readthedocs.io/en/latest/%E4%BC%98%E5%8C%96%E5%86%85%E5%AD%98%E5%8D%A0%E7%94%A8%E3%80%81%E9%85%8D%E7%BD%AE%20libc.html
 */

    /* optimize options */
        #define PIKA_OPTIMIZE_SIZE 0
        #define PIKA_OPTIMIZE_SPEED 1

    /* syntax support level */
        #define PIKA_SYNTAX_LEVEL_MINIMAL 0
        #define PIKA_SYNTAX_LEVEL_MAXIMAL 1

    /* use user config */
    #ifdef PIKA_CONFIG_ENABLE
        #include "pika_config.h"
    #endif

    /* default optimize */
    #ifndef PIKA_OPTIMIZE
        #define PIKA_OPTIMIZE PIKA_OPTIMIZE_SIZE
    #endif

    /* default syntax support level */
    #ifndef PIKA_SYNTAX_LEVEL
        #define PIKA_SYNTAX_LEVEL PIKA_SYNTAX_LEVEL_MAXIMAL
    #endif

    /* auto config for optimize */
    #if PIKA_OPTIMIZE == PIKA_OPTIMIZE_SIZE
        #ifndef PIKA_METHOD_CACHE_ENABLE
            #define PIKA_METHOD_CACHE_ENABLE 0
        #endif

    #elif PIKA_OPTIMIZE == PIKA_OPTIMIZE_SPEED
        #ifndef PIKA_METHOD_CACHE_ENABLE
            #define PIKA_METHOD_CACHE_ENABLE 1
        #endif
    #endif
    
    /* auto config for syntax level */
    #if PIKA_SYNTAX_LEVEL == PIKA_SYNTAX_LEVEL_MINIMAL
        #ifndef PIKA_SYNTEX_ITEM_SLICE_ENABLE
            #define PIKA_SYNTEX_ITEM_SLICE_ENABLE 0
        #endif

        #ifndef PIKA_BUILTIN_LIST_ENABLE
            #define PIKA_BUILTIN_LIST_ENABLE 0
        #endif

        #ifndef PIKA_BUILTIN_DICT_ENABLE
            #define PIKA_BUILTIN_DICT_ENABLE 0
        #endif

        #ifndef PIKA_SYNTEX_ITEM_FORMAT_ENABLE
            #define PIKA_SYNTEX_ITEM_FORMAT_ENABLE 0
        #endif

		#ifndef PIKA_STD_DEVICE_UNIX_TIME_ENABLE
		    #define PIKA_STD_DEVICE_UNIX_TIME_ENABLE 0
        #endif

    #elif PIKA_SYNTAX_LEVEL == PIKA_SYNTAX_LEVEL_MAXIMAL
        #ifndef PIKA_SYNTEX_ITEM_SLICE_ENABLE
            #define PIKA_SYNTEX_ITEM_SLICE_ENABLE 1
        #endif

        #ifndef PIKA_BUILTIN_LIST_ENABLE
            #define PIKA_BUILTIN_LIST_ENABLE 1 
        #endif

        #ifndef PIKA_BUILTIN_DICT_ENABLE
            #define PIKA_BUILTIN_DICT_ENABLE 1
        #endif

        #ifndef PIKA_SYNTEX_ITEM_FORMAT_ENABLE
            #define PIKA_SYNTEX_ITEM_FORMAT_ENABLE 1
        #endif

		#ifndef PIKA_STD_DEVICE_UNIX_TIME_ENABLE
		    #define PIKA_STD_DEVICE_UNIX_TIME_ENABLE 1
        #endif
    #endif

    /* default configuration  */
    #ifndef PIKA_LINE_BUFF_SIZE
        #define PIKA_LINE_BUFF_SIZE 128
    #endif

    #ifndef PIKA_SPRINTF_BUFF_SIZE
        #define PIKA_SPRINTF_BUFF_SIZE 256
    #endif

    #ifndef PIKA_STACK_BUFF_SIZE
        #define PIKA_STACK_BUFF_SIZE 256
    #endif

    #ifndef PIKA_NAME_BUFF_SIZE
        #define PIKA_NAME_BUFF_SIZE 32
    #endif

    #ifndef PIKA_PATH_BUFF_SIZE
        #define PIKA_PATH_BUFF_SIZE 64
    #endif

    #ifndef PIKA_BYTES_DEFAULT_SIZE
        #define PIKA_BYTES_DEFAULT_SIZE 64
    #endif

    #ifndef PIKA_ARG_ALIGN_ENABLE
        #define PIKA_ARG_ALIGN_ENABLE 1
    #endif

    #ifndef PIKA_METHOD_CACHE_ENABLE
        #define PIKA_METHOD_CACHE_ENABLE 0
    #endif

    #ifndef PIKA_BUILTIN_LIST_ENABLE
        #define PIKA_BUILTIN_LIST_ENABLE 0
    #endif

    #ifndef PIKA_BUILTIN_DICT_ENABLE
        #define PIKA_BUILTIN_DICT_ENABLE 0
    #endif

    #ifndef PIKA_READ_FILE_BUFF_SIZE
        #define PIKA_READ_FILE_BUFF_SIZE 0x10000
    #endif

    #ifndef PIKA_INIT_STRING_ENABLE
        #define PIKA_INIT_STRING_ENABLE 0
    #endif
    
    #ifndef PIKA_SYNTEX_ITEM_SLICE_ENABLE
        #define PIKA_SYNTEX_ITEM_SLICE_ENABLE 1
    #endif

    #ifndef PIKA_SYNTEX_ITEM_FORMAT_ENABLE
        #define PIKA_SYNTEX_ITEM_FORMAT_ENABLE 1
    #endif

    #ifndef PIKA_PLOOC_ENABLE
        #define PIKA_PLOOC_ENABLE 0
    #endif

    #ifndef PIKA_STD_DEVICE_UNIX_TIME_ENABLE
        #define PIKA_STD_DEVICE_UNIX_TIME_ENABLE 1
    #endif

    #ifndef PIKA_POOL_ENABLE
        #define PIKA_POOL_ENABLE 0
    #endif

    #ifndef PIKA_POOL_SIZE
        #define PIKA_POOL_SIZE 0x4000
    #endif

    #ifndef PIKA_POOL_ALIGN
        #define PIKA_POOL_ALIGN 8
    #endif

    #ifndef PIKA_ASSERT_ENABLE
        #define PIKA_ASSERT_ENABLE 0
    #endif
    

    /* configuration validation */

#endif
