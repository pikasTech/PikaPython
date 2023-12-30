#ifndef _H_NETOWRK_COMMON_H_
#define _H_NETOWRK_COMMON_H_
#include "_network_NET.h"

#define _net_check_res(res)                                                  \
    if (res != 0) {                                                          \
        pika_platform_printf("_net_check_res failed: %d, at %s:%d:%s()\r\n", \
                             res, __FILE__, __LINE__, __FUNCTION__);         \
    }

#define _NET_OBJ_2DEV(_self) obj_getPtr((_self), "hal_dev");

#endif
