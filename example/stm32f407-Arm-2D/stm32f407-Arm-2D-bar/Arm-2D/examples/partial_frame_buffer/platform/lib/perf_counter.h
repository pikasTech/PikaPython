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



#ifndef __PERFORMANCE_COUNTER_H__
#define __PERFORMANCE_COUNTER_H__

/*============================ INCLUDES ======================================*/
#include <stdbool.h>
#include <stdint.h>

/*============================ MACROS ========================================*/

//! \name The macros to identify the compiler
//! @{

//! \note for IAR
#ifdef __IS_COMPILER_IAR__
#   undef __IS_COMPILER_IAR__
#endif
#if defined(__IAR_SYSTEMS_ICC__)
#   define __IS_COMPILER_IAR__                  1
#endif

//! \note for arm compiler 5
#ifdef __IS_COMPILER_ARM_COMPILER_5__
#   undef __IS_COMPILER_ARM_COMPILER_5__
#endif
#if ((__ARMCC_VERSION >= 5000000) && (__ARMCC_VERSION < 6000000))
#   define __IS_COMPILER_ARM_COMPILER_5__       1
#endif
//! @}

//! \note for arm compiler 6
#ifdef __IS_COMPILER_ARM_COMPILER_6__
#   undef __IS_COMPILER_ARM_COMPILER_6__
#endif
#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#   define __IS_COMPILER_ARM_COMPILER_6__       1
#endif

#ifdef __IS_COMPILER_LLVM__
#   undef  __IS_COMPILER_LLVM__
#endif
#if defined(__clang__) && !__IS_COMPILER_ARM_COMPILER_6__
#   define __IS_COMPILER_LLVM__                 1
#else
//! \note for gcc
#   ifdef __IS_COMPILER_GCC__
#       undef __IS_COMPILER_GCC__
#   endif
#   if defined(__GNUC__) && !(  defined(__IS_COMPILER_ARM_COMPILER_5__)         \
                            ||  defined(__IS_COMPILER_ARM_COMPILER_6__)         \
                            ||  defined(__IS_COMPILER_LLVM__))
#       define __IS_COMPILER_GCC__              1
#   endif
//! @}
#endif
//! @}


#ifndef __PLOOC_VA_NUM_ARGS_IMPL
#   define __PLOOC_VA_NUM_ARGS_IMPL(   _0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,   \
                                    _12,_13,_14,_15,_16,__N,...)      __N
#endif

#ifndef __PLOOC_VA_NUM_ARGS
#define __PLOOC_VA_NUM_ARGS(...)                                                \
            __PLOOC_VA_NUM_ARGS_IMPL( 0,##__VA_ARGS__,16,15,14,13,12,11,10,9,   \
                                      8,7,6,5,4,3,2,1,0)
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcompound-token-split-by-macro"
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wpedantic"
#endif


#undef __CONNECT2
#undef __CONNECT3
#undef __CONNECT4
#undef __CONNECT5
#undef __CONNECT6
#undef __CONNECT7
#undef __CONNECT8
#undef __CONNECT9

#undef CONNECT2
#undef CONNECT3
#undef CONNECT4
#undef CONNECT5
#undef CONNECT6
#undef CONNECT7
#undef CONNECT8
#undef CONNECT9

#undef CONNECT

#define __CONNECT2(__A, __B)                        __A##__B
#define __CONNECT3(__A, __B, __C)                   __A##__B##__C
#define __CONNECT4(__A, __B, __C, __D)              __A##__B##__C##__D
#define __CONNECT5(__A, __B, __C, __D, __E)         __A##__B##__C##__D##__E
#define __CONNECT6(__A, __B, __C, __D, __E, __F)    __A##__B##__C##__D##__E##__F
#define __CONNECT7(__A, __B, __C, __D, __E, __F, __G)                           \
                                                    __A##__B##__C##__D##__E##__F##__G
#define __CONNECT8(__A, __B, __C, __D, __E, __F, __G, __H)                      \
                                                    __A##__B##__C##__D##__E##__F##__G##__H
#define __CONNECT9(__A, __B, __C, __D, __E, __F, __G, __H, __I)                 \
                                                    __A##__B##__C##__D##__E##__F##__G##__H##__I
                                                    
#define CONNECT2(__A, __B)                  __CONNECT2(__A, __B)
#define CONNECT3(__A, __B, __C)             __CONNECT3(__A, __B, __C)
#define CONNECT4(__A, __B, __C, __D)        __CONNECT4(__A, __B, __C, __D)
#define CONNECT5(__A, __B, __C, __D, __E)   __CONNECT5(__A, __B, __C, __D, __E)       
#define CONNECT6(__A, __B, __C, __D, __E, __F)                                  \
                                            __CONNECT6(__A, __B, __C, __D, __E, __F) 
#define CONNECT7(__A, __B, __C, __D, __E, __F, __G)                             \
                                            __CONNECT7(__A, __B, __C, __D, __E, __F, __G) 
#define CONNECT8(__A, __B, __C, __D, __E, __F, __G, __H)                        \
                                            __CONNECT8(__A, __B, __C, __D, __E, __F, __G, __H) 
#define CONNECT9(__A, __B, __C, __D, __E, __F, __G, __H, __I)                   \
                                            __CONNECT9(__A, __B, __C, __D, __E, __F, __G, __H, __I)
       
#define CONNECT(...)                                                            \
            CONNECT2(CONNECT, __PLOOC_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)
            
            


#undef __using1
#undef __using2
#undef __using3
#undef __using4
#undef using

#define __using1(__declare)                                                     \
            for (__declare, *CONNECT3(__using_, __LINE__,_ptr) = NULL;          \
                 CONNECT3(__using_, __LINE__,_ptr)++ == NULL;                   \
                )

#define __using2(__declare, __on_leave_expr)                                    \
            for (__declare, *CONNECT3(__using_, __LINE__,_ptr) = NULL;          \
                 CONNECT3(__using_, __LINE__,_ptr)++ == NULL;                   \
                 (__on_leave_expr)                                              \
                )

#define __using3(__declare, __on_enter_expr, __on_leave_expr)                   \
            for (__declare, *CONNECT3(__using_, __LINE__,_ptr) = NULL;          \
                 CONNECT3(__using_, __LINE__,_ptr)++ == NULL ?                  \
                    ((__on_enter_expr),1) : 0;                                  \
                 (__on_leave_expr)                                              \
                )
                
#define __using4(__dcl1, __dcl2, __on_enter_expr, __on_leave_expr)              \
            for (__dcl1, __dcl2, *CONNECT3(__using_, __LINE__,_ptr) = NULL;     \
                 CONNECT3(__using_, __LINE__,_ptr)++ == NULL ?                  \
                    ((__on_enter_expr),1) : 0;                                  \
                 (__on_leave_expr)                                              \
                )
               
#define using(...)                                                              \
            CONNECT2(__using, __PLOOC_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)


#undef __with2
#undef __with3
#undef with

#define __with2(__type, __addr)                                                 \
            using(__type *_=(__addr))
#define __with3(__type, __addr, __item)                                         \
            using(__type *_=(__addr), *__item = _, _=_,_=_ )

#define with(...)                                                               \
            CONNECT2(__with, __PLOOC_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

#undef _

#ifndef dimof
#   define dimof(__array)          (sizeof(__array)/sizeof(__array[0]))
#endif


#undef foreach2
#undef foreach3
#undef foreach

#define foreach2(__type, __array)                                               \
            using(__type *_ = __array)                                         \
            for (   uint_fast32_t CONNECT2(count,__LINE__) = dimof(__array);    \
                    CONNECT2(count,__LINE__) > 0;                               \
                    _++, CONNECT2(count,__LINE__)--                            \
                )

#define foreach3(__type, __array, __item)                                       \
            using(__type *_ = __array, *__item = _, _ = _, )                \
            for (   uint_fast32_t CONNECT2(count,__LINE__) = dimof(__array);    \
                    CONNECT2(count,__LINE__) > 0;                               \
                    _++, __item = _, CONNECT2(count,__LINE__)--               \
                )

#define foreach(...)                                                            \
            CONNECT2(foreach, __PLOOC_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

#ifndef safe_atom_code
#   define safe_atom_code()                                                     \
            using(  uint32_t CONNECT2(temp,__LINE__) =                          \
                        ({uint32_t temp=__get_PRIMASK();__disable_irq();temp;}),\
                        __set_PRIMASK(CONNECT2(temp,__LINE__)))
#endif
            
#ifndef __IRQ_SAFE
#   define __IRQ_SAFE                                                           \
            using(  uint32_t CONNECT2(temp,__LINE__) =                          \
                        ({uint32_t temp=__get_PRIMASK();__disable_irq();temp;}),\
                        __set_PRIMASK(CONNECT2(temp,__LINE__)))
#endif
            
/*============================ MACROFIED FUNCTIONS ===========================*/

            
#define __cycleof__(__STR, ...)                                                 \
            using(int64_t _ = get_system_ticks(), {                             \
                _ = get_system_ticks() - _;                                     \
                if (__PLOOC_VA_NUM_ARGS(__VA_ARGS__) == 0) {                    \
                    printf("\r\n");                                             \
                    printf("-[Cycle Report]");                                  \
                    printf("--------------------------------------------\r\n"); \
                    printf(__STR " total cycle count: %d [%08x]\r\n",           \
                            (int)_, (int)_);                                    \
                } else {                                                        \
                    __VA_ARGS__                                                 \
                };                                                              \
            })
                    
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/




/*! \brief try to set a start pointer for the performance counter
 *! \retval false the LOAD register is too small
 *! \retval true performance counter starts
*/
extern bool start_cycle_counter(void);

/*! \brief calculate the elapsed cycle count since the last start point
 *! 
 *! \note you can have multiple stop_cycle_counter following one start point
 *!  
 *! \return the elapsed cycle count.
 */ 
extern int32_t stop_cycle_counter(void);

/* Function : delay specified us with the help from systick
 */
extern void delay_us(int32_t iUs);

/*! \note the prototype of this clock() is different from the one defined in
 *!           time.h. As clock_t is usually defined as unsigned int, it is
 *!           not big enough in Cortex-M system to hold a time-stamp. clock()
 *!           defined here returns the timestamp since the begining of main()
 *!           and its unit is clock cycle (rather than 1ms). Hence, for a system
 *!           running under several hundreds MHz or even 1GHz, e.g. RT10xx from 
 *!           NXP, it is very easy to see a counter overflow as clock_t is 
 *!           defined as uint32_t in timer.h.
 *!           Since we are not allowed to change the defintion of clock_t in
 *!           official header file, i.e. time.h, I use a compatible prototype 
 *!           after I checked the AAPCS spec. So, the return of the clock() is 
 *!           int64_t, which will use the R0 to store the lower 32bits and R1 
 *!           to store the higher 32bits. When you are using the prototype from
 *!           timer.h, caller will only take the lower 32bits stored in R0 and 
 *!           the higher 32bits stored in R1 will be ignored.
 *! 
 *!           If you want to use the non-overflow version of this clock(), please
 *!           1) define the MACRO: __PERF_CNT_USE_LONG_CLOCK__ in your project 
 *!           and 2) do not include system header file <time.h>
 *!
 */
#ifdef __PERF_CNT_USE_LONG_CLOCK__
__attribute__((nothrow)) 
extern int64_t clock(void);
#endif

__attribute__((nothrow)) 
extern int64_t get_system_ticks(void);


/*----------------------------------------------------------------------------*
 * Please ignore the following APIs unless you have encountered some known    *
 * special conditions                                                         *
 *----------------------------------------------------------------------------*/ 
 

/*! \brief   initialise cycle counter service
 *!          and don't forget to tell the function whether the systick is already
 *!          used by user applications. 
 *!          Don't worry, this cycle counter service won't affect your existing
 *!          systick service.
 *!
 *! \note    Usually the perf_counter can initialise itself with the help of
 *!          __attribute__((constructor(255))), this works fine in Arm Compiler
 *!          5 (armcc), Arm Compiler 6 (armclang), arm gcc and llvm. It doesn't
 *!          work for IAR. So, when you are using IAR, please call this function
 *!          manually to initialise the perf_counter service.
 *!
 *! \note    Perf_counter library assumes that:
 *!          a. Your project has already using SysTick
 *!          b. It assumes that you have already implemented the SysTick_Handler
 *!          c. It assumes that you have enabled the exception handling for 
 *!             SysTick.
 *!          If these are not the case, please:
 *!          a. Add an empty SysTick_Handler to your project if you don't have 
 *!             one
 *!          b. Make sure you have the SysTick Exception handling enabled
 *!          c. And call function init_cycle_counter(false) if you doesn't 
 *!             use SysTick in your project at all.
 *!
 *! \param bSysTickIsOccupied  A boolean value which indicates whether SysTick
 *!          is already used by user application.
 */
extern void init_cycle_counter(bool bSysTickIsOccupied);


/*! \note  if you are using a compiler other than armcc or armclang, e.g. iar,
 *!        arm gcc etc, the systick_wrapper_ual.o doesn't work with the linker
 *!        of your target toolchain as it use the $Super$$ which is only supported
 *!        by armlink. For this condition, you have to manually put this function
 *!        into your existing SysTick_Handler to make the perf_counter library
 *!        work.
 *! 
 *! \note  if you are using Arm Compiler 5 (armcc) or Arm Compiler 6 (armclang)
 *!        you do NOT have to insert this function into your SysTick_Handler,
 *!        the systick_wrapper_ual.s will do the work for you.
 */
extern void user_code_insert_to_systick_handler(void);


#endif
