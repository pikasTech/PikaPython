#ifndef __GT911_H__
#define __GT911_H__

#include "SWM320.h"
#include "dev_systick.h"

#define TP_PRES_DOWN 0x80 //触屏被按下
#define CT_MAX_TOUCH 5    //电容屏支持的点数,固定为5点

//触摸屏控制器
typedef struct
{
    uint16_t x[CT_MAX_TOUCH]; //当前坐标
    uint16_t y[CT_MAX_TOUCH]; //电容屏有最多5组坐标
    uint8_t sta;              //笔的状态
                              //b7:按下1/松开0;
                              //b6~b5:保留
                              //b4~b0:电容触摸屏按下的点数(0,表示未按下,1表示按下)
} _m_tp_dev;

extern _m_tp_dev tp_dev;

uint8_t GT911_Init(void);
void GT911_Scan(void);

#endif //__GT911_H__
