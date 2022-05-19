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


/*============================ INCLUDES ======================================*/

#define __<BASE_CLASS_NAME>__CLASS_INHERIT
#define __<CLASS_NAME>_CLASS_IMPLEMENT

#include "./<class_name>.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
#ifndef ASSERT
#   define ASSERT(...)    assert(__VA_ARGS__)
#endif

#undef this
#define this        (*ptThis)

#undef base
#define base        (*ptBase)
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
const i_<class_name>_t <CLASS_NAME> = {
    .Init =             &<class_name>_init,
    .Depose =           &<class_name>_depose,
    
    /* other methods */
};


/*============================ IMPLEMENTATION ================================*/

/*! \brief the constructor of the class: <class_name> */
<class_name>_t * <class_name>_init(<class_name>_t *ptObj, <class_name>_cfg_t *ptCFG)
{
    /* initialise "this" (i.e. ptThis) to access class members */
    class_internal(ptObj, ptThis, <class_name>_t);
    
    /* initialise "base" (i.e. ptBase) to access protected members */
    protected_internal(&this.use_as__<base_class_name>_t, ptBase, <base_class_name>_t);
    
    
    ASSERT(NULL != ptObj && NULL != ptCFG);

    return ptObj;
}

/*! \brief the destructor of the class: <class_name> */
void <class_name>_depose(<class_name>_t *ptObj)
{
    /* initialise "this" (i.e. ptThis) to access class members */
    class_internal(ptObj, ptThis, <class_name>_t);
    
    /* initialise "base" (i.e. ptBase) to access protected members */
    protected_internal(&this.use_as__<base_class_name>_t, ptBase, <base_class_name>_t);
    
    
}


/*! \brief a method only visible for current class and derived class */
void <class_name>_protected_method_example(<class_name>_t *ptObj)
{
    /* initialise "this" (i.e. ptThis) to access class members */
    class_internal(ptObj, ptThis, <class_name>_t);
    
    /* initialise "base" (i.e. ptBase) to access protected members */
    protected_internal(&this.use_as__<base_class_name>_t, ptBase, <base_class_name>_t);
    
    
    
}

/* EOF */