#include <string.h>

#include "pico/stdlib.h"

#include "pika_application.h"
#include "pika_platform.h"
#include "pikapython_modules.h"

static void pika_pico2_fail(const char* message) {
    (void)pika_platform_write(message, strlen(message));
    for (;;) {
        tight_loop_contents();
    }
}

int main(void) {
    PikaStatus status;

    stdio_init_all();
    status = pikaPythonInit();
    if (status != PIKA_STATUS_OK) {
        static const char message[] = "PikaPython init failed\r\n";
        pika_pico2_fail(message);
    }

#if PIKA_REPL_ENABLE
    status = pikaPythonRepl();
    if (status != PIKA_STATUS_OK) {
        static const char message[] = "PikaPython REPL failed\r\n";
        pika_pico2_fail(message);
    }
#endif

    pika_application_stop();
    for (;;) {
        tight_loop_contents();
    }
}
