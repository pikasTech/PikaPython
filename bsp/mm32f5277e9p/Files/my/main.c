/*
 * Copyright 2021 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <stdio.h>

#include "board_init.h"


/*
 * Definitions.
 */

/*
 * Variables.
 */

/*
 * Declerations.
 */
void    app_uart_init(void);
void    app_uart_putchar(uint8_t c);
uint8_t app_uart_getchar(void);
void    app_uart_putstr(uint8_t *str);

/*
 * Functions.
 */
int main(void)
{
    uint8_t c;

    BOARD_Init();

    app_uart_init();
    app_uart_putstr((uint8_t *)"\r\nuart_basic example.\r\n");

    while (1)
    {
        c = app_uart_getchar();
        app_uart_putchar(c);
    }
}

void app_uart_init(void)
{
    UART_Init_Type uart_init;

    uart_init.ClockFreqHz   = BOARD_DEBUG_UART_FREQ;
    uart_init.BaudRate      = BOARD_DEBUG_UART_BAUDRATE;
    uart_init.WordLength    = UART_WordLength_8b;
    uart_init.StopBits      = UART_StopBits_1;
    uart_init.Parity        = UART_Parity_None;
    uart_init.XferMode      = UART_XferMode_RxTx;
    uart_init.HwFlowControl = UART_HwFlowControl_None;
    UART_Init(BOARD_DEBUG_UART_PORT, &uart_init);
    UART_Enable(BOARD_DEBUG_UART_PORT, true);
}

void app_uart_putchar(uint8_t c)
{
	while ( 0u == (UART_STATUS_TX_EMPTY & UART_GetStatus(BOARD_DEBUG_UART_PORT)) )
    {}
    UART_PutData(BOARD_DEBUG_UART_PORT, (uint8_t)(c));
}

uint8_t app_uart_getchar(void)
{
    while ( 0u == (UART_STATUS_RX_DONE & UART_GetStatus(BOARD_DEBUG_UART_PORT)) )
    {}
    return UART_GetData(BOARD_DEBUG_UART_PORT);
}

void app_uart_putstr(uint8_t *str)
{
    while ((*str) != '\0')
    {
        app_uart_putchar(*str);
        str++;
    }
}

/* EOF. */
