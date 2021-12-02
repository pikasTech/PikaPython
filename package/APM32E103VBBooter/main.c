/*!
 * @file        main.c
 *
 * @brief       Main program body
 *
 * @version     V1.0.0
 *
 * @date        2021-07-26
 *
 */

#include "main.h"
#include <stdio.h>
#include "apm32e10x.h"
#include "apm32e10x_gpio.h"
#include "apm32e10x_rcm.h"
#include "apm32e10x_eint.h"
#include "apm32e10x_usart.h"
#include "apm32e10x_misc.h"
#include "pikaScript.h"

volatile uint32_t tick = 0;
void printf_hardware_init(void);

void Delay(void);

/*!
 * @brief       Main program   
 *
 * @param       None
 *
 * @retval      None
 *
 */
int main(void)
{
    printf_hardware_init();
    SysTick_Config(SystemCoreClock / 1000);
    printf("[info]: Apm32e103vb System Init Ok\r\n");
    PikaObj * pikaMain = pikaScriptInit();
    while(1)
    {
    }
}

void printf_hardware_init(void){
    GPIO_Config_T GPIO_ConfigStruct;
    USART_Config_T USART_ConfigStruct;
    
    RCM_EnableAPB2PeriphClock((RCM_APB2_PERIPH_T)(RCM_APB2_PERIPH_GPIOA | RCM_APB2_PERIPH_USART1));
    
    GPIO_ConfigStruct.mode = GPIO_MODE_AF_PP;
    GPIO_ConfigStruct.pin = GPIO_PIN_9;
    GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
    GPIO_Config(GPIOA, &GPIO_ConfigStruct);
    
    USART_ConfigStruct.baudRate = 115200;
    USART_ConfigStruct.hardwareFlow = USART_HARDWARE_FLOW_NONE;
    USART_ConfigStruct.mode = USART_MODE_TX;
    USART_ConfigStruct.parity = USART_PARITY_NONE;
    USART_ConfigStruct.stopBits = USART_STOP_BIT_1;
    USART_ConfigStruct.wordLength = USART_WORD_LEN_8B;
    USART_Config(USART1, &USART_ConfigStruct);
    
    USART_Enable(USART1);
}

/*!
 * @brief       Delay
 *
 * @param       None
 *
 * @retval      None
 *
 */
void Delay(void)
{
    tick = 0;
    
    while(tick < 500);
}

int fputc(int ch, FILE *f)
{
    /** send a byte of data to the serial port */
    USART_TxData(USART1,(uint8_t)ch);
    /** wait for the data to be send  */
    while (USART_ReadStatusFlag(USART1, USART_FLAG_TXBE) == RESET);        
    return (ch);
}
