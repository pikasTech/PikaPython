/* SPEC: PJ2026-050111 portable packages v0.3; portable AES core. */
#ifndef PIKA_AES_ALGORITHM_H
#define PIKA_AES_ALGORITHM_H

#include <stdint.h>

#include "pika_status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t round_key[240];
    uint8_t rounds;
} PikaAesContext;

PikaStatus pika_aes_initialize(
    PikaAesContext* context,
    const uint8_t* key,
    uint32_t key_length);
void pika_aes_encrypt_block(
    const PikaAesContext* context,
    uint8_t block[16]);
void pika_aes_decrypt_block(
    const PikaAesContext* context,
    uint8_t block[16]);

#ifdef __cplusplus
}
#endif

#endif
