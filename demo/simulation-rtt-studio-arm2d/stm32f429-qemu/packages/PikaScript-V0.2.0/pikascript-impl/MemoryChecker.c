#include "MimiObj.h"
#include <stdio.h>
extern DMEM_STATE DMEMS;
void MemoryChecker_size(MimiObj *self)
{
    char buff[256] = { 0 };
    sprintf(buff, "memory loop size = %0.2f kB\r\n", DMEM_TOTAL_SIZE / 1024.0);
    printf(buff);
}
void MemoryChecker_max(MimiObj *self)
{
    char buff[256] = { 0 };
    sprintf(buff, "memory used max = %0.2f kB\r\n", DMEMS.maxNum * DMEM_BLOCK_SIZE / 1024.0);
    printf(buff);
}
void MemoryChecker_now(MimiObj *self)
{
    char buff[256] = { 0 };
    sprintf(buff, "memory used now = %0.2f kB\r\n", DMEMS.blk_num * DMEM_BLOCK_SIZE / 1024.0);
    printf(buff);
}
