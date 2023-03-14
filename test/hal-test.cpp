#include "test_common.h"

TEST(hal, open) {
    pika_dev* io = pika_hal_open(PIKA_HAL_GPIO, "PA10");
    pika_hal_close(io);
}
