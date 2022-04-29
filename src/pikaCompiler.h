#ifndef __PIKA_COMPILER__H
#define __PIKA_COMPILER__H
#include "PikaObj.h"

int pikaCompileFile(char* input_file_name);
int pikaCompileFileWithOutputName(char* output_file_name,
                                  char* input_file_name);
int pikaCompile(char* output_file_name, char* py_lines);

typedef PikaObj PikaLib;
PikaLib* New_PikaLib(void);
void PikaLib_deinit(PikaLib* self);
void PikaLib_importByteCodeFrame(PikaLib* self,
                                 char* module_name,
                                 ByteCodeFrame* bf);

#endif