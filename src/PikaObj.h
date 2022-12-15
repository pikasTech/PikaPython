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

#ifndef _Process__H
#define _Process__H

#include "dataArgs.h"
#include "dataLink.h"
#include "dataMemory.h"
#include "dataStrs.h"

typedef struct InstructUnit InstructUnit;
struct InstructUnit {
    uint8_t deepth;
    uint8_t isNewLine_instruct;
    uint16_t const_pool_index;
};

typedef struct ConstPool ConstPool;
struct ConstPool {
    Arg* arg_buff;
    uint32_t content_offset_now;
    uint32_t size;
    void* content_start;
    void (*output_redirect_fun)(ConstPool* self, char* content);
    FILE* output_f;
};

typedef struct InstructArray InstructArray;
struct InstructArray {
    Arg* arg_buff;
    uint32_t content_offset_now;
    uint32_t size;
    void* content_start;
    void (*output_redirect_fun)(InstructArray* self, InstructUnit* ins_unit);
    FILE* output_f;
};

typedef struct ByteCodeFrame ByteCodeFrame;
struct ByteCodeFrame {
    ConstPool const_pool;
    InstructArray instruct_array;
};

typedef struct NativeProperty NativeProperty;
struct NativeProperty {
    const NativeProperty* super;
    const Arg* methodGroup;
    uint32_t methodGroupCount;
};

typedef struct PikaObj PikaObj;
struct PikaObj {
    Args* list;
    uint8_t refcnt;
    void* constructor;
    uint8_t flag;
};

typedef struct RangeData RangeData;
struct RangeData {
    int64_t start;
    int64_t end;
    int64_t step;
    int64_t i;
};

#define OBJ_FLAG_PROXY_GETATTRIBUTE 0x01
#define OBJ_FLAG_PROXY_GETATTR 0x02
#define OBJ_FLAG_PROXY_SETATTR 0x04
#define OBJ_FLAG_ALREADY_INIT 0x08
#define OBJ_FLAG_RUN_AS 0x16
#define OBJ_FLAG_GLOBALS 0x32

#define KEY_UP 0x41
#define KEY_DOWN 0x42
#define KEY_RIGHT 0x43
#define KEY_LEFT 0x44

static inline uint8_t obj_getFlag(PikaObj* self, uint8_t flag) {
    return (self->flag & flag) == flag;
}

static inline void obj_setFlag(PikaObj* self, uint8_t flag) {
    self->flag |= flag;
}

static inline void obj_clearFlag(PikaObj* self, uint8_t flag) {
    self->flag &= ~flag;
}

typedef PikaObj* (*NewFun)(Args* args);
typedef PikaObj* (*InitFun)(PikaObj* self, Args* args);
typedef PikaObj VMParameters;
typedef void (*Method)(PikaObj* self, Args* args);

typedef struct MethodInfo MethodInfo;
struct MethodInfo {
    char* name;
    char* dec;
    char* ptr;
    char* typelist;
    PikaObj* def_context;
    ArgType type;
    ByteCodeFrame* bytecode_frame;
};

typedef struct MethodProp {
    void* ptr;
    char* type_list;
    char* name;
    ByteCodeFrame* bytecode_frame;
    PikaObj* def_context;
    char* declareation;
} MethodProp;

typedef struct MethodPropNative {
    void* ptr;
    char* type_list;
#if !PIKA_NANO_ENABLE
    char* name;
#endif
} MethodPropNative;

typedef PikaObj LibObj;
typedef PikaObj PikaMaker;

/* operation */
int32_t obj_deinit(PikaObj* self);
int32_t obj_init(PikaObj* self, Args* args);
int32_t obj_update(PikaObj* self);
int32_t obj_enable(PikaObj* self);
int32_t obj_disable(PikaObj* self);

// arg type operations
PIKA_RES obj_setInt(PikaObj* self, char* argPath, int64_t val);
PIKA_RES obj_setRef(PikaObj* self, char* argPath, PikaObj* pointer);
PIKA_RES obj_setPtr(PikaObj* self, char* argPath, void* pointer);
PIKA_RES obj_setFloat(PikaObj* self, char* argPath, pika_float value);
PIKA_RES obj_setStr(PikaObj* self, char* argPath, char* str);
PIKA_RES obj_setNone(PikaObj* self, char* argPath);
PIKA_RES obj_setArg(PikaObj* self, char* argPath, Arg* arg);
PIKA_RES obj_setArg_noCopy(PikaObj* self, char* argPath, Arg* arg);
PIKA_RES obj_setBytes(PikaObj* self, char* argPath, uint8_t* src, size_t size);

void* obj_getPtr(PikaObj* self, char* argPath);
pika_float obj_getFloat(PikaObj* self, char* argPath);
char* obj_getStr(PikaObj* self, char* argPath);
int64_t obj_getInt(PikaObj* self, char* argPath);
Arg* obj_getArg(PikaObj* self, char* argPath);
uint8_t* obj_getBytes(PikaObj* self, char* argPath);
size_t obj_getBytesSize(PikaObj* self, char* argPath);
size_t obj_loadBytes(PikaObj* self, char* argPath, uint8_t* out_buff);

char* obj_print(PikaObj* self, char* name);

// args operations
int32_t obj_load(PikaObj* self, Args* args, char* name);

// subObject
int32_t obj_addOther(PikaObj* self, char* subObjectName, void* new_projcetFun);
PikaObj* obj_getObj(PikaObj* self, char* objPath);
PikaObj* obj_getHostObj(PikaObj* self, char* objPath);
PikaObj* obj_getHostObjWithIsTemp(PikaObj* self,
                                  char* objPath,
                                  PIKA_BOOL* pIsClass);

// subProcess
int32_t obj_freeObj(PikaObj* self, char* subObjectName);

/* method */
int32_t class_defineMethod(PikaObj* self,
                           char* name,
                           char* typelist,
                           Method methodPtr);

int32_t class_defineObjectMethod(PikaObj* self,
                                 char* declareation,
                                 Method methodPtr,
                                 PikaObj* def_context,
                                 ByteCodeFrame* bytecode_frame);

int32_t class_defineStaticMethod(PikaObj* self,
                                 char* declareation,
                                 Method methodPtr,
                                 PikaObj* def_context,
                                 ByteCodeFrame* bytecode_frame);

int32_t class_defineConstructor(PikaObj* self,
                                char* name,
                                char* typelist,
                                Method methodPtr);

int32_t class_defineRunTimeConstructor(PikaObj* self,
                                       char* declareation,
                                       Method methodPtr,
                                       PikaObj* def_context,
                                       ByteCodeFrame* bytecode_frame);

int32_t obj_removeArg(PikaObj* self, char* argPath);
int32_t obj_isArgExist(PikaObj* self, char* argPath);
PikaObj* obj_getClassObjByNewFun(PikaObj* self, char* name, NewFun newClassFun);
PikaObj* newRootObj(char* name, NewFun newObjFun);
PikaObj* obj_getClassObj(PikaObj* obj);
Arg* obj_getMethodArg(PikaObj* obj, char* methodPath);
Arg* obj_getMethodArg_noalloc(PikaObj* obj, char* methodPath, Arg* arg_reg);

void obj_setErrorCode(PikaObj* self, int32_t errCode);
int32_t obj_getErrorCode(PikaObj* self);
void obj_setSysOut(PikaObj* self, char* str);
char* args_getSysOut(Args* args);
void args_setErrorCode(Args* args, int32_t errCode);
int32_t args_getErrorCode(Args* args);
void args_setSysOut(Args* args, char* str);
char* obj_getSysOut(PikaObj* self);
void obj_sysPrintf(PikaObj* self, char* fmt, ...);
uint8_t obj_getAnyArg(PikaObj* self,
                      char* targetArgName,
                      char* sourceArgPath,
                      Args* targetArgs);

void method_returnStr(Args* args, char* val);
void method_returnInt(Args* args, int64_t val);
void method_returnFloat(Args* args, pika_float val);
void method_returnPtr(Args* args, void* val);
void method_returnObj(Args* args, void* val);
int64_t method_getInt(Args* args, char* argName);
pika_float method_getFloat(Args* args, char* argName);
char* method_getStr(Args* args, char* argName);
void method_returnArg(Args* args, Arg* arg);
char* methodArg_getDec(Arg* method_arg);
char* methodArg_getTypeList(Arg* method_arg, char* buffs, size_t size);
char* methodArg_getName(Arg* method_arg, char* buffs, size_t size);
ByteCodeFrame* methodArg_getBytecodeFrame(Arg* method_arg);
Method methodArg_getPtr(Arg* method_arg);

VMParameters* obj_run(PikaObj* self, char* cmd);
PikaObj* New_PikaObj(void);

/* tools */
int64_t fast_atoi(char* src);
char* fast_itoa(char* buf, uint32_t val);

/* shell */
void pikaScriptShell(PikaObj* self);
enum shellCTRL { SHELL_CTRL_CONTINUE, SHELL_CTRL_EXIT };

typedef struct ShellConfig ShellConfig;
typedef enum shellCTRL (*sh_handler)(PikaObj*, char*, ShellConfig*);
typedef char (*sh_getchar)(void);

struct ShellConfig {
    char* prefix;
    sh_handler handler;
    void* context;
    char lineBuff[PIKA_LINE_BUFF_SIZE];
    size_t line_position;
    size_t line_curpos;
    char* blockBuffName;
    PIKA_BOOL inBlock;
    char lastChar;
    sh_getchar fn_getchar;
    uint8_t stat;
};

void _do_pikaScriptShell(PikaObj* self, ShellConfig* cfg);

void _temp__do_pikaScriptShell(PikaObj* self, ShellConfig* cfg);

/*
    need implament :
        __platform_fopen()
        __platform_fwrite()
        __platform_fclose()
*/
Method obj_getNativeMethod(PikaObj* self, char* method_name);
PIKA_RES obj_runNativeMethod(PikaObj* self, char* method_name, Args* args);
Arg* obj_newObjInPackage(NewFun newObjFun);

PikaObj* newNormalObj(NewFun newObjFun);
Arg* arg_setRef(Arg* self, char* name, PikaObj* obj);
Arg* arg_setObj(Arg* self, char* name, PikaObj* obj);

static inline Arg* arg_newObj(PikaObj* obj) {
    return arg_setObj(NULL, "", (obj));
}

static inline Arg* arg_newRef(PikaObj* obj) {
    return arg_setRef(NULL, "", (obj));
}

PikaObj* obj_importModuleWithByteCodeFrame(PikaObj* self,
                                           char* name,
                                           ByteCodeFrame* bytecode_frame);
PikaObj* obj_importModuleWithByteCode(PikaObj* self,
                                      char* name,
                                      uint8_t* byteCode);

int32_t obj_newObj(PikaObj* self,
                   char* objName,
                   char* className,
                   NewFun newFunPtr);

Arg* arg_newMetaObj(NewFun objPtr);
PikaObj* obj_linkLibObj(PikaObj* self, LibObj* library);
PikaObj* obj_linkLibrary(PikaObj* self, uint8_t* library_bytes);
int obj_importModule(PikaObj* self, char* module_name);
int32_t obj_newMetaObj(PikaObj* self, char* objName, NewFun newFunPtr);
int32_t obj_newDirectObj(PikaObj* self, char* objName, NewFun newFunPtr);
int obj_runModule(PikaObj* self, char* module_name);
char* obj_toStr(PikaObj* self);
Arg* arg_newDirectObj(NewFun new_obj_fun);
enum shellCTRL obj_runChar(PikaObj* self, char inputChar);

#define PIKA_PYTHON_BEGIN
#define PIKA_PYTHON(x)
#define PIKA_PYTHON_END

typedef PikaObj PikaEventListener;

void pks_eventLisener_sendSignal(PikaEventListener* self,
                                 uint32_t eventId,
                                 int eventSignal);

void pks_eventLicener_registEvent(PikaEventListener* self,
                                  uint32_t eventId,
                                  PikaObj* eventHandleObj);

void pks_eventLicener_removeEvent(PikaEventListener* self, uint32_t eventId);

PikaObj* pks_eventLisener_getEventHandleObj(PikaEventListener* self,
                                            uint32_t eventId);

void pks_eventLisener_init(PikaEventListener** p_self);
void pks_eventLisener_deinit(PikaEventListener** p_self);
PikaObj* methodArg_getDefContext(Arg* method_arg);
PikaObj* Obj_linkLibraryFile(PikaObj* self, char* input_file_name);
NewFun obj_getClass(PikaObj* obj);

void pks_printVersion(void);
void pks_getVersion(char* buff);
void* obj_getStruct(PikaObj* self, char* name);

static inline void obj_refcntDec(PikaObj* self) {
    self->refcnt--;
}

static inline void obj_refcntInc(PikaObj* self) {
    self->refcnt++;
}

static inline uint8_t obj_refcntNow(PikaObj* self) {
    return self->refcnt;
}

#define obj_setStruct(PikaObj_p_self, char_p_name, struct_) \
    args_setStruct(((PikaObj_p_self)->list), char_p_name, struct_)

#define ABSTRACT_METHOD_NEED_OVERRIDE_ERROR(_)                            \
    obj_setErrorCode(self, 1);                                            \
    __platform_printf("Error: abstract method `%s()` need override.\r\n", \
                      __FUNCTION__)

#define WEAK_FUNCTION_NEED_OVERRIDE_ERROR(_)                            \
    __platform_printf("Error: weak function `%s()` need override.\r\n", \
                      __FUNCTION__);                                    \
    while (1)

char* obj_cacheStr(PikaObj* self, char* str);
PikaObj* _arg_to_obj(Arg* self, PIKA_BOOL* pIsTemp);
char* __printBytes(PikaObj* self, Arg* arg);

#define PIKASCRIPT_VERSION_TO_NUM(majer, minor, micro) \
    majer * 100 * 100 + minor * 100 + micro

#define PIKASCRIPT_VERSION_NUM                                        \
    PIKASCRIPT_VERSION_TO_NUM(PIKA_VERSION_MAJOR, PIKA_VERSION_MINOR, \
                              PIKA_VERSION_MICRO)

#define PIKASCRIPT_VERSION_REQUIRE_MINIMUN(majer, minor, micro) \
    (PIKASCRIPT_VERSION_NUM >= PIKASCRIPT_VERSION_TO_NUM(majer, minor, micro))

/* [example]
const MethodProp floatMethod = {
    .ptr = (void*)PikaStdLib_SysObj_floatMethod,
    .bytecode_frame = NULL,
    .def_context = NULL,
    .declareation = "float(arg)",
    .type_list = "arg",
    .name = "float",
};
*/

#if !PIKA_NANO_ENABLE
#define method_typedef(_method, _name, _typelist) \
    const MethodPropNative _method##Prop = {      \
        .ptr = (void*)_method##Method,            \
        .type_list = _typelist,                   \
        .name = _name,                            \
    };
#else
#define method_typedef(_method, _name, _typelist) \
    const MethodPropNative _method##Prop = {      \
        .ptr = (void*)_method##Method,            \
        .type_list = _typelist,                   \
    };
#endif

/* clang-format off */
#if PIKA_ARG_CACHE_ENABLE
#define _method_def(_method, _hash, _type)           \
    {                                               \
        ._ =                                        \
            {                                       \
                .buffer = (uint8_t*)&_method##Prop  \
            },                                      \
        .size = sizeof(MethodPropNative),                 \
        .heap_size = 0,                             \
        .type = _type,                              \
        .flag = 0,                                  \
        .name_hash = _hash                          \
    }
#else
#define _method_def(_method, _hash, _type)          \
    {                                               \
        ._ =                                        \
            {                                       \
                .buffer = (uint8_t*)&_method##Prop  \
            },                                      \
        .size = sizeof(MethodPropNative),                 \
        .type = _type,                              \
        .flag = 0,                                  \
        .name_hash = _hash                          \
    }
#endif

#if defined(_WIN32) || \
    (defined(__ARMCC_VERSION) && (__ARMCC_VERSION < 6000000))
#define __BEFORE_MOETHOD_DEF                        \
    {                                               \
        ._ =                                        \
            {                                       \
                .buffer = NULL                      \
            },                                      \
        .size = 0,                                  \
        .type = ARG_TYPE_NONE,                      \
        .flag = 0,                                  \
        .name_hash = 0                              \
    },
#else
#define __BEFORE_MOETHOD_DEF                           
#endif
/* clang-format on */

#define method_def(_method, _hash) \
    _method_def(_method, _hash, ARG_TYPE_METHOD_NATIVE)
#define constructor_def(_method, _hash) \
    _method_def(_method, _hash, ARG_TYPE_METHOD_NATIVE_CONSTRUCTOR)
#define class_def(_class) const ConstArg _class##Collect[] =

#define class_inhert(_class, _super)                              \
    extern const NativeProperty _super##NativeProp;               \
    const NativeProperty _class##NativeProp = {                   \
        .super = &_super##NativeProp,                             \
        .methodGroup = (Arg*)_class##Collect,                     \
        .methodGroupCount =                                       \
            sizeof(_class##Collect) / sizeof(_class##Collect[0]), \
    }

#define pika_class(_method) _method##NativeProp

void _obj_updateProxyFlag(PikaObj* self);
#define obj_setClass(_self, _method)                        \
    obj_setPtr((_self), "@p", (void*)&pika_class(_method)); \
    _obj_updateProxyFlag((_self))

Arg* _obj_getProp(PikaObj* obj, char* name);
Arg* __eventLisener_runEvent(PikaEventListener* lisener,
                             uint32_t eventId,
                             int eventSignal);
Arg* pks_eventLisener_sendSignalAwaitResult(PikaEventListener* self,
                                            uint32_t eventId,
                                            int eventSignal);

void obj_printModules(PikaObj* self);

#endif
