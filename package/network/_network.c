#include "_network.h"
#include "../pikascript-lib/PikaStdDevice/pika_hal.h"

/*
STA_IF: int
AP_IF: int
STAT_IDLE: int
STAT_CONNECTING: int
STAT_WRONG_PASSWORD: int
STAT_NO_AP_FOUND: int
STAT_CONNECT_FAIL: int
STAT_GOT_IP: int
*/

void _network___init__(PikaObj* self) {
    obj_setInt(self, "STA_IF", PIKA_HAL_WIFI_MODE_STA);
    obj_setInt(self, "AP_IF", PIKA_HAL_WIFI_MODE_AP);
    obj_setInt(self, "STAT_IDLE", PIKA_HAL_WIFI_STATUS_IDLE);
    obj_setInt(self, "STAT_CONNECTING", PIKA_HAL_WIFI_STATUS_CONNECTING);
    obj_setInt(self, "STAT_WRONG_PASSWORD",
               PIKA_HAL_WIFI_STATUS_WRONG_PASSWORD);
    obj_setInt(self, "STAT_NO_AP_FOUND", PIKA_HAL_WIFI_STATUS_NO_AP_FOUND);
    obj_setInt(self, "STAT_CONNECT_FAIL", PIKA_HAL_WIFI_STATUS_CONNECT_FAIL);
    obj_setInt(self, "STAT_GOT_IP", PIKA_HAL_WIFI_STATUS_GOT_IP);
};
