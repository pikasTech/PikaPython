#include "PikaPiZero_LCD.h"
#include "LCD_Driver.h"
u16 LCD_getColorCode(char * color);

void PikaPiZero_LCD_clear(PikaObj *self, char * color){
    u16 color_code = WHITE;
    color_code = LCD_getColorCode(color);
    LCD_Clear(color_code);
}
void PikaPiZero_LCD_fill(PikaObj *self, char * color, int hight, int wight, int x0, int y0){
    u16 color_code = WHITE;
    color_code = LCD_getColorCode(color);
    LCD_Fill(x0, y0, hight, wight, color_code);
}
void PikaPiZero_LCD_init(PikaObj *self){
    LCD_Init();
}
u16 LCD_getColorCode(char * color){
    if(strEqu(color, "white")){
        return WHITE;
    }
    
    if(strEqu(color, "blue")){
        return BLUE;
    }
    
    if(strEqu(color, "red")){
        return RED;
    }
    
    if(strEqu(color, "yellow")){
        return YELLOW;
    }  
    
    if(strEqu(color, "black")){
        return BLACK;
    }    
    
    if(strEqu(color, "green")){
        return GREEN;
    }
    return WHITE;    
}