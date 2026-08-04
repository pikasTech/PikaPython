/* SPEC: PJ2026-050111 可移植包 v0.10; host module image exporter. */
#ifndef PIKA_PROGRAM_PREBUILD_H
#define PIKA_PROGRAM_PREBUILD_H

#include "pika_frontend.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char* name;
    const uint8_t* source_digest;
    uint16_t source_digest_length;
} PikaProgramImageSourceIdentity;

typedef struct {
    const PikaProgramImageSourceIdentity* modules;
    uint16_t module_count;
    const uint8_t* configuration_digest;
    uint16_t configuration_digest_length;
    const uint8_t* capability_digest;
    uint16_t capability_digest_length;
} PikaProgramImageExportIdentity;

PikaStatus pika_program_image_export(
    const PikaCompiledModule* module,
    const char* source_path,
    const char* header_path,
    const char* metadata_path,
    const char* symbol,
    const PikaProgramImageExportIdentity* identity);

#ifdef __cplusplus
}
#endif

#endif
