#ifndef HSLINK_PRO_USB_CONFIGURATION_H
#define HSLINK_PRO_USB_CONFIGURATION_H
#include "usbd_core.h"
#include "usbd_cdc.h"
#include "usbd_cdc_acm.h"
#include "usbd_msc.h"

#include "DAP_config.h"
#include "DAP.h"

#define DAP_IN_EP  0x81
#define DAP_OUT_EP 0x02

#define MSC_IN_EP  0x83
#define MSC_OUT_EP 0x04

#define CDC_IN_EP  0x85
#define CDC_OUT_EP 0x05
#define CDC_INT_EP 0x86

#define CDC_IN_EP1  0x87
#define CDC_OUT_EP1 0x07
#define CDC_INT_EP1 0x88

#define CDC_IN_EP2  0x89
#define CDC_OUT_EP2 0x09
#define CDC_INT_EP2 0x8A

#define CDC_IN_EP3  0x8B
#define CDC_OUT_EP3 0x0B
#define CDC_INT_EP3 0x8C

#define CDC_IN_EP4  0x8D
#define CDC_OUT_EP4 0x0D
#define CDC_INT_EP4 0x8E


#define USBD_VID           0x0D28
#define USBD_PID           0x0202
#define USBD_MAX_POWER     500
#define USBD_LANGID_STRING 1033

#define CMSIS_DAP_INTERFACE_SIZE (9 + 7 + 7)

#ifdef CONFIG_CHERRYDAP_USE_MSC
#define CONFIG_MSC_DESCRIPTOR_LEN CMSIS_DAP_INTERFACE_SIZE
#define CONFIG_MSC_INTF_NUM       1
#define MSC_INTF_NUM             (1)
#else
#define CONFIG_MSC_DESCRIPTOR_LEN 0
#define CONFIG_MSC_INTF_NUM       0
#define MSC_INTF_NUM              (0)
#endif

#ifdef CONFIG_USB485
#define CONFIG_USB485_DESCRIPTOR_LEN  CDC_ACM_DESCRIPTOR_LEN
#define CONFIG_USB485_INTF_NUM        2
#else
#define CONFIG_USB485_DESCRIPTOR_LEN  0
#define CONFIG_USB485_INTF_NUM        0
#define USB485_INTF_NUM              (0)
#endif

#ifdef CONFIG_SLCAN0
#define CONFIG_SLCAN0_DESCRIPTOR_LEN  CDC_ACM_DESCRIPTOR_LEN
#define CONFIG_SLCAN0_INTF_NUM        2
#else
#define CONFIG_SLCAN0_DESCRIPTOR_LEN  0
#define CONFIG_SLCAN0_INTF_NUM        0
#define SLCAN0_INTF_NUM              (0)
#endif

#ifdef CONFIG_SLCAN2
#define CONFIG_SLCAN2_DESCRIPTOR_LEN  CDC_ACM_DESCRIPTOR_LEN
#define CONFIG_SLCAN2_INTF_NUM        2
#else
#define CONFIG_SLCAN2_DESCRIPTOR_LEN  0
#define CONFIG_SLCAN2_INTF_NUM        0
#define SLCAN2_INTF_NUM              (0)
#endif


#define USBD_WINUSB_VENDOR_CODE 0x20

#define USBD_WEBUSB_ENABLE 0
#define USBD_BULK_ENABLE   1
#define USBD_WINUSB_ENABLE 1

/* WinUSB Microsoft OS 2.0 descriptor sizes */
#define WINUSB_DESCRIPTOR_SET_HEADER_SIZE  10
#define WINUSB_FUNCTION_SUBSET_HEADER_SIZE 8
#define WINUSB_FEATURE_COMPATIBLE_ID_SIZE  20

#define FUNCTION_SUBSET_LEN                160
#define DEVICE_INTERFACE_GUIDS_FEATURE_LEN 132

#define USBD_WINUSB_DESC_SET_LEN (WINUSB_DESCRIPTOR_SET_HEADER_SIZE + USBD_WEBUSB_ENABLE * FUNCTION_SUBSET_LEN + USBD_BULK_ENABLE * FUNCTION_SUBSET_LEN)

#define USBD_NUM_DEV_CAPABILITIES (USBD_WEBUSB_ENABLE + USBD_WINUSB_ENABLE)

#define USBD_WEBUSB_DESC_LEN 24
#define USBD_WINUSB_DESC_LEN 28

#define USBD_BOS_WTOTALLENGTH (0x05 +                                      \
                               USBD_WEBUSB_DESC_LEN * USBD_WEBUSB_ENABLE + \
                               USBD_WINUSB_DESC_LEN * USBD_WINUSB_ENABLE)

#define CONFIG_UARTRX_RINGBUF_SIZE (4 * 1024)
#define CONFIG_USBRX_RINGBUF_SIZE  (4 * 1024)


extern struct usbd_endpoint dap_out_ep;
extern struct usbd_endpoint dap_in_ep;
extern struct usbd_endpoint cdc_out_ep;
extern struct usbd_endpoint cdc_in_ep;
extern struct usbd_endpoint cdc_out_ep1;
extern struct usbd_endpoint cdc_in_ep1;
extern struct usbd_endpoint cdc_out_ep2;
extern struct usbd_endpoint cdc_in_ep2;

/**
 * @brief 本工程的USB配置函数，用于添加其它配置
 */
void USB_Configuration(void);

#endif //HSLINK_PRO_USB_CONFIGURATION_H
