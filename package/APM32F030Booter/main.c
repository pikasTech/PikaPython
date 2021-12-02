/*!
 * @file        main.c
 *
 * @brief       Main program body          
 *
 * @version     V1.0.1
 *
 * @date        2021-07-01
 */

#include "stdio.h"
#include "apm32f0xx_gpio.h"
#include "apm32f0xx_usart.h"
#include "apm32f0xx_misc.h"
#include "apm32f0xx_eint.h"
#include "apm32f0xx.h"
#include "apm32f0xx_gpio.h"
#include "apm32f0xx_usart.h"
#include "apm32f0xx_rcm.h"
#include "pikaScript.h"

/** printf function configs to USART1*/
#define DEBUG_USART  USART1

/** USART Write data */
void USART_Write(USART_T* usart,uint8_t *dat);
/** Delay */
void Delay(uint32_t count);
/** Buffer compare*/
BOOL BufferCompare(uint8_t *buf1, uint8_t *buf2, uint8_t size);

#define DATA_BUF_SIZE       (32)
/** USART1 receive buffer*/
uint8_t rxDataBufUSART1[DATA_BUF_SIZE] = {0};
/** USART2 transmit buffer*/
uint8_t txDataBufUSART2[DATA_BUF_SIZE] = {0};

void printf_hardware_init(void);

int main(void)
{
    printf_hardware_init();
    printf("[info]: Apm32f030r8 System Init OK\r\n");
    PikaObj * pikaMain = pikaScriptInit();
    while(1)
    {
    }
}

void printf_hardware_init(void){
    GPIO_Config_T gpioConfig;
    USART_Config_T usartConfigStruct;
    /** Enable GPIO clock */
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOA);

    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_USART1);

    /** Connect PXx to USARTx_Tx */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_9, GPIO_AF_PIN1);

    /** Connect PXx to USARTx_Rx */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_10, GPIO_AF_PIN1);

    /** Configure USART Tx as alternate function push-pull */
    gpioConfig.mode = GPIO_MODE_AF;
    gpioConfig.pin = GPIO_PIN_9;
    gpioConfig.speed = GPIO_SPEED_50MHz;
    gpioConfig.outtype = GPIO_OUT_TYPE_PP;
    gpioConfig.pupd = GPIO_PUPD_PU;
    GPIO_Config(GPIOA, &gpioConfig);

    /** Configure USART Rx as input floating */
    gpioConfig.pin = GPIO_PIN_10;
    GPIO_Config(GPIOA, &gpioConfig);


    /** MINI_USARTs configured as follow:
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled
    */
    /*

    */
    usartConfigStruct.baudRate = 115200;
    usartConfigStruct.mode     = USART_MODE_TX_RX;
    usartConfigStruct.hardwareFlowCtrl = USART_FLOW_CTRL_NONE;
    usartConfigStruct.parity   = USART_PARITY_NONE;
    usartConfigStruct.stopBits =  USART_STOP_BIT_1;
    usartConfigStruct.wordLength = USART_WORD_LEN_8B;
    USART_Config(USART1, &usartConfigStruct);

    /** Enable USART_Interrupt_RXBNEIE */
    USART_EnableInterrupt(USART1, USART_INT_RXBNEIE);

    NVIC_EnableIRQRequest(USART1_IRQn, 2);

    /** Enable USART */
    USART_Enable(USART1);
}

void Delay(uint32_t count)
{
    volatile uint32_t delay = count;
    
    while(delay--);
}

 /*!
 * @brief       Redirect C Library function printf to serial port.
 *              After Redirection, you can use printf function.
 *
 * @param       ch:  The characters that need to be send.
 *
 * @param       *f:  pointer to a FILE that can recording all information 
 *              needed to control a stream
 *     
 * @retval      The characters that need to be send.
 *
 * @note
 */
int fputc(int ch, FILE *f)
{
        /** send a byte of data to the serial port */
        USART_TxData(DEBUG_USART,(uint8_t)ch);
        
        /** wait for the data to be send  */
        while (USART_ReadStatusFlag(DEBUG_USART, USART_FLAG_TXBE) == RESET);        
       
        return (ch);
}
