/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __PikaStdData_FILEIO__H
#define __PikaStdData_FILEIO__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_PikaStdData_FILEIO(Args *args);

void PikaStdData_FILEIO_close(PikaObj *self);
void PikaStdData_FILEIO_init(PikaObj *self, char* path, char* mode);
Arg* PikaStdData_FILEIO_read(PikaObj *self, int size);
char* PikaStdData_FILEIO_readline(PikaObj *self);
PikaObj* PikaStdData_FILEIO_readlines(PikaObj *self);
int PikaStdData_FILEIO_seek(PikaObj *self, int offset, PikaTuple* fromwhere);
int PikaStdData_FILEIO_tell(PikaObj *self);
int PikaStdData_FILEIO_write(PikaObj *self, Arg* s);
void PikaStdData_FILEIO_writelines(PikaObj *self, PikaObj* lines);

#endif
