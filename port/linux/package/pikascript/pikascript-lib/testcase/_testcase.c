#include "_testcase.h"

void _testcase_test_tuple_nested(PikaObj* self, PikaObj* tuple_nasted) {
    size_t len = pikaTuple_getSize(tuple_nasted);
    for (size_t i = 0; i < len; i++) {
        Arg* item = pikaTuple_get(tuple_nasted, i);
        if (arg_isTuple(item)) {
            PikaObj* tuple_inner = arg_getObj(item);
            size_t len_inner = pikaTuple_getSize(tuple_inner);
            for (size_t j = 0; j < len_inner; j++) {
                Arg* item_inner = pikaTuple_get(tuple_inner, j);
                if (ARG_TYPE_INT == arg_getType(item_inner)) {
                    int64_t value = arg_getInt(item_inner);
                    pika_platform_printf("[%d, %d] %ld\n", i, j, value);
                }
                if (ARG_TYPE_FLOAT == arg_getType(item_inner)) {
                    double value = arg_getFloat(item_inner);
                    pika_platform_printf("[%d, %d] %f\n", i, j, value);
                }
                if (ARG_TYPE_STRING == arg_getType(item_inner)) {
                    char* value = arg_getStr(item_inner);
                    pika_platform_printf("[%d, %d] %s\n", i, j, value);
                }
            }
        }
    }
}
