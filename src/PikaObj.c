/*
 * This file is part of the PikaScript project.
 * http://github.com/pikastech/pikascript
 *
 * MIT License
 *
 * Copyright (c) 2021 lyon 李昂 liang6516@outlook.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "PikaObj.h"
#include <stdint.h>
#include "BaseObj.h"
#include "PikaCompiler.h"
#include "PikaPlatform.h"
#include "dataArgs.h"
#include "dataMemory.h"
#include "dataString.h"
#include "dataStrs.h"

static volatile Arg* _help_modules_cmodule = NULL;
static volatile PIKA_BOOL in_root_obj = PIKA_FALSE;

static enum shellCTRL __obj_shellLineHandler_REPL(PikaObj* self,
                                                  char* input_line,
                                                  ShellConfig* shell);

static const uint64_t __talbe_fast_atoi[][10] = {
    {0, 1e0, 2e0, 3e0, 4e0, 5e0, 6e0, 7e0, 8e0, 9e0},
    {0, 1e1, 2e1, 3e1, 4e1, 5e1, 6e1, 7e1, 8e1, 9e1},
    {0, 1e2, 2e2, 3e2, 4e2, 5e2, 6e2, 7e2, 8e2, 9e2},
    {0, 1e3, 2e3, 3e3, 4e3, 5e3, 6e3, 7e3, 8e3, 9e3},
    {0, 1e4, 2e4, 3e4, 4e4, 5e4, 6e4, 7e4, 8e4, 9e4},
    {0, 1e5, 2e5, 3e5, 4e5, 5e5, 6e5, 7e5, 8e5, 9e5},
    {0, 1e6, 2e6, 3e6, 4e6, 5e6, 6e6, 7e6, 8e6, 9e6},
    {0, 1e7, 2e7, 3e7, 4e7, 5e7, 6e7, 7e7, 8e7, 9e7},
    {0, 1e8, 2e8, 3e8, 4e8, 5e8, 6e8, 7e8, 8e8, 9e8},
    {0, 1e9, 2e9, 3e9, 4e9, 5e9, 6e9, 7e9, 8e9, 9e9},
};

int64_t fast_atoi(char* src) {
    const char* p = src;
    uint16_t size = strGetSize(src);
    p = p + size - 1;
    if (*p) {
        int64_t s = 0;
        const uint64_t* n = __talbe_fast_atoi[0];
        while (p != src) {
            s += n[(*p - '0')];
            n += 10;
            p--;
        }
        if (*p == '-') {
            return -s;
        }
        return s + n[(*p - '0')];
    }
    return 0;
}

static uint16_t const str100p[100] = {
    0x3030, 0x3130, 0x3230, 0x3330, 0x3430, 0x3530, 0x3630, 0x3730, 0x3830,
    0x3930, 0x3031, 0x3131, 0x3231, 0x3331, 0x3431, 0x3531, 0x3631, 0x3731,
    0x3831, 0x3931, 0x3032, 0x3132, 0x3232, 0x3332, 0x3432, 0x3532, 0x3632,
    0x3732, 0x3832, 0x3932, 0x3033, 0x3133, 0x3233, 0x3333, 0x3433, 0x3533,
    0x3633, 0x3733, 0x3833, 0x3933, 0x3034, 0x3134, 0x3234, 0x3334, 0x3434,
    0x3534, 0x3634, 0x3734, 0x3834, 0x3934, 0x3035, 0x3135, 0x3235, 0x3335,
    0x3435, 0x3535, 0x3635, 0x3735, 0x3835, 0x3935, 0x3036, 0x3136, 0x3236,
    0x3336, 0x3436, 0x3536, 0x3636, 0x3736, 0x3836, 0x3936, 0x3037, 0x3137,
    0x3237, 0x3337, 0x3437, 0x3537, 0x3637, 0x3737, 0x3837, 0x3937, 0x3038,
    0x3138, 0x3238, 0x3338, 0x3438, 0x3538, 0x3638, 0x3738, 0x3838, 0x3938,
    0x3039, 0x3139, 0x3239, 0x3339, 0x3439, 0x3539, 0x3639, 0x3739, 0x3839,
    0x3939,
};

char* fast_itoa(char* buf, uint32_t val) {
    char* p = &buf[10];
    *p = '\0';
    while (val >= 100) {
        uint32_t const old = val;
        p -= 2;
        val /= 100;
        __platform_memcpy(p, &str100p[old - (val * 100)], sizeof(uint16_t));
    }
    p -= 2;
    __platform_memcpy(p, &str100p[val], sizeof(uint16_t));
    return &p[val < 10];
}

static int32_t obj_deinit_no_del(PikaObj* self) {
    /* free the list */
    args_deinit(self->list);
    /* free the pointer */
    pikaFree(self, sizeof(PikaObj));
    self = NULL;
    return 0;
}

int32_t obj_deinit(PikaObj* self) {
    Arg* del = obj_getMethodArg(self, "__del__");
    if (NULL != del) {
        const uint8_t bytes[] = {
            0x04, 0x00, 0x00, 0x00, /* instruct array size */
            0x00, 0x82, 0x01, 0x00, /* instruct array */
            0x09, 0x00, 0x00, 0x00, /* const pool size */
            0x00, 0x5f, 0x5f, 0x64, 0x65,
            0x6c, 0x5f, 0x5f, 0x00, /* const pool */
        };
        pikaVM_runByteCode(self, (uint8_t*)bytes);
        arg_deinit(del);
    }
    extern volatile PikaObj* __pikaMain;
    if (self == (PikaObj*)__pikaMain) {
        void _mem_cache_deinit(void);
        void VMSignal_deinit(void);
        _mem_cache_deinit();
#if PIKA_EVENT_ENABLE
        VMSignal_deinit();
#endif
        if (NULL != _help_modules_cmodule) {
            arg_deinit((Arg*)_help_modules_cmodule);
            _help_modules_cmodule = NULL;
        }
        __pikaMain = NULL;
    }
    return obj_deinit_no_del(self);
}

PIKA_RES obj_setInt(PikaObj* self, char* argPath, int64_t val) {
    PikaObj* obj = obj_getHostObj(self, argPath);
    if (NULL == obj) {
        /* [error] object no found */
        return PIKA_RES_ERR_ARG_NO_FOUND;
    }
    char* name = strPointToLastToken(argPath, '.');
    args_setInt(obj->list, name, val);
    return PIKA_RES_OK;
}

PIKA_RES obj_setPtr(PikaObj* self, char* argPath, void* pointer) {
    PikaObj* obj = obj_getHostObj(self, argPath);
    if (NULL == obj) {
        /* [error] object no found */
        return PIKA_RES_ERR_ARG_NO_FOUND;
    }
    char* name = strPointToLastToken(argPath, '.');
    args_setPtr(obj->list, name, pointer);
    return PIKA_RES_OK;
}

PIKA_RES obj_setRef(PikaObj* self, char* argPath, PikaObj* pointer) {
    PikaObj* obj = obj_getHostObj(self, argPath);
    if (NULL == obj) {
        /* [error] object no found */
        return PIKA_RES_ERR_ARG_NO_FOUND;
    }
    char* name = strPointToLastToken(argPath, '.');
    args_setRef(obj->list, name, pointer);
    return PIKA_RES_OK;
}

PIKA_RES obj_setFloat(PikaObj* self, char* argPath, pika_float value) {
    PikaObj* obj = obj_getHostObj(self, argPath);
    if (NULL == obj) {
        /* [error] object no found */
        return PIKA_RES_ERR_ARG_NO_FOUND;
    }
    char* name = strPointToLastToken(argPath, '.');
    args_setFloat(obj->list, name, value);
    return PIKA_RES_OK;
}

PIKA_RES obj_setStr(PikaObj* self, char* argPath, char* str) {
    PikaObj* obj = obj_getHostObj(self, argPath);
    if (NULL == obj) {
        return PIKA_RES_ERR_ARG_NO_FOUND;
    }
    char* name = strPointToLastToken(argPath, '.');
    return args_setStr(obj->list, name, str);
}

PIKA_RES obj_setNone(PikaObj* self, char* argPath) {
    PikaObj* obj = obj_getHostObj(self, argPath);
    if (NULL == obj) {
        return PIKA_RES_ERR_ARG_NO_FOUND;
    }
    char* name = strPointToLastToken(argPath, '.');
    args_setNone(obj->list, name);
    return PIKA_RES_OK;
}

PIKA_RES obj_setBytes(PikaObj* self, char* argPath, uint8_t* src, size_t size) {
    PikaObj* obj = obj_getHostObj(self, argPath);
    if (NULL == obj) {
        return PIKA_RES_ERR_ARG_NO_FOUND;
    }
    char* name = strPointToLastToken(argPath, '.');
    args_setBytes(obj->list, name, src, size);
    return PIKA_RES_OK;
}

int64_t obj_getInt(PikaObj* self, char* argPath) {
    PikaObj* obj = obj_getHostObj(self, argPath);
    if (NULL == obj) {
        return -999999999;
    }
    char* argName = strPointToLastToken(argPath, '.');
    int64_t res = args_getInt(obj->list, argName);
    return res;
}

Arg* obj_getArg(PikaObj* self, char* argPath) {
    PIKA_BOOL is_temp = PIKA_FALSE;
    PikaObj* obj = obj_getHostObjWithIsTemp(self, argPath, &is_temp);
    if (NULL == obj) {
        return NULL;
    }
    Arg* res = NULL;
    char* argName = strPointToLastToken(argPath, '.');
    res = args_getArg(obj->list, argName);
    if (is_temp) {
        obj_setArg(self, "_buf", res);
        res = obj_getArg(self, "_buf");
        obj_deinit(obj);
    }
    return res;
}

uint8_t* obj_getBytes(PikaObj* self, char* argPath) {
    PikaObj* obj = obj_getHostObj(self, argPath);
    if (NULL == obj) {
        return NULL;
    }
    char* argName = strPointToLastToken(argPath, '.');
    return args_getBytes(obj->list, argName);
}

size_t obj_getBytesSize(PikaObj* self, char* argPath) {
    PikaObj* obj = obj_getHostObj(self, argPath);
    if (NULL == obj) {
        return 0;
    }
    char* argName = strPointToLastToken(argPath, '.');
    return args_getBytesSize(obj->list, argName);
}

size_t obj_loadBytes(PikaObj* self, char* argPath, uint8_t* out_buff) {
    size_t size_mem = obj_getBytesSize(self, argPath);
    void* src = obj_getBytes(self, argPath);
    if (0 == size_mem) {
        return 0;
    }
    if (NULL == src) {
        return 0;
    }
    __platform_memcpy(out_buff, src, size_mem);
    return size_mem;
}

static PIKA_RES __obj_setArg(PikaObj* self,
                             char* argPath,
                             Arg* arg,
                             uint8_t is_copy) {
    /* setArg would copy arg */
    PikaObj* obj = obj_getHostObj(self, argPath);
    if (NULL == obj) {
        /* object no found */
        return PIKA_RES_ERR_ARG_NO_FOUND;
    }
    char* argName = strPointToLastToken(argPath, '.');
    Arg* newArg;
    if (is_copy) {
        newArg = arg_copy(arg);
    } else {
        newArg = arg;
    }
    newArg = arg_setName(newArg, argName);
    args_setArg(obj->list, newArg);
    return PIKA_RES_OK;
}

PIKA_RES obj_setArg(PikaObj* self, char* argPath, Arg* arg) {
    return __obj_setArg(self, argPath, arg, 1);
};

PIKA_RES obj_setArg_noCopy(PikaObj* self, char* argPath, Arg* arg) {
    return __obj_setArg(self, argPath, arg, 0);
}

void* obj_getPtr(PikaObj* self, char* argPath) {
    PikaObj* obj = obj_getHostObj(self, argPath);
    if (NULL == obj) {
        return NULL;
    }
    char* argName = strPointToLastToken(argPath, '.');
    void* res = args_getPtr(obj->list, argName);
    return res;
}

pika_float obj_getFloat(PikaObj* self, char* argPath) {
    PikaObj* obj = obj_getHostObj(self, argPath);
    if (NULL == obj) {
        return -999.999;
    }
    char* argName = strPointToLastToken(argPath, '.');
    pika_float res = args_getFloat(obj->list, argName);
    return res;
}

char* obj_getStr(PikaObj* self, char* argPath) {
    PikaObj* obj = obj_getHostObj(self, argPath);
    if (NULL == obj) {
        return NULL;
    }
    char* argName = strPointToLastToken(argPath, '.');
    char* res = args_getStr(obj->list, argName);
    return res;
}

PikaObj* obj_getClassObjByNewFun(PikaObj* context,
                                 char* name,
                                 NewFun newClassFun) {
    Args* initArgs = New_args(NULL);
    PikaObj* thisClass = newClassFun(initArgs);
    thisClass->constructor = newClassFun;
    thisClass->refcnt = 0;
    args_deinit(initArgs);
    return thisClass;
}

Arg* _obj_getProp(PikaObj* obj, char* name) {
    NativeProperty* prop = obj_getPtr(obj, "@p");
    PikaObj* class_obj = NULL;
    Arg* method = NULL;
    if (NULL == prop) {
        if (NULL != obj->constructor) {
            class_obj = obj_getClassObj(obj);
            prop = obj_getPtr(class_obj, "@p");
        }
    }
    Hash method_hash = hash_time33(name);
    while (1) {
        if (prop == NULL) {
            break;
        }
        int size = prop->methodGroupCount;
        /* binary search */
        if (size == 0) {
            goto next;
        }
#if !PIKA_NANO_ENABLE
        if (size > 16) {
            int left = 0;
            int right = size - 1;
            int mid = 0;
            while (1) {
                if (left > right) {
                    break;
                }
                mid = (right + left) >> 1;
                Arg* prop_this = (Arg*)(prop->methodGroup + mid);
                if (prop_this->name_hash == method_hash) {
                    method = prop_this;
                    goto exit;
                } else if (prop_this->name_hash < method_hash) {
                    left = mid + 1;
                } else {
                    right = mid - 1;
                }
            }
            goto next;
        }
#endif
        for (int i = 0; i < (int)prop->methodGroupCount; i++) {
            Arg* prop_this = (Arg*)(prop->methodGroup + i);
            if (prop_this->name_hash == method_hash) {
                method = prop_this;
                goto exit;
            }
        }
        goto next;
    next:
        prop = (NativeProperty*)prop->super;
    }
exit:
    if (NULL != class_obj) {
        obj_deinit_no_del(class_obj);
    }
    return method;
}

Arg* _obj_getMethodArg(PikaObj* obj, char* methodPath, Arg* arg_reg) {
    Arg* method = NULL;
    char* methodName = strPointToLastToken(methodPath, '.');
    method = obj_getArg(obj, methodName);
    if (NULL != method) {
        method = arg_copy_noalloc(method, arg_reg);
        goto exit;
    }
    method = _obj_getProp(obj, methodName);
    if (NULL != method) {
        goto exit;
    }
exit:
    return method;
}

Arg* obj_getMethodArg(PikaObj* obj, char* methodPath) {
    return _obj_getMethodArg(obj, methodPath, NULL);
}

Arg* obj_getMethodArg_noalloc(PikaObj* obj, char* methodPath, Arg* arg_reg) {
    return _obj_getMethodArg(obj, methodPath, arg_reg);
}

NewFun obj_getClass(PikaObj* obj) {
    return (NewFun)obj->constructor;
}

PikaObj* obj_getClassObj(PikaObj* obj) {
    NewFun classPtr = obj_getClass(obj);
    if (NULL == classPtr) {
        return NULL;
    }
    PikaObj* classObj = obj_getClassObjByNewFun(obj, "", classPtr);
    return classObj;
}

void* getNewClassObjFunByName(PikaObj* obj, char* name) {
    char* classPath = name;
    /* init the subprocess */
    void* newClassObjFun = args_getPtr(obj->list, classPath);
    return newClassObjFun;
}

PikaObj* removeMethodInfo(PikaObj* thisClass) {
#if PIKA_METHOD_CACHE_ENABLE
#else
    args_removeArg(thisClass->list, args_getArg(thisClass->list, "@p"));
#endif
    return thisClass;
}

PikaObj* newNormalObj(NewFun newObjFun) {
    PikaObj* thisClass = obj_getClassObjByNewFun(NULL, "", newObjFun);
    obj_refcntInc(thisClass);
    obj_setFlag(thisClass, OBJ_FLAG_ALREADY_INIT);
    return removeMethodInfo(thisClass);
}

#ifdef __linux

#include <errno.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#define ECHOFLAGS (ECHO | ECHOE | ECHOK | ECHONL)
//函数set_disp_mode用于控制是否开启输入回显功能
//如果option为0，则关闭回显，为1则打开回显
static int set_disp_mode(int fd, int option) {
    int err;
    struct termios term;
    if (tcgetattr(fd, &term) == -1) {
        perror("Cannot get the attribution of the terminal");
        return 1;
    }
    if (option)
        term.c_lflag |= ECHOFLAGS;
    else
        term.c_lflag &= ~ECHOFLAGS;
    err = tcsetattr(fd, TCSAFLUSH, &term);
    if (err == -1 && err == EINTR) {
        perror("Cannot set the attribution of the terminal");
        return 1;
    }
    return 0;
}
#endif

static volatile uint8_t logo_printed = 0;

extern volatile PikaObj* __pikaMain;
PikaObj* newRootObj(char* name, NewFun newObjFun) {
    in_root_obj = PIKA_TRUE;
#if PIKA_POOL_ENABLE
    mem_pool_init();
#endif
#if __linux
    // set_disp_mode(STDIN_FILENO, 0);
#endif
    PikaObj* newObj = newNormalObj(newObjFun);
    if (!logo_printed) {
        logo_printed = 1;
        __platform_printf("\r\n");
        __platform_printf("~~~/ POWERED BY \\~~~\r\n");
        __platform_printf("~  pikascript.com  ~\r\n");
        __platform_printf("~~~~~~~~~~~~~~~~~~~~\r\n");
    }
    __pikaMain = newObj;
    in_root_obj = PIKA_FALSE;
    return newObj;
}

Arg* arg_newMetaObj(NewFun new_obj_fun) {
    Arg* arg_new = New_arg(NULL);
    /* m means meta-object */
    arg_new = arg_setPtr(arg_new, "", ARG_TYPE_OBJECT_META, (void*)new_obj_fun);
    return arg_new;
}

Arg* arg_newDirectObj(NewFun new_obj_fun) {
    PikaObj* newObj = newNormalObj(new_obj_fun);
    Arg* arg_new = arg_newPtr(ARG_TYPE_OBJECT_NEW, newObj);
    return arg_new;
}

Arg* obj_newObjInPackage(NewFun new_obj_fun) {
    return arg_newDirectObj(new_obj_fun);
}

static PikaObj* __obj_initSubObj(PikaObj* obj, char* name) {
    PikaObj* res = NULL;
    NewFun constructor = (NewFun)getNewClassObjFunByName(obj, name);
    Args buffs = {0};
    PikaObj* thisClass;
    PikaObj* new_obj;
    if (NULL == constructor) {
        /* no such object */
        res = NULL;
        goto exit;
    }
    thisClass = obj_getClassObjByNewFun(obj, name, constructor);
    new_obj = removeMethodInfo(thisClass);
    obj_refcntInc(new_obj);
    obj_runNativeMethod(new_obj, "__init__", NULL);
    args_setPtrWithType(obj->list, name, ARG_TYPE_OBJECT, new_obj);
    res = obj_getPtr(obj, name);
    goto exit;
exit:
    strsDeinit(&buffs);
    return res;
}

PikaObj* _arg_to_obj(Arg* self, PIKA_BOOL* pIsTemp) {
    if (NULL == self) {
        return NULL;
    }
    if (argType_isObject(arg_getType(self))) {
        return arg_getPtr(self);
    }
#if !PIKA_NANO_ENABLE
    if (arg_getType(self) == ARG_TYPE_STRING) {
        PikaObj* New_PikaStdData_String(Args * args);
        PikaObj* obj = newNormalObj(New_PikaStdData_String);
        obj_setStr(obj, "str", arg_getStr(self));
        if (NULL != pIsTemp) {
            *pIsTemp = PIKA_TRUE;
        }
        return obj;
    }
    if (arg_getType(self) == ARG_TYPE_BYTES) {
        PikaObj* New_PikaStdData_ByteArray(Args * args);
        PikaObj* obj = newNormalObj(New_PikaStdData_ByteArray);
        obj_setArg(obj, "raw", self);
        if (NULL != pIsTemp) {
            *pIsTemp = PIKA_TRUE;
        }
        return obj;
    }
#endif
    return NULL;
}

static PikaObj* __obj_getObjDirect(PikaObj* self,
                                   char* name,
                                   PIKA_BOOL* pIsTemp) {
    *pIsTemp = PIKA_FALSE;
    if (NULL == self) {
        return NULL;
    }
    /* finded object, check type*/
    Arg* arg_obj = args_getArg(self->list, name);
    ArgType type = ARG_TYPE_NONE;
    if (NULL == arg_obj) {
        arg_obj = _obj_getProp(self, name);
    }
    if (NULL == arg_obj) {
        return NULL;
    }
    type = arg_getType(arg_obj);
    /* found meta Object */
    if (type == ARG_TYPE_OBJECT_META) {
        return __obj_initSubObj(self, name);
    }
    /* found Objcet */
    if (argType_isObject(type)) {
        return arg_getPtr(arg_obj);
    }
#if !PIKA_NANO_ENABLE
    /* found class */
    if (type == ARG_TYPE_METHOD_NATIVE_CONSTRUCTOR ||
        type == ARG_TYPE_METHOD_CONSTRUCTOR) {
        *pIsTemp = PIKA_TRUE;
        PikaObj* method_args_obj = New_TinyObj(NULL);
        Arg* cls_obj_arg = obj_runMethodArg(self, method_args_obj, arg_obj);
        obj_deinit(method_args_obj);
        if (type == ARG_TYPE_METHOD_NATIVE_CONSTRUCTOR) {
            obj_runNativeMethod(arg_getPtr(cls_obj_arg), "__init__", NULL);
        }
        PikaObj* res = arg_getPtr(cls_obj_arg);
        arg_deinit(cls_obj_arg);
        return res;
    }
#endif
    return _arg_to_obj(arg_obj, pIsTemp);
}

static PikaObj* __obj_getObjWithKeepDeepth(PikaObj* self,
                                           char* objPath,
                                           PIKA_BOOL* pIsTemp,
                                           int32_t keepDeepth) {
    char objPath_buff[PIKA_PATH_BUFF_SIZE];
    char* objPath_ptr = objPath_buff;
    strcpy(objPath_buff, objPath);
    int32_t token_num = strGetTokenNum(objPath, '.');
    PikaObj* obj = self;
    for (int32_t i = 0; i < token_num - keepDeepth; i++) {
        char* token = strPopFirstToken(&objPath_ptr, '.');
        obj = __obj_getObjDirect(obj, token, pIsTemp);
        if (obj == NULL) {
            goto exit;
        }
    }
    goto exit;
exit:
    return obj;
}

PikaObj* obj_getObj(PikaObj* self, char* objPath) {
    PIKA_BOOL is_temp = PIKA_FALSE;
    return __obj_getObjWithKeepDeepth(self, objPath, &is_temp, 0);
}

PikaObj* obj_getHostObj(PikaObj* self, char* objPath) {
    PIKA_BOOL is_temp = PIKA_FALSE;
    return __obj_getObjWithKeepDeepth(self, objPath, &is_temp, 1);
}

PikaObj* obj_getHostObjWithIsTemp(PikaObj* self,
                                  char* objPath,
                                  PIKA_BOOL* pIsTemp) {
    return __obj_getObjWithKeepDeepth(self, objPath, pIsTemp, 1);
}

Method methodArg_getPtr(Arg* method_arg) {
    MethodProp* method_store = (MethodProp*)arg_getContent(method_arg);
    return method_store->ptr;
}

char* methodArg_getTypeList(Arg* method_arg, char* buffs, size_t size) {
    MethodProp* method_store = (MethodProp*)arg_getContent(method_arg);
    if (NULL != method_store->type_list) {
        return strcpy(buffs, method_store->type_list);
    }
    char* method_dec = methodArg_getDec(method_arg);
    pika_assert(strGetSize(method_dec) <= size);
    if (strGetSize(method_dec) > size) {
        __platform_printf(
            "OverFlowError: please use bigger PIKA_LINE_BUFF_SIZE\r\n");
        while (1) {
        }
    }
    char* res = strCut(buffs, method_dec, '(', ')');
    pika_assert(NULL != res);
    return res;
}

char* methodArg_getName(Arg* method_arg, char* buffs, size_t size) {
    MethodProp* method_store = (MethodProp*)arg_getContent(method_arg);
    if (NULL != method_store->name) {
        return strcpy(buffs, method_store->name);
    }
    char* method_dec = methodArg_getDec(method_arg);
    pika_assert(strGetSize(method_dec) <= size);
    char* res = strGetFirstToken(buffs, method_dec, '(');
    return res;
}

Method obj_getNativeMethod(PikaObj* self, char* method_name) {
    Arg* method_arg = obj_getMethodArg(self, method_name);
    if (NULL == method_arg) {
        return NULL;
    }
    Method res = methodArg_getPtr(method_arg);
    arg_deinit(method_arg);
    return res;
}

ByteCodeFrame* methodArg_getBytecodeFrame(Arg* method_arg) {
    MethodProp* method_store = (MethodProp*)arg_getContent(method_arg);
    return method_store->bytecode_frame;
}

char* methodArg_getDec(Arg* method_arg) {
    MethodProp* method_store = (MethodProp*)arg_getContent(method_arg);
    return method_store->declareation;
}

PikaObj* methodArg_getDefContext(Arg* method_arg) {
    MethodProp* method_store = (MethodProp*)arg_getContent(method_arg);
    return method_store->def_context;
}

void _update_proxy(PikaObj* self, char* name) {
#if PIKA_NANO_ENABLE
    return;
#endif
    if (name[0] != '_' || name[1] != '_') {
        return;
    }
    if (!obj_getFlag(self, OBJ_FLAG_PROXY_GETATTRIBUTE)) {
        if (strEqu(name, "__getattribute__")) {
            obj_setFlag(self, OBJ_FLAG_PROXY_GETATTRIBUTE);
            return;
        }
    }
    if (!obj_getFlag(self, OBJ_FLAG_PROXY_GETATTR)) {
        if (strEqu(name, "__getattr__")) {
            obj_setFlag(self, OBJ_FLAG_PROXY_GETATTR);
            return;
        }
    }
    if (!obj_getFlag(self, OBJ_FLAG_PROXY_SETATTR)) {
        if (strEqu(name, "__setattr__")) {
            obj_setFlag(self, OBJ_FLAG_PROXY_SETATTR);
            return;
        }
    }
}

static void obj_saveMethodInfo(PikaObj* self, MethodInfo* method_info) {
    Arg* arg = New_arg(NULL);
    MethodProp method_store = {
        .ptr = method_info->ptr,
        .type_list = method_info->typelist,
        .name = method_info->name,
        .bytecode_frame = method_info->bytecode_frame,
        .def_context = method_info->def_context,
        .declareation = method_info->dec,  // const
    };
    char* name = method_info->name;
    if (NULL == method_info->name) {
        char name_buff[PIKA_LINE_BUFF_SIZE / 2] = {0};
        name = strGetFirstToken(name_buff, method_info->dec, '(');
    }
    /* the first arg_value */
    arg = arg_setStruct(arg, name, &method_store, sizeof(method_store));
    pika_assert(NULL != arg);
    arg_setType(arg, method_info->type);
    _update_proxy(self, name);
    args_setArg(self->list, arg);
}

static int32_t __class_defineMethodWithType(PikaObj* self,
                                            char* declareation,
                                            char* name,
                                            char* typelist,
                                            Method method_ptr,
                                            ArgType method_type,
                                            PikaObj* def_context,
                                            ByteCodeFrame* bytecode_frame) {
    int32_t res = 0;
    Args buffs = {0};
    PikaObj* method_host = self;
    MethodInfo method_info = {0};
    if (NULL == method_host) {
        /* no found method object */
        res = 1;
        goto exit;
    }
    method_info.dec = declareation;
    method_info.name = name;
    method_info.ptr = (void*)method_ptr;
    method_info.type = method_type;
    method_info.def_context = def_context;
    method_info.bytecode_frame = bytecode_frame;
    method_info.typelist = typelist;
    obj_saveMethodInfo(method_host, &method_info);
    res = 0;
    goto exit;
exit:
    strsDeinit(&buffs);
    return res;
}

/* define a constructor method */
int32_t class_defineConstructor(PikaObj* self,
                                char* name,
                                char* typelist,
                                Method methodPtr) {
    return __class_defineMethodWithType(self, NULL, name, typelist, methodPtr,
                                        ARG_TYPE_METHOD_NATIVE_CONSTRUCTOR,
                                        NULL, NULL);
}

/* define a native method as default */
int32_t class_defineMethod(PikaObj* self,
                           char* name,
                           char* typelist,
                           Method methodPtr) {
    return __class_defineMethodWithType(self, NULL, name, typelist, methodPtr,
                                        ARG_TYPE_METHOD_NATIVE, NULL, NULL);
}

/* define object method, object method is which startwith (self) */
int32_t class_defineRunTimeConstructor(PikaObj* self,
                                       char* declareation,
                                       Method methodPtr,
                                       PikaObj* def_context,
                                       ByteCodeFrame* bytecode_frame) {
    return __class_defineMethodWithType(self, declareation, NULL, NULL,
                                        methodPtr, ARG_TYPE_METHOD_CONSTRUCTOR,
                                        def_context, bytecode_frame);
}

/* define object method, object method is which startwith (self) */
int32_t class_defineObjectMethod(PikaObj* self,
                                 char* declareation,
                                 Method methodPtr,
                                 PikaObj* def_context,
                                 ByteCodeFrame* bytecode_frame) {
    return __class_defineMethodWithType(self, declareation, NULL, NULL,
                                        methodPtr, ARG_TYPE_METHOD_OBJECT,
                                        def_context, bytecode_frame);
}

/* define a static method as default */
int32_t class_defineStaticMethod(PikaObj* self,
                                 char* declareation,
                                 Method methodPtr,
                                 PikaObj* def_context,
                                 ByteCodeFrame* bytecode_frame) {
    return __class_defineMethodWithType(self, declareation, NULL, NULL,
                                        methodPtr, ARG_TYPE_METHOD_STATIC,
                                        def_context, bytecode_frame);
}

int32_t obj_removeArg(PikaObj* self, char* argPath) {
    PikaObj* objHost = obj_getHostObj(self, argPath);
    char* argName;
    int32_t res;
    int32_t err = 0;
    if (NULL == objHost) {
        /* [error] object no found */
        err = 1;
        goto exit;
    }
    argName = strPointToLastToken(argPath, '.');
    res = args_removeArg(objHost->list, args_getArg(objHost->list, argName));
    if (1 == res) {
        /*[error] not found arg*/
        err = 2;
        goto exit;
    }
    goto exit;
exit:
    return err;
}

int32_t obj_isArgExist(PikaObj* self, char* argPath) {
    PikaObj* obj_host = obj_getHostObj(self, argPath);
    int32_t res = 0;
    char* argName;
    Arg* arg;
    if (NULL == obj_host) {
        /* [error] object no found */
        res = 1;
        goto exit;
    }
    argName = strPointToLastToken(argPath, '.');
    arg = args_getArg(obj_host->list, argName);
    if (NULL == arg) {
        /* no found arg */
        res = 0;
        goto exit;
    }
    /* found arg */
    res = 1;
    goto exit;

exit:
    return res;
}

VMParameters* obj_run(PikaObj* self, char* cmd) {
    return pikaVM_run(self, cmd);
}

PIKA_RES obj_runNativeMethod(PikaObj* self, char* method_name, Args* args) {
    Method native_method = obj_getNativeMethod(self, method_name);
    if (NULL == native_method) {
        return PIKA_RES_ERR_ARG_NO_FOUND;
    }
    native_method(self, args);
    return PIKA_RES_OK;
}

static void __clearBuff(ShellConfig* shell) {
    memset(shell->lineBuff, 0, PIKA_LINE_BUFF_SIZE);
    shell->line_curpos = 0;
    shell->line_position = 0;
}

enum PIKA_SHELL_STATE {
    PIKA_SHELL_STATE_NORMAL,
    PIKA_SHELL_STATE_WAIT_SPEC_KEY,
    PIKA_SHELL_STATE_WAIT_FUNC_KEY,
};

static void _obj_runChar_beforeRun(PikaObj* self, ShellConfig* shell) {
    /* create the line buff for the first time */
    shell->inBlock = PIKA_FALSE;
    shell->stat = PIKA_SHELL_STATE_NORMAL;
    shell->line_position = 0;
    shell->line_curpos = 0;
    __clearBuff(shell);
    __platform_printf("%s", shell->prefix);
}

static void _putc_cmd(char KEY_POS, int pos) {
    const char cmd[] = {0x1b, 0x5b, KEY_POS, 0x00};
    for (int i = 0; i < pos; i++) {
        __platform_printf((char*)cmd);
    }
}

enum shellCTRL _do_obj_runChar(PikaObj* self,
                               char inputChar,
                               ShellConfig* shell) {
    char* input_line = NULL;
    enum shellCTRL ctrl = SHELL_CTRL_CONTINUE;
#if !(defined(__linux) || defined(_WIN32))
    __platform_printf("%c", inputChar);
#endif
    if (inputChar == '\n' && shell->lastChar == '\r') {
        ctrl = SHELL_CTRL_CONTINUE;
        goto exit;
    }
    if (inputChar == 0x1b) {
        shell->stat = PIKA_SHELL_STATE_WAIT_SPEC_KEY;
        ctrl = SHELL_CTRL_CONTINUE;
        goto exit;
    }
    if (shell->stat == PIKA_SHELL_STATE_WAIT_SPEC_KEY) {
        if (inputChar == 0x5b) {
            shell->stat = PIKA_SHELL_STATE_WAIT_FUNC_KEY;
            ctrl = SHELL_CTRL_CONTINUE;
            goto exit;
        }
        shell->stat = PIKA_SHELL_STATE_NORMAL;
    }
    if (shell->stat == PIKA_SHELL_STATE_WAIT_FUNC_KEY) {
        if (inputChar == KEY_LEFT) {
            if (shell->line_curpos) {
                shell->line_curpos--;
            } else {
                __platform_printf(" ");
            }
            ctrl = SHELL_CTRL_CONTINUE;
            goto exit;
        }
        if (inputChar == KEY_RIGHT) {
            if (shell->line_curpos < shell->line_position) {
                // __platform_printf("%c", shell->lineBuff[shell->line_curpos]);
                shell->line_curpos++;
            } else {
                __platform_printf("\b");
            }
            ctrl = SHELL_CTRL_CONTINUE;
            goto exit;
        }
        if (inputChar == KEY_UP) {
            _putc_cmd(KEY_DOWN, 1);
            ctrl = SHELL_CTRL_CONTINUE;
            goto exit;
        }
        if (inputChar == KEY_DOWN) {
            ctrl = SHELL_CTRL_CONTINUE;
            goto exit;
        }
    }
    if ((inputChar == '\b') || (inputChar == 127)) {
        if (shell->line_position == 0) {
            __platform_printf(" ");
            ctrl = SHELL_CTRL_CONTINUE;
            goto exit;
        }
        __platform_printf(" \b");
        shell->line_position--;
        shell->line_curpos--;
        __platform_memmove(shell->lineBuff + shell->line_curpos,
                           shell->lineBuff + shell->line_curpos + 1,
                           shell->line_position - shell->line_curpos);
        shell->lineBuff[shell->line_position] = 0;
        if (shell->line_curpos != shell->line_position) {
            /* update screen */
            __platform_printf(shell->lineBuff + shell->line_curpos);
            __platform_printf(" ");
            _putc_cmd(KEY_LEFT, shell->line_position - shell->line_curpos + 1);
        }
        ctrl = SHELL_CTRL_CONTINUE;
        goto exit;
    }
    if ((inputChar != '\r') && (inputChar != '\n')) {
        if (shell->line_position + 1 >= PIKA_LINE_BUFF_SIZE) {
            __platform_printf(
                "\r\nError: line buff overflow, please use bigger "
                "'PIKA_LINE_BUFF_SIZE'\r\n");
            ctrl = SHELL_CTRL_EXIT;
            __clearBuff(shell);
            goto exit;
        }
        if ('\0' != inputChar) {
            __platform_memmove(shell->lineBuff + shell->line_curpos + 1,
                               shell->lineBuff + shell->line_curpos,
                               shell->line_position - shell->line_curpos);
            shell->lineBuff[shell->line_position + 1] = 0;
            if (shell->line_curpos != shell->line_position) {
                __platform_printf(shell->lineBuff + shell->line_curpos + 1);
                _putc_cmd(KEY_LEFT, shell->line_position - shell->line_curpos);
            }
            shell->lineBuff[shell->line_curpos] = inputChar;
            shell->line_position++;
            shell->line_curpos++;
        }
        ctrl = SHELL_CTRL_CONTINUE;
        goto exit;
    }
    if ((inputChar == '\r') || (inputChar == '\n')) {
#if !(defined(__linux) || defined(_WIN32))
        __platform_printf("\r\n");
#endif
        /* still in block */
        if (shell->blockBuffName != NULL && shell->inBlock) {
            /* load new line into buff */
            Args buffs = {0};
            char _n = '\n';
            strAppendWithSize(shell->lineBuff, &_n, 1);
            char* shell_buff_new =
                strsAppend(&buffs, obj_getStr(self, shell->blockBuffName),
                           shell->lineBuff);
            obj_setStr(self, shell->blockBuffName, shell_buff_new);
            strsDeinit(&buffs);
            /* go out from block */
            if ((shell->lineBuff[0] != ' ') && (shell->lineBuff[0] != '\t')) {
                shell->inBlock = PIKA_FALSE;
                input_line = obj_getStr(self, shell->blockBuffName);
                ctrl = shell->handler(self, input_line, shell);
                __clearBuff(shell);
                __platform_printf(">>> ");
                goto exit;
            } else {
                __platform_printf("... ");
            }
            __clearBuff(shell);
            ctrl = SHELL_CTRL_CONTINUE;
            goto exit;
        }
        /* go in block */
        if (shell->blockBuffName != NULL && 0 != strGetSize(shell->lineBuff)) {
            if (shell->lineBuff[strGetSize(shell->lineBuff) - 1] == ':') {
                shell->inBlock = PIKA_TRUE;
                char _n = '\n';
                strAppendWithSize(shell->lineBuff, &_n, 1);
                obj_setStr(self, shell->blockBuffName, shell->lineBuff);
                __clearBuff(shell);
                __platform_printf("... ");
                ctrl = SHELL_CTRL_CONTINUE;
                goto exit;
            }
        }
        shell->lineBuff[shell->line_position] = '\0';
        ctrl = shell->handler(self, shell->lineBuff, shell);
        __platform_printf("%s", shell->prefix);
        __clearBuff(shell);
        goto exit;
    }
exit:
    shell->lastChar = inputChar;
    return ctrl;
}

enum shellCTRL obj_runChar(PikaObj* self, char inputChar) {
    ShellConfig* shell = args_getStruct(self->list, "@shcfg");
    if (NULL == shell) {
        /* init the shell */
        ShellConfig newShell = {
            .prefix = ">>> ",
            .blockBuffName = "@sh1",
            .handler = __obj_shellLineHandler_REPL,
        };
        args_setStruct(self->list, "@shcfg", newShell);
        shell = args_getStruct(self->list, "@shcfg");
        _obj_runChar_beforeRun(self, shell);
    }
    return _do_obj_runChar(self, inputChar, shell);
}

void _do_pikaScriptShell(PikaObj* self, ShellConfig* cfg) {
    /* init the shell */
    _obj_runChar_beforeRun(self, cfg);

    /* getchar and run */
    char inputChar[2] = {0};
    while (1) {
        inputChar[1] = inputChar[0];
        inputChar[0] = cfg->fn_getchar();
#if !PIKA_NANO_ENABLE
        /* run python script */
        if (inputChar[0] == '!' && inputChar[1] == '#') {
            /* #! xxx */
            /* start */
            char* buff = pikaMalloc(PIKA_READ_FILE_BUFF_SIZE);
            char input[2] = {0};
            int buff_i = 0;
            PIKA_BOOL is_exit = PIKA_FALSE;
            PIKA_BOOL is_first_line = PIKA_TRUE;
            while (1) {
                input[1] = input[0];
                input[0] = cfg->fn_getchar();
                if (input[0] == '!' && input[1] == '#') {
                    buff[buff_i - 1] = 0;
                    for (int i = 0; i < 4; i++) {
                        /* eat 'pika' */
                        cfg->fn_getchar();
                    }
                    break;
                }
                if ('\r' == input[0]) {
                    continue;
                }
                if (is_first_line) {
                    if ('\n' == input[0]) {
                        is_first_line = PIKA_FALSE;
                    }
                    continue;
                }
                buff[buff_i++] = input[0];
            }
            /* end */
            __platform_printf("\r\n=============== [Code] ===============\r\n");
            size_t len = strGetSize(buff);
            for (size_t i = 0; i < len; i++) {
                if (buff[i] == '\r') {
                    continue;
                }
                if (buff[i] == '\n') {
                    __platform_printf("\r\n");
                    continue;
                }
                __platform_printf("%c", buff[i]);
            }
            __platform_printf("\r\n");
            __platform_printf("=============== [File] ===============\r\n");
            __platform_printf("[   Info] File buff used: %d/%d (%0.2f%%)\r\n",
                              (int)len, (int)PIKA_READ_FILE_BUFF_SIZE,
                              ((float)len / (float)PIKA_READ_FILE_BUFF_SIZE));
#if PIKA_SHELL_SAVE_FILE_ENABLE
            char* file_name = PIKA_SHELL_SAVE_FILE_NAME;
            __platform_printf("[   Info] Saving file to '%s'...\r\n",
                              file_name);
            FILE* fp = __platform_fopen(file_name, "w+");
            if (NULL == fp) {
                __platform_printf("[  Error] Open file '%s' error!\r\n",
                                  file_name);
                __platform_fclose(fp);
            } else {
                __platform_fwrite(buff, 1, len, fp);
                __platform_printf("[   Info] Writing %d bytes to '%s'...\r\n",
                                  (int)(len), file_name);
                __platform_fclose(fp);
                __platform_printf("[    OK ] Writing to '%s' succeed!\r\n",
                                  file_name);
            }
#endif
            __platform_printf("=============== [ Run] ===============\r\n");
            obj_run(self, (char*)buff);
            if (NULL != strstr(buff, "exit()")) {
                is_exit = PIKA_TRUE;
            }
            pikaFree(buff, PIKA_READ_FILE_BUFF_SIZE);
            if (is_exit) {
                return;
            }
            __platform_printf("%s", cfg->prefix);
            continue;
        }

        /* run xx.py.o */
        if (inputChar[0] == 'p' && inputChar[1] == 0x0f) {
            for (int i = 0; i < 2; i++) {
                /* eat 'yo' */
                cfg->fn_getchar();
            }
            uint32_t size = 0;
            for (int i = 0; i < 4; i++) {
                uint8_t* size_byte = (uint8_t*)&size;
                size_byte[i] = cfg->fn_getchar();
            }
            uint8_t* buff = pikaMalloc(size);
            for (uint32_t i = 0; i < size; i++) {
                buff[i] = cfg->fn_getchar();
            }
            __platform_printf("\r\n=============== [Code] ===============\r\n");
            __platform_printf("[   Info] Bytecode size: %d\r\n", size);
            __platform_printf("=============== [ RUN] ===============\r\n");
            pikaVM_runByteCodeInconstant(self, buff);
            pikaFree(buff, size);
            return;
        }
#endif
        if (SHELL_CTRL_EXIT == _do_obj_runChar(self, inputChar[0], cfg)) {
            break;
        }
    }
}

void _temp__do_pikaScriptShell(PikaObj* self, ShellConfig* shell) {
    /* init the shell */
    _obj_runChar_beforeRun(self, shell);

    /* getchar and run */
    while (1) {
        char inputChar = shell->fn_getchar();
        if (SHELL_CTRL_EXIT == _do_obj_runChar(self, inputChar, shell)) {
            break;
        }
    }
}

static enum shellCTRL __obj_shellLineHandler_REPL(PikaObj* self,
                                                  char* input_line,
                                                  ShellConfig* shell) {
    /* exit */
    if (strEqu("exit()", input_line)) {
        /* exit pika shell */
        return SHELL_CTRL_EXIT;
    }
    /* run single line */
    obj_run(self, input_line);
    return SHELL_CTRL_CONTINUE;
}

static volatile ShellConfig g_repl_shell = {
    .handler = __obj_shellLineHandler_REPL,
    .prefix = ">>> ",
    .blockBuffName = "@sh0",
};

void pikaScriptShell_withGetchar(PikaObj* self, sh_getchar getchar_fn) {
    g_repl_shell.fn_getchar = getchar_fn;
    _do_pikaScriptShell(self, (ShellConfig*)&g_repl_shell);
}

void pikaScriptShell(PikaObj* self) {
    pikaScriptShell_withGetchar(self, __platform_getchar);
}

void obj_setErrorCode(PikaObj* self, int32_t errCode) {
    obj_setInt(self, "__errCode", errCode);
}

int32_t obj_getErrorCode(PikaObj* self) {
    if (!obj_isArgExist(self, "__errCode")) {
        return 0;
    }
    return obj_getInt(self, "__errCode");
}

int32_t args_getErrorCode(Args* args) {
    if (!args_isArgExist(args, "__errCode")) {
        return 0;
    }
    return args_getInt(args, "__errCode");
}

void obj_setSysOut(PikaObj* self, char* str) {
    args_setSysOut(self->list, str);
}

char* obj_getSysOut(PikaObj* self) {
    return obj_getStr(self, "__sysOut");
}

void args_setSysOut(Args* args, char* str) {
    // args_setStr(args, "__sysOut", str);
    if (NULL == str) {
        return;
    }
    if (strEqu("", str)) {
        return;
    }
    __platform_printf("%s\r\n", str);
}

void method_returnBytes(Args* args, uint8_t* val) {
    args_pushArg_name(args, "@rt", arg_newBytes(val, PIKA_BYTES_DEFAULT_SIZE));
}

void method_returnStr(Args* args, char* val) {
    args_pushArg_name(args, "@rt", arg_newStr(val));
}

void method_returnInt(Args* args, int64_t val) {
    args_pushArg_name(args, "@rt", arg_newInt(val));
}

void method_returnFloat(Args* args, pika_float val) {
    args_pushArg_name(args, "@rt", arg_newFloat(val));
}

void method_returnPtr(Args* args, void* val) {
    args_pushArg_name(args, "@rt", arg_newPtr(ARG_TYPE_POINTER, val));
}

void method_returnObj(Args* args, void* val) {
    if (NULL == val) {
        args_pushArg_name(args, "@rt", arg_newNull());
        return;
    }
    ArgType type;
    PikaObj* obj = (PikaObj*)val;
    if (obj_getFlag(obj, OBJ_FLAG_ALREADY_INIT)) {
        type = ARG_TYPE_OBJECT;
    } else {
        type = ARG_TYPE_OBJECT_NEW;
    }
    args_pushArg_name(args, "@rt", arg_newPtr(type, val));
}

void method_returnArg(Args* args, Arg* arg) {
    args_pushArg_name(args, "@rt", arg);
}

int64_t method_getInt(Args* args, char* argName) {
    return args_getInt(args, argName);
}

pika_float method_getFloat(Args* args, char* argName) {
    return args_getFloat(args, argName);
}

char* method_getStr(Args* args, char* argName) {
    return args_getStr(args, argName);
}

PikaObj* New_PikaObj(void) {
    PikaObj* self = pikaMalloc(sizeof(PikaObj));
    /* List */
    self->list = New_args(NULL);
    self->refcnt = 0;
    self->constructor = NULL;
    self->flag = 0;
    return self;
}

Arg* arg_setObj(Arg* self, char* name, PikaObj* obj) {
    return arg_setPtr(self, name, ARG_TYPE_OBJECT, obj);
}

Arg* arg_setRef(Arg* self, char* name, PikaObj* obj) {
    pika_assert(NULL != obj);
    obj_refcntInc(obj);
    return arg_setObj(self, name, obj);
}

int32_t obj_newDirectObj(PikaObj* self, char* objName, NewFun newFunPtr) {
    Arg* new_obj = arg_newDirectObj(newFunPtr);
    new_obj = arg_setName(new_obj, objName);
    arg_setType(new_obj, ARG_TYPE_OBJECT);
    args_setArg(self->list, new_obj);
    return 0;
}

int32_t obj_newMetaObj(PikaObj* self, char* objName, NewFun newFunPtr) {
    /* add meta Obj, no inited */
    Arg* new_obj = arg_newMetaObj(newFunPtr);
    new_obj = arg_setName(new_obj, objName);
    args_setArg(self->list, new_obj);
    return 0;
}

static void _append_help(char* name) {
    if (NULL == _help_modules_cmodule) {
        _help_modules_cmodule = (volatile Arg*)arg_newStr("");
    }
    Arg* _help = (Arg*)_help_modules_cmodule;
    _help = arg_strAppend(_help, name);
    _help = arg_strAppend(_help, "\r\n");
    _help_modules_cmodule = (volatile Arg*)_help;
}

int32_t obj_newObj(PikaObj* self,
                   char* objName,
                   char* className,
                   NewFun newFunPtr) {
    /* before init root object */
    if (in_root_obj) {
        _append_help(objName);
    }
    return obj_newMetaObj(self, objName, newFunPtr);
}

PikaObj* obj_importModuleWithByteCode(PikaObj* self,
                                      char* name,
                                      uint8_t* byteCode) {
    PikaObj* New_PikaStdLib_SysObj(Args * args);
    if (!obj_isArgExist((PikaObj*)__pikaMain, name)) {
        obj_newDirectObj((PikaObj*)__pikaMain, name, New_PikaStdLib_SysObj);
        pikaVM_runByteCode(obj_getObj((PikaObj*)__pikaMain, name),
                           (uint8_t*)byteCode);
    }
    if (self != (PikaObj*)__pikaMain) {
        Arg* module_arg = obj_getArg((PikaObj*)__pikaMain, name);
        PikaObj* module_obj = arg_getPtr(module_arg);
        obj_setArg(self, name, module_arg);
        arg_setIsWeakRef(obj_getArg(self, name), PIKA_TRUE);
        pika_assert(argType_isObject(arg_getType(module_arg)));
        /* decrase refcnt to avoid circle reference */
        obj_refcntDec(module_obj);
    }
    return self;
}

PikaObj* obj_importModuleWithByteCodeFrame(PikaObj* self,
                                           char* name,
                                           ByteCodeFrame* byteCode_frame) {
    PikaObj* New_PikaStdLib_SysObj(Args * args);
    obj_newDirectObj(self, name, New_PikaStdLib_SysObj);
    pikaVM_runByteCodeFrame(obj_getObj(self, name), byteCode_frame);
    return self;
}

PikaObj* Obj_linkLibraryFile(PikaObj* self, char* input_file_name) {
    obj_newMetaObj(self, "@lib", New_LibObj);
    LibObj* lib = obj_getObj(self, "@lib");
    LibObj_loadLibraryFile(lib, input_file_name);
    return self;
}

PikaObj* obj_linkLibrary(PikaObj* self, uint8_t* library_bytes) {
    obj_newMetaObj(self, "@lib", New_LibObj);
    LibObj* lib = obj_getObj(self, "@lib");
    LibObj_loadLibrary(lib, library_bytes);
    obj_setPtr(self, "@libraw", library_bytes);
    return self;
}

void obj_printModules(PikaObj* self) {
    LibObj* lib = obj_getObj(self, "@lib");
    __platform_printf(arg_getStr((Arg*)_help_modules_cmodule));
    LibObj_printModules(lib);
}

PikaObj* obj_linkLibObj(PikaObj* self, LibObj* library) {
    obj_setPtr(self, "@lib", library);
    return self;
}

uint8_t* obj_getByteCodeFromModule(PikaObj* self, char* module_name) {
    /* exit when no found '@lib' */
    if (!obj_isArgExist(self, "@lib")) {
        return NULL;
    }
    /* find module from the library */
    LibObj* lib = obj_getPtr(self, "@lib");
    PikaObj* module = obj_getObj(lib, module_name);
    /* exit when no module in '@lib' */
    if (NULL == module) {
        return NULL;
    }
    return obj_getPtr(module, "bytecode");
}

int obj_runModule(PikaObj* self, char* module_name) {
    uint8_t* bytecode = obj_getByteCodeFromModule(self, module_name);
    if (NULL == bytecode) {
        return 1;
    }
    pikaVM_runByteCode(self, bytecode);
    return 0;
}

int obj_importModule(PikaObj* self, char* module_name) {
    /* import bytecode of the module */
    uint8_t* bytecode =
        obj_getByteCodeFromModule((PikaObj*)__pikaMain, module_name);
    if (NULL == bytecode) {
        return 1;
    }
    obj_importModuleWithByteCode(self, module_name, bytecode);
    return 0;
}

char* obj_toStr(PikaObj* self) {
    /* clang-format off */
    PIKA_PYTHON(
        __res = __str__()
    )
    /* clang-format on */

    /* check method arg */
    Arg* method_arg = obj_getMethodArg(self, "__str__");
    if (NULL != method_arg) {
        arg_deinit(method_arg);
        const uint8_t bytes[] = {
            0x08, 0x00, 0x00, 0x00, /* instruct array size */
            0x00, 0x82, 0x01, 0x00, 0x00, 0x04, 0x09, 0x00, /* instruct
                                                               array */
            0x0f, 0x00, 0x00, 0x00, /* const pool size */
            0x00, 0x5f, 0x5f, 0x73, 0x74, 0x72, 0x5f, 0x5f, 0x00,
            0x5f, 0x5f, 0x72, 0x65, 0x73, 0x00, /* const pool */
        };
        pikaVM_runByteCode(self, (uint8_t*)bytes);
        char* str_res = obj_getStr(self, "__res");
        return str_res;
    }

    /* normal object */
    Args buffs = {0};
    char* str_res =
        strsFormat(&buffs, PIKA_SPRINTF_BUFF_SIZE, "<object at %p>", self);
    obj_setStr(self, "__res", str_res);
    strsDeinit(&buffs);
    return obj_getStr(self, "__res");
}

void pks_eventLicener_registEvent(PikaEventListener* self,
                                  uint32_t eventId,
                                  PikaObj* eventHandleObj) {
    Args buffs = {0};
    char* event_name =
        strsFormat(&buffs, PIKA_SPRINTF_BUFF_SIZE, "%ld", eventId);
    obj_newDirectObj(self, event_name, New_TinyObj);
    PikaObj* event_item = obj_getObj(self, event_name);
    obj_setRef(event_item, "eventHandleObj", eventHandleObj);
    strsDeinit(&buffs);
}

void pks_eventLicener_removeEvent(PikaEventListener* self, uint32_t eventId) {
    Args buffs = {0};
    char* event_name =
        strsFormat(&buffs, PIKA_SPRINTF_BUFF_SIZE, "%ld", eventId);
    obj_removeArg(self, event_name);
    strsDeinit(&buffs);
}

PikaObj* pks_eventLisener_getEventHandleObj(PikaEventListener* self,
                                            uint32_t eventId) {
    Args buffs = {0};
    char* event_name =
        strsFormat(&buffs, PIKA_SPRINTF_BUFF_SIZE, "%ld", eventId);
    PikaObj* event_item = obj_getObj(self, event_name);
    PikaObj* eventHandleObj = obj_getPtr(event_item, "eventHandleObj");
    strsDeinit(&buffs);
    return eventHandleObj;
}

void pks_eventLisener_init(PikaEventListener** p_self) {
    *p_self = newNormalObj(New_TinyObj);
}

void pks_eventLisener_deinit(PikaEventListener** p_self) {
    if (NULL != *p_self) {
        obj_deinit(*p_self);
        *p_self = NULL;
    }
}

Arg* __eventLisener_runEvent(PikaEventListener* lisener,
                             uint32_t eventId,
                             int eventSignal) {
    PikaObj* handler = pks_eventLisener_getEventHandleObj(lisener, eventId);
    if (NULL == handler) {
        __platform_printf(
            "Error: can not find event handler by id: [0x%02x]\r\n", eventId);
        return NULL;
    }
    obj_setInt(handler, "eventSignal", eventSignal);
    /* clang-format off */
    PIKA_PYTHON(
    _res = eventCallBack(eventSignal)
    )
    /* clang-format on */
    const uint8_t bytes[] = {
        0x0c, 0x00, 0x00, 0x00, /* instruct array size */
        0x10, 0x81, 0x01, 0x00, 0x00, 0x02, 0x0d, 0x00, 0x00, 0x04, 0x1b, 0x00,
        /* instruct array */
        0x20, 0x00, 0x00, 0x00, /* const pool size */
        0x00, 0x65, 0x76, 0x65, 0x6e, 0x74, 0x53, 0x69, 0x67, 0x6e, 0x61, 0x6c,
        0x00, 0x65, 0x76, 0x65, 0x6e, 0x74, 0x43, 0x61, 0x6c, 0x6c, 0x42, 0x61,
        0x63, 0x6b, 0x00, 0x5f, 0x72, 0x65, 0x73, 0x00, /* const pool */
    };
    pikaVM_runByteCode(handler, (uint8_t*)bytes);
    Arg* res = obj_getArg(handler, "_res");
    res = arg_copy(res);
    obj_removeArg(handler, "_res");
    return res;
}

void pks_eventLisener_sendSignal(PikaEventListener* self,
                                 uint32_t eventId,
                                 int eventSignal) {
#if !PIKA_EVENT_ENABLE
    __platform_printf("PIKA_EVENT_ENABLE is not enable");
    while (1) {
    };
#else
    /* push event handler to vm event list */
    if (PIKA_RES_OK != VMSignal_pushEvent(self, eventId, eventSignal)) {
        __platform_printf(
            "OverflowError: event list is full, please use bigger "
            "PIKA_EVENT_LIST_SIZE\r\n");
        while (1) {
        }
    }
    if (0 == VMSignal_getVMCnt()) {
        /* no vm running, pick up event imediately */
        VMSignale_pickupEvent();
    }
#endif
}

Arg* pks_eventLisener_sendSignalAwaitResult(PikaEventListener* self,
                                            uint32_t eventId,
                                            int eventSignal) {
    /*
     * Await result from event.
     * need implement `__platform_thread_delay()` to support thread switch */
#if !PIKA_EVENT_ENABLE
    __platform_printf("PIKA_EVENT_ENABLE is not enable");
    while (1) {
    };
#else
    extern volatile VMSignal PikaVMSignal;
    int tail = PikaVMSignal.cq.tail;
    pks_eventLisener_sendSignal(self, eventId, eventSignal);
    while (1) {
        Arg* res = PikaVMSignal.cq.res[tail];
        __platform_thread_delay();
        if (NULL != res) {
            return res;
        }
    }
#endif
}

/* print major version info */
void pks_printVersion(void) {
    __platform_printf("pikascript-core==v%d.%d.%d (%s)\r\n", PIKA_VERSION_MAJOR,
                      PIKA_VERSION_MINOR, PIKA_VERSION_MICRO, PIKA_EDIT_TIME);
}

void pks_getVersion(char* buff) {
    __platform_sprintf(buff, "%d.%d.%d", PIKA_VERSION_MAJOR, PIKA_VERSION_MINOR,
                       PIKA_VERSION_MICRO);
}

void* obj_getStruct(PikaObj* self, char* name) {
    return args_getStruct(self->list, name);
}

char* obj_cacheStr(PikaObj* self, char* str) {
    return args_cacheStr(self->list, str);
}

void _obj_updateProxyFlag(PikaObj* self) {
    if (!obj_getFlag(self, OBJ_FLAG_PROXY_GETATTRIBUTE)) {
        if (NULL != _obj_getProp(self, "__getattribute__")) {
            obj_setFlag(self, OBJ_FLAG_PROXY_GETATTRIBUTE);
        }
    }
    if (!obj_getFlag(self, OBJ_FLAG_PROXY_GETATTR)) {
        if (NULL != _obj_getProp(self, "__getattr__")) {
            obj_setFlag(self, OBJ_FLAG_PROXY_GETATTR);
        }
    }
    if (!obj_getFlag(self, OBJ_FLAG_PROXY_SETATTR)) {
        if (NULL != _obj_getProp(self, "__setattr__")) {
            obj_setFlag(self, OBJ_FLAG_PROXY_SETATTR);
        }
    }
};
