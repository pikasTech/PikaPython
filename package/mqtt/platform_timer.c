/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-10 22:16:41
 * @LastEditTime: 2020-06-05 17:18:48
 * @Description: the code belongs to jiejie, please keep the author information
 * and source code according to the license.
 */

#include "platform_timer.h"

#if PIKA_FREERTOS_ENABLE
static uint32_t platform_uptime_ms(void)
{
#if (configTICK_RATE_HZ == 1000)
    return (uint32_t)xTaskGetTickCount();
#else
    TickType_t tick = 0u;

    tick = xTaskGetTickCount() * 1000;
    return (uint32_t)((tick + configTICK_RATE_HZ - 1) / configTICK_RATE_HZ);
#endif
}
#endif

PIKA_WEAK void platform_timer_init(platform_timer_t* timer) {
#ifdef __linux
    timer->time = (struct timeval){0, 0};
#elif PIKA_FREERTOS_ENABLE
    timer->time = 0;
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK void platform_timer_cutdown(platform_timer_t* timer,
                                      unsigned int timeout) {
#ifdef __linux
    struct timeval now;
    gettimeofday(&now, NULL);
    struct timeval interval = {timeout / 1000, (timeout % 1000) * 1000};
    timeradd(&now, &interval, &timer->time);
#elif PIKA_FREERTOS_ENABLE
    timer->time = platform_uptime_ms();
    timer->time += timeout;
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK char platform_timer_is_expired(platform_timer_t* timer) {
#ifdef __linux
    struct timeval now, res;
    gettimeofday(&now, NULL);
    timersub(&timer->time, &now, &res);
    return ((res.tv_sec < 0) || (res.tv_sec == 0 && res.tv_usec <= 0));
#elif PIKA_FREERTOS_ENABLE
    return platform_uptime_ms() > timer->time ? 1 : 0;
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int platform_timer_remain(platform_timer_t* timer) {
#ifdef __linux
    struct timeval now, res;
    gettimeofday(&now, NULL);
    timersub(&timer->time, &now, &res);
    return (res.tv_sec < 0) ? 0 : res.tv_sec * 1000 + res.tv_usec / 1000;
#elif PIKA_FREERTOS_ENABLE
    uint32_t now;
    now = platform_uptime_ms();
    if (timer->time <= now) {
        return 0;
    }
    return timer->time - now;
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK unsigned long platform_timer_now(void) {
#ifdef __linux
    return (unsigned long)time(NULL);
#elif PIKA_FREERTOS_ENABLE
    return (unsigned long)platform_uptime_ms();
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK void platform_timer_usleep(unsigned long usec) {
#ifdef __linux
    usleep(usec);
#elif PIKA_FREERTOS_ENABLE
    TickType_t tick = 1;
    if (usec != 0) {
        tick = usec / portTICK_PERIOD_MS;

        if (tick == 0)
            tick = 1;
    }
    vTaskDelay(tick);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}
