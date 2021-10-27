#include "PikaPiZero_OLED.h"
#include "oled.h"

void PikaPiZero_OLED_drawPoint(PikaObj *self, int x, int y){
    OLED_DrawPoint(x, y, 1);
}

void PikaPiZero_OLED_refresh(PikaObj *self){
    OLED_Refresh();
}

void PikaPiZero_OLED_init(PikaObj *self){
    OLED_Init();
	OLED_ColorTurn(0);
    OLED_DisplayTurn(0);
    OLED_Clear();
}

void PikaPiZero_OLED_clear(PikaObj *self){
    OLED_Clear();
}