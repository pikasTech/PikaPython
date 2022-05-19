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


#ifndef __<CLASS_NAME>_H__
#define __<CLASS_NAME>_H__

/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include <stdbool.h>

#include "<base_class_name>.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

/*! \NOTE: Make sure #include "plooc_class.h" is close to the class definition 
 */
//#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__
   
#if     defined(__<CLASS_NAME>_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__<CLASS_NAME>_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#endif   

#include "plooc_class.h"


//! \name class <class_name>_t
//! @{
declare_class(<class_name>_t)

def_class(<class_name>_t,  which(implement(<base_class_name>_t))

    public_member(
        //!< place your public members here
    )

    private_member(
        //!< place your private members here
    )
    
    protected_member(
        //!< place your private members here
    )
)

end_def_class(<class_name>_t) /* do not remove this for forward compatibility  */
//! @}


typedef struct <class_name>_cfg_t {
    
    //! put your configuration members here
    
} <class_name>_cfg_t;

//! \name interface i_<class_name>_t
//! @{
def_interface(i_<class_name>_t)
    <class_name>_t *  (*Init)       (<class_name>_t *ptObj, <class_name>_cfg_t *ptCFG);
    void              (*Depose)     (<class_name>_t *ptObj);
    /* other methods */

end_def_interface(i_<class_name>_t) /*do not remove this for forward compatibility */
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
extern const i_<class_name>_t <CLASS_NAME>;

/*============================ PROTOTYPES ====================================*/

/*! \brief the constructor of the class: <class_name> */
extern
<class_name>_t * <class_name>_init(<class_name>_t *ptObj, <class_name>_cfg_t *ptCFG);

/*! \brief the destructor of the class: <class_name> */
extern
void <class_name>_depose(<class_name>_t *ptObj);

#if defined(__<CLASS_NAME>_CLASS_IMPLEMENT) || defined(__<CLASS_NAME>_CLASS_INHERIT__)
/*! \brief a method only visible for current class and derived class */
extern void <class_name>_protected_method_example(<class_name>_t *ptObj);
#endif

/*! \note it is very important to undef those macros */
#undef __<CLASS_NAME>_CLASS_INHERIT
#undef __<CLASS_NAME>_CLASS_IMPLEMENT__

#endif
/* EOF */