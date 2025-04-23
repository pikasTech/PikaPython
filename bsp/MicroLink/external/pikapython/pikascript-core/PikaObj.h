/*
 * This file is part of the PikaPython project.
 * http://github.com/pikastech/pikapython
 *
 * MIT License
 *
 * Copyright (c) 2021 lyon liang6516@outlook.com
 * Copyright (c) 2023 Gorgon Meducer embedded_zhuroan@hotmail.com
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
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#ifndef _Process__H
#define _Process__H

#include "dataArgs.h"
#include "dataLink.h"
#include "dataMemory.h"
#include "dataQueue.h"
#include "dataStack.h"
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
    char* name;
    Hash name_hash;
    ConstPool const_pool;
    InstructArray instruct_array;
    int32_t label_pc;
};

typedef struct NativeProperty NativeProperty;
struct NativeProperty {
    const NativeProperty* super;
    const Arg* methodGroup;
    uint32_t methodGroupCount;
};

enum InstructIndex {
#define __INS_ENUM
#include "__instruction_table.h"
    __INSTRUCTION_CNT,
    __INSTRUCTION_INDEX_MAX = 0xFFFF,
    __INSTRUCTION_UNKNOWN = 0xFFFF,
};

typedef enum {
    VM_JMP_EXIT = -999,
    VM_JMP_CONTINUE = -997,
    VM_JMP_BREAK = -998,
    VM_JMP_RAISE = -996,
} VM_JMP;

#define VM_PC_EXIT (-99999)

typedef enum {
    TRY_STATE_NONE = 0,
    TRY_STATE_INNER,
} TRY_STATE;

typedef enum {
    TRY_RESULT_NONE = 0,
    TRY_RESULT_RAISE,
} TRY_RESULT;

typedef struct PikaVMError PikaVMError;
struct PikaVMError {
    int8_t code;
    int8_t line_code;
    PikaVMError* next;
};

typedef struct PikaVMThread PikaVMThread;
struct PikaVMThread {
    TRY_STATE try_state;
    TRY_RESULT try_result;
    PikaVMError* error_stack;
    PikaVMError* exception_stack;
    uint32_t error_stack_deepth;
    uint32_t error_stack_deepth_max;
    uint8_t invoke_deepth;
    uint64_t thread_id;
    struct PikaVMThread* next;
    uint8_t is_sub_thread;
};

typedef PikaObj VMParameters;
typedef struct PikaVMFrame PikaVMFrame;
struct PikaVMFrame {
    VMParameters* locals;
    VMParameters* globals;
    Stack stack;
    int32_t jmp;
    int32_t pc;
    ByteCodeFrame* bytecode_frame;
    uint8_t loop_deepth;
    uint32_t ins_cnt;
    pika_bool in_super;
    uint8_t super_invoke_deepth;
    PikaVMThread* vm_thread;
    pika_bool ireg[PIKA_REGIST_SIZE];
    PikaObj* oreg[16];
    pika_bool in_repl;
    PikaVMError error;
};

typedef PikaObj* (*NewFun)(Args* args);
struct PikaObj {
    Args* list;
    NewFun constructor;
#if PIKA_GC_MARK_SWEEP_ENABLE
    PikaObj* gcNext;
#endif
#if PIKA_KERNAL_DEBUG_ENABLE
    char* name;
    Arg* aName;
    PikaObj* parent;
    pika_bool isAlive;
    pika_bool isGCRoot;
#endif
#if PIKA_GC_MARK_SWEEP_ENABLE && PIKA_KERNAL_DEBUG_ENABLE
    PikaObj* gcRoot;
#endif
    uint8_t refcnt;
    uint16_t flag;
    PikaVMFrame* vmFrame;
};

typedef struct PikaGC PikaGC;
typedef int (*pikaGC_hook)(PikaGC* gc);
struct PikaGC {
    uint32_t markDeepth;
    pikaGC_hook onMarkObj;
    PikaObj* oThis;
};

typedef struct RangeData RangeData;
struct RangeData {
    int64_t start;
    int64_t end;
    int64_t step;
    int64_t i;
};

typedef struct PikaObjState PikaObjState;
struct PikaObjState {
    Arg* helpModulesCmodule;
    pika_bool inRootObj;
#if PIKA_GC_MARK_SWEEP_ENABLE
    PikaObj* gcChain;
    uint32_t objCnt;
    uint32_t objCntMax;
    uint32_t objCntLastGC;
    uint32_t markSweepBusy;
#endif
};

#define OBJ_FLAG_PROXY_GETATTRIBUTE 1 << 0
#define OBJ_FLAG_PROXY_GETATTR 1 << 1
#define OBJ_FLAG_PROXY_SETATTR 1 << 2
#define OBJ_FLAG_ALREADY_INIT 1 << 3
#define OBJ_FLAG_RUN_AS 1 << 4
#define OBJ_FLAG_GLOBALS 1 << 5
#define OBJ_FLAG_GC_MARKED 1 << 6
#define OBJ_FLAG_GC_ROOT 1 << 7
#define OBJ_FLAG_PROXY_METHOD 1 << 8
#define OBJ_FLAG_IN_DEL 1 << 9

#define PIKA_KEY_UP 0x41
#define PIKA_KEY_DOWN 0x42
#define PIKA_KEY_RIGHT 0x43
#define PIKA_KEY_LEFT 0x44

static inline uint8_t obj_getFlag(PikaObj* self, uint16_t flag) {
    pika_assert(self);
    return (self->flag & flag) == flag;
}

static inline void obj_setFlag(PikaObj* self, uint16_t flag) {
    pika_assert(self);
    self->flag |= flag;
#if PIKA_KERNAL_DEBUG_ENABLE
    if (flag == OBJ_FLAG_GC_ROOT) {
        self->isGCRoot = pika_true;
    }
#endif
}

static inline void obj_clearFlag(PikaObj* self, uint16_t flag) {
    self->flag &= ~flag;
#if PIKA_KERNAL_DEBUG_ENABLE
    if (flag == OBJ_FLAG_GC_ROOT) {
        self->isGCRoot = pika_false;
    }
#endif
}

typedef PikaObj* (*InitFun)(PikaObj* self, Args* args);
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
    PikaObj* host_obj;
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
int obj_GC(PikaObj* self);
int32_t obj_init(PikaObj* self, Args* args);
int32_t obj_update(PikaObj* self);
int32_t obj_enable(PikaObj* self);
int32_t obj_disable(PikaObj* self);

// arg type operations
PIKA_RES obj_setInt(PikaObj* self, char* argPath, int64_t val);
PIKA_RES obj_setObj(PikaObj* self, char* argPath, PikaObj* obj);
PIKA_RES obj_setRef(PikaObj* self, char* argPath, PikaObj* pointer);
PIKA_RES obj_setPtr(PikaObj* self, char* argPath, void* pointer);
PIKA_RES obj_setFloat(PikaObj* self, char* argPath, pika_float value);
PIKA_RES obj_setStr(PikaObj* self, char* argPath, char* str);
PIKA_RES obj_setNone(PikaObj* self, char* argPath);
PIKA_RES obj_setArg(PikaObj* self, char* argPath, Arg* arg);
PIKA_RES obj_setArg_noCopy(PikaObj* self, char* argPath, Arg* arg);
PIKA_RES obj_setBytes(PikaObj* self, char* argPath, uint8_t* src, size_t size);

void* obj_getPtr(PikaObj* self, char* argPath);
PikaObj* obj_getBuiltins(void);
pika_float obj_getFloat(PikaObj* self, char* argPath);
char* obj_getStr(PikaObj* self, char* argPath);
int64_t obj_getInt(PikaObj* self, char* argPath);
int64_t obj_getSize(PikaObj* arg_obj);
pika_bool obj_getBool(PikaObj* self, char* argPath);
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
                                  pika_bool* pIsClass);

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
pika_bool obj_isArgExist(PikaObj* self, char* argPath);
pika_bool obj_isMethodExist(PikaObj* self, char* method);
PikaObj* obj_newObjFromConstructor(PikaObj* self,
                                   char* name,
                                   NewFun newClassFun);
PikaObj* newRootObj(char* name, NewFun newObjFun);
PikaObj* obj_getClassObj(PikaObj* obj);
Arg* obj_getMethodArg(PikaObj* obj, char* methodName);
Arg* obj_getMethodArg_noalloc(PikaObj* obj, char* methodName, Arg* arg_reg);
Arg* obj_getMethodArgWithFullPath(PikaObj* obj, char* methodPath);
Arg* obj_getMethodArgWithFullPath_noalloc(PikaObj* obj,
                                          char* methodPath,
                                          Arg* arg_reg);

void obj_setErrorCode(PikaObj* self, int32_t errCode);

void obj_setSysOut(PikaObj* self, char* fmt, ...);

uint8_t obj_getAnyArg(PikaObj* self,
                      char* targetArgName,
                      char* sourceArgPath,
                      Args* targetArgs);

void method_returnStr(Args* args, char* val);
void method_returnInt(Args* args, int64_t val);
void method_returnBool(Args* args, pika_bool val);
void method_returnFloat(Args* args, pika_float val);
void method_returnPtr(Args* args, void* val);
void method_returnObj(Args* args, void* val);
int64_t method_getInt(Args* args, char* argName);
pika_float method_getFloat(Args* args, char* argName);
char* method_getStr(Args* args, char* argName);
void method_returnArg(Args* args, Arg* arg);
MethodProp* methodArg_getProp(Arg* method_arg);
char* methodArg_getDec(Arg* method_arg);
char* methodArg_getTypeList(Arg* method_arg, char* buffs, size_t size);
char* methodArg_getName(Arg* method_arg, char* buffs, size_t size);
Arg* methodArg_setHostObj(Arg* method_arg, PikaObj* host_obj);
Arg* methodArg_super(Arg* aThis, NativeProperty** p_prop);
PikaObj* methodArg_getHostObj(Arg* method_arg);
ByteCodeFrame* methodArg_getBytecodeFrame(Arg* method_arg);
Method methodArg_getPtr(Arg* method_arg);

VMParameters* obj_run(PikaObj* self, char* cmd);
Arg* pika_runFunction0(Arg* functionArg);
Arg* pika_runFunction1(Arg* methodArg, Arg* arg1);
Arg* pika_runFunction2(Arg* methodArg, Arg* arg1, Arg* arg2);
Arg* obj_runMethodArg0(PikaObj* self, Arg* methodArg);
Arg* obj_runMethodArg1(PikaObj* self, Arg* methodArg, Arg* arg1);
Arg* obj_runMethodArg2(PikaObj* self, Arg* methodArg, Arg* arg1, Arg* arg2);
Arg* obj_runMethod0(PikaObj* self, char* methodName);
Arg* obj_runMethod1(PikaObj* self, char* methodName, Arg* arg1);
Arg* obj_runMethod2(PikaObj* self, char* methodName, Arg* arg1, Arg* arg2);
PikaObj* New_PikaObj(Args* args);
PikaObj* New_PikaObj_noGC(void);

/* tools */
int64_t fast_atoi(char* src);
PIKA_RES fast_atoi_safe(char* src, int64_t* out);
char* fast_itoa(char* buf, uint32_t val);

/* shell */
void pikaPythonShell(PikaObj* self);
void pikaShellSetEcho(pika_bool enable_echo);
enum shellCTRL { SHELL_CTRL_CONTINUE, SHELL_CTRL_EXIT };

typedef struct ShellConfig ShellConfig;
typedef enum shellCTRL (*sh_handler)(PikaObj*, char*, ShellConfig*);
typedef char (*sh_getchar)(void);

int shConfig_deinit(ShellConfig* self);

#if PIKA_SHELL_FILTER_ENABLE
typedef struct FilterFIFO {
    ByteQueue queue;
    uint8_t ignore_mask;
    uint8_t buffer[PIKA_SHELL_FILTER_FIFO_SIZE];
} FilterFIFO;

typedef struct FilterItem FilterItem;

typedef pika_bool FilterMessageHandler(FilterItem* msg,
                                       PikaObj* self,
                                       ShellConfig* shell);

struct FilterItem {
    FilterMessageHandler* handler;
    const uint8_t* message;
    uint16_t size;
    uint8_t is_visible : 1;
    uint8_t is_case_insensitive : 1;
    uint8_t : 6;
    uint8_t ignore_mask;
    uintptr_t target;
};

#endif

typedef struct {
    int max_size;
    int current;
    int count;
    int last_offset;
    char** history;
    int cached_current;
} ShellHistory;

struct ShellConfig {
#if PIKA_SHELL_FILTER_ENABLE
    FilterFIFO filter_fifo;
    FilterItem* messages;
    uint16_t message_count;
    uint16_t : 16; /* padding to suppress warning*/
#endif
    char* prefix;
    sh_handler handler;
    void* context;
    char lineBuff[PIKA_LINE_BUFF_SIZE];
    size_t line_position;
    size_t line_curpos;
    char* blockBuffName;
    pika_bool inBlock;
    char lastChar;
    sh_getchar fn_getchar;
    uint8_t stat;
#if PIKA_SHELL_HISTORY_ENABLE
    ShellHistory* history;
#endif
    pika_bool no_echo;
    PikaObj* locals;
    PikaObj* globals;
};

#if PIKA_SHELL_HISTORY_ENABLE
ShellHistory* shHistory_create(int max_size);
void shHistory_destroy(ShellHistory* self);
void shHistory_add(ShellHistory* self, char* command);
void shHistory_setMaxSize(ShellHistory* self, int max_size);
char* shHistory_get(ShellHistory* self, int index);
char* shHistory_getPrev(ShellHistory* self);
char* shHistory_getNext(ShellHistory* self);
#endif

void _do_pikaScriptShell(PikaObj* self, ShellConfig* cfg);

void _temp__do_pikaScriptShell(PikaObj* self, ShellConfig* cfg);

/*
    need implament :
        pika_platform_fopen()
        pika_platform_fwrite()
        pika_platform_fclose()
*/
Method obj_getNativeMethod(PikaObj* self, char* method_name);
PIKA_RES obj_runNativeMethod(PikaObj* self, char* method_name, Args* args);
Arg* obj_newObjInPackage(NewFun newObjFun);

/* A helper function to create a new tuple PikaObj and append the given
 * arguments (of type Arg*) to it. */
PikaObj* _pika_tuple_new(int num_args, ...);

/* A helper function to create a new list PikaObj and append the given
 * arguments (of type Arg*) to it. */
PikaObj* _pika_list_new(int num_args, ...);
PikaObj* _pika_dict_new(int num_args, ...);

/* Macro to create a new tuple PikaObj with the given arguments (of type Arg*).
 */
#define New_PikaTupleFromVarArgs(...) \
    _pika_tuple_new(sizeof((Arg*[]){__VA_ARGS__}) / sizeof(Arg*), __VA_ARGS__)

/* Macro to create a new list PikaObj with the given arguments (of type Arg*).
 */
#define New_PikaListFromVarArgs(...) \
    _pika_list_new(sizeof((Arg*[]){__VA_ARGS__}) / sizeof(Arg*), __VA_ARGS__)

/* Macro to create a new dict PikaObj with the given arguments (of type
 * Arg*).
 */

#define New_PikaDictFromVarArgs(...)                                       \
    _pika_dict_new(sizeof((Arg*[]){__VA_ARGS__, NULL}) / sizeof(Arg*) - 1, \
                   __VA_ARGS__)

PikaObj* newNormalObj(NewFun newObjFun);
Arg* arg_setRef(Arg* self, char* name, PikaObj* obj);
Arg* arg_setObj(Arg* self, char* name, PikaObj* obj);
Arg* arg_setWeakRef(Arg* self, char* name, PikaObj* obj);

PikaObj* arg_getObj(Arg* self);
pika_bool arg_isList(Arg* arg);
pika_bool arg_isDict(Arg* arg);
pika_bool arg_isTuple(Arg* arg);

static inline void arg_setObjFlag(Arg* self, uint8_t flag) {
    if (!arg_isObject(self)) {
        return;
    }
    obj_setFlag((PikaObj*)arg_getPtr(self), flag);
}

static inline void arg_clearObjFlag(Arg* self, uint8_t flag) {
    if (!arg_isObject(self)) {
        return;
    }
    obj_clearFlag((PikaObj*)arg_getPtr(self), flag);
}

static inline Arg* arg_newObj(PikaObj* obj) {
    return arg_setObj(NULL, (char*)"", (obj));
}

static inline Arg* arg_newRef(PikaObj* obj) {
    return arg_setRef(NULL, (char*)"", (obj));
}

uint8_t* pika_getByteCodeFromModule(char* module_name);
LibObj* pika_getLibObj(void);

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
int32_t obj_newHostObj(PikaObj* self, char* objName);
int obj_runModule(PikaObj* self, char* module_name);
PikaObj* obj_runFile(PikaObj* self, char* file_name);
PikaObj* obj_runSingleFile(PikaObj* self, char* file_name);
char* obj_toStr(PikaObj* self);
Arg* arg_newDirectObj(NewFun new_obj_fun);
enum shellCTRL obj_runChar(PikaObj* self, char inputChar);

#define PIKA_PYTHON_BEGIN
#define PIKA_PYTHON(x)
#define PIKA_PYTHON_END

typedef PikaObj PikaEventListener;

void pika_eventListener_registEventHandler(PikaEventListener* self,
                                           uintptr_t eventId,
                                           PikaObj* eventHandleObj);

void pika_eventListener_registEventCallback(PikaEventListener* listener_p,
                                            uintptr_t eventId,
                                            Arg* eventCallback);

void pika_eventListener_removeEvent(PikaEventListener* self, uintptr_t eventId);

PIKA_RES _do_pika_eventListener_send(PikaEventListener* self,
                                     uintptr_t eventId,
                                     Arg* eventData,
                                     int eventSignal,
                                     pika_bool pickupWhenNoVM);

PIKA_RES pika_eventListener_sendSignal(PikaEventListener* self,
                                       uintptr_t eventId,
                                       int eventSignal);

Arg* pika_eventListener_sendSignalAwaitResult(PikaEventListener* self,
                                              uintptr_t eventId,
                                              int eventSignal);

PIKA_RES pika_eventListener_send(PikaEventListener* self,
                                 uintptr_t eventId,
                                 Arg* eventData);

Arg* pika_eventListener_syncSendAwaitResult(PikaEventListener* self,
                                            uintptr_t eventId,
                                            Arg* eventData);

PIKA_RES pika_eventListener_syncSend(PikaEventListener* self,
                                     uintptr_t eventId,
                                     Arg* eventData);

PIKA_RES pika_eventListener_syncSendSignal(PikaEventListener* self,
                                           uintptr_t eventId,
                                           int eventSignal);

Arg* pika_eventListener_syncSendSignalAwaitResult(PikaEventListener* self,
                                                  uintptr_t eventId,
                                                  int eventSignal);

void pika_eventListener_init(PikaEventListener** p_self);
void pika_eventListener_deinit(PikaEventListener** p_self);
PikaObj* methodArg_getDefContext(Arg* method_arg);
int obj_linkLibraryFile(PikaObj* self, char* input_file_name);
NewFun obj_getClass(PikaObj* obj);

void pika_printVersion(void);
void pika_getVersion(char* buff);
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

#define PIKA_ERR_STRING_SYNTAX_ERROR \
    ANSI_COLOR_RED "SyntaxError: invalid syntax" ANSI_COLOR_RESET

#define ABSTRACT_METHOD_NEED_OVERRIDE_ERROR(_)                           \
    obj_setErrorCode(self, 1);                                           \
    obj_setSysOut(                                                       \
        self,                                                            \
        ANSI_COLOR_RED                                                   \
        "Error: abstract method `%s()` need override." ANSI_COLOR_RESET, \
        __FUNCTION__)

char* obj_cacheStr(PikaObj* self, char* str);
PikaObj* _arg_to_obj(Arg* self, pika_bool* pIsTemp);
Arg* arg_toStrArg(Arg* arg);

#define PIKASCRIPT_VERSION_TO_NUM(majer, minor, micro) \
    majer * 100 * 100 + minor * 100 + micro

#define PIKASCRIPT_VERSION_NUM                                        \
    PIKASCRIPT_VERSION_TO_NUM(PIKA_VERSION_MAJOR, PIKA_VERSION_MINOR, \
                              PIKA_VERSION_MICRO)

#define PIKASCRIPT_VERSION_REQUIRE_MINIMUN(majer, minor, micro) \
    (PIKASCRIPT_VERSION_NUM >= PIKASCRIPT_VERSION_TO_NUM(majer, minor, micro))

/* [example]
const MethodProp floatMethod = {
    .ptr = (void*)builtins_floatMethod,
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

#if PIKA_ARG_CACHE_ENABLE
#define _ARG_HEAP_SIZE_DEF() .heap_size = 0,
#else
#define _ARG_HEAP_SIZE_DEF()
#endif

#if PIKA_KERNAL_DEBUG_ENABLE
#define _ARG_VALUE_DEF(_method)                                          \
    .name = (char*)&_method##Prop, .value = (_arg_value*)&_method##Prop, \
    .str = (char*)&_method##Prop, .bytes = (uint8_t*)&_method##Prop,     \
    ._name_buff = ""
#else
#define _ARG_VALUE_DEF(_method)
#endif

/* clang-format off */
#define _method_def(_method, _hash, _type)          \
    {                                               \
        ._ =                                        \
            {                                       \
                .buffer = (uint8_t*)&_method##Prop  \
            },                                      \
        .size = sizeof(MethodPropNative),           \
        _ARG_HEAP_SIZE_DEF()                        \
        .type = _type,                              \
        .flag = 0,                                  \
        .name_hash = _hash,                         \
        _ARG_VALUE_DEF(_method)                     \
    }


#if defined(_WIN32) || \
    (defined(__ARMCC_VERSION) && (__ARMCC_VERSION < 6000000)) || defined(__IAR_SYSTEMS_ICC__)
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
#define obj_setClass(_self, _method)                         \
    obj_setPtr((_self), "@p_", (void*)&pika_class(_method)); \
    _obj_updateProxyFlag((_self))

Arg* _obj_getPropArg(PikaObj* obj, char* name);

Arg* __eventListener_runEvent(PikaEventListener* lisener,
                              uintptr_t eventId,
                              Arg* eventData);

void obj_printModules(PikaObj* self);
#if PIKA_DEBUG_ENABLE
#define pika_debug(fmt, ...)                                       \
    pika_platform_printf(ANSI_COLOR_GREEN "[PikaDBG] " fmt         \
                                          "\r\n" ANSI_COLOR_RESET, \
                         ##__VA_ARGS__)

#define pika_debug_raw(fmt, ...) \
    pika_platform_printf(ANSI_COLOR_GREEN fmt ANSI_COLOR_RESET, ##__VA_ARGS__)
#else
#define pika_debug(...) \
    do {                \
    } while (0)

#define pika_debug_raw(...) \
    do {                    \
    } while (0)
#endif

void pika_debug_bytes(uint8_t* buff, size_t len);

#define pika_assert_arg_alive(__arg)                              \
    do {                                                          \
        if (NULL != (__arg)) {                                    \
            if (arg_isObject((__arg))) {                          \
                pika_assert(obj_checkAlive(arg_getPtr((__arg)))); \
            }                                                     \
        }                                                         \
    } while (0)

#define pika_assert_obj_alive(__obj)          \
    do {                                      \
        pika_assert(obj_checkAlive((__obj))); \
    } while (0)

void obj_appendGcChain(PikaObj* self);
void obj_removeGcChain(PikaObj* self);
void obj_enableGC(PikaObj* self);
pika_bool obj_checkAlive(PikaObj* self);
void obj_setName(PikaObj* self, char* name);

void pikaGC_mark(void);
void pikaGC_markDump(void);
void pikaGC_lock(void);
void pikaGC_unlock(void);
pika_bool pikaGC_islock(void);
uint32_t pikaGC_count(void);
uint32_t pikaGC_countMarked(void);
uint32_t pikaGC_markSweep(void);
uint32_t pikaGC_printFreeList(void);

int pika_GIL_EXIT(void);
int pika_GIL_ENTER(void);
int pika_GIL_getBareLock(void);

int32_t pika_debug_find_break_point_pc(char* pyFile, uint32_t pyLine);

typedef PikaObj PikaList;
typedef PikaObj PikaTuple;
typedef PikaObj PikaDict;

pika_bool pika_GIL_isInit(void);
int pika_GIL_deinit(void);

pika_bool _bytes_contains(Arg* self, Arg* others);

/* builtins */
PikaObj* New_builtins(Args* args);

void pika_sleep_ms(uint32_t ms);
PIKA_RES _transeBool(Arg* arg, pika_bool* res);

#define _OBJ2LIST(_obj) ((Args*)obj_getPtr((_obj), "list"))
#define _OBJ2DICT(_obj) ((Args*)obj_getPtr((_obj), "dict"))
#define _OBJ2KEYS(_obj) ((Args*)obj_getPtr((_obj), "_keys"))

/* list api */
PikaList* New_PikaList(void);
#define New_PikaListFrom New_PikaListFromVarArgs
PIKA_RES pikaList_append(PikaList* self, Arg* arg);
PIKA_RES pikaList_set(PikaList* self, int index, Arg* arg);
void pikaList_init(PikaObj* self);
int pikaList_getInt(PikaList* self, int index);
pika_bool pikaList_getBool(PikaList* self, int index);
pika_bool pikaTuple_getBool(PikaTuple* self, int index);
pika_float pikaList_getFloat(PikaList* self, int index);
char* pikaList_getStr(PikaList* self, int index);
void* pikaList_getPtr(PikaList* self, int index);
PikaObj* pikaList_getObj(PikaList* self, int index);
Arg* pikaList_get(PikaList* self, int index);
#define pikaList_getArg pikaList_get
size_t pikaList_getSize(PikaList* self);
void pikaList_reverse(PikaList* self);
PIKA_RES pikaList_insert(PikaList* self, int index, Arg* arg);
Arg* pikaList_pop(PikaList* list);
Arg* pikaList_pop_withIndex(PikaList* list, int index);
PIKA_RES pikaList_remove(PikaList* list, Arg* arg);
void pikaList_deinit(PikaList* self);
ArgType pikaList_getType(PikaList* self, int index);
int32_t pikaList_forEach(PikaObj* self,
                         int32_t (*eachHandle)(PikaObj* self,
                                               int itemIndex,
                                               Arg* itemEach,
                                               void* context),
                         void* context);

/* tuple api */
PikaTuple* New_PikaTuple(void);
#define New_PikaTupleFrom New_PikaTupleFromVarArgs
#define pikaTuple_forEach pikaList_forEach
void pikaTuple_deinit(PikaTuple* self);
Arg* pikaTuple_getArg(PikaTuple* self, int index);
#define pikaTuple_get pikaTuple_getArg
size_t pikaTuple_getSize(PikaTuple* self);
int64_t pikaTuple_getInt(PikaTuple* self, int index);
pika_float pikaTuple_getFloat(PikaTuple* self, int index);
char* pikaTuple_getStr(PikaTuple* self, int index);
void* pikaTuple_getPtr(PikaTuple* self, int index);
ArgType pikaTuple_getType(PikaTuple* self, int index);

/* dict api */
PikaDict* New_PikaDict(void);
#define New_PikaDictFrom New_PikaDictFromVarArgs
PIKA_RES _pikaDict_setVal(PikaDict* self, Arg* val);
PIKA_RES pikaDict_setInt(PikaDict* self, char* name, int64_t val);
pika_bool pikaDict_setBool(PikaDict* self, char* name, pika_bool val);
PIKA_RES pikaDict_setFloat(PikaDict* self, char* name, pika_float val);
PIKA_RES pikaDict_setStr(PikaDict* self, char* name, char* val);
PIKA_RES pikaDict_setPtr(PikaDict* self, char* name, void* val);
PIKA_RES pikaDict_set(PikaDict* self, char* name, Arg* val);
#define pikaDict_setArg pikaDict_set
PIKA_RES pikaDict_removeArg(PikaDict* self, Arg* val);
PIKA_RES pikaDict_reverse(PikaDict* self);
PIKA_RES pikaDict_setBytes(PikaDict* self,
                           char* name,
                           uint8_t* val,
                           size_t size);
int64_t pikaDict_getInt(PikaDict* self, char* name);
pika_bool pikaDict_getBool(PikaDict* self, char* name);
pika_float pikaDict_getFloat(PikaDict* self, char* name);
char* pikaDict_getStr(PikaDict* self, char* name);
void* pikaDict_getPtr(PikaDict* self, char* name);
PikaObj* pikaDict_getObj(PikaDict* self, char* name);
int pikaDict_getSize(PikaDict* self);
Arg* pikaDict_getArgByidex(PikaDict* self, int index);
Arg* pikaDict_get(PikaDict* self, char* name);
#define pikaDict_getArg pikaDict_get
int32_t pikaDict_isArgExist(PikaDict* self, char* name);
uint8_t* pikaDict_getBytes(PikaDict* self, char* name);
ArgType pikaDict_getType(PikaDict* self, char* name);
size_t pikaDict_getBytesSize(PikaDict* self, char* name);
int32_t pikaDict_forEach(PikaObj* self,
                         int32_t (*eachHandle)(PikaObj* self,
                                               Arg* keyEach,
                                               Arg* valEach,
                                               void* context),
                         void* context);
void pikaDict_init(PikaObj* self);
void pikaDict_deinit(PikaDict* self);

PikaTuple* args_getTuple(Args* self, char* name);
PikaDict* args_getDict(Args* self, char* name);
char* strsFormatList(Args* out_buffs, char* fmt, PikaList* list);
PIKA_RES obj_setEventCallback(PikaObj* self,
                              uintptr_t eventId,
                              Arg* eventCallback,
                              PikaEventListener* eventListener);

#if 1
#define _RETURN_WHEN_NOT_ZERO(_stmt_, _ret_) \
    do {                                     \
        if (0 != (_stmt_)) {                 \
            return _ret_;                    \
        }                                    \
    } while (0)
#else
#define _RETURN_WHEN_NOT_ZERO(_stmt_, _ret_) (_stmt_)
#endif

#endif
#ifdef __cplusplus
}
#endif
