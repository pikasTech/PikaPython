/* SPEC: PJ2026-050110 PikaPython CLI v0.6; interactive QEMU startup. */
#include "pikapython_modules.h"
#include "pika_platform.h"

extern void pika_qemu_exit(int status);
extern void pika_qemu_report_resources(void);

int main(void) {
    PikaStatus status = pikaPythonInit();
    if (status != PIKA_STATUS_OK) {
        static const char message[] = "PikaPython init failed\n";
        (void)pika_platform_write(message, sizeof(message) - 1u);
        pika_qemu_report_resources();
        pika_qemu_exit(1);
    }
#if PIKA_REPL_ENABLE
    status = pikaPythonRepl();
    if (status != PIKA_STATUS_OK) {
        static const char message[] = "PikaPython REPL failed\n";
        (void)pika_platform_write(message, sizeof(message) - 1u);
        pika_qemu_report_resources();
        pika_qemu_exit(2);
    }
#endif
    pika_qemu_report_resources();
    pika_qemu_exit(0);
    return 0;
}
