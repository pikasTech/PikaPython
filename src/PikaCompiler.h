#ifndef __PIKA_COMPILER__H
#define __PIKA_COMPILER__H
#include "PikaObj.h"
#include "stdint.h"

PIKA_RES pikaCompileFile(char* input_file_name);
PIKA_RES pikaCompileFileWithOutputName(char* output_file_name,
                                       char* input_file_name);
PIKA_RES pikaCompile(char* output_file_name, char* py_lines);

LibObj* New_LibObj(Args* args);
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
PIKA_RES pikaMaker_compileModule(PikaMaker* self, char* module_name);
int pikaMaker_getDependencies(PikaMaker* self, char* module_name);
void pikaMaker_printStates(PikaMaker* self);
char* pikaMaker_getFirstNocompiled(PikaMaker* self);
PIKA_RES pikaMaker_compileModuleWithDepends(PikaMaker* self, char* module_name);
PIKA_RES pikaMaker_linkCompiledModulesFullPath(PikaMaker* self, char* lib_path);
PIKA_RES pikaMaker_linkCompiledModules(PikaMaker* self, char* lib_name);
int LibObj_loadLibrary(LibObj* self, uint8_t* library_bytes);
void LibObj_printModules(LibObj* self);
void pikaMaker_deinit(PikaMaker* self);
PIKA_RES pikaMaker_linkRaw(PikaMaker* self, char* file_path);
PIKA_RES _loadModuleDataWithName(uint8_t* library_bytes,
                                 char* module_name,
                                 uint8_t** addr_p,
                                 size_t* size_p);

#define LIB_VERSION_NUMBER 2
#define LIB_INFO_BLOCK_SIZE 32

#endif
