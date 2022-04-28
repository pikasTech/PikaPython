#ifndef __PIKA_COMPILER__H
#define __PIKA_COMPILER__H

int pikaCompileFile(char* input_file_name);
int pikaCompileFileWithOutputName(char* output_file_name, char* input_file_name);
int pikaCompile(char* output_file_name, char* py_lines);

#endif