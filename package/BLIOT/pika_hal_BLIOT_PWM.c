#include <hosal_pwm.h>
#include "../PikaStdDevice/pika_hal.h"
#define PWM_CHANNAL_MAX 5
int pika_hal_platform_PWM_open(pika_dev* dev, char* name) {
    dev->platform_data = pikaMalloc(sizeof(hosal_pwm_dev_t));
    memset(dev->platform_data, 0, sizeof(hosal_pwm_dev_t));
    hosal_pwm_dev_t* platform_pwm = (hosal_pwm_dev_t*)dev->platform_data;
    /* support P0/P1 ... */
    if (name[0] == 'P') {
        platform_pwm->config.pin = fast_atoi(name + 1);
        platform_pwm->port = platform_pwm->config.pin % PWM_CHANNAL_MAX;
    } else {
        __platform_printf("Error: pwm name error.\r\n");
        __platform_printf("Require: Px (x: 0~24)\r\n");
        return -1;
    }
    return 0;
}

int pika_hal_platform_PWM_close(pika_dev* dev) {
    pikaFree(dev->platform_data, sizeof(hosal_pwm_dev_t));
    return 0;
}

int pika_hal_platform_PWM_read(pika_dev* dev, void* buf, size_t count) {
    /* not support */
    return -1;
}

int pika_hal_platform_PWM_write(pika_dev* dev, void* buf, size_t count) {
    /* not support */
    return -1;
}

int pika_hal_platform_PWM_ioctl_enable(pika_dev* dev) {
    hosal_pwm_dev_t* platform_pwm = (hosal_pwm_dev_t*)dev->platform_data;
#if PIKA_DEBUG_ENABLE
    __platform_printf("is enable %d\r\n", dev->is_enabled);
#endif
    if (!dev->is_enabled) {
        // printf("pwm enable\r\n");
        hosal_pwm_init(platform_pwm);
        hosal_pwm_start(platform_pwm);
        return 0;
    }
    return -1;
}

int pika_hal_platform_PWM_ioctl_disable(pika_dev* dev) {
    hosal_pwm_dev_t* platform_pwm = (hosal_pwm_dev_t*)dev->platform_data;
    if (dev->is_enabled) {
        hosal_pwm_stop(platform_pwm);
        return 0;
    }
    return -1;
}

#define PERIOD_TO_FREQ(period) \
    ((uint32_t)(pika_float)1000 * 1000 * 1000 / (pika_float)period)
#define DUTY_TUO_DUTY_CYCLE(duty, period) \
    (uint32_t)((pika_float)duty * (pika_float)10000 / (pika_float)period)

int pika_hal_platform_PWM_ioctl_config(pika_dev* dev,
                                       pika_hal_PWM_config* cfg) {
    hosal_pwm_dev_t* platform_pwm = (hosal_pwm_dev_t*)dev->platform_data;
    uint32_t freq = PERIOD_TO_FREQ(cfg->period);
    uint32_t duty_cycle = DUTY_TUO_DUTY_CYCLE(cfg->duty, cfg->period);
#if PIKA_DEBUG_ENABLE
    __platform_printf("freq: %d, duty_cycle: %d\r\n", freq, duty_cycle);
#endif
    /* not enabled */
    if (!dev->is_enabled) {
#if PIKA_DEBUG_ENABLE
        __platform_printf("not enabled, just config\r\n");
#endif
        // MHz
        platform_pwm->config.freq = freq;
        // 0 ~ 10000(0 ~ 100%)
        platform_pwm->config.duty_cycle = duty_cycle;
        return 0;
    }
#if PIKA_DEBUG_ENABLE
    __platform_printf("enabled, change config\r\n");
#endif
    /* enabled */
    if (platform_pwm->config.freq != freq ||
        platform_pwm->config.duty_cycle != duty_cycle) {
        hosal_pwm_config_t para;
        para.duty_cycle = duty_cycle;
        para.freq = freq;
        hosal_pwm_para_chg(platform_pwm, para);
    }
    return 0;
}
