#include "_bluetooth_BLE.h"

int _bluetooth_BLE___init__(PikaObj* self) {
    pika_platform_printf("Init BLE\r\n");
    return 1;
}

int _bluetooth_BLE_active(PikaObj* self) {
    pika_platform_printf("Active BLE\r\n");
    // esp_err_t ret = nvs_flash_init();
    // if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret ==
    // ESP_ERR_NVS_NEW_VERSION_FOUND) {
    //     ESP_ERROR_CHECK(nvs_flash_erase());
    //     ret = nvs_flash_init();
    // }
    // ESP_ERROR_CHECK(ret);

    // ret = nimble_port_init();
    // if (ret != ESP_OK) {
    //     printf("Failed to init nimble %d \n", ret);
    //     return;
    // }
    return 0;
}

int _bluetooth_BLE_test(PikaObj* self) {
    printf("test");
    return 0;
}
