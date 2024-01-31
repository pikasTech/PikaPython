/**
 * @file    _modbus_rt.c
 * @brief   modbus_rt的pika层的API实现
 * @author  SenySunny (senysunny@163.com)
 * @date    2023-05-14
 *
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 Ma Longwei.
 * All rights reserved.</center></h2>
 *
 */
#include "PikaStdData_List.h"
#include "_modbus_rt__data_trans.h"

#include "modbus_rt_platform_memory.h"
#include "modbus_data_trans.h"

int _modbus_rt__data_trans__reg2reg(PikaObj *self, int val) {
#if  (!MODBUS_DATA_TRANS_ENABLE)
    pika_platform_printf("MODBUS_DATA_TRANS_ENABLE is not enabled.\n");
    return 0;
#else
    return modbus_data_reg2reg((uint16_t)val);
#endif
}

PikaObj* _modbus_rt__data_trans__regs2regs(PikaObj *self, PikaObj* val) {
#if  (!MODBUS_DATA_TRANS_ENABLE)
    pika_platform_printf("MODBUS_DATA_TRANS_ENABLE is not enabled.\n");
    return NULL;
#else
    (void)(self);
    size_t len_list = pikaList_getSize(val);
    PikaList* list = New_PikaList();
    for(size_t i = 0; i < len_list; i++ ) {
        uint16_t temp = (uint16_t)pikaList_getInt(val, i);
        pikaList_append(list, arg_newInt(modbus_data_reg2reg(temp)));
    }
    return list;
#endif
}

Arg* _modbus_rt__data_trans__regs2bytes(PikaObj *self, PikaObj* val, int mode) {
#if  (!MODBUS_DATA_TRANS_ENABLE)
    pika_platform_printf("MODBUS_DATA_TRANS_ENABLE is not enabled.\n");
    return NULL;
#else
    (void)(self);
    uint8_t *_Dst = NULL;
    uint16_t *_Src = NULL;
    size_t len_list = pikaList_getSize(val);
    size_t len = len_list << 1;
    _Dst = modbus_rt_calloc(1,len);
    _Src = modbus_rt_calloc(1,len);
    for(size_t i = 0;i < len_list; i++) {
        _Src[i] =  (uint16_t)pikaList_getInt(val, i);
    }
    modbus_data_regs2bytes(mode, _Dst, _Src, len_list);
    Arg* r = arg_newBytes(_Dst, len);
    modbus_rt_free(_Dst);
    modbus_rt_free(_Src);
    return r;
#endif
}

char* _modbus_rt__data_trans__regs2str(PikaObj *self, PikaObj* val, int mode) {
#if  (!MODBUS_DATA_TRANS_ENABLE)
    pika_platform_printf("MODBUS_DATA_TRANS_ENABLE is not enabled.\n");
    return NULL;
#else
    (void)(self);
    char *_Dst = NULL;
    uint16_t *_Src = NULL;
    size_t len_list = pikaList_getSize(val);
    _Dst = modbus_rt_calloc(1,(len_list << 1) + 1);
    _Src = modbus_rt_calloc(1,len_list << 1);
    for(size_t i = 0;i < len_list; i++) {
        _Src[i] =  (uint16_t)pikaList_getInt(val, i);
    }
    modbus_data_regs2str(mode, _Dst, _Src, len_list);
    char* str = obj_cacheStr(self, _Dst);
    modbus_rt_free(_Dst);
    modbus_rt_free(_Src);
    return str;
#endif
}

PikaObj* _modbus_rt__data_trans__regs2signed(PikaObj *self, PikaObj* val, int mode){
#if  (!MODBUS_DATA_TRANS_ENABLE)
    pika_platform_printf("MODBUS_DATA_TRANS_ENABLE is not enabled.\n");
    return NULL;
#else
    (void)(self);
    int16_t *_Dst = NULL;
    uint16_t *_Src = NULL;
    size_t len_list = pikaList_getSize(val);
    _Dst = modbus_rt_calloc(1,len_list << 1);
    _Src = modbus_rt_calloc(1,len_list << 1);
    for(size_t i = 0;i < len_list; i++) {
        _Src[i] =  (uint16_t)pikaList_getInt(val, i);
    }
    modbus_data_regs2signed(mode, _Dst, _Src, len_list);
    PikaList* list = New_PikaList();
    for(size_t i = 0; i < len_list; i++ ) {
        pikaList_append(list, arg_newInt(_Dst[i]));
    }
    modbus_rt_free(_Dst);
    modbus_rt_free(_Src);
    return list;   
#endif
}

PikaObj* _modbus_rt__data_trans__regs2int(PikaObj *self, PikaObj* val, int mode) {
#if  (!MODBUS_DATA_TRANS_ENABLE)
    pika_platform_printf("MODBUS_DATA_TRANS_ENABLE is not enabled.\n");
    return NULL;
#else
    (void)(self);
    int32_t *_Dst = NULL;
    uint16_t *_Src = NULL;
    size_t len_list = pikaList_getSize(val);
    size_t len_ret = len_list >> 1;
    if(len_list & 0x01) {
        len_ret += 1;
    }
    _Dst = modbus_rt_calloc(1,len_ret << 2);
    _Src = modbus_rt_calloc(1,len_list << 1);
    for(size_t i = 0;i < len_list; i++) {
        _Src[i] =  (uint16_t)pikaList_getInt(val, i);
    }
    modbus_data_regs2int(mode, _Dst, _Src, len_list);
    PikaList* list = New_PikaList();
    for(size_t i = 0; i < len_ret; i++ ) {
        pikaList_append(list, arg_newInt(_Dst[i]));
    }
    modbus_rt_free(_Dst);
    modbus_rt_free(_Src);
    return list;   
#endif
}

PikaObj* _modbus_rt__data_trans__regs2uint(PikaObj *self, PikaObj* val, int mode) {
#if  (!MODBUS_DATA_TRANS_ENABLE)
    pika_platform_printf("MODBUS_DATA_TRANS_ENABLE is not enabled.\n");
    return NULL;
#else
    (void)(self);
    uint32_t *_Dst = NULL;
    uint16_t *_Src = NULL;
    size_t len_list = pikaList_getSize(val);
    size_t len_ret = len_list >> 1;
    if(len_list & 0x01) {
        len_ret += 1;
    }
    _Dst = modbus_rt_calloc(1,len_ret << 2);
    _Src = modbus_rt_calloc(1,len_list << 1);
    for(size_t i = 0;i < len_list; i++) {
        _Src[i] =  (uint16_t)pikaList_getInt(val, i);
    }
    modbus_data_regs2uint(mode, _Dst, _Src, len_list);
    PikaList* list = New_PikaList();
    for(size_t i = 0; i < len_ret; i++ ) {
        pikaList_append(list, arg_newInt(_Dst[i]));
    }
    modbus_rt_free(_Dst);
    modbus_rt_free(_Src);
    return list;   
#endif
}

PikaObj* _modbus_rt__data_trans__regs2long(PikaObj *self, PikaObj* val, int mode) {
#if  (!MODBUS_DATA_TRANS_ENABLE)
    pika_platform_printf("MODBUS_DATA_TRANS_ENABLE is not enabled.\n");
    return NULL;
#else
    (void)(self);
    int64_t *_Dst = NULL;
    uint16_t *_Src = NULL;
    size_t len_list = pikaList_getSize(val);
    size_t len_ret = len_list >> 2;
    if(len_list & 0x03) {
        len_ret += 1;
    }
    _Dst = modbus_rt_calloc(1,len_ret << 3);
    _Src = modbus_rt_calloc(1,len_list << 1);
    for(size_t i = 0;i < len_list; i++) {
        _Src[i] =  (uint16_t)pikaList_getInt(val, i);
    }
    modbus_data_regs2long(mode, _Dst, _Src, len_list);
    PikaList* list = New_PikaList();
    for(size_t i = 0; i < len_ret; i++ ) {
        pikaList_append(list, arg_newInt(_Dst[i]));
    }
    modbus_rt_free(_Dst);
    modbus_rt_free(_Src);
    return list;   
#endif
}

PikaObj* _modbus_rt__data_trans__regs2float(PikaObj *self, PikaObj* val, int mode) {
#if  (!MODBUS_DATA_TRANS_ENABLE)
    pika_platform_printf("MODBUS_DATA_TRANS_ENABLE is not enabled.\n");
    return NULL;
#else
    (void)(self);
    float *_Dst = NULL;
    uint16_t *_Src = NULL;
    size_t len_list = pikaList_getSize(val);
    size_t len_ret = len_list >> 1;
    if(len_list & 0x01) {
        len_ret += 1;
    }
    _Dst = modbus_rt_calloc(1,len_ret << 2);
    _Src = modbus_rt_calloc(1,len_list << 1);
    for(size_t i = 0;i < len_list; i++) {
        _Src[i] =  (uint16_t)pikaList_getInt(val, i);
    }
    modbus_data_regs2float(mode, _Dst, _Src, len_list);
    PikaList* list = New_PikaList();
    for(size_t i = 0; i < len_ret; i++ ) {
        pikaList_append(list, arg_newFloat(_Dst[i]));
    }
    modbus_rt_free(_Dst);
    modbus_rt_free(_Src);
    return list; 
#endif
}

PikaObj* _modbus_rt__data_trans__regs2double(PikaObj *self, PikaObj* val, int mode) {
#if  (!MODBUS_DATA_TRANS_ENABLE)
    pika_platform_printf("MODBUS_DATA_TRANS_ENABLE is not enabled.\n");
    return NULL;
#else
    (void)(self);
    double *_Dst = NULL;
    uint16_t *_Src = NULL;
    size_t len_list = pikaList_getSize(val);
    size_t len_ret = len_list >> 2;
    if(len_list & 0x03) {
        len_ret += 1;
    }
    _Dst = modbus_rt_calloc(1,len_ret << 3);
    _Src = modbus_rt_calloc(1,len_list << 1);
    for(size_t i = 0;i < len_list; i++) {
        _Src[i] =  (uint16_t)pikaList_getInt(val, i);
    }
    modbus_data_regs2double(mode, _Dst, _Src, len_list);
    PikaList* list = New_PikaList();
    for(size_t i = 0; i < len_ret; i++ ) {
        pikaList_append(list, arg_newFloat(_Dst[i]));
    }
    modbus_rt_free(_Dst);
    modbus_rt_free(_Src);
    return list; 
#endif
}


PikaObj* _modbus_rt__data_trans__bytes2regs(PikaObj *self, Arg* val, int mode) {
#if  (!MODBUS_DATA_TRANS_ENABLE)
    pika_platform_printf("MODBUS_DATA_TRANS_ENABLE is not enabled.\n");
    return NULL;
#else
    (void)(self);
    ArgType t = arg_getType(val);
    if (ARG_TYPE_BYTES != t) {
        return NULL;
    }
    uint16_t *_Dst = NULL;
    uint8_t* _Src = arg_getBytes(val);
    size_t _Src_len = arg_getBytesSize(val);
    size_t len_ret = _Src_len >> 1;
    if(_Src_len & 0x01) {
        len_ret += 1;
    }
    _Dst = modbus_rt_calloc(1,len_ret << 1);
    modbus_data_bytes2regs(mode, _Dst, _Src, _Src_len);
    PikaList* list = New_PikaList();
    for(size_t i = 0; i < len_ret; i++ ) {
        pikaList_append(list, arg_newInt(_Dst[i]));
    }
    modbus_rt_free(_Dst);
    return list;
#endif
}

PikaObj* _modbus_rt__data_trans__str2regs(PikaObj *self, char* val, int mode) {
#if  (!MODBUS_DATA_TRANS_ENABLE)
    pika_platform_printf("MODBUS_DATA_TRANS_ENABLE is not enabled.\n");
    return NULL;
#else
    (void)(self);
    uint16_t *_Dst = NULL;
    char * _Src = val;
    size_t _Src_len = strlen(val) + 1;
    size_t len_ret = _Src_len >> 1;
    if(_Src_len & 0x01) {
        len_ret += 1;
    }
    _Dst = modbus_rt_calloc(1,len_ret << 1);
    modbus_data_str2regs(mode, _Dst, _Src, _Src_len);
    PikaList* list = New_PikaList();
    for(size_t i = 0; i < len_ret; i++ ) {
        pikaList_append(list, arg_newInt(_Dst[i]));
    }
    modbus_rt_free(_Dst);
    return list;
#endif
}

PikaObj* _modbus_rt__data_trans__signed2regs(PikaObj *self, PikaObj* val, int mode) {
#if  (!MODBUS_DATA_TRANS_ENABLE)
    pika_platform_printf("MODBUS_DATA_TRANS_ENABLE is not enabled.\n");
    return NULL;
#else
    (void)(self);
    uint16_t *_Dst = NULL;
    int16_t * _Src = NULL;
    size_t len_list = pikaList_getSize(val);
    size_t len_ret = len_list;
    _Dst = modbus_rt_calloc(1,len_ret << 1);
    _Src = modbus_rt_calloc(1,len_list << 1);
    for(int i = 0; i < len_list; i++ ) {
        _Src[i] = (int16_t)pikaList_getInt(val, i);
    }
    modbus_data_signed2regs(mode, _Dst, _Src, len_list);
    PikaList* list = New_PikaList();
    for(int i = 0; i < len_ret; i++ ) {
        pikaList_append(list, arg_newInt(_Dst[i]));
    }
    modbus_rt_free(_Dst);
    modbus_rt_free(_Src);
    return list;
#endif
}

PikaObj* _modbus_rt__data_trans__int2regs(PikaObj *self, PikaObj* val, int mode) {
#if  (!MODBUS_DATA_TRANS_ENABLE)
    pika_platform_printf("MODBUS_DATA_TRANS_ENABLE is not enabled.\n");
    return NULL;
#else
    (void)(self);
    uint16_t *_Dst = NULL;
    int32_t * _Src = NULL;
    size_t len_list = pikaList_getSize(val);
    size_t len_ret = len_list << 1;
    _Dst = modbus_rt_calloc(1,len_ret << 1);
    _Src = modbus_rt_calloc(1,len_list << 2);
    for(int i = 0; i < len_list; i++ ) {
        _Src[i] = (int32_t)pikaList_getInt(val, i);
    }
    modbus_data_int2regs(mode, _Dst, _Src, len_list);
    PikaList* list = New_PikaList();
    for(int i = 0; i < len_ret; i++ ) {
        pikaList_append(list, arg_newInt(_Dst[i]));
    }
    modbus_rt_free(_Dst);
    modbus_rt_free(_Src);
    return list;
#endif
}

PikaObj* _modbus_rt__data_trans__uint2regs(PikaObj *self, PikaObj* val, int mode) {
#if  (!MODBUS_DATA_TRANS_ENABLE)
    pika_platform_printf("MODBUS_DATA_TRANS_ENABLE is not enabled.\n");
    return NULL;
#else
    (void)(self);
    uint16_t *_Dst = NULL;
    uint32_t * _Src = NULL;
    size_t len_list = pikaList_getSize(val);
    size_t len_ret = len_list << 1;
    _Dst = modbus_rt_calloc(1,len_ret << 1);
    _Src = modbus_rt_calloc(1,len_list << 2);
    for(int i = 0; i < len_list; i++ ) {
        _Src[i] = (uint32_t)pikaList_getInt(val, i);
    }
    modbus_data_uint2regs(mode, _Dst, _Src, len_list);
    PikaList* list = New_PikaList();
    for(int i = 0; i < len_ret; i++ ) {
        pikaList_append(list, arg_newInt(_Dst[i]));
    }
    modbus_rt_free(_Dst);
    modbus_rt_free(_Src);
    return list;
#endif
}

PikaObj* _modbus_rt__data_trans__long2regs(PikaObj *self, PikaObj* val, int mode) {
#if  (!MODBUS_DATA_TRANS_ENABLE)
    pika_platform_printf("MODBUS_DATA_TRANS_ENABLE is not enabled.\n");
    return NULL;
#else
    (void)(self);
    uint16_t *_Dst = NULL;
    int64_t * _Src = NULL;
    size_t len_list = pikaList_getSize(val);
    size_t len_ret = len_list << 2;
    _Dst = modbus_rt_calloc(1,len_ret << 1);
    _Src = modbus_rt_calloc(1,len_list << 3);
    for(int i = 0; i < len_list; i++ ) {
        _Src[i] = (int64_t)pikaList_getInt(val, i);
    }
    modbus_data_long2regs(mode, _Dst, _Src, len_list);
    PikaList* list = New_PikaList();
    for(int i = 0; i < len_ret; i++ ) {
        pikaList_append(list, arg_newInt(_Dst[i]));
    }
    modbus_rt_free(_Dst);
    modbus_rt_free(_Src);
    return list;
#endif
}

PikaObj* _modbus_rt__data_trans__float2regs(PikaObj *self, PikaObj* val, int mode) {
#if  (!MODBUS_DATA_TRANS_ENABLE)
    pika_platform_printf("MODBUS_DATA_TRANS_ENABLE is not enabled.\n");
    return NULL;
#else
    (void)(self);
    uint16_t *_Dst = NULL;
    float * _Src = NULL;
    size_t len_list = pikaList_getSize(val);
    size_t len_ret = len_list << 1;
    _Dst = modbus_rt_calloc(1,len_ret << 1);
    _Src = modbus_rt_calloc(1,len_list << 2);
    for(int i = 0; i < len_list; i++ ) {
        _Src[i] = (float)pikaList_getFloat(val, i);
    }
    modbus_data_float2regs(mode, _Dst, _Src, len_list);
    PikaList* list = New_PikaList();
    for(int i = 0; i < len_ret; i++ ) {
        pikaList_append(list, arg_newInt(_Dst[i]));
    }
    modbus_rt_free(_Dst);
    modbus_rt_free(_Src);
    return list;
#endif
}

PikaObj* _modbus_rt__data_trans__double2regs(PikaObj *self, PikaObj* val, int mode) {
#if  (!MODBUS_DATA_TRANS_ENABLE)
    pika_platform_printf("MODBUS_DATA_TRANS_ENABLE is not enabled.\n");
    return NULL;
#else
    (void)(self);
    uint16_t *_Dst = NULL;
    double * _Src = NULL;
    size_t len_list = pikaList_getSize(val);
    size_t len_ret = len_list << 2;
    _Dst = modbus_rt_calloc(1,len_ret << 1);
    _Src = modbus_rt_calloc(1,len_list << 3);
    for(int i = 0; i < len_list; i++ ) {
        _Src[i] = (double)pikaList_getFloat(val, i);
    }
    modbus_data_double2regs(mode, _Dst, _Src, len_list);
    PikaList* list = New_PikaList();
    for(int i = 0; i < len_ret; i++ ) {
        pikaList_append(list, arg_newInt(_Dst[i]));
    }
    modbus_rt_free(_Dst);
    modbus_rt_free(_Src);
    return list;
#endif
}
