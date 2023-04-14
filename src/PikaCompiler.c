/*
 * This file is part of the PikaPython project.
 * http://github.com/pikastech/pikapython
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

/*
 * @brief check magic code of pyo file
 * @param bytecode
 * @return PIKA_TRUE or PIKA_FALSE
 */
static PIKA_BOOL _check_magic_code_pyo(uint8_t* bytecode) {
    char* data = (char*)bytecode;
    if (data[0] == magic_code_pyo[0] && data[1] == magic_code_pyo[1] &&
        data[2] == magic_code_pyo[2] && data[3] == magic_code_pyo[3]) {
        return PIKA_TRUE;
    }
    return PIKA_FALSE;
}

/*
 * @brief get bytecode from bytes arg
 * @param self bytes arg
 * @return bytecode
 */
static uint8_t* arg_getBytecode(Arg* self) {
    uint8_t* bytecode_file = arg_getBytes(self);
    if (_check_magic_code_pyo(bytecode_file)) {
        return bytecode_file + sizeof(magic_code_pyo) + sizeof(uint32_t);
    }
    return bytecode_file;
}

/*
 * @brief get bytecode size from bytes arg
 * @param self bytes arg
 * @return bytecode size
 */
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
    pika_platform_fwrite(content, 1, size, self->output_f);
}

/* instruct array output redirect */
static void __handler_instructArray_output_none(InstructArray* self,
                                                InstructUnit* ins_unit) {
    /* none */
}

static void __handler_instructArray_output_file(InstructArray* self,
                                                InstructUnit* ins_unit) {
    /* to flash */
    pika_platform_fwrite(ins_unit, 1, instructUnit_getSize(), self->output_f);
}

/*
    need implament :
        pika_platform_fopen()
        pika_platform_fwrite()
        pika_platform_fclose()
*/

PIKA_RES pikaCompile(char* output_file_name, char* py_lines) {
    PIKA_RES res = PIKA_RES_OK;
    ByteCodeFrame bytecode_frame = {0};
    uint32_t const_pool_size = 0;
    uint32_t instruct_array_size = 0;
    uint32_t bytecode_size = 0;
    char void_ = 0;
    FILE* bytecode_f = pika_platform_fopen(output_file_name, "wb+");
    if (NULL == bytecode_f) {
        pika_platform_printf("Error: open file %s failed.\r\n",
                             output_file_name);
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
    res = pika_lines2Bytes(&bytecode_frame, py_lines);
    if (PIKA_RES_OK != res) {
        pika_platform_printf("    Error: Syntax error.\r\n");
        goto exit;
    }
    const_pool_size = bytecode_frame.const_pool.size;
    instruct_array_size = bytecode_frame.instruct_array.size;
    bytecode_size = const_pool_size + instruct_array_size +
                    sizeof(const_pool_size) + sizeof(instruct_array_size);
    byteCodeFrame_deinit(&bytecode_frame);

    /* step 2, write instruct array to file */
    /* write magic code */
    pika_platform_fwrite(magic_code_pyo, 1, sizeof(magic_code_pyo), bytecode_f);
    /* write bytecode size */
    pika_platform_fwrite(&bytecode_size, 1, sizeof(bytecode_size), bytecode_f);
    /* write ins array size */
    pika_platform_fwrite(&instruct_array_size, 1, sizeof(instruct_array_size),
                         bytecode_f);
    byteCodeFrame_init(&bytecode_frame);
    bytecode_frame.const_pool.output_f = bytecode_f;
    bytecode_frame.instruct_array.output_f = bytecode_f;
    /* instruct array to file */
    bytecode_frame.instruct_array.output_redirect_fun =
        __handler_instructArray_output_file;
    pika_lines2Bytes(&bytecode_frame, py_lines);
    byteCodeFrame_deinit(&bytecode_frame);

    /* step 3, write const pool to file */
    pika_platform_fwrite(&const_pool_size, 1, sizeof(const_pool_size),
                         bytecode_f);
    void_ = 0;
    /* add \0 at the start */
    pika_platform_fwrite(&void_, 1, 1, bytecode_f);
    byteCodeFrame_init(&bytecode_frame);
    bytecode_frame.const_pool.output_f = bytecode_f;
    bytecode_frame.instruct_array.output_f = bytecode_f;
    /* const pool to file */
    bytecode_frame.const_pool.output_redirect_fun =
        __handler_constPool_output_file;
    /* instruct array to none */
    bytecode_frame.instruct_array.output_redirect_fun =
        __handler_instructArray_output_none;
    pika_lines2Bytes(&bytecode_frame, py_lines);

    /* deinit */
exit:
    byteCodeFrame_deinit(&bytecode_frame);
    if (NULL != bytecode_f) {
        pika_platform_fclose(bytecode_f);
    }
    /* succeed */
    return res;
};

/*
    need implament :
        pika_platform_fopen()
        pika_platform_fread()
        pika_platform_fwrite()
        pika_platform_fclose()
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
    LibObj* self = New_PikaObj();
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

/*
 * @brief add bytecode to lib, and copy the bytecode to the buff in the lib
 * @param self the lib obj
 * @param module_name the module name
 * @param bytecode the bytecode
 * @param size the size of the bytecode
 * @return error code
 */
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
        pika_platform_printf("error: can't open file %s\r\n", input_file_name);
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
        // pika_platform_printf("linking raw  %s:%s:%ld\r\n", input_file_name,
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

static int32_t __foreach_handler_listModules(Arg* argEach, void* context) {
    if (arg_isObject(argEach)) {
        PikaObj* module_obj = arg_getPtr(argEach);
        pika_platform_printf("%s\r\n", obj_getStr(module_obj, "name"));
    }
    return 0;
}

void LibObj_listModules(LibObj* self) {
    args_foreach(self->list, __foreach_handler_listModules, NULL);
}

static int32_t __foreach_handler_libWriteBytecode(Arg* argEach, void* context) {
    FILE* out_file = args_getPtr((Args*)context, "out_file");
    if (arg_isObject(argEach)) {
        PikaObj* module_obj = arg_getPtr(argEach);
        char* bytecode = obj_getPtr(module_obj, "bytecode");
        size_t bytecode_size = obj_getBytesSize(module_obj, "buff");
        size_t aline_size =
            align_by(bytecode_size, sizeof(uint32_t)) - bytecode_size;
        char aline_buff[sizeof(uint32_t)] = {0};
        pika_platform_fwrite(bytecode, 1, bytecode_size, out_file);
        pika_platform_fwrite(aline_buff, 1, aline_size, out_file);
    }
    return 0;
}

#define NAME_BUFF_SIZE LIB_INFO_BLOCK_SIZE - sizeof(uint32_t)
static int32_t __foreach_handler_libWriteIndex(Arg* argEach, void* context) {
    Args* args = context;
    FILE* out_file = args_getPtr(args, "out_file");
    Args buffs = {0};
    if (arg_isObject(argEach)) {
        PikaObj* module_obj = arg_getPtr(argEach);
        uint32_t bytecode_size = obj_getBytesSize(module_obj, "buff");
        char name_buff[NAME_BUFF_SIZE] = {0};
        // bytecode_size = align_by(bytecode_size, sizeof(uint32_t));
        char* module_name = obj_getStr(module_obj, "name");
        module_name = strsReplace(&buffs, module_name, "|", ".");
        // pika_platform_printf("   %s:%d\r\n", module_name, bytecode_size);
        pika_platform_memcpy(name_buff, module_name, strGetSize(module_name));
        pika_platform_fwrite(name_buff, 1, NAME_BUFF_SIZE, out_file);
        pika_platform_fwrite(&bytecode_size, 1, sizeof(bytecode_size),
                             out_file);
    }
    strsDeinit(&buffs);
    return 0;
}

static int32_t __foreach_handler_libSumSize(Arg* argEach, void* context) {
    Args* args = context;
    if (arg_isObject(argEach)) {
        PikaObj* module_obj = arg_getPtr(argEach);
        uint32_t bytecode_size = obj_getBytesSize(module_obj, "buff");
        bytecode_size = align_by(bytecode_size, sizeof(uint32_t));
        args_setInt(args, "sum_size",
                    args_getInt(args, "sum_size") + bytecode_size);
    }
    return 0;
}

static int32_t __foreach_handler_getModuleNum(Arg* argEach, void* context) {
    Args* args = (Args*)context;
    if (arg_isObject(argEach)) {
        args_setInt(args, "module_num", args_getInt(args, "module_num") + 1);
    }
    return 0;
}

int LibObj_saveLibraryFile(LibObj* self, char* output_file_name) {
    FILE* out_file = pika_platform_fopen(output_file_name, "wb+");

    Args context = {0};
    args_setPtr(&context, "out_file", out_file);
    args_setInt(&context, "module_num", 0);
    args_setInt(&context, "sum_size", 0);

    /* write meta information */
    char buff[LIB_INFO_BLOCK_SIZE] = {0};
    args_foreach(self->list, __foreach_handler_getModuleNum, &context);

    /* get sum size of pya */
    args_foreach(self->list, __foreach_handler_libSumSize, &context);

    /* meta info */
    char magic_code[] = {0x0f, 'p', 'y', 'a'};
    uint32_t version_num = LIB_VERSION_NUMBER;
    uint32_t module_num = args_getInt(&context, "module_num");
    uint32_t modules_size = args_getInt(&context, "sum_size") +
                            (module_num + 1) * LIB_INFO_BLOCK_SIZE -
                            sizeof(uint32_t) * 2;

    /* write meta info */
    const uint32_t magic_code_offset =
        sizeof(uint32_t) * PIKA_APP_MAGIC_CODE_OFFSET;
    const uint32_t modules_size_offset =
        sizeof(uint32_t) * PIKA_APP_MODULE_SIZE_OFFSET;
    const uint32_t version_offset = sizeof(uint32_t) * PIKA_APP_VERSION_OFFSET;
    const uint32_t module_num_offset =
        sizeof(uint32_t) * PIKA_APP_MODULE_NUM_OFFSET;

    pika_platform_memcpy(buff + magic_code_offset, &magic_code,
                         sizeof(uint32_t));
    pika_platform_memcpy(buff + version_offset, &version_num, sizeof(uint32_t));
    /* write module_num to the file */
    pika_platform_memcpy(buff + module_num_offset, &module_num,
                         sizeof(uint32_t));
    /* write modules_size to the file */
    pika_platform_memcpy(buff + modules_size_offset, &modules_size,
                         sizeof(uint32_t));
    /* aline to 32 bytes */
    pika_platform_fwrite(buff, 1, LIB_INFO_BLOCK_SIZE, out_file);
    /* write module index to file */
    args_foreach(self->list, __foreach_handler_libWriteIndex, &context);
    /* write module bytecode to file */
    args_foreach(self->list, __foreach_handler_libWriteBytecode, &context);
    args_deinit_stack(&context);
    /* main process */
    /* deinit */
    pika_platform_fclose(out_file);
    return 0;
}

static int _getModuleNum(uint8_t* library_bytes) {
    if (0 != ((intptr_t)library_bytes & 0x03)) {
        return PIKA_RES_ERR_UNALIGNED_PTR;
    }

    char* magic_code = (char*)library_bytes;

    uint32_t* library_info = (uint32_t*)library_bytes;
    uint32_t version_num = library_info[PIKA_APP_VERSION_OFFSET];
    uint32_t module_num = library_info[PIKA_APP_MODULE_NUM_OFFSET];

    /* check magic_code */
    if (!((magic_code[0] == 0x0f) && (magic_code[1] == 'p') &&
          (magic_code[2] == 'y') && (magic_code[3] == 'a'))) {
        pika_platform_printf("Error: invalid magic code.\r\n");
        return PIKA_RES_ERR_ILLEGAL_MAGIC_CODE;
    }
    /* check version num */
    if (version_num != LIB_VERSION_NUMBER) {
        pika_platform_printf(
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
        // pika_platform_printf("loading module: %s\r\n", module_name);
        *name_p = module_name;
        *addr_p = bytecode_addr;
        size_t module_size =
            *(uint32_t*)(module_name + LIB_INFO_BLOCK_SIZE - sizeof(uint32_t));
        *size = module_size;
        /* fix size for string */
        PIKA_BOOL bIsString = PIKA_TRUE;
        for (size_t i = 0; i < *size - 1; ++i) {
            if (bytecode_addr[i] == 0) {
                bIsString = PIKA_FALSE;
                break;
            }
        }
        if (bIsString) {
            /* remove the last '\0' for stirng */
            if (bytecode_addr[*size - 1] == 0) {
                *size -= 1;
            }
        }
        /* next module */
        bytecode_addr += align_by(module_size, sizeof(uint32_t));
    }
    return PIKA_RES_OK;
}

PIKA_RES _loadModuleDataWithName(uint8_t* library_bytes,
                                 char* module_name,
                                 uint8_t** addr_p,
                                 size_t* size_p) {
    int module_num = _getModuleNum(library_bytes);
    if (module_num < 0) {
        return (PIKA_RES)module_num;
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
            return PIKA_RES_OK;
        }
    }
    return PIKA_RES_ERR_ARG_NO_FOUND;
}

/**
 * @brief 打开 .pack 文件，并返回这个pack 文件的library_bytes
 *
 * @param pikafs_FILE** fp pikafs_FILE
 * 二级文件指针，提供了文件加载内存中的地址以及大小等信息
 * @param Arg** f_arg
 * @param char* pack_name pack 文件的名字
 * @return  PIKA_RES_OK when success, otherwise failed;
 * @note  if failed *fp if freed
 *
 */
PIKA_RES _getPack_libraryBytes(pikafs_FILE** fp, Arg** f_arg, char* pack_name) {
    if (NULL == pack_name) {
        return PIKA_RES_ERR_INVALID_PTR;
    }

    *fp = (pikafs_FILE*)pikaMalloc(sizeof(pikafs_FILE));
    if (NULL == *fp) {
        pika_platform_printf("Error: malloc failed \r\n");
        return PIKA_RES_ERR_OUT_OF_RANGE;
    }
    memset(*fp, 0, sizeof(pikafs_FILE));

    *f_arg = arg_loadFile(NULL, pack_name);
    if (NULL == *f_arg) {
        pika_platform_printf("Error: Could not load file \'%s\'\r\n",
                             pack_name);
        pikaFree(*fp, sizeof(pikafs_FILE));
        // fp == NULL;
        return PIKA_RES_ERR_IO_ERROR;
    }
    return PIKA_RES_OK;
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

int32_t __foreach_handler_printModule(Arg* argEach, void* context) {
    if (arg_isObject(argEach)) {
        PikaObj* module_obj = arg_getPtr(argEach);
        char* module_name = obj_getStr(module_obj, "name");
        if (NULL != module_name) {
            pika_platform_printf(module_name);
            pika_platform_printf("\r\n");
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
        pika_platform_printf("Error: Could not load library file '%s'\n",
                             lib_file_name);
        return PIKA_RES_ERR_IO_ERROR;
    }
    /* save file_arg as @lib_buf to libObj */
    obj_setArg_noCopy(self, "@lib_buf", file_arg);
    if (0 != LibObj_loadLibrary(self, arg_getBytes(file_arg))) {
        pika_platform_printf("Error: Could not load library from '%s'\n",
                             lib_file_name);
        return PIKA_RES_ERR_OPERATION_FAILED;
    }
    return PIKA_RES_OK;
}

/**
 * @brief unpack *.pack file to Specified path
 *
 * @param pack_name  the name of *.pack file
 * @param out_path   output path
 * @return
 */
PIKA_RES pikafs_unpack_files(char* pack_name, char* out_path) {
    PIKA_RES stat = PIKA_RES_OK;
    Arg* file_arg = NULL;
    uint8_t* library_bytes = NULL;
    pikafs_FILE* fptr = NULL;

    stat = _getPack_libraryBytes(&fptr, &file_arg, pack_name);
    if (PIKA_RES_OK == stat) {
        library_bytes = arg_getBytes(file_arg);
    } else {
        return stat;
    }

    int module_num = _getModuleNum(library_bytes);
    if (module_num < 0) {
        return (PIKA_RES)module_num;
    }

    Args buffs = {0};
    char* output_file_path = NULL;
    FILE* new_fp = NULL;

    for (int i = 0; i < module_num; ++i) {
        char* name = NULL;
        uint8_t* addr = NULL;
        size_t size = 0;
        _loadModuleDataWithIndex(library_bytes, module_num, i, &name, &addr,
                                 &size);
        output_file_path = strsPathJoin(&buffs, out_path, name);
        new_fp = pika_platform_fopen(output_file_path, "wb+");

        if (NULL != new_fp) {
            pika_platform_fwrite(addr, size, 1, new_fp);
            pika_platform_fclose(new_fp);
            pika_platform_printf("unpack %s to %s\r\n", name, output_file_path);
        } else {
            pika_platform_printf("can't open %s\r\n", output_file_path);
            break;
        }
    }

    arg_deinit(file_arg);
    strsDeinit(&buffs);
    pikaFree(fptr, sizeof(pikafs_FILE));
    return PIKA_RES_OK;
}

/** @brief pack files to *.pack file
 *
 * @param pack_name the name of *.pack file
 * @param ...       the name of files to pack
 * @return         PIKA_RES_OK if success
 */
PIKA_RES pikafs_pack_files(char* pack_name, int file_num, ...) {
    PikaMaker* maker = New_PikaMaker();
    PIKA_RES ret = PIKA_RES_OK;
    va_list args;
    va_start(args, file_num);
    for (int i = 0; i < file_num; i++) {
        char* file_name = va_arg(args, char*);
        ret = pikaMaker_linkRaw(maker, file_name);
        if (PIKA_RES_OK != ret) {
            goto __exit;
        }
    }
    ret = pikaMaker_linkCompiledModulesFullPath(maker, pack_name);
    if (PIKA_RES_OK != ret) {
        goto __exit;
    }
__exit:
    va_end(args);
    pikaMaker_deinit(maker);
    return ret;
}

size_t pika_fputs(char* str, FILE* fp) {
    size_t size = strGetSize(str);
    return pika_platform_fwrite(str, 1, size, fp);
}

int Lib_loadLibraryFileToArray(char* origin_file_name, char* out_folder) {
    Args buffs = {0};
    Arg* file_arg = arg_loadFile(NULL, origin_file_name);
    int res = 0;
    if (NULL == file_arg) {
        pika_platform_printf("Error: Could not load file '%s'\n",
                             origin_file_name);
        return 1;
    }
    char* output_file_name = NULL;
    output_file_name = strsGetLastToken(&buffs, origin_file_name, '/');
    output_file_name = strsAppend(&buffs, "__asset_", output_file_name);
    output_file_name = strsReplace(&buffs, output_file_name, ".", "_");
    output_file_name = strsAppend(&buffs, output_file_name, ".c");

    char* output_file_path = strsPathJoin(&buffs, out_folder, output_file_name);

    FILE* fp = pika_platform_fopen(output_file_path, "wb+");
    char* array_name = strsGetLastToken(&buffs, origin_file_name, '/');
    array_name = strsReplace(&buffs, array_name, ".", "_");
    pika_platform_printf("  loading %s[]...\n", array_name);
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
        pika_sprintf(byte_buff, "0x%02x, ", array[i]);
        pika_fputs(byte_buff, fp);
    }

    pika_fputs("\n};\n", fp);
    res = 0;
    goto exit;

exit:
    pika_platform_fclose(fp);
    strsDeinit(&buffs);
    arg_deinit(file_arg);
    return res;
}

static PIKA_RES __Maker_compileModuleWithInfo(PikaMaker* self,
                                              char* module_name) {
    Args buffs = {0};
    char* input_file_name = strsAppend(&buffs, module_name, ".py");
    char* input_file_path =
        strsPathJoin(&buffs, obj_getStr(self, "pwd"), input_file_name);
    pika_platform_printf("  compiling %s...\r\n", input_file_name);
    char* output_file_name = strsAppend(&buffs, module_name, ".py.o");
    char* output_file_path = NULL;
    output_file_path =
        strsPathJoin(&buffs, obj_getStr(self, "pwd"), "pikascript-api");
    output_file_path = strsPathJoin(&buffs, output_file_path, output_file_name);
    PIKA_RES res =
        pikaCompileFileWithOutputName(output_file_path, input_file_path);
    strsDeinit(&buffs);
    return res;
}

PikaMaker* New_PikaMaker(void) {
    PikaMaker* self = New_PikaObj();
    obj_setStr(self, "pwd", "");
    obj_setInt(self, "err", 0);
    LibObj* lib = New_LibObj(NULL);
    obj_setPtr(self, "lib", lib);
    return self;
}

/*
 * @brief: deinit PikaMaker
 * @param: self PikaMaker
 * @return: void
 */
void pikaMaker_deinit(PikaMaker* self) {
    LibObj* lib = obj_getPtr(self, "lib");
    LibObj_deinit(lib);
    obj_deinit(self);
}

/*
 * @brief: set pwd
 * @param: self PikaMaker
 * @param: pwd
 * @return: void
 */
void pikaMaker_setPWD(PikaMaker* self, char* pwd) {
    obj_setStr(self, "pwd", pwd);
}

/*
 * @brief: set state
 * @param: self PikaMaker
 * @param: module_name
 * @param: state
 * @return: void
 */
void pikaMaker_setState(PikaMaker* self, char* module_name, char* state) {
    obj_newMetaObj(self, module_name, New_TinyObj);
    PikaObj* module_obj = obj_getObj(self, module_name);
    obj_setStr(module_obj, "name", module_name);
    obj_setStr(module_obj, "state", state);
}

/*
 * @brief: compile module
 * @param: self PikaMaker
 * @param: module_name
 * @return: PIKA_RES
 */
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
        strsPathJoin(&buffs, obj_getStr(self, "pwd"), "pikascript-api/");
    module_path = strsPathJoin(&buffs, module_path, module_name);
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
        if (instructUnit_getInstructIndex(ins_unit) == IMP) {
            char* imp_module_name =
                constPool_getByOffset(const_pool, ins_unit->const_pool_index);
            char* imp_module_path =
                strsPathJoin(&buffs, obj_getStr(self, "pwd"), imp_module_name);
            /* check if compiled the module */
            if (obj_isArgExist(self, imp_module_name)) {
                /* module info is exist, do nothing */
            } else {
                /* module info is not exist */
                /* set module to be compile */
                FILE* imp_file_py = pika_platform_fopen(
                    strsAppend(&buffs, imp_module_path, ".py"), "rb");
                FILE* imp_file_pyi = pika_platform_fopen(
                    strsAppend(&buffs, imp_module_path, ".pyi"), "rb");
                FILE* imp_file_pyo = pika_platform_fopen(
                    strsAppend(&buffs, imp_module_path, ".py.o"), "rb");
                if (NULL != imp_file_pyo) {
                    pika_platform_printf("  loading %s.py.o...\r\n",
                                         imp_module_path);
                    /* found *.py.o, push to compiled list */
                    pikaMaker_setState(self, imp_module_name, "compiled");
                    char* imp_api_path = strsPathJoin(
                        &buffs, obj_getStr(self, "pwd"), "pikascript-api/");
                    imp_api_path =
                        strsPathJoin(&buffs, imp_api_path, imp_module_name);
                    FILE* imp_file_pyo_api = pika_platform_fopen(
                        strsAppend(&buffs, imp_api_path, ".py.o"), "wb+");
                    pika_assert(imp_file_pyo_api != NULL);
                    /* copy imp_file_pyo to imp_api_path */
                    uint8_t* buff = (uint8_t*)pika_platform_malloc(128);
                    size_t read_size = 0;
                    while (1) {
                        read_size =
                            pika_platform_fread(buff, 1, 128, imp_file_pyo);
                        if (read_size > 0) {
                            pika_platform_fwrite(buff, 1, read_size,
                                                 imp_file_pyo_api);
                        } else {
                            break;
                        }
                    }
                    pika_platform_free(buff);
                    pika_platform_fclose(imp_file_pyo_api);
                } else if (NULL != imp_file_py) {
                    /* found *.py, push to nocompiled list */
                    pikaMaker_setState(self, imp_module_name, "nocompiled");
                } else if (NULL != imp_file_pyi) {
                    /* found *.py, push to nocompiled list */
                    pikaMaker_setState(self, imp_module_name, "cmodule");
                } else {
                    pika_platform_printf(
                        "    [warning]: file: '%s.pyi', '%s.py' or '%s.py.o' "
                        "no found\n",
                        imp_module_name, imp_module_name, imp_module_name);
                }
                if (NULL != imp_file_pyo) {
                    pika_platform_fclose(imp_file_pyo);
                }
                if (NULL != imp_file_pyi) {
                    pika_platform_fclose(imp_file_pyi);
                }
                if (NULL != imp_file_py) {
                    pika_platform_fclose(imp_file_py);
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

int32_t __foreach_handler_printStates(Arg* argEach, void* context) {
    if (arg_isObject(argEach)) {
        PikaObj* module_obj = arg_getPtr(argEach);
        pika_platform_printf("%s: %s\r\n", obj_getStr(module_obj, "name"),
                             obj_getStr(module_obj, "state"));
    }
    return 0;
}

void pikaMaker_printStates(PikaMaker* self) {
    args_foreach(self->list, __foreach_handler_printStates, NULL);
}

int32_t __foreach_handler_getFirstNocompiled(Arg* argEach, void* context) {
    if (arg_isObject(argEach)) {
        PikaObj* module_obj = arg_getPtr(argEach);
        char* state = obj_getStr(module_obj, "state");
        if (args_isArgExist((Args*)context, "res")) {
            /* already get method */
            return 0;
        }
        if (strEqu("nocompiled", state)) {
            /* push module */
            args_setStr((Args*)context, "res", obj_getStr(module_obj, "name"));
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

/*
 * @brief compile module with depends
 * @param self PikaMaker
 * @param module_name
 * @return PIKA_RES
 */
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

int32_t __foreach_handler_linkCompiledModules(Arg* argEach, void* context) {
    Args buffs = {0};
    if (arg_isObject(argEach)) {
        LibObj* lib = args_getPtr((Args*)context, "@lib");
        PikaMaker* maker = args_getPtr((Args*)context, "__maker");
        PikaObj* module_obj = arg_getPtr(argEach);
        char* module_name = obj_getStr(module_obj, "name");
        char* state = obj_getStr(module_obj, "state");
        if (strEqu(state, "compiled")) {
            char* pwd = obj_getStr(maker, "pwd");
            char* folder_path = strsPathJoin(&buffs, pwd, "pikascript-api/");
            char* module_file_name = strsAppend(&buffs, module_name, ".py.o");
            char* module_file_path =
                strsPathJoin(&buffs, folder_path, module_file_name);
            LibObj_staticLinkFile(lib, module_file_path);
        }
    }
    strsDeinit(&buffs);
    return 0;
}

PIKA_RES _do_pikaMaker_linkCompiledModulesFullPath(PikaMaker* self,
                                                   char* lib_path,
                                                   PIKA_BOOL gen_c_array) {
    PIKA_RES compile_err = (PIKA_RES)obj_getInt(self, "err");
    if (PIKA_RES_OK != compile_err) {
        pika_platform_printf("  Error: compile failed, link aborted.\r\n");
        return compile_err;
    }
    Args context = {0};
    Args buffs = {0};
    pika_platform_printf("  linking %s...\n", lib_path);
    LibObj* lib = obj_getPtr(self, "lib");
    args_setPtr(&context, "@lib", lib);
    args_setPtr(&context, "__maker", self);
    args_foreach(self->list, __foreach_handler_linkCompiledModules, &context);
    args_deinit_stack(&context);
    char* pwd = obj_getStr(self, "pwd");
    char* lib_path_folder = strsPathGetFolder(&buffs, lib_path);
    char* folder_path = strsPathJoin(&buffs, pwd, lib_path_folder);
    char* lib_file_path = strsPathJoin(&buffs, pwd, lib_path);
    LibObj_saveLibraryFile(lib, lib_file_path);
    if (gen_c_array) {
        Lib_loadLibraryFileToArray(lib_file_path, folder_path);
    }
    strsDeinit(&buffs);
    return PIKA_RES_OK;
}

PIKA_RES pikaMaker_linkCompiledModulesFullPath(PikaMaker* self,
                                               char* lib_path) {
    return _do_pikaMaker_linkCompiledModulesFullPath(self, lib_path, PIKA_TRUE);
}

PIKA_RES _do_pikaMaker_linkCompiledModules(PikaMaker* self,
                                           char* lib_name,
                                           PIKA_BOOL gen_c_array) {
    Args buffs = {0};
    char* lib_file_path = strsPathJoin(&buffs, "pikascript-api/", lib_name);
    PIKA_RES res = _do_pikaMaker_linkCompiledModulesFullPath(
        self, lib_file_path, gen_c_array);
    strsDeinit(&buffs);
    return res;
}

PIKA_RES pikaMaker_linkCompiledModules(PikaMaker* self, char* lib_name) {
    return _do_pikaMaker_linkCompiledModules(self, lib_name, PIKA_TRUE);
}

/*
 * @brief link raw file to library
 * @param self PikaMaker
 * @param file_path
 * @return PIKA_RES
 */
PIKA_RES pikaMaker_linkRaw(PikaMaker* self, char* file_path) {
    LibObj* lib = obj_getPtr(self, "lib");
    LibObj_staticLinkFile(lib, file_path);
    return PIKA_RES_OK;
}

/*
 * @brief open file from library
 * @param file_name
 * @param mode "r" or "rb"
 * @return pikafs_FILE* or NULL if failed
 */
pikafs_FILE* pikafs_fopen(char* file_name, char* mode) {
    pikafs_FILE* f = (pikafs_FILE*)pikaMalloc(sizeof(pikafs_FILE));
    memset(f, 0, sizeof(pikafs_FILE));
    extern volatile PikaObj* __pikaMain;
    uint8_t* library_bytes = obj_getPtr((PikaObj*)__pikaMain, "@libraw");
    if (NULL == library_bytes) {
        return NULL;
    }
    if (PIKA_RES_OK !=
        _loadModuleDataWithName(library_bytes, file_name, &f->addr, &f->size)) {
        return NULL;
    }
    return f;
}

pikafs_FILE* pikafs_fopen_pack(char* pack_name, char* file_name) {
    pikafs_FILE* f = NULL;
    Arg* file_arg = NULL;
    PIKA_RES stat = PIKA_RES_OK;
    uint8_t* library_bytes = NULL;
    stat = _getPack_libraryBytes(&f, &file_arg, pack_name);
    if (PIKA_RES_OK == stat) {
        library_bytes = arg_getBytes(file_arg);
    } else {
        return NULL;
    }

    if (PIKA_RES_OK !=
        _loadModuleDataWithName(library_bytes, file_name, &f->addr, &f->size)) {
        return NULL;
    }

    f->addr = (uint8_t*)pikaMalloc(f->size);
    pika_platform_memcpy(f->addr, arg_getBytes(file_arg), f->size);
    f->need_free = PIKA_TRUE;

    arg_deinit(file_arg);
    return f;
}

/*
 * @brief read file
 * @param buf the buffer to read
 * @param size size of each item
 * @param count count of items
 * @param f file
 * @return read count
 */
int pikafs_fread(void* buf, size_t size, size_t count, pikafs_FILE* f) {
    if (f->pos >= f->size) {
        return 0;
    }
    if (f->pos + size * count > f->size) {
        count = (f->size - f->pos) / size;
    }
    __platform_memcpy(buf, f->addr + f->pos, size * count);
    f->pos += size * count;
    return count;
}

/*
 * @brief write file
 * @param buf the buffer to write
 * @param size size of each item
 * @param count count of items
 * @param f file
 * @return write count or -1 if failed
 */
int pikafs_fwrite(void* buf, size_t size, size_t count, pikafs_FILE* file) {
    return -1;
}

/*
 * @brief close file
 * @param f file
 * @return 0 if success
 */
int pikafs_fclose(pikafs_FILE* file) {
    if (file->need_free){
        pikaFree(file->addr, file->size);
    }
    pikaFree(file, sizeof(pikafs_FILE));
    return 0;
}
