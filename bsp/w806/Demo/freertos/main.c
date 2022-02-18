
#include <stdio.h>
#include "wm_hal.h"
#include "FreeRTOS.h"
#include "task.h"

void Error_Handler(void);
void task1_handle(void *p);
void task2_handle(void *p);

const HeapRegion_t xHeapRegions[] = 
{ 
    { ( uint8_t * ) 0x20020000UL, 0x5000 },
    { NULL, 0 }/*终止数组 */ 
};

TaskHandle_t htask1 = NULL;
TaskHandle_t htask2 = NULL;

/* 使用RreeRTOS时，需要将/include/arch/xt804/csi_config.h里
*  的#define CONFIG_KERNEL_NONE 1宏定义注释掉，否则os的systic
*  会出错导致os系统异常
*/
int main(void)
{
	SystemClock_Config(CPU_CLK_160M);
	printf("enter main\r\n");

	vPortDefineHeapRegions( xHeapRegions );
	xTaskCreate(task1_handle, "task1", 512, NULL, 35, &htask1);
	xTaskCreate(task2_handle, "task2", 512, NULL, 32, &htask2);
	vTaskStartScheduler();

	return 0;
}

void task1_handle(void *p)
{
	for (;;)
	{
		printf("task1_handle\r\n");
		vTaskDelay(1000);
	}
}

void task2_handle(void *p)
{
	for (;;)
	{
		printf("task2_handle\r\n");
		vTaskDelay(1000);
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