#ifdef __cplusplus
extern "C" {
#endif

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
int LibObj_linkFile(LibObj* self, char* output_file_name);
int LibObj_loadLibraryFile(LibObj* self, char* input_file_name);
PikaObj* LibObj_getModule(LibObj* self, char* module_name);
int Lib_loadLibraryFileToArray(char* origin_file_name, char* pikascript_root);
char* LibObj_redirectModule(LibObj* self, Args* buffs, char* module_name);
PikaMaker* New_PikaMaker(void);
void pikaMaker_setPWD(PikaMaker* self, char* pwd);
PIKA_RES pikaMaker_compileModule(PikaMaker* self, char* module_name);
int pikaMaker_getDependencies(PikaMaker* self, char* module_name);
void pikaMaker_printStates(PikaMaker* self);
char* pikaMaker_getFirstNocompiled(PikaMaker* self);
PIKA_RES pikaMaker_compileModuleWithDepends(PikaMaker* self, char* module_name);
PIKA_RES pikaMaker_linkCompiledModulesFullPath(PikaMaker* self, char* lib_path);
PIKA_RES pikaMaker_linkCompiledModules(PikaMaker* self, char* lib_name);
PIKA_RES _do_pikaMaker_linkCompiledModules(PikaMaker* self,
                                           char* lib_name,
                                           pika_bool gen_c_array);
int LibObj_loadLibrary(LibObj* self, uint8_t* library_bytes);
void LibObj_printModules(LibObj* self);
void pikaMaker_deinit(PikaMaker* self);
PIKA_RES pikaMaker_linkRaw(PikaMaker* self, char* file_path);
PIKA_RES pikaMaker_linkRawWithPath(PikaMaker* self,
                                   char* file_path,
                                   char* pack_path);
PIKA_RES _loadModuleDataWithName(uint8_t* library_bytes,
                                 char* module_name,
                                 uint8_t** addr_p,
                                 size_t* size_p);

#define LIB_VERSION_NUMBER 7
#define LIB_INFO_BLOCK_SIZE 32
#define PIKA_APP_MAGIC_CODE_OFFSET 0
#define PIKA_APP_MODULE_SIZE_OFFSET 1
#define PIKA_APP_VERSION_OFFSET 2
#define PIKA_APP_MODULE_NUM_OFFSET 3
#define PIKA_APP_INFO_BLOCK_SIZE_OFFSET 4

typedef struct {
    Arg* farg;
    uint8_t* addr;
    size_t size;
    size_t pos;
    pika_bool need_free;
} pikafs_FILE;

pikafs_FILE* pikafs_fopen(char* file_name, char* mode);
pikafs_FILE* pikafs_fopen_pack(char* pack_name, char* file_name);
int pikafs_fread(void* buf, size_t size, size_t count, pikafs_FILE* file);
int pikafs_fwrite(void* buf, size_t size, size_t count, pikafs_FILE* file);
int pikafs_fclose(pikafs_FILE* file);
PIKA_RES pikafs_unpack_files(char* pack_name, char* out_path);
PIKA_RES pikafs_pack_files(char* pack_name, int file_num, ...);
PIKA_RES pikaMaker_compileModuleWithList(PikaMaker* self, char* list_content);
PIKA_RES pikaMaker_compileModuleWithListFile(PikaMaker* self,
                                             char* list_file_name);

#endif
#ifdef __cplusplus
}
#endif
