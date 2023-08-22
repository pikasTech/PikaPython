/*
 * This file is part of the PikaPython project, https://pikapython.com
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2014 Paul Sokolovsky
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
#include "_struct.h"
#include "pika_adapter_mpy.h"

#define _SKIP_COMPILE 1

/*
    This module implements most of character typecodes from CPython, with
    some extensions:

    O - (Pointer to) an arbitrary Python object. This is useful for callback
        data, etc. Note that you must keep reference to passed object in
        your Python application, otherwise it may be garbage-collected,
        and then when you get back this value from callback it may be
        invalid (and lead to crash).
    S - Pointer to a string (returned as a Python string). Note the
        difference from "Ns", - the latter says "in this place of structure
        is character data of up to N bytes length", while "S" means
        "in this place of a structure is a pointer to zero-terminated
        character data".
 */

STATIC char get_fmt_type(const char** fmt) {
    char t = **fmt;
    switch (t) {
        case '!':
            t = '>';
            break;
        case '@':
        case '=':
        case '<':
        case '>':
            break;
        default:
            return '@';
    }
    // Skip type char
    (*fmt)++;
    return t;
}

STATIC mp_uint_t get_fmt_num(const char** p) {
    const char* num = *p;
    uint len = 1;
    while (unichar_isdigit(*++num)) {
        len++;
    }
    char buff[11] = {0};
    pika_platform_memcpy(buff, *p, len);
    mp_uint_t val = (mp_uint_t)fast_atoi((char*)buff);
    *p = num;
    return val;
}

STATIC size_t calc_size_items(const char* fmt, size_t* total_sz) {
    char fmt_type = get_fmt_type(&fmt);
    size_t total_cnt = 0;
    size_t size;
    for (size = 0; *fmt; fmt++) {
        mp_uint_t cnt = 1;
        if (unichar_isdigit(*fmt)) {
            cnt = get_fmt_num(&fmt);
        }

        if (*fmt == 's') {
            total_cnt += 1;
            size += cnt;
        } else {
            total_cnt += cnt;
            size_t align;
            size_t sz = mp_binary_get_size(fmt_type, *fmt, &align);
            while (cnt--) {
                // Apply alignment
                size = (size + align - 1) & ~(align - 1);
                size += sz;
            }
        }
    }
    *total_sz = size;
    return total_cnt;
}

STATIC mp_obj_t struct_calcsize(mp_obj_t fmt_in) {
    const char* fmt = mp_obj_str_get_str(fmt_in);
    size_t size;
    calc_size_items(fmt, &size);
    return MP_OBJ_NEW_SMALL_INT(size);
}

STATIC mp_obj_t struct_unpack_from(size_t n_args, mp_obj_t* args) {
    // unpack requires that the buffer be exactly the right size.
    // unpack_from requires that the buffer be "big enough".
    // Since we implement unpack and unpack_from using the same function
    // we relax the "exact" requirement, and only implement "big enough".
    const char* fmt = mp_obj_str_get_str((Arg*)args[0]);
    size_t total_sz;
    size_t num_items = calc_size_items(fmt, &total_sz);
    char fmt_type = get_fmt_type(&fmt);
    mp_obj_t tuple = mp_obj_new_tuple(num_items, NULL);
    mp_obj_tuple_t* res = MP_OBJ_TO_PTR(tuple);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise((Arg*)args[1], &bufinfo, MP_BUFFER_READ);
    byte* p = bufinfo.buf;
    byte* end_p = &p[bufinfo.len];
    mp_int_t offset = 0;

    if (n_args > 2) {
        // offset arg provided
        offset = mp_obj_get_int((Arg*)args[2]);
        if (offset < 0) {
            // negative offsets are relative to the end of the buffer
            offset = bufinfo.len + offset;
            if (offset < 0) {
                mp_raise_ValueError(MP_ERROR_TEXT("buffer too small"));
            }
        }
        p += offset;
    }
    byte* p_base = p;

    // Check that the input buffer is big enough to unpack all the values
    if (p + total_sz > end_p) {
        mp_raise_ValueError(MP_ERROR_TEXT("buffer too small"));
    }

    for (size_t i = 0; i < num_items;) {
        mp_uint_t cnt = 1;
        if (unichar_isdigit(*fmt)) {
            cnt = get_fmt_num(&fmt);
        }
        mp_obj_t item;
        if (*fmt == 's') {
            item = mp_obj_new_bytes(p, cnt);
            p += cnt;
            res->items[i++] = item;
        } else {
            while (cnt--) {
                item = mp_binary_get_val(fmt_type, *fmt, p_base, &p);
                res->items[i++] = item;
            }
        }
        fmt++;
    }
    mp_obj_t ret = MP_OBJ_FROM_TUPLE(res);
    mp_buff_info_deinit(&bufinfo);
    mp_obj_tuple_deinit(tuple);
    return ret;
}

Arg* _struct_unpack(PikaObj* self, char* fmt, Arg* data, int offset) {
    Arg* aFmt = arg_newStr(fmt);
    Arg* aOffset = arg_newInt(offset);
    Arg** args = pikaMalloc(sizeof(Arg) * 3);
    args[0] = aFmt;
    args[1] = data;
    args[2] = aOffset;
    Arg* tuple = struct_unpack_from(3, args);
    arg_deinit(aFmt);
    arg_deinit(aOffset);
    pikaFree(args, sizeof(Arg) * 3);
    return tuple;
}

// This function assumes there is enough room in p to store all the values
STATIC void struct_pack_into_internal(mp_obj_t fmt_in,
                                      byte* p,
                                      size_t n_args,
                                      mp_obj_t* args) {
    const char* fmt = mp_obj_str_get_str(fmt_in);
    char fmt_type = get_fmt_type(&fmt);

    byte* p_base = p;
    size_t i;
    for (i = 0; i < n_args;) {
        mp_uint_t cnt = 1;
        if (*fmt == '\0') {
            // more arguments given than used by format string; CPython raises
            // struct.error here
            break;
        }
        if (unichar_isdigit(*fmt)) {
            cnt = get_fmt_num(&fmt);
        }

        if (*fmt == 's') {
            mp_buffer_info_t bufinfo;
            mp_get_buffer_raise(args[i++], &bufinfo, MP_BUFFER_READ);
            mp_uint_t to_copy = cnt;
            if (bufinfo.len < to_copy) {
                to_copy = bufinfo.len;
            }
            pika_platform_memcpy(p, bufinfo.buf, to_copy);
            pika_platform_memset(p + to_copy, 0, cnt - to_copy);
            p += cnt;
            mp_buff_info_deinit(&bufinfo);
        } else {
            // If we run out of args then we just finish; CPython would raise
            // struct.error
            while (cnt-- && i < n_args) {
                mp_binary_set_val(fmt_type, *fmt, args[i++], p_base, &p);
            }
        }
        fmt++;
    }
}

Arg* _struct_pack(PikaObj* self, char* fmt, PikaTuple* vars) {
    Arg* aFmt = arg_newStr(fmt);
    size_t varlen = pikaTuple_getSize(vars);
    Arg* aSize = struct_calcsize(aFmt);
    Arg** args = pikaMalloc(sizeof(Arg) * varlen);
    for (size_t i = 0; i < varlen; i++) {
        args[i] = pikaTuple_get(vars, i);
    }
    size_t size = arg_getInt(aSize);
    Arg* byte = arg_newBytes(NULL, size);
    uint8_t* p = arg_getBytes(byte);
    struct_pack_into_internal(aFmt, p, varlen, args);
    pikaFree(args, sizeof(Arg) * varlen);
    arg_deinit(aFmt);
    arg_deinit(aSize);
    return byte;
}

#ifndef _SKIP_COMPILE

STATIC mp_obj_t struct_pack_into(size_t n_args, const mp_obj_t* args) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_WRITE);
    mp_int_t offset = mp_obj_get_int(args[2]);
    if (offset < 0) {
        // negative offsets are relative to the end of the buffer
        offset = (mp_int_t)bufinfo.len + offset;
        if (offset < 0) {
            mp_raise_ValueError(MP_ERROR_TEXT("buffer too small"));
        }
    }
    byte* p = (byte*)bufinfo.buf;
    byte* end_p = &p[bufinfo.len];
    p += offset;

    // Check that the output buffer is big enough to hold all the values
    mp_int_t sz = MP_OBJ_SMALL_INT_VALUE(struct_calcsize(args[0]));
    if (p + sz > end_p) {
        mp_raise_ValueError(MP_ERROR_TEXT("buffer too small"));
    }

    struct_pack_into_internal(args[0], p, n_args - 3, &args[3]);
    return mp_const_none;
}

#endif
