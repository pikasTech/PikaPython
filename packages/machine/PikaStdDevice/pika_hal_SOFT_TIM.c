#include "pika_hal.h"
#include <stdint.h>
#include "PikaObj.h"

typedef struct platform_data_SOFT_TIM {
    pika_platform_timer_t thread_timer;
    pika_platform_thread_t* thread;
    pika_bool need_exit;
} platform_data_SOFT_TIM;

void _SOFT_TIM_thread(void* arg) {
    pika_dev* dev = (pika_dev*)arg;
    platform_data_SOFT_TIM* platform_tim = dev->platform_data;
    pika_platform_timer_t* thread_timer = &platform_tim->thread_timer;
    pika_hal_SOFT_TIM_config* cfg = dev->ioctl_config;
    while (1) {
        uint32_t timeout_ms = cfg->period / PIKA_HAL_TIM_PERIOD_1MS;
        pika_platform_thread_timer_cutdown(thread_timer, timeout_ms);
        while (!pika_platform_thread_timer_is_expired(thread_timer)) {
            pika_platform_thread_yield();
            if (platform_tim->need_exit) {
                platform_tim->need_exit = pika_false;
                pika_debug("SOFT_TIM: exit signal got, exit thread");
                return;
            }
        }
        if (cfg->event_callback_ena && cfg->event_callback != NULL) {
            cfg->event_callback(dev, PIKA_HAL_TIM_EVENT_SIGNAL_TIMEOUT);
        }
        if (cfg->mode == PIKA_HAL_TIM_MODE_ONESHOT) {
            pika_debug("SOFT_TIM: oneshot mode, exit thread");
        }
    }
}

int pika_hal_platform_SOFT_TIM_open(pika_dev* dev, char* name) {
    platform_data_SOFT_TIM* platform_tim =
        pikaMalloc(sizeof(platform_data_SOFT_TIM));
    memset(platform_tim, 0, sizeof(platform_data_SOFT_TIM));
    dev->platform_data = platform_tim;
    /* create a thread to simulate the TIM */
    pika_platform_thread_timer_init(&platform_tim->thread_timer);
    return 0;
}

int pika_hal_platform_SOFT_TIM_close(pika_dev* dev) {
    pika_hal_platform_SOFT_TIM_ioctl_disable(dev);
    platform_data_SOFT_TIM* platform_tim = dev->platform_data;
    pikaFree(platform_tim, sizeof(platform_data_SOFT_TIM));
    return 0;
}

int pika_hal_platform_SOFT_TIM_read(pika_dev* dev, void* buf, size_t count) {
    return -1;
}

int pika_hal_platform_SOFT_TIM_write(pika_dev* dev, void* buf, size_t count) {
    return -1;
}

int pika_hal_platform_SOFT_TIM_ioctl_enable(pika_dev* dev) {
    platform_data_SOFT_TIM* platform_tim = dev->platform_data;
    if (NULL == platform_tim->thread) {
        platform_tim->thread = pika_platform_thread_init(
            "pika_hal_SOFT_TIM", _SOFT_TIM_thread, dev,
            PIKA_EVENT_THREAD_STACK_SIZE, PIKA_THREAD_PRIO, PIKA_THREAD_TICK);
        pika_debug("pika_hal_platform_SOFT_TIM_ioctl_enable: thread created");
    }
    return 0;
}

int pika_hal_platform_SOFT_TIM_ioctl_disable(pika_dev* dev) {
    platform_data_SOFT_TIM* platform_tim = dev->platform_data;
    if (NULL != platform_tim->thread) {
        platform_tim->need_exit = pika_true;
        while (platform_tim->need_exit) {
            pika_platform_thread_yield();
        }
        pika_platform_thread_destroy(platform_tim->thread);
        platform_tim->thread = NULL;
        pika_debug("pika_hal_platform_SOFT_TIM_ioctl_disable: thread deleted");
    }
    return 0;
}

int pika_hal_platform_SOFT_TIM_ioctl_config(pika_dev* dev,
                                            pika_hal_SOFT_TIM_config* cfg) {
    return 0;
}
