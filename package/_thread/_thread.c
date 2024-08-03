#include "_thread.h"
#include "PikaVM.h"
#include "TinyObj.h"
#include "dataMemory.h"

static volatile int g_thread_stack_size = PIKA_THREAD_STACK_SIZE;
#if !PIKA_THREAD_MALLOC_STACK_ENABLE
extern volatile PikaMemInfo g_PikaMemInfo;
#endif

#if PIKA_FREERTOS_ENABLE
static pika_platform_thread_t* g_thread_to_free = NULL;
static pika_platform_thread_mutex_t g_thread_idle_hook_mutex = {0};
static int g_thread_idle_hook_mutex_inited = 0;

#if PIKA_THREAD_MALLOC_STACK_ENABLE
void pika_thread_idle_hook(void) {
    if (0 == g_thread_idle_hook_mutex_inited) {
        pika_platform_thread_mutex_init(&g_thread_idle_hook_mutex);
        g_thread_idle_hook_mutex_inited = 1;
    }
    pika_platform_thread_mutex_lock(&g_thread_idle_hook_mutex);
    if (NULL != g_thread_to_free) {
        pika_platform_thread_exit(g_thread_to_free);
        pikaFree(g_thread_to_free->thread_stack,
                 g_thread_to_free->thread_stack_size);
        pikaFree(g_thread_to_free, sizeof(pika_platform_thread_t));
        g_thread_to_free = NULL;
    }
    pika_platform_thread_mutex_unlock(&g_thread_idle_hook_mutex);
    return;
}
#endif

#endif
typedef struct pika_thread_info {
    Arg* function;
    Arg* args;
    pika_platform_thread_t* thread;
    int stack_size;
} pika_thread_info;

static void _thread_func(void* arg) {
    pika_debug("thread start");
    pika_GIL_ENTER();
    PikaObj* ctx = New_TinyObj(NULL);
    pika_thread_info* info = (pika_thread_info*)arg;
    if (NULL != info->args) {
        obj_setArg(ctx, "args", info->args);
    }
    obj_setArg(ctx, "thread", info->function);

    if (NULL == info->args) {
        /* clang-format off */
        PIKA_PYTHON(
        thread()
        )
        /* clang-format on */
        const uint8_t bytes[] = {
            0x04, 0x00, 0x00, 0x00, /* instruct array size */
            0x00, 0x82, 0x01, 0x00, /* instruct array */
            0x08, 0x00, 0x00, 0x00, /* const pool size */
            0x00, 0x74, 0x68, 0x72, 0x65, 0x61, 0x64, 0x00, /* const pool */
        };
        pikaVM_runByteCode(ctx, (uint8_t*)bytes);
    } else {
        /* clang-format off */
        PIKA_PYTHON(
            thread(*args)
        )
        /* clang-format on */
        const uint8_t bytes[] = {
            0x0c, 0x00, 0x00, 0x00, /* instruct array size */
            0x20, 0x81, 0x01, 0x00, 0x10, 0x08, 0x06, 0x00, 0x00, 0x02, 0x08,
            0x00,
            /* instruct array */
            0x0f, 0x00, 0x00, 0x00, /* const pool size */
            0x00, 0x61, 0x72, 0x67, 0x73, 0x00, 0x2a, 0x00, 0x74, 0x68, 0x72,
            0x65, 0x61, 0x64, 0x00, /* const pool */
        };
        pikaVM_runByteCode(ctx, (uint8_t*)bytes);
    }
    obj_deinit(ctx);
    arg_deinit(info->function);
    if (NULL != info->args) {
        arg_deinit(info->args);
    }
#if !PIKA_THREAD_MALLOC_STACK_ENABLE
    g_PikaMemInfo.heapUsed -= info->stack_size;
#endif
    pika_debug("thread exiting");
    pika_platform_thread_t* thread = info->thread;
    pikaFree(info, sizeof(pika_thread_info));
    pika_GIL_EXIT();
#if PIKA_FREERTOS_ENABLE
#if PIKA_THREAD_MALLOC_STACK_ENABLE
    if (0 == g_thread_idle_hook_mutex_inited) {
        pika_platform_thread_mutex_init(&g_thread_idle_hook_mutex);
        g_thread_idle_hook_mutex_inited = 1;
    }
    pika_platform_thread_mutex_lock(&g_thread_idle_hook_mutex);
    while (NULL != g_thread_to_free) {
        pika_platform_thread_mutex_unlock(&g_thread_idle_hook_mutex);
        vTaskDelay(10);
        pika_platform_thread_mutex_lock(&g_thread_idle_hook_mutex);
    }
    g_thread_to_free = thread;
    pika_platform_thread_mutex_unlock(&g_thread_idle_hook_mutex);
    while (1) {
        vTaskDelay(1);
    }
#else
    pikaFree(thread, sizeof(pika_platform_thread_t));
    pika_platform_thread_exit(NULL);
#endif
#else
    pika_platform_thread_exit(thread);
#endif
}

void _thread_start_new_thread(PikaObj* self, Arg* function, Arg* args_) {
    pika_thread_info* info =
        (pika_thread_info*)pikaMalloc(sizeof(pika_thread_info));
    pika_platform_memset(info, 0, sizeof(pika_thread_info));
    info->function = arg_copy(function);

    if (arg_isObject(args_)) {
        PikaObj* tuple = arg_getPtr(args_);
        size_t tuple_size = pikaTuple_getSize(tuple);
        pika_debug("type of args is %d", arg_getType(args_));
        pika_debug("new_thread: args tuple size %d", tuple_size);
        if (tuple_size > 0) {
            info->args = arg_copy(args_);
        }
    } else {
        info->args = arg_copy(args_);
    }

    _VM_lock_init();

    if (!_VM_is_first_lock()) {
        pika_debug("first lock for main thread");
        pika_GIL_ENTER();
        pika_debug("VM num %d", _VMEvent_getVMCnt());
    }

    info->stack_size = g_thread_stack_size;
    pika_debug("thread stack size %d", info->stack_size);
    info->thread = pika_platform_thread_init("pika_thread", _thread_func, info,
                                             info->stack_size, PIKA_THREAD_PRIO,
                                             PIKA_THREAD_TICK);
    if (NULL == info->thread) {
        pikaFree(info, sizeof(pika_thread_info));
        obj_setErrorCode(self, PIKA_RES_ERR_RUNTIME_ERROR);
        obj_setSysOut(self, "thread create failed");
        return;
    }
#if !PIKA_THREAD_MALLOC_STACK_ENABLE
    g_PikaMemInfo.heapUsed += info->stack_size;
#endif
}

int _thread_stack_size(PikaObj* self, PikaTuple* size) {
    if (pikaTuple_getSize(size) == 1) {
        int stack_size = pikaTuple_getInt(size, 0);
        if (stack_size == 0) {
            stack_size = PIKA_THREAD_STACK_SIZE;
        }
        g_thread_stack_size = stack_size;
    }
    return g_thread_stack_size;
}
