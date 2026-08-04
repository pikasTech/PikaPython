/* SPEC: PJ2026-050109 network-ready v0.1; text method contract. */
#ifndef PIKA_TEXT_METHOD_H
#define PIKA_TEXT_METHOD_H

#include <stdint.h>

#include "pika_builtin.h"
#include "pika_status.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PIKA_TEXT_METHOD_PART_LIMIT
#define PIKA_TEXT_METHOD_PART_LIMIT 16u
#endif

typedef enum {
    PIKA_TEXT_METHOD_ENCODE = PIKA_BUILTIN_CATALOG_COUNT + 1u,
    PIKA_TEXT_METHOD_DECODE = PIKA_BUILTIN_CATALOG_COUNT + 2u,
    PIKA_TEXT_METHOD_FIND = PIKA_BUILTIN_CATALOG_COUNT + 3u,
    PIKA_TEXT_METHOD_SPLIT = PIKA_BUILTIN_CATALOG_COUNT + 4u,
    PIKA_TEXT_METHOD_STARTSWITH = PIKA_BUILTIN_CATALOG_COUNT + 5u,
    PIKA_TEXT_METHOD_LOWER = PIKA_BUILTIN_CATALOG_COUNT + 6u,
} PikaTextMethodId;

typedef struct {
    uint8_t name_offset;
    uint8_t minimum_arguments;
    uint8_t maximum_arguments;
} PikaTextMethodDescriptor;

const PikaTextMethodDescriptor* pika_text_method_descriptor(
    uint16_t method_id);
PikaStatus pika_text_method_find(
    const char* name,
    uint16_t* method_id);

#ifdef __cplusplus
}
#endif

#endif
