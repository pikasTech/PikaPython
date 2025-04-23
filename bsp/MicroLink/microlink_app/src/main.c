/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_dma_mgr.h"
#include "usb_config.h"
#include "usb_configuration.h"
#include "dap_main.h"
#include "usb2uart.h"
#include "usb2python.h"
#include "microboot.h"
#include "ff.h"
#include "ymodem_send.h"
#include "chry_ringbuffer.h"
#include "SEGGER_RTTView.h"

ATTR_RAMFUNC_WITH_ALIGNMENT(4) 
static uint8_t s_chBuffer[1024] ;
static byte_queue_t                  s_tCheckUsePeekQueue;
static fsm(check_use_peek)           s_fsmCheckUsePeek;
static check_shell_t                 s_tShellObj;
#define LED_FLASH_PERIOD_IN_MS 10
static volatile uint8_t ledUsbInActivity = 0;
static volatile uint8_t ledUsbOutActivity = 0;
static void usb_led_toggle(void);

extern USB_NOCACHE_RAM_SECTION chry_ringbuffer_t g_usbrx;
extern USB_NOCACHE_RAM_SECTION chry_ringbuffer_t g_uartrx;
extern USB_NOCACHE_RAM_SECTION chry_ringbuffer_t g_python_usbtx;
extern ymodem_lib_send_t tYmodemLibSend;
extern FRESULT flash_mount_fs(void);

int main(void)
{
    board_init();
    dma_mgr_init();
    board_init_gpio_pins();
    board_init_led_pins();
    board_init_usb((USB_Type *)CONFIG_HPM_USBD_BASE);
    intc_set_irq_priority(CONFIG_HPM_USBD_IRQn, 2);
    FRESULT fresult =  flash_mount_fs();
    USB_Configuration();
    uartx_preinit();
    if(fresult == FR_OK){
        pika_script_Init();
    }
    board_timer_create(LED_FLASH_PERIOD_IN_MS, usb_led_toggle);
    queue_init(&s_tCheckUsePeekQueue, s_chBuffer, sizeof(s_chBuffer));
    init_fsm(check_use_peek, &s_fsmCheckUsePeek, args(&s_tCheckUsePeekQueue));
   
    ymodem_lib_send_init(&tYmodemLibSend, get_read_byte_interface(&s_fsmCheckUsePeek));

    connect(&tUartMsgObj, SIGNAL(uart_sig), &s_tCheckUsePeekQueue, SLOT(enqueue_bytes));    
    connect(&tUartMsgObj, SIGNAL(uart_sig), &g_uartrx, SLOT(chry_ringbuffer_write));  
    connect(&tRTTMsgObj, SIGNAL(rtt_sig), &g_python_usbtx, SLOT(chry_ringbuffer_write)); 
    connect(&tYmodemLibSend.tYmodemSent, SIGNAL(ymodem_send_sig), &g_usbrx, SLOT(chry_ringbuffer_write));   
    connect(&tYmodemLibSend.tYmodemSent, SIGNAL(ymodem_send_sig), &g_uartrx, SLOT(chry_ringbuffer_write));     
     
    while(1) {
        chry_dap_handle();
        chry_dap_usb2uart_handle();
        usb2uart_handler();
        chry_dap_pikapython_handle();
        call_fsm( check_use_peek,  &s_fsmCheckUsePeek );
    }
    return 0;
}

int64_t get_system_time_ms(void)
{
    return hpm_csr_get_core_cycle() / (int64_t)clock_get_core_clock_ticks_per_ms();
}

void ymodme_agent_register(void)
{
    agent_register(&s_fsmCheckUsePeek, &tYmodemLibSend.tCheckAgent);
}

void ymodme_unagent_register(void)
{
    agent_unregister(&s_fsmCheckUsePeek, &tYmodemLibSend.tCheckAgent);
}


static void usb_led_toggle(void)
{
    static uint8_t usb_in_led_value = BOARD_LED_OFF_LEVEL;
    static uint8_t usb_out_led_value = BOARD_LED_OFF_LEVEL;
    if(ledUsbInActivity){
         usb_in_led_value = BOARD_LED_ON_LEVEL == usb_in_led_value ? BOARD_LED_OFF_LEVEL : BOARD_LED_ON_LEVEL;
         gpio_write_pin(BOARD_LED1_GPIO_CTRL, BOARD_LED1_GPIO_INDEX, BOARD_LED1_GPIO_PIN,usb_in_led_value);
         if(usb_in_led_value == BOARD_LED_OFF_LEVEL){
             ledUsbInActivity = 0;
         }
    }else{
         gpio_write_pin(BOARD_LED1_GPIO_CTRL, BOARD_LED1_GPIO_INDEX, BOARD_LED1_GPIO_PIN,BOARD_LED_OFF_LEVEL);
    }

    if(ledUsbOutActivity){
         usb_out_led_value = BOARD_LED_ON_LEVEL == usb_out_led_value ? BOARD_LED_OFF_LEVEL : BOARD_LED_ON_LEVEL;
         gpio_write_pin(BOARD_LED2_GPIO_CTRL, BOARD_LED2_GPIO_INDEX, BOARD_LED2_GPIO_PIN,usb_out_led_value);
         if(usb_out_led_value == BOARD_LED_OFF_LEVEL){
             ledUsbOutActivity = 0;
         }
    }else{
         gpio_write_pin(BOARD_LED2_GPIO_CTRL, BOARD_LED2_GPIO_INDEX, BOARD_LED2_GPIO_PIN,BOARD_LED_OFF_LEVEL);
    }
}

void led_usb_in_activity(void)
{
    ledUsbInActivity = 1;
    return;
}

void led_usb_out_activity(void)
{
    ledUsbOutActivity = 1;
    return;
}

void reboot(void)
{
    HPM_PPOR->RESET_ENABLE |= HPM_BITSMASK(1UL, 31); // Enable Software reset   HPM_PPOR->RESET_HOT &= ~HPM_BITSMASK(1UL, 31);
    HPM_PPOR->SOFTWARE_RESET = 1000U;
    while(1)
    {
    }
}
