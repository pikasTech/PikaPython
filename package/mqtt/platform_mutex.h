/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-15 18:31:33
 * @LastEditTime: 2020-10-17 14:17:31
 * @Description: the code belongs to jiejie, please keep the author information
 * and source code according to the license.
 */
#ifndef _PLATFORM_MUTEX_H_
#define _PLATFORM_MUTEX_H_
#include "PikaObj.h"
#ifdef __linux
    #include <pthread.h>
    typedef struct platform_mutex {
        pthread_mutex_t mutex;
    } platform_mutex_t;
#elif PIKA_FREERTOS_ENABLE
    #include "freertos/FreeRTOS.h"
    #include "freertos/semphr.h"
    typedef struct platform_mutex {
        SemaphoreHandle_t mutex;
    } platform_mutex_t;
#else
    /*
        You need to create the __platform_thread.h for your platform.
        For example:
        You can #include <rtthread.h> in the __platform_thread.h
    */
    #include "__platform_thread.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


int platform_mutex_init(platform_mutex_t* m);
int platform_mutex_lock(platform_mutex_t* m);
int platform_mutex_trylock(platform_mutex_t* m);
int platform_mutex_unlock(platform_mutex_t* m);
int platform_mutex_destroy(platform_mutex_t* m);

#ifdef __cplusplus
}
#endif

#endif
