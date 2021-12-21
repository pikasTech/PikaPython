/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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

#include "pika_config.h"
#include "PikaPlatform.h"

/*============================ MACROS ========================================*/

#if VSF_USE_HEAP != ENABLED
#   error "Please enable VSF_USE_HEAP"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static unsigned int __crit_cnt = 0U;
static vsf_arch_prio_t __crit_saved = 0U;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void __platform_disable_irq_handle(void)
{
    if (0U == __crit_cnt) {
        __crit_saved = vsf_disable_interrupt();
    }
    __crit_cnt++;
}

void __platform_enable_irq_handle(void)
{
    __crit_cnt--;
    if (0U == __crit_cnt) {
        vsf_set_interrupt(__crit_saved);
    }
}

void * __platform_malloc(size_t size) {
    return vsf_heap_malloc(size);
}
void __platform_free(void* ptr) {
    vsf_heap_free(ptr);
}
