#ifndef __DELAY_H
#define __DELAY_H 			   
#include 	"air32f10x.h"
	 
void Delay_Init(void);
void Delay_Ms(uint16_t nms);
void Delay_Us(uint32_t nus);

#endif

