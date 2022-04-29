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

PikaLib* New_PikaLib(void) {
    return New_TinyObj(NULL);
}

void PikaLib_deinit(PikaLib* self) {
    obj_deinit(self);
}

void PikaLib_LinkByteCode(PikaLib* self, char* module_name, uint8_t* bytecode) {
}
