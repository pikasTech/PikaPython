#include <stdio.h>
#define LOG_BUFF_MAX 100
#define LOG_SIZE 512
void mem_pool_deinit(void);
void mem_pool_init(void);
int write_to_getchar_buffer(const char* str, size_t size);
