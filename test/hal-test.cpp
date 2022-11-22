#include "test_common.h"
extern "C" {
#include "../package/pikascript/pikascript-lib/PikaStdDevice/pika_hal.h"
}

TEST(hal, open) {
    pika_dev* io = pika_hal_open(PIKA_HAL_GPIO, "PA10");
    EXPECT_EQ(0, (uintptr_t)io);
}
