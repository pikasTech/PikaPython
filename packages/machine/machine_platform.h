/* SPEC: PJ2026-050112 STM32 machine v0.3; device platform boundary. */
#ifndef PIKA_MACHINE_PLATFORM_H
#define PIKA_MACHINE_PLATFORM_H

#include <stdint.h>

#include "pika_capability_config.h"
#include "pika_status.h"

#ifdef __cplusplus
extern "C" {
#endif

#if PIKA_CAPABILITY_MACHINE_PIN_ENABLE

typedef enum {
    PIKA_MACHINE_PIN_MODE_IN = 0,
    PIKA_MACHINE_PIN_MODE_OUT = 1,
    PIKA_MACHINE_PIN_MODE_OPEN_DRAIN = 2,
} PikaMachinePinMode;

typedef enum {
    PIKA_MACHINE_PIN_PULL_NONE = -1,
    PIKA_MACHINE_PIN_PULL_UP = 1,
    PIKA_MACHINE_PIN_PULL_DOWN = 2,
} PikaMachinePinPull;

typedef struct {
    int64_t identifier;
    PikaMachinePinMode mode;
    PikaMachinePinPull pull;
    uint8_t initial_value_valid;
    uint8_t initial_value;
} PikaMachinePinConfig;

PikaStatus pika_machine_platform_pin_open(
    const PikaMachinePinConfig* config,
    void** handle);
PikaStatus pika_machine_platform_pin_configure(
    void* handle,
    const PikaMachinePinConfig* config);
PikaStatus pika_machine_platform_pin_read(
    void* handle,
    int* value);
PikaStatus pika_machine_platform_pin_write(
    void* handle,
    int value);
PikaStatus pika_machine_platform_pin_close(void* handle);

#endif

#if PIKA_CAPABILITY_MACHINE_ADC_ENABLE

typedef struct {
    int64_t identifier;
} PikaMachineAdcConfig;

PikaStatus pika_machine_platform_adc_open(
    const PikaMachineAdcConfig* config,
    void** handle);
PikaStatus pika_machine_platform_adc_read(
    void* handle,
    uint32_t* raw_value,
    uint8_t* resolution_bits);
PikaStatus pika_machine_platform_adc_read_uv(
    void* handle,
    int64_t* microvolts);
PikaStatus pika_machine_platform_adc_close(void* handle);

#endif

#if PIKA_CAPABILITY_MACHINE_PWM_ENABLE

typedef enum {
    PIKA_MACHINE_PWM_DUTY_UNCHANGED = 0,
    PIKA_MACHINE_PWM_DUTY_U16 = 1,
    PIKA_MACHINE_PWM_DUTY_NS = 2,
} PikaMachinePwmDutyMode;

typedef struct {
    int64_t identifier;
} PikaMachinePwmConfig;

typedef struct {
    uint32_t frequency_hz;
    uint16_t duty_u16;
    uint64_t duty_ns;
    PikaMachinePwmDutyMode duty_mode;
    uint8_t frequency_present;
    uint8_t enabled;
} PikaMachinePwmSettings;

typedef struct {
    uint32_t frequency_hz;
    uint16_t duty_u16;
    uint64_t duty_ns;
    PikaMachinePwmDutyMode duty_mode;
    uint8_t frequency_valid;
    uint8_t enabled;
} PikaMachinePwmState;

PikaStatus pika_machine_platform_pwm_open(
    const PikaMachinePwmConfig* config,
    void** handle);
PikaStatus pika_machine_platform_pwm_configure(
    void* handle,
    const PikaMachinePwmSettings* settings);
PikaStatus pika_machine_platform_pwm_state(
    void* handle,
    PikaMachinePwmState* state);
PikaStatus pika_machine_platform_pwm_disable(void* handle);
PikaStatus pika_machine_platform_pwm_close(void* handle);

#endif

uintptr_t pika_machine_platform_critical_enter(void);
void pika_machine_platform_critical_leave(uintptr_t state);

#ifdef __cplusplus
}
#endif

#endif
