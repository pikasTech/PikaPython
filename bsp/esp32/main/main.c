#include "PikaVM.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "driver/usb_serial_jtag.h"
#include "esp_core_dump.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_vfs_dev.h"
#include "esp_vfs_fat.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "sdmmc_cmd.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_ota_ops.h"
#include "esp_partition.h"
#include "esp_wifi.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "pikaScript.h"
#include <esp_ota_ops.h>
#include <esp_partition.h>
#include <time.h>

static volatile char gRXC = EOF;
static volatile uint8_t gNeedPut = 0;

char pika_platform_getchar() {
  while (1) {
    char buff[1] = {0};
    if (usb_serial_jtag_read_bytes(buff, 1, 100) > 0) {
      return buff[0];
    }
    vTaskDelay(1);
  }
}

int pika_platform_putchar(char ch) {
  usb_serial_jtag_write_bytes(&ch, 1, 0);
  return 0;
}

int64_t pika_platform_get_tick(void) {
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

void app_main(void) {
    usb_serial_jtag_driver_config_t usb_serial_jtag_config = {
      .rx_buffer_size = 1024 * 8, .tx_buffer_size = 1024};
    usb_serial_jtag_driver_install(&usb_serial_jtag_config);
    esp_vfs_usb_serial_jtag_use_driver();

    printf("Minimum free heap size: %" PRIu32 " bytes\n",
           esp_get_minimum_free_heap_size());

    pikaScriptShell(pikaScriptInit());
}

void pika_platform_reboot(void) {
  /* reboot */
  abort();
}
