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
#define __BYTE_QUEUE_CLASS_IMPLEMENT

#include "./byte_queue.h"
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

/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
const i_byte_queue_t BYTE_QUEUE = {
    .Init =             &byte_queue_init,
    .Depose =           &byte_queue_depose,
    .Enqueue =          &byte_queue_enqueue,
    .Dequeue =          &byte_queue_dequeue,
    .Count =            &byte_queue_count,
    .Target  = {
        .Get =          &byte_queue_target_get,
        .Set =          &byte_queue_target_set,
    },
};

/*============================ IMPLEMENTATION ================================*/


byte_queue_t * byte_queue_init(byte_queue_t *ptObj, byte_queue_cfg_t *ptCFG)
{
    /* initialise "this" (i.e. ptThis) to access class members */
    class_internal(ptObj, ptThis, byte_queue_t);
    
    ASSERT(NULL != ptObj && NULL != ptCFG);

    printf("construct byte_queue. \r\n");
    
    /* access inherited member of mem_t directly */
    if (    (NULL == ptCFG->use_as__mem_t.pchBuffer) 
        ||  (0 == ptCFG->use_as__mem_t.hwSize)) {

        return NULL;
    }
   
    
    memset(ptObj, 0, sizeof(byte_queue_t));     //! clear object
    /*
    this.hwHead = 0;
    this.hwTail = 0;
    this.hwCount = 0;
    */
    
    this.use_as__mem_t = ptCFG->use_as__mem_t;
    this.pTarget = ptCFG->pTarget;              //!< user target
    
    return ptObj;
}

void byte_queue_depose(byte_queue_t *ptObj)
{
    /*! nothing to do */
    printf("destruct byte_queue. \r\n");
    
    return ;
}

bool byte_queue_enqueue(byte_queue_t *ptObj, uint8_t chByte)
{
    /* initialise "this" (i.e. ptThis) to access class members */
    class_internal(ptObj, ptThis, byte_queue_t);
    
    ASSERT(NULL != ptObj);
    
    /* ------------------atomicity sensitive start---------------- */
    if (    (this.hwHead == this.hwTail)
        &&  (0 != this.hwCount)) {
        //! queue is full
        return false;
    }
    this.use_as__mem_t.pchBuffer[this.hwHead++] = chByte;
    this.hwCount++;
    if (this.hwHead >= this.use_as__mem_t.hwSize) {
        this.hwHead = 0;
    }
    /* ------------------atomicity sensitive end---------------- */
    
    return true;
}

bool byte_queue_dequeue(byte_queue_t *ptObj, uint8_t *pchByte)
{
    /* initialise "this" (i.e. ptThis) to access class members */
    class_internal(ptObj, ptThis, byte_queue_t);
    uint8_t chByte;
    ASSERT(NULL != ptObj);
    
    /* ------------------atomicity sensitive start---------------- */
    if (    (this.hwHead == this.hwTail)
        &&  (0 == this.hwCount)) {
        //! queue is empty
        return false;
    }
    
    chByte = this.use_as__mem_t.pchBuffer[this.hwTail++];
    this.hwCount--;
    if (this.hwTail >= this.use_as__mem_t.hwSize) {
        this.hwTail = 0;
    }
    /* ------------------atomicity sensitive end---------------- */
    
    if (NULL != pchByte) {
        *pchByte = chByte;
    }
    
    return true;
}

uint_fast16_t byte_queue_count(byte_queue_t *ptObj)
{
    class_internal(ptObj, ptThis, byte_queue_t);
    ASSERT(NULL != ptObj);
    
    return this.hwCount;
}

void *byte_queue_target_get(byte_queue_t *ptObj)
{
    class_internal(ptObj, ptThis, byte_queue_t);
    ASSERT(NULL != ptObj);
    
    return this.pTarget;
}

void  byte_queue_target_set(byte_queue_t *ptObj, void *pTarget)
{
    class_internal(ptObj, ptThis, byte_queue_t);
    ASSERT(NULL != ptObj);
    
    this.pTarget = pTarget;
}


mem_t byte_queue_buffer_get(byte_queue_t *ptObj)
{
    class_internal(ptObj, ptThis, byte_queue_t);
    ASSERT(NULL != ptObj);
    
    return this.use_as__mem_t;
}

/* EOF */