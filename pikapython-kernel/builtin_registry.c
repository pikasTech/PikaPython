/* SPEC: PJ2026-0501 V2 kernel v0.5; embedded builtin registry. */
#include "pika_builtin.h"
#include "pika_capability.h"

#include <stddef.h>
#include <string.h>

#if PIKA_BUILTIN_RUNTIME_ENABLE

static const PikaBuiltinDescriptor descriptors[PIKA_BUILTIN_CATALOG_COUNT] = {
#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE
    [PIKA_BUILTIN_ABS] = {"abs", 1u, 1u, 1u},
    [PIKA_BUILTIN_ALL] = {"all", 1u, 1u, 1u},
    [PIKA_BUILTIN_ANY] = {"any", 1u, 1u, 1u},
    [PIKA_BUILTIN_ASCII] = {"ascii", 1u, 1u, 1u},
    [PIKA_BUILTIN_BIN] = {"bin", 1u, 1u, 1u},
    [PIKA_BUILTIN_CHR] = {"chr", 1u, 1u, 1u},
    [PIKA_BUILTIN_DIVMOD] = {"divmod", 2u, 2u, 1u},
    [PIKA_BUILTIN_ENUMERATE] = {"enumerate", 1u, 2u, 1u},
    [PIKA_BUILTIN_FLOAT] = {"float", 0u, 1u, 1u},
    [PIKA_BUILTIN_FORMAT] = {"format", 1u, 2u, 1u},
    [PIKA_BUILTIN_GETATTR] = {"getattr", 2u, 3u, 0u},
    [PIKA_BUILTIN_HASATTR] = {"hasattr", 2u, 2u, 0u},
    [PIKA_BUILTIN_HASH] = {"hash", 1u, 1u, 1u},
    [PIKA_BUILTIN_HEX] = {"hex", 1u, 1u, 1u},
    [PIKA_BUILTIN_ID] = {"id", 1u, 1u, 1u},
    [PIKA_BUILTIN_ISINSTANCE] = {"isinstance", 2u, 2u, 1u},
    [PIKA_BUILTIN_MAX] = {"max", 1u, 255u, 1u},
    [PIKA_BUILTIN_MIN] = {"min", 1u, 255u, 1u},
    [PIKA_BUILTIN_OCT] = {"oct", 1u, 1u, 1u},
    [PIKA_BUILTIN_ORD] = {"ord", 1u, 1u, 1u},
    [PIKA_BUILTIN_POW] = {"pow", 2u, 3u, 1u},
    [PIKA_BUILTIN_REPR] = {"repr", 1u, 1u, 1u},
    [PIKA_BUILTIN_REVERSED] = {"reversed", 1u, 1u, 1u},
    [PIKA_BUILTIN_ROUND] = {"round", 1u, 2u, 1u},
    [PIKA_BUILTIN_SET] = {"set", 0u, 1u, 1u},
    [PIKA_BUILTIN_SLICE] = {"slice", 1u, 3u, 1u},
    [PIKA_BUILTIN_SORTED] = {"sorted", 1u, 4u, 1u},
    [PIKA_BUILTIN_SUM] = {"sum", 1u, 2u, 1u},
    [PIKA_BUILTIN_ZIP] = {"zip", 0u, 255u, 1u},
    [PIKA_BUILTIN_ITER] = {"iter", 1u, 1u, 1u},
    [PIKA_BUILTIN_NEXT] = {"next", 1u, 2u, 1u},
    [PIKA_BUILTIN_MAP] = {"map", 2u, 255u, 0u},
    [PIKA_BUILTIN_FILTER] = {"filter", 2u, 2u, 0u},
#endif
#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_ITER_RANGE_ENABLE
    [PIKA_BUILTIN_RANGE] = {"range", 1u, 3u, 1u},
#endif
#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_TYPE_ENABLE
    [PIKA_BUILTIN_TYPE] = {"type", 1u, 1u, 1u},
#endif
#if PIKA_CAPABILITY_BUILTIN_HELP_ENABLE
    [PIKA_BUILTIN_HELP] = {"help", 0u, 1u, 1u},
#endif
#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_SCALAR_CONVERT_ENABLE
    [PIKA_BUILTIN_BOOL] = {"bool", 0u, 1u, 1u},
    [PIKA_BUILTIN_BYTES] = {"bytes", 0u, 2u, 1u},
    [PIKA_BUILTIN_INT] = {"int", 0u, 2u, 1u},
    [PIKA_BUILTIN_STR] = {"str", 0u, 1u, 1u},
#endif
#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_CONTAINER_CONSTRUCT_ENABLE
    [PIKA_BUILTIN_BYTEARRAY] = {"bytearray", 0u, 2u, 1u},
    [PIKA_BUILTIN_DICT] = {"dict", 0u, 1u, 1u},
    [PIKA_BUILTIN_LIST] = {"list", 0u, 1u, 1u},
    [PIKA_BUILTIN_TUPLE] = {"tuple", 0u, 1u, 1u},
#endif
#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_LEN_ENABLE
    [PIKA_BUILTIN_LEN] = {"len", 1u, 1u, 1u},
#endif
#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_PRINT_ENABLE
    [PIKA_BUILTIN_PRINT] = {"print", 0u, 255u, 1u},
#endif
};

uint16_t pika_builtin_catalog_count(void) {
    return (uint16_t)PIKA_BUILTIN_CATALOG_COUNT;
}

uint16_t pika_builtin_implemented_count(void) {
    uint16_t count = 0u;
    uint16_t index;
    for (index = 0u; index < PIKA_BUILTIN_CATALOG_COUNT; ++index) {
        count = (uint16_t)(count + descriptors[index].implemented);
    }
    return count;
}

const PikaBuiltinDescriptor* pika_builtin_descriptor(PikaBuiltinId id) {
    return id < PIKA_BUILTIN_CATALOG_COUNT
               ? &descriptors[id]
               : NULL;
}

PikaStatus pika_builtin_find(const char* name, PikaBuiltinId* id) {
    uint16_t index;
    if (name == NULL || id == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0u; index < PIKA_BUILTIN_CATALOG_COUNT; ++index) {
        if (descriptors[index].name != NULL &&
            strcmp(name, descriptors[index].name) == 0) {
            *id = (PikaBuiltinId)index;
            return descriptors[index].implemented
                       ? PIKA_STATUS_OK
                       : PIKA_STATUS_UNSUPPORTED_SYNTAX;
        }
    }
    return PIKA_STATUS_UNDEFINED_NAME;
}

#endif
