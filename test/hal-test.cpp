#include "test_common.h"
extern "C" {
#include "../package/pikascript/pikascript-lib/PikaStdDevice/pika_hal.h"

int pika_hal_platform_GPIO_open(pika_dev* dev, char* name){
    return 0;
}

int pika_hal_platform_GPIO_close(pika_dev* dev){
    return 0;
}

}


TEST(hal, open) {
    pika_dev* io = pika_hal_open(PIKA_HAL_GPIO, "PA10");
    pika_hal_close(io);
}
