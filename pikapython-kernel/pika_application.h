/* SPEC: PJ2026-0501 kernel v0.34; embedded application startup API. */
#ifndef PIKA_APPLICATION_H
#define PIKA_APPLICATION_H

#include <stddef.h>
#include <stdint.h>

#include "pika_binding.h"
#include "pika_capability_config.h"
#include "pika_status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PIKA_FROZEN_MODULE_SOURCE = 1,
    PIKA_FROZEN_MODULE_PROGRAM = 2,
} PikaFrozenModuleFormat;

typedef struct {
    const char* name;
    const uint8_t* data;
    size_t size;
    PikaFrozenModuleFormat format;
    const uint8_t* source_digest;
    uint16_t source_digest_length;
} PikaFrozenModule;

typedef struct {
    uint32_t schema_version;
    const PikaFrozenModule* modules;
    size_t module_count;
    const PikaBindingModule* const* binding_modules;
    size_t binding_module_count;
    const uint8_t* configuration_digest;
    uint16_t configuration_digest_length;
    const uint8_t* capability_digest;
    uint16_t capability_digest_length;
} PikaApplicationDescriptor;

#define PIKA_APPLICATION_SCHEMA_VERSION 2u

PikaStatus pika_application_start(
    const PikaApplicationDescriptor* descriptor);
#if PIKA_REPL_ENABLE
PikaStatus pika_application_repl(void);
#endif
void pika_application_stop(void);

#ifdef __cplusplus
}
#endif

#endif
