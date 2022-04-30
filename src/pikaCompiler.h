#ifndef __PIKA_COMPILER__H
#define __PIKA_COMPILER__H
#include "PikaObj.h"
#include "stdint.h"

int pikaCompileFile(char* input_file_name);
int pikaCompileFileWithOutputName(char* output_file_name,
                                  char* input_file_name);
int pikaCompile(char* output_file_name, char* py_lines);

typedef PikaObj LibObj;
LibObj* New_LibObj(void);
void LibObj_deinit(LibObj* self);
void LibObj_linkByteCode(LibObj* self, char* module_name, uint8_t* bytecode);
int LibObj_pushByteCode(LibObj* self,
                        char* module_name,
                        uint8_t* bytecode,
                        size_t size);
int LibObj_pushByteCodeFile(LibObj* self, char* input_file_name);
void LibObj_listModules(LibObj* self);
#endif