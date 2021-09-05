#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "pikaScript.h"

void Device_LED_off(PikaObj *self)
{
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
}
void Device_LED_on(PikaObj *self)
{
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
}
