#include "_network_WLAN.h"
#include "../pikascript-lib/PikaStdDevice/pika_hal.h"

void _network_WLAN___init__(PikaObj* self, int interface_id) {}

void _network_WLAN_active(PikaObj* self, int is_active) {}

int _network_WLAN_checkActive(PikaObj* self) {
    return -1;
}

Arg* _network_WLAN_checkConfig(PikaObj* self, char* param) {
    return NULL;
}

void _network_WLAN_config(PikaObj* self, PikaDict* kwargs) {}

void _network_WLAN_connect(PikaObj* self, char* ssid, char* key) {}

void _network_WLAN_connectWIthBssid(PikaObj* self,
                                    char* ssid,
                                    char* key,
                                    char* bssid) {}

void _network_WLAN_disconnect(PikaObj* self) {}

int _network_WLAN_isconnected(PikaObj* self) {
    return -1;
}

int _network_WLAN_status(PikaObj* self) {
    return -1;
}

int _network_WLAN_statusWithParam(PikaObj* self, char* param) {
    return -1;
}

PikaObj* _network_WLAN_checkIfconfig(PikaObj* self) {
    return NULL;
}

void _network_WLAN_ifconfig(PikaObj* self, PikaObj* config) {}
