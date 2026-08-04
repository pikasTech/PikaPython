/* Linux console application startup. */
#include "pika_application.h"
#include "pikapython_modules.h"
#include "pika_platform.h"

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
#endif
    pika_application_stop();
    return 0;
}
