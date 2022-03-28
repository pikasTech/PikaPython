/*
 * mytask.c
 *
 *  Created on: Oct 27, 2021
 *      Author: derek
 */
 
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "mytask.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define START_TASK_PRIO		1		//任务优先级
#define START_STK_SIZE 		128 	//任务堆栈大小	

#define TASK1_TASK_PRIO		2		//任务优先级
#define TASK1_STK_SIZE 		4096 	//任务堆栈大小	

#define TASK2_TASK_PRIO		3		//任务优先级
#define TASK2_STK_SIZE 		128 	//任务堆栈大小	

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
TaskHandle_t StartTask_Handler;		//任务句柄
TaskHandle_t Task1Task_Handler;		//任务句柄
TaskHandle_t Task2Task_Handler;		//任务句柄
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
//指定heap_5.c用到的内存范围
const HeapRegion_t xHeapRegions[] =  
{
	{ ( uint8_t * ) 0x20020000UL, 0x28000 }, 
	{ NULL, 0 }                
};
/* USER CODE END Variables */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void start_task(void *pvParameters);//任务函数
void task1_task(void *pvParameters);//任务函数
void task2_task(void *pvParameters);//任务函数
/* USER CODE END FunctionPrototypes */

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */

/* USER CODE END GET_IDLE_TASK_MEMORY */

/* Create the thread(s) */
/* definition and creation of MX_FREERTOS_Init */
void MX_FREERTOS_Init(void)
{
	//指定heap_5.c用到的内存范围
	vPortDefineHeapRegions( xHeapRegions ); 

	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
    //创建TASK1任务
    xTaskCreate((TaskFunction_t )task1_task,             
                (const char*    )"task1_task",           
                (uint16_t       )TASK1_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )TASK1_TASK_PRIO,        
                (TaskHandle_t*  )&Task1Task_Handler);   
    //创建TASK2任务
    xTaskCreate((TaskFunction_t )task2_task,     
                (const char*    )"task2_task",   
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK2_TASK_PRIO,
                (TaskHandle_t*  )&Task2Task_Handler); 
    vTaskDelete(StartTask_Handler); //删除开始任务
	printf("start_task del sucess!\r\n");
    taskEXIT_CRITICAL();            //退出临界区
}

/* USER CODE BEGIN task1_task */
/**
  * @brief  Function implementing the My freertos thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END task1_task */
void task1_task(void *pvParameters)
{
  /* USER CODE BEGIN task1_task */
  /* Infinite loop */
  uint8_t task1 = 0;
  printf("task python script is enter\r\n");
  
  
  for(;;)
  {
	task1++;
	pikaScriptShell(pikaScriptInit());
	vTaskDelay(10);
  }
  /* USER CODE task1_task */
}

/* USER CODE BEGIN task2_task */
/**
  * @brief  Function implementing the My freertos thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END task2_task */
void task2_task(void *pvParameters)
{
  /* USER CODE BEGIN task2_task */
  /* Infinite loop */
  uint8_t task2 = 0;
  for(;;)
  {
	task2++;
	//printf("task2_task:%d\r\n",task2);
	vTaskDelay(2000);
  }
  /* USER CODE task2_task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void vApplicationIdleHook( void )
{

}
/* USER CODE END Application */



