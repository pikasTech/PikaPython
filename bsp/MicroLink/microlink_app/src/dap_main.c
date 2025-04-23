#include "dap_main.h"
#include "usbd_core.h"
#include "SEGGER_RTTView.h"

static volatile uint16_t USB_RequestIndexI = 0; // Request  Index In
static volatile uint16_t USB_RequestIndexO = 0; // Request  Index Out
static volatile uint16_t USB_RequestCountI = 0; // Request  Count In
static volatile uint16_t USB_RequestCountO = 0; // Request  Count Out
volatile uint8_t USB_RequestIdle = 1;    // Request  Idle  Flag

static volatile uint16_t USB_ResponseIndexI = 0; // Response Index In
static volatile uint16_t USB_ResponseIndexO = 0; // Response Index Out
static volatile uint16_t USB_ResponseCountI = 0; // Response Count In
static volatile uint16_t USB_ResponseCountO = 0; // Response Count Out
static volatile uint8_t USB_ResponseIdle = 1;    // Response Idle  Flag

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t USB_Request[DAP_PACKET_COUNT][DAP_PACKET_SIZE];  // Request  Buffer
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t USB_Response[DAP_PACKET_COUNT][DAP_PACKET_SIZE]; // Response Buffer
static uint16_t USB_RespSize[DAP_PACKET_COUNT];                                                        // Response Size
extern void led_usb_in_activity(void);
extern void led_usb_out_activity(void);
void dap_out_callback(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    (void) busid;
    led_usb_out_activity();
    if (USB_Request[USB_RequestIndexI][0] == ID_DAP_TransferAbort) {
        DAP_TransferAbort = 1U;
    } else {
        USB_RequestIndexI++;
        if (USB_RequestIndexI == DAP_PACKET_COUNT) {
            USB_RequestIndexI = 0U;
        }
        USB_RequestCountI++;
    }

    // Start reception of next request packet
    if ((uint16_t) (USB_RequestCountI - USB_RequestCountO) != DAP_PACKET_COUNT) {
        usbd_ep_start_read(0, DAP_OUT_EP, USB_Request[USB_RequestIndexI], DAP_PACKET_SIZE);
    } else {
        USB_RequestIdle = 1U;
    }
}

void dap_in_callback(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    (void) busid;
    led_usb_in_activity();
    if (USB_ResponseCountI != USB_ResponseCountO) {
        // Load data from response buffer to be sent back
        usbd_ep_start_write(0, DAP_IN_EP, USB_Response[USB_ResponseIndexO], USB_RespSize[USB_ResponseIndexO]);
        USB_ResponseIndexO++;
        if (USB_ResponseIndexO == DAP_PACKET_COUNT) {
            USB_ResponseIndexO = 0U;
        }
        USB_ResponseCountO++;
    } else {
        USB_ResponseIdle = 1U;
    }
}


struct usbd_endpoint dap_out_ep = {
        .ep_addr = DAP_OUT_EP,
        .ep_cb = dap_out_callback
};

struct usbd_endpoint dap_in_ep = {
        .ep_addr = DAP_IN_EP,
        .ep_cb = dap_in_callback
};


void chry_dap_handle(void)
{
    uint32_t n;
    extern int64_t get_system_time_ms(void);
    static int64_t current_time = 0;
    // Process pending requests
    while (USB_RequestCountI != USB_RequestCountO) {
        // Handle Queue Commands
        n = USB_RequestIndexO;
        while (USB_Request[n][0] == ID_DAP_QueueCommands) {
            USB_Request[n][0] = ID_DAP_ExecuteCommands;
            n++;
            if (n == DAP_PACKET_COUNT) {
                n = 0U;
            }
            if (n == USB_RequestIndexI) {
                // flags = osThreadFlagsWait(0x81U, osFlagsWaitAny, osWaitForever);
                // if (flags & 0x80U) {
                //     break;
                // }
            }
        }

        // Execute DAP Command (process request and prepare response)
        USB_RespSize[USB_ResponseIndexI] =
                (uint16_t) DAP_ExecuteCommand(USB_Request[USB_RequestIndexO], USB_Response[USB_ResponseIndexI]);

        current_time = get_system_time_ms() + 10;

        // Update Request Index and Count
        USB_RequestIndexO++;
        if (USB_RequestIndexO == DAP_PACKET_COUNT) {
            USB_RequestIndexO = 0U;
        }
        USB_RequestCountO++;

        if (USB_RequestIdle) {
            if ((uint16_t) (USB_RequestCountI - USB_RequestCountO) != DAP_PACKET_COUNT) {
                USB_RequestIdle = 0U;
                usbd_ep_start_read(0, DAP_OUT_EP, USB_Request[USB_RequestIndexI], DAP_PACKET_SIZE);
            }
        }

        // Update Response Index and Count
        USB_ResponseIndexI++;
        if (USB_ResponseIndexI == DAP_PACKET_COUNT) {
            USB_ResponseIndexI = 0U;
        }
        USB_ResponseCountI++;

        if (USB_ResponseIdle) {
            if (USB_ResponseCountI != USB_ResponseCountO) {
                // Load data from response buffer to be sent back
                n = USB_ResponseIndexO++;
                if (USB_ResponseIndexO == DAP_PACKET_COUNT) {
                    USB_ResponseIndexO = 0U;
                }
                USB_ResponseCountO++;
                USB_ResponseIdle = 0U;
                usbd_ep_start_write(0, DAP_IN_EP, USB_Response[n], USB_RespSize[n]);
            }
        }
    }
    if(get_system_time_ms() >= current_time) {
        read_rtt_and_send_usb();
    }
}





