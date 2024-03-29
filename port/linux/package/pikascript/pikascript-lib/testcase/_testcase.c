#include "_testcase.h"
#include "_testcase_test.h"

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

void _testcase_test___init__(PikaObj* self) {
    PikaDict* dict = New_pikaDict();
    pikaDict_setStr(dict, "PATH", "/usr/local/bin");
    Arg* arg_dict = arg_newObj(dict);
    obj_setArg(self, "environ", arg_dict);
    arg_deinit(arg_dict);
    return;
}

void _testcase_bytes_test(PikaObj* self, uint8_t* bytes, int len) {
    for (int i = 0; i < len; i++) {
        pika_platform_printf("%d ", bytes[i]);
    }
    pika_platform_printf("\n");
}
