#include "pika_config_gtest.h"
#include <stdio.h>
#include <stdlib.h>
#include "dataMemory.h"

char log_buff[LOG_BUFF_MAX][LOG_SIZE] = {0};
uint32_t log_index = 0;

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

/* quick_malloc is always open */
uint8_t __is_quick_malloc(void) {
    // return 1;
    return 0;
}
