#include "LCD_Driver.h"
#include "LCD_Config.h"
#include "arm2d_config.h"

void __aeabi_assert (const char *expr, const char *file, int line) {
}


int32_t __Arm2D_platform_drawRegin(uint32_t x,
                                   uint32_t y,
                                   uint32_t width,
                                   uint32_t height,
                                   const uint8_t* bitmap) {
    LCD_DrawRegin(x, y, x + width, y + height, (uint16_t *)bitmap);
    return 0;
}

int32_t __Arm2D_platform_Init() {
    LCD_Init();
    return 0;
}
