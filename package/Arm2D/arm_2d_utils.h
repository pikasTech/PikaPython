/*
 * Copyright (C) 2010-2021 Arm Limited or its affiliates. All rights reserved.
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

/* ----------------------------------------------------------------------
 * Project:      Arm-2D Library
 * Title:        arm_2d_utils.h
 * Description:  Public header file for Arm-2D Library
 *
 * $Date:        20. sep 2021
 * $Revision:    V.0.6.0
 *
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_UTILS_H__
#define __ARM_2D_UTILS_H__

/*============================ INCLUDES ======================================*/

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wimplicit-int-conversion"
#   pragma clang diagnostic ignored "-Wundef"
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wpedantic"
#   pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif

#ifdef  __ARM_2D_HAS_USER_HEADER__
#   include __ARM_2D_HAS_USER_HEADER__
#endif

/*! \note arm-2d relies on CMSIS 5.4.0 and above.
 */
#include "cmsis_compiler.h"
#include <arm_math.h>

#ifdef   __cplusplus
extern "C" {
#endif



/*============================ MACROS ========================================*/


/*----------------------------------------------------------------------------*
 * Environment Detection                                                      *
 *----------------------------------------------------------------------------*/

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


/*----------------------------------------------------------------------------*
 * OOC and Private Protection                                                 *
 *----------------------------------------------------------------------------*/
/*! \brief minimal support for OOPC
 */
#undef __implement_ex
#undef __implement
#undef implement
#undef implement_ex
#undef inherit
#undef inherit_ex

#define __implement_ex(__type, __name)                                          \
            union {                                                             \
                __type  __name;                                                 \
                __type;                                                         \
            }

#define __inherit_ex(__type, __name)                                            \
            __type  __name                                                      \

#define __implement(__type)             __implement_ex( __type,                 \
                                                        use_as__##__type)

#define __inherit(__type)               __inherit_ex(__type,use_as__##__type)

#define implement(__type)               __implement(__type)
#define implement_ex(__type, __name)    __implement_ex(__type, __name)

#define inherit(__type)                 __inherit(__type)
#define inherit_ex(__type, __name)      __inherit_ex(__type, __name)


/*----------------------------------------------------------------------------*
 * Misc                                                                       *
 *----------------------------------------------------------------------------*/

#ifndef ARM_2D_UNUSED
#   define ARM_2D_UNUSED(__VAR)     (__VAR) = (__VAR)
#endif

#ifndef ARM_TEST_BITS
#   define ARM_TEST_BITS(__VALUE, __BITS)   ((__BITS) == ((__VALUE) & (__BITS)))
#endif

#ifndef dimof
#   define dimof(__array)          (sizeof(__array)/sizeof(__array[0]))
#endif

#define __ARM_TO_STRING(A)          #A
#define TO_STRING(A)                __ARM_TO_STRING(A)

#define __ARM_VA_NUM_ARGS_IMPL(   _0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,    \
                                    _13,_14,_15,_16,__N,...)      __N
#define __ARM_VA_NUM_ARGS(...)                                                  \
            __ARM_VA_NUM_ARGS_IMPL( 0,##__VA_ARGS__,16,15,14,13,12,11,10,9,     \
                                      8,7,6,5,4,3,2,1,0)

#define __ARM_CONNECT2(__A, __B)                        __A##__B
#define __ARM_CONNECT3(__A, __B, __C)                   __A##__B##__C
#define __ARM_CONNECT4(__A, __B, __C, __D)              __A##__B##__C##__D
#define __ARM_CONNECT5(__A, __B, __C, __D, __E)         __A##__B##__C##__D##__E
#define __ARM_CONNECT6(__A, __B, __C, __D, __E, __F)                            \
                                    __A##__B##__C##__D##__E##__F
#define __ARM_CONNECT7(__A, __B, __C, __D, __E, __F, __G)                       \
                                    __A##__B##__C##__D##__E##__F##__G
#define __ARM_CONNECT8(__A, __B, __C, __D, __E, __F, __G, __H)                  \
                                    __A##__B##__C##__D##__E##__F##__G##__H
#define __ARM_CONNECT9(__A, __B, __C, __D, __E, __F, __G, __H, __I)             \
                                    __A##__B##__C##__D##__E##__F##__G##__H##__I

#define ARM_CONNECT2(__A, __B)                  __ARM_CONNECT2(__A, __B)
#define ARM_CONNECT3(__A, __B, __C)             __ARM_CONNECT3(__A, __B, __C)
#define ARM_CONNECT4(__A, __B, __C, __D)        __ARM_CONNECT4(__A, __B, __C, __D)
#define ARM_CONNECT5(__A, __B, __C, __D, __E)                                   \
                __ARM_CONNECT5(__A, __B, __C, __D, __E)
#define ARM_CONNECT6(__A, __B, __C, __D, __E, __F)                              \
                __ARM_CONNECT6(__A, __B, __C, __D, __E, __F)
#define ARM_CONNECT7(__A, __B, __C, __D, __E, __F, __G)                         \
                __ARM_CONNECT7(__A, __B, __C, __D, __E, __F, __G)
#define ARM_CONNECT8(__A, __B, __C, __D, __E, __F, __G, __H)                    \
                __ARM_CONNECT8(__A, __B, __C, __D, __E, __F, __G, __H)
#define ARM_CONNECT9(__A, __B, __C, __D, __E, __F, __G, __H, __I)               \
                __ARM_CONNECT9(__A, __B, __C, __D, __E, __F, __G, __H, __I)

#define arm_connect(...)                                                        \
            ARM_CONNECT2(ARM_CONNECT, __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

#define ARM_CONNECT(...)        arm_connect(__VA_ARGS__)

#define __ARM_USING1(__declare)                                                 \
            for (__declare, *ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr) = NULL;  \
                 ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr)++ == NULL;           \
                )

#define __ARM_USING2(__declare, __on_leave_expr)                                \
            for (__declare, *ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr) = NULL;  \
                 ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr)++ == NULL;           \
                 __on_leave_expr                                                \
                )

#define __ARM_USING3(__declare, __on_enter_expr, __on_leave_expr)               \
            for (__declare, *ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr) = NULL;  \
                 ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr)++ == NULL ?          \
                    ((__on_enter_expr),1) : 0;                                  \
                 __on_leave_expr                                                \
                )

#define __ARM_USING4(__dcl1, __dcl2, __on_enter_expr, __on_leave_expr)          \
            for (__dcl1,__dcl2,*ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr)= NULL;\
                 ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr)++ == NULL ?          \
                    ((__on_enter_expr),1) : 0;                                  \
                 __on_leave_expr                                                \
                )

#define arm_using(...)                                                          \
            ARM_CONNECT2(__ARM_USING, __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)


#define __ARM_WITH2(__type, __addr)                                             \
            ARM_USING(__type *_p=(__addr))
#define __ARM_WITH3(__type, __addr, __item)                                     \
            ARM_USING(__type *_p=(__addr), *__item = _p, _p=_p, )

#define arm_with(...)                                                           \
            ARM_CONNECT2(__ARM_WITH, __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

#define ARM_FOREACH2(__type, __array)                                           \
            arm_using(__type *_ = __array)                                      \
            for (   uint_fast32_t ARM_CONNECT2(count,__LINE__) = dimof(__array);\
                    ARM_CONNECT2(count,__LINE__) > 0;                           \
                    _++, ARM_CONNECT2(count,__LINE__)--                         \
                )

#define ARM_FOREACH3(__type, __array, __item)                                   \
            arm_using(__type *_ = __array, *__item = _, _ = _, )                \
            for (   uint_fast32_t ARM_CONNECT2(count,__LINE__) = dimof(__array);\
                    ARM_CONNECT2(count,__LINE__) > 0;                           \
                    _++, __item = _, ARM_CONNECT2(count,__LINE__)--             \
                )

#define ARM_FOREACH4(__type, __array, __count, __item)                          \
            arm_using(__type *_ = __array, *__item = _, _ = _, )                \
            for (   uint_fast32_t ARM_CONNECT2(count,__LINE__) = (__count);     \
                    ARM_CONNECT2(count,__LINE__) > 0;                           \
                    _++, __item = _, ARM_CONNECT2(count,__LINE__)--             \
                )

#define arm_foreach(...)                                                        \
            ARM_CONNECT2(ARM_FOREACH, __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)


#ifndef ARM_NONNULL
#   if  defined(__IS_COMPILER_ARM_COMPILER_5__) ||\
        defined(__IS_COMPILER_ARM_COMPILER_6__) ||\
        defined(__IS_COMPILER_GCC__)            ||\
        defined(__IS_COMPILER_LLVM__)
#       define ARM_NONNULL(...)     __attribute__((nonnull(__VA_ARGS__)))
#   else
#       define ARM_NONNULL(...)
#   endif
#endif

#ifndef ARM_NOINIT
#   if     defined(__IS_COMPILER_ARM_COMPILER_5__)
#       define ARM_NOINIT       __attribute__(( section( ".bss.noinit"),zero_init))
#   elif   defined(__IS_COMPILER_ARM_COMPILER_6__)
#       define ARM_NOINIT       __attribute__(( section( ".bss.noinit")))
#   elif   defined(__IS_COMPILER_IAR__)
#       define ARM_NOINIT       __no_init
#   elif   defined(__IS_COMPILER_GCC__) || defined(__IS_COMPILER_LLVM__)
#       define ARM_NOINIT       __attribute__(( section( ".bss.noinit")))
#   else
#       define ARM_NOINIT
#   endif
#endif


#ifndef ARM_ALIGN
#   define __ARM_ALIGN(__N)        __attribute__((aligned(__N)))
#   define ARM_ALIGN(__N)          __ARM_ALIGN(__N)
#endif


#ifndef __RESTRICT
#   define __RESTRICT                __restrict
#endif

#ifndef __OVERRIDE_WEAK
#   define __OVERRIDE_WEAK          __USED
#endif

#define ARM_2D_SAFE_NAME(...)    ARM_CONNECT(__,__LINE__,##__VA_ARGS__)
#define arm_2d_safe_name(...)    ARM_2D_SAFE_NAME(__VA_ARGS__)

#undef arm_irq_safe

#define arm_irq_safe                                                            \
            arm_using(  uint32_t ARM_2D_SAFE_NAME(temp) =                       \
                        ({uint32_t temp=__get_PRIMASK();__disable_irq();temp;}),\
                        __set_PRIMASK(ARM_2D_SAFE_NAME(temp)))





/*----------------------------------------------------------------------------*
 * List Operations                                                            *
 *----------------------------------------------------------------------------*/

/*! \note ALL the parameters passed to following macros must be pointer
 *!       variables.
 */

#define __ARM_LIST_STACK_PUSH(__P_TOP, __P_NODE)                                \
        do {                                                                    \
            ((__arm_slist_node_t *)(__P_NODE))->ptNext =                        \
                (__arm_slist_node_t *)(__P_TOP);                                \
            (*(__arm_slist_node_t **)&(__P_TOP)) =                              \
                (__arm_slist_node_t *)(__P_NODE);                               \
        } while(0)
#define ARM_LIST_STACK_PUSH(__P_TOP, __P_NODE)                                  \
            __ARM_LIST_STACK_PUSH((__P_TOP), (__P_NODE))
#define ARM_LIST_INSERT_AFTER(__P_TARGET, __P_NODE)                             \
            __ARM_LIST_STACK_PUSH((__P_TARGET), (__P_NODE))

#define __ARM_LIST_STACK_POP(__P_TOP, __P_NODE)                                 \
        do {                                                                    \
            (*(__arm_slist_node_t **)&(__P_NODE)) =                             \
                (__arm_slist_node_t *)(__P_TOP);                                \
            if (NULL != (__P_TOP)) {                                            \
                (*(__arm_slist_node_t **)&(__P_TOP)) =                          \
                    ((__arm_slist_node_t *)(__P_NODE))->ptNext;                 \
                ((__arm_slist_node_t *)(__P_NODE))->ptNext = NULL;              \
            }                                                                   \
        } while(0)
#define ARM_LIST_STACK_POP(__P_TOP, __P_NODE)    __ARM_LIST_STACK_POP((__P_TOP), (__P_NODE))
#define ARM_LIST_REMOVE_AFTER(__P_TARGET, __P_NODE)                             \
            ARM_LIST_STACK_POP((__P_TARGET), (__P_NODE))


#define __ARM_LIST_QUEUE_ENQUEUE(__HEAD, __TAIL, __ITEM)                        \
    do {                                                                        \
        if (NULL == (__TAIL)) {                                                 \
            (*((__arm_slist_node_t **)&(__TAIL))) =                             \
                (__arm_slist_node_t *)(__ITEM);                                 \
            ((__arm_slist_node_t *)(__ITEM))->ptNext = NULL;                    \
            (*((__arm_slist_node_t **)&(__HEAD))) =                             \
                (__arm_slist_node_t *)(__ITEM);                                 \
        } else {                                                                \
            ((__arm_slist_node_t *)(__TAIL))->ptNext =                          \
                (__arm_slist_node_t *)(__ITEM);                                 \
            ((__arm_slist_node_t *)(__ITEM))->ptNext = NULL;                    \
            (*(__arm_slist_node_t **)&(__TAIL)) =                               \
                (__arm_slist_node_t *)(__ITEM);                                 \
        }                                                                       \
    } while(0)
#define ARM_LIST_QUEUE_ENQUEUE(__HEAD, __TAIL, __ITEM)  \
            __ARM_LIST_QUEUE_ENQUEUE((__HEAD), (__TAIL), (__ITEM))

#define __ARM_LIST_QUEUE_DEQUEUE(__HEAD, __TAIL, __ITEM)                        \
    do {                                                                        \
        (*(__arm_slist_node_t **)&(__ITEM)) =  (__arm_slist_node_t *)(__HEAD);  \
        if (NULL != (__HEAD)) {                                                 \
            (*(__arm_slist_node_t **)&(__HEAD)) =                               \
                ((__arm_slist_node_t *)(__HEAD))->ptNext;                       \
            if (NULL == (__HEAD)) {                                             \
                (__TAIL) = NULL;                                                \
            }                                                                   \
        }                                                                       \
    } while(0)

#define ARM_LIST_QUEUE_DEQUEUE(__HEAD, __TAIL, __ITEM)  \
            __ARM_LIST_QUEUE_DEQUEUE((__HEAD), (__TAIL), (__ITEM))

#define __ARM_LIST_QUEUE_PEEK(__HEAD, __TAIL, __ITEM)                           \
    do {                                                                        \
        (*(__arm_slist_node_t **)&(__ITEM)) =  (__arm_slist_node_t *)(__HEAD);  \
    } while(0)
#define ARM_LIST_QUEUE_PEEK(__HEAD, __TAIL, __ITEM)  \
            __ARM_LIST_QUEUE_PEEK((__HEAD), (__TAIL), (__ITEM))                 \

/*----------------------------------------------------------------------------*
 * Definition Template                                                        *
 *----------------------------------------------------------------------------*/

#define __def_low_lv_io(__NAME, __SW, ...)                                      \
const __arm_2d_low_level_io_t LOW_LEVEL_IO##__NAME = {                          \
    .SW = (__arm_2d_io_func_t *)&(__SW),                                        \
    .HW = (NULL, ##__VA_ARGS__)                                                 \
}
#define def_low_lv_io(__NAME, __SW, ...)                                        \
            __def_low_lv_io(__NAME, __SW, ##__VA_ARGS__)


#define __ref_low_lv_io(__NAME)     &LOW_LEVEL_IO##__NAME
#define ref_low_lv_io(__NAME)       __ref_low_lv_io(__NAME)

/*============================ TYPES =========================================*/

typedef struct __arm_slist_node_t __arm_slist_node_t;
struct __arm_slist_node_t {
    __arm_slist_node_t     *ptNext;
};


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif __IS_COMPILER_ARM_COMPILER_5__
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif  /* end of __ARM_2D_UTILS_H__ */


/*============================ MACROS ========================================*/
/*----------------------------------------------------------------------------*
 * Reentrant Macros                                                           *
 *----------------------------------------------------------------------------*/

/* un-define macros */
#undef ARM_PRIVATE


/* redefine macros */

#if defined(__ARM_2D_IMPL__) || defined(__IS_COMPILER_IAR__)

#   define ARM_PRIVATE(...)                                                     \
        struct  {                                                               \
            __VA_ARGS__                                                         \
        } __ALIGNED(__alignof__(struct {__VA_ARGS__}));

#else
#   define ARM_PRIVATE(...)                                                     \
        uint8_t ARM_CONNECT3(chMask,__LINE__,__COUNTER__)                       \
                [sizeof(struct {__VA_ARGS__})]                                  \
                __ALIGNED(__alignof__(struct {__VA_ARGS__}));
#endif




/* post un-define macros */

#undef __ARM_2D_IMPL__

