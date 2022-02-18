/*
 * flash.c
 *
 *  Created on: Oct 27, 2021
 *      Author: jiangyuanyuan
 ******************************0x80FFFFF
 *            USER PARAM
 * ****************************0x80XXXXX
 *            RUN IMAGE
 * ****************************0x8010400
 *            RUN IMAGE HEADER
 * ****************************0x8010000
 *            SECBOOT IMAGE
 * ****************************0x8002400
 *            SECBOOT IMAGE HEAER
 * ****************************0x8002000
 *            CHIP DATA
 * ****************************0x8000000
 */
/* Includes ------------------------------------------------------------------*/
#include "flash.h"

/* USER CODE BEGIN 0 */
#define FLASH_ADDR_LEN 5000
#define TEST_FLASH_ADDR 0x80000
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */
uint8_t test_flash_buf[FLASH_ADDR_LEN]={0};
/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/

/* USER CODE BEGIN 2 */
void MY_Flash_Test(void)
{
	int i = 0;
	for (i = 0; i < FLASH_ADDR_LEN; i++)
	{
		test_flash_buf[i] = i % 256;
	}
	
	HAL_FLASH_Write(TEST_FLASH_ADDR, test_flash_buf, 100);
	HAL_FLASH_Write(TEST_FLASH_ADDR + 100, test_flash_buf + 100, 4096);
	HAL_FLASH_Write(TEST_FLASH_ADDR + 100 + 4096, test_flash_buf + 100 + 4096, FLASH_ADDR_LEN - 100 - 4096);
	
	memset(test_flash_buf, 0, FLASH_ADDR_LEN);
	HAL_FLASH_Read(TEST_FLASH_ADDR, test_flash_buf, FLASH_ADDR_LEN);
	for (i = 0; i < 100; i++)
	{
		if (test_flash_buf[i] != (i % 256))
		{
			printf("test flash failed\r\n");
			break;
		}
	}
	if (i == 100)
	{
		printf("test flash success\r\n");
	}
}
/* USER CODE END 2 */

/* USER CODE BEGIN 3 */

/* USER CODE END 3 */

/************************ (C) COPYRIGHT winnermicro *****END OF FILE****/
