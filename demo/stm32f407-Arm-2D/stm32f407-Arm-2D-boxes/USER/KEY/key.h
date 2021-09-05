#ifndef __KEY_H
#define __KEY_H	 
#include "common.h" 

//////////////////////////////////////////////////////////////////////////////////	 

//按键IO端口定义
#define KEY0 		PFin(9)   
#define KEY1 		PFin(8)		
#define KEY2 		PFin(7)		
#define KEY3 	  PFin(6)		


//按键值定义
#define KEY0_DATA	  1
#define KEY1_DATA	  2
#define KEY2_DATA	  3
#define KEY3_DATA   4

//变量声明
extern u8   keydown_data;    //按键按下后就返回的值
extern u8   keyup_data;      //按键抬起返回值
extern u16  key_time;
extern u8   key_tem; 

//函数声明
void KEY_Init(void);	      //IO初始化
void key_scan(u8 mode);  		//按键扫描函数	

#endif
