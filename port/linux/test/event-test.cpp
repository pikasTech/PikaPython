#include "test_common.h"

extern PikaEventListener* g_pika_device_event_listener;
#if PIKA_EVENT_ENABLE
TEST(event, gpio) {
    /* init */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    pikaVM_runSingleFile(pikaMain, "../../examples/TemplateDevice/gpio_cb.py");

#define EVENT_SIGAL_IO_RISING_EDGE 0x01
#define EVENT_SIGAL_IO_FALLING_EDGE 0x02
#define GPIO_PA8_EVENT_ID 0x08

    /* simulate run in the call back */
    pks_eventLisener_sendSignal(g_pika_device_event_listener, GPIO_PA8_EVENT_ID,
                                EVENT_SIGAL_IO_RISING_EDGE);
    pks_eventLisener_sendSignal(g_pika_device_event_listener, GPIO_PA8_EVENT_ID,
                                EVENT_SIGAL_IO_FALLING_EDGE);
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[1], "get rising edge!\r\n");
    EXPECT_STREQ(log_buff[0], "get falling edge!\r\n");
    /* deinit */
    obj_deinit(pikaMain);

    EXPECT_EQ(pikaMemNow(), 0);
}
#endif
