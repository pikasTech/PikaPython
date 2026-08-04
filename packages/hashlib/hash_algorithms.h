/* SPEC: PJ2026-050111 portable packages v0.3; portable hash core. */
#ifndef PIKA_HASH_ALGORITHMS_H
#define PIKA_HASH_ALGORITHMS_H

#include <stdint.h>

#include "pika_status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PIKA_HASH_ALGORITHM_MD5 = 0,
    PIKA_HASH_ALGORITHM_SHA1 = 1,
    PIKA_HASH_ALGORITHM_SHA224 = 2,
    PIKA_HASH_ALGORITHM_SHA256 = 3,
    PIKA_HASH_ALGORITHM_SHA384 = 4,
    PIKA_HASH_ALGORITHM_SHA512 = 5
} PikaHashAlgorithm;

typedef struct {
    uint32_t state[4];
    uint64_t total;
    uint8_t block[64];
    uint32_t used;
} PikaMd5Context;

typedef struct {
    uint32_t state[5];
    uint64_t total;
    uint8_t block[64];
    uint32_t used;
} PikaSha1Context;

typedef struct {
    uint32_t state[8];
    uint64_t total;
    uint8_t block[64];
    uint32_t used;
} PikaSha256Context;

typedef struct {
    uint64_t state[8];
    uint64_t total_low;
    uint64_t total_high;
    uint8_t block[128];
    uint32_t used;
} PikaSha512Context;

typedef struct {
    PikaHashAlgorithm algorithm;
    union {
        PikaMd5Context md5;
        PikaSha1Context sha1;
        PikaSha256Context sha256;
        PikaSha512Context sha512;
    } state;
} PikaHashContext;

PikaStatus pika_hash_algorithm_from_name(
    const uint8_t* name,
    uint32_t length,
    PikaHashAlgorithm* algorithm);
uint32_t pika_hash_digest_size(PikaHashAlgorithm algorithm);
uint32_t pika_hash_block_size(PikaHashAlgorithm algorithm);
PikaStatus pika_hash_initialize(
    PikaHashContext* context,
    PikaHashAlgorithm algorithm);
PikaStatus pika_hash_update(
    PikaHashContext* context,
    const uint8_t* data,
    uint32_t length);
PikaStatus pika_hash_finalize(
    const PikaHashContext* context,
    uint8_t* output,
    uint32_t capacity);

#ifdef __cplusplus
}
#endif

#endif
