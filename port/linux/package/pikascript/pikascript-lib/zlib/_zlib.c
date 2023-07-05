#include "_zlib.h"
#include "fastlz.h"

Arg* _zlib_compress(PikaObj* self, Arg* data, int level) {
    if (arg_getType(data) != ARG_TYPE_BYTES) {
        obj_setErrorCode(self, PIKA_RES_ERR_INVALID_PARAM);
        obj_setSysOut(self, "TypeError: a bytes is required");
        return NULL;
    }
    Arg* aRet = NULL;
    uint8_t* input = arg_getBytes(data);
    size_t input_len = arg_getBytesSize(data);
    size_t max_output_len =
        input_len * 1.1 + 64;  // Fastlz's maximum output size guideline
    uint8_t* ret = pikaMalloc(max_output_len);
    size_t ret_len = fastlz_compress_level(level, input, input_len, ret);
    if (ret_len == 0) {
        obj_setErrorCode(self, PIKA_RES_ERR_INVALID_PARAM);
        obj_setSysOut(self, "Compression failed");
        aRet = NULL;
        goto __exit;
    }
    aRet = arg_newBytes(ret, ret_len);
    goto __exit;
__exit:
    pikaFree(ret, max_output_len);
    return aRet;
}

Arg* _zlib_decompress(PikaObj* self, Arg* data) {
    if (arg_getType(data) != ARG_TYPE_BYTES) {
        obj_setErrorCode(self, PIKA_RES_ERR_INVALID_PARAM);
        obj_setSysOut(self, "TypeError: a bytes is required");
        return NULL;
    }
    uint8_t* input = arg_getBytes(data);
    size_t input_len = arg_getBytesSize(data);
    size_t max_output_len =
        input_len * 4;  // Assume the decompressed data is no more than 4 times
                        // the input data
    size_t ret_len = 0;
    uint8_t* ret = NULL;
    Arg* aRet = NULL;
    int multiplier = 1;
    do {
        multiplier *= 2;
        max_output_len = input_len * multiplier;
        ret = pikaMalloc(max_output_len);
        ret_len = fastlz_decompress(input, input_len, ret, max_output_len);
        if (ret_len == 0) {
            pikaFree(ret, max_output_len);
        }
    } while (ret_len == 0 && multiplier <= 8);
    if (ret_len == 0) {
        obj_setErrorCode(self, PIKA_RES_ERR_INVALID_PARAM);
        obj_setSysOut(self, "Decompression failed");
        return NULL;
    }
    aRet = arg_newBytes(ret, ret_len);
    pikaFree(ret, max_output_len);
    return aRet;
}
