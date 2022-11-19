#ifndef __PIKA_ADAPTER_MPY_H__
#define __PIKA_ADAPTER_MPY_H__
#include <stdint.h>
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

/* object API */
#define MP_OBJ_NEW_SMALL_INT(...) arg_newInt(__VA_ARGS__)
#define mp_obj_new_bool(...) arg_newInt(__VA_ARGS__)
#define mp_obj_new_bytes(...) arg_newBytes(__VA_ARGS__)
#define mp_obj_new_float(...) arg_newFloat(__VA_ARGS__)
#define MP_OBJ_TO_PTR(...) arg_getPtr(__VA_ARGS__)
#define MP_OBJ_FROM_PTR(_p) arg_newPtr(ARG_TYPE_OBJECT, (_p))
#define mp_obj_get_int(...) arg_getInt(__VA_ARGS__)
#define mp_obj_is_true(...) (bool)arg_getInt(__VA_ARGS__)
#define mp_const_true arg_newInt(1)
#define mp_const_false arg_newInt(0)
#define mp_const_none arg_newNull()

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
#define MP_BUFFER_READ "read"
#define MP_ERROR_TEXT(_s) _s
#define mp_raise_msg_varg(_, ...) __platform_printf(__VA_ARGS__)
#define mp_raise_msg(_, _s) mp_raise_msg_varg(_, _s)
#define mp_raise_TpyeXXX(_s) mp_raise_msg(NULL, _s)
#define mp_raise_ValueError mp_raise_TpyeXXX
#define mp_raise_TypeError mp_raise_TpyeXXX
#define mp_raise_OSError mp_raise_TpyeXXX

/* utils */
#define MIN(_a, _b) ((_a) < (_b) ? (_a) : (_b))
#define MAX(_a, _b) ((_a) > (_b) ? (_a) : (_b))

/* list */
typedef struct mp_obj_tuple_t {
    size_t len;
    mp_obj_t* items;
} mp_obj_tuple_t;

#define mp_obj_str_get_str(...) arg_getStr(__VA_ARGS__)
#define mp_obj_get_float(...) arg_getFloat(__VA_ARGS__)

typedef struct _mp_map_elem_t {
    mp_obj_t key;
    mp_obj_t value;
} mp_map_elem_t;

typedef struct mp_map_t {
    size_t all_keys_are_qstrs : 1;
    size_t
        is_fixed : 1;  // if set, table is fixed/read-only and can't be modified
    size_t is_ordered : 1;  // if set, table is an ordered array, not a hash map
    size_t used : (8 * sizeof(size_t) - 3);
    size_t alloc;
    mp_map_elem_t* table;
} mp_map_t;

static inline bool mp_map_slot_is_filled(const mp_map_t* map, size_t pos) {
    return (map)->table[pos].key != NULL;
}

static inline int mp_obj_str_get_qstr(Arg* arg) {
    return hash_time33(arg_getStr(arg));
}

#define mp_obj_new_str(str, len) arg_newStr(str)

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

static inline mp_obj_tuple_t* mp_obj_new_tuple(int n, Arg** items_in) {
    mp_obj_tuple_t* tuple = (mp_obj_tuple_t*)malloc(sizeof(mp_obj_tuple_t));
    Arg** items = (Arg**)malloc(sizeof(Arg*) * n);
    if (NULL == items_in) {
        tuple->len = n;
        tuple->items = items;
    }
    return tuple;
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
        map->table[i].key = arg_getNameHash(item);
        map->table[i].value = item;
    }
}

static inline void mp_get_buffer_raise(mp_obj_t item,
                                       mp_buffer_info_t* buf,
                                       char* msg) {
    buf->len = arg_getSize(item);
    buf->buf = malloc(buf->len);
    if (NULL == buf->buf) {
        mp_raise_OSError(msg);
    }
    memcpy(buf->buf, arg_getBytes(item), buf->len);
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

#define mp_uint_t size_t
#define mp_int_t int

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

#define MICROPY_OBJ_BASE_ALIGNMENT
typedef struct _mp_obj_type_t mp_obj_type_t;
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
    // more code, and we also need to be able to take the address of them for
    // mp_obj_class_lookup.

    // Corresponds to __new__ and __init__ special methods, to make an instance
    // of the type.
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
    //          MP_OBJ_SENTINEL for attr, dest[0] = value for method, dest[0] =
    //          method, dest[1] = self
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

    // This slot's behaviour depends on the MP_TYPE_FLAG_ITER_IS_* flags above.
    // - If MP_TYPE_FLAG_ITER_IS_GETITER flag is set, then this corresponds to
    // the __iter__
    //   special method (of type mp_getiter_fun_t). Can use the given
    //   mp_obj_iter_buf_t to store the iterator object, otherwise can return a
    //   pointer to an object on the heap.
    // - If MP_TYPE_FLAG_ITER_IS_ITERNEXT is set, then this corresponds to
    // __next__ special method.
    //   May return MP_OBJ_STOP_ITERATION as an optimisation instead of raising
    //   StopIteration() with no args. The type will implicitly implement
    //   getiter as "return self".
    // - If MP_TYPE_FLAG_ITER_IS_CUSTOM is set, then this slot must point to an
    //   mp_getiter_iternext_custom_t instance with both the getiter and
    //   iternext fields set.
    // - If MP_TYPE_FLAG_ITER_IS_STREAM is set, this this slot should be unset.
    uint8_t slot_index_iter;

    // Implements the buffer protocol if supported by this type.
    uint8_t slot_index_buffer;

    // One of disjoint protocols (interfaces), like mp_stream_p_t, etc.
    uint8_t slot_index_protocol;

    // A pointer to the parents of this type:
    //  - 0 parents: pointer is NULL (object is implicitly the single parent)
    //  - 1 parent: a pointer to the type of that parent
    //  - 2 or more parents: pointer to a tuple object containing the parent
    //  types
    uint8_t slot_index_parent;

    // A dict mapping qstrs to objects local methods/constants/etc.
    uint8_t slot_index_locals_dict;

    const void* slots[];
};

typedef struct _mp_obj_dict_t {
    mp_obj_base_t base;
    mp_map_t map;
} mp_obj_dict_t;

typedef struct _mp_obj_module_t {
    mp_obj_base_t base;
    mp_obj_dict_t* globals;
} mp_obj_module_t;

typedef struct _vstr_t {
    size_t alloc;
    size_t len;
    char *buf;
    bool fixed_buf;
} vstr_t;

typedef struct _mp_obj_list_t {
    mp_obj_base_t base;
    size_t alloc;
    size_t len;
    mp_obj_t *items;
} mp_obj_list_t;


#endif
