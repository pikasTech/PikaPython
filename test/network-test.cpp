#include "test_common.h"
TEST_START
#if !PIKA_NANO_ENABLE

extern "C" {
#include "../package/pikascript/pikascript-lib/PikaStdDevice/pika_hal.h"
int pika_hal_platform_WIFI_open(pika_dev* dev, char* name) {
    pika_platform_printf("pika_hal_platform_WIFI_open: %s\r\n", name);
    return 0;
}

int pika_hal_platform_WIFI_close(pika_dev* dev) {
    return 0;
}

int pika_hal_platform_WIFI_ioctl_config(pika_dev* dev,
                                        pika_hal_WIFI_config* cfg) {
    return 0;
}

int pika_hal_platform_WIFI_ioctl_enable(pika_dev* dev) {
    return 0;
}

int pika_hal_platform_WIFI_ioctl_disable(pika_dev* dev) {
    return 0;
}

int pika_hal_platform_WIFI_ioctl_others(pika_dev* dev,
                                        PIKA_HAL_IOCTL_CMD cmd,
                                        void* arg) {
    if (cmd == PIKA_HAL_IOCTL_WIFI_SCAN) {
        pika_platform_printf(
            "pika_hal_platform_WIFI_ioctl_others: "
            "PIKA_HAL_IOCTL_WIFI_SCAN\r\n");
        int count = 3;
        size_t size = sizeof(pika_hal_WIFI_scan_result) + count * sizeof(pika_hal_WIFI_record);
        pika_hal_WIFI_scan_result* result = (pika_hal_WIFI_scan_result*)
                                            pika_platform_malloc(size);
        memset(result, 0, size);
        result->count = count;
        strcpy(result->records[0].ssid, "ssid1");
        strcpy(result->records[0].bssid, "bssid1");
        result->records[0].channel = 1;
        result->records[0].rssi = -10;
        result->records[0].authmode = 0;
        strcpy(result->records[1].ssid, "ssid2");
        strcpy(result->records[1].bssid, "bssid2");
        result->records[1].channel = 2;
        result->records[1].rssi = -20;
        result->records[1].authmode = 1;
        strcpy(result->records[2].ssid, "ssid3");
        strcpy(result->records[2].bssid, "bssid3");
        result->records[2].channel = 3;
        result->records[2].rssi = -30;
        result->records[2].authmode = 2;
        *(pika_hal_WIFI_scan_result**)arg = result;
        return 0;
    }
    return -1;
}
}


TEST(network, scan) {
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/network/network_scan.py");
    /* assert */
    EXPECT_STREQ(log_buff[0], "[('ssid1', 'bssid1', 1, -10, 0, 0), ('ssid2', 'bssid2', 2, -20, 1, 0), ('ssid3', 'bssid3', 3, -30, 2, 0)]\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#endif
TEST_END
