/****************************************************************************
*  Copyright 2021 Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)       *
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
#include "cmsis_compiler.h"
#include "RTE_Components.h"
#include <stdio.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
extern int _read(int handle, char *buffer, int length);
extern int _write(int handle, char *buffer, int length);

/*============================ IMPLEMENTATION ================================*/
int __real_vprintf(const char *format, __va_list va) 
{
    extern int $Super$$vprintf(const char *format, __va_list va);
    
    return $Super$$vprintf(format, va);
}


/*----------------------------------------------------------------------------*
 * bridge the Arm Compiler's stdio and the pico-sdk's stdio                   *
 *----------------------------------------------------------------------------*/

#if     !defined(LIB_PICO_STDIO_SEMIHOSTING)                                    \
    ||  (defined(LIB_PICO_STDIO_SEMIHOSTING) && !LIB_PICO_STDIO_SEMIHOSTING)
__attribute__((weak))
int stdin_getchar(void)
{
    /*! \note If you don't want to use pico-sdk stdio, then you can implement 
     *!       function by yourself in other c source code. Your scanf will work
     *!       directly.
     *!       by default, we use this function to bridge the _read implemented 
     *!       in stdio.c of pico-sdk
     */

    int byte;
    _read(0, (char *)&byte, 1);
    return byte;
}

__attribute__((weak))
int stdout_putchar(int ch)
{
    /*! \note If you don't want to use pico-sdk stdio, then you can implement 
     *!       function by yourself in other c source code. Your printf will work
     *!       directly.
     *!       by default, we use this function to bridge the _write implemented 
     *!       in stdio.c of pico-sdk
     */
    
    return _write(1, (char *)&ch, 1);
}
#else
/*! \note If you want to use semihosting (currently only Arm-DS supports it), 
 *!       pleae open the RTE configuration and unselect the Compiler->I/O->STDIN
 *!       and Compiler->I/O->STDOUT
 */
#endif



#if     defined(__IS_COMPILER_ARM_COMPILER_6__)                                 \
    &&  (   !defined(LIB_PICO_STDIO_SEMIHOSTING)                                \
        ||  (defined(LIB_PICO_STDIO_SEMIHOSTING) && !LIB_PICO_STDIO_SEMIHOSTING))
__asm(".global __use_no_semihosting\n\t");
#   ifndef __MICROLIB
__asm(".global __ARM_use_no_argv\n\t");
#   endif

#if !defined(RTE_Compiler_IO_STDOUT)
#if defined(__MICROLIB)
_ARMABI_NORETURN 
void __aeabi_assert(const char *chCond, const char *chLine, int wErrCode) 
{
    UNUSED_PARAM(chCond);
    UNUSED_PARAM(chLine);
    UNUSED_PARAM(wErrCode);
    while(1) {
        __NOP();
    }
}
#endif
#endif

void _ttywrch(int ch)
{
    UNUSED_PARAM(ch);
}

#if !defined(RETARGET_SYS)
#include <rt_sys.h>

FILEHANDLE $Sub$$_sys_open(const char *name, int openmode)
{
    UNUSED_PARAM(name);
    UNUSED_PARAM(openmode);
    return 0;
}
#endif


__NO_RETURN
void _sys_exit(int ret)
{
    UNUSED_PARAM(ret);
    while(1) {}
}

#endif
