/*
 * This file is part of the PikaPython project.
 * http://github.com/pikastech/pikapython
 *
 * MIT License
 *
 * Copyright (c) 2021 lyon liang6516@outlook.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef __PIKA_CFG_VALID_H__
#define __PIKA_CFG_VALID_H__

/* clang-format off */

/*
 * Don't modify the "pika_config_valid.h" file!
 * If you want to change the config, create "pika_config.h",
 * then #define PIKA_CONFIG_ENABLE in the Compiler Options.
 * To see more:
 * https://pikadoc-en.readthedocs.io/en/latest/%E4%BC%98%E5%8C%96%E5%86%85%E5%AD%98%E5%8D%A0%E7%94%A8%E3%80%81%E9%85%8D%E7%BD%AE%20libc.html
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

    /* default pika_nano_enabled */
    #ifndef PIKA_NANO_ENABLE
        #define PIKA_NANO_ENABLE 0
    #endif

    #if PIKA_NANO_ENABLE
        #ifndef PIKA_OPTIMIZE
            #define PIKA_OPTIMIZE PIKA_OPTIMIZE_SIZE
        #endif

        /* default syntax support level */
        #ifndef PIKA_SYNTAX_LEVEL
            #define PIKA_SYNTAX_LEVEL PIKA_SYNTAX_LEVEL_MINIMAL
        #endif

        #ifndef PIKA_STRING_UTF8_ENABLE
            #define PIKA_STRING_UTF8_ENABLE 0
        #endif

        #ifndef PIKA_SHELL_FILTER_ENABLE
            #define PIKA_SHELL_FILTER_ENABLE 0
        #endif

        #ifdef PIKA_TYPE_FULL_FEATURE_ENABLE
            #define PIKA_TYPE_FULL_FEATURE_ENABLE 0
        #endif

        #ifndef PIKA_EVENT_THREAD_ENABLE
            #define PIKA_EVENT_THREAD_ENABLE 0
        #endif

        #ifndef PIKA_GC_MARK_SWEEP_ENABLE
            #define PIKA_GC_MARK_SWEEP_ENABLE 0
        #endif

        #ifndef PIKA_SHELL_HISTORY_ENABLE
            #define PIKA_SHELL_HISTORY_ENABLE 0
        #endif

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
        #ifndef PIKA_ARG_CACHE_ENABLE
            #define PIKA_ARG_CACHE_ENABLE 1
        #endif
    #endif
    
    /* auto config for syntax level */
    #if PIKA_SYNTAX_LEVEL == PIKA_SYNTAX_LEVEL_MINIMAL
        #ifndef PIKA_SYNTAX_SLICE_ENABLE
            #define PIKA_SYNTAX_SLICE_ENABLE 0
        #endif

        #ifndef PIKA_BUILTIN_STRUCT_ENABLE
            #define PIKA_BUILTIN_STRUCT_ENABLE 0
        #endif

        #ifndef PIKA_SYNTAX_FORMAT_ENABLE
            #define PIKA_SYNTAX_FORMAT_ENABLE 0
        #endif

        #ifndef PIKA_STD_DEVICE_UNIX_TIME_ENABLE
            #define PIKA_STD_DEVICE_UNIX_TIME_ENABLE 0
        #endif
        
        #ifndef PIKA_SYNTAX_EXCEPTION_ENABLE
            #define PIKA_SYNTAX_EXCEPTION_ENABLE 0
        #endif
        
        #ifndef PIKA_SYNTAX_IMPORT_EX_ENABLE
            #define PIKA_SYNTAX_IMPORT_EX_ENABLE 0
        #endif

        #ifndef PIKA_EVENT_ENABLE
            #define PIKA_EVENT_ENABLE 0
        #endif

        #ifndef PIKA_FILEIO_ENABLE
            #define PIKA_FILEIO_ENABLE 0
        #endif

        #ifndef PIKA_EXEC_ENABLE
            #define PIKA_EXEC_ENABLE 0
        #endif

    #elif PIKA_SYNTAX_LEVEL == PIKA_SYNTAX_LEVEL_MAXIMAL
        #ifndef PIKA_SYNTAX_SLICE_ENABLE
            #define PIKA_SYNTAX_SLICE_ENABLE 1
        #endif

        #ifndef PIKA_BUILTIN_STRUCT_ENABLE
            #define PIKA_BUILTIN_STRUCT_ENABLE 1
        #endif

        #ifndef PIKA_SYNTAX_FORMAT_ENABLE
            #define PIKA_SYNTAX_FORMAT_ENABLE 1
        #endif

        #ifndef PIKA_STD_DEVICE_UNIX_TIME_ENABLE
            #define PIKA_STD_DEVICE_UNIX_TIME_ENABLE 1
        #endif
        
        #ifndef PIKA_SYNTAX_EXCEPTION_ENABLE
            #define PIKA_SYNTAX_EXCEPTION_ENABLE 1
        #endif
        
        #ifndef PIKA_SYNTAX_IMPORT_EX_ENABLE
            #define PIKA_SYNTAX_IMPORT_EX_ENABLE 1
        #endif

        #ifndef PIKA_EVENT_ENABLE
            #define PIKA_EVENT_ENABLE 1
        #endif

        #ifndef PIKA_FILEIO_ENABLE
            #define PIKA_FILEIO_ENABLE 1
        #endif

        #ifndef PIKA_EXEC_ENABLE
            #define PIKA_EXEC_ENABLE 1
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
        #define PIKA_STACK_BUFF_SIZE 512
    #endif

    #ifndef PIKA_NAME_BUFF_SIZE
        #define PIKA_NAME_BUFF_SIZE 32
    #endif

    #ifndef PIKA_PATH_BUFF_SIZE
        #define PIKA_PATH_BUFF_SIZE 96
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

    #ifndef PIKA_BUILTIN_STRUCT_ENABLE
        #define PIKA_BUILTIN_STRUCT_ENABLE 0
    #endif

    #ifndef PIKA_READ_FILE_BUFF_SIZE
        #define PIKA_READ_FILE_BUFF_SIZE 1024 * 10
    #endif

    #ifndef PIKA_INIT_STRING_ENABLE
        #define PIKA_INIT_STRING_ENABLE 0
    #endif
    
    #ifndef PIKA_SYNTAX_SLICE_ENABLE
        #define PIKA_SYNTAX_SLICE_ENABLE 1
    #endif

    #ifndef PIKA_SYNTAX_FORMAT_ENABLE
        #define PIKA_SYNTAX_FORMAT_ENABLE 1
    #endif

    #ifndef PIKA_SYNTAX_EXCEPTION_ENABLE
        #define PIKA_SYNTAX_EXCEPTION_ENABLE 1
    #endif

    #ifndef PIKA_SYNTAX_IMPORT_EX_ENABLE
        #define PIKA_SYNTAX_IMPORT_EX_ENABLE 1
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
        #define PIKA_ASSERT_ENABLE 1
    #endif

    #ifndef PIKA_EVENT_ENABLE
        #define PIKA_EVENT_ENABLE 1
    #endif

    #ifndef PIKA_DEBUG_ENABLE
        #define PIKA_DEBUG_ENABLE 0
    #endif

    #ifndef PIKA_DEBUG_BREAK_POINT_MAX
        #define PIKA_DEBUG_BREAK_POINT_MAX 8
    #endif

    #ifndef PIKA_THERAD_NUM_MAX
        #define PIKA_THERAD_NUM_MAX 16
    #endif

    #ifndef PIKA_FILEIO_ENABLE
        #define PIKA_FILEIO_ENABLE 1
    #endif

    #ifndef PIKA_ARG_NUM_MAX
        #define PIKA_ARG_NUM_MAX 16
    #endif

    #ifndef PIKA_MATH_ENABLE
        #define PIKA_MATH_ENABLE 0
    #endif

    #ifndef PIKA_REGIST_SIZE
        #define PIKA_REGIST_SIZE 10
    #endif

    #ifndef PIKA_ARG_BUFF_SIZE
        #define PIKA_ARG_BUFF_SIZE 8
    #endif

    #ifndef PIKA_INSTRUCT_HOOK_ENABLE
        #define PIKA_INSTRUCT_HOOK_ENABLE 0
    #endif

    #ifndef PIKA_INSTRUCT_HOOK_PERIOD
        #define PIKA_INSTRUCT_HOOK_PERIOD 50
    #endif

    #ifndef PIKA_INSTRUCT_YIELD_PERIOD
        #define PIKA_INSTRUCT_YIELD_PERIOD 1
    #endif
    
    #ifndef PIKA_INSTRUCT_EXTENSION_ENABLE
        #define PIKA_INSTRUCT_EXTENSION_ENABLE 1
    #endif
    
    #ifndef PIKA_INSTRUCT_SIGNATURE_DICT_COUNT
        #define PIKA_INSTRUCT_SIGNATURE_DICT_COUNT 1
    #endif

    #ifndef PIKA_EXEC_ENABLE
        #define PIKA_EXEC_ENABLE 1
    #endif

    /* support for UTF-8 in PikaStdData_String */
    #ifndef PIKA_STRING_UTF8_ENABLE
        #define PIKA_STRING_UTF8_ENABLE 1
    #endif
    
    #ifndef PIKA_PRINT_LLD_ENABLE
        #define PIKA_PRINT_LLD_ENABLE 1
    #endif

    #ifndef PIKA_FLOAT_TYPE_DOUBLE
        #define PIKA_FLOAT_TYPE_DOUBLE 1
    #endif

    #ifndef PIKA_ARG_CACHE_ENABLE
        #define PIKA_ARG_CACHE_ENABLE 0
    #endif

    #ifndef PIKA_ARG_CACHE_POOL_SIZE
        #define PIKA_ARG_CACHE_POOL_SIZE 32
    #endif

    #ifndef PIKA_ARG_CACHE_SIZE
        #define PIKA_ARG_CACHE_SIZE sizeof(Arg) + 16
    #endif

    #ifndef PIKA_SHELL_SAVE_FILE_ENABLE
        #define PIKA_SHELL_SAVE_FILE_ENABLE 0
    #endif

    #ifndef PIKA_SHELL_SAVE_FILE_PATH
        #define PIKA_SHELL_SAVE_FILE_PATH "app.py"
    #endif

    #ifndef PIKA_SHELL_SAVE_BYTECODE_ENABLE
        #define PIKA_SHELL_SAVE_BYTECODE_ENABLE 0
    #endif

    #ifndef PIKA_SHELL_SAVE_BYTECODE_PATH
        #define PIKA_SHELL_SAVE_BYTECODE_PATH "app.py.o"
    #endif

    #ifndef PIKA_SHELL_SAVE_APP_ENABLE
        #define PIKA_SHELL_SAVE_APP_ENABLE 0
    #endif

    #ifndef PIKA_SHELL_SAVE_APP_PATH
        #define PIKA_SHELL_SAVE_APP_PATH "app.pika"
    #endif

    #ifndef PIKA_SHELL_FILTER_ENABLE
        #define PIKA_SHELL_FILTER_ENABLE 1
    #endif

    #ifndef PIKA_SHELL_FILTER_FIFO_SIZE
        #define PIKA_SHELL_FILTER_FIFO_SIZE    32
    #endif

    #ifndef PIKA_EVENT_LIST_SIZE
        #define PIKA_EVENT_LIST_SIZE 16
    #endif

    #ifndef PIKA_EVENT_PICKUP_MAX
        #define PIKA_EVENT_PICKUP_MAX 2
    #endif
        
    #ifndef PIKA_BYTECODE_ONLY_ENABLE
        #define PIKA_BYTECODE_ONLY_ENABLE 0
    #endif

    #ifndef PIKA_OLD_API_ENABLE
        #define PIKA_OLD_API_ENABLE 1
    #endif

    #ifndef PIKA_FREERTOS_ENABLE
        #define PIKA_FREERTOS_ENABLE 0
    #endif

    #ifndef PIKA_RTTHREAD_ENABLE
        #define PIKA_RTTHREAD_ENABLE 0
    #endif

    #ifndef PIKA_ZEUSOS_ENABLE
        #define PIKA_ZEUSOS_ENABLE 0
    #endif
    
    #ifndef PIKA_LINUX_COMPATIBLE
        #define PIKA_LINUX_COMPATIBLE 0
    #endif

    #ifndef PIKA_LINUX_BASH_ENABLE
        #define PIKA_LINUX_BASH_ENABLE 1
    #endif

    #ifndef PIKA_LWIP_ENABLE
        #define PIKA_LWIP_ENABLE 0
    #endif

    #ifndef PIKA_SHELL_NO_NEWLINE
        #define PIKA_SHELL_NO_NEWLINE 0
    #endif

    #ifndef PIKA_SETJMP_ENABLE
        #define PIKA_SETJMP_ENABLE 0
    #endif

    #ifndef PIKA_JMP_BUF_LIST_SIZE
        #define PIKA_JMP_BUF_LIST_SIZE 16
    #endif

    #ifndef PIKA_THREAD_STACK_SIZE
        #define PIKA_THREAD_STACK_SIZE 2048
    #endif

    #ifndef PIKA_EVENT_THREAD_STACK_SIZE
        #define PIKA_EVENT_THREAD_STACK_SIZE PIKA_THREAD_STACK_SIZE
    #endif

    #ifndef PIKA_THREAD_MALLOC_STACK_ENABLE
        #define PIKA_THREAD_MALLOC_STACK_ENABLE 0
    #endif

    #ifndef PIKA_THREAD_PRIO
        #define PIKA_THREAD_PRIO 5
    #endif

    #ifndef PIKA_THREAD_TICK
        #define PIKA_THREAD_TICK 50
    #endif

    #ifndef PIKA_TYPE_FULL_FEATURE_ENABLE
        #define PIKA_TYPE_FULL_FEATURE_ENABLE 1
    #endif

    #ifndef PIKA_EVENT_THREAD_ENABLE
        #define PIKA_EVENT_THREAD_ENABLE 1
    #endif

    #ifndef PIKA_GC_MARK_SWEEP_ENABLE
        #define PIKA_GC_MARK_SWEEP_ENABLE 0
    #endif

    #ifndef PIKA_GC_MARK_SWEEP_THRESHOLD
        #define PIKA_GC_MARK_SWEEP_THRESHOLD 20
    #endif

    #ifndef PIKA_KERNAL_DEBUG_ENABLE
        #define PIKA_KERNAL_DEBUG_ENABLE 0
    #endif

    #ifndef PIKA_SHELL_HISTORY_ENABLE
        #define PIKA_SHELL_HISTORY_ENABLE 1
    #endif

    #ifndef PIKA_SHELL_HISTORY_NUM
        #define PIKA_SHELL_HISTORY_NUM 5
    #endif

    #ifndef PIKA_UNBUFFERED_ENABLE
        #define PIKA_UNBUFFERED_ENABLE 0
    #endif

    #ifndef PIKA_SHELL_NO_ECHO
        #define PIKA_SHELL_NO_ECHO 0
    #endif

    #ifndef PIKA_PLATFORM_NO_WEAK
        #define PIKA_PLATFORM_NO_WEAK 0
    #endif

    #ifndef PIKA_WIN_PTHREAD_ENABLE
        #define PIKA_WIN_PTHREAD_ENABLE 0
    #endif

    /* configuration validation */

#endif
#ifdef __cplusplus
}
#endif
