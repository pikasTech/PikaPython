#ifndef DAP_MAIN_H
#define DAP_MAIN_H


#include "usb_configuration.h"


#ifdef CONFIG_USB_HS
#if DAP_PACKET_SIZE != 512
#error "DAP_PACKET_SIZE must be 512 in hs"
#endif
#else
#if DAP_PACKET_SIZE != 64
#error "DAP_PACKET_SIZE must be 64 in fs"
#endif
#endif

extern volatile uint8_t USB_RequestIdle;
extern USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t USB_Request[DAP_PACKET_COUNT][DAP_PACKET_SIZE];

void chry_dap_handle(void);
void chry_dap_usb2uart_handle(void);


#endif