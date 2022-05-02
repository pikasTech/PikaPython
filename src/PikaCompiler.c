#include "PikaCompiler.h"
#include "BaseObj.h"
#include "PikaObj.h"
#include "PikaParser.h"
#include "dataQueue.h"
#include "dataQueueObj.h"
#include "dataStack.h"
#include "dataStrs.h"

/* const Pool output redirect */
static void __handler_constPool_output_file(ConstPool* self, char* content) {
    /* to ram */
    uint16_t size = strGetSize(content) + 1;
    self->arg_buff = arg_append(self->arg_buff, content, size);
    /* to flash */
    __platform_fwrite(content, 1, size, self->output_f);
}

/* instruct array output redirect */
static void __handler_instructArray_output_none(InstructArray* self,
                                                InstructUnit* ins_unit) {
    /* none */
}

static void __handler_instructArray_output_file(InstructArray* self,
                                                InstructUnit* ins_unit) {
    /* to flash */
    __platform_fwrite(ins_unit, 1, instructUnit_getSize(), self->output_f);
}

/*
    need implament :
        __platform_fopen()
        __platform_fwrite()
        __platform_fclose()
*/

int pikaCompile(char* output_file_name, char* py_lines) {
    ByteCodeFrame bytecode_frame = {0};

    FILE* bytecode_f = __platform_fopen(output_file_name, "w+");
    /* main process */

    /* step 1, get size of const pool and instruct array */
    byteCodeFrame_init(&bytecode_frame);
    bytecode_frame.const_pool.output_f = bytecode_f;
    bytecode_frame.instruct_array.output_f = bytecode_f;
    bytecode_frame.instruct_array.output_redirect_fun =
        __handler_instructArray_output_none;
    Parser_parsePyLines(NULL, &bytecode_frame, py_lines);
    uint16_t const_pool_size = bytecode_frame.const_pool.size;
    uint16_t instruct_array_size = bytecode_frame.instruct_array.size;
    byteCodeFrame_deinit(&bytecode_frame);

    /* step 2, write instruct array to file */
    __platform_fwrite(&instruct_array_size, 1, 2, bytecode_f);
    byteCodeFrame_init(&bytecode_frame);
    bytecode_frame.const_pool.output_f = bytecode_f;
    bytecode_frame.instruct_array.output_f = bytecode_f;
    /* instruct array to file */
    bytecode_frame.instruct_array.output_redirect_fun =
        __handler_instructArray_output_file;
    Parser_parsePyLines(NULL, &bytecode_frame, py_lines);
    byteCodeFrame_deinit(&bytecode_frame);

    /* step 3, write const pool to file */
    __platform_fwrite(&const_pool_size, 1, 2, bytecode_f);
    char void_ = 0;
    /* add \0 at the start */
    __platform_fwrite(&void_, 1, 1, bytecode_f);
    byteCodeFrame_init(&bytecode_frame);
    bytecode_frame.const_pool.output_f = bytecode_f;
    bytecode_frame.instruct_array.output_f = bytecode_f;
    /* const pool to file */
    bytecode_frame.const_pool.output_redirect_fun =
        __handler_constPool_output_file;
    /* instruct array to none */
    bytecode_frame.instruct_array.output_redirect_fun =
        __handler_instructArray_output_none;
    Parser_parsePyLines(NULL, &bytecode_frame, py_lines);
    byteCodeFrame_deinit(&bytecode_frame);

    /* deinit */
    __platform_fclose(bytecode_f);
    /* succeed */
    return 0;
};

/*
    need implament :
        __platform_fopen()
        __platform_fread()
        __platform_fwrite()
        __platform_fclose()
*/
int pikaCompileFileWithOutputName(char* output_file_name,
                                  char* input_file_name) {
    Arg* input_file_arg = arg_loadFile(NULL, input_file_name);
    if (NULL == input_file_arg) {
        return 1;
    }
    pikaCompile(output_file_name, (char*)arg_getBytes(input_file_arg));
    arg_deinit(input_file_arg);
    return 0;
}

int pikaCompileFile(char* input_file_name) {
    Args buffs = {0};
    char* output_file_name = strsGetFirstToken(&buffs, input_file_name, '.');
    output_file_name = strsAppend(&buffs, input_file_name, ".o");
    pikaCompileFileWithOutputName(output_file_name, input_file_name);
    strsDeinit(&buffs);
    return 0;
}

LibObj* New_LibObj(void) {
    LibObj* self = New_TinyObj(NULL);
    return self;
}

void LibObj_deinit(LibObj* self) {
    obj_deinit(self);
}

/* add bytecode to lib, not copy the bytecode */
void LibObj_dynamicLink(LibObj* self, char* module_name, uint8_t* bytecode) {
    if (!obj_isArgExist(self, module_name)) {
        obj_newObj(self, module_name, "", New_TinyObj);
    }
    PikaObj* module_obj = obj_getObj(self, module_name);
    obj_setStr(module_obj, "name", module_name);
    obj_setPtr(module_obj, "bytecode", bytecode);
}

/* add bytecode to lib, and copy the bytecode to the buff in the lib */
int LibObj_staticLink(LibObj* self,
                      char* module_name,
                      uint8_t* bytecode,
                      size_t size) {
    if (!obj_isArgExist(self, module_name)) {
        obj_newObj(self, module_name, "", New_TinyObj);
    }
    PikaObj* module_obj = obj_getObj(self, module_name);
    /* copy bytecode to buff */
    obj_setBytes(module_obj, "buff", bytecode, size);
    /* link to buff */
    LibObj_dynamicLink(self, module_name, obj_getBytes(module_obj, "buff"));
    return 0;
}

int LibObj_staticLinkFile(LibObj* self, char* input_file_name) {
    Args buffs = {0};
    /* read file */
    Arg* input_file_arg = arg_loadFile(NULL, input_file_name);
    char* module_name = strsGetLastToken(&buffs, input_file_name, '/');

    /* cut off '.py.o' */
    module_name[strlen(module_name) - (sizeof(".py.o") - 1)] = 0;

    /* push bytecode */
    LibObj_staticLink(self, module_name, arg_getBytes(input_file_arg),
                      arg_getBytesSize(input_file_arg));

    /* deinit */
    strsDeinit(&buffs);
    arg_deinit(input_file_arg);
    return 0;
}

static int32_t __foreach_handler_listModules(Arg* argEach, Args* handleArgs) {
    if (arg_getType(argEach) == ARG_TYPE_OBJECT) {
        PikaObj* module_obj = arg_getPtr(argEach);
        __platform_printf("%s\r\n", obj_getStr(module_obj, "name"));
    }
    return 0;
}

void LibObj_listModules(LibObj* self) {
    args_foreach(self->list, __foreach_handler_listModules, NULL);
}

static int32_t __foreach_handler_writeBytecode(Arg* argEach, Args* handleArgs) {
    FILE* out_file = args_getPtr(handleArgs, "out_file");
    if (arg_getType(argEach) == ARG_TYPE_OBJECT) {
        PikaObj* module_obj = arg_getPtr(argEach);
        char* bytecode = obj_getPtr(module_obj, "bytecode");
        size_t bytecode_size = obj_getBytesSize(module_obj, "buff");
        size_t aline_size =
            aline_by(bytecode_size, sizeof(uint32_t)) - bytecode_size;
        char aline_buff[sizeof(uint32_t)] = {0};
        __platform_fwrite(bytecode, 1, bytecode_size, out_file);
        __platform_fwrite(aline_buff, 1, aline_size, out_file);
    }
    return 0;
}

static int32_t __foreach_handler_writeIndex(Arg* argEach, Args* handleArgs) {
    FILE* out_file = args_getPtr(handleArgs, "out_file");
    if (arg_getType(argEach) == ARG_TYPE_OBJECT) {
        PikaObj* module_obj = arg_getPtr(argEach);
        uint32_t bytecode_size = obj_getBytesSize(module_obj, "buff");
        char buff[LIB_INFO_BLOCK_SIZE - sizeof(uint32_t)] = {0};
        bytecode_size = aline_by(bytecode_size, sizeof(uint32_t));
        char* module_name = obj_getStr(module_obj, "name");
        __platform_memcpy(buff, module_name, strGetSize(module_name));
        __platform_fwrite(buff, 1, LIB_INFO_BLOCK_SIZE - sizeof(bytecode_size),
                          out_file);
        __platform_fwrite(&bytecode_size, 1, sizeof(bytecode_size), out_file);
    }
    return 0;
}

static int32_t __foreach_handler_getModuleNum(Arg* argEach, Args* handleArgs) {
    if (arg_getType(argEach) == ARG_TYPE_OBJECT) {
        args_setInt(handleArgs, "module_num",
                    args_getInt(handleArgs, "module_num") + 1);
    }
    return 0;
}

int LibObj_saveLibraryFile(LibObj* self, char* output_file_name) {
    FILE* out_file = __platform_fopen(output_file_name, "w+");

    Args handleArgs = {0};
    args_setPtr(&handleArgs, "out_file", out_file);
    args_setInt(&handleArgs, "module_num", 0);

    /* write meta information */
    char buff[LIB_INFO_BLOCK_SIZE] = {0};
    args_foreach(self->list, __foreach_handler_getModuleNum, &handleArgs);
    uint32_t module_num = args_getInt(&handleArgs, "module_num");
    /* write module_num to the file */
    __platform_memcpy(buff, &module_num, sizeof(uint32_t));
    /* aline to 32 bytes */
    __platform_fwrite(buff, 1, LIB_INFO_BLOCK_SIZE, out_file);
    /* write module index to file */
    args_foreach(self->list, __foreach_handler_writeIndex, &handleArgs);
    /* write module bytecode to file */
    args_foreach(self->list, __foreach_handler_writeBytecode, &handleArgs);
    args_deinit_stack(&handleArgs);
    /* main process */
    /* deinit */
    __platform_fclose(out_file);
    return 0;
}

int LibObj_loadLibrary(LibObj* self, uint8_t* library) {
    uint32_t module_num = library[0];
    uint8_t* bytecode_addr = library + LIB_INFO_BLOCK_SIZE * (module_num + 1);
    for (uint32_t i = 0; i < module_num; i++) {
        char* module_name = (char*)(library + LIB_INFO_BLOCK_SIZE * (i + 1));
        LibObj_dynamicLink(self, module_name, bytecode_addr);
        uint32_t module_size =
            *(uint32_t*)(module_name + LIB_INFO_BLOCK_SIZE - sizeof(uint32_t));
        bytecode_addr += module_size;
    }
    return 0;
}

int LibObj_loadLibraryFile(LibObj* self, char* input_file_name) {
    Arg* file_arg = arg_loadFile(NULL, input_file_name);
    /* save file_arg as __lib_buf to libObj */
    obj_setArg_noCopy(self, "__lib_buf", file_arg);
    LibObj_loadLibrary(self, arg_getBytes(file_arg));
    return 0;
}
