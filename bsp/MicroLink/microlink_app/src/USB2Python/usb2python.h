#ifndef HSLINK_PRO_USB2PYTHON_H
#define HSLINK_PRO_USB2PYTHON_H
#include "usb_configuration.h"


extern USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t cdc1_tmpbuffer[];

extern volatile struct cdc_line_coding g_cdc1_lincoding;
extern void chry_dap_pikapython_handle(void);
extern void pika_script_Init(void);
extern volatile uint8_t config_uart_python;
#endif //HSLINK_PRO_USB2UART_H
