#include "PikaPiZero_RGB.h"
#include <stdint.h>
#include "BaseObj.h"
#include "RGB_ASM.h"
#include "STM32_common.h"
#include "dataStrs.h"

void RGB_reset() {
    GPIOB->BRR = GPIO_PIN_12;  // reset
    delay_us(50);
}

#define RED 0x000800
#define GREEN 0x080000
#define BLUE 0x000008
#define WHITE 0x080808
#define CUTDOWN 0x000000

void RGB_setVoid() {
    __asm("nop");
}

void PikaPiZero_RGB_enable(PikaObj* self) {
    __HAL_RCC_GPIOB_CLK_ENABLE();
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    /*Configure GPIO*/
    /* PB12 */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_12;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);    
}
void PikaPiZero_RGB_init(PikaObj* self) {}

void PikaPiZero_RGB_red(PikaObj* self) {
    RGB_set(RED);
    RGB_set(RED);
    RGB_set(RED);
    RGB_set(RED);
}

void PikaPiZero_RGB_blue(PikaObj* self) {
    RGB_set(BLUE);
    RGB_set(BLUE);
    RGB_set(BLUE);
    RGB_set(BLUE);
}

void PikaPiZero_RGB_green(PikaObj* self) {
    RGB_set(GREEN);
    RGB_set(GREEN);
    RGB_set(GREEN);
    RGB_set(GREEN);
}

void PikaPiZero_RGB_white(PikaObj* self) {
    RGB_set(WHITE);
    RGB_set(WHITE);
    RGB_set(WHITE);
    RGB_set(WHITE);
}

void PikaPiZero_RGB_flow(PikaObj* self) {
    if (!obj_isArgExist(self, "flowState")) {
        obj_setInt(self, "flowState", 0);
    }
    int flowState = obj_getInt(self, "flowState");
    if (0 == flowState) {
        RGB_set(RED);
        RGB_set(BLUE);
        RGB_set(GREEN);
        RGB_set(WHITE);
        goto exit;
    }
    if (1 == flowState) {
        RGB_set(BLUE);
        RGB_set(GREEN);
        RGB_set(WHITE);
        RGB_set(RED);
        goto exit;
    }
    if (2 == flowState) {
        RGB_set(GREEN);
        RGB_set(WHITE);
        RGB_set(RED);
        RGB_set(BLUE);
        goto exit;
    }
    if (3 == flowState) {
        RGB_set(WHITE);
        RGB_set(RED);
        RGB_set(BLUE);
        RGB_set(GREEN);
        goto exit;
    }

exit:
    obj_setInt(self, "flowState", (flowState + 1) % 4);
}
