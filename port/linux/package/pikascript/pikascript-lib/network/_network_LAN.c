#include "_network_LAN.h"
#include "../pikascript-lib/PikaStdDevice/pika_hal.h"
#include "PikaStdData_List.h"
#include "PikaStdData_Tuple.h"
#include "_network_common.h"

void _network_LAN___init__(PikaObj* self, int interface_id) {
    char LAN_NAME[] = "LAN0";
    LAN_NAME[3] = '0' + interface_id;
    pika_debug("LAN_NAME: %s\n", LAN_NAME);
    pika_dev* hal_lan = pika_hal_open(PIKA_HAL_LAN, LAN_NAME);
    if (hal_lan == NULL) {
        return;
    }
    pika_hal_LAN_config cfg = {0};
    _net_check_res(pika_hal_ioctl(hal_lan, PIKA_HAL_IOCTL_CONFIG, &cfg));
    obj_setPtr(self, "hal_dev", hal_lan);
}

int _network_LAN_isconnected(PikaObj* self) {
    if (_network_NET_status(self) == PIKA_HAL_LAN_STATUS_GOT_IP) {
        return 1;
    } else {
        return 0;
    }
}
