/* SPEC: PJ2026-0501 V2 kernel v0.5; embedded builtin registry contract. */
#ifndef PIKA_BUILTIN_H
#define PIKA_BUILTIN_H

#include <stdint.h>

#include "pika_status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PIKA_BUILTIN_CATALOG_COUNT 46u

typedef enum {
    PIKA_BUILTIN_ABS = 0,
    PIKA_BUILTIN_ALL = 1,
    PIKA_BUILTIN_ANY = 2,
    PIKA_BUILTIN_ASCII = 3,
    PIKA_BUILTIN_BIN = 4,
    PIKA_BUILTIN_BOOL = 5,
    PIKA_BUILTIN_BYTES = 6,
    PIKA_BUILTIN_BYTEARRAY = 7,
    PIKA_BUILTIN_CHR = 8,
    PIKA_BUILTIN_DICT = 9,
    PIKA_BUILTIN_DIVMOD = 10,
    PIKA_BUILTIN_ENUMERATE = 11,
    PIKA_BUILTIN_FLOAT = 12,
    PIKA_BUILTIN_FORMAT = 13,
    PIKA_BUILTIN_GETATTR = 14,
    PIKA_BUILTIN_HASATTR = 15,
    PIKA_BUILTIN_HASH = 16,
    PIKA_BUILTIN_HEX = 17,
    PIKA_BUILTIN_ID = 18,
    PIKA_BUILTIN_INT = 19,
    PIKA_BUILTIN_ISINSTANCE = 20,
    PIKA_BUILTIN_LEN = 21,
    PIKA_BUILTIN_LIST = 22,
    PIKA_BUILTIN_MAX = 23,
    PIKA_BUILTIN_MIN = 24,
    PIKA_BUILTIN_OCT = 25,
    PIKA_BUILTIN_ORD = 26,
    PIKA_BUILTIN_POW = 27,
    PIKA_BUILTIN_PRINT = 28,
    PIKA_BUILTIN_RANGE = 29,
    PIKA_BUILTIN_REPR = 30,
    PIKA_BUILTIN_REVERSED = 31,
    PIKA_BUILTIN_ROUND = 32,
    PIKA_BUILTIN_SET = 33,
    PIKA_BUILTIN_SLICE = 34,
    PIKA_BUILTIN_SORTED = 35,
    PIKA_BUILTIN_STR = 36,
    PIKA_BUILTIN_SUM = 37,
    PIKA_BUILTIN_TUPLE = 38,
    PIKA_BUILTIN_TYPE = 39,
    PIKA_BUILTIN_ZIP = 40,
    PIKA_BUILTIN_ITER = 41,
    PIKA_BUILTIN_NEXT = 42,
    PIKA_BUILTIN_MAP = 43,
    PIKA_BUILTIN_FILTER = 44,
    PIKA_BUILTIN_HELP = 45,
} PikaBuiltinId;

typedef struct {
    const char* name;
    uint8_t minimum_arguments;
    uint8_t maximum_arguments;
    uint8_t implemented;
} PikaBuiltinDescriptor;

uint16_t pika_builtin_catalog_count(void);
uint16_t pika_builtin_implemented_count(void);
const PikaBuiltinDescriptor* pika_builtin_descriptor(PikaBuiltinId id);
PikaStatus pika_builtin_find(const char* name, PikaBuiltinId* id);

#ifdef __cplusplus
}
#endif

#endif
