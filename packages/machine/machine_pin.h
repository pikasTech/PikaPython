/* SPEC: PJ2026-050112 STM32 machine v0.2; Pin facade contract. */
#ifndef PIKA_MACHINE_PIN_H
#define PIKA_MACHINE_PIN_H

#include "pika_binding.h"
#include "pika_capability_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#if PIKA_CAPABILITY_MACHINE_PIN_ENABLE

#ifndef PIKA_MACHINE_PIN_OBJECT_LIMIT
#define PIKA_MACHINE_PIN_OBJECT_LIMIT 8u
#endif

#if PIKA_MACHINE_PIN_OBJECT_LIMIT < 1u
#error "PIKA_MACHINE_PIN_OBJECT_LIMIT must be at least 1"
#endif

PikaStatus pika_binding_machine_Pin_init(
    const PikaBindingCall* call,
    PikaBindingValue* result);
PikaStatus pika_binding_machine_Pin_method_init(
    const PikaBindingCall* call,
    PikaBindingValue* result);
PikaStatus pika_binding_machine_Pin_value(
    const PikaBindingCall* call,
    PikaBindingValue* result);
PikaStatus pika_binding_machine_Pin_on(
    const PikaBindingCall* call,
    PikaBindingValue* result);
PikaStatus pika_binding_machine_Pin_off(
    const PikaBindingCall* call,
    PikaBindingValue* result);
#if PIKA_CAPABILITY_BINDING_PYTHON_CALLABLE_ENABLE
PikaStatus pika_binding_machine_Pin_irq(
    const PikaBindingCall* call,
    PikaBindingValue* result);
PikaStatus pika_binding_machine_Pin_trigger(
    const PikaBindingCall* call,
    PikaBindingValue* result);
#endif
void pika_binding_machine_Pin_destroy(
    void* module_context,
    void* instance);
PikaStatus pika_machine_pin_identifier(
    void* instance,
    int64_t* identifier);

#endif

#ifdef __cplusplus
}
#endif

#endif
