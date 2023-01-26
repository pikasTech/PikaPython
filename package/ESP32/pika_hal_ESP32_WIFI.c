#include "../pikascript-lib/PikaStdDevice/pika_hal.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "nvs_flash.h"

static volatile PIKA_BOOL wifi_started = PIKA_FALSE;
static volatile PIKA_BOOL wifi_sta_connect_requested = PIKA_FALSE;
static volatile PIKA_BOOL wifi_sta_connected = PIKA_FALSE;
static volatile PIKA_HAL_WIFI_STATUS wifi_sta_disconn_reason =
    PIKA_HAL_WIFI_STATUS_IDLE;
static EventGroupHandle_t wifi_event_group;
static esp_netif_t* sta_netif = NULL;

static void event_handler(void* event_handler_arg,
                          esp_event_base_t event_base,
                          int32_t event_id,
                          void* event_data) {
    pika_dev* dev = (pika_dev*)event_handler_arg;
    pika_hal_WIFI_config* cfg = (pika_hal_WIFI_config*)dev->ioctl_config;
    pika_debug("event_handler: %p %d", event_base, event_id);
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        pika_debug("event_handler: WIFI_EVENT_STA_START");
        pika_debug("wifi_sta_connect_requested: %d",
                   wifi_sta_connect_requested);
        if (wifi_sta_connect_requested) {
            pika_debug("event_handler: WIFI_EVENT_STA_START: connect");
            esp_wifi_connect();
        }
    } else if (event_base == WIFI_EVENT &&
               event_id == WIFI_EVENT_STA_DISCONNECTED) {
        wifi_sta_connected = PIKA_FALSE;
        wifi_sta_disconn_reason = PIKA_HAL_WIFI_STATUS_CONNECT_FAIL;
        pika_debug("event_handler: WIFI_EVENT_STA_DISCONNECTED");
        if (wifi_sta_connect_requested) {
            pika_debug("event_handler: WIFI_EVENT_STA_DISCONNECTED: connect");
            esp_wifi_connect();
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        pika_debug("event_handler: IP_EVENT_STA_GOT_IP");
        ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
        sprintf(cfg->ip, IPSTR, IP2STR(&event->ip_info.ip));
        sprintf(cfg->gateway, IPSTR, IP2STR(&event->ip_info.gw));
        sprintf(cfg->netmask, IPSTR, IP2STR(&event->ip_info.netmask));
        esp_netif_dns_info_t dns_info;
        esp_netif_get_dns_info(sta_netif, ESP_NETIF_DNS_MAIN, &dns_info);
        sprintf(cfg->dns, IPSTR, IP2STR(&dns_info.ip.u_addr.ip4));
        wifi_sta_connected = PIKA_TRUE;
        wifi_sta_disconn_reason = PIKA_HAL_WIFI_STATUS_GOT_IP;
    }
}

int pika_hal_platform_WIFI_open(pika_dev* dev, char* name) {
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_ERROR_CHECK(esp_netif_init());
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    sta_netif = esp_netif_create_default_wifi_sta();
    return 0;
}

int pika_hal_platform_WIFI_close(pika_dev* dev) {
    return 0;
}

int pika_hal_platform_WIFI_ioctl_config(pika_dev* dev,
                                        pika_hal_WIFI_config* cfg) {
    return 0;
}

int pika_hal_platform_WIFI_ioctl_enable(pika_dev* dev) {
    /* connect here */
    pika_debug("pika_hal_platform_WIFI_ioctl_enable");
    wifi_config_t wifi_sta_config = {0};
    pika_hal_WIFI_config* cfg = (pika_hal_WIFI_config*)dev->ioctl_config;
    strcpy((char*)wifi_sta_config.sta.ssid, cfg->ssid);
    strcpy((char*)wifi_sta_config.sta.password, cfg->password);
    if (cfg->bssid[0] != '\0') {
        wifi_sta_config.sta.bssid_set = 1;
        strcpy((char*)wifi_sta_config.sta.bssid, cfg->bssid);
    } else {
        wifi_sta_config.sta.bssid_set = 0;
    }
    wifi_sta_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_sta_config.sta.sae_pwe_h2e = WPA3_SAE_PWE_BOTH;
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_sta_config));
    pika_debug("connecting to %s", cfg->ssid);
    wifi_sta_connect_requested = PIKA_TRUE;
    ESP_ERROR_CHECK(esp_wifi_connect());
    return 0;
}

int pika_hal_platform_WIFI_ioctl_disable(pika_dev* dev) {
    /* disconenct here */
    return -1;
}

int pika_hal_platform_WIFI_ioctl_others(pika_dev* dev,
                                        PIKA_HAL_IOCTL_CMD cmd,
                                        void* arg) {
    if (cmd == PIKA_HAL_IOCTL_WIFI_SET_ACTIVE) {
        wifi_mode_t mode;
        pika_hal_WIFI_config* cfg = (pika_hal_WIFI_config*)dev->ioctl_config;
        int active = *(int*)arg;
        pika_debug("set wifi active: %d", active);
        if (active) {
            if (cfg->mode == PIKA_HAL_WIFI_MODE_STA) {
                mode = WIFI_MODE_STA;
            } else if (cfg->mode == PIKA_HAL_WIFI_MODE_AP) {
                mode = WIFI_MODE_AP;
            } else {
                mode = WIFI_MODE_APSTA;
            }
            if (wifi_started) {
                esp_wifi_set_mode(mode);
            } else {
                pika_debug("regist event handler");
                wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
                pika_debug("init wifi");
                ESP_ERROR_CHECK(esp_wifi_init(&cfg));

                ESP_ERROR_CHECK(esp_event_handler_register(
                    WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, dev));
                ESP_ERROR_CHECK(esp_event_handler_register(
                    IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, dev));

                pika_debug("set mode: %d", mode);
                esp_wifi_set_mode(mode);
                pika_debug("start wifi");
                ESP_ERROR_CHECK(esp_wifi_start());
                wifi_started = PIKA_TRUE;
            }
        } else {
            if (wifi_started) {
                esp_wifi_stop();
                esp_wifi_deinit();
                wifi_started = PIKA_FALSE;
            }
        }
        return 0;
    }
    if (cmd == PIKA_HAL_IOCTL_WIFI_GET_STATUS) {
        if (wifi_sta_connect_requested) {
            if (wifi_sta_connected) {
                *(PIKA_HAL_WIFI_STATUS*)arg = PIKA_HAL_WIFI_STATUS_GOT_IP;
                pika_debug("wifi status: got ip");
                return 0;
            } else {
                *(PIKA_HAL_WIFI_STATUS*)arg = PIKA_HAL_WIFI_STATUS_CONNECTING;
                pika_debug("wifi status: connecting");
                return 0;
            }
        }
        *(PIKA_HAL_WIFI_STATUS*)arg = wifi_sta_disconn_reason;
        return 0;
    }
    if (cmd == PIKA_HAL_IOCTL_WIFI_GET_ACTIVE) {
        *(int*)arg = wifi_started;
        return 0;
    }
    if (cmd == PIKA_HAL_IOCTL_WIFI_SCAN) {
        wifi_mode_t mode;
        ESP_ERROR_CHECK(esp_wifi_get_mode(&mode));
        if ((mode & WIFI_MODE_STA) == 0) {
            pika_platform_printf("Error: STA must be active\r\n");
            return -1;
        }
        wifi_scan_config_t config = {0};
        config.show_hidden = true;
        esp_err_t status = esp_wifi_scan_start(&config, 1);
        if (status != 0) {
            pika_platform_printf("Error: scan failed\r\n");
            return -1;
        }
        uint16_t count = 0;
        ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&count));
        if (count == 0) {
            // esp_wifi_scan_get_ap_records must be called to free internal
            // buffers from the scan. But it returns an error if
            // wifi_ap_records==NULL.  So allocate at least 1 AP entry.
            // esp_wifi_scan_get_ap_records will then return the actual
            // number of APs in count.
            count = 1;
        }
        wifi_ap_record_t* wifi_ap_records =
            calloc(count, sizeof(wifi_ap_record_t));
        ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&count, wifi_ap_records));
        size_t hal_res_size = sizeof(pika_hal_WIFI_scan_result) +
                              sizeof(pika_hal_WIFI_record) * count;
        pika_hal_WIFI_scan_result* result = calloc(1, hal_res_size);
        pika_debug("scan result: %d", count);
        result->count = count;
        for (uint16_t i = 0; i < count; i++) {
            strcpy(result->records[i].ssid, (char*)wifi_ap_records[i].ssid);
            // strcpy(result->records[i].bssid,
            // (char*)wifi_ap_records[i].bssid);
            memcpy(result->records[i].bssid, wifi_ap_records[i].bssid,
                   sizeof(wifi_ap_records[i].bssid));
            result->records[i].bssid_len = sizeof(wifi_ap_records[i].bssid);
            result->records[i].channel = wifi_ap_records[i].primary;
            result->records[i].rssi = wifi_ap_records[i].rssi;
            result->records[i].authmode = wifi_ap_records[i].authmode;
        }
        free(wifi_ap_records);
        *(pika_hal_WIFI_scan_result**)arg = result;
        return 0;
    }
    return -1;
}
