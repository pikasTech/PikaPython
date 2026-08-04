/* SPEC: PJ2026-050111 portable packages v0.3; MD5 and SHA family. */
#include "hash_algorithms.h"

#include <stddef.h>
#include <string.h>

static uint32_t load32_little(const uint8_t* value) {
    return (uint32_t)value[0] |
           ((uint32_t)value[1] << 8u) |
           ((uint32_t)value[2] << 16u) |
           ((uint32_t)value[3] << 24u);
}

static uint32_t load32_big(const uint8_t* value) {
    return ((uint32_t)value[0] << 24u) |
           ((uint32_t)value[1] << 16u) |
           ((uint32_t)value[2] << 8u) |
           (uint32_t)value[3];
}

static uint64_t load64_big(const uint8_t* value) {
    uint64_t result = 0u;
    uint32_t index;
    for (index = 0u; index < 8u; ++index) {
        result = (result << 8u) | value[index];
    }
    return result;
}

static void store32_little(uint8_t* output, uint32_t value) {
    output[0] = (uint8_t)value;
    output[1] = (uint8_t)(value >> 8u);
    output[2] = (uint8_t)(value >> 16u);
    output[3] = (uint8_t)(value >> 24u);
}

static void store32_big(uint8_t* output, uint32_t value) {
    output[0] = (uint8_t)(value >> 24u);
    output[1] = (uint8_t)(value >> 16u);
    output[2] = (uint8_t)(value >> 8u);
    output[3] = (uint8_t)value;
}

static void store64_little(uint8_t* output, uint64_t value) {
    uint32_t index;
    for (index = 0u; index < 8u; ++index) {
        output[index] = (uint8_t)(value >> (index * 8u));
    }
}

static void store64_big(uint8_t* output, uint64_t value) {
    uint32_t index;
    for (index = 0u; index < 8u; ++index) {
        output[index] =
            (uint8_t)(value >> ((7u - index) * 8u));
    }
}

static uint32_t rotate_left32(uint32_t value, uint32_t shift) {
    return (value << shift) | (value >> (32u - shift));
}

static uint32_t rotate_right32(uint32_t value, uint32_t shift) {
    return (value >> shift) | (value << (32u - shift));
}

static uint64_t rotate_right64(uint64_t value, uint32_t shift) {
    return (value >> shift) | (value << (64u - shift));
}

static void md5_transform(PikaMd5Context* context,
                          const uint8_t block[64]) {
    static const uint32_t shifts[64] = {
        7u, 12u, 17u, 22u, 7u, 12u, 17u, 22u,
        7u, 12u, 17u, 22u, 7u, 12u, 17u, 22u,
        5u, 9u, 14u, 20u, 5u, 9u, 14u, 20u,
        5u, 9u, 14u, 20u, 5u, 9u, 14u, 20u,
        4u, 11u, 16u, 23u, 4u, 11u, 16u, 23u,
        4u, 11u, 16u, 23u, 4u, 11u, 16u, 23u,
        6u, 10u, 15u, 21u, 6u, 10u, 15u, 21u,
        6u, 10u, 15u, 21u, 6u, 10u, 15u, 21u,
    };
    static const uint32_t constants[64] = {
        UINT32_C(0xd76aa478), UINT32_C(0xe8c7b756),
        UINT32_C(0x242070db), UINT32_C(0xc1bdceee),
        UINT32_C(0xf57c0faf), UINT32_C(0x4787c62a),
        UINT32_C(0xa8304613), UINT32_C(0xfd469501),
        UINT32_C(0x698098d8), UINT32_C(0x8b44f7af),
        UINT32_C(0xffff5bb1), UINT32_C(0x895cd7be),
        UINT32_C(0x6b901122), UINT32_C(0xfd987193),
        UINT32_C(0xa679438e), UINT32_C(0x49b40821),
        UINT32_C(0xf61e2562), UINT32_C(0xc040b340),
        UINT32_C(0x265e5a51), UINT32_C(0xe9b6c7aa),
        UINT32_C(0xd62f105d), UINT32_C(0x02441453),
        UINT32_C(0xd8a1e681), UINT32_C(0xe7d3fbc8),
        UINT32_C(0x21e1cde6), UINT32_C(0xc33707d6),
        UINT32_C(0xf4d50d87), UINT32_C(0x455a14ed),
        UINT32_C(0xa9e3e905), UINT32_C(0xfcefa3f8),
        UINT32_C(0x676f02d9), UINT32_C(0x8d2a4c8a),
        UINT32_C(0xfffa3942), UINT32_C(0x8771f681),
        UINT32_C(0x6d9d6122), UINT32_C(0xfde5380c),
        UINT32_C(0xa4beea44), UINT32_C(0x4bdecfa9),
        UINT32_C(0xf6bb4b60), UINT32_C(0xbebfbc70),
        UINT32_C(0x289b7ec6), UINT32_C(0xeaa127fa),
        UINT32_C(0xd4ef3085), UINT32_C(0x04881d05),
        UINT32_C(0xd9d4d039), UINT32_C(0xe6db99e5),
        UINT32_C(0x1fa27cf8), UINT32_C(0xc4ac5665),
        UINT32_C(0xf4292244), UINT32_C(0x432aff97),
        UINT32_C(0xab9423a7), UINT32_C(0xfc93a039),
        UINT32_C(0x655b59c3), UINT32_C(0x8f0ccc92),
        UINT32_C(0xffeff47d), UINT32_C(0x85845dd1),
        UINT32_C(0x6fa87e4f), UINT32_C(0xfe2ce6e0),
        UINT32_C(0xa3014314), UINT32_C(0x4e0811a1),
        UINT32_C(0xf7537e82), UINT32_C(0xbd3af235),
        UINT32_C(0x2ad7d2bb), UINT32_C(0xeb86d391),
    };
    uint32_t words[16];
    uint32_t a = context->state[0];
    uint32_t b = context->state[1];
    uint32_t c = context->state[2];
    uint32_t d = context->state[3];
    uint32_t index;
    for (index = 0u; index < 16u; ++index) {
        words[index] = load32_little(block + index * 4u);
    }
    for (index = 0u; index < 64u; ++index) {
        uint32_t mixed;
        uint32_t word_index;
        uint32_t previous_d = d;
        if (index < 16u) {
            mixed = (b & c) | ((~b) & d);
            word_index = index;
        } else if (index < 32u) {
            mixed = (d & b) | ((~d) & c);
            word_index = (5u * index + 1u) & 15u;
        } else if (index < 48u) {
            mixed = b ^ c ^ d;
            word_index = (3u * index + 5u) & 15u;
        } else {
            mixed = c ^ (b | (~d));
            word_index = (7u * index) & 15u;
        }
        d = c;
        c = b;
        b += rotate_left32(
            a + mixed + constants[index] + words[word_index],
            shifts[index]);
        a = previous_d;
    }
    context->state[0] += a;
    context->state[1] += b;
    context->state[2] += c;
    context->state[3] += d;
}

static void md5_update(PikaMd5Context* context,
                       const uint8_t* data,
                       uint32_t length) {
    uint32_t consumed = 0u;
    context->total += length;
    while (consumed < length) {
        uint32_t available = 64u - context->used;
        uint32_t copy = length - consumed < available
                            ? length - consumed
                            : available;
        memcpy(context->block + context->used, data + consumed, copy);
        context->used += copy;
        consumed += copy;
        if (context->used == 64u) {
            md5_transform(context, context->block);
            context->used = 0u;
        }
    }
}

static void md5_finalize(PikaMd5Context context, uint8_t* output) {
    uint8_t padding[128];
    uint8_t length_bytes[8];
    uint64_t bits = context.total * UINT64_C(8);
    uint32_t padding_length =
        context.used < 56u ? 56u - context.used : 120u - context.used;
    uint32_t index;
    memset(padding, 0, sizeof(padding));
    padding[0] = 0x80u;
    md5_update(&context, padding, padding_length);
    store64_little(length_bytes, bits);
    md5_update(&context, length_bytes, sizeof(length_bytes));
    for (index = 0u; index < 4u; ++index) {
        store32_little(output + index * 4u, context.state[index]);
    }
}

static void sha1_transform(PikaSha1Context* context,
                           const uint8_t block[64]) {
    uint32_t words[80];
    uint32_t a = context->state[0];
    uint32_t b = context->state[1];
    uint32_t c = context->state[2];
    uint32_t d = context->state[3];
    uint32_t e = context->state[4];
    uint32_t index;
    for (index = 0u; index < 16u; ++index) {
        words[index] = load32_big(block + index * 4u);
    }
    for (; index < 80u; ++index) {
        words[index] = rotate_left32(
            words[index - 3u] ^ words[index - 8u] ^
                words[index - 14u] ^ words[index - 16u],
            1u);
    }
    for (index = 0u; index < 80u; ++index) {
        uint32_t choice;
        uint32_t constant;
        uint32_t next;
        if (index < 20u) {
            choice = (b & c) | ((~b) & d);
            constant = UINT32_C(0x5a827999);
        } else if (index < 40u) {
            choice = b ^ c ^ d;
            constant = UINT32_C(0x6ed9eba1);
        } else if (index < 60u) {
            choice = (b & c) | (b & d) | (c & d);
            constant = UINT32_C(0x8f1bbcdc);
        } else {
            choice = b ^ c ^ d;
            constant = UINT32_C(0xca62c1d6);
        }
        next = rotate_left32(a, 5u) + choice + e +
               constant + words[index];
        e = d;
        d = c;
        c = rotate_left32(b, 30u);
        b = a;
        a = next;
    }
    context->state[0] += a;
    context->state[1] += b;
    context->state[2] += c;
    context->state[3] += d;
    context->state[4] += e;
}

static void sha1_update(PikaSha1Context* context,
                        const uint8_t* data,
                        uint32_t length) {
    uint32_t consumed = 0u;
    context->total += length;
    while (consumed < length) {
        uint32_t available = 64u - context->used;
        uint32_t copy = length - consumed < available
                            ? length - consumed
                            : available;
        memcpy(context->block + context->used, data + consumed, copy);
        context->used += copy;
        consumed += copy;
        if (context->used == 64u) {
            sha1_transform(context, context->block);
            context->used = 0u;
        }
    }
}

static void sha1_finalize(PikaSha1Context context, uint8_t* output) {
    uint8_t padding[128];
    uint8_t length_bytes[8];
    uint64_t bits = context.total * UINT64_C(8);
    uint32_t padding_length =
        context.used < 56u ? 56u - context.used : 120u - context.used;
    uint32_t index;
    memset(padding, 0, sizeof(padding));
    padding[0] = 0x80u;
    sha1_update(&context, padding, padding_length);
    store64_big(length_bytes, bits);
    sha1_update(&context, length_bytes, sizeof(length_bytes));
    for (index = 0u; index < 5u; ++index) {
        store32_big(output + index * 4u, context.state[index]);
    }
}

static const uint32_t sha256_constants[64] = {
    UINT32_C(0x428a2f98), UINT32_C(0x71374491),
    UINT32_C(0xb5c0fbcf), UINT32_C(0xe9b5dba5),
    UINT32_C(0x3956c25b), UINT32_C(0x59f111f1),
    UINT32_C(0x923f82a4), UINT32_C(0xab1c5ed5),
    UINT32_C(0xd807aa98), UINT32_C(0x12835b01),
    UINT32_C(0x243185be), UINT32_C(0x550c7dc3),
    UINT32_C(0x72be5d74), UINT32_C(0x80deb1fe),
    UINT32_C(0x9bdc06a7), UINT32_C(0xc19bf174),
    UINT32_C(0xe49b69c1), UINT32_C(0xefbe4786),
    UINT32_C(0x0fc19dc6), UINT32_C(0x240ca1cc),
    UINT32_C(0x2de92c6f), UINT32_C(0x4a7484aa),
    UINT32_C(0x5cb0a9dc), UINT32_C(0x76f988da),
    UINT32_C(0x983e5152), UINT32_C(0xa831c66d),
    UINT32_C(0xb00327c8), UINT32_C(0xbf597fc7),
    UINT32_C(0xc6e00bf3), UINT32_C(0xd5a79147),
    UINT32_C(0x06ca6351), UINT32_C(0x14292967),
    UINT32_C(0x27b70a85), UINT32_C(0x2e1b2138),
    UINT32_C(0x4d2c6dfc), UINT32_C(0x53380d13),
    UINT32_C(0x650a7354), UINT32_C(0x766a0abb),
    UINT32_C(0x81c2c92e), UINT32_C(0x92722c85),
    UINT32_C(0xa2bfe8a1), UINT32_C(0xa81a664b),
    UINT32_C(0xc24b8b70), UINT32_C(0xc76c51a3),
    UINT32_C(0xd192e819), UINT32_C(0xd6990624),
    UINT32_C(0xf40e3585), UINT32_C(0x106aa070),
    UINT32_C(0x19a4c116), UINT32_C(0x1e376c08),
    UINT32_C(0x2748774c), UINT32_C(0x34b0bcb5),
    UINT32_C(0x391c0cb3), UINT32_C(0x4ed8aa4a),
    UINT32_C(0x5b9cca4f), UINT32_C(0x682e6ff3),
    UINT32_C(0x748f82ee), UINT32_C(0x78a5636f),
    UINT32_C(0x84c87814), UINT32_C(0x8cc70208),
    UINT32_C(0x90befffa), UINT32_C(0xa4506ceb),
    UINT32_C(0xbef9a3f7), UINT32_C(0xc67178f2),
};

static void sha256_transform(PikaSha256Context* context,
                             const uint8_t block[64]) {
    uint32_t words[64];
    uint32_t a = context->state[0];
    uint32_t b = context->state[1];
    uint32_t c = context->state[2];
    uint32_t d = context->state[3];
    uint32_t e = context->state[4];
    uint32_t f = context->state[5];
    uint32_t g = context->state[6];
    uint32_t h = context->state[7];
    uint32_t index;
    for (index = 0u; index < 16u; ++index) {
        words[index] = load32_big(block + index * 4u);
    }
    for (; index < 64u; ++index) {
        uint32_t lower = rotate_right32(words[index - 15u], 7u) ^
                         rotate_right32(words[index - 15u], 18u) ^
                         (words[index - 15u] >> 3u);
        uint32_t upper = rotate_right32(words[index - 2u], 17u) ^
                         rotate_right32(words[index - 2u], 19u) ^
                         (words[index - 2u] >> 10u);
        words[index] = words[index - 16u] + lower +
                       words[index - 7u] + upper;
    }
    for (index = 0u; index < 64u; ++index) {
        uint32_t upper = rotate_right32(e, 6u) ^
                         rotate_right32(e, 11u) ^
                         rotate_right32(e, 25u);
        uint32_t choice = (e & f) ^ ((~e) & g);
        uint32_t first = h + upper + choice +
                         sha256_constants[index] + words[index];
        uint32_t lower = rotate_right32(a, 2u) ^
                         rotate_right32(a, 13u) ^
                         rotate_right32(a, 22u);
        uint32_t majority = (a & b) ^ (a & c) ^ (b & c);
        uint32_t second = lower + majority;
        h = g;
        g = f;
        f = e;
        e = d + first;
        d = c;
        c = b;
        b = a;
        a = first + second;
    }
    context->state[0] += a;
    context->state[1] += b;
    context->state[2] += c;
    context->state[3] += d;
    context->state[4] += e;
    context->state[5] += f;
    context->state[6] += g;
    context->state[7] += h;
}

static void sha256_update(PikaSha256Context* context,
                          const uint8_t* data,
                          uint32_t length) {
    uint32_t consumed = 0u;
    context->total += length;
    while (consumed < length) {
        uint32_t available = 64u - context->used;
        uint32_t copy = length - consumed < available
                            ? length - consumed
                            : available;
        memcpy(context->block + context->used, data + consumed, copy);
        context->used += copy;
        consumed += copy;
        if (context->used == 64u) {
            sha256_transform(context, context->block);
            context->used = 0u;
        }
    }
}

static void sha256_finalize(PikaSha256Context context,
                            uint8_t* output,
                            uint32_t words) {
    uint8_t padding[128];
    uint8_t length_bytes[8];
    uint64_t bits = context.total * UINT64_C(8);
    uint32_t padding_length =
        context.used < 56u ? 56u - context.used : 120u - context.used;
    uint32_t index;
    memset(padding, 0, sizeof(padding));
    padding[0] = 0x80u;
    sha256_update(&context, padding, padding_length);
    store64_big(length_bytes, bits);
    sha256_update(&context, length_bytes, sizeof(length_bytes));
    for (index = 0u; index < words; ++index) {
        store32_big(output + index * 4u, context.state[index]);
    }
}

static const uint64_t sha512_constants[80] = {
    UINT64_C(0x428a2f98d728ae22), UINT64_C(0x7137449123ef65cd),
    UINT64_C(0xb5c0fbcfec4d3b2f), UINT64_C(0xe9b5dba58189dbbc),
    UINT64_C(0x3956c25bf348b538), UINT64_C(0x59f111f1b605d019),
    UINT64_C(0x923f82a4af194f9b), UINT64_C(0xab1c5ed5da6d8118),
    UINT64_C(0xd807aa98a3030242), UINT64_C(0x12835b0145706fbe),
    UINT64_C(0x243185be4ee4b28c), UINT64_C(0x550c7dc3d5ffb4e2),
    UINT64_C(0x72be5d74f27b896f), UINT64_C(0x80deb1fe3b1696b1),
    UINT64_C(0x9bdc06a725c71235), UINT64_C(0xc19bf174cf692694),
    UINT64_C(0xe49b69c19ef14ad2), UINT64_C(0xefbe4786384f25e3),
    UINT64_C(0x0fc19dc68b8cd5b5), UINT64_C(0x240ca1cc77ac9c65),
    UINT64_C(0x2de92c6f592b0275), UINT64_C(0x4a7484aa6ea6e483),
    UINT64_C(0x5cb0a9dcbd41fbd4), UINT64_C(0x76f988da831153b5),
    UINT64_C(0x983e5152ee66dfab), UINT64_C(0xa831c66d2db43210),
    UINT64_C(0xb00327c898fb213f), UINT64_C(0xbf597fc7beef0ee4),
    UINT64_C(0xc6e00bf33da88fc2), UINT64_C(0xd5a79147930aa725),
    UINT64_C(0x06ca6351e003826f), UINT64_C(0x142929670a0e6e70),
    UINT64_C(0x27b70a8546d22ffc), UINT64_C(0x2e1b21385c26c926),
    UINT64_C(0x4d2c6dfc5ac42aed), UINT64_C(0x53380d139d95b3df),
    UINT64_C(0x650a73548baf63de), UINT64_C(0x766a0abb3c77b2a8),
    UINT64_C(0x81c2c92e47edaee6), UINT64_C(0x92722c851482353b),
    UINT64_C(0xa2bfe8a14cf10364), UINT64_C(0xa81a664bbc423001),
    UINT64_C(0xc24b8b70d0f89791), UINT64_C(0xc76c51a30654be30),
    UINT64_C(0xd192e819d6ef5218), UINT64_C(0xd69906245565a910),
    UINT64_C(0xf40e35855771202a), UINT64_C(0x106aa07032bbd1b8),
    UINT64_C(0x19a4c116b8d2d0c8), UINT64_C(0x1e376c085141ab53),
    UINT64_C(0x2748774cdf8eeb99), UINT64_C(0x34b0bcb5e19b48a8),
    UINT64_C(0x391c0cb3c5c95a63), UINT64_C(0x4ed8aa4ae3418acb),
    UINT64_C(0x5b9cca4f7763e373), UINT64_C(0x682e6ff3d6b2b8a3),
    UINT64_C(0x748f82ee5defb2fc), UINT64_C(0x78a5636f43172f60),
    UINT64_C(0x84c87814a1f0ab72), UINT64_C(0x8cc702081a6439ec),
    UINT64_C(0x90befffa23631e28), UINT64_C(0xa4506cebde82bde9),
    UINT64_C(0xbef9a3f7b2c67915), UINT64_C(0xc67178f2e372532b),
    UINT64_C(0xca273eceea26619c), UINT64_C(0xd186b8c721c0c207),
    UINT64_C(0xeada7dd6cde0eb1e), UINT64_C(0xf57d4f7fee6ed178),
    UINT64_C(0x06f067aa72176fba), UINT64_C(0x0a637dc5a2c898a6),
    UINT64_C(0x113f9804bef90dae), UINT64_C(0x1b710b35131c471b),
    UINT64_C(0x28db77f523047d84), UINT64_C(0x32caab7b40c72493),
    UINT64_C(0x3c9ebe0a15c9bebc), UINT64_C(0x431d67c49c100d4c),
    UINT64_C(0x4cc5d4becb3e42b6), UINT64_C(0x597f299cfc657e2a),
    UINT64_C(0x5fcb6fab3ad6faec), UINT64_C(0x6c44198c4a475817),
};

static void sha512_transform(PikaSha512Context* context,
                             const uint8_t block[128]) {
    uint64_t words[80];
    uint64_t a = context->state[0];
    uint64_t b = context->state[1];
    uint64_t c = context->state[2];
    uint64_t d = context->state[3];
    uint64_t e = context->state[4];
    uint64_t f = context->state[5];
    uint64_t g = context->state[6];
    uint64_t h = context->state[7];
    uint32_t index;
    for (index = 0u; index < 16u; ++index) {
        words[index] = load64_big(block + index * 8u);
    }
    for (; index < 80u; ++index) {
        uint64_t lower = rotate_right64(words[index - 15u], 1u) ^
                         rotate_right64(words[index - 15u], 8u) ^
                         (words[index - 15u] >> 7u);
        uint64_t upper = rotate_right64(words[index - 2u], 19u) ^
                         rotate_right64(words[index - 2u], 61u) ^
                         (words[index - 2u] >> 6u);
        words[index] = words[index - 16u] + lower +
                       words[index - 7u] + upper;
    }
    for (index = 0u; index < 80u; ++index) {
        uint64_t upper = rotate_right64(e, 14u) ^
                         rotate_right64(e, 18u) ^
                         rotate_right64(e, 41u);
        uint64_t choice = (e & f) ^ ((~e) & g);
        uint64_t first = h + upper + choice +
                         sha512_constants[index] + words[index];
        uint64_t lower = rotate_right64(a, 28u) ^
                         rotate_right64(a, 34u) ^
                         rotate_right64(a, 39u);
        uint64_t majority = (a & b) ^ (a & c) ^ (b & c);
        uint64_t second = lower + majority;
        h = g;
        g = f;
        f = e;
        e = d + first;
        d = c;
        c = b;
        b = a;
        a = first + second;
    }
    context->state[0] += a;
    context->state[1] += b;
    context->state[2] += c;
    context->state[3] += d;
    context->state[4] += e;
    context->state[5] += f;
    context->state[6] += g;
    context->state[7] += h;
}

static void sha512_update(PikaSha512Context* context,
                          const uint8_t* data,
                          uint32_t length) {
    uint32_t consumed = 0u;
    uint64_t previous = context->total_low;
    context->total_low += length;
    if (context->total_low < previous) ++context->total_high;
    while (consumed < length) {
        uint32_t available = 128u - context->used;
        uint32_t copy = length - consumed < available
                            ? length - consumed
                            : available;
        memcpy(context->block + context->used, data + consumed, copy);
        context->used += copy;
        consumed += copy;
        if (context->used == 128u) {
            sha512_transform(context, context->block);
            context->used = 0u;
        }
    }
}

static void sha512_finalize(PikaSha512Context context,
                            uint8_t* output,
                            uint32_t words) {
    uint8_t padding[256];
    uint8_t length_bytes[16];
    uint64_t bit_high = (context.total_high << 3u) |
                        (context.total_low >> 61u);
    uint64_t bit_low = context.total_low << 3u;
    uint32_t padding_length =
        context.used < 112u ? 112u - context.used
                            : 240u - context.used;
    uint32_t index;
    memset(padding, 0, sizeof(padding));
    padding[0] = 0x80u;
    sha512_update(&context, padding, padding_length);
    store64_big(length_bytes, bit_high);
    store64_big(length_bytes + 8u, bit_low);
    sha512_update(&context, length_bytes, sizeof(length_bytes));
    for (index = 0u; index < words; ++index) {
        store64_big(output + index * 8u, context.state[index]);
    }
}

static int name_equals(const uint8_t* name,
                       uint32_t length,
                       const char* expected) {
    uint32_t index;
    if (strlen(expected) != length) return 0;
    for (index = 0u; index < length; ++index) {
        uint8_t actual = name[index];
        uint8_t wanted = (uint8_t)expected[index];
        if (actual >= (uint8_t)'A' && actual <= (uint8_t)'Z') {
            actual = (uint8_t)(actual + ((uint8_t)'a' - (uint8_t)'A'));
        }
        if (actual != wanted) return 0;
    }
    return 1;
}

PikaStatus pika_hash_algorithm_from_name(
    const uint8_t* name,
    uint32_t length,
    PikaHashAlgorithm* algorithm) {
    if (algorithm == NULL || (length > 0u && name == NULL)) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (name_equals(name, length, "md5")) {
        *algorithm = PIKA_HASH_ALGORITHM_MD5;
    } else if (name_equals(name, length, "sha1") ||
               name_equals(name, length, "sha-1")) {
        *algorithm = PIKA_HASH_ALGORITHM_SHA1;
    } else if (name_equals(name, length, "sha224") ||
               name_equals(name, length, "sha-224")) {
        *algorithm = PIKA_HASH_ALGORITHM_SHA224;
    } else if (name_equals(name, length, "sha256") ||
               name_equals(name, length, "sha-256")) {
        *algorithm = PIKA_HASH_ALGORITHM_SHA256;
    } else if (name_equals(name, length, "sha384") ||
               name_equals(name, length, "sha-384")) {
        *algorithm = PIKA_HASH_ALGORITHM_SHA384;
    } else if (name_equals(name, length, "sha512") ||
               name_equals(name, length, "sha-512")) {
        *algorithm = PIKA_HASH_ALGORITHM_SHA512;
    } else {
        return PIKA_STATUS_UNSUPPORTED_FORMAT;
    }
    return PIKA_STATUS_OK;
}

uint32_t pika_hash_digest_size(PikaHashAlgorithm algorithm) {
    switch (algorithm) {
        case PIKA_HASH_ALGORITHM_MD5: return 16u;
        case PIKA_HASH_ALGORITHM_SHA1: return 20u;
        case PIKA_HASH_ALGORITHM_SHA224: return 28u;
        case PIKA_HASH_ALGORITHM_SHA256: return 32u;
        case PIKA_HASH_ALGORITHM_SHA384: return 48u;
        case PIKA_HASH_ALGORITHM_SHA512: return 64u;
        default: return 0u;
    }
}

uint32_t pika_hash_block_size(PikaHashAlgorithm algorithm) {
    if (algorithm == PIKA_HASH_ALGORITHM_SHA384 ||
        algorithm == PIKA_HASH_ALGORITHM_SHA512) {
        return 128u;
    }
    return pika_hash_digest_size(algorithm) == 0u ? 0u : 64u;
}

PikaStatus pika_hash_initialize(
    PikaHashContext* context,
    PikaHashAlgorithm algorithm) {
    if (context == NULL || pika_hash_digest_size(algorithm) == 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    memset(context, 0, sizeof(*context));
    context->algorithm = algorithm;
    switch (algorithm) {
        case PIKA_HASH_ALGORITHM_MD5:
            context->state.md5.state[0] = UINT32_C(0x67452301);
            context->state.md5.state[1] = UINT32_C(0xefcdab89);
            context->state.md5.state[2] = UINT32_C(0x98badcfe);
            context->state.md5.state[3] = UINT32_C(0x10325476);
            break;
        case PIKA_HASH_ALGORITHM_SHA1:
            context->state.sha1.state[0] = UINT32_C(0x67452301);
            context->state.sha1.state[1] = UINT32_C(0xefcdab89);
            context->state.sha1.state[2] = UINT32_C(0x98badcfe);
            context->state.sha1.state[3] = UINT32_C(0x10325476);
            context->state.sha1.state[4] = UINT32_C(0xc3d2e1f0);
            break;
        case PIKA_HASH_ALGORITHM_SHA224: {
            static const uint32_t initial[8] = {
                UINT32_C(0xc1059ed8), UINT32_C(0x367cd507),
                UINT32_C(0x3070dd17), UINT32_C(0xf70e5939),
                UINT32_C(0xffc00b31), UINT32_C(0x68581511),
                UINT32_C(0x64f98fa7), UINT32_C(0xbefa4fa4),
            };
            memcpy(context->state.sha256.state, initial, sizeof(initial));
            break;
        }
        case PIKA_HASH_ALGORITHM_SHA256: {
            static const uint32_t initial[8] = {
                UINT32_C(0x6a09e667), UINT32_C(0xbb67ae85),
                UINT32_C(0x3c6ef372), UINT32_C(0xa54ff53a),
                UINT32_C(0x510e527f), UINT32_C(0x9b05688c),
                UINT32_C(0x1f83d9ab), UINT32_C(0x5be0cd19),
            };
            memcpy(context->state.sha256.state, initial, sizeof(initial));
            break;
        }
        case PIKA_HASH_ALGORITHM_SHA384: {
            static const uint64_t initial[8] = {
                UINT64_C(0xcbbb9d5dc1059ed8),
                UINT64_C(0x629a292a367cd507),
                UINT64_C(0x9159015a3070dd17),
                UINT64_C(0x152fecd8f70e5939),
                UINT64_C(0x67332667ffc00b31),
                UINT64_C(0x8eb44a8768581511),
                UINT64_C(0xdb0c2e0d64f98fa7),
                UINT64_C(0x47b5481dbefa4fa4),
            };
            memcpy(context->state.sha512.state, initial, sizeof(initial));
            break;
        }
        case PIKA_HASH_ALGORITHM_SHA512: {
            static const uint64_t initial[8] = {
                UINT64_C(0x6a09e667f3bcc908),
                UINT64_C(0xbb67ae8584caa73b),
                UINT64_C(0x3c6ef372fe94f82b),
                UINT64_C(0xa54ff53a5f1d36f1),
                UINT64_C(0x510e527fade682d1),
                UINT64_C(0x9b05688c2b3e6c1f),
                UINT64_C(0x1f83d9abfb41bd6b),
                UINT64_C(0x5be0cd19137e2179),
            };
            memcpy(context->state.sha512.state, initial, sizeof(initial));
            break;
        }
        default: return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_OK;
}

PikaStatus pika_hash_update(
    PikaHashContext* context,
    const uint8_t* data,
    uint32_t length) {
    if (context == NULL || (length > 0u && data == NULL)) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    switch (context->algorithm) {
        case PIKA_HASH_ALGORITHM_MD5:
            md5_update(&context->state.md5, data, length);
            break;
        case PIKA_HASH_ALGORITHM_SHA1:
            sha1_update(&context->state.sha1, data, length);
            break;
        case PIKA_HASH_ALGORITHM_SHA224:
        case PIKA_HASH_ALGORITHM_SHA256:
            sha256_update(&context->state.sha256, data, length);
            break;
        case PIKA_HASH_ALGORITHM_SHA384:
        case PIKA_HASH_ALGORITHM_SHA512:
            sha512_update(&context->state.sha512, data, length);
            break;
        default: return PIKA_STATUS_INVALID_OPERAND;
    }
    return PIKA_STATUS_OK;
}

PikaStatus pika_hash_finalize(
    const PikaHashContext* context,
    uint8_t* output,
    uint32_t capacity) {
    uint32_t required;
    if (context == NULL || output == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    required = pika_hash_digest_size(context->algorithm);
    if (required == 0u) return PIKA_STATUS_INVALID_OPERAND;
    if (capacity < required) return PIKA_STATUS_STORAGE_TOO_SMALL;
    switch (context->algorithm) {
        case PIKA_HASH_ALGORITHM_MD5:
            md5_finalize(context->state.md5, output);
            break;
        case PIKA_HASH_ALGORITHM_SHA1:
            sha1_finalize(context->state.sha1, output);
            break;
        case PIKA_HASH_ALGORITHM_SHA224:
            sha256_finalize(context->state.sha256, output, 7u);
            break;
        case PIKA_HASH_ALGORITHM_SHA256:
            sha256_finalize(context->state.sha256, output, 8u);
            break;
        case PIKA_HASH_ALGORITHM_SHA384:
            sha512_finalize(context->state.sha512, output, 6u);
            break;
        case PIKA_HASH_ALGORITHM_SHA512:
            sha512_finalize(context->state.sha512, output, 8u);
            break;
        default: return PIKA_STATUS_INVALID_OPERAND;
    }
    return PIKA_STATUS_OK;
}
