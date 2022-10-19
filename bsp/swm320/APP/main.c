#include "main.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lv_port_fs.h"
#include "lvgl.h"
#include "pikaScript.h"

#define SYSTEM_LOG(_fun) printf("%s...\r\n", #_fun);_fun

char __platform_getchar(void){
    uint32_t chr;
    uint32_t err;

    while(UART_IsRXFIFOEmpty(UART0));
    err = UART_ReadByte(UART0, &chr);
    if(err == 0)
    {
        return (char)chr;
    }
    return 0;
}

int main(void)
{
    SystemInit();

    systick_init();
    serial_init();
    SYSTEM_LOG(lcd_memory_init());

    SYSTEM_LOG(lv_init());
    SYSTEM_LOG(lv_port_disp_init());
    SYSTEM_LOG(lv_port_indev_init());
    SYSTEM_LOG(lv_port_fs_init());
     
    pikaScriptShell(pikaScriptInit());

    while (1 == 1)
    {
        lv_task_handler();
        gui2show();
    }
    
    typedef struct NativeMethod {
    const Hash hash;
    const char* name;
    const char* pars;
    const void* funPtr;
    } NativeMethod;
    
    const NativeMethod methodGroup[] = {
        {
            .hash = 0,
            .name = NULL,
            .pars = NULL,
            .funPtr = NULL,
        },
    };
}

uint8_t tick_indev = 0;
void SysTick_Handler_cb(void)
{
    lv_tick_inc(1);
    tick_indev++;
    if (tick_indev > 30)
    {
        tick_indev = 0;
        GT911_Scan();
    }
}

void SysTick_Handler(void)
{
    swm_inctick();
    SysTick_Handler_cb();
}
