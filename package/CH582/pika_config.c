#include "PikaPlatform.h"
#include "printf.h"


void __platform_printf(char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf_(fmt, args);
    va_end(args);
}

int __platform_vsnprintf(char *buff, size_t size, const char *fmt, va_list args)
{
    return vsnprintf_(buff, size, fmt, args);
}

int __platform_sprintf(char *buff, char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int res = vsnprintf_(buff, PIKA_SPRINTF_BUFF_SIZE, fmt, args);
    va_end(args);
    return res;
}

