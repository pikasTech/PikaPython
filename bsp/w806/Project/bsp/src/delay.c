/*
 * delay.c
 *
 *  Created on: Oct 27, 2021
 *      Author: jiangyuanyuan
 */

/* Includes ------------------------------------------------------------------*/
#include "delay.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure DELAY                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void delay_us(uint32_t nus)
{  
  uint32_t i,j;  
  for(i = 0; i < nus; i++)
  {     
		j=51;   //change it  for timing adjustment
		while(j--)
		{  
			__NOP();  // nop's may be added or removed for timing  
		} 
  }
}

void delay_ms(uint16_t nms)
{
  uint16_t i,j,k; 
	for(i = 0; i < nms; i++)
	{
		for(j = 0; j < 1000; j++)
		{     
			k=51;   //change it  for timing adjustment
			while(k--)
			{  
				__NOP();  // nop's may be added or removed for timing  
			} 
		}
	}	
}

void HAL_Delayms(uint32_t ms)
{
	HAL_Delay(ms);
}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

/* USER CODE BEGIN 3 */

/* USER CODE END 3 */

/************************ (C) COPYRIGHT winnermicro *****END OF FILE****/
