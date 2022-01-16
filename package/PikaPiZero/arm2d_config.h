#include "LCD_Driver.h"
#include "LCD_Config.h"

#define ARM2D_LCD_WIDTH X_MAX_PIXEL
#define ARM2D_LCD_HEIGHT Y_MAX_PIXEL

#define ARM2D_PFB_BLOCK_WIDTH ARM2D_LCD_WIDTH
#define ARM2D_PFB_BLOCK_HEIGHT 2



int32_t __Arm2D_platform_drawRegin(uint32_t x,
                                   uint32_t y,
                                   uint32_t width,
                                   uint32_t height,
                                   const uint8_t* bitmap);

int32_t __Arm2D_platform_Init();
