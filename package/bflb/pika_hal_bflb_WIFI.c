#include <lwip/tcpip.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>

#include "bl_fw_api.h"
#include "wifi_mgmr_ext.h"
#include "wifi_mgmr.h"

#include "bflb_irq.h"
#include "bflb_uart.h"

#include "bl616_glb.h"
#include "pika_hal_bflb_common.h"

#define WIFI_STACK_SIZE  (1024 * 2)
#define TASK_PRIORITY_FW (16)

static TaskHandle_t wifi_fw_task;

static wifi_conf_t conf = {
    .country_code = "CN",
};

static volatile int g_wifi_active = 0;
static volatile int g_wifi_connected = 0;

#define LOG_I printf

void wifi_event_handler(uint32_t code)
{
    switch (code) {
        case CODE_WIFI_ON_INIT_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_INIT_DONE\r\n", __func__);
            wifi_mgmr_init(&conf);
        } break;
        case CODE_WIFI_ON_MGMR_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_MGMR_DONE\r\n", __func__);
        } break;
        case CODE_WIFI_ON_SCAN_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_SCAN_DONE\r\n", __func__);
            wifi_mgmr_sta_scanlist();
        } break;
        case CODE_WIFI_ON_CONNECTED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_CONNECTED\r\n", __func__);
            void mm_sec_keydump();
            mm_sec_keydump();
        } break;
        case CODE_WIFI_ON_GOT_IP: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_GOT_IP\r\n", __func__);
            pika_debug("wifi connected");
            g_wifi_connected = 1;
        } break;
        case CODE_WIFI_ON_DISCONNECT: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_DISCONNECT\r\n", __func__);
            pika_debug("wifi disconnected");
            g_wifi_connected = 0;
        } break;
        case CODE_WIFI_ON_AP_STARTED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_AP_STARTED\r\n", __func__);
        } break;
        case CODE_WIFI_ON_AP_STOPPED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_AP_STOPPED\r\n", __func__);
        } break;
        case CODE_WIFI_ON_AP_STA_ADD: {
            LOG_I("[APP] [EVT] [AP] [ADD] %lld\r\n", xTaskGetTickCount());
        } break;
        case CODE_WIFI_ON_AP_STA_DEL: {
            LOG_I("[APP] [EVT] [AP] [DEL] %lld\r\n", xTaskGetTickCount());
        } break;
        default: {
            LOG_I("[APP] [EVT] Unknown code %u \r\n", code);
        }
    }
}

int wifi_start_firmware_task(void)
{
    /* enable wifi clock */
    tcpip_init(NULL, NULL);

    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_IP_WIFI_PHY | GLB_AHB_CLOCK_IP_WIFI_MAC_PHY | GLB_AHB_CLOCK_IP_WIFI_PLATFORM);
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_WIFI);

    /* set ble controller EM Size */

    GLB_Set_EM_Sel(GLB_WRAM160KB_EM0KB);

    if (0 != rfparam_init(0, NULL, 0)) {
        LOG_I("PHY RF init failed!\r\n");
        return 0;
    }

    LOG_I("PHY RF init success!\r\n");

    /* Enable wifi irq */

    extern void interrupt0_handler(void);
    bflb_irq_attach(WIFI_IRQn, (irq_callback)interrupt0_handler, NULL);
    bflb_irq_enable(WIFI_IRQn);

    xTaskCreate(wifi_main, (char *)"fw", WIFI_STACK_SIZE, NULL, TASK_PRIORITY_FW, &wifi_fw_task);

    return 0;
}

int pika_hal_platform_WIFI_open(pika_dev* dev, char* name) {
    // Initialize Wi-Fi device
    return 0;
}

int pika_hal_platform_WIFI_close(pika_dev* dev) {
    // Deinitialize Wi-Fi device
    // return wifi_mgmr_wifi_pwr_off();
}

int pika_hal_platform_WIFI_ioctl_config(pika_dev* dev, pika_hal_WIFI_config* cfg) {
    return 0;
}

int pika_hal_platform_WIFI_ioctl_enable(pika_dev* dev) {
    // Enable Wi-Fi device
    g_wifi_active = 1;
    return wifi_start_firmware_task();
}

int pika_hal_platform_WIFI_ioctl_disable(pika_dev* dev) {
    // Disable Wi-Fi device
    // return wifi_mgmr_wifi_pwr_off();
}

int pika_hal_platform_WIFI_ioctl_others(pika_dev* dev, PIKA_HAL_IOCTL_CMD cmd, void* arg) {
    int result = -1;
    switch (cmd) {
        case PIKA_HAL_IOCTL_WIFI_GET_STATUS:
            // Call the appropriate function to get Wi-Fi status
            if(g_wifi_connected){
                *(PIKA_HAL_WIFI_STATUS*)arg = PIKA_HAL_WIFI_STATUS_GOT_IP;
                pika_debug("wifi status: got ip");
            }else{
                *(PIKA_HAL_WIFI_STATUS*)arg = PIKA_HAL_WIFI_STATUS_CONNECTING;
                pika_debug("wifi status: connecting");
                return 0;
            }
            return 0;
        case PIKA_HAL_IOCTL_WIFI_GET_ACTIVE:
            // Call the appropriate function to get Wi-Fi active state
            *(int*)arg = g_wifi_active;
            return 0;
        case PIKA_HAL_IOCTL_WIFI_SCAN:
            // Call the appropriate function to perform Wi-Fi scan
            break;
        case PIKA_HAL_IOCTL_WIFI_CONNECT:
            // Call the appropriate function to connect to Wi-Fi network
            pika_debug("ioctl connect");
            pika_hal_WIFI_connect_config* conncfg =
                (pika_hal_WIFI_connect_config*)arg;
            int ret = wifi_sta_connect(conncfg->ssid, conncfg->password, NULL, NULL, 1, 0, 0, 1);
            return ret;
        case PIKA_HAL_IOCTL_WIFI_GET_IFCONFIG:
            // Call the appropriate function to get Wi-Fi interface configuration
            break;
        case PIKA_HAL_IOCTL_WIFI_SET_IFCONFIG:
            // Call the appropriate function to set Wi-Fi interface configuration
            break;
        default:
            // Unknown command
            result = -1;
            break;
    }
    return result;
}

