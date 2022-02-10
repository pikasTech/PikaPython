#pragma once

#ifndef NULL
#define NULL ((void *)0)
#endif

//equal to printf
//not support : float,64bit integer,
//support: %d, %x, %s, %c, %p
void myprintf (const char *fmt, ...);
//init usart2 for printf
void myprintf_init();


