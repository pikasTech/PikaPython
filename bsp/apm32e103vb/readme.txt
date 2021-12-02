/*!
 * @file        readme.txt
 *
 * @brief       This file is routine instruction
 *
 * @version     V1.0.0
 *
 * @date        2021-07-26
 *
 */
 
 
&par Example Description 
 
The program shows how to send data by using USART, in this case, 
USART1 sends data to upper computer. You can check the data in a Serial Port Utility

  - USART1 configured as follow:
  - BaudRate = 115200 
  - Word Length = USART_WordLength_8b
  - Stop Bit = USART_StopBits_1
  - Parity = USART_Parity_No
  - Hardware flow control disabled (RTS and CTS signals)
  - Transmit enabled

&par Directory contents 

  - USART/Printf/src/apm32e10x_int.c     Interrupt handlers
  - USART/Printf/src/main.c                      Main program


&par Hardware and Software environment

  - This example runs on APM32E103_MINI Devices.