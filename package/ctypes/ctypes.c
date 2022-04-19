#include "ctypes_Test.h"
#include "ctypes_c_float.h"
#include "ctypes_c_uint.h"
#include "ctypes_c_wchar_p.h"
#include "ctypes_utils.h"

void ctypes_c_uint___init__(PikaObj* self, int value) {
    ctypesUtils_setInt(self, value);
}

void ctypes_c_wchar_p___init__(PikaObj* self, char* value) {
    ctypesUtils_setStr(self, value);
}

void ctypes_c_float___init__(PikaObj* self, float value) {
    ctypesUtils_setFloat(self, value);
}

int ctypes_Test_add(PikaObj* self, PikaObj* c_uint1, PikaObj* c_uint2) {
    return ctypesUtils_getInt(c_uint1) + ctypesUtils_getInt(c_uint2);
}

int ctypes_Test_dc_cpuapdu_hex(PikaObj* self,
                               PikaObj* rcvbuf,
                               PikaObj* rlen,
                               uint8_t* sendbuf,
                               int slen) {
    printf("input: slen = %d, sendbuf = %s\n", slen, sendbuf);
    ctypesUtils_setInt(rlen, 5);
    char rcv[] = {0x01, 0x02, 0x03, 0x00, 0x05, 0x08};
    ctypesUtils_setBytes(rcvbuf, rcv, sizeof(rcv));
    return 0;
}

void ctypes_Test_print_rcv(PikaObj* self, PikaObj* rcvbuf) {
    char* rcv = ctypesUtils_getBytes(rcvbuf);
    size_t rcv_size = ctypesUtils_getBytesSize(rcvbuf);
    __platform_printf("{");
    for (size_t i = 0; i < rcv_size - 1; i++) {
        __platform_printf("%x, ", rcv[i]);
    }
    __platform_printf("%x", rcv[rcv_size - 1]);
    __platform_printf("}\r\n");
}
