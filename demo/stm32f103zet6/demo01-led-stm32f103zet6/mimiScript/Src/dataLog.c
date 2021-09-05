#include "dataLog.h"
#include "dataMemory.h"
#include "dataString.h"
static void print(Loger *loger, char *str)
{
    //get size of log
    loger->log_size += (sizeof(char) * strGetSize(str));
    //get the memory of log
    DMEM *new_log_mem = DynMemGet(loger->log_size);
    ((char *)(new_log_mem->addr))[0] = 0;
    //copy old log to new log
    if (NULL != loger->log)
    {
        strAppend_unlimited(new_log_mem->addr, loger->log);
    }
    //add str to log
    strAppend_unlimited(new_log_mem->addr, str);
    //free the old log
    DynMemPut(loger->log_mem);
    //update the pointer of log
    loger->log_mem = new_log_mem;
    loger->log = new_log_mem->addr;
}

static void deinit(Loger *loger)
{
    DynMemPut(loger->mem);
    DynMemPut(loger->log_mem);
}

Loger *dataLog_init(void)
{
    DMEM *mem = DynMemGet(sizeof(Loger));
    Loger *loger = (Loger *)mem->addr;
    loger->mem = mem;
    loger->log_mem = NULL;
    loger->log = NULL;
    loger->log_size = 0;
    loger->print = print;
    loger->deinit = deinit;
    return loger;
}
