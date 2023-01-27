#include "../pikascript-lib/PikaStdDevice/pika_hal.h"
#include "esp_event.h"
#include "esp_mac.h"
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
static esp_netif_t* ap_netif = NULL;

uint32_t _ip_str2u32(char* ip_str) {
    uint32_t ip = 0;
    uint8_t* ip_u8 = (uint8_t*)&ip;
    char* p = ip_str;
    for (int i = 0; i < 4; i++) {
        ip_u8[i] = atoi(p);
        p = strchr(p, '.');
        if (p == NULL) {
            break;
        }
        p++;
    }
    return ip;
}

static void event_handler(void* event_handler_arg,
                          esp_event_base_t event_base,
                          int32_t event_id,
                          void* event_data) {
    pika_debug("event_handler: %p %d", event_base, event_id);
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        pika_debug("event_handler: WIFI_EVENT_STA_START");
        pika_debug("wifi_sta_connect_requested: %d",
                   wifi_sta_connect_requested);
        if (wifi_sta_connect_requested) {
            pika_debug("event_handler: WIFI_EVENT_STA_START: connect");
            esp_wifi_connect();
        }
    } else if (event_id == WIFI_EVENT_AP_START) {
        pika_debug("event_handler: WIFI_EVENT_AP_START");
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
        wifi_sta_connected = PIKA_TRUE;
        wifi_sta_disconn_reason = PIKA_HAL_WIFI_STATUS_GOT_IP;
    } else if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event =
            (wifi_event_ap_staconnected_t*)event_data;
        pika_debug("station " MACSTR " join, AID=%d", MAC2STR(event->mac),
                   event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event =
            (wifi_event_ap_stadisconnected_t*)event_data;
        pika_debug("station " MACSTR " leave, AID=%d", MAC2STR(event->mac),
                   event->aid);
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
    return 0;
}

int pika_hal_platform_WIFI_close(pika_dev* dev) {
    return 0;
}

int pika_hal_platform_WIFI_ioctl_config(pika_dev* dev,
                                        pika_hal_WIFI_config* cfg) {
    if (!wifi_started) {
        return 0;
    }
    if (cfg->mode == PIKA_HAL_WIFI_MODE_STA) {
        pika_platform_printf("Error: STA mode not support config\r\n");
        return -1;
    }
    if (cfg->mode == PIKA_HAL_WIFI_MODE_AP) {
        wifi_config_t esp_wifi_cfg = {0};
        // ESP_ERROR_CHECK(esp_wifi_get_config(WIFI_IF_AP, &esp_wifi_cfg));
        strcpy((char*)esp_wifi_cfg.ap.ssid, cfg->ap_ssid);
        strcpy((char*)esp_wifi_cfg.ap.password, cfg->ap_password);
        esp_wifi_cfg.ap.ssid_len = strlen(cfg->ap_ssid);
        esp_wifi_cfg.ap.channel = cfg->channel;
        esp_wifi_cfg.ap.authmode = WIFI_AUTH_WPA2_PSK;
        esp_wifi_cfg.ap.max_connection = cfg->max_connection - 1;
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &esp_wifi_cfg));
    }
    return 0;
}

int pika_hal_platform_WIFI_ioctl_enable(pika_dev* dev) {
    wifi_mode_t mode;
    pika_hal_WIFI_config* cfg = (pika_hal_WIFI_config*)dev->ioctl_config;
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

        ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                                   &event_handler, dev));
        ESP_ERROR_CHECK(esp_event_handler_register(
            IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, dev));
        pika_debug("set mode: %d", mode);
        esp_wifi_set_mode(mode);
        sta_netif = esp_netif_create_default_wifi_sta();
        ap_netif = esp_netif_create_default_wifi_ap();
        pika_debug("start wifi");
        ESP_ERROR_CHECK(esp_wifi_start());
        wifi_started = PIKA_TRUE;
    }
    return 0;
}

int pika_hal_platform_WIFI_ioctl_disable(pika_dev* dev) {
    if (wifi_started) {
        esp_wifi_stop();
        esp_wifi_deinit();
        wifi_started = PIKA_FALSE;
        return 0;
    }
    return -1;
}

int pika_hal_platform_WIFI_ioctl_others(pika_dev* dev,
                                        PIKA_HAL_IOCTL_CMD cmd,
                                        void* arg) {
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
    if (cmd == PIKA_HAL_IOCTL_WIFI_CONNECT) {
        pika_debug("ioctl connect");
        wifi_config_t esp_wifi_cfg = {0};
        pika_hal_WIFI_connect_config* conncfg =
            (pika_hal_WIFI_connect_config*)arg;
        // ESP_ERROR_CHECK(esp_wifi_get_config(WIFI_IF_STA, &esp_wifi_cfg));
        strcpy((char*)esp_wifi_cfg.sta.ssid, conncfg->ssid);
        strcpy((char*)esp_wifi_cfg.sta.password, conncfg->password);
        if (conncfg->bssid[0] != '\0') {
            esp_wifi_cfg.sta.bssid_set = 1;
            strcpy((char*)esp_wifi_cfg.sta.bssid, conncfg->bssid);
        } else {
            esp_wifi_cfg.sta.bssid_set = 0;
        }
        esp_wifi_cfg.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
        esp_wifi_cfg.sta.sae_pwe_h2e = WPA3_SAE_PWE_BOTH;
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &esp_wifi_cfg));
        pika_debug("connecting to %s, password:%s, bssid_set:%d", conncfg->ssid,
                   conncfg->password, esp_wifi_cfg.sta.bssid_set);
        wifi_sta_connect_requested = PIKA_TRUE;
        ESP_ERROR_CHECK(esp_wifi_connect());
        return 0;
    }
    if (cmd == PIKA_HAL_IOCTL_WIFI_GET_IFCONFIG) {
        pika_hal_WIFI_config* cfg = (pika_hal_WIFI_config*)dev->ioctl_config;
        esp_netif_t* netif = NULL;
        if (cfg->mode == PIKA_HAL_WIFI_MODE_STA) {
            netif = sta_netif;
        } else if (cfg->mode == PIKA_HAL_WIFI_MODE_AP) {
            netif = ap_netif;
        }
        esp_netif_ip_info_t ip_info;
        esp_netif_dns_info_t dns_info;
        esp_netif_get_ip_info(netif, &ip_info);
        esp_netif_get_dns_info(netif, ESP_NETIF_DNS_MAIN, &dns_info);
        pika_hal_WIFI_ifconfig* hal_if = (pika_hal_WIFI_ifconfig*)arg;
        sprintf(hal_if->ip, IPSTR, IP2STR(&ip_info.ip));
        sprintf(hal_if->netmask, IPSTR, IP2STR(&ip_info.netmask));
        sprintf(hal_if->gateway, IPSTR, IP2STR(&ip_info.gw));
        sprintf(hal_if->dns, IPSTR, IP2STR(&dns_info.ip.u_addr.ip4));
        return 0;
    }
    if (cmd == PIKA_HAL_IOCTL_WIFI_SET_IFCONFIG) {
        pika_hal_WIFI_config* cfg = (pika_hal_WIFI_config*)dev->ioctl_config;
        esp_netif_t* netif = NULL;
        if (cfg->mode == PIKA_HAL_WIFI_MODE_STA) {
            netif = sta_netif;
        } else if (cfg->mode == PIKA_HAL_WIFI_MODE_AP) {
            netif = ap_netif;
        }
        pika_hal_WIFI_ifconfig* hal_if = (pika_hal_WIFI_ifconfig*)arg;
        esp_netif_ip_info_t ip_info;
        esp_netif_dns_info_t dns_info;
        ESP_ERROR_CHECK(esp_netif_get_ip_info(netif, &ip_info));
        ESP_ERROR_CHECK(
            esp_netif_get_dns_info(netif, ESP_NETIF_DNS_MAIN, &dns_info));
        ip_info.ip.addr = _ip_str2u32(hal_if->ip);
        ip_info.netmask.addr = _ip_str2u32(hal_if->netmask);
        ip_info.gw.addr = _ip_str2u32(hal_if->gateway);
        dns_info.ip.u_addr.ip4.addr = _ip_str2u32(hal_if->dns);
        if (cfg->mode == PIKA_HAL_WIFI_MODE_STA) {
            esp_netif_dhcpc_stop(netif);
        } else if (cfg->mode == PIKA_HAL_WIFI_MODE_AP) {
            esp_netif_dhcps_stop(netif);
        }
        ESP_ERROR_CHECK(esp_netif_set_ip_info(netif, &ip_info));
        if (dns_info.ip.u_addr.ip4.addr != 0) {
            ESP_ERROR_CHECK(
                esp_netif_set_dns_info(netif, ESP_NETIF_DNS_MAIN, &dns_info));
        }
        if (cfg->mode == PIKA_HAL_WIFI_MODE_AP) {
            esp_netif_dhcps_start(netif);
        }
        return 0;
    }
    return -1;
}
