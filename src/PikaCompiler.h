#ifndef __PIKA_COMPILER__H
#define __PIKA_COMPILER__H
#include "PikaObj.h"
#include "stdint.h"

int pikaCompileFile(char* input_file_name);
int pikaCompileFileWithOutputName(char* output_file_name,
                                  char* input_file_name);
int pikaCompile(char* output_file_name, char* py_lines);

LibObj* New_LibObj(void);
void LibObj_deinit(LibObj* self);
void LibObj_dynamicLink(LibObj* self, char* module_name, uint8_t* bytecode);
int LibObj_staticLink(LibObj* self,
                      char* module_name,
                      uint8_t* bytecode,
                      size_t size);
int LibObj_staticLinkFile(LibObj* self, char* input_file_name);
void LibObj_listModules(LibObj* self);
int LibObj_saveLibraryFile(LibObj* self, char* output_file_name);
int LibObj_loadLibraryFile(LibObj* self, char* input_file_name);
int Lib_loadLibraryFileToArray(char* origin_file_name, char* pikascript_root);
PikaMaker* New_PikaMaker(void);
void pikaMaker_setPWD(PikaMaker* self, char* pwd);
void pikaMaker_compileModule(PikaMaker* self, char* module_name);


#define LIB_VERSION_NUMBER 1
#define LIB_INFO_BLOCK_SIZE 32

#endif