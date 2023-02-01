#include "_thread.h"
#include "PikaVM.h"

typedef struct pika_thread_info {
    Arg* function;
    Arg* args;
    pika_platform_thread_t* thread;
} pika_thread_info;

static void _thread_func(void* arg) {
    while (!_VM_is_first_lock()) {
        pika_platform_thread_delay();
    }
    _VM_lock();
    PikaObj* ctx = New_PikaObj();
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
    pika_platform_thread_destroy(info->thread);
    pikaFree(info, sizeof(pika_thread_info));
    _VM_unlock();
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
