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


#ifndef __PLOOC_EXAMPLE_BYTE_QUEUE_H__
#define __PLOOC_EXAMPLE_BYTE_QUEUE_H__

/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include <stdbool.h>
#include "plooc.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef struct mem_t {
    uint8_t     *pchBuffer;             //!< queue buffer
    uint16_t    hwSize;                 //!< buffer size
}mem_t;

/*! \NOTE: Make sure #include "plooc_class.h" is close to the class definition 
 */
#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__
   
#if     defined(__BYTE_QUEUE_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__BYTE_QUEUE_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#endif   

#include "plooc_class.h"


//! \name class byte_queue_t
//! @{
declare_class(byte_queue_t)

def_class(byte_queue_t,

    private_member(
        implement(mem_t)                    //!< queue buffer
        void        *pTarget;               //!< user target
    )
    
    protected_member(
        uint16_t    hwHead;                 //!< head pointer
        uint16_t    hwTail;                 //!< tail pointer
        uint16_t    hwCount;                //!< byte count
    )
)

end_def_class(byte_queue_t) /* do not remove this for forward compatibility  */
//! @}


typedef struct byte_queue_cfg_t {
    implement(mem_t)                        //!< queue buffer
    void *      pTarget;                    //!< user target
} byte_queue_cfg_t;

//! \name interface i_byte_queue_t
//! @{
def_interface(i_byte_queue_t)
    byte_queue_t *  (*Init)     (byte_queue_t *ptObj, byte_queue_cfg_t *ptCFG);
    void            (*Depose)   (byte_queue_t *ptObj);
    bool            (*Enqueue)  (byte_queue_t *ptObj, uint8_t chByte);
    bool            (*Dequeue)  (byte_queue_t *ptObj, uint8_t *pchByte);
    uint_fast16_t   (*Count)    (byte_queue_t *ptObj);
    struct {
        void *      (*Get)      (byte_queue_t *ptObj);
        void        (*Set)      (byte_queue_t *ptObj, void *pTarget);
    }Target;
end_def_interface(i_byte_queue_t) /*do not remove this for forward compatibility */
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
extern const i_byte_queue_t BYTE_QUEUE;

/*============================ PROTOTYPES ====================================*/

extern
byte_queue_t * byte_queue_init(byte_queue_t *ptObj, byte_queue_cfg_t *ptCFG);

extern
void byte_queue_depose(byte_queue_t *ptObj);

extern 
bool byte_queue_enqueue(byte_queue_t *ptObj, uint8_t chByte);

extern
bool byte_queue_dequeue(byte_queue_t *ptObj, uint8_t *pchByte);

extern
uint_fast16_t byte_queue_count(byte_queue_t *ptObj);

extern 
void *byte_queue_target_get(byte_queue_t *ptObj);

extern 
void  byte_queue_target_set(byte_queue_t *ptObj, void *pTarget);

#if defined(__BYTE_QUEUE_CLASS_IMPLEMENT) || defined(__BYTE_QUEUE_CLASS_INHERIT__)
extern mem_t byte_queue_buffer_get(byte_queue_t *ptObj);
#endif

/*! \note it is very important to undef those macros */
#undef __BYTE_QUEUE_CLASS_INHERIT
#undef __BYTE_QUEUE_CLASS_IMPLEMENT__

#endif
/* EOF */