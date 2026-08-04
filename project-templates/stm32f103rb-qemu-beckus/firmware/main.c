#include "pikapython_modules.h"
#include "pika_capability_config.h"
#include "pika_platform.h"

#ifndef PIKA_QEMU_PWM_VERIFY_ENABLE
#define PIKA_QEMU_PWM_VERIFY_ENABLE 0
#endif

extern void pika_qemu_platform_initialize(void);
extern void pika_qemu_report_resources(void);
extern void pika_qemu_exit(int status);
#if PIKA_CAPABILITY_MACHINE_PWM_ENABLE && \
    PIKA_QEMU_PWM_VERIFY_ENABLE
extern PikaStatus pika_qemu_pwm_verify(void);
#endif

static void write_message(const char* data, size_t size) {
    (void)pika_platform_write(data, size);
}

int main(void) {
    PikaStatus status;
    static const char ready[] = "PIKA_F103_READY\n";
    pika_qemu_platform_initialize();
    status = pikaPythonInit();
    if (status != PIKA_STATUS_OK) {
        static const char message[] = "PikaPython init failed\n";
        write_message(message, sizeof(message) - 1u);
        pika_qemu_report_resources();
        pika_qemu_exit(1);
    }
    write_message(ready, sizeof(ready) - 1u);
#if PIKA_CAPABILITY_MACHINE_PWM_ENABLE && \
    PIKA_QEMU_PWM_VERIFY_ENABLE
    status = pika_qemu_pwm_verify();
    if (status != PIKA_STATUS_OK) {
        static const char message[] =
            "PikaPython PWM register verification failed\n";
        write_message(message, sizeof(message) - 1u);
        pika_qemu_report_resources();
        pika_qemu_exit(3);
    }
    {
        static const char verified[] =
            "PIKA_F103_PWM_VERIFIED\n";
        write_message(verified, sizeof(verified) - 1u);
    }
#endif
#if PIKA_REPL_ENABLE
    status = pikaPythonRepl();
    if (status != PIKA_STATUS_OK) {
        static const char message[] = "PikaPython REPL failed\n";
        write_message(message, sizeof(message) - 1u);
        pika_qemu_report_resources();
        pika_qemu_exit(2);
    }
#endif
    pika_qemu_report_resources();
    pika_qemu_exit(0);
    return 0;
}
