/* SPEC: PJ2026-050112 STM32 machine v0.3; PWM facade contract. */
#ifndef PIKA_MACHINE_PWM_H
#define PIKA_MACHINE_PWM_H

#include "pika_binding.h"
#include "pika_capability_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#if PIKA_CAPABILITY_MACHINE_PWM_ENABLE

#ifndef PIKA_MACHINE_PWM_OBJECT_LIMIT
#define PIKA_MACHINE_PWM_OBJECT_LIMIT 8u
#endif

#if PIKA_MACHINE_PWM_OBJECT_LIMIT < 1u
#error "PIKA_MACHINE_PWM_OBJECT_LIMIT must be at least 1"
#endif

PikaStatus pika_binding_machine_PWM_init(
    const PikaBindingCall* call,
    PikaBindingValue* result);
PikaStatus pika_binding_machine_PWM_method_init(
    const PikaBindingCall* call,
    PikaBindingValue* result);
PikaStatus pika_binding_machine_PWM_freq(
    const PikaBindingCall* call,
    PikaBindingValue* result);
PikaStatus pika_binding_machine_PWM_duty_u16(
    const PikaBindingCall* call,
    PikaBindingValue* result);
PikaStatus pika_binding_machine_PWM_duty_ns(
    const PikaBindingCall* call,
    PikaBindingValue* result);
PikaStatus pika_binding_machine_PWM_deinit(
    const PikaBindingCall* call,
    PikaBindingValue* result);
void pika_binding_machine_PWM_destroy(
    void* module_context,
    void* instance);

#endif

#ifdef __cplusplus
}
#endif

#endif
