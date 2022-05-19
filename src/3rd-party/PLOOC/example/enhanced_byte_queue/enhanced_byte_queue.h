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


#ifndef __PLOOC_EXAMPLE_ENHANCED_BYTE_QUEUE_H__
#define __PLOOC_EXAMPLE_ENHANCED_BYTE_QUEUE_H__

/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include <stdbool.h>
#include "plooc.h"
#include "../byte_queue/byte_queue.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

/*! \NOTE: Make sure #include "plooc_class.h" is close to the class definition 
 */
#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__
   
#if     defined(__ENHANCED_BYTE_QUEUE_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#   undef __ENHANCED_BYTE_QUEUE_CLASS_IMPLEMENT
#elif   defined(__ENHANCED_BYTE_QUEUE_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#   undef __ENHANCED_BYTE_QUEUE_CLASS_INHERIT__
#endif   

#include "plooc_class.h"

//! \name class byte_queue_t
//! @{
declare_class(enhanced_byte_queue_t)

def_class(enhanced_byte_queue_t,
    which(implement(byte_queue_t))

    private_member(
        uint16_t hwPeek;
        uint16_t hwPeekCount;
    )
)

end_def_class(enhanced_byte_queue_t) /* do not remove this for forward compatibility  */
//! @}

typedef byte_queue_cfg_t enhanced_byte_queue_cfg_t;

//! \name interface i_byte_queue_t
//! @{
def_interface(i_enhanced_byte_queue_t)
    inherit(i_byte_queue_t)
    enhanced_byte_queue_t * (*Init)     (   enhanced_byte_queue_t *ptObj, 
                                            enhanced_byte_queue_cfg_t *ptCFG);
    void                    (*Depose)   (   enhanced_byte_queue_t *ptObj);
    bool                    (*Enqueue)  (   enhanced_byte_queue_t *ptObj, 
                                            uint8_t chByte);
    bool                    (*Dequeue)  (   enhanced_byte_queue_t *ptObj, 
                                            uint8_t *pchByte);
    uint_fast16_t           (*Count)    (   enhanced_byte_queue_t *ptObj);
    struct {
        void *              (*Get)      (   enhanced_byte_queue_t *ptObj);
        void                (*Set)      (   enhanced_byte_queue_t *ptObj, 
                                            void *pTarget);
    }Target;
    struct {
        bool                (*PeekByte)  (  enhanced_byte_queue_t *ptObj, 
                                            uint8_t *pchByte);
        void                (*Reset)     (  enhanced_byte_queue_t *ptObj);
        void                (*GetAllPeeked)(enhanced_byte_queue_t *ptObj);
    }Peek;
end_def_interface(i_enhanced_byte_queue_t) /*do not remove this for forward compatibility */
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
extern const i_enhanced_byte_queue_t ENHANCED_BYTE_QUEUE;

/*============================ PROTOTYPES ====================================*/

extern
enhanced_byte_queue_t * enhanced_byte_queue_init(enhanced_byte_queue_t *ptObj, 
                                                 enhanced_byte_queue_cfg_t *ptCFG);

extern
void enhanced_byte_queue_depose(enhanced_byte_queue_t *ptObj);

extern 
bool enhanced_byte_queue_enqueue(enhanced_byte_queue_t *ptObj, uint8_t chByte);

extern 
bool enhanced_byte_queue_dequeue(enhanced_byte_queue_t *ptObj, uint8_t *pchByte);

extern 
bool enhanced_byte_queue_peek(enhanced_byte_queue_t *ptObj, uint8_t *pchByte);

extern 
void enhanced_byte_queue_peek_reset(enhanced_byte_queue_t *ptObj);

extern 
void enhanced_byte_queue_peek_get_all_peeked(enhanced_byte_queue_t *ptObj);
#endif
/* EOF */