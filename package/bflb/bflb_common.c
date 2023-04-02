#include "bflb_Camera.h"

#include "bflb_i2c.h"
#include "bflb_cam.h"
#include "bflb_gpio.h"
#include "image_sensor.h"
#include "lvgl.h"
#include "board.h"
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"

PikaEventListener* g_pika_bflb_event_listener = NULL;
volatile uint8_t g_callback_thread_inited = 0;

volatile uint8_t g_cam_callback_task_flag = 0;
volatile uint8_t g_mic_callback_task_flag = 0;

void cam_py_callback(void);
void mic_py_callback(void);

static void _callback_task(void *arg) {
    while (1) {
        if (g_cam_callback_task_flag) {
            cam_py_callback();
            g_cam_callback_task_flag = 0;
        }
        if (g_mic_callback_task_flag) {
            mic_py_callback();
            g_mic_callback_task_flag = 0;
        }
        vTaskDelay(10);
    }
}

void _callback_thread_init(void){
    /* crate callbck thread */
    if (!g_callback_thread_inited) {
        g_callback_thread_inited = 1;
        xTaskCreate(_callback_task, "cam_callback_task", 8192, NULL, 1,
                    NULL);
    }
}
