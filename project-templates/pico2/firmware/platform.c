#include <stddef.h>
#include <stdint.h>

#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/sync.h"
#include "pico/stdlib.h"

#include "pika_capability_config.h"
#include "pika_platform.h"

#if PIKA_CAPABILITY_MACHINE_PIN_ENABLE || \
    PIKA_CAPABILITY_MACHINE_ADC_ENABLE || \
    PIKA_CAPABILITY_MACHINE_PWM_ENABLE
#include "machine_platform.h"
#endif

PikaStatus pika_platform_write(const char* data, size_t size) {
    size_t index;
    if (data == NULL && size != 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0u; index < size; ++index) {
        if (stdio_putchar_raw((unsigned char)data[index]) < 0) {
            return PIKA_STATUS_IO_ERROR;
        }
    }
    stdio_flush();
    return PIKA_STATUS_OK;
}

PikaStatus pika_platform_repl_read(
    uint8_t* data,
    size_t capacity,
    size_t* received) {
    int value;
    if (data == NULL || received == NULL || capacity == 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    value = stdio_getchar();
    if (value < 0) {
        *received = 0u;
        return PIKA_STATUS_IO_ERROR;
    }
    data[0] = (uint8_t)value;
    *received = 1u;
    return PIKA_STATUS_OK;
}

PikaStatus pika_platform_time_ticks_ms(uint32_t* ticks) {
    if (ticks == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    *ticks = to_ms_since_boot(get_absolute_time());
    return PIKA_STATUS_OK;
}

PikaStatus pika_platform_time_sleep_ms(uint32_t duration) {
    sleep_ms(duration);
    return PIKA_STATUS_OK;
}

#if PIKA_CAPABILITY_MACHINE_PIN_ENABLE

#ifndef PIKA_PICO2_PIN_OBJECT_LIMIT
#define PIKA_PICO2_PIN_OBJECT_LIMIT 16u
#endif

typedef struct {
    int64_t identifier;
    PikaMachinePinMode mode;
    uint8_t active;
} PikaPico2Pin;

static PikaPico2Pin pika_pico2_pins[PIKA_PICO2_PIN_OBJECT_LIMIT];

static PikaStatus pika_pico2_pin_validate(
    const PikaMachinePinConfig* config) {
    if (config == NULL || config->identifier < 0 ||
        config->identifier >= 30) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    if (config->mode != PIKA_MACHINE_PIN_MODE_IN &&
        config->mode != PIKA_MACHINE_PIN_MODE_OUT &&
        config->mode != PIKA_MACHINE_PIN_MODE_OPEN_DRAIN) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    if (config->pull != PIKA_MACHINE_PIN_PULL_NONE &&
        config->pull != PIKA_MACHINE_PIN_PULL_UP &&
        config->pull != PIKA_MACHINE_PIN_PULL_DOWN) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    if (config->mode == PIKA_MACHINE_PIN_MODE_OUT &&
        config->pull != PIKA_MACHINE_PIN_PULL_NONE) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    if (config->mode == PIKA_MACHINE_PIN_MODE_OPEN_DRAIN &&
        config->pull == PIKA_MACHINE_PIN_PULL_DOWN) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    if (config->mode == PIKA_MACHINE_PIN_MODE_IN &&
        config->initial_value_valid != 0u) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    return PIKA_STATUS_OK;
}

static void pika_pico2_pin_apply(
    const PikaMachinePinConfig* config) {
    uint gpio = (uint)config->identifier;
    gpio_init(gpio);
    if (config->mode == PIKA_MACHINE_PIN_MODE_OPEN_DRAIN) {
        gpio_set_pulls(gpio,
                       config->pull == PIKA_MACHINE_PIN_PULL_UP,
                       false);
        if (config->initial_value_valid != 0u &&
            config->initial_value == 0u) {
            gpio_put(gpio, 0u);
            gpio_set_dir(gpio, GPIO_OUT);
        } else {
            gpio_set_dir(gpio, GPIO_IN);
        }
        return;
    }
    gpio_set_pulls(gpio,
                   config->pull == PIKA_MACHINE_PIN_PULL_UP,
                   config->pull == PIKA_MACHINE_PIN_PULL_DOWN);
    if (config->mode == PIKA_MACHINE_PIN_MODE_OUT &&
        config->initial_value_valid != 0u) {
        gpio_put(gpio, config->initial_value != 0u);
    }
    gpio_set_dir(gpio, config->mode == PIKA_MACHINE_PIN_MODE_OUT);
}

static PikaPico2Pin* pika_pico2_pin_allocate(void) {
    uint32_t index;
    for (index = 0u; index < PIKA_PICO2_PIN_OBJECT_LIMIT; ++index) {
        if (pika_pico2_pins[index].active == 0u) {
            pika_pico2_pins[index].active = 1u;
            return &pika_pico2_pins[index];
        }
    }
    return NULL;
}

PikaStatus pika_machine_platform_pin_open(
    const PikaMachinePinConfig* config,
    void** handle) {
    PikaPico2Pin* pin;
    PikaStatus status;
    if (handle == NULL || config == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    *handle = NULL;
    status = pika_pico2_pin_validate(config);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    pin = pika_pico2_pin_allocate();
    if (pin == NULL) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    pin->identifier = config->identifier;
    pin->mode = config->mode;
    pika_pico2_pin_apply(config);
    *handle = pin;
    return PIKA_STATUS_OK;
}

PikaStatus pika_machine_platform_pin_configure(
    void* handle,
    const PikaMachinePinConfig* config) {
    PikaPico2Pin* pin = (PikaPico2Pin*)handle;
    PikaStatus status;
    if (pin == NULL || pin->active == 0u || config == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    status = pika_pico2_pin_validate(config);
    if (status != PIKA_STATUS_OK || pin->identifier != config->identifier) {
        return status == PIKA_STATUS_OK ? PIKA_STATUS_INVALID_ARGUMENT : status;
    }
    pin->mode = config->mode;
    pika_pico2_pin_apply(config);
    return PIKA_STATUS_OK;
}

PikaStatus pika_machine_platform_pin_read(void* handle, int* value) {
    PikaPico2Pin* pin = (PikaPico2Pin*)handle;
    if (pin == NULL || pin->active == 0u || value == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    *value = gpio_get((uint)pin->identifier) != 0u ? 1 : 0;
    return PIKA_STATUS_OK;
}

PikaStatus pika_machine_platform_pin_write(void* handle, int value) {
    PikaPico2Pin* pin = (PikaPico2Pin*)handle;
    uint gpio;
    if (pin == NULL || pin->active == 0u || (value != 0 && value != 1)) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (pin->mode == PIKA_MACHINE_PIN_MODE_IN) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    gpio = (uint)pin->identifier;
    if (pin->mode == PIKA_MACHINE_PIN_MODE_OPEN_DRAIN) {
        if (value == 0) {
            gpio_put(gpio, 0u);
            gpio_set_dir(gpio, GPIO_OUT);
        } else {
            gpio_set_dir(gpio, GPIO_IN);
        }
    } else {
        gpio_put(gpio, value != 0);
    }
    return PIKA_STATUS_OK;
}

PikaStatus pika_machine_platform_pin_close(void* handle) {
    PikaPico2Pin* pin = (PikaPico2Pin*)handle;
    if (pin == NULL || pin->active == 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    gpio_deinit((uint)pin->identifier);
    pin->active = 0u;
    return PIKA_STATUS_OK;
}

#endif

#if PIKA_CAPABILITY_MACHINE_ADC_ENABLE

typedef struct {
    uint8_t channel;
    uint8_t active;
} PikaPico2Adc;

static PikaPico2Adc pika_pico2_adc_objects[4];
static uint8_t pika_pico2_adc_initialized;

static PikaStatus pika_pico2_adc_channel(
    int64_t identifier,
    uint8_t* channel) {
    if (channel == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (identifier >= 0 && identifier <= 3) {
        *channel = (uint8_t)identifier;
    } else if (identifier >= 26 && identifier <= 29) {
        *channel = (uint8_t)(identifier - 26);
    } else if (identifier == 4) {
        *channel = 4u;
    } else {
        return PIKA_STATUS_VALUE_ERROR;
    }
    return PIKA_STATUS_OK;
}

PikaStatus pika_machine_platform_adc_open(
    const PikaMachineAdcConfig* config,
    void** handle) {
    uint32_t index;
    uint8_t channel;
    PikaStatus status;
    if (config == NULL || handle == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    *handle = NULL;
    status = pika_pico2_adc_channel(config->identifier, &channel);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (pika_pico2_adc_initialized == 0u) {
        adc_init();
        pika_pico2_adc_initialized = 1u;
    }
    if (channel == 4u) {
        adc_set_temp_sensor_enabled(true);
    } else {
        adc_gpio_init((uint)(26u + channel));
    }
    for (index = 0u; index < 4u; ++index) {
        if (pika_pico2_adc_objects[index].active == 0u) {
            pika_pico2_adc_objects[index].channel = channel;
            pika_pico2_adc_objects[index].active = 1u;
            *handle = &pika_pico2_adc_objects[index];
            return PIKA_STATUS_OK;
        }
    }
    return PIKA_STATUS_STORAGE_TOO_SMALL;
}

PikaStatus pika_machine_platform_adc_read(
    void* handle,
    uint32_t* raw_value,
    uint8_t* resolution_bits) {
    PikaPico2Adc* adc = (PikaPico2Adc*)handle;
    if (adc == NULL || adc->active == 0u || raw_value == NULL ||
        resolution_bits == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    adc_select_input(adc->channel);
    *raw_value = adc_read();
    *resolution_bits = 12u;
    return PIKA_STATUS_OK;
}

PikaStatus pika_machine_platform_adc_read_uv(
    void* handle,
    int64_t* microvolts) {
    uint32_t raw_value;
    uint8_t resolution_bits;
    PikaStatus status;
    if (microvolts == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    status = pika_machine_platform_adc_read(
        handle, &raw_value, &resolution_bits);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    *microvolts = ((int64_t)raw_value * 3300000ll) / 4095ll;
    return PIKA_STATUS_OK;
}

PikaStatus pika_machine_platform_adc_close(void* handle) {
    PikaPico2Adc* adc = (PikaPico2Adc*)handle;
    if (adc == NULL || adc->active == 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    adc->active = 0u;
    return PIKA_STATUS_OK;
}

#endif

uintptr_t pika_machine_platform_critical_enter(void) {
    return (uintptr_t)save_and_disable_interrupts();
}

void pika_machine_platform_critical_leave(uintptr_t state) {
    restore_interrupts((uint32_t)state);
}

#if PIKA_CAPABILITY_MACHINE_PWM_ENABLE

typedef struct {
    uint slice;
    uint gpio;
    uint32_t frequency_hz;
    uint16_t duty_u16;
    uint8_t enabled;
    uint8_t active;
} PikaPico2Pwm;

static PikaPico2Pwm pika_pico2_pwm_objects[16];

static PikaStatus pika_pico2_pwm_source(
    int64_t identifier,
    uint* gpio) {
    if (gpio == NULL || identifier < 0 || identifier >= 30) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    *gpio = (uint)identifier;
    return PIKA_STATUS_OK;
}

static PikaPico2Pwm* pika_pico2_pwm_allocate(void) {
    uint32_t index;
    for (index = 0u; index < 16u; ++index) {
        if (pika_pico2_pwm_objects[index].active == 0u) {
            pika_pico2_pwm_objects[index].active = 1u;
            return &pika_pico2_pwm_objects[index];
        }
    }
    return NULL;
}

static void pika_pico2_pwm_apply(PikaPico2Pwm* pwm) {
    pwm_config config = pwm_get_default_config();
    uint32_t clock_hz = clock_get_hz(clk_sys);
    uint32_t divider;
    uint32_t top;
    uint64_t denominator;
    if (pwm->frequency_hz == 0u) {
        return;
    }
    denominator = (uint64_t)pwm->frequency_hz * 65536ull;
    divider = (uint32_t)(((uint64_t)clock_hz + denominator / 2ull) /
                         denominator);
    if (divider < 1u) {
        divider = 1u;
    } else if (divider > 255u) {
        divider = 255u;
    }
    top = clock_hz / (divider * pwm->frequency_hz);
    if (top == 0u) {
        top = 1u;
    }
    --top;
    if (top > 65535u) {
        top = 65535u;
    }
    pwm_config_set_clkdiv_int(&config, divider);
    pwm_config_set_wrap(&config, (uint16_t)top);
    pwm_init(pwm->slice, &config, false);
    pwm_set_gpio_level(pwm->gpio,
                       (uint16_t)(((uint32_t)pwm->duty_u16 * (top + 1u)) /
                                  65535u));
    pwm_set_enabled(pwm->slice, pwm->enabled != 0u);
}

PikaStatus pika_machine_platform_pwm_open(
    const PikaMachinePwmConfig* config,
    void** handle) {
    PikaPico2Pwm* pwm;
    uint gpio;
    PikaStatus status;
    if (config == NULL || handle == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    *handle = NULL;
    status = pika_pico2_pwm_source(config->identifier, &gpio);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    pwm = pika_pico2_pwm_allocate();
    if (pwm == NULL) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    pwm->gpio = gpio;
    pwm->slice = pwm_gpio_to_slice_num(gpio);
    pwm->frequency_hz = 1000u;
    pwm->duty_u16 = 0u;
    pwm->enabled = 0u;
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    *handle = pwm;
    return PIKA_STATUS_OK;
}

PikaStatus pika_machine_platform_pwm_configure(
    void* handle,
    const PikaMachinePwmSettings* settings) {
    PikaPico2Pwm* pwm = (PikaPico2Pwm*)handle;
    if (pwm == NULL || pwm->active == 0u || settings == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (settings->frequency_present != 0u) {
        pwm->frequency_hz = settings->frequency_hz;
    }
    if (settings->duty_mode == PIKA_MACHINE_PWM_DUTY_U16) {
        pwm->duty_u16 = settings->duty_u16;
    } else if (settings->duty_mode == PIKA_MACHINE_PWM_DUTY_NS) {
        uint64_t period_ns = 1000000000ull / pwm->frequency_hz;
        pwm->duty_u16 = settings->duty_ns >= period_ns
                            ? 65535u
                            : (uint16_t)((settings->duty_ns * 65535ull) /
                                         period_ns);
    }
    pwm->enabled = settings->enabled;
    pika_pico2_pwm_apply(pwm);
    return PIKA_STATUS_OK;
}

PikaStatus pika_machine_platform_pwm_state(
    void* handle,
    PikaMachinePwmState* state) {
    PikaPico2Pwm* pwm = (PikaPico2Pwm*)handle;
    if (pwm == NULL || pwm->active == 0u || state == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    state->frequency_hz = pwm->frequency_hz;
    state->duty_u16 = pwm->duty_u16;
    state->duty_ns = ((uint64_t)pwm->duty_u16 * 1000000000ull) /
                     (uint64_t)pwm->frequency_hz / 65535ull;
    state->duty_mode = PIKA_MACHINE_PWM_DUTY_U16;
    state->frequency_valid = 1u;
    state->enabled = pwm->enabled;
    return PIKA_STATUS_OK;
}

PikaStatus pika_machine_platform_pwm_disable(void* handle) {
    PikaPico2Pwm* pwm = (PikaPico2Pwm*)handle;
    if (pwm == NULL || pwm->active == 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    pwm->enabled = 0u;
    pwm_set_enabled(pwm->slice, false);
    return PIKA_STATUS_OK;
}

PikaStatus pika_machine_platform_pwm_close(void* handle) {
    PikaPico2Pwm* pwm = (PikaPico2Pwm*)handle;
    if (pwm == NULL || pwm->active == 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    pwm_set_enabled(pwm->slice, false);
    gpio_set_function(pwm->gpio, GPIO_FUNC_SIO);
    gpio_set_dir(pwm->gpio, GPIO_IN);
    pwm->active = 0u;
    return PIKA_STATUS_OK;
}

#endif
