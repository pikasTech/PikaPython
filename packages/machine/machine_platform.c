/* SPEC: PJ2026-050112 STM32 machine v0.3; weak device defaults. */
#include "machine_platform.h"

#include <stddef.h>

#include "pika_capability_config.h"
#include "pika_platform.h"

#if PIKA_CAPABILITY_MACHINE_PIN_ENABLE

PIKA_WEAK PikaStatus pika_machine_platform_pin_open(
    const PikaMachinePinConfig* config,
    void** handle) {
    if (handle == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    *handle = NULL;
    if (config == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_UNSUPPORTED_CAPABILITY;
}

PIKA_WEAK PikaStatus pika_machine_platform_pin_configure(
    void* handle,
    const PikaMachinePinConfig* config) {
    if (handle == NULL || config == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_UNSUPPORTED_CAPABILITY;
}

PIKA_WEAK PikaStatus pika_machine_platform_pin_read(
    void* handle,
    int* value) {
    if (handle == NULL || value == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_UNSUPPORTED_CAPABILITY;
}

PIKA_WEAK PikaStatus pika_machine_platform_pin_write(
    void* handle,
    int value) {
    (void)value;
    if (handle == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_UNSUPPORTED_CAPABILITY;
}

PIKA_WEAK PikaStatus pika_machine_platform_pin_close(void* handle) {
    if (handle == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_OK;
}

#endif

#if PIKA_CAPABILITY_MACHINE_ADC_ENABLE

PIKA_WEAK PikaStatus pika_machine_platform_adc_open(
    const PikaMachineAdcConfig* config,
    void** handle) {
    if (handle == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    *handle = NULL;
    if (config == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_UNSUPPORTED_CAPABILITY;
}

PIKA_WEAK PikaStatus pika_machine_platform_adc_read(
    void* handle,
    uint32_t* raw_value,
    uint8_t* resolution_bits) {
    if (handle == NULL || raw_value == NULL ||
        resolution_bits == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_UNSUPPORTED_CAPABILITY;
}

PIKA_WEAK PikaStatus pika_machine_platform_adc_read_uv(
    void* handle,
    int64_t* microvolts) {
    if (handle == NULL || microvolts == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_UNSUPPORTED_CAPABILITY;
}

PIKA_WEAK PikaStatus pika_machine_platform_adc_close(void* handle) {
    if (handle == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_OK;
}

#endif

#if PIKA_CAPABILITY_MACHINE_PWM_ENABLE

PIKA_WEAK PikaStatus pika_machine_platform_pwm_open(
    const PikaMachinePwmConfig* config,
    void** handle) {
    if (handle == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    *handle = NULL;
    if (config == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_UNSUPPORTED_CAPABILITY;
}

PIKA_WEAK PikaStatus pika_machine_platform_pwm_configure(
    void* handle,
    const PikaMachinePwmSettings* settings) {
    if (handle == NULL || settings == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_UNSUPPORTED_CAPABILITY;
}

PIKA_WEAK PikaStatus pika_machine_platform_pwm_state(
    void* handle,
    PikaMachinePwmState* state) {
    if (handle == NULL || state == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_UNSUPPORTED_CAPABILITY;
}

PIKA_WEAK PikaStatus pika_machine_platform_pwm_disable(void* handle) {
    if (handle == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_UNSUPPORTED_CAPABILITY;
}

PIKA_WEAK PikaStatus pika_machine_platform_pwm_close(void* handle) {
    if (handle == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_OK;
}

#endif

PIKA_WEAK uintptr_t pika_machine_platform_critical_enter(void) {
    return (uintptr_t)0u;
}

PIKA_WEAK void pika_machine_platform_critical_leave(uintptr_t state) {
    (void)state;
}
