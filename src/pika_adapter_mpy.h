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
    size_t len = tuple_getSize(tuple);
    size_t i = 0;
    if (NULL != mp_self) {
        args[0] = mp_self;
        i = 1;
    }
    for (i = 0; i < len; i++) {
        args[i] = tuple_getArg(tuple, i);
    }
    return len;
}

static inline void pks_load_mp_map(PikaDict* kw, mp_map_t* map) {
    size_t len = dict_getSize(kw);
    map->alloc = len;
    map->used = len;
    map->table = (mp_map_elem_t*)malloc(sizeof(mp_map_elem_t) * len);
    for (int i = 0; i < len; i++) {
        Arg* item = dict_getArgByidex(kw, i);
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
        arrray[i] = tuple_getArg((PikaTuple*)arg_getPtr(tuple), i);
    }
}

#endif
