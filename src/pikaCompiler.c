#include "pikaCompiler.h"
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
    char* file_buff = __platform_malloc(PIKA_READ_FILE_BUFF_SIZE);
    FILE* input_f = __platform_fopen(input_file_name, "r");
    __platform_fread(file_buff, 1, PIKA_READ_FILE_BUFF_SIZE, input_f);
    pikaCompile(output_file_name, file_buff);
    __platform_free(file_buff);
    __platform_fclose(input_f);
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
    obj_newObj(self, "index", "", New_TinyObj);
    return self;
}

void LibObj_deinit(LibObj* self) {
    obj_deinit(self);
}

/* add bytecode to lib, not copy the bytecode */
void LibObj_LinkByteCode(LibObj* self, char* module_name, uint8_t* bytecode) {
    PikaObj* index_obj = obj_getObj(self, "index");
    obj_newObj(index_obj, module_name, "", New_TinyObj);
    PikaObj* module_obj = obj_getObj(index_obj, module_name);
    obj_setStr(module_obj, "name", module_name);
    obj_setPtr(module_obj, "bytecode", bytecode);
}

/* add bytecode to lib, and copy the bytecode to the buff in the lib */
int LibObj_pushByteCode(LibObj* self,
                        char* module_name,
                        uint8_t* bytecode,
                        size_t size) {
    PikaObj* index_obj = obj_getObj(self, "index");
    PikaObj* module_obj = obj_getObj(index_obj, module_name);
    /* copy bytecode to buff */
    obj_setBytes(module_obj, "buff", bytecode, size);
    /* link to buff */
    LibObj_LinkByteCode(self, module_name, obj_getBytes(module_obj, "buff"));
    return 0;
}

int LibObj_pushByteCodeFile(LibObj* self, char* input_file_name) {
    char* file_buff = __platform_malloc(PIKA_READ_FILE_BUFF_SIZE);
    Args buffs = {0};
    FILE* input_f = __platform_fopen(input_file_name, "r");
    /* read file */
    size_t size =
        __platform_fread(file_buff, 1, PIKA_READ_FILE_BUFF_SIZE, input_f);
    char* module_name = strsGetLastToken(&buffs, input_file_name, '/');

    /* cut off '.py.o' */
    module_name[strlen(module_name) - (sizeof(".py.o") - 1)] = 0;

    /* push bytecode */
    LibObj_pushByteCode(self, module_name, (uint8_t*)file_buff, size);

    /* deinit */
    __platform_free(file_buff);
    __platform_fclose(input_f);
    strsDeinit(&buffs);
    return 0;
}
