#include "_network_NET.h"
#include "../pikascript-lib/PikaStdDevice/pika_hal.h"
#include "PikaStdData_List.h"
#include "PikaStdData_Tuple.h"
#include "_network_common.h"

void _network_NET___init__(PikaObj* self, int interface_id) {
    obj_setErrorCode(self, -__LINE__);
    obj_setSysOut(self, "Error: NET class need inhert");
}

void _network_NET_active(PikaObj* self, int is_active) {
    pika_dev* hal_net = _NET_OBJ_2DEV(self);
    if (hal_net == NULL) {
        return;
    }
    if (is_active) {
        _net_check_res(pika_hal_ioctl(hal_net, PIKA_HAL_IOCTL_ENABLE));
        return;
    }
    _net_check_res(pika_hal_ioctl(hal_net, PIKA_HAL_IOCTL_DISABLE));
    return;
}

int _network_NET_checkActive(PikaObj* self) {
    pika_dev* hal_net = _NET_OBJ_2DEV(self);
    if (hal_net == NULL) {
        return -1;
    }
    int is_active = 0;
    _net_check_res(
        pika_hal_ioctl(hal_net, PIKA_HAL_IOCTL_NET_GET_ACTIVE, &is_active));
    return is_active;
}

int _network_NET_status(PikaObj* self) {
    pika_dev* hal_net = _NET_OBJ_2DEV(self);
    if (hal_net == NULL) {
        return -1;
    }
    int status = 0;
    _net_check_res(
        pika_hal_ioctl(hal_net, PIKA_HAL_IOCTL_NET_GET_STATUS, &status));
    return status;
}

int _network_NET_statusWithParam(PikaObj* self, char* param) {
    return _network_NET_status(self);
}

PikaObj* _network_NET_checkIfconfig(PikaObj* self) {
    pika_dev* hal_net = _NET_OBJ_2DEV(self);
    if (hal_net == NULL) {
        return NULL;
    }
    pika_hal_NET_ifconfig ifconfig = {0};
    _net_check_res(
        pika_hal_ioctl(hal_net, PIKA_HAL_IOCTL_NET_GET_IFCONFIG, &ifconfig));
    PikaObj* ifconfig_list = newNormalObj(New_PikaStdData_List);
    PikaStdData_List___init__(ifconfig_list);
    Arg* arg = arg_newStr(ifconfig.ip);
    PikaStdData_List_append(ifconfig_list, arg);
    arg_deinit(arg);
    arg = arg_newStr(ifconfig.netmask);
    PikaStdData_List_append(ifconfig_list, arg);
    arg_deinit(arg);
    arg = arg_newStr(ifconfig.gateway);
    PikaStdData_List_append(ifconfig_list, arg);
    arg_deinit(arg);
    arg = arg_newStr(ifconfig.dns);
    PikaStdData_List_append(ifconfig_list, arg);
    arg_deinit(arg);
    return ifconfig_list;
}

void _network_NET_ifconfig(PikaObj* self,
                           char* ip,
                           char* mask,
                           char* gateway,
                           char* dns) {
    pika_dev* hal_net = _NET_OBJ_2DEV(self);
    if (hal_net == NULL) {
        return;
    }
    pika_hal_NET_ifconfig ifconfig = {0};
    strcpy(ifconfig.ip, ip);
    strcpy(ifconfig.netmask, mask);
    strcpy(ifconfig.gateway, gateway);
    strcpy(ifconfig.dns, dns);
    pika_debug("ip:%s, mask:%s, gateway:%s, dns:%s\n", ip, mask, gateway, dns);
    _net_check_res(
        pika_hal_ioctl(hal_net, PIKA_HAL_IOCTL_NET_SET_IFCONFIG, &ifconfig));
    return;
}

void _network_NET_close(PikaObj* self) {
    pika_dev* hal_net = _NET_OBJ_2DEV(self);
    if (hal_net == NULL) {
        return;
    }
    _net_check_res(pika_hal_close(hal_net));
}
