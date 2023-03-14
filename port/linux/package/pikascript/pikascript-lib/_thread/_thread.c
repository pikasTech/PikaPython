#include "_thread.h"
#include "PikaVM.h"
#include "TinyObj.h"

typedef struct pika_thread_info {
    Arg* function;
    Arg* args;
    pika_platform_thread_t* thread;
} pika_thread_info;

static void _thread_func(void* arg) {
    pika_debug("waiting for first lock");
    while (1) {
        if (_VM_is_first_lock()) {
            break;
        }
        //! This May break the thread
        // if (_VMEvent_getVMCnt() <= 0) {
        //     break;
        // }
        pika_debug("VM num %d", _VMEvent_getVMCnt());
        pika_platform_thread_delay();
    }
    pika_debug("thread start");
    pika_GIL_ENTER();
    PikaObj* ctx = New_TinyObj(NULL);
    pika_thread_info* info = (pika_thread_info*)arg;
    obj_setArg(ctx, "args", info->args);
    obj_setArg(ctx, "thread", info->function);
    /* clang-format off */
    PIKA_PYTHON(
        thread(*args)
    )
    /* clang-format on */
    const uint8_t bytes[] = {
        0x0c, 0x00, 0x00, 0x00, /* instruct array size */
        0x20, 0x81, 0x01, 0x00, 0x10, 0x08, 0x06, 0x00, 0x00, 0x02, 0x08, 0x00,
        /* instruct array */
        0x0f, 0x00, 0x00, 0x00, /* const pool size */
        0x00, 0x61, 0x72, 0x67, 0x73, 0x00, 0x2a, 0x00, 0x74, 0x68, 0x72, 0x65,
        0x61, 0x64, 0x00, /* const pool */
    };
    pikaVM_runByteCode(ctx, (uint8_t*)bytes);
    obj_deinit(ctx);
    arg_deinit(info->function);
    arg_deinit(info->args);
    pika_debug("thread exiting");
    pika_platform_thread_t* thread = info->thread;
    pikaFree(info, sizeof(pika_thread_info));
    pika_GIL_EXIT();
#if PIKA_FREERTOS_ENABLE
    pikaFree(thread, sizeof(pika_platform_thread_t));
    pika_platform_thread_exit(NULL);
#else
    pika_platform_thread_exit(thread);
#endif
}

void _thread_start_new_thread(PikaObj* self, Arg* function, Arg* args_) {
    pika_thread_info* info =
        (pika_thread_info*)pikaMalloc(sizeof(pika_thread_info));
    info->function = arg_copy(function);
    info->args = arg_copy(args_);
    _VM_lock_init();
    info->thread = pika_platform_thread_init(
        "pika_thread", _thread_func, info, PIKA_THREAD_STACK_SIZE,
        PIKA_THREAD_PRIO, PIKA_THREAD_TICK);
}
