#include "usb_configuration.h"
#include "usbd_core.h"
#include "dap_main.h"
#include "usb2uart.h"
#include "usb2msc.h"
#include "usb2python.h"


#define USB_CONFIG_SIZE (9 + CMSIS_DAP_INTERFACE_SIZE + CONFIG_MSC_DESCRIPTOR_LEN + CDC_ACM_DESCRIPTOR_LEN + CDC_ACM_DESCRIPTOR_LEN \
                           + CONFIG_USB485_DESCRIPTOR_LEN + CONFIG_SLCAN0_DESCRIPTOR_LEN + CONFIG_SLCAN2_DESCRIPTOR_LEN)
#define INTF_NUM        (2 + 1 + CONFIG_MSC_INTF_NUM + 2 + CONFIG_USB485_INTF_NUM + CONFIG_SLCAN0_INTF_NUM + CONFIG_SLCAN2_INTF_NUM)

static const uint8_t device_descriptor[] = {
        USB_DEVICE_DESCRIPTOR_INIT(USB_2_1, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01),
};


static const uint8_t config_descriptor[] = {
        USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, INTF_NUM, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
        /* Interface 0 */
        USB_INTERFACE_DESCRIPTOR_INIT(0x00, 0x00, 0x02, 0xFF, 0x00, 0x00, 0x02),
        /* Endpoint OUT 2 */
        USB_ENDPOINT_DESCRIPTOR_INIT(DAP_OUT_EP, USB_ENDPOINT_TYPE_BULK, DAP_PACKET_SIZE, 0x00),
        /* Endpoint IN 1 */
        USB_ENDPOINT_DESCRIPTOR_INIT(DAP_IN_EP, USB_ENDPOINT_TYPE_BULK, DAP_PACKET_SIZE, 0x00),
#ifdef CONFIG_CHERRYDAP_USE_MSC
        MSC_DESCRIPTOR_INIT(MSC_INTF_NUM, MSC_OUT_EP, MSC_IN_EP, DAP_PACKET_SIZE, 0x00),
#endif
        CDC_ACM_DESCRIPTOR_INIT(MSC_INTF_NUM + 1, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, DAP_PACKET_SIZE, 0x00),
        CDC_ACM_DESCRIPTOR_INIT(MSC_INTF_NUM + 3, CDC_INT_EP1, CDC_OUT_EP1, CDC_IN_EP1, DAP_PACKET_SIZE, 0x00),
#ifdef CONFIG_USB485
        CDC_ACM_DESCRIPTOR_INIT(MSC_INTF_NUM + 5, CDC_INT_EP2, CDC_OUT_EP2, CDC_IN_EP2, DAP_PACKET_SIZE, 0x00),
#endif
#ifdef CONFIG_SLCAN0
        CDC_ACM_DESCRIPTOR_INIT(MSC_INTF_NUM + 7, CDC_INT_EP3, CDC_OUT_EP3, CDC_IN_EP3, DAP_PACKET_SIZE, 0x00),
#endif
#ifdef CONFIG_SLCAN2
        CDC_ACM_DESCRIPTOR_INIT(MSC_INTF_NUM + 9, CDC_INT_EP4, CDC_OUT_EP4, CDC_IN_EP4, DAP_PACKET_SIZE, 0x00),
#endif
};

static const uint8_t other_speed_config_descriptor[] = {
        USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, INTF_NUM, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
        /* Interface 0 */
        USB_INTERFACE_DESCRIPTOR_INIT(0x00, 0x00, 0x02, 0xFF, 0x00, 0x00, 0x02),
        /* Endpoint OUT 2 */
        USB_ENDPOINT_DESCRIPTOR_INIT(DAP_OUT_EP, USB_ENDPOINT_TYPE_BULK, DAP_PACKET_SIZE, 0x00),
        /* Endpoint IN 1 */
        USB_ENDPOINT_DESCRIPTOR_INIT(DAP_IN_EP, USB_ENDPOINT_TYPE_BULK, DAP_PACKET_SIZE, 0x00),
#ifdef CONFIG_CHERRYDAP_USE_MSC
        MSC_DESCRIPTOR_INIT(MSC_INTF_NUM, MSC_OUT_EP, MSC_IN_EP, DAP_PACKET_SIZE, 0x00),
#endif
        CDC_ACM_DESCRIPTOR_INIT(MSC_INTF_NUM + 1, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, DAP_PACKET_SIZE, 0x00),
        CDC_ACM_DESCRIPTOR_INIT(MSC_INTF_NUM + 3, CDC_INT_EP1, CDC_OUT_EP1, CDC_IN_EP1, DAP_PACKET_SIZE, 0x00),
#ifdef CONFIG_USB485
        CDC_ACM_DESCRIPTOR_INIT(MSC_INTF_NUM + 5, CDC_INT_EP2, CDC_OUT_EP2, CDC_IN_EP2, DAP_PACKET_SIZE, 0x00),
#endif
#ifdef CONFIG_SLCAN0
        CDC_ACM_DESCRIPTOR_INIT(MSC_INTF_NUM + 7, CDC_INT_EP3, CDC_OUT_EP3, CDC_IN_EP3, DAP_PACKET_SIZE, 0x00),
#endif
#ifdef CONFIG_SLCAN2
        CDC_ACM_DESCRIPTOR_INIT(MSC_INTF_NUM + 9, CDC_INT_EP4, CDC_OUT_EP4, CDC_IN_EP4, DAP_PACKET_SIZE, 0x00),
#endif
};

char *string_descriptors[] = {
        (char[]) {0x09, 0x04},              /* Langid */
        "CherryUSB",                        /* Manufacturer */
        "MicroLink CMSIS-DAP",              /* Product */
        "123456789ABCDEF",                  /* Serial Number */
};

static const uint8_t device_quality_descriptor[] = {
        USB_DEVICE_QUALIFIER_DESCRIPTOR_INIT(USB_2_1, 0x00, 0x00, 0x00, 0x01),
};

__ALIGN_BEGIN const uint8_t USBD_WinUSBDescriptorSetDescriptor[] = {
        WBVAL(WINUSB_DESCRIPTOR_SET_HEADER_SIZE), /* wLength */
        WBVAL(WINUSB_SET_HEADER_DESCRIPTOR_TYPE), /* wDescriptorType */
        0x00, 0x00, 0x03, 0x06, /* >= Win 8.1 */  /* dwWindowsVersion*/
        WBVAL(USBD_WINUSB_DESC_SET_LEN),          /* wDescriptorSetTotalLength */
#if (USBD_WEBUSB_ENABLE)
        WBVAL(WINUSB_FUNCTION_SUBSET_HEADER_SIZE), // wLength
        WBVAL(WINUSB_SUBSET_HEADER_FUNCTION_TYPE), // wDescriptorType
        0,                                         // bFirstInterface USBD_WINUSB_IF_NUM
        0,                                         // bReserved
        WBVAL(FUNCTION_SUBSET_LEN),                // wSubsetLength
        WBVAL(WINUSB_FEATURE_COMPATIBLE_ID_SIZE),  // wLength
        WBVAL(WINUSB_FEATURE_COMPATIBLE_ID_TYPE),  // wDescriptorType
        'W', 'I', 'N', 'U', 'S', 'B', 0, 0,        // CompatibleId
        0, 0, 0, 0, 0, 0, 0, 0,                    // SubCompatibleId
        WBVAL(DEVICE_INTERFACE_GUIDS_FEATURE_LEN), // wLength
        WBVAL(WINUSB_FEATURE_REG_PROPERTY_TYPE),   // wDescriptorType
        WBVAL(WINUSB_PROP_DATA_TYPE_REG_MULTI_SZ), // wPropertyDataType
        WBVAL(42),                                 // wPropertyNameLength
        'D', 0, 'e', 0, 'v', 0, 'i', 0, 'c', 0, 'e', 0,
        'I', 0, 'n', 0, 't', 0, 'e', 0, 'r', 0, 'f', 0, 'a', 0, 'c', 0, 'e', 0,
        'G', 0, 'U', 0, 'I', 0, 'D', 0, 's', 0, 0, 0,
        WBVAL(80), // wPropertyDataLength
        '{', 0,
        '9', 0, '2', 0, 'C', 0, 'E', 0, '6', 0, '4', 0, '6', 0, '2', 0, '-', 0,
        '9', 0, 'C', 0, '7', 0, '7', 0, '-', 0,
        '4', 0, '6', 0, 'F', 0, 'E', 0, '-', 0,
        '9', 0, '3', 0, '3', 0, 'B', 0, '-',
        0, '3', 0, '1', 0, 'C', 0, 'B', 0, '9', 0, 'C', 0, '5', 0, 'A', 0, 'A', 0, '3', 0, 'B', 0, '9', 0,
        '}', 0, 0, 0, 0, 0
#endif
#if USBD_BULK_ENABLE
        WBVAL(WINUSB_FUNCTION_SUBSET_HEADER_SIZE), /* wLength */
        WBVAL(WINUSB_SUBSET_HEADER_FUNCTION_TYPE), /* wDescriptorType */
        0,                                         /* bFirstInterface USBD_BULK_IF_NUM*/
        0,                                         /* bReserved */
        WBVAL(FUNCTION_SUBSET_LEN),                /* wSubsetLength */
        WBVAL(WINUSB_FEATURE_COMPATIBLE_ID_SIZE),  /* wLength */
        WBVAL(WINUSB_FEATURE_COMPATIBLE_ID_TYPE),  /* wDescriptorType */
        'W', 'I', 'N', 'U', 'S', 'B', 0, 0,        /* CompatibleId*/
        0, 0, 0, 0, 0, 0, 0, 0,                    /* SubCompatibleId*/
        WBVAL(DEVICE_INTERFACE_GUIDS_FEATURE_LEN), /* wLength */
        WBVAL(WINUSB_FEATURE_REG_PROPERTY_TYPE),   /* wDescriptorType */
        WBVAL(WINUSB_PROP_DATA_TYPE_REG_MULTI_SZ), /* wPropertyDataType */
        WBVAL(42),                                 /* wPropertyNameLength */
        'D', 0, 'e', 0, 'v', 0, 'i', 0, 'c', 0, 'e', 0,
        'I', 0, 'n', 0, 't', 0, 'e', 0, 'r', 0, 'f', 0, 'a', 0, 'c', 0, 'e', 0,
        'G', 0, 'U', 0, 'I', 0, 'D', 0, 's', 0, 0, 0,
        WBVAL(80), /* wPropertyDataLength */
        '{', 0,
        'C', 0, 'D', 0, 'B', 0, '3', 0, 'B', 0, '5', 0, 'A', 0, 'D', 0, '-', 0,
        '2', 0, '9', 0, '3', 0, 'B', 0, '-', 0,
        '4', 0, '6', 0, '6', 0, '3', 0, '-', 0,
        'A', 0, 'A', 0, '3', 0, '6', 0, '-',
        0, '1', 0, 'A', 0, 'A', 0, 'E', 0, '4', 0, '6', 0, '4', 0, '6', 0, '3', 0, '7', 0, '7', 0, '6', 0,
        '}', 0, 0, 0, 0, 0
#endif
};

__ALIGN_BEGIN const uint8_t USBD_BinaryObjectStoreDescriptor[] = {
        0x05,                         /* bLength */
        0x0f,                         /* bDescriptorType */
        WBVAL(USBD_BOS_WTOTALLENGTH), /* wTotalLength */
        USBD_NUM_DEV_CAPABILITIES,    /* bNumDeviceCaps */
#if (USBD_WEBUSB_ENABLE)
        USBD_WEBUSB_DESC_LEN,           /* bLength */
        0x10,                           /* bDescriptorType */
        USB_DEVICE_CAPABILITY_PLATFORM, /* bDevCapabilityType */
        0x00,                           /* bReserved */
        0x38, 0xB6, 0x08, 0x34,         /* PlatformCapabilityUUID */
        0xA9, 0x09, 0xA0, 0x47,
        0x8B, 0xFD, 0xA0, 0x76,
        0x88, 0x15, 0xB6, 0x65,
        WBVAL(0x0100), /* 1.00 */ /* bcdVersion */
        USBD_WINUSB_VENDOR_CODE,  /* bVendorCode */
        0,                        /* iLandingPage */
#endif
#if (USBD_WINUSB_ENABLE)
        USBD_WINUSB_DESC_LEN,           /* bLength */
        0x10,                           /* bDescriptorType */
        USB_DEVICE_CAPABILITY_PLATFORM, /* bDevCapabilityType */
        0x00,                           /* bReserved */
        0xDF, 0x60, 0xDD, 0xD8,         /* PlatformCapabilityUUID */
        0x89, 0x45, 0xC7, 0x4C,
        0x9C, 0xD2, 0x65, 0x9D,
        0x9E, 0x64, 0x8A, 0x9F,
        0x00, 0x00, 0x03, 0x06, /* >= Win 8.1 */ /* dwWindowsVersion*/
        WBVAL(USBD_WINUSB_DESC_SET_LEN),         /* wDescriptorSetTotalLength */
        USBD_WINUSB_VENDOR_CODE,                 /* bVendorCode */
        0,                                       /* bAltEnumCode */
#endif
};


const uint8_t *config_descriptor_callback(uint8_t speed)
{
    (void) speed;
    return config_descriptor;
}

const uint8_t *other_speed_config_descriptor_callback(uint8_t speed)
{
    (void) speed;
    return other_speed_config_descriptor;
}

const uint8_t *device_descriptor_callback(uint8_t speed)
{
    (void) speed;
    return device_descriptor;
}


const uint8_t *device_quality_descriptor_callback(uint8_t speed)
{
    (void) speed;
    return device_quality_descriptor;
}


const char *string_descriptor_callback(uint8_t speed, uint8_t index)
{
    (void) speed;

    if (index >= (sizeof(string_descriptors) / sizeof(char *))) {
        return NULL;
    }
    return string_descriptors[index];
}

struct usb_msosv2_descriptor msosv2_desc = {
        .vendor_code = USBD_WINUSB_VENDOR_CODE,
        .compat_id = USBD_WinUSBDescriptorSetDescriptor,
        .compat_id_len = USBD_WINUSB_DESC_SET_LEN,
};

struct usb_bos_descriptor bos_desc = {
        .string = USBD_BinaryObjectStoreDescriptor,
        .string_len = USBD_BOS_WTOTALLENGTH
};

const struct usb_descriptor cmsisdap_descriptor = {
        .device_descriptor_callback = device_descriptor_callback,
        .config_descriptor_callback = config_descriptor_callback,
        .device_quality_descriptor_callback = device_quality_descriptor_callback,
        .other_speed_descriptor_callback = other_speed_config_descriptor_callback,
        .string_descriptor_callback = string_descriptor_callback,
        .bos_descriptor = &bos_desc,
        .msosv2_descriptor = &msosv2_desc,
};

void usbd_event_handler(uint8_t busid, uint8_t event)
{
    (void) busid;
    switch (event) {
        case USBD_EVENT_RESET:
            usbrx_idle_flag = 0;
            usbtx_idle_flag = 0;
            uarttx_idle_flag = 0;
            config_uart_transfer = 0;
            break;
        case USBD_EVENT_CONNECTED:
            break;
        case USBD_EVENT_DISCONNECTED:
            break;
        case USBD_EVENT_RESUME:
            break;
        case USBD_EVENT_SUSPEND:
            break;
        case USBD_EVENT_CONFIGURED:
            /* setup first out ep read transfer */
            USB_RequestIdle = 0U;

            usbd_ep_start_read(0, DAP_OUT_EP, USB_Request[0], DAP_PACKET_SIZE);
            usbd_ep_start_read(0, CDC_OUT_EP, cdc_tmpbuffer, DAP_PACKET_SIZE);
            usbd_ep_start_read(0, CDC_OUT_EP1, cdc1_tmpbuffer, DAP_PACKET_SIZE);
            #ifdef CONFIG_USB485
            usbd_ep_start_read(0, CDC_OUT_EP2, cdc2_tmpbuffer, DAP_PACKET_SIZE);
            #endif
            #ifdef CONFIG_SLCAN0
            usbd_ep_start_read(0, CDC_OUT_EP3, slcan0.g_cdc_can_device.read_buffer, usbd_get_ep_mps(busid, CDC_OUT_EP3));
            #endif
            #ifdef CONFIG_SLCAN2
            usbd_ep_start_read(0, CDC_OUT_EP4, slcan2.g_cdc_can_device.read_buffer, usbd_get_ep_mps(busid, CDC_OUT_EP4));
            #endif
            break;
        case USBD_EVENT_SET_REMOTE_WAKEUP:
            break;
        case USBD_EVENT_CLR_REMOTE_WAKEUP:
            break;

        default:
            break;
    }
}

static struct usbd_interface dap_intf;
static struct usbd_interface intf1;
static struct usbd_interface intf2;
static struct usbd_interface intf3;
static struct usbd_interface intf4;
static struct usbd_interface intf5;
static struct usbd_interface intf6;
static struct usbd_interface intf7;

void usbd_cdc_acm_set_line_coding(uint8_t busid, uint8_t intf, struct cdc_line_coding *line_coding)
{
    (void) busid;
    if(intf == intf2.intf_num){
          memcpy((uint8_t *) &g_cdc_lincoding, line_coding, sizeof(struct cdc_line_coding));
          config_uart = 1;
          config_uart_transfer = 0;
    }

    if(intf == intf4.intf_num){
          memcpy((uint8_t *) &g_cdc1_lincoding, line_coding, sizeof(struct cdc_line_coding));  
          config_uart_python = 1;
    }

#ifdef CONFIG_USB485
    if(intf == intf6.intf_num){
      if (memcmp(line_coding, (uint8_t *) &g_cdc2_lincoding, sizeof(struct cdc_line_coding)) != 0) {
          memcpy((uint8_t *) &g_cdc2_lincoding, line_coding, sizeof(struct cdc_line_coding));
          config_uart_485 = 1;
          config_uart_485_transfer = 0;
          printf("config_uart_485 = %d\r\n",line_coding->dwDTERate);
      }
    }
#endif 

#ifdef CONFIG_SLCAN0
    if (intf == slcan0.g_cdc_can_device.cdc_device.intf0.intf_num) {
        slcan0.g_cdc_can_device.cdc_device.is_open = true;
        slcan0.g_cdc_can_device.cdc_device.s_line_coding.dwDTERate = line_coding->dwDTERate;
        slcan0.g_cdc_can_device.cdc_device.s_line_coding.bDataBits = line_coding->bDataBits;
        slcan0.g_cdc_can_device.cdc_device.s_line_coding.bParityType = line_coding->bParityType;
        slcan0.g_cdc_can_device.cdc_device.s_line_coding.bCharFormat = line_coding->bCharFormat;
        printf("config_uart_can0 = %d\r\n",line_coding->dwDTERate);
    }
#endif 

#ifdef CONFIG_SLCAN2
    if (intf == slcan2.g_cdc_can_device.cdc_device.intf0.intf_num) {
        slcan2.g_cdc_can_device.cdc_device.is_open = true;
        slcan2.g_cdc_can_device.cdc_device.s_line_coding.dwDTERate = line_coding->dwDTERate;
        slcan2.g_cdc_can_device.cdc_device.s_line_coding.bDataBits = line_coding->bDataBits;
        slcan2.g_cdc_can_device.cdc_device.s_line_coding.bParityType = line_coding->bParityType;
        slcan2.g_cdc_can_device.cdc_device.s_line_coding.bCharFormat = line_coding->bCharFormat;
        printf("config_uart_can2 = %d\r\n",line_coding->dwDTERate);
    }   
#endif      
}

void usbd_cdc_acm_get_line_coding(uint8_t busid, uint8_t intf, struct cdc_line_coding *line_coding)
{
    (void) busid;
    if(intf == intf2.intf_num){
        memcpy(line_coding, (uint8_t *) &g_cdc_lincoding, sizeof(struct cdc_line_coding));
    }
    if(intf == intf4.intf_num){
        memcpy(line_coding, (uint8_t *) &g_cdc1_lincoding, sizeof(struct cdc_line_coding));
    }
#ifdef CONFIG_USB485
    if(intf == intf6.intf_num){
        memcpy(line_coding, (uint8_t *) &g_cdc2_lincoding, sizeof(struct cdc_line_coding));
    }
#endif   
#ifdef CONFIG_SLCAN0
    if (intf == slcan0.g_cdc_can_device.cdc_device.intf0.intf_num) {
        slcan0.g_cdc_can_device.cdc_device.is_open = true;
        line_coding->dwDTERate = slcan0.g_cdc_can_device.cdc_device.s_line_coding.dwDTERate;
        line_coding->bDataBits = slcan0.g_cdc_can_device.cdc_device.s_line_coding.bDataBits;
        line_coding->bParityType = slcan0.g_cdc_can_device.cdc_device.s_line_coding.bParityType;
        line_coding->bCharFormat = slcan0.g_cdc_can_device.cdc_device.s_line_coding.bCharFormat;
    }
#endif
#ifdef CONFIG_SLCAN2
    if (intf == slcan2.g_cdc_can_device.cdc_device.intf0.intf_num) {
        slcan2.g_cdc_can_device.cdc_device.is_open = true;
        line_coding->dwDTERate = slcan2.g_cdc_can_device.cdc_device.s_line_coding.dwDTERate;
        line_coding->bDataBits = slcan2.g_cdc_can_device.cdc_device.s_line_coding.bDataBits;
        line_coding->bParityType = slcan2.g_cdc_can_device.cdc_device.s_line_coding.bParityType;
        line_coding->bCharFormat = slcan2.g_cdc_can_device.cdc_device.s_line_coding.bCharFormat;
    }
#endif
}



void USB_Configuration(void)
{
    DAP_Setup();

    usbd_desc_register(0, &cmsisdap_descriptor);

    /*!< winusb */
    usbd_add_interface(0, &dap_intf);
    usbd_add_endpoint(0, &dap_out_ep);
    usbd_add_endpoint(0, &dap_in_ep);

#ifdef CONFIG_CHERRYDAP_USE_MSC
    usbd_add_interface(0, usbd_msc_init_intf(0, &intf1, MSC_OUT_EP, MSC_IN_EP));
#endif

    /*!< cdc acm */
    usbd_add_interface(0, usbd_cdc_acm_init_intf(0, &intf2));
    usbd_add_interface(0, usbd_cdc_acm_init_intf(0, &intf3));
    usbd_add_endpoint(0, &cdc_out_ep);
    usbd_add_endpoint(0, &cdc_in_ep);

    usbd_add_interface(0, usbd_cdc_acm_init_intf(0, &intf4));
    usbd_add_interface(0, usbd_cdc_acm_init_intf(0, &intf5));
    usbd_add_endpoint(0, &cdc_out_ep1);
    usbd_add_endpoint(0, &cdc_in_ep1);

#ifdef CONFIG_USB485
    usbd_add_interface(0, usbd_cdc_acm_init_intf(0, &intf6));
    usbd_add_interface(0, usbd_cdc_acm_init_intf(0, &intf7));
    usbd_add_endpoint(0, &cdc_out_ep2);
    usbd_add_endpoint(0, &cdc_in_ep2);
#endif

    extern void usbd_event_handler(uint8_t busid, uint8_t event);
    usbd_initialize(0, HPM_USB0_BASE, usbd_event_handler);
}
