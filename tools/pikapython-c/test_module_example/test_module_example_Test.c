#include "pikaScript.h"
// 注入示例 C 源: run_pika.py --module test_module_example 复制使用 (无自动生成)
int test_module_example_Test_add(PikaObj* self, int a, int b){
    return a + b;
}
char* test_module_example_Test_greet(PikaObj* self, char* name){
    char buf[64];
    int n = snprintf(buf, sizeof(buf), "Hello,%s", name);
    (void)n;
    return obj_cacheStr(self, buf);
}
