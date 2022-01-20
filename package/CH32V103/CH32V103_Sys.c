#include "CH32V103_Sys.h"
#include "debug.h"
#include <rtthread.h>

void CH32V103_Sys_delay(PikaObj *self, int ms){
    rt_thread_delay(ms);
}
