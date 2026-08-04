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
    pika_application_stop();
    return 0;
}
