#include "BaseObj.h"
#include "PikaCompiler.h"
#include "PikaObj.h"
#include "PikaParser.h"
#include "dataStrs.h"
#include "libpikabinder.h"
#include "pikascript/pikascript-core/dataStrs.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void help(char *argv0) {
  Args buffs = {0};
  char *exe = argv0;
  printf("Usage:\r\n"
         "    %s"
         " - [Binding C modules and compile all from main.py]\r\n"
         "    %s test.py"
         " - [Compile all from test.py]\r\n"
         "    %s test.py -o out.a"
         " - [Compile all from test.py and link to out.a]\r\n"
         "    %s -c test.py"
         " - [Only compile test.py to test.py.o]\r\n"
         "    %s -c test.py -o out.o"
         " - [Only compile test.py to out.o]\r\n",
         exe, exe, exe, exe, exe);
  strsDeinit(&buffs);
}

/* fake implement */
PikaObj *__pikaMain;
void New_PikaStdData_List(void) {}
void New_PikaStdData_Dict(void) {}
void New_PikaStdData_Tuple(void) {}
void New_PikaStdData_String(void) {}
void New_PikaStdData_ByteArray(void) {}
int strGetSizeUtf8(char *str) { return 0; }
void PikaStdData_Tuple___init__(PikaObj *self) {}
void PikaStdData_List___init__(PikaObj *self) {}
void PikaStdData_List_append(PikaObj *self, Arg *arg) {}
void PikaStdData_Dict___init__(PikaObj *self) {}
void PikaStdData_Dict_set(PikaObj *self, char *key, Arg *val) {}
PikaObj *New_builtins_object(Args *args){return NULL;};
PikaObj *New_PikaStdLib_SysObj(Args *args) { return NULL; };
PikaObj *New_builtins(Args *args) { return NULL; };
PikaObj *New_builtins_RangeObj(Args *args) { return NULL; }
PikaObj *New_PikaStdData_FILEIO(Args *args) { return NULL; }
char *string_slice(Args *outBuffs, char *str, int start, int end) {
  return NULL;
}
int PikaStdData_FILEIO_init(PikaObj *self, char *path, char *mode) { return 0; }

static int _do_main(int argc, char **argv) {
  int parc = argc - 1;
  PikaMaker *maker = New_PikaMaker();

  /* --add-file xxx --add-file yyy */
  // __platform_printf("parc: %d\r\n", parc);
  for (int i = 1; i < argc; i++) {
    // __platform_printf("%s\r\n", argv[i]);
    if (0 == strcmp(argv[i], "--add-file")) {
      // __platform_printf("add file: %s\r\n", argv[i + 1]);
      if (i + 1 < argc) {
        pikaMaker_linkRaw(maker, argv[i + 1]);
      }
    }
  }

  /* delete --xxx yyy */
  for (int i = 1; i < argc; i++) {
    if (0 == strcmp(argv[i], "--add-file")) {
      // printf("before delete: %d\r\n", parc);
      // for (int j = 0; j < parc; j++) {
      //     printf("%s\r\n", argv[j + 1]);
      // }
      parc -= 2;
      for (int j = i; j < argc - 2; j++) {
        argv[j] = argv[j + 2];
      }
      // printf("after delete: %d\r\n", parc);
      // for (int j = 0; j < parc; j++) {
      //     printf("%s\r\n", argv[j + 1]);
      // }
    }
  }

  if (0 == parc) {
    /* no input, default to main.py */
    /* run pika_binder to bind C modules */
    pika_binder();
    pikaMaker_compileModuleWithDepends(maker, "main");
    PIKA_RES res = pikaMaker_linkCompiledModules(maker, "pikaModules.py.a");
    pikaMaker_deinit(maker);
    return res;
  }

  /* example: ./rust-msc-latest-linux -h | --help */
  if (1 == parc) {
    if (0 == strcmp(argv[1], "-h") || 0 == strcmp(argv[1], "--help")) {
      help(argv[0]);
      return 0;
    }
  }

  /* example: ./rust-msc-latest-linux main.py */
  if (1 == parc) {
    char *module_entry = argv[1];
    /* remove subfix */
    char *subfix = strrchr(module_entry, '.');
    if (subfix) {
      *subfix = '\0';
    }
    pikaMaker_compileModuleWithDepends(maker, module_entry);
    PIKA_RES res = pikaMaker_linkCompiledModules(maker, "pikaModules.py.a");
    pikaMaker_deinit(maker);
    return res;
  }

  /* example ./rust-msc-latest-linux main.py -o out.a */
  if (3 == parc) {
    if (0 == strcmp(argv[2], "-o")) {
      char *module_entry = argv[1];
      /* remove subfix */
      char *subfix = strrchr(module_entry, '.');
      if (subfix) {
        *subfix = '\0';
      }
      pikaMaker_compileModuleWithDepends(maker, module_entry);
      PIKA_RES res = pikaMaker_linkCompiledModules(maker, argv[3]);
      pikaMaker_deinit(maker);
      return res;
    }
  }

  /* example ./rust-msc-latest --docgen main.py */
  if (2 == parc) {
    if (0 == strcmp(argv[1], "--docgen")) {
      Args buffs = {0};
      char *file_path = argv[2];
      char *file_name = strsPathGetFileName(&buffs, file_path);
      printf("file_name: %s\r\n", file_name);
      char *file_folder = strsPathGetFolder(&buffs, file_path);
      char *file_name_no_ext = strsGetFirstToken(&buffs, file_name, '.');
      printf("file_name_no_ext: %s\r\n", file_name_no_ext);
      char *file_out_name = strsAppend(&buffs, file_name_no_ext, ".md");
      printf("file_out_name: %s\r\n", file_out_name);
      char *file_out_path = strsPathJoin(&buffs, file_folder, file_out_name);
      Parser *parser = parser_create();
      printf("generating doc %s: %s\r\n", file_path, file_out_path);
      parser_file2DocFile(parser, file_path, file_out_path);
      strsDeinit(&buffs);
      parser_deinit(parser);
      return 0;
    }
  }

  /* example: ./rust-msc-latest-linux -docgen main.py -o main.md */
  if (4 == parc) {
    if (0 == strcmp(argv[1], "--docgen") && 0 == strcmp(argv[3], "-o")) {
      Args buffs = {0};
      char *file_path = argv[2];
      char *file_out_path = argv[4];
      Parser *parser = parser_create();
      printf("generating doc %s: %s\r\n", file_path, file_out_path);
      parser_file2DocFile(parser, file_path, file_out_path);
      strsDeinit(&buffs);
      parser_deinit(parser);
      return 0;
    }
  }

  /* example: ./rust-msc-latest-linux -c main.py */
  if (2 == parc) {
    if (0 == strcmp(argv[1], "-c")) {
      Args buffs = {0};
      /* compile only */
      char *module_entry = argv[2];
      char *module_out = strsCopy(&buffs, module_entry);
      char *subfix = strrchr(module_out, '.');
      if (subfix) {
        *subfix = '\0';
      }
      module_out = strsAppend(&buffs, module_out, ".py.o");
      printf("compiling %s to %s...\r\n", module_entry, module_out);
      PIKA_RES res = pikaCompileFileWithOutputName(module_out, module_entry);
      strsDeinit(&buffs);
      return res;
    }
  }

  /* example: ./rust-msc-latest-linux -c main.py -o main.py.o */
  if (4 == parc) {
    if (0 == strcmp(argv[1], "-c") && 0 == strcmp(argv[3], "-o")) {
      /* compile only */
      char *module_entry = argv[2];
      printf("compiling %s to %s...\r\n", module_entry, argv[4]);
      PIKA_RES res = pikaCompileFileWithOutputName(argv[4], module_entry);
      return res;
    }
  }

  /* no valid input */
  printf("Invalid input.\r\n");
  help(argv[0]);

  return -1;
}

int main(int argc, char **argv) {
  char *buf = NULL;
  FILE *pika_studio = NULL;
  if (argc == 1) {
    pika_studio = __platform_fopen("pika.studio", "r");
    if (NULL != pika_studio) {
      buf = __platform_malloc(1024);
      __platform_fread(buf, 1, 1024, pika_studio);
      /* find <args> </args> */
      char *start = strstr(buf, "<args>");
      char *end = strstr(buf, "</args>");
      if (start && end) {
        start += 6;
        *end = '\0';
        __platform_printf("args: %s\r\n", start);
        /* split args */
        char *args[32] = {0};
        args[0] = argv[0];
        char *p = start;
        while (p < end) {
          char *q = strchr(p, ' ');
          if (q) {
            *q = '\0';
            args[argc++] = p;
            p = q + 1;
          } else {
            args[argc++] = p;
            break;
          }
        }
        argv = args;
        // for (int i = 0; i < argc; i++) {
        //     __platform_printf("argv[%d]: %s\r\n", i, argv[i]);
        // }
      }
    }
  }
  int ret = _do_main(argc, argv);
  if (NULL != buf) {
    __platform_free(buf);
  }
  if (NULL != pika_studio) {
    __platform_fclose(pika_studio);
  }
  return ret;
}
