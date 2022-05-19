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
#define __ENHANCED_BYTE_QUEUE_CLASS_IMPLEMENT
#define __BYTE_QUEUE_CLASS_INHERIT__
#include "./enhanced_byte_queue.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
#ifndef ASSERT
#   define ASSERT(...)
#endif

#undef this
#define this        (*ptThis)

#undef base
#define base        (*ptBase)
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
const i_enhanced_byte_queue_t ENHANCED_BYTE_QUEUE = {
    
    /*! override base interface */
    .use_as__i_byte_queue_t  = {
        .Init =     (byte_queue_t * (*)(byte_queue_t *, byte_queue_cfg_t *))
                            &enhanced_byte_queue_init,
        .Depose =   (void (*)   (byte_queue_t *ptObj)) 
                            &enhanced_byte_queue_depose,
        .Enqueue =  (bool (*)(byte_queue_t *, uint8_t ))
                            &enhanced_byte_queue_enqueue,
        .Dequeue =  (bool (*)(byte_queue_t *, uint8_t *))
                            &enhanced_byte_queue_dequeue,
        .Count =            &byte_queue_count,
        .Target  = {
            .Get =          &byte_queue_target_get,
            .Set =          &byte_queue_target_set,
        },
    },
    .Init =                 &enhanced_byte_queue_init,
    .Depose =               &enhanced_byte_queue_depose,
    .Enqueue =              &enhanced_byte_queue_enqueue,
    .Dequeue =              &enhanced_byte_queue_dequeue,
    .Count =          (uint_fast16_t(*)(enhanced_byte_queue_t *))
                            &byte_queue_count,
    .Target  = {
        .Get =        (void *(*)(enhanced_byte_queue_t *))
                            &byte_queue_target_get,
        .Set =        (void (*)(enhanced_byte_queue_t *, void *))
                            &byte_queue_target_set,
    },
    .Peek = {
        .PeekByte =         &enhanced_byte_queue_peek,
        .Reset =            &enhanced_byte_queue_peek_reset,
        .GetAllPeeked =     &enhanced_byte_queue_peek_get_all_peeked,
    },
};

/*============================ IMPLEMENTATION ================================*/

enhanced_byte_queue_t * enhanced_byte_queue_init(enhanced_byte_queue_t *ptObj, 
                                        byte_queue_cfg_t *ptCFG)
{
    class_internal(ptObj, ptThis, enhanced_byte_queue_t);
    //! call base constructor
    if (NULL == byte_queue_init(&this.use_as__byte_queue_t, ptCFG)) {
        return NULL;
    }
    
    printf("construct enhanced_byte_queue. \r\n");
    
    this.hwPeek = 0;
    this.hwPeekCount = 0;
    
    return ptObj;
}
void enhanced_byte_queue_depose(enhanced_byte_queue_t *ptObj)
{
    printf("destruct enhanced_byte_queue. \r\n");
    
    //! call base destructor
    byte_queue_depose(&(ptObj->use_as__byte_queue_t));

}

bool enhanced_byte_queue_enqueue(enhanced_byte_queue_t *ptObj, uint8_t chByte)
{
    /* initialise "this" (i.e. ptThis) to access class members */
    class_internal(ptObj, ptThis, enhanced_byte_queue_t);
    /* initialise "base" (i.e. ptBase) to access protected members */
    protected_internal(&this.use_as__byte_queue_t, ptBase, byte_queue_t);
    ASSERT(NULL != ptObj);
    
    /* ------------------atomicity sensitive start---------------- */
    if (!byte_queue_enqueue(&this.use_as__byte_queue_t, chByte)) {
        return false;
    }
    this.hwPeekCount++;
    /* ------------------atomicity sensitive end---------------- */
    
    return true;
}

bool enhanced_byte_queue_dequeue(enhanced_byte_queue_t *ptObj, uint8_t *pchByte)
{
    /* initialise "this" (i.e. ptThis) to access class members */
    class_internal(ptObj, ptThis, enhanced_byte_queue_t);
    /* initialise "base" (i.e. ptBase) to access protected members */
    protected_internal(&this.use_as__byte_queue_t, ptBase, byte_queue_t);
    ASSERT(NULL != ptObj);
    
    /* ------------------atomicity sensitive start---------------- */
    if (!byte_queue_dequeue(&this.use_as__byte_queue_t, pchByte)) {
        return false;
    }
    this.hwPeek = base.hwTail;
    this.hwPeekCount = base.hwCount;
    /* ------------------atomicity sensitive end---------------- */
    
    return true;
}

bool enhanced_byte_queue_peek(enhanced_byte_queue_t *ptObj, uint8_t *pchByte)
{
    /* initialise "this" (i.e. ptThis) to access class members */
    class_internal(ptObj, ptThis, enhanced_byte_queue_t);
    /* initialise "base" (i.e. ptBase) to access protected members */
    protected_internal(&this.use_as__byte_queue_t, ptBase, byte_queue_t);
    uint8_t chByte;
    ASSERT(NULL != ptObj);
    mem_t tBuffer;
    
    /* ------------------atomicity sensitive start---------------- */
    if (    (this.hwPeek == base.hwHead) 
        &&  (0 == this.hwPeekCount)) {
        //! empty
        return false;
    }
    tBuffer = byte_queue_buffer_get(&this.use_as__byte_queue_t);
    chByte = tBuffer.pchBuffer[this.hwPeek++];
    this.hwPeekCount--;
    if (this.hwPeek >= tBuffer.hwSize) {
        this.hwPeek = 0;
    }
    /* ------------------atomicity sensitive end---------------- */
    
    if (NULL != pchByte) {
        *pchByte = chByte;
    }

    return true;
}

void enhanced_byte_queue_peek_reset(enhanced_byte_queue_t *ptObj)
{
    /* initialise "this" (i.e. ptThis) to access class members */
    class_internal(ptObj, ptThis, enhanced_byte_queue_t);
    /* initialise "base" (i.e. ptBase) to access protected members */
    protected_internal(&this.use_as__byte_queue_t, ptBase, byte_queue_t);
    
    ASSERT(NULL != ptObj);
    /* ------------------atomicity sensitive start---------------- */
    this.hwPeek = base.hwTail;
    this.hwPeekCount = base.hwCount;
    /* ------------------atomicity sensitive end---------------- */
}

void enhanced_byte_queue_peek_get_all_peeked(enhanced_byte_queue_t *ptObj)
{
    /* initialise "this" (i.e. ptThis) to access class members */
    class_internal(ptObj, ptThis, enhanced_byte_queue_t);
    /* initialise "base" (i.e. ptBase) to access protected members */
    protected_internal(&this.use_as__byte_queue_t, ptBase, byte_queue_t);
    
    ASSERT(NULL != ptObj);
    /* ------------------atomicity sensitive start---------------- */
    base.hwCount = this.hwPeekCount;
    base.hwTail = this.hwPeek;
    /* ------------------atomicity sensitive end---------------- */
}

/* EOF */