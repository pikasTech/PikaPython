#include "driver/ledc.h"
#include "../PikaStdDevice/pika_hal.h"
#include <ctype.h>

#define PWM_CHANNAL_MAX LEDC_CHANNEL_MAX

typedef struct 
{
    ledc_timer_config_t timer;
    ledc_channel_config_t channel;
}pika_pwm_dev_t;


int pika_hal_platform_PWM_open(pika_dev* dev, char* name) {
    dev->platform_data = pikaMalloc(sizeof(pika_pwm_dev_t));
    memset(dev->platform_data, 0, sizeof(pika_pwm_dev_t));
    pika_pwm_dev_t* platform_pwm = (pika_pwm_dev_t*)dev->platform_data;
    /* support P0/P1 ... */
    if (name[0] == 'P' && isdigit((int)name[1])) {
        platform_pwm->timer.speed_mode = LEDC_LOW_SPEED_MODE;
        platform_pwm->timer.timer_num = LEDC_TIMER_0;
        platform_pwm->timer.duty_resolution = LEDC_TIMER_10_BIT;
        platform_pwm->timer.freq_hz = 1000;
        platform_pwm->timer.clk_cfg = LEDC_AUTO_CLK;
        platform_pwm->channel.speed_mode = LEDC_LOW_SPEED_MODE;
        platform_pwm->channel.gpio_num = fast_atoi(name + 1);
        platform_pwm->channel.channel = platform_pwm->channel.gpio_num % PWM_CHANNAL_MAX;
        platform_pwm->channel.timer_sel = LEDC_TIMER_0;
        platform_pwm->channel.intr_type = LEDC_INTR_DISABLE;
        platform_pwm->channel.duty = 100;
        platform_pwm->channel.hpoint = 0;
    } else {
        __platform_printf("Error: pwm name error.\r\n");
        __platform_printf("Require: Px (x: 0~24)\r\n");
        return -1;
    }
    return 0;
}

int pika_hal_platform_PWM_close(pika_dev* dev) {
    pikaFree(dev->platform_data, sizeof(pika_pwm_dev_t));
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
    pika_pwm_dev_t* platform_pwm = (pika_pwm_dev_t*)dev->platform_data;
    if (!dev->is_enabled) {
        ledc_timer_config(&platform_pwm->timer);
        ledc_channel_config(&platform_pwm->channel);
        return 0;
    }
    return -1;
}

int pika_hal_platform_PWM_ioctl_disable(pika_dev* dev) {
    pika_pwm_dev_t* platform_pwm = (pika_pwm_dev_t*)dev->platform_data;
    if (dev->is_enabled) {
        ledc_stop(platform_pwm->channel.speed_mode, platform_pwm->channel.channel, 0);
        return 0;
    }
    return -1;
}

#define PERIOD_TO_FREQ(period) \
    ((uint32_t)(pika_float)1000 * 1000 * 1000 / (pika_float)period)
#define DUTY_TUO_DUTY_CYCLE(duty, period) \
    (uint32_t)((pika_float)duty * (pika_float)1024 / (pika_float)period)

int pika_hal_platform_PWM_ioctl_config(pika_dev* dev,
                                       pika_hal_PWM_config* cfg) {
    pika_pwm_dev_t* platform_pwm = (pika_pwm_dev_t*)dev->platform_data;
    uint32_t freq = PERIOD_TO_FREQ(cfg->period);
    uint32_t duty_cycle = DUTY_TUO_DUTY_CYCLE(cfg->duty, cfg->period);

    /* not enabled */
    if (!dev->is_enabled) {
        // MHz
        platform_pwm->timer.freq_hz = freq;
        // 0 ~ 10000(0 ~ 100%)
        platform_pwm->channel.duty = duty_cycle;
    }
    else if (platform_pwm->timer.freq_hz != freq ||
        platform_pwm->channel.duty != duty_cycle) {
        platform_pwm->timer.freq_hz = freq;
        platform_pwm->channel.duty = duty_cycle;
        ledc_set_freq(platform_pwm->timer.speed_mode, platform_pwm->timer.timer_num, platform_pwm->timer.freq_hz);
        ledc_set_duty(platform_pwm->channel.speed_mode, platform_pwm->channel.channel, platform_pwm->channel.duty);
        ledc_update_duty(platform_pwm->channel.speed_mode, platform_pwm->channel.channel);
    }
    return 0;
}
