/*
 * @Author       : jiejie
 * @GitHub       : https://github.com/jiejieTop
 * @Date         : 2021-02-26 12:00:24
 * @LastEditors  : jiejie
 * @LastEditTime : 2022-06-15 19:44:22
 * @FilePath     : /mqttclient/common/random.c
 * Copyright (c) 2022 jiejie, All Rights Reserved. Please keep the author information and source code according to the license.
 */

#include <stdlib.h>
#include "random.h"

extern int platform_timer_now();
static unsigned int last_seed = 1;


int __attribute__((weak)) platform_timer_now()
{
    return 1;
}

static int do_random(unsigned int seed)
{
    srand(seed);
    return rand();
}

int random_number(void)
{
    unsigned int seed = (unsigned int) platform_timer_now();
    last_seed += (seed >> ((seed ^ last_seed) % 3));
    return do_random(last_seed ^ seed);
}

// random number range interval [min, max)
int random_number_range(unsigned int min, unsigned int max)
{
    return (random_number() % (max - min)) + min;
}

int random_string(char *buffer, int len)
{
    unsigned int i, flag, seed, random;

    if (NULL == buffer)
        return 0;
    
    seed = (unsigned int) random_number();
    seed += (unsigned int) ((size_t)buffer ^ seed);
    
    random = (unsigned int)do_random(seed);
    
	for (i = 0; i < len; i++) {
        random = do_random(seed ^ random);
		flag = (unsigned int)random % 3;
		switch (flag) {
            case 0:
                buffer[i] = 'A' + do_random(random ^ (i & flag)) % 26;
                break;
            case 1:
                buffer[i] = 'a' + do_random(random ^ (i & flag)) % 26;
                break;
            case 2:
                buffer[i] = '0' + do_random(random ^ (i & flag)) % 10;
                break;
            default:
                buffer[i] = 'x';
                break;
		}
        random += ((0xb433e5c6 ^ random)  << (i & flag));
	}

    buffer[len] = '\0';
	return len;
}

