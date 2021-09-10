#include "STM32_UART.h"
#include "BaseObj.h"
#include "STM32_common.h"
#include "dataStrs.h"
#include <stdint.h>

UART_HandleTypeDef pika_huart1;
UART_HandleTypeDef pika_huart2;
UART_HandleTypeDef pika_huart3;
UART_HandleTypeDef pika_huart4;

#define RX_BUFF_LENGTH 256

char pika_huart1_rxBuff[RX_BUFF_LENGTH] = {0};
char pika_huart2_rxBuff[RX_BUFF_LENGTH] = {0};
char pika_huart3_rxBuff[RX_BUFF_LENGTH] = {0};
char pika_huart4_rxBuff[RX_BUFF_LENGTH] = {0};

PikaObj *pika_huart1_obj = NULL;
PikaObj *pika_huart2_obj = NULL;
PikaObj *pika_huart3_obj = NULL;
PikaObj *pika_huart4_obj = NULL;

static void setUartObj(uint8_t id, PikaObj *obj) {
  if (1 == id) {
    pika_huart1_obj = obj;
  }
  if (2 == id) {
    pika_huart2_obj = obj;
  }
  if (3 == id) {
    pika_huart3_obj = obj;
  }
  if (4 == id) {
    pika_huart4_obj = obj;
  }
}

static PikaObj *getUartObj(uint8_t id) {
  if (1 == id) {
    return pika_huart1_obj;
  }
  if (2 == id) {
    return pika_huart2_obj;
  }
  if (3 == id) {
    return pika_huart3_obj;
  }
  if (4 == id) {
    return pika_huart4_obj;
  }
  return NULL;
}

static USART_TypeDef *getUartInstance(uint8_t id) {
  if (1 == id) {
    return USART1;
  }
  if (2 == id) {
    return USART2;
  }
  if (3 == id) {
    return USART3;
  }
  if (4 == id) {
    return USART4;
  }
  return NULL;
}

static uint8_t getUartId(UART_HandleTypeDef *huart) {
  if (huart == &pika_huart1) {
    return 1;
  }
  if (huart == &pika_huart2) {
    return 2;
  }
  if (huart == &pika_huart3) {
    return 3;
  }
  if (huart == &pika_huart4) {
    return 4;
  }
  return 0;
}

static UART_HandleTypeDef *getUartHandle(uint8_t id) {
  if (1 == id) {
    return &pika_huart1;
  }
  if (2 == id) {
    return &pika_huart2;
  }
  if (3 == id) {
    return &pika_huart3;
  }
  if (4 == id) {
    return &pika_huart4;
  }
  return NULL;
}

static char *getUartRxBuff(uint8_t id) {
  if (1 == id) {
    return pika_huart1_rxBuff;
  }
  if (2 == id) {
    return pika_huart2_rxBuff;
  }
  if (3 == id) {
    return pika_huart3_rxBuff;
  }
  if (4 == id) {
    return pika_huart4_rxBuff;
  }
  return NULL;
}

static uint8_t USART_UART_Init(uint32_t baudRate, uint8_t id) {
  uint8_t errCode = 0;

  UART_HandleTypeDef *huart = getUartHandle(id);
  huart->Instance = getUartInstance(id);
  huart->Init.BaudRate = baudRate;
  huart->Init.WordLength = UART_WORDLENGTH_8B;
  huart->Init.StopBits = UART_STOPBITS_1;
  huart->Init.Parity = UART_PARITY_NONE;
  huart->Init.Mode = UART_MODE_TX_RX;
  huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart->Init.OverSampling = UART_OVERSAMPLING_16;
  huart->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart->Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(huart) != HAL_OK) {
    errCode = 1;
    goto exit;
  }
  if (HAL_UARTEx_SetTxFifoThreshold(huart, UART_TXFIFO_THRESHOLD_1_8) !=
      HAL_OK) {
    errCode = 2;
    goto exit;
  }
  if (HAL_UARTEx_SetRxFifoThreshold(huart, UART_RXFIFO_THRESHOLD_1_8) !=
      HAL_OK) {
    errCode = 3;
    goto exit;
  }
  if (HAL_UARTEx_DisableFifoMode(huart) != HAL_OK) {
    errCode = 4;
    goto exit;
  }
exit:
  return errCode;
}

static void UART_MspInit(UART_HandleTypeDef *uartHandle) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (uartHandle->Instance == USART1) {
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  } else if (uartHandle->Instance == USART2) {
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  } else if (uartHandle->Instance == USART3) {
    /* USART3 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART3 GPIO Configuration
    PA5     ------> USART3_TX
    PB0     ------> USART3_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF4_USART3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF4_USART3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USART3 interrupt Init */
    HAL_NVIC_SetPriority(USART3_4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART3_4_IRQn);
  } else if (uartHandle->Instance == USART4) {
    /* USART4 clock enable */
    __HAL_RCC_USART4_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART4 GPIO Configuration
    PA0     ------> USART4_TX
    PA1     ------> USART4_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF4_USART4;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART4 interrupt Init */
    HAL_NVIC_SetPriority(USART3_4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART3_4_IRQn);
  }
}

/* Msp handle interrupt */
void USART1_IRQHandler(void) { HAL_UART_IRQHandler(&pika_huart1); }
void USART2_IRQHandler(void) { HAL_UART_IRQHandler(&pika_huart2); }
void USART3_4_IRQHandler(void) {
  HAL_UART_IRQHandler(&pika_huart3);
  HAL_UART_IRQHandler(&pika_huart4);
}

void STM32_UART_platformEnable(PikaObj *self, int baudRate, int id) {
  setUartObj(id, self);
  UART_HandleTypeDef * huart = getUartHandle(id);
  huart->Instance = getUartInstance(id);
  UART_MspInit(huart);
  int errCode = USART_UART_Init(baudRate, id);
  if (0 != errCode) {
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] uart init faild.");
    return;
  }
  HAL_UART_Receive_IT(getUartHandle(id), (uint8_t *)getUartRxBuff(id), 1);
}

char *STM32_UART_platformRead(PikaObj *self, int id, int length) {}

void STM32_UART_platformWrite(PikaObj *self, char *data, int id) {
  HAL_UART_Transmit(getUartHandle(id), (uint8_t *)data, strGetSize(data), 100);
}

/* Recive Interrupt Handler */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  uint8_t id = getUartId(huart);
  char *RxBuff = getUartRxBuff(id);
  uint16_t offSet = strGetSize(RxBuff);
  HAL_UART_Receive_IT(getUartHandle(id), (uint8_t *)(RxBuff + offSet), 1);  
  goto exit;
exit:
  return;
}

/**
  * 函数功能: 重定向c库函数printf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&pika_huart1, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}
 
/**
  * 函数功能: 重定向c库函数getchar,scanf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
int fgetc(FILE *f)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&pika_huart1, &ch, 1, 0xffff);
  return ch;
}