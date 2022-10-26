#include "pika_config_gtest.h"
#include <stdio.h>
#include <stdlib.h>
#include "dataMemory.h"

char log_buff[LOG_BUFF_MAX][LOG_SIZE] = {0};
uint32_t log_index = 0;

#ifndef __platform_printf
/* save printf content to log_buff */
void __platform_printf(char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    for (int i = LOG_BUFF_MAX - 2; i >= 0; i--) {
        memcpy(log_buff[i + 1], log_buff[i], LOG_SIZE);
    }
    va_start(args, fmt);
    vsnprintf(log_buff[0], LOG_SIZE - 1, fmt, args);
    va_end(args);
}
#endif

/* quick_malloc is always open */
uint8_t __is_quick_malloc(void) {
    // return 1;
    return 0;
}

void __gtest_hook_default(void) {
    return;
}
typedef void (*hook_func)(void);

volatile int g_hook_cnt = 0;
volatile hook_func g_hook_func = __gtest_hook_default;
void __pks_hook_instruct(void) {
    g_hook_cnt++;
    g_hook_func();
}
