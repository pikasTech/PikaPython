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
 * Title:        arm-2d_async.c
 * Description:  Pixel pipeline extensions for support hardware acceleration.
 *
 * $Date:        29. April 2021
 * $Revision:    V.0.8.0
 *
 * Target Processor:  Cortex-M cores
 *
 * -------------------------------------------------------------------- */


/*============================ INCLUDES ======================================*/
#define __ARM_2D_IMPL__

#include "arm_2d.h"
#include "__arm_2d_impl.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__ARM_2D_HAS_ASYNC__) && __ARM_2D_HAS_ASYNC__

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wcast-align"
#   pragma clang diagnostic ignored "-Wextra-semi-stmt"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wunused-function"
#   pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"
#   pragma clang diagnostic ignored "-Wdouble-promotion"
#   pragma clang diagnostic ignored "-Wunused-parameter"
#   pragma clang diagnostic ignored "-Wimplicit-float-conversion"
#   pragma clang diagnostic ignored "-Wimplicit-int-conversion"
#   pragma clang diagnostic ignored "-Wtautological-pointer-compare"
#   pragma clang diagnostic ignored "-Wsign-compare"
#   pragma clang diagnostic ignored "-Wmissing-prototypes"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wswitch-enum"
#   pragma clang diagnostic ignored "-Wswitch"
#   pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#   pragma clang diagnostic ignored "-Wgnu-statement-expression"
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#   pragma diag_suppress 174,177,188,68,513,144
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wswitch"
#   pragma GCC diagnostic ignored "-Wenum-compare"
#   pragma GCC diagnostic ignored "-Wpedantic"
#   pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif

/*============================ MACROS ========================================*/
#ifndef __ARM_2D_DEFAULT_SUB_TASK_POOL_SIZE
#   define __ARM_2D_DEFAULT_SUB_TASK_POOL_SIZE        4
#endif
#if __ARM_2D_DEFAULT_SUB_TASK_POOL_SIZE < 4
#   warning The __ARM_2D_DEFAULT_SUB_TASK_POOL_SIZE should be larger than or\
 equal to 3, set it to the default value 4.
#   undef __ARM_2D_DEFAULT_SUB_TASK_POOL_SIZE
#   define __ARM_2D_DEFAULT_SUB_TASK_POOL_SIZE    4
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

/*----------------------------------------------------------------------------*
 * Subtask Pool                                                               *
 *----------------------------------------------------------------------------*/

static void __arm_2d_sub_task_booking(uint_fast16_t hwCount)
{
    arm_irq_safe {
        ARM_2D_CTRL.hwBookCount += hwCount;
    }
}

static void __arm_2d_sub_task_cancel_booking(void)
{
    arm_irq_safe {
        ARM_2D_CTRL.hwBookCount = 0;
    }
}

static uint_fast16_t __arm_2d_sub_task_available_count(void)
{
    uint_fast16_t hwResult = 0;
    
    arm_irq_safe {
        hwResult = ARM_2D_CTRL.hwFreeCount;
        hwResult = (hwResult > ARM_2D_CTRL.hwBookCount)
                    ?   hwResult - ARM_2D_CTRL.hwBookCount
                    :   0;
    }
    
    return hwResult;
}

static
__arm_2d_sub_task_t *__arm_2d_sub_task_new(void)
{
    __arm_2d_sub_task_t *ptTask = NULL;
    
    arm_irq_safe {    
        if (NULL != ARM_2D_CTRL.ptFreeList) {
            ARM_LIST_STACK_POP(ARM_2D_CTRL.ptFreeList, ptTask);
            if (ARM_2D_CTRL.hwBookCount) {
                ARM_2D_CTRL.hwBookCount--;
            }
            
            ARM_2D_CTRL.hwFreeCount--;
        }
    }
    
    memset(ptTask, 0 ,sizeof(__arm_2d_sub_task_t));
    
    return ptTask;
}

static 
void __arm_2d_sub_task_free(__arm_2d_sub_task_t *ptTask)
{
    if (NULL == ptTask) {
        return ;
    }
    
    arm_irq_safe {
        ARM_LIST_STACK_PUSH(ARM_2D_CTRL.ptFreeList, ptTask);
        ARM_2D_CTRL.hwFreeCount++;
    }
}

/*----------------------------------------------------------------------------*
 * Subtask FIFO                                                               *
 *----------------------------------------------------------------------------*/
#if 0
__WEAK 
void arm_2d_notif_sub_task_fifo_task_arrive(void)
{
}
#endif

static 
void __arm_2d_sub_task_add(__arm_2d_sub_task_t *ptTask)
{
    assert(NULL != ptTask);
    //bool bIsEmpty = false;
    arm_irq_safe {
        //bIsEmpty = (0 == ARM_2D_CTRL.hwTaskCount);
        
        ARM_LIST_QUEUE_ENQUEUE( ARM_2D_CTRL.TaskFIFO.ptHead, 
                                ARM_2D_CTRL.TaskFIFO.ptTail,
                                ptTask);
        ARM_2D_CTRL.hwTaskCount++;
    }
#if 0
    if (bIsEmpty) {
        arm_2d_notif_sub_task_fifo_task_arrive();
    }
#endif
}

static 
__arm_2d_sub_task_t * __arm_2d_sub_task_fetch(void)
{
    __arm_2d_sub_task_t *ptTask = NULL;
    
    arm_irq_safe {
        if (NULL != ARM_2D_CTRL.TaskFIFO.ptHead) {
            ARM_LIST_QUEUE_DEQUEUE( ARM_2D_CTRL.TaskFIFO.ptHead, 
                                    ARM_2D_CTRL.TaskFIFO.ptTail,
                                    ptTask);
            ARM_2D_CTRL.hwTaskCount--;
        }
    }
    
    return ptTask;
}


/*----------------------------------------------------------------------------*
 * Subtask Processing                                                         *
 *----------------------------------------------------------------------------*/
 
static 
arm_fsm_rt_t __arm_2d_call_default_io(  __arm_2d_sub_task_t *ptTask, 
                                        uint_fast8_t chInterfaceIndex)
{
    assert(NULL != ptTask);

    arm_fsm_rt_t tResult = (arm_fsm_rt_t)ARM_2D_ERR_NOT_SUPPORT;
    ARM_2D_IMPL(arm_2d_op_t, ptTask->ptOP);
    uint_fast8_t chAccPreference = OP_CORE.Preference.u2ACCMethods;
    
    do {
        if (chAccPreference != ARM_2D_PREF_ACC_SW_ONLY) {
        /* call hardware accelerator */
        /*! \note the HW accelerator service routine will check the paratmer
         *!       and decide whether the requested service is supported or
         *!       not:
         *!       - if it is not supported, ARM_2D_ERR_NOT_SUPPORT will be
         *!         returned. 
         *!
         *!       - if it is supported and the service is to be done later,
         *!         the sub-task will be queued and arm_fsm_rt_async is 
         *!         returned. Once the sub-task is complete, 
         *!         __arm_2d_notify_sub_task_cpl() should be called by HW 
         *!         accelerator service routine/tasks.
         *!
         *!       - if it is supported and the service is complete immediately
         *!         the arm_fsm_rt_cpl is returned. 
         *!
         *!       - if there are any error detected, negative error code will be
         *!         returned. 
         */
             
            ARM_2D_TRY_ACCELERATION( chInterfaceIndex, __arm_2d_io_func_t ); 
                            
            assert(tResult != arm_fsm_rt_on_going);
            
            switch (tResult) {
                case arm_fsm_rt_wait_for_obj:
                    /*! \note the HW accelerator wants to sync-up with user 
                     *!       application
                     */
                    if (chAccPreference == ARM_2D_PREF_ACC_DONT_CARE) {
                        /* since people choose don't care, then use SW */
                        break;
                    }
                    return tResult;
                    
                case arm_fsm_rt_on_going:
                    /*! \note HW accelerator should NOT return on_going otherwise
                     *!       it will be treated as ARM_2D_ERR_IO_BUSY
                     */
                    if (chAccPreference == ARM_2D_PREF_ACC_DONT_CARE) {
                        /* since people choose don't care, then use SW */
                        break;
                    }
                    return (arm_fsm_rt_t)ARM_2D_ERR_IO_BUSY;
                    
                case ARM_2D_ERR_NOT_SUPPORT:
                    break;
                
                case arm_fsm_rt_async:
                case arm_fsm_rt_cpl:
                default:
                    /* other errors */
                    return tResult;
            }
             
            if (    (chAccPreference == ARM_2D_PREF_ACC_HW_ONLY)  
                &&  (ARM_2D_ERR_NOT_SUPPORT == tResult)) {                
                //! the hardware acceleration isn't avaialble
                break;
            }
        }

        /* call default software implementation */
        /*! \note the default software implemenation will only return following
         *!       values:
         *!       - arm_fsm_rt_cpl          the service is complete.
         *!       - arm_fsm_rt_on_going     the software algorithm wants to yield
         *!       - arm_fsm_rt_wait_for_obj the software algorithm wants to sync
         *!                                 with user application
         *!       - arm_fsm_rt_async        the software algorithm is implemented
         *!                                 in asynchronous mode
         *!       - negative error code
         */
         
        ARM_2D_RUN_DEFAULT(  chInterfaceIndex, __arm_2d_io_func_t  );
    } while(0);
    return tResult;
}


__WEAK 
void arm_2d_notif_aync_op_cpl(uintptr_t pUserParam)
{
}


static void __arm_2d_notify_op_cpl(arm_2d_op_core_t *ptOP, arm_fsm_rt_t tResult)
{
    assert(tResult != arm_fsm_rt_async);
    assert(tResult != arm_fsm_rt_on_going);
    assert(tResult != arm_fsm_rt_wait_for_obj);
    
    //! error detected
    if (tResult < 0) {
        //! update error info
        ptOP->tResult = tResult;
        ptOP->Status.bIOError = true;
    }
    
    //! handle target OP

    
    if (0 == ptOP->Status.u4SubTaskCount) {
        //! this is the last sub task 
        
        //! no error has ever happened
        if (ptOP->tResult >= 0) {
            ptOP->tResult = tResult;
        }
        
        //! call Operation Complete event handler
        if (NULL != ptOP->evt2DOpCpl.fnHandler) {
            (ptOP->evt2DOpCpl.fnHandler) (  ptOP, 
                                            ptOP->tResult,
                                            ptOP->evt2DOpCpl.pTarget);
            ptOP->evt2DOpCpl.fnHandler = NULL;
        } 
        
        /*! \note complete doesn't mean no err */
        ptOP->Status.bOpCpl = true;
        
        //! reset preference
        ptOP->Preference.u2ACCMethods = 0;
        
        /*! arm_thread_safe */
        arm_irq_safe {
            //! only clear busy flag after bOpCpl is set properly.
            ptOP->Status.bIsBusy = false;
            
            arm_2d_notif_aync_op_cpl(ptOP->pUserParam);
        }
    }
}

__WEAK 
void arm_2d_notif_aync_sub_task_cpl(uintptr_t pUserParam)
{
}

/*! \note This API should be called by both arm_2d_task and hardware 
 *!       accelerator to indicate the completion of a sub task
 */
void __arm_2d_notify_sub_task_cpl(  __arm_2d_sub_task_t *ptTask, 
                                    arm_fsm_rt_t tResult,
                                    bool bFromHW)
{
    ARM_2D_UNUSED(bFromHW);
    
    arm_2d_op_core_t *ptOP = ptTask->ptOP;
    
    assert(NULL != ptTask);
    assert(NULL != ptOP);
    assert(ptOP->Status.u4SubTaskCount > 0);

    //! free sub task
    __arm_2d_sub_task_free(ptTask);
    
    //if (bFromHW) {
        arm_2d_notif_aync_sub_task_cpl(ptOP->pUserParam);
    //}
    
    ptOP->Status.u4SubTaskCount--;
    __arm_2d_notify_op_cpl(ptOP, tResult);
    
}

/*! \note You can override this to add support for new types of interface
 *! \param ptTask pointer of target sub task
 *! \return the result
 */ 
arm_fsm_rt_t __arm_2d_sub_task_dispatch(__arm_2d_sub_task_t *ptTask)
{

    arm_fsm_rt_t tResult = (arm_fsm_rt_t)ARM_2D_ERR_INVALID_OP;
    //ARM_2D_IMPL(arm_2d_op_t, ptTask->ptOP);
    
    //if (ptTask->chLowLeveIOIndex < dimof(__ARM_2D_IO_TABLE.OP)) {
        tResult = __arm_2d_call_default_io( ptTask, ptTask->chLowLeveIOIndex);
    //}
    
    return tResult;

}




#define __ARM_2D_BACKEND_TASK_RESET_FSM()     do {this.chState = START;} while(0);

 /*! \brief arm-2d pixel pipeline backend task entery
  *! \note  This function is *TRHEAD-SAFE*
  *! \param none
  *! \retval arm_fsm_rt_cpl The sub-task FIFO is empty, the caller, i.e. the host
  *!            RTOS thread can block itself by waiting for a semaphore which is
  *!            set by arm_2d_notif_sub_task_fifo_task_arrive()
  *! \retval arm_fsm_rt_on_going The arm_2d_task issued one sub-task without 
  *!            problem and it yields. 
  *! \retval arm_fsm_rt_async You shouldn't see this value
  *! \retval arm_fsm_rt_wait_for_obj some algorithm or hardware accelerator wants
  *!            to sync-up with applications.
  *! \retval (<0) Serious error is detected.
  */
static 
arm_fsm_rt_t __arm_2d_backend_task(arm_2d_task_t *ptThis)
{
    
    enum {
        START = 0,
        FETCH,
        DISPATCH,
    };
    
    switch(this.chState) {
        case START:
            this.tResult = (arm_fsm_rt_t)ARM_2D_ERR_INVALID_OP;
            this.chState++;
            //break;
        
        case FETCH:
        
            //! fetch a sub task from FIFO
            this.ptTask = __arm_2d_sub_task_fetch();
            if (NULL == this.ptTask) {
                __ARM_2D_BACKEND_TASK_RESET_FSM();
                return arm_fsm_rt_cpl;
            }
            this.chState++;
            //break;
            
        case DISPATCH:

            //! dispatch sub tasks
            this.tResult = 
                __arm_2d_sub_task_dispatch((__arm_2d_sub_task_t *)this.ptTask);

            if (    (arm_fsm_rt_on_going == this.tResult)                       //!< sub task wants to yield
                ||  (arm_fsm_rt_wait_for_obj == this.tResult)) {                //!< sub task wants to sync-up with applications
                return this.tResult;
            }
            
            if (this.tResult != arm_fsm_rt_async) {
                //! get result immediately
                __arm_2d_notify_sub_task_cpl(   
                                            (__arm_2d_sub_task_t *)this.ptTask, 
                                            this.tResult, 
                                            false); 
            } 
            /*! \note (arm_fsm_rt_async == tResult) means the sub task hasn't 
             *!       been handled yet 
             */
            
            __ARM_2D_BACKEND_TASK_RESET_FSM();
            
            //! unsupported operation
            if (ARM_2D_ERR_INVALID_OP == this.tResult){
                return this.tResult;
            }
            
            break;
    }
    
    
    return arm_fsm_rt_on_going;
}

 /*! \brief arm-2d pixel pipeline backend task entery
  *! \note  This function is *TRHEAD-SAFE*
  *! \param none
  *! \retval arm_fsm_rt_cpl The OPCODE FIFO is empty, the caller, i.e. the host
  *!            RTOS thread can block itself by waiting for a semaphore which is
  *!            set by arm_2d_notif_sub_task_fifo_task_arrive()
  *! \retval arm_fsm_rt_on_going The frontend issued one OPCODE without 
  *!            problem and it yields. 
  *! \retval arm_fsm_rt_async You shouldn't see this value
  *! \retval arm_fsm_rt_wait_for_obj some algorithm or hardware accelerator wants
  *!            to sync-up with applications.
  *! \retval (<0) Serious error is detected.
  */
static 
arm_fsm_rt_t __arm_2d_frontend_task(arm_2d_task_t *ptThis)
{
    arm_2d_op_core_t *ptOP = NULL;
    arm_fsm_rt_t tResult;
    
    arm_irq_safe {
        ARM_LIST_QUEUE_PEEK(ARM_2D_CTRL.OPFIFO.ptHead, 
                            ARM_2D_CTRL.OPFIFO.ptTail,
                            ptOP);
    }
    
    if (NULL == ptOP) {
        return arm_fsm_rt_cpl;
    }

    tResult = __arm_2d_op_frontend_op_decoder(ptOP);
    
    if ((arm_fsm_rt_cpl == tResult) || (tResult < 0)) {

        arm_irq_safe {
            ARM_LIST_QUEUE_DEQUEUE( ARM_2D_CTRL.OPFIFO.ptHead, 
                                    ARM_2D_CTRL.OPFIFO.ptTail,
                                    ptOP);
        }
        ptOP->Status.u4SubTaskCount = 0;
        __arm_2d_notify_op_cpl(ptOP, tResult);
    } else if (arm_fsm_rt_async == tResult) {
        arm_irq_safe {
            ARM_LIST_QUEUE_DEQUEUE( ARM_2D_CTRL.OPFIFO.ptHead, 
                                    ARM_2D_CTRL.OPFIFO.ptTail,
                                    ptOP);
        }
    }
    
    /* release resources here */
    __arm_2d_sub_task_cancel_booking();
    
    if (    arm_fsm_rt_wait_for_obj == tResult 
        ||  arm_fsm_rt_wait_for_res == tResult) {
        return tResult;
    }

    assert(arm_fsm_rt_on_going != tResult);

    return arm_fsm_rt_on_going;
}


 /*! \brief arm-2d pixel pipeline task entery
  *! \note  This function is *TRHEAD-SAFE*
  *! \param none
  *! \retval arm_fsm_rt_cpl The sub-task FIFO is empty, the caller, i.e. the host
  *!            RTOS thread can block itself by waiting for a semaphore which is
  *!            set by arm_2d_notif_sub_task_fifo_task_arrive()
  *! \retval arm_fsm_rt_on_going The arm_2d_task issued one sub-task without 
  *!            problem and it yields. 
  *! \retval arm_fsm_rt_async You shouldn't see this value
  *! \retval arm_fsm_rt_wait_for_obj some algorithm or hardware accelerator wants
  *!            to sync-up with applications.
  *! \retval (<0) Serious error is detected.
  */
arm_fsm_rt_t arm_2d_task(arm_2d_task_t *ptThis)
{
    arm_fsm_rt_t tResult;
    
    do {
        tResult = __arm_2d_backend_task(ptThis);
        
        if (arm_fsm_rt_cpl != tResult) {
            break;
        }
        
        tResult = __arm_2d_frontend_task(ptThis);
        if (arm_fsm_rt_on_going == tResult){
            continue;
        }
        
        break;
        
    } while(true);
    
    return tResult;
}


/*----------------------------------------------------------------------------*
 * Overridden Implementations                                                 *
 *----------------------------------------------------------------------------*/

__OVERRIDE_WEAK
arm_fsm_rt_t __arm_2d_op_frontend_on_leave( arm_2d_op_core_t *ptThis, 
                                            arm_fsm_rt_t tResult)
{  

    
    if (!ARM_2D_RUNTIME_FEATURE.HAS_DEDICATED_THREAD_FOR_2D_TASK) {
        arm_fsm_rt_t tTaskResult;
        arm_2d_task_t tTaskCB = {0};
        do {
            tTaskResult = arm_2d_task(&tTaskCB);
        } while(arm_fsm_rt_on_going == tTaskResult);
        
        if (tTaskResult < 0) {
            //! a serious error is detected
            tResult = tTaskResult;
        } else if (arm_fsm_rt_wait_for_obj == tTaskResult) {
            tResult = tTaskResult;
        } else {
            tResult = this.tResult;
        }
    }
   
    return tResult;
}

__WEAK 
void arm_2d_notif_new_op_arrive(uintptr_t pUserParam)
{
}


__OVERRIDE_WEAK
arm_fsm_rt_t __arm_2d_op_frontend(arm_2d_op_core_t *ptThis)
{
    bool bEmptyQueue = false;
    arm_fsm_rt_t tResult;
    arm_irq_safe {
        if (NULL == ARM_2D_CTRL.OPFIFO.ptHead) {
            bEmptyQueue = true;
        }
        
        ARM_LIST_QUEUE_ENQUEUE( ARM_2D_CTRL.OPFIFO.ptHead, 
                                ARM_2D_CTRL.OPFIFO.ptTail,
                                ptThis);
    }
    
    tResult =  __arm_2d_op_frontend_on_leave(ptThis, this.tResult);

    if (bEmptyQueue) {
        arm_2d_notif_new_op_arrive(this.pUserParam);
    }

    return tResult;
}

__OVERRIDE_WEAK
bool __arm_2d_op_ensure_resource(   arm_2d_op_core_t *ptOP, 
                                    uint_fast16_t hwRequired)
{
    bool bResult = false;
    
    ARM_2D_UNUSED(ptOP);
    
    arm_irq_safe {
        bResult = (__arm_2d_sub_task_available_count() >= hwRequired);
        if (bResult) {
            __arm_2d_sub_task_booking(hwRequired);
        }
    }

    return bResult;
}



__OVERRIDE_WEAK
arm_fsm_rt_t __arm_2d_issue_sub_task_tile_process(  
                                        arm_2d_op_t *ptThis,
                                        __arm_2d_tile_param_t *ptParam)
{    
    __arm_2d_sub_task_t *ptTask = __arm_2d_sub_task_new();
    assert(NULL != ptTask);         

    (*ptTask) = (__arm_2d_sub_task_t) {
                    .ptOP = &(ptThis->use_as__arm_2d_op_core_t),
                    .chLowLeveIOIndex = 0,
                    .Param.tTileProcess = *ptParam,
                };
    
    OP_CORE.Status.u4SubTaskCount++;
    
    __arm_2d_sub_task_add(ptTask);
    
    return arm_fsm_rt_async;
}

__OVERRIDE_WEAK
arm_fsm_rt_t __arm_2d_issue_sub_task_fill(
                                    arm_2d_op_cp_t *ptThis,
                                    __arm_2d_tile_param_t *ptSource,
                                    __arm_2d_tile_param_t *ptTarget)
{
    __arm_2d_sub_task_t *ptTask = __arm_2d_sub_task_new();
    assert(NULL != ptTask);         

    (*ptTask) = (__arm_2d_sub_task_t) {
                    .ptOP = &(ptThis->use_as__arm_2d_op_core_t),
                    .chLowLeveIOIndex = 1,
                    .Param.tFill = {
                        .tSource    = *ptSource,
                        .tTarget    = *ptTarget,
                    },
                };
    
    OP_CORE.Status.u4SubTaskCount++;
    
    __arm_2d_sub_task_add(ptTask);

    return arm_fsm_rt_async;
}

__OVERRIDE_WEAK
arm_fsm_rt_t __arm_2d_issue_sub_task_fill_with_mask(
                                    arm_2d_op_cp_t *ptThis,
                                    __arm_2d_tile_param_t *ptSource,
                                    __arm_2d_tile_param_t *ptSourceMask,
                                    __arm_2d_tile_param_t *ptTarget,
                                    __arm_2d_tile_param_t *ptTargetMask)
{
    __arm_2d_sub_task_t *ptTask = __arm_2d_sub_task_new();
    assert(NULL != ptTask);         

    (*ptTask) = (__arm_2d_sub_task_t) {
                    .ptOP = &(ptThis->use_as__arm_2d_op_core_t),
                    .chLowLeveIOIndex = 1,
                    .Param.tFillMask = {
                        .use_as____arm_2d_param_fill_t = {
                            .tSource        = *ptSource,
                            .tTarget        = *ptTarget,
                        },
                    },
                };
    
    if (NULL == ptSourceMask){
        ptTask->Param.tFillMask.tSrcMask.bInvalid = true;
    } else {
        ptTask->Param.tFillMask.tSrcMask = *ptSourceMask;
    }
    
    if (NULL == ptTargetMask){
        ptTask->Param.tFillMask.tDesMask.bInvalid = true;
    } else {
        ptTask->Param.tFillMask.tDesMask = *ptTargetMask;
    }
    
    OP_CORE.Status.u4SubTaskCount++;
    
    __arm_2d_sub_task_add(ptTask);

    return arm_fsm_rt_async;
}



__OVERRIDE_WEAK
arm_fsm_rt_t __arm_2d_issue_sub_task_copy(
                                    arm_2d_op_cp_t *ptThis,
                                    __arm_2d_tile_param_t *ptSource,
                                    __arm_2d_tile_param_t *ptTarget,
                                    arm_2d_size_t * ptCopySize)
{
    __arm_2d_sub_task_t *ptTask = __arm_2d_sub_task_new();
    assert(NULL != ptTask);         

    (*ptTask) = (__arm_2d_sub_task_t) {
                    .ptOP = &(ptThis->use_as__arm_2d_op_core_t),
                    .chLowLeveIOIndex = 0,
                    .Param.tCopy = {
                        .tSource            = *ptSource,
                        .tTarget            = *ptTarget,
                        .tCopySize          = *ptCopySize,
                    },
                };
    OP_CORE.Status.u4SubTaskCount++;
    
    __arm_2d_sub_task_add(ptTask);

    return arm_fsm_rt_async;
}

__OVERRIDE_WEAK
arm_fsm_rt_t __arm_2d_issue_sub_task_copy_with_mask(
                                    arm_2d_op_cp_t *ptThis,
                                    __arm_2d_tile_param_t *ptSource,
                                    __arm_2d_tile_param_t *ptSourceMask,
                                    __arm_2d_tile_param_t *ptTarget,
                                    __arm_2d_tile_param_t *ptTargetMask,
                                    arm_2d_size_t * __RESTRICT ptCopySize)
{

    __arm_2d_sub_task_t *ptTask = __arm_2d_sub_task_new();
    assert(NULL != ptTask);         

    (*ptTask) = (__arm_2d_sub_task_t) {
                    .ptOP = &(ptThis->use_as__arm_2d_op_core_t),
                    .chLowLeveIOIndex = 0,
                    .Param.tCopyMask = {
                        .use_as____arm_2d_param_copy_t = {
                            .tSource        = *ptSource,
                            .tTarget        = *ptTarget,
                            .tCopySize      = *ptCopySize,
                        },
                    },
                };
    
    if (NULL == ptSourceMask){
        ptTask->Param.tCopyMask.tSrcMask.bInvalid = true;
    } else {
        ptTask->Param.tCopyMask.tSrcMask = *ptSourceMask;
    }
    
    if (NULL == ptTargetMask){
        ptTask->Param.tCopyMask.tDesMask.bInvalid = true;
    } else {
        ptTask->Param.tCopyMask.tDesMask = *ptTargetMask;
    }
    
    OP_CORE.Status.u4SubTaskCount++;
    
    __arm_2d_sub_task_add(ptTask);

    return arm_fsm_rt_async;
}


__OVERRIDE_WEAK
arm_fsm_rt_t __arm_2d_issue_sub_task_fill_origin(
                                    arm_2d_op_cp_t *ptThis,
                                    __arm_2d_tile_param_t *ptSource,
                                    __arm_2d_tile_param_t *ptOrigin,
                                    __arm_2d_tile_param_t *ptTarget)
{
    __arm_2d_sub_task_t *ptTask = __arm_2d_sub_task_new();
    assert(NULL != ptTask);         

    (*ptTask) = (__arm_2d_sub_task_t) {
                    .ptOP = &(ptThis->use_as__arm_2d_op_core_t),
                    .chLowLeveIOIndex = 1,
                    .Param.tFillOrig = {
                        .use_as____arm_2d_param_fill_t = {
                            .tSource    = *ptSource,
                            .tTarget    = *ptTarget,
                        },
                        .tOrigin        = *ptOrigin,
                    },
                };
    
    OP_CORE.Status.u4SubTaskCount++;
    
    __arm_2d_sub_task_add(ptTask);

    return arm_fsm_rt_async;
}

__OVERRIDE_WEAK
arm_fsm_rt_t __arm_2d_issue_sub_task_copy_origin(
                                    arm_2d_op_cp_t *ptThis,
                                    __arm_2d_tile_param_t *ptSource,
                                    __arm_2d_tile_param_t *ptOrigin,
                                    __arm_2d_tile_param_t *ptTarget,
                                    arm_2d_size_t * __RESTRICT ptCopySize)
{
    __arm_2d_sub_task_t *ptTask = __arm_2d_sub_task_new();
    assert(NULL != ptTask);         

    (*ptTask) = (__arm_2d_sub_task_t) {
                    .ptOP = &(ptThis->use_as__arm_2d_op_core_t),
                    .chLowLeveIOIndex = 0,
                    .Param.tCopyOrig = {
                        .use_as____arm_2d_param_copy_t = {
                            .tSource            = *ptSource,
                            .tTarget            = *ptTarget,
                            .tCopySize          = *ptCopySize,
                        },
                        .tOrigin                = *ptOrigin,
                    },
                };
    OP_CORE.Status.u4SubTaskCount++;
    
    __arm_2d_sub_task_add(ptTask);

    return arm_fsm_rt_async;
}




/*! \brief initialise the whole arm-2d service
 *! \param none
 *! \return none
 */
void __arm_2d_async_init(void)
{   
    //! initialise sub task pool
    do {
        static __arm_2d_sub_task_t 
            s_tDefaultTaskPool[__ARM_2D_DEFAULT_SUB_TASK_POOL_SIZE];
        
        arm_foreach(__arm_2d_sub_task_t, s_tDefaultTaskPool) {
            __arm_2d_sub_task_free(_);
        }
        
    } while(0);
}


__WEAK
bool arm_2d_port_wait_for_async(uintptr_t pUserParam)
{
    return false;
}

__OVERRIDE_WEAK
/*! \brief sync up with operation 
 *! \retval true operation is busy
 *! \retval false operation isn't busy
 */
bool arm_2d_op_wait_async(arm_2d_op_core_t *ptOP)
{
    ARM_2D_IMPL(arm_2d_op_core_t, ptOP)

    volatile arm_2d_op_status_t *ptStatus 
        = (volatile arm_2d_op_status_t *)&(this.Status);
        
    bool bIsBusy = false;
    do {
        bIsBusy = ptStatus->bIsBusy;
        
        if (!bIsBusy) {
            break;
        }
        
        if (!arm_2d_port_wait_for_async(this.pUserParam)) {
            break;
        }
    } while(bIsBusy);
    
    return !bIsBusy;
}


__OVERRIDE_WEAK
/*! \brief sync up with operation 
 *! \retval true operation is busy
 *! \retval false operation isn't busy
 */
bool __arm_2d_op_acquire(arm_2d_op_core_t *ptOP)
{
    ARM_2D_IMPL(arm_2d_op_core_t, ptOP)
    volatile arm_2d_op_status_t *ptStatus 
        = (volatile arm_2d_op_status_t *)&(this.Status);

    bool bResult = false;
    do {
        arm_irq_safe {
            bResult = ptStatus->bIsBusy;
            if (!bResult) {
                this.tResult = arm_fsm_rt_async;
                ptStatus->tValue = 0;                                           //! reset status
                ptStatus->bIsBusy = true;                                       //! set busy flag
            }
        }
        
        if (!bResult) {
            break;
        }

        if (!arm_2d_port_wait_for_async(this.pUserParam)) {
            break;
        }

    } while (bResult);
    
    
    
    return !bResult;
}



#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#   pragma diag_warning 174,177,188,68,513,144
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
