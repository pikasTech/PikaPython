
#include <stdio.h>
#include "wm_hal.h"

void Error_Handler(void);

/******************************0x80FFFFF
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
 * ****************************0x8000000*/

#define LEN 5000
#define TEST_ADDR 0x80000
uint8_t test_buf[LEN]={0};

int main(void)
{
	int i = 0;
	
	SystemClock_Config(CPU_CLK_160M);
	printf("enter main\r\n");
	for (i = 0; i < LEN; i++)
	{
		test_buf[i] = i % 256;
	}
	
	HAL_FLASH_Write(TEST_ADDR, test_buf, 100);
	HAL_FLASH_Write(TEST_ADDR + 100, test_buf + 100, 4096);
	HAL_FLASH_Write(TEST_ADDR + 100 + 4096, test_buf + 100 + 4096, LEN - 100 - 4096);
	
	memset(test_buf, 0, LEN);
	HAL_FLASH_Read(TEST_ADDR, test_buf, LEN);
	for (i = 0; i < 100; i++)
	{
		if (test_buf[i] != (i % 256))
		{
			printf("test failed\r\n");
			break;
		}
	}
	if (i == 100)
	{
		printf("test success\r\n");
	}

	while (1)
	{
		printf(".");
		HAL_Delay(1000);
	}
}

void Error_Handler(void)
{
	while (1)
	{
	}
}

void assert_failed(uint8_t *file, uint32_t line)
{
	printf("Wrong parameters value: file %s on line %d\r\n", file, line);
}