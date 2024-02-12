/*
 * This file is part of the PikaPython project, http://pikapython.com
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2017 Paul Sokolovsky
 * Copyright (c) 2014-2019 Damien P. George
 * Copyright (c) 2023 Lyon
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef __PIKA_ADAPTER_MPY_H__
#define __PIKA_ADAPTER_MPY_H__
#include <stdint.h>
#include <stddef.h>
#include "PikaObj.h"
#include "PikaStdData_List.h"
#define bool int
#define true 1
#define false 0
/* object type */
#define mp_obj_t Arg*
/* type define*/
#define STATIC static
#define NORETURN
typedef unsigned char byte;
typedef unsigned int uint;
#define mp_uint_t size_t
#define mp_int_t int
typedef uint32_t unichar;
#define UTF8_IS_NONASCII(ch) ((ch)&0x80)
#define UTF8_IS_CONT(ch) (((ch)&0xC0) == 0x80)

/* object API */
#define MP_OBJ_NEW_SMALL_INT(...) arg_newInt(__VA_ARGS__)
#define mp_obj_new_bool(...) arg_newInt(__VA_ARGS__)
#define mp_obj_new_bytes(...) arg_newBytes(__VA_ARGS__)
#define mp_obj_new_float(...) arg_newFloat(__VA_ARGS__)
#define mp_obj_new_int(...) arg_newInt(__VA_ARGS__)
#define MP_OBJ_TO_PTR(...) arg_getPtr(__VA_ARGS__)
#define MP_OBJ_FROM_PTR(_p) arg_newPtr(ARG_TYPE_OBJECT, (_p))
#define mp_obj_get_int(...) arg_getInt(__VA_ARGS__)
#define mp_obj_is_true(...) (bool)arg_getInt(__VA_ARGS__)
#define mp_const_true arg_newInt(1)
#define mp_const_false arg_newInt(0)
#define mp_const_none arg_newNull()

#define mp_obj_new_int_from_ll mp_obj_new_int
#define mp_obj_new_int_from_ull mp_obj_new_int
#define mp_obj_new_float_from_f mp_obj_new_float
#define mp_obj_new_float_from_d mp_obj_new_float

#define MP_OBJ_SMALL_INT_VALUE(...) arg_getInt(__VA_ARGS__)

/* module API */
#define MP_DEFINE_CONST_DICT(...)
#define MP_DEFINE_CONST_FUN_OBJ_KW(...)
#define MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(...)
#define MP_DEFINE_CONST_FUN_OBJ_1(...)
#define MP_DEFINE_CONST_FUN_OBJ_2(...)
#define MP_DEFINE_CONST_FUN_OBJ_3(...)
#define MP_DEFINE_CONST_FUN_OBJ_4(...)
#define MP_DEFINE_CONST_FUN_OBJ_5(...)
#define MP_DEFINE_CONST_FUN_OBJ_6(...)
#define MP_DEFINE_CONST_FUN_OBJ_7(...)
#define MP_DEFINE_CONST_FUN_OBJ_8(...)

/* gil */
#define MP_THREAD_GIL_EXIT(...)
#define MP_THREAD_GIL_ENTER(...)

/* raise */
#define MP_ETIMEDOUT "timed out"
#define MP_ENOMEM "out of memory"
#define MP_ENDIANNESS_LITTLE (1)
#define MP_ENDIANNESS_BIG (0)
#define MICROPY_PY_BUILTINS_FLOAT (1)
#define MP_BYTES_PER_OBJ_WORD (sizeof(mp_uint_t))
#define MP_BUFFER_READ "read"
#define MP_ERROR_TEXT(_s) _s
#define mp_raise_msg_varg(_, ...) pika_platform_printf(__VA_ARGS__)
#define mp_raise_msg(_, _s) mp_raise_msg_varg(_, _s)
#define mp_raise_TpyeXXX(_s) mp_raise_msg(NULL, _s)
#define mp_raise_ValueError mp_raise_TpyeXXX
#define mp_raise_TypeError mp_raise_TpyeXXX
#define mp_raise_OSError mp_raise_TpyeXXX

/* utils */
#define MIN(_a, _b) ((_a) < (_b) ? (_a) : (_b))
#define MAX(_a, _b) ((_a) > (_b) ? (_a) : (_b))

typedef struct _mp_obj_type_t mp_obj_type_t;

#define MICROPY_OBJ_BASE_ALIGNMENT
struct _mp_obj_base_t {
    const mp_obj_type_t* type MICROPY_OBJ_BASE_ALIGNMENT;
};
typedef struct _mp_obj_base_t mp_obj_base_t;

struct _mp_obj_type_t {
    // A type is an object so must start with this entry, which points to
    // mp_type_type.
    mp_obj_base_t base;

    // Flags associated with this type.
    uint16_t flags;

    // The name of this type, a qstr.
    uint16_t name;

    // Slots: For the rest of the fields, the slot index points to the
    // relevant function in the variable-length "slots" field. Ideally these
    // would be only 4 bits, but the extra overhead of accessing them adds
    // more code, and we also need to be able to take the address of them
    // for mp_obj_class_lookup.

    // Corresponds to __new__ and __init__ special methods, to make an
    // instance of the type.
    uint8_t slot_index_make_new;

    // Corresponds to __repr__ and __str__ special methods.
    uint8_t slot_index_print;

    // Corresponds to __call__ special method, ie T(...).
    uint8_t slot_index_call;

    // Implements unary and binary operations.
    // Can return MP_OBJ_NULL if the operation is not supported.
    uint8_t slot_index_unary_op;
    uint8_t slot_index_binary_op;

    // Implements load, store and delete attribute.
    //
    // dest[0] = MP_OBJ_NULL means load
    //  return: for fail, do nothing
    //          for fail but continue lookup in locals_dict, dest[1] =
    //          MP_OBJ_SENTINEL for attr, dest[0] = value for method,
    //          dest[0] = method, dest[1] = self
    //
    // dest[0,1] = {MP_OBJ_SENTINEL, MP_OBJ_NULL} means delete
    // dest[0,1] = {MP_OBJ_SENTINEL, object} means store
    //  return: for fail, do nothing
    //          for success set dest[0] = MP_OBJ_NULL
    uint8_t slot_index_attr;

    // Implements load, store and delete subscripting:
    //  - value = MP_OBJ_SENTINEL means load
    //  - value = MP_OBJ_NULL means delete
    //  - all other values mean store the value
    // Can return MP_OBJ_NULL if operation not supported.
    uint8_t slot_index_subscr;

    // This slot's behaviour depends on the MP_TYPE_FLAG_ITER_IS_* flags
    // above.
    // - If MP_TYPE_FLAG_ITER_IS_GETITER flag is set, then this corresponds
    // to the __iter__
    //   special method (of type mp_getiter_fun_t). Can use the given
    //   mp_obj_iter_buf_t to store the iterator object, otherwise can
    //   return a pointer to an object on the heap.
    // - If MP_TYPE_FLAG_ITER_IS_ITERNEXT is set, then this corresponds to
    // __next__ special method.
    //   May return MP_OBJ_STOP_ITERATION as an optimisation instead of
    //   raising StopIteration() with no args. The type will implicitly
    //   implement getiter as "return self".
    // - If MP_TYPE_FLAG_ITER_IS_CUSTOM is set, then this slot must point to
    // an
    //   mp_getiter_iternext_custom_t instance with both the getiter and
    //   iternext fields set.
    // - If MP_TYPE_FLAG_ITER_IS_STREAM is set, this this slot should be
    // unset.
    uint8_t slot_index_iter;

    // Implements the buffer protocol if supported by this type.
    uint8_t slot_index_buffer;

    // One of disjoint protocols (interfaces), like mp_stream_p_t, etc.
    uint8_t slot_index_protocol;

    // A pointer to the parents of this type:
    //  - 0 parents: pointer is NULL (object is implicitly the single
    //  parent)
    //  - 1 parent: a pointer to the type of that parent
    //  - 2 or more parents: pointer to a tuple object containing the parent
    //  types
    uint8_t slot_index_parent;

    // A dict mapping qstrs to objects local methods/constants/etc.
    uint8_t slot_index_locals_dict;

    const void* slots[];
};

typedef struct _mp_map_elem_t {
    mp_obj_t key;
    mp_obj_t value;
} mp_map_elem_t;

typedef struct mp_map_t {
    size_t all_keys_are_qstrs : 1;
    size_t is_fixed : 1;    // if set, table is fixed/read-only and can't be
                            // modified
    size_t is_ordered : 1;  // if set, table is an ordered array, not a hash
                            // map
    size_t used : (8 * sizeof(size_t) - 3);
    size_t alloc;
    mp_map_elem_t* table;
} mp_map_t;

/* tuple */
typedef struct mp_obj_tuple_t {
    size_t len;
    mp_obj_t* items;
} mp_obj_tuple_t;

/* list */
typedef struct _mp_obj_list_t {
    mp_obj_base_t base;
    size_t alloc;
    size_t len;
    mp_obj_t* items;
} mp_obj_list_t;

typedef struct _mp_obj_dict_t {
    mp_obj_base_t base;
    mp_map_t map;
} mp_obj_dict_t;

#define mp_obj_str_get_str(...) arg_getStr(__VA_ARGS__)
#define mp_obj_get_float(...) arg_getFloat(__VA_ARGS__)

static inline float mp_obj_get_float_to_f(mp_obj_t o) {
    return mp_obj_get_float(o);
}

static inline double mp_obj_get_float_to_d(mp_obj_t o) {
    return (double)mp_obj_get_float(o);
}

static inline bool mp_map_slot_is_filled(const mp_map_t* map, size_t pos) {
    return (map)->table[pos].key != NULL;
}

static inline int mp_obj_str_get_qstr(Arg* arg) {
    return hash_time33(arg_getStr(arg));
}

static inline Arg* mp_obj_new_str(const char* str, size_t len) {
    return arg_newStrN((char*)str, len);
}

typedef struct _mp_buffer_info_t {
    void* buf;     // can be NULL if len == 0
    size_t len;    // in bytes
    int typecode;  // as per binary.h
} mp_buffer_info_t;

#define MP_QSTR(_str) hash_time33(#_str)

static inline Arg* mp_obj_new_list(int n, Arg** items) {
    PikaObj* list = newNormalObj(New_PikaStdData_List);
    PikaStdData_List___init__(list);
    return arg_newObj(list);
}

static inline mp_obj_t mp_obj_new_tuple(int n, Arg** items_in) {
    mp_obj_tuple_t* tuple = (mp_obj_tuple_t*)pikaMalloc(sizeof(mp_obj_tuple_t));
    Arg** items = (Arg**)pikaMalloc(sizeof(Arg*) * n);
    if (NULL == items_in) {
        tuple->len = n;
        tuple->items = items;
    }
    return arg_newPtr(ARG_TYPE_POINTER, tuple);
}

static inline void mp_obj_tuple_deinit(mp_obj_t tuple) {
    mp_obj_tuple_t* tuple_obj = (mp_obj_tuple_t*)arg_getPtr(tuple);
    pikaFree(tuple_obj->items, sizeof(Arg*) * tuple_obj->len);
    pikaFree(tuple_obj, sizeof(mp_obj_tuple_t));
    arg_deinit(tuple);
}

static inline void mp_obj_list_append(Arg* list, mp_obj_tuple_t* tuple) {
    PikaObj* list_obj = (PikaObj*)arg_getPtr(list);
    for (int i = 0; i < tuple->len; i++) {
        PikaStdData_List_append(list_obj, tuple->items[i]);
        arg_deinit(tuple->items[i]);
    }
    free(tuple->items);
    free(tuple);
}

static inline char* mp_obj_str_get_data(Arg* self, size_t* len) {
    char* str = arg_getStr(self);
    *len = strGetSize(str);
    return str;
}

static inline mp_obj_t MP_OBJ_FROM_TUPLE(mp_obj_tuple_t* tuple) {
    PikaObj* oTuple = New_PikaTuple();
    for (int i = 0; i < tuple->len; i++) {
        pikaList_append(oTuple, tuple->items[i]);
    }
    return arg_newObj(oTuple);
}

static inline size_t pks_load_mp_args(PikaTuple* tuple,
                                      mp_obj_t mp_self,
                                      mp_obj_t* args) {
    size_t len = pikaTuple_getSize(tuple);
    size_t i = 0;
    if (NULL != mp_self) {
        args[0] = mp_self;
        i = 1;
    }
    for (i = 0; i < len; i++) {
        args[i] = pikaTuple_getArg(tuple, i);
    }
    return len;
}

static inline void pks_load_mp_map(PikaDict* kw, mp_map_t* map) {
    size_t len = pikaDict_getSize(kw);
    map->alloc = len;
    map->used = len;
    map->table = (mp_map_elem_t*)malloc(sizeof(mp_map_elem_t) * len);
    for (int i = 0; i < len; i++) {
        Arg* item = pikaDict_getArgByidex(kw, i);
        map->table[i].key = arg_newInt(arg_getNameHash(item));
        map->table[i].value = item;
    }
}

static inline void mp_get_buffer_raise(const mp_obj_t item,
                                       mp_buffer_info_t* buf,
                                       char* msg) {
    buf->len = arg_getSize((Arg*)item);
    buf->buf = pikaMalloc(buf->len);
    pika_platform_memset(buf->buf, 0, buf->len);
    if (NULL == buf->buf) {
        mp_raise_OSError(msg);
    }
    pika_platform_memcpy(buf->buf, arg_getBytes((Arg*)item),
                         arg_getBytesSize((Arg*)item));
}

static inline void mp_buff_info_deinit(mp_buffer_info_t* buf) {
    pikaFree(buf->buf, buf->len);
}

static const ArgType mp_type_tuple = ARG_TYPE_TUPLE;
static const ArgType mp_type_list = ARG_TYPE_TUPLE;
static const ArgType mp_type_str = ARG_TYPE_STRING;
static const ArgType mp_type_bytes = ARG_TYPE_BYTES;
static const ArgType mp_type_int = ARG_TYPE_INT;
static const ArgType mp_type_float = ARG_TYPE_FLOAT;
static const ArgType mp_type_bool = ARG_TYPE_INT;
static const ArgType mp_type_none = ARG_TYPE_NONE;

static inline bool mp_obj_is_type(mp_obj_t self, ArgType* arg_type_ptr) {
    if (arg_getType(self) == *arg_type_ptr) {
        return true;
    }
    return false;
}

#define mp_obj_is_integer(self) mp_obj_is_type(self, &mp_type_int)

typedef void (*mp_print_strn_t)(void* data, const char* str, size_t len);

typedef struct _mp_print_t {
    void* data;
    mp_print_strn_t print_strn;
} mp_print_t;

static inline void mp_obj_get_array_fixed_n(mp_obj_t tuple,
                                            size_t n,
                                            mp_obj_t* arrray) {
    for (int i = 0; i < n; i++) {
        arrray[i] = pikaTuple_getArg((PikaTuple*)arg_getPtr(tuple), i);
    }
}

typedef const void* mp_const_obj_t;
typedef mp_const_obj_t mp_rom_obj_t;
typedef struct _mp_rom_map_elem_t {
    mp_rom_obj_t key;
    mp_rom_obj_t value;
} mp_rom_map_elem_t;

#define MP_ALIGN(ptr, alignment) \
    (void*)(((uintptr_t)(ptr) + ((alignment)-1)) & ~((alignment)-1))
#define MP_ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

typedef struct _mp_obj_module_t {
    mp_obj_base_t base;
    mp_obj_dict_t* globals;
} mp_obj_module_t;

typedef struct _vstr_t {
    size_t alloc;
    size_t len;
    char* buf;
    bool fixed_buf;
} vstr_t;

// attribute flags
#define FL_PRINT (0x01)
#define FL_SPACE (0x02)
#define FL_DIGIT (0x04)
#define FL_ALPHA (0x08)
#define FL_UPPER (0x10)
#define FL_LOWER (0x20)
#define FL_XDIGIT (0x40)

// shorthand character attributes
#define AT_PR (FL_PRINT)
#define AT_SP (FL_SPACE | FL_PRINT)
#define AT_DI (FL_DIGIT | FL_PRINT | FL_XDIGIT)
#define AT_AL (FL_ALPHA | FL_PRINT)
#define AT_UP (FL_UPPER | FL_ALPHA | FL_PRINT)
#define AT_LO (FL_LOWER | FL_ALPHA | FL_PRINT)
#define AT_UX (FL_UPPER | FL_ALPHA | FL_PRINT | FL_XDIGIT)
#define AT_LX (FL_LOWER | FL_ALPHA | FL_PRINT | FL_XDIGIT)

// table of attributes for ascii characters
STATIC const uint8_t attr[] = {
    0,     0,     0,     0,     0,     0,     0,     0,     0,     AT_SP, AT_SP,
    AT_SP, AT_SP, AT_SP, 0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     AT_SP,
    AT_PR, AT_PR, AT_PR, AT_PR, AT_PR, AT_PR, AT_PR, AT_PR, AT_PR, AT_PR, AT_PR,
    AT_PR, AT_PR, AT_PR, AT_PR, AT_DI, AT_DI, AT_DI, AT_DI, AT_DI, AT_DI, AT_DI,
    AT_DI, AT_DI, AT_DI, AT_PR, AT_PR, AT_PR, AT_PR, AT_PR, AT_PR, AT_PR, AT_UX,
    AT_UX, AT_UX, AT_UX, AT_UX, AT_UX, AT_UP, AT_UP, AT_UP, AT_UP, AT_UP, AT_UP,
    AT_UP, AT_UP, AT_UP, AT_UP, AT_UP, AT_UP, AT_UP, AT_UP, AT_UP, AT_UP, AT_UP,
    AT_UP, AT_UP, AT_UP, AT_PR, AT_PR, AT_PR, AT_PR, AT_PR, AT_PR, AT_LX, AT_LX,
    AT_LX, AT_LX, AT_LX, AT_LX, AT_LO, AT_LO, AT_LO, AT_LO, AT_LO, AT_LO, AT_LO,
    AT_LO, AT_LO, AT_LO, AT_LO, AT_LO, AT_LO, AT_LO, AT_LO, AT_LO, AT_LO, AT_LO,
    AT_LO, AT_LO, AT_PR, AT_PR, AT_PR, AT_PR, 0};

static unichar utf8_get_char(const byte* s) {
    unichar ord = *s++;
    if (!UTF8_IS_NONASCII(ord)) {
        return ord;
    }
    ord &= 0x7F;
    for (unichar mask = 0x40; ord & mask; mask >>= 1) {
        ord &= ~mask;
    }
    while (UTF8_IS_CONT(*s)) {
        ord = (ord << 6) | (*s++ & 0x3F);
    }
    return ord;
}

static const byte* utf8_next_char(const byte* s) {
    ++s;
    while (UTF8_IS_CONT(*s)) {
        ++s;
    }
    return s;
}

static mp_uint_t utf8_ptr_to_index(const byte* s, const byte* ptr) {
    mp_uint_t i = 0;
    while (ptr > s) {
        if (!UTF8_IS_CONT(*--ptr)) {
            i++;
        }
    }

    return i;
}

static size_t utf8_charlen(const byte* str, size_t len) {
    size_t charlen = 0;
    for (const byte* top = str + len; str < top; ++str) {
        if (!UTF8_IS_CONT(*str)) {
            ++charlen;
        }
    }
    return charlen;
}

// Be aware: These unichar_is* functions are actually ASCII-only!
static bool unichar_isspace(unichar c) {
    return c < 128 && (attr[c] & FL_SPACE) != 0;
}

static bool unichar_isalpha(unichar c) {
    return c < 128 && (attr[c] & FL_ALPHA) != 0;
}

/* unused
bool unichar_isprint(unichar c) {
    return c < 128 && (attr[c] & FL_PRINT) != 0;
}
*/

static bool unichar_isdigit(unichar c) {
    return c < 128 && (attr[c] & FL_DIGIT) != 0;
}

static bool unichar_isxdigit(unichar c) {
    return c < 128 && (attr[c] & FL_XDIGIT) != 0;
}

static bool unichar_isident(unichar c) {
    return c < 128 && ((attr[c] & (FL_ALPHA | FL_DIGIT)) != 0 || c == '_');
}

static bool unichar_isalnum(unichar c) {
    return c < 128 && ((attr[c] & (FL_ALPHA | FL_DIGIT)) != 0);
}

static bool unichar_isupper(unichar c) {
    return c < 128 && (attr[c] & FL_UPPER) != 0;
}

static bool unichar_islower(unichar c) {
    return c < 128 && (attr[c] & FL_LOWER) != 0;
}

static unichar unichar_tolower(unichar c) {
    if (unichar_isupper(c)) {
        return c + 0x20;
    }
    return c;
}

static unichar unichar_toupper(unichar c) {
    if (unichar_islower(c)) {
        return c - 0x20;
    }
    return c;
}

static mp_uint_t unichar_xdigit_value(unichar c) {
    // c is assumed to be hex digit
    mp_uint_t n = c - '0';
    if (n > 9) {
        n &= ~('a' - 'A');
        n -= ('A' - ('9' + 1));
    }
    return n;
}

static bool utf8_check(const byte* p, size_t len) {
    uint8_t need = 0;
    const byte* end = p + len;
    for (; p < end; p++) {
        byte c = *p;
        if (need) {
            if (UTF8_IS_CONT(c)) {
                need--;
            } else {
                // mismatch
                return 0;
            }
        } else {
            if (c >= 0xc0) {
                if (c >= 0xf8) {
                    // mismatch
                    return 0;
                }
                need = (0xe5 >> ((c >> 3) & 0x6)) & 3;
            } else if (c >= 0x80) {
                // mismatch
                return 0;
            }
        }
    }
    return need == 0;  // no pending fragments allowed
}

#ifndef alignof
#define alignof(type) \
    offsetof(         \
        struct {      \
            char c;   \
            type t;   \
        },            \
        t)
#endif

#define BYTEARRAY_TYPECODE 1

static size_t mp_binary_get_size(char struct_type,
                                 char val_type,
                                 size_t* palign) {
    size_t size = 0;
    int align = 1;
    switch (struct_type) {
        case '<':
        case '>':
            switch (val_type) {
                case 'b':
                case 'B':
                    size = 1;
                    break;
                case 'h':
                case 'H':
                    size = 2;
                    break;
                case 'i':
                case 'I':
                    size = 4;
                    break;
                case 'l':
                case 'L':
                    size = 4;
                    break;
                case 'q':
                case 'Q':
                    size = 8;
                    break;
                case 'P':
                case 'O':
                case 'S':
                    size = sizeof(void*);
                    break;
                case 'f':
                    size = sizeof(float);
                    break;
                case 'd':
                    size = sizeof(double);
                    break;
            }
            break;
        case '@': {
            // TODO:
            // The simplest heuristic for alignment is to align by value
            // size, but that doesn't work for "bigger than int" types,
            // for example, long long may very well have long alignment
            // So, we introduce separate alignment handling, but having
            // formal support for that is different from actually supporting
            // particular (or any) ABI.
            switch (val_type) {
                case BYTEARRAY_TYPECODE:
                case 'b':
                case 'B':
                    align = size = 1;
                    break;
                case 'h':
                case 'H':
                    align = alignof(short);
                    size = sizeof(short);
                    break;
                case 'i':
                case 'I':
                    align = alignof(int);
                    size = sizeof(int);
                    break;
                case 'l':
                case 'L':
                    align = alignof(long);
                    size = sizeof(long);
                    break;
                case 'q':
                case 'Q':
                    align = alignof(long long);
                    size = sizeof(long long);
                    break;
                case 'P':
                case 'O':
                case 'S':
                    align = alignof(void*);
                    size = sizeof(void*);
                    break;
                case 'f':
                    align = alignof(float);
                    size = sizeof(float);
                    break;
                case 'd':
                    align = alignof(double);
                    size = sizeof(double);
                    break;
            }
        }
    }

    if (size == 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("bad typecode"));
    }

    if (palign != NULL) {
        *palign = align;
    }
    return size;
}

// The long long type is guaranteed to hold at least 64 bits, and size is at
// most 8 (for q and Q), so we will always be able to parse the given data
// and fit it into a long long.
static inline long long mp_binary_get_int(size_t size,
                                          bool is_signed,
                                          bool big_endian,
                                          const byte* src) {
    int delta;
    if (!big_endian) {
        delta = -1;
        src += size - 1;
    } else {
        delta = 1;
    }

    unsigned long long val = 0;
    if (is_signed && *src & 0x80) {
        val = -1;
    }
    for (uint32_t i = 0; i < size; i++) {
        val <<= 8;
        val |= *src;
        src += delta;
    }

    return val;
}

#define is_signed(typecode) (typecode > 'Z')
static mp_obj_t mp_binary_get_val(char struct_type,
                                  char val_type,
                                  byte* p_base,
                                  byte** ptr) {
    byte* p = *ptr;
    size_t align;

    size_t size = mp_binary_get_size(struct_type, val_type, &align);
    if (struct_type == '@') {
        // Align p relative to p_base
        p = p_base + (uintptr_t)MP_ALIGN(p - p_base, align);
#if MP_ENDIANNESS_LITTLE
        struct_type = '<';
#else
        struct_type = '>';
#endif
    }
    *ptr = p + size;

    long long val =
        mp_binary_get_int(size, is_signed(val_type), (struct_type == '>'), p);

    if (val_type == 'O') {
        return (mp_obj_t)(mp_uint_t)val;
    } else if (val_type == 'S') {
        const char* s_val = (const char*)(uintptr_t)(mp_uint_t)val;
        return mp_obj_new_str(s_val, strlen(s_val));
    } else if (val_type == 'f') {
        union {
            uint32_t i;
            float f;
        } fpu = {val};
        return mp_obj_new_float_from_f(fpu.f);
    } else if (val_type == 'd') {
        union {
            uint64_t i;
            double f;
        } fpu = {val};
        return mp_obj_new_float_from_d(fpu.f);
    } else if (is_signed(val_type)) {
        return mp_obj_new_int_from_ll(val);
    } else {
        return mp_obj_new_int_from_ull(val);
    }
}

void mp_binary_set_int(size_t val_sz,
                       bool big_endian,
                       byte* dest,
                       mp_uint_t val) {
    if (MP_ENDIANNESS_LITTLE && !big_endian) {
        memcpy(dest, &val, val_sz);
    } else if (MP_ENDIANNESS_BIG && big_endian) {
        // only copy the least-significant val_sz bytes
        memcpy(dest, (byte*)&val + sizeof(mp_uint_t) - val_sz, val_sz);
    } else {
        const byte* src;
        if (MP_ENDIANNESS_LITTLE) {
            src = (const byte*)&val + val_sz;
        } else {
            src = (const byte*)&val + sizeof(mp_uint_t);
        }
        while (val_sz--) {
            *dest++ = *--src;
        }
    }
}

static void mp_binary_set_val(char struct_type,
                              char val_type,
                              mp_obj_t val_in,
                              byte* p_base,
                              byte** ptr) {
    byte* p = *ptr;
    size_t align;

    size_t size = mp_binary_get_size(struct_type, val_type, &align);
    if (struct_type == '@') {
        // Align p relative to p_base
        p = p_base + (uintptr_t)MP_ALIGN(p - p_base, align);
        if (MP_ENDIANNESS_LITTLE) {
            struct_type = '<';
        } else {
            struct_type = '>';
        }
    }
    *ptr = p + size;

    mp_uint_t val;
    switch (val_type) {
        case 'O':
            val = (mp_uint_t)val_in;
            break;
#if MICROPY_PY_BUILTINS_FLOAT
        case 'f': {
            union {
                uint32_t i;
                float f;
            } fp_sp;
            fp_sp.f = mp_obj_get_float_to_f(val_in);
            val = fp_sp.i;
            break;
        }
        case 'd': {
            union {
                uint64_t i64;
                uint32_t i32[2];
                double f;
            } fp_dp;
            fp_dp.f = mp_obj_get_float_to_d(val_in);
            if (MP_BYTES_PER_OBJ_WORD == 8) {
                val = fp_dp.i64;
            } else {
                int be = struct_type == '>';
                mp_binary_set_int(sizeof(uint32_t), be, p,
                                  fp_dp.i32[MP_ENDIANNESS_BIG ^ be]);
                p += sizeof(uint32_t);
                val = fp_dp.i32[MP_ENDIANNESS_LITTLE ^ be];
            }
            break;
        }
#endif
        default:
#if MICROPY_LONGINT_IMPL != MICROPY_LONGINT_IMPL_NONE
            if (mp_obj_is_exact_type(val_in, &mp_type_int)) {
                mp_obj_int_to_bytes_impl(val_in, struct_type == '>', size, p);
                return;
            }
#endif

            val = mp_obj_get_int(val_in);
            // zero/sign extend if needed
            if (MP_BYTES_PER_OBJ_WORD < 8 && size > sizeof(val)) {
                int c = (mp_int_t)val < 0 ? 0xff : 0x00;
                memset(p, c, size);
                if (struct_type == '>') {
                    p += size - sizeof(val);
                }
            }
            break;
    }

    mp_binary_set_int(MIN((size_t)size, sizeof(val)), struct_type == '>', p,
                      val);
}

#endif
#ifdef __cplusplus
}
#endif
