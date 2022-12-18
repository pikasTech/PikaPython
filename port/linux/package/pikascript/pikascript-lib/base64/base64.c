#include "base64.h"

#include "mbedtls/base64.h"

Arg* base64_b64decode(PikaObj* self, Arg* s) {
    ArgType t = arg_getType(s);
    if (ARG_TYPE_BYTES != t) {
        obj_setErrorCode(self, -2);  // io error
        obj_setSysOut(self, "base64.b64decode input not bytes");
    }

    uint8_t* input_buff = arg_getBytes(s);
    size_t input_len = arg_getBytesSize(s);
    obj_setBytes(self, "__de_buff", NULL, input_len);
    uint8_t* output_buff = obj_getBytes(self, "__de_buff");
    size_t output_len = 0;
    mbedtls_base64_decode(output_buff, obj_getBytesSize(self, "__de_buff"),
                          &output_len, input_buff, input_len);
    Arg* r = arg_newBytes(output_buff, output_len);
    obj_removeArg(self, "__de_buff");
    return r;
}

Arg* base64_b64encode(PikaObj* self, Arg* s) {
    ArgType t = arg_getType(s);
    if (ARG_TYPE_BYTES != t) {
        obj_setErrorCode(self, -2);  // io error
        obj_setSysOut(self, "base64.b64encode input not bytes");
    }
    uint8_t* input_buff = arg_getBytes(s);
    size_t input_len = arg_getBytesSize(s);
    obj_setBytes(self, "__de_buff", NULL, (size_t)(input_len * 2));
    uint8_t* output_buff = obj_getBytes(self, "__de_buff");
    size_t output_len = 0;
    mbedtls_base64_encode(output_buff, obj_getBytesSize(self, "__de_buff"),
                          &output_len, input_buff, input_len);
    Arg* r = arg_newBytes(output_buff, output_len);
    obj_removeArg(self, "__de_buff");
    return r;
}
