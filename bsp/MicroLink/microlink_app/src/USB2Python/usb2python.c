#include "board.h"
#include "usb2python.h"
#include "chry_ringbuffer.h"
#include "pikascript.h"
#include "PikaObj.h"
#include "PikaVM.h"
#include "ff.h"
#include "SEGGER_RTTView.h"

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t cdc1_tmpbuffer[DAP_PACKET_SIZE];

static volatile bool usbtx_idle_flag = false,usbrx_idle_flag = false;
volatile struct cdc_line_coding g_cdc1_lincoding;
volatile uint8_t config_uart_python = 0;
volatile uint8_t config_uart_open = 0;
static PikaObj* pikaMain = NULL;

USB_NOCACHE_RAM_SECTION chry_ringbuffer_t g_python_usbtx;
static USB_NOCACHE_RAM_SECTION chry_ringbuffer_t g_python_usbrx;

static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t usbtx_ringbuffer[CONFIG_UARTRX_RINGBUF_SIZE];
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t usbrx_ringbuffer[CONFIG_USBRX_RINGBUF_SIZE];

volatile bool ep_tx_busy_flag = false;
extern void led_usb_in_activity(void);
extern void led_usb_out_activity(void);
int pika_platform_putchar(char ch) 
{
    if(config_uart_open && pikaMain != NULL){
        while (ep_tx_busy_flag);
        usbd_ep_start_write(0, CDC_IN_EP1, (const uint8_t *)&ch, 1);
        ep_tx_busy_flag = true;
        while (ep_tx_busy_flag);
    }else{
        chry_ringbuffer_write(&g_python_usbtx, &ch, 1);
    }
    return 1;
}


static void usbd_cdc_acm_bulk_out(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    (void) busid;
    chry_ringbuffer_write(&g_python_usbrx, cdc1_tmpbuffer, nbytes);
    usbd_ep_start_read(busid, ep, &cdc1_tmpbuffer[0], usbd_get_ep_mps(busid, ep));  
}

static void usbd_cdc_acm_bulk_in(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    (void) busid;
    chry_ringbuffer_linear_read_done(&g_python_usbtx, nbytes);
    if ((nbytes % usbd_get_ep_mps(busid, ep)) == 0 && nbytes) {
        /* send zlp */
        usbd_ep_start_write(busid, ep, NULL, 0);
    }
    ep_tx_busy_flag = false;
}

struct usbd_endpoint cdc_out_ep1 = {
        .ep_addr = CDC_OUT_EP1,
        .ep_cb = usbd_cdc_acm_bulk_out
};

struct usbd_endpoint cdc_in_ep1 = {
        .ep_addr = CDC_IN_EP1,
        .ep_cb = usbd_cdc_acm_bulk_in
};

void pika_script_Init(void)
{
    chry_ringbuffer_init(&g_python_usbtx, usbtx_ringbuffer, CONFIG_USBRX_RINGBUF_SIZE);
    chry_ringbuffer_init(&g_python_usbrx, usbrx_ringbuffer, CONFIG_USBRX_RINGBUF_SIZE);    
    pikaMain = pikaScriptInit();
    if(pikaMain){
        pikaVM_runSingleFile(pikaMain,"flm_config.py");
    }
}


void chry_dap_pikapython_handle(void)
{
    uint8_t *buffer;
    uint32_t size;
    uint8_t ch ;
    static uint32_t delay_count = 50000;
    if(config_uart_python){
        if(delay_count != 0){
            delay_count--;
        }
        if(delay_count == 0){
            config_uart_open = 1;
            if (chry_ringbuffer_get_used(&g_python_usbtx) && ep_tx_busy_flag == false) {
                ep_tx_busy_flag = true;
                buffer = chry_ringbuffer_linear_read_setup(&g_python_usbtx,&size);
                usbd_ep_start_write(0, CDC_IN_EP1, buffer, size);
            }

            if (chry_ringbuffer_get_used(&g_python_usbrx)) {
                chry_ringbuffer_read_byte(&g_python_usbrx,&ch);
                if(receive_usb_and_write_rtt(ch) == 0){            
                    if(pikaMain != NULL){
                        obj_runChar(pikaMain, ch);
                    }
                }
            }
        }
    }
}




