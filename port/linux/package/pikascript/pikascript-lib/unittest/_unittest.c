#include "_unittest.h"

void _unittest__case_run(PikaObj* self,
                         PikaObj* test_case,
                         PikaObj* test_result,
                         char* suite_name) {
    __platform_printf("testing %s\n", suite_name);
}
