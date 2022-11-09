#ifndef __DEV_GT9147_H__
#define __DEV_GT9147_H__

#include "SWM320.h"
#include "dev_systick.h"

/*复位引脚*/
#define GPIO_GT_RST GPIOC
#define PIN_GT_RST PIN4
/*中断引脚*/
#define GPIO_GT_INT GPIOC
#define PIN_GT_INT PIN5
//I2C读写命令
//#define GT_CMD_WR 0x28 //写命令
//#define GT_CMD_RD 0x29 //读命令
#define GT_CMD_WR 0xBA //写命令
#define GT_CMD_RD 0xBB //读命令
//GT9147 部分寄存器定义
#define GT_CTRL_REG 0x8040  //GT9147控制寄存器
#define GT_CFGS_REG 0x8047  //GT9147配置起始地址寄存器
#define GT_CHECK_REG 0x80FF //GT9147校验和寄存器
#define GT_PID_REG 0x8140   //GT9147产品ID寄存器

#define GT_GSTID_REG 0x814E //GT9147当前检测到的触摸情况
#define GT_TP1_REG 0x8150   //第一个触摸点数据地址
#define GT_TP2_REG 0x8158   //第二个触摸点数据地址
#define GT_TP3_REG 0x8160   //第三个触摸点数据地址
#define GT_TP4_REG 0x8168   //第四个触摸点数据地址
#define GT_TP5_REG 0x8170   //第五个触摸点数据地址

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
    struct Timer timer;
} _m_tp_dev;

extern _m_tp_dev tp_dev;

void gt9147_init(void);
void gt9147_scan(void);
#endif
