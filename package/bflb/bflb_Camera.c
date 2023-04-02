#include "bflb_Camera.h"
#include "bflb_common.h"

#include "bflb_i2c.h"
#include "bflb_cam.h"
#include "bflb_gpio.h"
#include "image_sensor.h"
#include "lvgl.h"
#include "board.h"
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"

// Global variables
static struct bflb_device_s *i2c0;
struct bflb_device_s *cam0;
static struct bflb_cam_config_s cam_config;
static struct image_sensor_config_s *sensor_config;
static volatile uint8_t g_cam_inited = 0;
static volatile uint8_t g_cam_callback_inited = 0;
static lv_obj_t *canvas_cam;

extern PikaEventListener* g_pika_bflb_event_listener;
extern volatile uint8_t g_callback_thread_inited;
extern volatile uint8_t g_cam_callback_task_flag;

void canvas_cam_demo_update(void *pic_addr);

static void cam_demo_callback(void) {
    if (!canvas_cam) {
        return;
    }

    uint16_t* pic_addr;
    uint32_t pic_size;

    pic_size = bflb_cam_get_frame_info(cam0, (void*)&pic_addr);
    bflb_cam_pop_one_frame(cam0);
    for (size_t i = 0; i < pic_size / sizeof(uint16_t); i++) {
        pic_addr[i] = __bswap16(pic_addr[i]);
    }
    canvas_cam_demo_update(pic_addr);
}

void cam_py_callback(void) {
    if (!g_cam_callback_inited) {
        return;
    }
    pika_debug("cam_py_callback\r\n");
    pks_eventListener_sendSignal(g_pika_bflb_event_listener, (uintptr_t)cam0, 0);
}

static void cam_isr(int irq, void* arg) {
    static volatile uint32_t cam_int_cnt = 0;

    bflb_cam_int_clear(cam0, CAM_INTCLR_NORMAL);
    // pika_debug("CAM interrupt, pop picture %d: 0x%08x, len: %d\r\n",
    // cam_int_cnt++, (uint32_t)pic_addr, pic_size);
    cam_demo_callback();
    g_cam_callback_task_flag = 1;
    // cam_py_callback();
}

static void cam_init(void) {
    struct bflb_cam_config_s cam_config;
    struct image_sensor_config_s* sensor_config;
    struct bflb_device_s* i2c0;

    i2c0 = bflb_device_get_by_name("i2c0");
    cam0 = bflb_device_get_by_name("cam0");

    if (image_sensor_scan(i2c0, &sensor_config)) {
        pika_debug("Sensor name: %s", sensor_config->name);
    } else {
        pika_platform_printf("\r\nError! Can't identify sensor!\r\n");
        cam0 = NULL;
        return;
    }

    _callback_thread_init();

    bflb_cam_int_mask(cam0, CAM_INTMASK_NORMAL, false);
    bflb_irq_attach(cam0->irq_num, cam_isr, NULL);
    bflb_irq_enable(cam0->irq_num);

    memcpy(&cam_config, sensor_config, IMAGE_SENSOR_INFO_COPY_SIZE);
    cam_config.with_mjpeg = false;
    cam_config.output_format = CAM_OUTPUT_FORMAT_AUTO;
    static lv_color_t cam_buffer[4][320 * 240] __section(".psmram_data");
    cam_config.output_bufaddr = (uint32_t)(uintptr_t)(void*)cam_buffer;
    cam_config.output_bufsize = sizeof(cam_buffer);

    bflb_cam_init(cam0, &cam_config);
    pika_debug("cam init ok");
    // bflb_cam_stop(cam0);
}

void init_cam(struct bflb_device_s *gpio) {
    /* DVP0 GPIO init */
    /* I2C GPIO */
    // bflb_gpio_init(gpio, GPIO_PIN_0, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    // bflb_gpio_init(gpio, GPIO_PIN_1, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);

    /* Power down GPIO */
    bflb_gpio_init(gpio, GPIO_PIN_16, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_reset(gpio, GPIO_PIN_16);

    /* MCLK GPIO */
    bflb_gpio_init(gpio, GPIO_PIN_6, GPIO_FUNC_CLKOUT | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);

    /* DVP0 GPIO */
    bflb_gpio_init(gpio, GPIO_PIN_24, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_25, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_26, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_27, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_28, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_29, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_30, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_31, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_32, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_33, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_34, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);

    cam_init();
}

void bflb_Camera___init__(PikaObj *self) {
    pika_debug("cam init");
    if(!g_cam_inited) {
        init_cam(bflb_device_get_by_name("gpio"));
        g_cam_inited = 1;
    }
}

void bflb_Camera_start(PikaObj *self) {
    pika_debug("cam start");
    bflb_cam_start(cam0);
}

void bflb_Camera_stop(PikaObj *self) {
    pika_debug("cam stop");
    bflb_cam_stop(cam0);
}

int bflb_Camera_get_frame_count(PikaObj *self) {
    return (int)bflb_cam_get_frame_count(cam0);
}

PikaObj* bflb_Camera_get_frame_info(PikaObj *self) {
    uint8_t *pic_addr;
    uint32_t pic_size;
    pic_size = bflb_cam_get_frame_info(cam0, &pic_addr);
    obj_setInt(self, "pic_addr", (uintptr_t)pic_addr);
    obj_setInt(self, "pic_size", pic_size);
    return obj_newTuple(arg_newInt((uintptr_t)pic_addr), arg_newInt(pic_size));
}

void bflb_Camera_pop_one_frame(PikaObj *self) {
    bflb_cam_pop_one_frame(cam0);
    uint16_t* pic_addr = (uint16_t*)obj_getInt(self, "pic_addr");
    uint32_t pic_size = obj_getInt(self, "pic_size");
    pika_debug("pic_addr: %p, pic_size: %d", pic_addr, pic_size);
    for (size_t i = 0; i < pic_size / sizeof(uint16_t); i++) {
        pic_addr[i] = __bswap16(pic_addr[i]);
    }
    pika_debug("after bswap16");
}

static lv_obj_t *canvas_cam_create(lv_obj_t *parent);

void demo(void) {
    bflb_cam_start(cam0);
    canvas_cam = canvas_cam_create(lv_scr_act());
}

void canvas_cam_demo_update(void *pic_addr) {
    lv_obj_t *canvas = canvas_cam;
    lv_canvas_set_buffer(canvas, pic_addr, 320, 240, LV_IMG_CF_TRUE_COLOR);
}

static lv_obj_t *canvas_cam_create(lv_obj_t *parent) {
    lv_obj_t *canvas;
    canvas = lv_canvas_create(parent);
    lv_obj_set_size(canvas, 320, 240);
    lv_obj_align(canvas, LV_ALIGN_TOP_MID, 0, 0);

    return canvas;
}

void bflb_Camera_demo(PikaObj *self) {
    demo();
}

void bflb_Camera_set_callback(PikaObj *self, Arg* callback){
    obj_setArg(self, "eventCallBack", callback);
    /* init event_listener for the first time */
    if (NULL == g_pika_bflb_event_listener) {
        pks_eventListener_init(&g_pika_bflb_event_listener);
    }
    uint32_t eventId = (uintptr_t)cam0;
    pks_eventListener_registEvent(g_pika_bflb_event_listener, eventId, self);
    g_cam_callback_inited = 1;
    pika_debug("bflb_Camera_set_callback: %p\r\n", eventId);
}
