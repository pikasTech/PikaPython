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
#include "PikaCompiler.h"
#include "BaseObj.h"
#include "PikaObj.h"
#include "PikaParser.h"
#include "dataQueue.h"
#include "dataQueueObj.h"
#include "dataStack.h"
#include "dataStrs.h"

const char magic_code_pyo[] = {0x0f, 'p', 'y', 'o'};

static PIKA_BOOL _check_magic_code_pyo(uint8_t* bytecode) {
    char* data = (char*)bytecode;
    if (data[0] == magic_code_pyo[0] && data[1] == magic_code_pyo[1] &&
        data[2] == magic_code_pyo[2] && data[3] == magic_code_pyo[3]) {
        return PIKA_TRUE;
    }
    return PIKA_FALSE;
}

static uint8_t* arg_getBytecode(Arg* self) {
    uint8_t* bytecode_file = arg_getBytes(self);
    if (_check_magic_code_pyo(bytecode_file)) {
        return bytecode_file + sizeof(magic_code_pyo) + sizeof(uint32_t);
    }
    return bytecode_file;
}

static size_t arg_getBytecodeSize(Arg* self) {
    size_t size_all = arg_getBytesSize(self);
    uint8_t* bytecode_file = arg_getBytes(self);
    if (_check_magic_code_pyo(bytecode_file)) {
        return size_all - sizeof(magic_code_pyo) - sizeof(uint32_t);
    }
    return size_all;
}

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

PIKA_RES pikaCompile(char* output_file_name, char* py_lines) {
    PIKA_RES res = PIKA_RES_OK;
    ByteCodeFrame bytecode_frame = {0};

    FILE* bytecode_f = __platform_fopen(output_file_name, "wb+");
    if (NULL == bytecode_f) {
        __platform_printf("Error: open file %s failed.\r\n", output_file_name);
        res = PIKA_RES_ERR_IO_ERROR;
        goto exit;
    }
    /* main process */

    /* step 1, get size of const pool and instruct array */
    byteCodeFrame_init(&bytecode_frame);
    bytecode_frame.const_pool.output_f = bytecode_f;
    bytecode_frame.instruct_array.output_f = bytecode_f;
    bytecode_frame.instruct_array.output_redirect_fun =
        __handler_instructArray_output_none;
    res = Parser_linesToBytes(&bytecode_frame, py_lines);
    if (PIKA_RES_OK != res) {
        __platform_printf("    Error: Syntax error.\r\n");
        goto exit;
    }
    uint32_t const_pool_size = bytecode_frame.const_pool.size;
    uint32_t instruct_array_size = bytecode_frame.instruct_array.size;
    uint32_t bytecode_size = const_pool_size + instruct_array_size +
                             sizeof(const_pool_size) +
                             sizeof(instruct_array_size);
    byteCodeFrame_deinit(&bytecode_frame);

    /* step 2, write instruct array to file */
    /* write magic code */
    __platform_fwrite(magic_code_pyo, 1, sizeof(magic_code_pyo), bytecode_f);
    /* write bytecode size */
    __platform_fwrite(&bytecode_size, 1, sizeof(bytecode_size), bytecode_f);
    /* write ins array size */
    __platform_fwrite(&instruct_array_size, 1, sizeof(instruct_array_size),
                      bytecode_f);
    byteCodeFrame_init(&bytecode_frame);
    bytecode_frame.const_pool.output_f = bytecode_f;
    bytecode_frame.instruct_array.output_f = bytecode_f;
    /* instruct array to file */
    bytecode_frame.instruct_array.output_redirect_fun =
        __handler_instructArray_output_file;
    Parser_linesToBytes(&bytecode_frame, py_lines);
    byteCodeFrame_deinit(&bytecode_frame);

    /* step 3, write const pool to file */
    __platform_fwrite(&const_pool_size, 1, sizeof(const_pool_size), bytecode_f);
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
    Parser_linesToBytes(&bytecode_frame, py_lines);

    /* deinit */
exit:
    byteCodeFrame_deinit(&bytecode_frame);
    if (NULL != bytecode_f) {
        __platform_fclose(bytecode_f);
    }
    /* succeed */
    return res;
};

/*
    need implament :
        __platform_fopen()
        __platform_fread()
        __platform_fwrite()
        __platform_fclose()
*/
PIKA_RES pikaCompileFileWithOutputName(char* output_file_name,
                                       char* input_file_name) {
    Args buffs = {0};
    Arg* input_file_arg = arg_loadFile(NULL, input_file_name);
    if (NULL == input_file_arg) {
        return PIKA_RES_ERR_IO_ERROR;
    }
    char* lines = (char*)arg_getBytes(input_file_arg);
    /* replace the "\r\n" to "\n" */
    lines = strsReplace(&buffs, lines, "\r\n", "\n");
    /* clear the void line */
    lines = strsReplace(&buffs, lines, "\n\n", "\n");
    /* add '\n' at the end */
    lines = strsAppend(&buffs, lines, "\n\n");
    PIKA_RES res = pikaCompile(output_file_name, lines);
    arg_deinit(input_file_arg);
    strsDeinit(&buffs);
    return res;
}

PIKA_RES pikaCompileFile(char* input_file_name) {
    Args buffs = {0};
    char* output_file_name = strsGetFirstToken(&buffs, input_file_name, '.');
    output_file_name = strsAppend(&buffs, input_file_name, ".o");
    PIKA_RES res =
        pikaCompileFileWithOutputName(output_file_name, input_file_name);
    strsDeinit(&buffs);
    return res;
}

LibObj* New_LibObj(Args* args) {
    LibObj* self = New_TinyObj(NULL);
    return self;
}

void LibObj_deinit(LibObj* self) {
    obj_deinit(self);
}

/* add bytecode to lib, not copy the bytecode */
void LibObj_dynamicLink(LibObj* self, char* module_name, uint8_t* bytecode) {
    if (strIsContain(module_name, '.')) {
        /* skip file */
        return;
    }
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
    if (NULL == input_file_arg) {
        __platform_printf("error: can't open file %s\r\n", input_file_name);
        return -1;
    }
    char* module_name = strsGetLastToken(&buffs, input_file_name, '/');

    size_t module_name_len = strlen(module_name);

    /* cut off '.py.o' */
    if (module_name[module_name_len - 1] == 'o' &&
        module_name[module_name_len - 2] == '.' &&
        module_name[module_name_len - 3] == 'y' &&
        module_name[module_name_len - 4] == 'p' &&
        module_name[module_name_len - 5] == '.') {
        module_name[module_name_len - 5] = 0;
    } else {
        // __platform_printf("linking raw  %s:%s:%ld\r\n", input_file_name,
        //                   module_name, arg_getBytecodeSize(input_file_arg));
        /* replace . to | */
        module_name = strsReplace(&buffs, module_name, ".", "|");
    }

    /* push bytecode */
    LibObj_staticLink(self, module_name, arg_getBytecode(input_file_arg),
                      arg_getBytecodeSize(input_file_arg));

    /* deinit */
    strsDeinit(&buffs);
    arg_deinit(input_file_arg);
    return 0;
}

static int32_t __foreach_handler_listModules(Arg* argEach, Args* context) {
    if (argType_isObject(arg_getType(argEach))) {
        PikaObj* module_obj = arg_getPtr(argEach);
        __platform_printf("%s\r\n", obj_getStr(module_obj, "name"));
    }
    return 0;
}

void LibObj_listModules(LibObj* self) {
    args_foreach(self->list, __foreach_handler_listModules, NULL);
}

static int32_t __foreach_handler_libWriteBytecode(Arg* argEach, Args* context) {
    FILE* out_file = args_getPtr(context, "out_file");
    if (argType_isObject(arg_getType(argEach))) {
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

static int32_t __foreach_handler_libWriteIndex(Arg* argEach, Args* context) {
    FILE* out_file = args_getPtr(context, "out_file");
    Args buffs = {0};
    if (argType_isObject(arg_getType(argEach))) {
        PikaObj* module_obj = arg_getPtr(argEach);
        uint32_t bytecode_size = obj_getBytesSize(module_obj, "buff");
        char buff[LIB_INFO_BLOCK_SIZE - sizeof(uint32_t)] = {0};
        bytecode_size = aline_by(bytecode_size, sizeof(uint32_t));
        char* module_name = obj_getStr(module_obj, "name");
        module_name = strsReplace(&buffs, module_name, "|", ".");
        // __platform_printf("   %s:%d\r\n", module_name, bytecode_size);
        __platform_memcpy(buff, module_name, strGetSize(module_name));
        __platform_fwrite(buff, 1, LIB_INFO_BLOCK_SIZE - sizeof(bytecode_size),
                          out_file);
        __platform_fwrite(&bytecode_size, 1, sizeof(bytecode_size), out_file);
    }
    strsDeinit(&buffs);
    return 0;
}

static int32_t __foreach_handler_getModuleNum(Arg* argEach, Args* context) {
    if (argType_isObject(arg_getType(argEach))) {
        args_setInt(context, "module_num",
                    args_getInt(context, "module_num") + 1);
    }
    return 0;
}

int LibObj_saveLibraryFile(LibObj* self, char* output_file_name) {
    FILE* out_file = __platform_fopen(output_file_name, "wb+");

    Args context = {0};
    args_setPtr(&context, "out_file", out_file);
    args_setInt(&context, "module_num", 0);

    /* write meta information */
    char buff[LIB_INFO_BLOCK_SIZE] = {0};
    args_foreach(self->list, __foreach_handler_getModuleNum, &context);

    /* meta info */
    char magic_code[] = {0x7f, 'p', 'y', 'a'};
    uint32_t version_num = LIB_VERSION_NUMBER;
    uint32_t module_num = args_getInt(&context, "module_num");

    /* write meta info */
    const uint32_t magic_code_offset = sizeof(uint32_t) * 0;
    const uint32_t version_offset = sizeof(uint32_t) * 1;
    const uint32_t module_num_offset = sizeof(uint32_t) * 2;

    __platform_memcpy(buff + magic_code_offset, &magic_code, sizeof(uint32_t));
    __platform_memcpy(buff + version_offset, &version_num, sizeof(uint32_t));
    /* write module_num to the file */
    __platform_memcpy(buff + module_num_offset, &module_num, sizeof(uint32_t));
    /* aline to 32 bytes */
    __platform_fwrite(buff, 1, LIB_INFO_BLOCK_SIZE, out_file);
    /* write module index to file */
    args_foreach(self->list, __foreach_handler_libWriteIndex, &context);
    /* write module bytecode to file */
    args_foreach(self->list, __foreach_handler_libWriteBytecode, &context);
    args_deinit_stack(&context);
    /* main process */
    /* deinit */
    __platform_fclose(out_file);
    return 0;
}

static int _getModuleNum(uint8_t* library_bytes) {
    if (0 != ((intptr_t)library_bytes & 0x03)) {
        return PIKA_RES_ERR_UNALIGNED_PTR;
    }

    char* magic_code = (char*)library_bytes;

    uint32_t* library_info = (uint32_t*)library_bytes;
    uint32_t version_num = library_info[1];
    uint32_t module_num = library_info[2];

    /* check magic_code */
    if (!((magic_code[0] == 0x7f) && (magic_code[1] == 'p') &&
          (magic_code[2] == 'y') && (magic_code[3] == 'a'))) {
        __platform_printf("Error: invalid magic code.\r\n");
        return PIKA_RES_ERR_ILLEGAL_MAGIC_CODE;
    }
    /* check version num */
    if (version_num != LIB_VERSION_NUMBER) {
        __platform_printf(
            "Error: invalid version number. Expected %d, got %d\r\n",
            LIB_VERSION_NUMBER, version_num);
        return PIKA_RES_ERR_INVALID_VERSION_NUMBER;
    }
    return module_num;
}

static PIKA_RES _loadModuleDataWithIndex(uint8_t* library_bytes,
                                         int module_num,
                                         int module_index,
                                         char** name_p,
                                         uint8_t** addr_p,
                                         size_t* size) {
    uint8_t* bytecode_addr =
        library_bytes + LIB_INFO_BLOCK_SIZE * (module_num + 1);
    for (uint32_t i = 0; i < module_index + 1; i++) {
        char* module_name =
            (char*)(library_bytes + LIB_INFO_BLOCK_SIZE * (i + 1));
        // __platform_printf("loading module: %s\r\n", module_name);
        *name_p = module_name;
        *addr_p = bytecode_addr;
        uint32_t module_size =
            *(uint32_t*)(module_name + LIB_INFO_BLOCK_SIZE - sizeof(uint32_t));
        *size = module_size;
        bytecode_addr += module_size;
    }
    return 0;
}

PIKA_RES _loadModuleDataWithName(uint8_t* library_bytes,
                                 char* module_name,
                                 uint8_t** addr_p,
                                 size_t* size_p) {
    int module_num = _getModuleNum(library_bytes);
    if (module_num < 0) {
        return module_num;
    }
    for (int i = 0; i < module_num; i++) {
        char* name = NULL;
        uint8_t* addr = NULL;
        size_t size = 0;
        _loadModuleDataWithIndex(library_bytes, module_num, i, &name, &addr,
                                 &size);
        if (strEqu(module_name, name)) {
            *addr_p = addr;
            *size_p = size;
            return 0;
        }
    }
    return PIKA_RES_ERR_ARG_NO_FOUND;
}

int LibObj_loadLibrary(LibObj* self, uint8_t* library_bytes) {
    int module_num = _getModuleNum(library_bytes);
    if (module_num < 0) {
        /* load error */
        return module_num;
    }
    for (uint32_t i = 0; i < module_num; i++) {
        char* module_name = NULL;
        uint8_t* bytecode_addr = NULL;
        size_t bytecode_size = 0;
        _loadModuleDataWithIndex(library_bytes, module_num, i, &module_name,
                                 &bytecode_addr, &bytecode_size);
        LibObj_dynamicLink(self, module_name, bytecode_addr);
    }
    return PIKA_RES_OK;
}

int32_t __foreach_handler_printModule(Arg* argEach, Args* context) {
    if (argType_isObject(arg_getType(argEach))) {
        PikaObj* module_obj = arg_getPtr(argEach);
        char* module_name = obj_getStr(module_obj, "name");
        if (NULL != module_name) {
            __platform_printf(module_name);
            __platform_printf("\r\n");
        }
    }
    return 0;
}

void LibObj_printModules(LibObj* self) {
    args_foreach(self->list, __foreach_handler_printModule, NULL);
}

int LibObj_loadLibraryFile(LibObj* self, char* lib_file_name) {
    Arg* file_arg = arg_loadFile(NULL, lib_file_name);
    if (NULL == file_arg) {
        __platform_printf("Error: Could not load library file '%s'\n",
                          lib_file_name);
        return PIKA_RES_ERR_IO_ERROR;
    }
    /* save file_arg as @lib_buf to libObj */
    obj_setArg_noCopy(self, "@lib_buf", file_arg);
    if (0 != LibObj_loadLibrary(self, arg_getBytes(file_arg))) {
        __platform_printf("Error: Could not load library from '%s'\n",
                          lib_file_name);
        return PIKA_RES_ERR_OPERATION_FAILED;
    }
    return PIKA_RES_OK;
}

size_t pika_fputs(char* str, FILE* fp) {
    size_t size = strGetSize(str);
    return __platform_fwrite(str, 1, size, fp);
}

int Lib_loadLibraryFileToArray(char* origin_file_name, char* out_folder) {
    Args buffs = {0};
    Arg* file_arg = arg_loadFile(NULL, origin_file_name);
    int res = 0;
    if (NULL == file_arg) {
        __platform_printf("Error: Could not load file '%s'\n",
                          origin_file_name);
        return 1;
    }
    char* output_file_name = NULL;
    output_file_name = strsGetLastToken(&buffs, origin_file_name, '/');
    output_file_name = strsAppend(&buffs, "__asset_", output_file_name);
    output_file_name = strsReplace(&buffs, output_file_name, ".", "_");
    output_file_name = strsAppend(&buffs, output_file_name, ".c");

    char* output_file_path = strsAppend(&buffs, out_folder, "/");
    output_file_path = strsAppend(&buffs, output_file_path, output_file_name);

    FILE* fp = __platform_fopen(output_file_path, "wb+");
    char* array_name = strsGetLastToken(&buffs, origin_file_name, '/');
    array_name = strsReplace(&buffs, array_name, ".", "_");
    __platform_printf("  loading %s[]...\n", array_name);
    pika_fputs("#include \"PikaPlatform.h\"\n", fp);
    pika_fputs("/* warning: auto generated file, please do not modify */\n",
               fp);
    pika_fputs("PIKA_BYTECODE_ALIGN const unsigned char ", fp);
    pika_fputs(array_name, fp);
    pika_fputs("[] = {", fp);
    char byte_buff[32] = {0};
    uint8_t* array = arg_getBytes(file_arg);
    for (size_t i = 0; i < arg_getBytesSize(file_arg); i++) {
        if (i % 12 == 0) {
            pika_fputs("\n    ", fp);
        }
        __platform_sprintf(byte_buff, "0x%02x, ", array[i]);
        pika_fputs(byte_buff, fp);
    }

    pika_fputs("\n};\n", fp);
    res = 0;
    goto exit;

exit:
    __platform_fclose(fp);
    strsDeinit(&buffs);
    arg_deinit(file_arg);
    return res;
}

static PIKA_RES __Maker_compileModuleWithInfo(PikaMaker* self,
                                              char* module_name) {
    Args buffs = {0};
    char* input_file_name = strsAppend(&buffs, module_name, ".py");
    char* input_file_path =
        strsAppend(&buffs, obj_getStr(self, "pwd"), input_file_name);
    __platform_printf("  compiling %s...\r\n", input_file_name);
    char* output_file_name = strsAppend(&buffs, module_name, ".py.o");
    char* output_file_path = NULL;
    output_file_path =
        strsAppend(&buffs, obj_getStr(self, "pwd"), "pikascript-api/");
    output_file_path = strsAppend(&buffs, output_file_path, output_file_name);
    PIKA_RES res =
        pikaCompileFileWithOutputName(output_file_path, input_file_path);
    strsDeinit(&buffs);
    return res;
}

PikaMaker* New_PikaMaker(void) {
    PikaMaker* self = New_TinyObj(NULL);
    obj_setStr(self, "pwd", "");
    obj_setInt(self, "err", 0);
    LibObj* lib = New_LibObj(NULL);
    obj_setPtr(self, "lib", lib);
    return self;
}

void pikaMaker_deinit(PikaMaker* self) {
    LibObj* lib = obj_getPtr(self, "lib");
    LibObj_deinit(lib);
    obj_deinit(self);
}

void pikaMaker_setPWD(PikaMaker* self, char* pwd) {
    obj_setStr(self, "pwd", pwd);
}

void pikaMaker_setState(PikaMaker* self, char* module_name, char* state) {
    obj_newMetaObj(self, module_name, New_TinyObj);
    PikaObj* module_obj = obj_getObj(self, module_name);
    obj_setStr(module_obj, "name", module_name);
    obj_setStr(module_obj, "state", state);
}

PIKA_RES pikaMaker_compileModule(PikaMaker* self, char* module_name) {
    PIKA_RES res = __Maker_compileModuleWithInfo(self, module_name);
    /* update compile info */
    if (PIKA_RES_OK == res) {
        pikaMaker_setState(self, module_name, "compiled");
    } else {
        pikaMaker_setState(self, module_name, "failed");
    }
    return res;
}

int pikaMaker_getDependencies(PikaMaker* self, char* module_name) {
    int res = 0;
    ByteCodeFrame bf = {0};
    Args buffs = {0};
    byteCodeFrame_init(&bf);
    ConstPool* const_pool = NULL;
    InstructArray* ins_array = NULL;
    char* module_path =
        strsAppend(&buffs, obj_getStr(self, "pwd"), "pikascript-api/");
    module_path = strsAppend(&buffs, module_path, module_name);
    char* file_path = strsAppend(&buffs, module_path, ".py.o");
    Arg* file_arg = arg_loadFile(NULL, file_path);
    uint8_t offset_befor = 0;
    if (NULL == file_arg) {
        res = 1;
        goto exit;
    }
    byteCodeFrame_loadByteCode(&bf, arg_getBytes(file_arg));
    const_pool = &bf.const_pool;
    ins_array = &bf.instruct_array;

    offset_befor = ins_array->content_offset_now;
    ins_array->content_offset_now = 0;
    while (1) {
        InstructUnit* ins_unit = instructArray_getNow(ins_array);
        if (NULL == ins_unit) {
            goto exit;
        }
        if (instructUnit_getInstruct(ins_unit) == IMP) {
            char* imp_module_name =
                constPool_getByOffset(const_pool, ins_unit->const_pool_index);
            char* imp_module_path =
                strsAppend(&buffs, obj_getStr(self, "pwd"), imp_module_name);
            /* check if compiled the module */
            if (obj_isArgExist(self, imp_module_name)) {
                /* module info is exist, do nothing */
            } else {
                /* module info is not exist */
                /* set module to be compile */
                FILE* imp_file_py = __platform_fopen(
                    strsAppend(&buffs, imp_module_path, ".py"), "rb");
                FILE* imp_file_pyi = __platform_fopen(
                    strsAppend(&buffs, imp_module_path, ".pyi"), "rb");
                FILE* imp_file_pyo = __platform_fopen(
                    strsAppend(&buffs, imp_module_path, ".py.o"), "rb");
                if (NULL != imp_file_pyo) {
                    __platform_printf("  loading %s.py.o...\r\n",
                                      imp_module_path);
                    /* found *.py.o, push to compiled list */
                    pikaMaker_setState(self, imp_module_name, "compiled");
                    char* imp_api_path = strsAppend(
                        &buffs, obj_getStr(self, "pwd"), "pikascript-api/");
                    imp_api_path =
                        strsAppend(&buffs, imp_api_path, imp_module_name);
                    FILE* imp_file_pyo_api = __platform_fopen(
                        strsAppend(&buffs, imp_api_path, ".py.o"), "wb+");
                    /* copy imp_file_pyo to imp_api_path */
                    uint8_t* buff = (uint8_t*)__platform_malloc(128);
                    size_t read_size = 0;
                    while (1) {
                        read_size =
                            __platform_fread(buff, 1, 128, imp_file_pyo);
                        if (read_size > 0) {
                            __platform_fwrite(buff, 1, read_size,
                                              imp_file_pyo_api);
                        } else {
                            break;
                        }
                    }
                    __platform_free(buff);
                    __platform_fclose(imp_file_pyo_api);
                } else if (NULL != imp_file_py) {
                    /* found *.py, push to nocompiled list */
                    pikaMaker_setState(self, imp_module_name, "nocompiled");
                } else if (NULL != imp_file_pyi) {
                    /* found *.py, push to nocompiled list */
                    pikaMaker_setState(self, imp_module_name, "cmodule");
                } else {
                    __platform_printf(
                        "    [warning]: file: '%s.pyi', '%s.py' or '%s.py.o' "
                        "no found\n",
                        imp_module_name, imp_module_name, imp_module_name);
                }
                if (NULL != imp_file_pyo) {
                    __platform_fclose(imp_file_pyo);
                }
                if (NULL != imp_file_pyi) {
                    __platform_fclose(imp_file_pyi);
                }
                if (NULL != imp_file_py) {
                    __platform_fclose(imp_file_py);
                }
            }
        }
        instructArray_getNext(ins_array);
    }

exit:
    ins_array->content_offset_now = offset_befor;
    if (NULL != file_arg) {
        arg_deinit(file_arg);
    }
    strsDeinit(&buffs);
    byteCodeFrame_deinit(&bf);
    return res;
}

int32_t __foreach_handler_printStates(Arg* argEach, Args* context) {
    if (argType_isObject(arg_getType(argEach))) {
        PikaObj* module_obj = arg_getPtr(argEach);
        __platform_printf("%s: %s\r\n", obj_getStr(module_obj, "name"),
                          obj_getStr(module_obj, "state"));
    }
    return 0;
}

void pikaMaker_printStates(PikaMaker* self) {
    args_foreach(self->list, __foreach_handler_printStates, NULL);
}

int32_t __foreach_handler_getFirstNocompiled(Arg* argEach, Args* context) {
    if (argType_isObject(arg_getType(argEach))) {
        PikaObj* module_obj = arg_getPtr(argEach);
        char* state = obj_getStr(module_obj, "state");
        if (args_isArgExist(context, "res")) {
            /* already get method */
            return 0;
        }
        if (strEqu("nocompiled", state)) {
            /* push module */
            args_setStr(context, "res", obj_getStr(module_obj, "name"));
            return 0;
        }
    }
    return 0;
}

char* pikaMaker_getFirstNocompiled(PikaMaker* self) {
    Args context = {0};
    args_foreach(self->list, __foreach_handler_getFirstNocompiled, &context);
    char* res = args_getStr(&context, "res");
    if (NULL == res) {
        /* remove res in maker */
        obj_removeArg(self, "res");
    } else {
        obj_setStr(self, "res", res);
    }
    args_deinit_stack(&context);
    return obj_getStr(self, "res");
}

PIKA_RES pikaMaker_compileModuleWithDepends(PikaMaker* self,
                                            char* module_name) {
    PIKA_RES res = PIKA_RES_OK;
    res = pikaMaker_compileModule(self, module_name);
    if (PIKA_RES_OK != res) {
        obj_setInt(self, "err", res);
        return res;
    }
    pikaMaker_getDependencies(self, module_name);
    while (1) {
        char* uncompiled = pikaMaker_getFirstNocompiled(self);
        /* compiled all modules */
        if (NULL == uncompiled) {
            break;
        }
        res = pikaMaker_compileModule(self, uncompiled);
        if (PIKA_RES_OK != res) {
            obj_setInt(self, "err", res);
            return res;
        }
        pikaMaker_getDependencies(self, uncompiled);
    }
    return PIKA_RES_OK;
}

int32_t __foreach_handler_linkCompiledModules(Arg* argEach, Args* context) {
    Args buffs = {0};
    if (argType_isObject(arg_getType(argEach))) {
        LibObj* lib = args_getPtr(context, "@lib");
        PikaMaker* maker = args_getPtr(context, "__maker");
        PikaObj* module_obj = arg_getPtr(argEach);
        char* module_name = obj_getStr(module_obj, "name");
        char* state = obj_getStr(module_obj, "state");
        if (strEqu(state, "compiled")) {
            char* pwd = obj_getStr(maker, "pwd");
            char* folder_path = strsAppend(&buffs, pwd, "pikascript-api/");
            char* module_file_name = strsAppend(&buffs, module_name, ".py.o");
            char* module_file_path =
                strsAppend(&buffs, folder_path, module_file_name);
            LibObj_staticLinkFile(lib, module_file_path);
        }
    }
    strsDeinit(&buffs);
    return 0;
}

PIKA_RES pikaMaker_linkCompiledModulesFullPath(PikaMaker* self,
                                               char* lib_path) {
    PIKA_RES compile_err = obj_getInt(self, "err");
    if (PIKA_RES_OK != compile_err) {
        __platform_printf("  Error: compile failed, link aborted.\r\n");
        return compile_err;
    }
    Args context = {0};
    Args buffs = {0};
    __platform_printf("  linking %s...\n", lib_path);
    LibObj* lib = obj_getPtr(self, "lib");
    args_setPtr(&context, "@lib", lib);
    args_setPtr(&context, "__maker", self);
    args_foreach(self->list, __foreach_handler_linkCompiledModules, &context);
    args_deinit_stack(&context);
    char* pwd = obj_getStr(self, "pwd");
    char* lib_path_folder = strsCopy(&buffs, lib_path);
    strPopLastToken(lib_path_folder, '/');
    char* folder_path = strsAppend(&buffs, pwd, lib_path_folder);
    folder_path = strsAppend(&buffs, folder_path, "/");
    char* lib_file_path = strsAppend(&buffs, pwd, lib_path);
    LibObj_saveLibraryFile(lib, lib_file_path);
    Lib_loadLibraryFileToArray(lib_file_path, folder_path);
    strsDeinit(&buffs);
    return PIKA_RES_OK;
}

PIKA_RES pikaMaker_linkCompiledModules(PikaMaker* self, char* lib_name) {
    Args buffs = {0};
    char* lib_file_path = strsAppend(&buffs, "pikascript-api/", lib_name);
    PIKA_RES res = pikaMaker_linkCompiledModulesFullPath(self, lib_file_path);
    strsDeinit(&buffs);
    return res;
}

PIKA_RES pikaMaker_linkRaw(PikaMaker* self, char* file_path) {
    LibObj* lib = obj_getPtr(self, "lib");
    LibObj_staticLinkFile(lib, file_path);
    return 0;
}
