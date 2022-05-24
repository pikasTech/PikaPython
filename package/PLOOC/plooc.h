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

#ifndef __PROTECTED_LOW_OVERHEAD_OBJECT_ORIENTED_C_H__
#define __PROTECTED_LOW_OVERHEAD_OBJECT_ORIENTED_C_H__

/*============================ INCLUDES ======================================*/
#if (!defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L) && !defined(__cplusplus)
//! you have to define this by yourselves
#else
#include <stdint.h>
#include <stdbool.h>
#endif

/*! \NOTE the uint_fast8_t used in this header file is defined in stdint.h 
          if you don't have stdint.h supported in your toolchain, you should
          define uint_fast8_t all by yourself with following rule:
          a. if the target processor is 8 bits, define it as uint8_t
          b. if the target processor is 16 bits, define it as uint16_t 
          c. if the target processor is 32 bits, define it as uint32_t
          d. if the target processor is 64 bits, define it as either uint32_t or 
             uint64_t
 */

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wtypedef-redefinition"
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wempty-body"
#   pragma clang diagnostic ignored "-Wmicrosoft-anon-tag"
#elif ((__ARMCC_VERSION >= 5000000) && (__ARMCC_VERSION < 6000000))
/*! arm compiler 5 */
#   pragma push
#   pragma diag_suppress 1,64,174,177,188,68,513,144,2525
#elif defined(__IAR_SYSTEMS_ICC__)
/*! IAR */
#elif defined(__GNUC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wmissing-declarations"
#   pragma GCC diagnostic ignored "-Wempty-body"
#   pragma GCC diagnostic ignored "-Wpragmas"
#   pragma GCC diagnostic ignored "-Wformat="
#   pragma GCC diagnostic ignored "-Wmissing-braces"
#endif


 #ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
#ifndef __cplusplus
#   ifndef plooc_private
#       define plooc_private    static
#   endif

#   ifndef plooc_public
#       define plooc_public      
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/


/*! \note add which macro to support multiple inheriting and implementations
 *!
 *!       declare_interface( i_lv0_abc_t )
 *!       declare_interface( i_lv0_efg_t )
 *!       def_interface( i_lv0_abc_t )
 *!           ...
 *!       end_def_interface( i_lv0_abc_t )
 *!
 *!       def_interface( i_lv0_efg_t )
 *!           ...
 *!       end_def_interface( i_lv0_efg_t )
 *!     
 *!       declare_interface( i_lv1_t )
 *!       def_interface( i_lv1_t, which( inherit( i_lv0_abc_t ) 
 *!                                      inherit( i_lv0_efg_t ) ) )
 *!           ...
 *!       end_def_interface( i_lv1_t )
  */

#define __declare_interface(__name)   typedef struct __name __name;
#define __declare_structure(__name)   typedef struct __name __name;

#if (!defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L) && !defined(__cplusplus)

//! \name interface definition
//! @{
#define __def_interface(__name)                                                 \
            /*typedef struct __name __name;*/                                   \
            struct __name {

#define __end_def_interface(__name)                                             \
            };
//! @}

//! \name structure definition
//! @{
#define __def_structure(__name)                                                 \
            /*typedef struct __name __name; */                                  \
            struct __name {

#define __end_def_structure(__name)                                             \
            };
//! @}

#else
//! \name interface definition
//! @{
#define __def_interface(__name,...)                                             \
            typedef struct __name __name;                                       \
            __VA_ARGS__                                                         \
            struct __name {

#define __end_def_interface(__name)                                             \
            };
//! @}

//! \name structure definition
//! @{
#define __def_structure(__name,...)                                             \
            typedef struct __name __name;                                       \
            __VA_ARGS__                                                         \
            struct __name {

#define __end_def_structure(__name)                                             \
            };
//! @}
#endif

//! \brief macro for inheritance

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#define __IMPLEMENT_EX(__TYPE, __NAME)                                          \
            __TYPE  __NAME;                                                 
#else
#define __IMPLEMENT_EX(__TYPE, __NAME)                                          \
            union {                                                             \
                __TYPE  __NAME;                                                 \
                __TYPE;                                                         \
            };
#endif

#define __INHERIT_EX(__TYPE, __NAME)    __TYPE  __NAME;
#define INHERIT_EX(__TYPE, __NAME)      __INHERIT_EX(__TYPE, __NAME)

#define __INHERIT(__TYPE)              INHERIT_EX(__TYPE, use_as__##__TYPE)
#define INHERIT(__TYPE)                __INHERIT(__TYPE)



/*! \note You can only use IMPLEMENT when defining INTERFACE. For Implement 
 *        interface when defining class, you should use DEF_CLASS_IMPLEMENT 
 *        instead.
 */
#define __IMPLEMENT(__INTERFACE)        __IMPLEMENT_EX( __INTERFACE,            \
                                                        use_as__##__INTERFACE)
#define IMPLEMENT(__INTERFACE)          __IMPLEMENT(__INTERFACE)  

/*! \note if you have used INHERIT or IMPLEMENT to define a class / INTERFACE, 
          you can use OBJ_CONVERT_AS to extract the reference to the inherited 
          object. 
  \*/
#define __OBJ_CONVERT_AS(__OBJ, __INTERFACE)    (__OBJ.use_as__##__INTERFACE)
#define OBJ_CONVERT_AS(__OBJ, __INTERFACE)      __OBJ_CONVERT_AS(   (__OBJ),    \
                                                                    __INTERFACE)

#define __REF_OBJ_AS(__OBJ, __TYPE)             (&(__OBJ.use_as__##__TYPE))
#define REF_OBJ_AS(__OBJ, __TYPE)               __REF_OBJ_AS((__OBJ), __TYPE)
           
           
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || defined(__cplusplus)
/*! \brief You can use __PLOOC_EVAL() to dynamically select the right API which
 *!        has the right number of parameters (no more than 8).
 */
//! @{
#define __PLOOC_VA_NUM_ARGS_IMPL(   _0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,  \
                                    _13,_14,_15,_16,__N,...)      __N
#define __PLOOC_VA_NUM_ARGS(...)                                                \
            __PLOOC_VA_NUM_ARGS_IMPL( 0,##__VA_ARGS__,16,15,14,13,12,11,10,9,   \
                                      8,7,6,5,4,3,2,1,0)

#define __16_PLOOC_EVAL(__FUNC, __NO_ARGS)  __FUNC##__NO_ARGS
#define __15_PLOOC_EVAL(__FUNC, __NO_ARGS)  __16_PLOOC_EVAL(__FUNC, __NO_ARGS)
#define __14_PLOOC_EVAL(__FUNC, __NO_ARGS)  __15_PLOOC_EVAL(__FUNC, __NO_ARGS)
#define __13_PLOOC_EVAL(__FUNC, __NO_ARGS)  __14_PLOOC_EVAL(__FUNC, __NO_ARGS)

#define __12_PLOOC_EVAL(__FUNC, __NO_ARGS)  __13_PLOOC_EVAL(__FUNC, __NO_ARGS)
#define __11_PLOOC_EVAL(__FUNC, __NO_ARGS)  __12_PLOOC_EVAL(__FUNC, __NO_ARGS)
#define __10_PLOOC_EVAL(__FUNC, __NO_ARGS)  __11_PLOOC_EVAL(__FUNC, __NO_ARGS)
#define __9_PLOOC_EVAL(__FUNC, __NO_ARGS)   __10_PLOOC_EVAL(__FUNC, __NO_ARGS)
#define __8_PLOOC_EVAL(__FUNC, __NO_ARGS)   __9_PLOOC_EVAL(__FUNC, __NO_ARGS)

#define __7_PLOOC_EVAL(__FUNC, __NO_ARGS)   __8_PLOOC_EVAL(__FUNC, __NO_ARGS)
#define __6_PLOOC_EVAL(__FUNC, __NO_ARGS)   __7_PLOOC_EVAL(__FUNC, __NO_ARGS)
#define __5_PLOOC_EVAL(__FUNC, __NO_ARGS)   __6_PLOOC_EVAL(__FUNC, __NO_ARGS)
#define __4_PLOOC_EVAL(__FUNC, __NO_ARGS)   __5_PLOOC_EVAL(__FUNC, __NO_ARGS)
#define __3_PLOOC_EVAL(__FUNC, __NO_ARGS)   __4_PLOOC_EVAL(__FUNC, __NO_ARGS)
#define __2_PLOOC_EVAL(__FUNC, __NO_ARGS)   __3_PLOOC_EVAL(__FUNC, __NO_ARGS)
#define __1_PLOOC_EVAL(__FUNC, __NO_ARGS)   __2_PLOOC_EVAL(__FUNC, __NO_ARGS)
#define __0_PLOOC_EVAL(__FUNC, __NO_ARGS)   __1_PLOOC_EVAL(__FUNC, __NO_ARGS)

#define __PLOOC_EVAL(__FUNC, ...)           __0_PLOOC_EVAL(                     \
                                                __FUNC,                         \
                                                __PLOOC_VA_NUM_ARGS(__VA_ARGS__))
//! @}   
#endif
           
/*----------------------------------------------------------------------------* 
 * new standard (lower case)                                                  *
 *----------------------------------------------------------------------------*/
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#   define def_interface(__name)            __def_interface(__name)
#   define define_interface(__name)         __def_interface(__name)
#   define def_structure(__name)            __def_structure(__name)
#   define define_structure(__name)         __def_structure(__name)
#   define def_params(__code)               __code
#   define define_params(__code)            __code
#   define end_def_params()
#   define end_define_params()
#   define def_members(__code)              __code
#   define define_members(__code)           __code
#   define end_def_members()
#   define end_define_members()
#else
#   define def_interface(__name, ...)       __def_interface(__name, __VA_ARGS__)
#   define define_interface(__name, ...)    __def_interface(__name, __VA_ARGS__)
#   define def_structure(__name, ...)       __def_structure(__name, __VA_ARGS__)
#   define define_structure(__name, ...)       __def_structure(__name, __VA_ARGS__)
#   define def_params(...)                  __VA_ARGS__
#   define define_params(...)               __VA_ARGS__
#   define end_def_params(...)
#   define end_define_params(...)
#   define def_members(...)                 __VA_ARGS__
#   define define_members(...)              __VA_ARGS__
#   define end_def_members(...)
#   define end_define_members(...)
#endif

#define implement(__type)                   IMPLEMENT(__type)
#define implement_ex(__type, __name)        __IMPLEMENT_EX(__type, __name)
#define inherit_ex(__type, __name)          INHERIT_EX(__type, __name)
#define inherit(__type)                     INHERIT(__type)
#define ref_interface(__INTERFACE)          const __INTERFACE *ptMethod;
#define convert_obj_as(__obj, __type)       OBJ_CONVERT_AS(__obj, __type)
#define obj_convert_as(__obj, __type)       OBJ_CONVERT_AS(__obj, __type)       /*  obsolete */
#define ref_obj_as(__obj, __type)           REF_OBJ_AS(__obj, __type)

#define end_def_interface(__name)           __end_def_interface(__name)
#define end_define_interface(__name)        __end_def_interface(__name)
#define dcl_interface(__name)               __declare_interface(__name)
#define declare_interface(__name)           __declare_interface(__name)
#define end_def_structure(__name)           __end_def_structure(__name)
#define end_define_structure(__name)        __end_def_structure(__name)
#define dcl_structure(__name)               __declare_structure(__name)
#define declare_structure(__name)           __declare_structure(__name)

#define this_interface(__INTERFACE)         convert_obj_as(this, __INTERFACE)
#define base_obj(__type)                    convert_obj_as(this, __type)
           

        
/*============================ TYPES =========================================*/

//! \name interface: u32_property_t
//! @{
dcl_interface(u32_property_t)
def_interface(u32_property_t)
    bool (*Set)(uint32_t wValue);
    uint32_t (*Get)(void);
end_def_interface(u32_property_t)
//! @}

//! \name interface: u16_property_t
//! @{
dcl_interface(u16_property_t)
def_interface(u16_property_t)
    bool (*Set)(uint_fast16_t wValue);
    uint_fast16_t (*Get)(void);
end_def_interface(u16_property_t)
//! @}

//! \name interface: u8_property_t
//! @{
dcl_interface(u8_property_t)
def_interface(u8_property_t)
    bool (*Set)(uint_fast8_t wValue);
    uint_fast8_t (*Get)(void);
end_def_interface(u8_property_t)
//! @}


//! \name interface: i32_property_t
//! @{
dcl_interface(i32_property_t)
def_interface(i32_property_t)
    bool (*Set)(int32_t wValue);
    int32_t (*Get)(void);
end_def_interface(i32_property_t)
//! @}

//! \name interface: i16_property_t
//! @{
dcl_interface(i16_property_t)
def_interface(i16_property_t)
    bool (*Set)(int_fast16_t wValue);
    int_fast16_t (*Get)(void);
end_def_interface(i16_property_t)
//! @}

//! \name interface: u8_property_t
//! @{
dcl_interface(i8_property_t)
def_interface(i8_property_t)
    bool (*Set)(int_fast8_t wValue);
    int_fast8_t (*Get)(void);
end_def_interface(i8_property_t)
//! @}

//! \name interface: bool_property_t
//! @{
dcl_interface(bool_property_t)
def_interface(bool_property_t)
    bool (*Set)(bool bValue);
    bool (*Get)(void);
end_def_interface(bool_property_t)
//! @}

//! \name interface: bool_property_t
//! @{
dcl_interface(en_property_t)
def_interface(en_property_t)
    bool (*Enable)(void);
    bool (*Disable)(void);
end_def_interface(en_property_t)
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

//#if defined(__clang__)
//#   pragma clang diagnostic pop
//#endif

#endif
/* EOF */
