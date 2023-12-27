#include "_network_WLAN.h"
#include "_network_NET.h"
#include "../pikascript-lib/PikaStdDevice/pika_hal.h"
#include "PikaStdData_List.h"
#include "PikaStdData_Tuple.h"
#include "_network_common.h"

void _network_WLAN___init__(PikaObj* self, int interface_id) {
    pika_dev* hal_wifi = pika_hal_open(PIKA_HAL_WIFI, "WLAN0");
    if (hal_wifi == NULL) {
        return;
    }
    pika_hal_WIFI_config cfg = {0};
    cfg.mode = interface_id;
    _net_check_res(pika_hal_ioctl(hal_wifi, PIKA_HAL_IOCTL_CONFIG, &cfg));
    obj_setPtr(self, "hal_dev", hal_wifi);
}

void _network_WLAN_connect(PikaObj* self, char* ssid, char* key) {
    pika_dev* hal_wifi = _NET_OBJ_2DEV(self);
    if (hal_wifi == NULL) {
        return;
    }
    pika_hal_WIFI_connect_config conncfg = {0};
    strcpy(conncfg.ssid, ssid);
    strcpy(conncfg.password, key);
    _net_check_res(pika_hal_ioctl(hal_wifi, PIKA_HAL_IOCTL_WIFI_CONNECT, &conncfg));
    return;
}

void _network_WLAN_connectWithBssid(PikaObj* self,
                                    char* ssid,
                                    char* key,
                                    char* bssid) {
    pika_dev* hal_wifi = _NET_OBJ_2DEV(self);
    if (hal_wifi == NULL) {
        return;
    }
    pika_hal_WIFI_connect_config conncfg = {0};
    strcpy(conncfg.ssid, ssid);
    strcpy(conncfg.password, key);
    strcpy(conncfg.bssid, bssid);
    _net_check_res(pika_hal_ioctl(hal_wifi, PIKA_HAL_IOCTL_WIFI_CONNECT, &conncfg));
}

void _network_WLAN_disconnect(PikaObj* self) {
    pika_dev* hal_wifi = _NET_OBJ_2DEV(self);
    if (hal_wifi == NULL) {
        return;
    }
    _net_check_res(pika_hal_ioctl(hal_wifi, PIKA_HAL_IOCTL_WIFI_DISCONNECT));
}

int _network_WLAN_isconnected(PikaObj* self) {
    if (_network_NET_status(self) == PIKA_HAL_WIFI_STATUS_GOT_IP) {
        return 1;
    } else {
        return 0;
    }
}

PikaObj* _network_WLAN_scan(PikaObj* self) {
    pika_dev* hal_wifi = _NET_OBJ_2DEV(self);
    if (hal_wifi == NULL) {
        return NULL;
    }
    pika_hal_WIFI_scan_result* result = NULL;
    _net_check_res(pika_hal_ioctl(hal_wifi, PIKA_HAL_IOCTL_WIFI_SCAN, &result));
    if (NULL == result) {
        return NULL;
    }
    PikaObj* scan_list = newNormalObj(New_PikaStdData_List);
    PikaStdData_List___init__(scan_list);
    for (int i = 0; i < result->count; i++) {
        // (ssid, bssid, channel, RSSI, authmode, hidden)
        PikaObj* record = newNormalObj(New_PikaStdData_Tuple);
        PikaStdData_Tuple___init__(record);
        Arg* arg = arg_newStr(result->records[i].ssid);
        PikaStdData_List_append(record, arg);
        arg_deinit(arg);
        arg = arg_newBytes(result->records[i].bssid,
                           result->records[i].bssid_len);
        PikaStdData_List_append(record, arg);
        arg_deinit(arg);
        arg = arg_newInt(result->records[i].channel);
        PikaStdData_List_append(record, arg);
        arg_deinit(arg);
        arg = arg_newInt(result->records[i].rssi);
        PikaStdData_List_append(record, arg);
        arg_deinit(arg);
        arg = arg_newInt(result->records[i].authmode);
        PikaStdData_List_append(record, arg);
        arg_deinit(arg);
        arg = arg_newInt(0);
        PikaStdData_List_append(record, arg);
        arg_deinit(arg);
        arg = arg_newObj(record);
        PikaStdData_List_append(scan_list, arg);
        arg_deinit(arg);
    }
    pika_platform_free(result);
    return scan_list;
}


void _network_WLAN_config(PikaObj* self, PikaDict* kwargs) {
    pika_dev* hal_net = _NET_OBJ_2DEV(self);
    if (hal_net == NULL) {
        return;
    }
    pika_hal_WIFI_config cfg = {0};
    if (pikaDict_isArgExist(kwargs, "ssid")) {
        strcpy(cfg.ap_ssid, pikaDict_getStr(kwargs, "ssid"));
    }
    if (pikaDict_isArgExist(kwargs, "password")) {
        strcpy(cfg.ap_password, pikaDict_getStr(kwargs, "password"));
    }
    if (pikaDict_isArgExist(kwargs, "channel")) {
        cfg.channel = pikaDict_getInt(kwargs, "channel") + 1;
    }
    _net_check_res(pika_hal_ioctl(hal_net, PIKA_HAL_IOCTL_CONFIG, &cfg));
}

Arg* _network_WLAN_checkConfig(PikaObj* self, char* param) {
    pika_dev* hal_net = _NET_OBJ_2DEV(self);
    if (hal_net == NULL) {
        return NULL;
    }
    pika_hal_WIFI_config* cfg = hal_net->ioctl_config;
    if (strcmp(param, "ssid") == 0) {
        return arg_newStr(cfg->ap_ssid);
    } else if (strcmp(param, "password") == 0) {
        return arg_newStr(cfg->ap_password);
    } else if (strcmp(param, "channel") == 0) {
        return arg_newInt(cfg->channel - 1);
    } else {
        return NULL;
    }
}
