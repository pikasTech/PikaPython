#include "ctypes_Test.h"
#include "ctypes_c_buffer.h"
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

void ctypes_c_float___init__(PikaObj* self, double value) {
    ctypesUtils_setFloat(self, value);
}

int ctypes_Test_add(PikaObj* self, PikaObj* c_uint1, PikaObj* c_uint2) {
    return ctypesUtils_getInt(c_uint1) + ctypesUtils_getInt(c_uint2);
}

int ctypes_Test_dc_cpuapdu_hex(PikaObj* self,
                               int slen,
                               uint8_t* sendbuf,
                               PikaObj* rlen,
                               PikaObj* rcvbuf) {
    /* print input */
    __platform_printf("input: slen = %d\n", slen);
    __platform_printf("sendbuf :\n");
    for (int i = 0; i < slen; i++) {
        __platform_printf("0x%02x ", sendbuf[i]);
    }
    __platform_printf("\n");

    /* set output */
    ctypesUtils_setInt(rlen, 5);
    uint8_t rcv[] = {0x01, 0x02, 0x03, 0x00, 0x05, 0x08};
    ctypesUtils_setBytes(rcvbuf, rcv, sizeof(rcv));
    return 0;
}

void ctypes_Test_print_rcv(PikaObj* self, PikaObj* rcvbuf) {
    uint8_t* rcv = ctypesUtils_getBytes(rcvbuf);
    size_t rcv_size = ctypesUtils_getBytesSize(rcvbuf);
    __platform_printf("{");
    for (size_t i = 0; i < rcv_size - 1; i++) {
        __platform_printf("0x%02x, ", rcv[i]);
    }
    __platform_printf("0x%02x", rcv[rcv_size - 1]);
    __platform_printf("}\r\n");
}

void ctypes_create_string_buffer___init__(PikaObj* self, int size) {
    uint8_t* buffer;
    obj_setBytes(self, "raw", NULL, size);
    buffer = obj_getBytes(self, "raw");
    __platform_printf("0x%x", &buffer);
}

int ctypes_create_string_buffer___getitem__(PikaObj* self, int __key) {
    uint8_t* buffer;
    int i;

    i = __key;
    buffer = obj_getBytes(self, "raw");
    return buffer[i];
}

int ctypes_c_buffer___getitem__(PikaObj* self, int __key) {
    int i;
    uint8_t* buffer;

    i = __key;
    buffer = obj_getBytes(self, "raw");
    return buffer[i];
}

void ctypes_c_buffer___init__(PikaObj *self, Arg* value, int size){
    uint8_t* buffer;
    uint8_t* value_buffer;
    size_t value_size;
    ArgType arg_type;

    arg_type = arg_getType(value);
    if (arg_type == ARG_TYPE_BYTES) {
        obj_setBytes(self, "raw", NULL, size);
        buffer = obj_getBytes(self, "raw");
        value_size = arg_getBytesSize(value);
        value_buffer = arg_getBytes(value);
        __platform_memcpy(buffer, value_buffer, value_size);
    } else if (ARG_TYPE_STRING == arg_type) {
        obj_setBytes(self, "raw", NULL, size);
        buffer = obj_getBytes(self, "raw");
        value_buffer = (uint8_t*)arg_getStr(value);
        __platform_memcpy(buffer, value_buffer,
                          strGetSize((char*)value_buffer) + 1);
    } else {
        __platform_printf("value type is not support!");
        while (1)
            ;
    }
}
