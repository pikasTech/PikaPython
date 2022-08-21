/****************************************************************************
*  Copyright 2017 Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)       *
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
//#include <stdint.h>
//#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
#undef declare_class
#undef dcl_class
#undef def_class
#undef define_class
#undef __def_class
#undef end_def_class
#undef end_define_class
#undef __end_def_class
#undef extern_class
#undef __extern_class
#undef end_extern_class
#undef __end_extern_class
#undef class
#undef __class
/*============================ MACROFIED FUNCTIONS ===========================*/

#if   defined(__PLOOC_CLASS_IMPLEMENT__) || defined(__PLOOC_CLASS_IMPLEMENT)

#   ifdef __OOC_DEBUG__
#       define __def_class(__name, __public ,...)                               \
    typedef struct __name __name;                                               \
    typedef struct __##__name __##__name;                                       \
    struct __##__name {                                                         \
        __public                                                                \
        PLOOC_VISIBLE(__VA_ARGS__)                                              \
    };                                                                          \
    struct __name {                                                             \
        __public                                                                \
        PLOOC_VISIBLE(__VA_ARGS__)                                              \
    };                                                                          
#   else
#       define __def_class(__name, __public, ...)                               \
    typedef struct __name __name;                                               \
    typedef struct __##__name __##__name;                                       \
    struct __##__name {                                                         \
        __public                                                                \
        PLOOC_VISIBLE(__VA_ARGS__)                                              \
    };                                                                          \
    struct __name {                                                             \
        __public                                                                \
        PLOOC_INVISIBLE(__VA_ARGS__)                                            \
    };                                                                          
    
#   endif
          
#   define __end_def_class(__name, ...)

#   undef private_member
#   define private_member(...)              PLOOC_VISIBLE(__VA_ARGS__)

#   undef public_member
#   define public_member(...)               PLOOC_VISIBLE(__VA_ARGS__)

#   undef  __class
#   define __class(__name)                  __##__name

#   define class(__name)                    __class(__name)

#define __extern_class(__name,...)
#define __end_extern_class(__name, ...)

#else

#ifndef __OOC_DEBUG__        

#   define __def_class(__name, __public, ...)                                   \
    typedef struct __name __name;                                               \
    typedef struct __##__name __##__name;                                       \
    struct __##__name {                                                         \
        __public                                                                \
        PLOOC_VISIBLE(__VA_ARGS__)                                              \
    };                                                                          
  
#define __extern_class(__name,__public, ...)                                    \
    typedef struct __name __name;                                               \
    struct __name {                                                             \
        __public                                                                \
        PLOOC_INVISIBLE(__VA_ARGS__)                                            \
    }; 


#define __end_extern_class(__name, ...)

#   undef private_member
#   define private_member(...)              PLOOC_VISIBLE(__VA_ARGS__)

#   undef public_member
#   define public_member(...)               PLOOC_VISIBLE(__VA_ARGS__)
    
#else
               
#   define __def_class(__name, __public, ...)                                   \
    typedef struct __name __name;                                               \
    typedef struct __##__name __##__name;                                       \
    struct __##__name {                                                         \
        __public                                                                \
        PLOOC_VISIBLE(__VA_ARGS__)                                              \
    };                                                                          
  
#define __extern_class(__name,__public, ...)                                    \
    typedef struct __name __name;                                               \
    struct __name {                                                             \
        __public                                                                \
        PLOOC_VISIBLE(__VA_ARGS__)                                              \
    }; 


#define __end_extern_class(__name, ...)
    
#   undef private_member
#   define private_member(...)              PLOOC_INVISIBLE(__VA_ARGS__)

#   undef public_member
#   define public_member(...)               PLOOC_VISIBLE(__VA_ARGS__)
    
#endif



#   define __end_def_class(__name, ...)  

#endif


#   undef  __class
#   define __class(__name)                  __##__name

#   undef  class
#   define class(__name)                    __class(__name)


#define declare_class(__name)           typedef struct __name __name;  
#define dcl_class(__name)               declare_class(__name)

#define end_def_class(__name, ...)      __end_def_class(__name, __VA_ARGS__)
#define end_define_class(__name, ...)   end_def_class(__name, ...)

#define def_class(__name,__public, ...)                                         \
            __def_class(__name, __public, __VA_ARGS__)
#define define_class(__name, __public, ...)                                     \
            def_class(__name,__public, __VA_ARGS__) 

#define class(__name)                   __class(__name)

#define extern_class(__name, __public, ...)                                     \
            __extern_class(__name, __public,__VA_ARGS__)
#define end_extern_class(__name, ...)   __end_extern_class(__name, __VA_ARGS__)

#ifndef __PLOOC_CLASS_BLACK_BOX_H__
#define __PLOOC_CLASS_BLACK_BOX_H__

/*! \brief macro for initializing class in compiler-time
 *! \param __type class name
 *! \param __obj target object
 *! \param ... initialization list
 */
#define __INIT_CLASS_OBJ(__type, __obj, ...)                                    \
            union {                                                             \
                class(__type) __##__obj;                                        \
                __type;                                                         \
            } __obj = {                                                         \
                .__##__obj = __VA_ARGS__                                        \
            }
#define init_class_obj(__type, __obj, ...)                                      \
            __INIT_CLASS_OBJ(__type, __obj, __VA_ARGS__)

#define __EXTERN_CLASS_OBJ( __type, __obj )                                     \
            extern union {                                                      \
                class(__type) __##__obj;                                        \
                __type;                                                         \
            }__obj;
#define extern_class_obj(__type, __obj)                                         \
            __EXTERN_CLASS_OBJ( __type, __obj )
#endif

/*----------------------------------------------------------------------------*
 * new standard (lower case)                                                  *
 *----------------------------------------------------------------------------*/

#   undef __class_internal
#   define __class_internal(__src, __des, __type, ...)                          \
            class(__type) *(__des) = (class(__type) *)(__src);                  \
            PLOOC_UNUSED_PARAM(__des);                                          \
            __with_class(__type, (__src), __VA_ARGS__)
            
#   undef class_internal
#   define class_internal(__src, __des, __type,...)                             \
            __class_internal(__src, __des, __type, __VA_ARGS__)
            
#   undef __with_class
#   define __with_class(__type, __src, ...)                                     \
        {                                                                       \
            class(__type)*_ =(class(__type) *)(__src);                          \
            PLOOC_UNUSED_PARAM(_);                                              \
            __VA_ARGS__;                                                        \
        }                                                                       \
        for (class(__type)*_ =(class(__type) *)(__src); NULL != _; _ = NULL)

#   undef with_class
#   define with_class(__type, __src, ...)                                       \
            __with_class(__type, __src, __VA_ARGS__)                  

#undef which
#define which(...)                      PLOOC_VISIBLE(__VA_ARGS__)
        
/*============================ TYPES =========================================*/

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#undef __PLOOC_CLASS_IMPLEMENT__
#undef __PLOOC_CLASS_INHERIT__
#undef __PLOOC_CLASS_IMPLEMENT
#undef __PLOOC_CLASS_INHERIT

#ifdef __cplusplus
}
#endif

/* EOF */
