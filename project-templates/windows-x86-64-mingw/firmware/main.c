#include "pika_application.h"
#include "pika_platform.h"
#include "pikapython_modules.h"

int main(void) {
    PikaStatus status = pikaPythonInit();
    if (status != PIKA_STATUS_OK) {
        static const char message[] = "PikaPython init failed\n";
        (void)pika_platform_write(message, sizeof(message) - 1u);
        return 1;
    }
#if PIKA_REPL_ENABLE
    status = pikaPythonRepl();
    if (status != PIKA_STATUS_OK) {
        static const char message[] = "PikaPython REPL failed\n";
        (void)pika_platform_write(message, sizeof(message) - 1u);
        pika_application_stop();
        return 2;
    }
    return 0;
#endif
    pika_application_stop();
    return 0;
}
