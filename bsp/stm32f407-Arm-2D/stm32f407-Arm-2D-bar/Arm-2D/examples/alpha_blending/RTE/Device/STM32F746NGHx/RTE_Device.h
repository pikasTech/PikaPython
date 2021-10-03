/* -----------------------------------------------------------------------------
 * Copyright (c) 2013-2019 Arm Limited (or its affiliates). All 
 * rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * $Date:        17. June 2019
 * $Revision:    V1.5.1
 *
 * Project:      RTE Device Configuration for ST STM32F7xx
 * -------------------------------------------------------------------------- */

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

#ifndef __RTE_DEVICE_H
#define __RTE_DEVICE_H


#define GPIO_PORT0      GPIOA
#define GPIO_PORT1      GPIOB
#define GPIO_PORT2      GPIOC
#define GPIO_PORT3      GPIOD
#define GPIO_PORT4      GPIOE
#define GPIO_PORT5      GPIOF
#define GPIO_PORT6      GPIOG
#define GPIO_PORT7      GPIOH
#define GPIO_PORT8      GPIOI
#define GPIO_PORT9      GPIOJ
#define GPIO_PORT10     GPIOK

#define GPIO_PORT(num)  GPIO_PORT##num


// <e> USART1 (Universal synchronous asynchronous receiver transmitter) [Driver_USART1]
// <i> Configuration settings for Driver_USART1 in component ::CMSIS Driver:USART
#define RTE_USART1                      0

//   <o> USART1_TX Pin <0=>Not Used <1=>PA9 <2=>PB6 <3=>PB14
#define RTE_USART1_TX_ID                0
#if    (RTE_USART1_TX_ID == 0)
#define RTE_USART1_TX                   0
#elif  (RTE_USART1_TX_ID == 1)
#define RTE_USART1_TX                   1
#define RTE_USART1_TX_PORT              GPIOA
#define RTE_USART1_TX_BIT               9
#elif  (RTE_USART1_TX_ID == 2)
#define RTE_USART1_TX                   1
#define RTE_USART1_TX_PORT              GPIOB
#define RTE_USART1_TX_BIT               6
#elif  (RTE_USART1_TX_ID == 3)
#define RTE_USART1_TX                   1
#define RTE_USART1_TX_PORT              GPIOB
#define RTE_USART1_TX_BIT               14
#else
#error "Invalid USART1_TX Pin Configuration!"
#endif

//   <o> USART1_RX Pin <0=>Not Used <1=>PA10 <2=>PB7 <3=>PB15
#define RTE_USART1_RX_ID                0
#if    (RTE_USART1_RX_ID == 0)
#define RTE_USART1_RX                   0
#elif  (RTE_USART1_RX_ID == 1)
#define RTE_USART1_RX                   1
#define RTE_USART1_RX_PORT              GPIOA
#define RTE_USART1_RX_BIT               10
#elif  (RTE_USART1_RX_ID == 2)
#define RTE_USART1_RX                   1
#define RTE_USART1_RX_PORT              GPIOB
#define RTE_USART1_RX_BIT               7
#elif  (RTE_USART1_RX_ID == 3)
#define RTE_USART1_RX                   1
#define RTE_USART1_RX_PORT              GPIOB
#define RTE_USART1_RX_BIT               15
#else
#error "Invalid USART1_RX Pin Configuration!"
#endif

//   <o> USART1_CK Pin <0=>Not Used <1=>PA8
#define RTE_USART1_CK_ID                0
#if    (RTE_USART1_CK_ID == 0)
#define RTE_USART1_CK                   0
#elif  (RTE_USART1_CK_ID == 1)
#define RTE_USART1_CK                   1
#define RTE_USART1_CK_PORT              GPIOA
#define RTE_USART1_CK_BIT               8
#else
#error "Invalid USART1_CK Pin Configuration!"
#endif

//   <o> USART1_CTS Pin <0=>Not Used <1=>PA11
#define RTE_USART1_CTS_ID               0
#if    (RTE_USART1_CTS_ID == 0)
#define RTE_USART1_CTS                  0
#elif  (RTE_USART1_CTS_ID == 1)
#define RTE_USART1_CTS                  1
#define RTE_USART1_CTS_PORT             GPIOA
#define RTE_USART1_CTS_BIT              11
#else
#error "Invalid USART1_CTS Pin Configuration!"
#endif

//   <o> USART1_RTS Pin <0=>Not Used <1=>PA12
#define RTE_USART1_RTS_ID               0
#if    (RTE_USART1_RTS_ID == 0)
#define RTE_USART1_RTS                  0
#elif  (RTE_USART1_RTS_ID == 1)
#define RTE_USART1_RTS                  1
#define RTE_USART1_RTS_PORT             GPIOA
#define RTE_USART1_RTS_BIT              12
#else
#error "Invalid USART1_RTS Pin Configuration!"
#endif

//   <e> DMA Rx
//     <o1> Number <2=>2
//     <i>  Selects DMA Number (only DMA2 can be used)
//     <o2> Stream <2=>2 <5=>5
//     <i>  Selects DMA Stream (only Stream 2 or 5 can be used)
//     <o3> Channel <4=>4
//     <i>  Selects DMA Channel (only Channel 4 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_USART1_RX_DMA               0
#define RTE_USART1_RX_DMA_NUMBER        2
#define RTE_USART1_RX_DMA_STREAM        2
#define RTE_USART1_RX_DMA_CHANNEL       4
#define RTE_USART1_RX_DMA_PRIORITY      0

//   <e> DMA Tx
//     <o1> Number <2=>2
//     <i>  Selects DMA Number (only DMA2 can be used)
//     <o2> Stream <7=>7
//     <i>  Selects DMA Stream (only Stream 7 can be used)
//     <o3> Channel <4=>4
//     <i>  Selects DMA Channel (only Channel 4 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_USART1_TX_DMA               0
#define RTE_USART1_TX_DMA_NUMBER        2
#define RTE_USART1_TX_DMA_STREAM        7
#define RTE_USART1_TX_DMA_CHANNEL       4
#define RTE_USART1_TX_DMA_PRIORITY      0

// </e>


// <e> USART2 (Universal synchronous asynchronous receiver transmitter) [Driver_USART2]
// <i> Configuration settings for Driver_USART2 in component ::CMSIS Driver:USART
#define RTE_USART2                      0

//   <o> USART2_TX Pin <0=>Not Used <1=>PA2 <2=>PD5
#define RTE_USART2_TX_ID                0
#if    (RTE_USART2_TX_ID == 0)
#define RTE_USART2_TX                   0
#elif  (RTE_USART2_TX_ID == 1)
#define RTE_USART2_TX                   1
#define RTE_USART2_TX_PORT              GPIOA
#define RTE_USART2_TX_BIT               2
#elif  (RTE_USART2_TX_ID == 2)
#define RTE_USART2_TX                   1
#define RTE_USART2_TX_PORT              GPIOD
#define RTE_USART2_TX_BIT               5
#else
#error "Invalid USART2_TX Pin Configuration!"
#endif

//   <o> USART2_RX Pin <0=>Not Used <1=>PA3 <2=>PD6
#define RTE_USART2_RX_ID                0
#if    (RTE_USART2_RX_ID == 0)
#define RTE_USART2_RX                   0
#elif  (RTE_USART2_RX_ID == 1)
#define RTE_USART2_RX                   1
#define RTE_USART2_RX_PORT              GPIOA
#define RTE_USART2_RX_BIT               3
#elif  (RTE_USART2_RX_ID == 2)
#define RTE_USART2_RX                   1
#define RTE_USART2_RX_PORT              GPIOD
#define RTE_USART2_RX_BIT               6
#else
#error "Invalid USART2_RX Pin Configuration!"
#endif

//   <o> USART2_CK Pin <0=>Not Used <1=>PA4 <2=>PD7
#define RTE_USART2_CK_ID                0
#if    (RTE_USART2_CK_ID == 0)  
#define RTE_USART2_CK                   0
#elif  (RTE_USART2_CK_ID == 1)
#define RTE_USART2_CK                   1
#define RTE_USART2_CK_PORT              GPIOA
#define RTE_USART2_CK_BIT               4
#elif  (RTE_USART2_CK_ID == 2)
#define RTE_USART2_CK                   1
#define RTE_USART2_CK_PORT              GPIOD
#define RTE_USART2_CK_BIT               7
#else
#error "Invalid USART2_CK Pin Configuration!"
#endif

//   <o> USART2_CTS Pin <0=>Not Used <1=>PA0 <2=>PD3
#define RTE_USART2_CTS_ID               0
#if    (RTE_USART2_CTS_ID == 0)
#define RTE_USART2_CTS                  0
#elif  (RTE_USART2_CTS_ID == 1)
#define RTE_USART2_CTS                  1
#define RTE_USART2_CTS_PORT             GPIOA
#define RTE_USART2_CTS_BIT              0
#elif  (RTE_USART2_CTS_ID == 2)
#define RTE_USART2_CTS                  1
#define RTE_USART2_CTS_PORT             GPIOD
#define RTE_USART2_CTS_BIT              3
#else
#error "Invalid USART2_CTS Pin Configuration!"
#endif

//   <o> USART2_RTS Pin <0=>Not Used <1=>PA1 <2=>PD4
#define RTE_USART2_RTS_ID               0
#if    (RTE_USART2_RTS_ID == 0)
#define RTE_USART2_RTS                  0
#elif  (RTE_USART2_RTS_ID == 1)
#define RTE_USART2_RTS                  1
#define RTE_USART2_RTS_PORT             GPIOA
#define RTE_USART2_RTS_BIT              1
#elif  (RTE_USART2_RTS_ID == 2)
#define RTE_USART2_RTS                  1
#define RTE_USART2_RTS_PORT             GPIOD
#define RTE_USART2_RTS_BIT              4
#else
#error "Invalid USART2_RTS Pin Configuration!"
#endif

//   <e> DMA Rx
//     <o1> Number <1=>1
//     <i>  Selects DMA Number (only DMA1 can be used)
//     <o2> Stream <5=>5
//     <i>  Selects DMA Stream (only Stream 5 can be used)
//     <o3> Channel <4=>4
//     <i>  Selects DMA Channel (only Channel 4 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_USART2_RX_DMA               0
#define RTE_USART2_RX_DMA_NUMBER        1
#define RTE_USART2_RX_DMA_STREAM        5
#define RTE_USART2_RX_DMA_CHANNEL       4
#define RTE_USART2_RX_DMA_PRIORITY      0

//   <e> DMA Tx
//     <o1> Number <1=>1
//     <i>  Selects DMA Number (only DMA1 can be used)
//     <o2> Stream <6=>6
//     <i>  Selects DMA Stream (only Stream 6 can be used)
//     <o3> Channel <4=>4
//     <i>  Selects DMA Channel (only Channel 4 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_USART2_TX_DMA               0
#define RTE_USART2_TX_DMA_NUMBER        1
#define RTE_USART2_TX_DMA_STREAM        6
#define RTE_USART2_TX_DMA_CHANNEL       4
#define RTE_USART2_TX_DMA_PRIORITY      0

// </e>


// <e> USART3 (Universal synchronous asynchronous receiver transmitter) [Driver_USART3]
// <i> Configuration settings for Driver_USART3 in component ::CMSIS Driver:USART
#define RTE_USART3                      0

//   <o> USART3_TX Pin <0=>Not Used <1=>PB10 <2=>PC10 <3=>PD8
#define RTE_USART3_TX_ID                0
#if    (RTE_USART3_TX_ID == 0)
#define RTE_USART3_TX                   0
#elif  (RTE_USART3_TX_ID == 1)
#define RTE_USART3_TX                   1
#define RTE_USART3_TX_PORT              GPIOB
#define RTE_USART3_TX_BIT               10
#elif  (RTE_USART3_TX_ID == 2)
#define RTE_USART3_TX                   1
#define RTE_USART3_TX_PORT              GPIOC
#define RTE_USART3_TX_BIT               10
#elif  (RTE_USART3_TX_ID == 3)
#define RTE_USART3_TX                   1
#define RTE_USART3_TX_PORT              GPIOD
#define RTE_USART3_TX_BIT               8
#else
#error "Invalid USART3_TX Pin Configuration!"
#endif

//   <o> USART3_RX Pin <0=>Not Used <1=>PB11 <2=>PC11 <3=>PD9
#define RTE_USART3_RX_ID                0
#if    (RTE_USART3_RX_ID == 0)
#define RTE_USART3_RX                   0
#elif  (RTE_USART3_RX_ID == 1)
#define RTE_USART3_RX                   1
#define RTE_USART3_RX_PORT              GPIOB
#define RTE_USART3_RX_BIT               11
#elif  (RTE_USART3_RX_ID == 2)
#define RTE_USART3_RX                   1
#define RTE_USART3_RX_PORT              GPIOC
#define RTE_USART3_RX_BIT               11
#elif  (RTE_USART3_RX_ID == 3)
#define RTE_USART3_RX                   1
#define RTE_USART3_RX_PORT              GPIOD
#define RTE_USART3_RX_BIT               9
#else
#error "Invalid USART3_RX Pin Configuration!"
#endif

//   <o> USART3_CK Pin <0=>Not Used <1=>PB12 <2=>PC12 <3=>PD10
#define RTE_USART3_CK_ID                0
#if    (RTE_USART3_CK_ID == 0)
#define RTE_USART3_CK                   0
#elif  (RTE_USART3_CK_ID == 1)
#define RTE_USART3_CK                   1
#define RTE_USART3_CK_PORT              GPIOB
#define RTE_USART3_CK_BIT               12
#elif  (RTE_USART3_CK_ID == 2)
#define RTE_USART3_CK                   1
#define RTE_USART3_CK_PORT              GPIOC
#define RTE_USART3_CK_BIT               12
#elif  (RTE_USART3_CK_ID == 3)
#define RTE_USART3_CK                   1
#define RTE_USART3_CK_PORT              GPIOD
#define RTE_USART3_CK_BIT               10
#else
#error "Invalid USART3_CK Pin Configuration!"
#endif

//   <o> USART3_CTS Pin <0=>Not Used <1=>PB13 <2=>PD11
#define RTE_USART3_CTS_ID               0
#if    (RTE_USART3_CTS_ID == 0)
#define RTE_USART3_CTS                  0
#elif  (RTE_USART3_CTS_ID == 1)
#define RTE_USART3_CTS                  1
#define RTE_USART3_CTS_PORT             GPIOB
#define RTE_USART3_CTS_BIT              13
#elif  (RTE_USART3_CTS_ID == 2)
#define RTE_USART3_CTS                  1
#define RTE_USART3_CTS_PORT             GPIOD
#define RTE_USART3_CTS_BIT              11
#else
#error "Invalid USART3_CTS Pin Configuration!"
#endif

//   <o> USART3_RTS Pin <0=>Not Used <1=>PB14 <2=>PD12
#define RTE_USART3_RTS_ID               0
#if    (RTE_USART3_RTS_ID == 0)
#define RTE_USART3_RTS                  0
#elif  (RTE_USART3_RTS_ID == 1)
#define RTE_USART3_RTS                  1
#define RTE_USART3_RTS_PORT             GPIOB
#define RTE_USART3_RTS_BIT              14
#elif  (RTE_USART3_RTS_ID == 2)
#define RTE_USART3_RTS                  1
#define RTE_USART3_RTS_PORT             GPIOD
#define RTE_USART3_RTS_BIT              12
#else
#error "Invalid USART3_RTS Pin Configuration!"
#endif

//   <e> DMA Rx
//     <o1> Number <1=>1
//     <i>  Selects DMA Number (only DMA1 can be used)
//     <o2> Stream <1=>1
//     <i>  Selects DMA Stream (only Stream 1 can be used)
//     <o3> Channel <4=>4
//     <i>  Selects DMA Channel (only Channel 4 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_USART3_RX_DMA               0
#define RTE_USART3_RX_DMA_NUMBER        1
#define RTE_USART3_RX_DMA_STREAM        1
#define RTE_USART3_RX_DMA_CHANNEL       4
#define RTE_USART3_RX_DMA_PRIORITY      0

//   <e> DMA Tx
//     <o1> Number <1=>1
//     <i>  Selects DMA Number (only DMA1 can be used)
//     <o2> Stream <3=>3 <4=>4 
//     <i>  Selects DMA Stream (only Stream 3 or 4 can be used)
//     <o3> Channel <4=>4 <7=>7
//     <i>  Selects DMA Channel (only Channel 4 or 7 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_USART3_TX_DMA               0
#define RTE_USART3_TX_DMA_NUMBER        1
#define RTE_USART3_TX_DMA_STREAM        3
#define RTE_USART3_TX_DMA_CHANNEL       4
#define RTE_USART3_TX_DMA_PRIORITY      0

// </e>


// <e> UART4 (Universal asynchronous receiver transmitter) [Driver_USART4]
// <i> Configuration settings for Driver_USART4 in component ::CMSIS Driver:USART
#define RTE_UART4                       0

//   <o> UART4_TX Pin <0=>Not Used <1=>PA0 <2=>PC10 <3=>PA12 <4=>PD1 <5=>PH13
#define RTE_UART4_TX_ID                 0
#if    (RTE_UART4_TX_ID == 0)
#define RTE_UART4_TX                    0
#elif  (RTE_UART4_TX_ID == 1)
#define RTE_UART4_TX                    1
#define RTE_UART4_TX_PORT               GPIOA
#define RTE_UART4_TX_BIT                0
#elif  (RTE_UART4_TX_ID == 2)
#define RTE_UART4_TX                    1
#define RTE_UART4_TX_PORT               GPIOC
#define RTE_UART4_TX_BIT                10
#elif  (RTE_UART4_TX_ID == 3)
#define RTE_UART4_TX                    1
#define RTE_UART4_TX_PORT               GPIOA
#define RTE_UART4_TX_BIT                12
#elif  (RTE_UART4_TX_ID == 4)
#define RTE_UART4_TX                    1
#define RTE_UART4_TX_PORT               GPIOD
#define RTE_UART4_TX_BIT                1
#elif  (RTE_UART4_TX_ID == 5)
#define RTE_UART4_TX                    1
#define RTE_UART4_TX_PORT               GPIOH
#define RTE_UART4_TX_BIT                13
#else
#error "Invalid UART4_TX Pin Configuration!"
#endif

//   <o> UART4_RX Pin <0=>Not Used <1=>PA1 <2=>PC11 <3=>PA11 <4=>PD0 <5=>PH14 <6=>PI9
#define RTE_UART4_RX_ID                 0
#if    (RTE_UART4_RX_ID == 0)
#define RTE_UART4_RX                    0
#elif  (RTE_UART4_RX_ID == 1)
#define RTE_UART4_RX                    1
#define RTE_UART4_RX_PORT               GPIOA
#define RTE_UART4_RX_BIT                1
#elif  (RTE_UART4_RX_ID == 2)
#define RTE_UART4_RX                    1
#define RTE_UART4_RX_PORT               GPIOC
#define RTE_UART4_RX_BIT                11
#elif  (RTE_UART4_RX_ID == 3)
#define RTE_UART4_RX                    1
#define RTE_UART4_RX_PORT               GPIOA
#define RTE_UART4_RX_BIT                11
#elif  (RTE_UART4_RX_ID == 4)
#define RTE_UART4_RX                    1
#define RTE_UART4_RX_PORT               GPIOD
#define RTE_UART4_RX_BIT                0
#elif  (RTE_UART4_RX_ID == 5)
#define RTE_UART4_RX                    1
#define RTE_UART4_RX_PORT               GPIOH
#define RTE_UART4_RX_BIT                14
#elif  (RTE_UART4_RX_ID == 6)
#define RTE_UART4_RX                    1
#define RTE_UART4_RX_PORT               GPIOI
#define RTE_UART4_RX_BIT                9
#else
#error "Invalid UART4_RX Pin Configuration!"
#endif

//   <o> UART4_CTS Pin <0=>Not Used <1=>PB0 <2=>PB15
#define RTE_UART4_CTS_ID                0
#if    (RTE_UART4_CTS_ID == 0)
#define RTE_UART4_CTS                   0
#elif  (RTE_UART4_CTS_ID == 1)
#define RTE_UART4_CTS                   1
#define RTE_UART4_CTS_PORT              GPIOB
#define RTE_UART4_CTS_BIT               0
#elif  (RTE_UART4_CTS_ID == 2)
#define RTE_UART4_CTS                   1
#define RTE_UART4_CTS_PORT              GPIOB
#define RTE_UART4_CTS_BIT               15
#else
#error "Invalid UART4_CTS Pin Configuration!"
#endif

//   <o> UART4_RTS Pin <0=>Not Used <1=>PA15 <2=>PB14
#define RTE_UART4_RTS_ID                0
#if    (RTE_UART4_RTS_ID == 0)
#define RTE_UART4_RTS                   0
#elif  (RTE_UART4_RTS_ID == 1)
#define RTE_UART4_RTS                   1
#define RTE_UART4_RTS_PORT              GPIOA
#define RTE_UART4_RTS_BIT               15
#elif  (RTE_UART4_RTS_ID == 2)
#define RTE_UART4_RTS                   1
#define RTE_UART4_RTS_PORT              GPIOB
#define RTE_UART4_RTS_BIT               14
#else
#error "Invalid UART4_RTS Pin Configuration!"
#endif

//   <e> DMA Rx
//     <o1> Number <1=>1
//     <i>  Selects DMA Number (only DMA1 can be used)
//     <o2> Stream <2=>2
//     <i>  Selects DMA Stream (only Stream 2 can be used)
//     <o3> Channel <4=>4
//     <i>  Selects DMA Channel (only Channel 4 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_UART4_RX_DMA                0
#define RTE_UART4_RX_DMA_NUMBER         1
#define RTE_UART4_RX_DMA_STREAM         2
#define RTE_UART4_RX_DMA_CHANNEL        4
#define RTE_UART4_RX_DMA_PRIORITY       0

//   <e> DMA Tx
//     <o1> Number <1=>1
//     <i>  Selects DMA Number (only DMA1 can be used)
//     <o2> Stream <4=>4
//     <i>  Selects DMA Stream (only Stream 4 can be used)
//     <o3> Channel <4=>4
//     <i>  Selects DMA Channel (only Channel 4 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_UART4_TX_DMA                0
#define RTE_UART4_TX_DMA_NUMBER         1
#define RTE_UART4_TX_DMA_STREAM         4
#define RTE_UART4_TX_DMA_CHANNEL        4
#define RTE_UART4_TX_DMA_PRIORITY       0

// </e>


// <e> UART5 (Universal asynchronous receiver transmitter) [Driver_USART5]
// <i> Configuration settings for Driver_USART5 in component ::CMSIS Driver:USART
#define RTE_UART5                       0

//   <o> UART5_TX Pin <0=>Not Used <1=>PC12 <2=>PB6 <3=>PB9 <4=>PB13
#define RTE_UART5_TX_ID                 0
#if    (RTE_UART5_TX_ID == 0)
#define RTE_UART5_TX                    0
#elif  (RTE_UART5_TX_ID == 1)
#define RTE_UART5_TX                    1
#define RTE_UART5_TX_PORT               GPIOC
#define RTE_UART5_TX_BIT                12
#elif  (RTE_UART5_TX_ID == 2)
#define RTE_UART5_TX                    1
#define RTE_UART5_TX_PORT               GPIOB
#define RTE_UART5_TX_BIT                6
#elif  (RTE_UART5_TX_ID == 3)
#define RTE_UART5_TX                    1
#define RTE_UART5_TX_PORT               GPIOB
#define RTE_UART5_TX_BIT                9
#elif  (RTE_UART5_TX_ID == 4)
#define RTE_UART5_TX                    1
#define RTE_UART5_TX_PORT               GPIOB
#define RTE_UART5_TX_BIT                13
#else
#error "Invalid UART5_TX Pin Configuration!"
#endif

//   <o> UART5_RX Pin <0=>Not Used <1=>PD2 <2=>PB5 <3=>PB8 <4=>PB12
#define RTE_UART5_RX_ID                 0
#if    (RTE_UART5_RX_ID == 0)
#define RTE_UART5_RX                    0
#elif  (RTE_UART5_RX_ID == 1)
#define RTE_UART5_RX                    1
#define RTE_UART5_RX_PORT               GPIOD
#define RTE_UART5_RX_BIT                2
#elif  (RTE_UART5_RX_ID == 2)
#define RTE_UART5_RX                    1
#define RTE_UART5_RX_PORT               GPIOB
#define RTE_UART5_RX_BIT                5
#elif  (RTE_UART5_RX_ID == 3)
#define RTE_UART5_RX                    1
#define RTE_UART5_RX_PORT               GPIOB
#define RTE_UART5_RX_BIT                8
#elif  (RTE_UART5_RX_ID == 4)
#define RTE_UART5_RX                    1
#define RTE_UART5_RX_PORT               GPIOB
#define RTE_UART5_RX_BIT                12
#else
#error "Invalid UART5_RX Pin Configuration!"
#endif

//   <o> UART5_CTS Pin <0=>Not Used <1=>PC9
#define RTE_UART5_CTS_ID                0
#if    (RTE_UART5_CTS_ID == 0)
#define RTE_UART5_CTS                   0
#elif  (RTE_UART5_CTS_ID == 1)
#define RTE_UART5_CTS                   1
#define RTE_UART5_CTS_PORT              GPIOC
#define RTE_UART5_CTS_BIT               9
#else
#error "Invalid UART5_CTS Pin Configuration!"
#endif

//   <o> UART5_RTS Pin <0=>Not Used <1=>PC8
#define RTE_UART5_RTS_ID                0
#if    (RTE_UART5_RTS_ID == 0)
#define RTE_UART5_RTS                   0
#elif  (RTE_UART5_RTS_ID == 1)
#define RTE_UART5_RTS                   1
#define RTE_UART5_RTS_PORT              GPIOC
#define RTE_UART5_RTS_BIT               8
#else
#error "Invalid UART5_RTS Pin Configuration!"
#endif

//   <e> DMA Rx
//     <o1> Number <1=>1
//     <i>  Selects DMA Number (only DMA1 can be used)
//     <o2> Stream <0=>0
//     <i>  Selects DMA Stream (only Stream 0 can be used)
//     <o3> Channel <4=>4
//     <i>  Selects DMA Channel (only Channel 4 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_UART5_RX_DMA                0
#define RTE_UART5_RX_DMA_NUMBER         1
#define RTE_UART5_RX_DMA_STREAM         0
#define RTE_UART5_RX_DMA_CHANNEL        4
#define RTE_UART5_RX_DMA_PRIORITY       0

//   <e> DMA Tx
//     <o1> Number <1=>1
//     <i>  Selects DMA Number (only DMA1 can be used)
//     <o2> Stream <7=>7
//     <i>  Selects DMA Stream (only Stream 7 can be used)
//     <o3> Channel <4=>4
//     <i>  Selects DMA Channel (only Channel 4 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_UART5_TX_DMA                0
#define RTE_UART5_TX_DMA_NUMBER         1
#define RTE_UART5_TX_DMA_STREAM         7
#define RTE_UART5_TX_DMA_CHANNEL        4
#define RTE_UART5_TX_DMA_PRIORITY       0

// </e>


// <e> USART6 (Universal synchronous asynchronous receiver transmitter) [Driver_USART6]
// <i> Configuration settings for Driver_USART6 in component ::CMSIS Driver:USART
#define RTE_USART6                      0

//   <o> USART6_TX Pin <0=>Not Used <1=>PC6 <2=>PG14
#define RTE_USART6_TX_ID                0
#if    (RTE_USART6_TX_ID == 0)
#define RTE_USART6_TX                   0
#elif  (RTE_USART6_TX_ID == 1)
#define RTE_USART6_TX                   1
#define RTE_USART6_TX_PORT              GPIOC
#define RTE_USART6_TX_BIT               6
#elif  (RTE_USART6_TX_ID == 2)
#define RTE_USART6_TX                   1
#define RTE_USART6_TX_PORT              GPIOG
#define RTE_USART6_TX_BIT               14
#else
#error "Invalid USART6_TX Pin Configuration!"
#endif

//   <o> USART6_RX Pin <0=>Not Used <1=>PC7 <2=>PG9
#define RTE_USART6_RX_ID                0
#if    (RTE_USART6_RX_ID == 0)
#define RTE_USART6_RX                   0
#elif  (RTE_USART6_RX_ID == 1)
#define RTE_USART6_RX                   1
#define RTE_USART6_RX_PORT              GPIOC
#define RTE_USART6_RX_BIT               7
#elif  (RTE_USART6_RX_ID == 2)
#define RTE_USART6_RX                   1
#define RTE_USART6_RX_PORT              GPIOG
#define RTE_USART6_RX_BIT               9
#else
#error "Invalid USART6_RX Pin Configuration!"
#endif

//   <o> USART6_CK Pin <0=>Not Used <1=>PC8 <2=>PG7
#define RTE_USART6_CK_ID                0
#if    (RTE_USART6_CK_ID == 0)
#define RTE_USART6_CK                   0
#elif  (RTE_USART6_CK_ID == 1)
#define RTE_USART6_CK                   1
#define RTE_USART6_CK_PORT              GPIOC
#define RTE_USART6_CK_BIT               8
#elif  (RTE_USART6_CK_ID == 2)
#define RTE_USART6_CK                   1
#define RTE_USART6_CK_PORT              GPIOG
#define RTE_USART6_CK_BIT               7
#else
#error "Invalid USART6_CK Pin Configuration!"
#endif

//   <o> USART6_CTS Pin <0=>Not Used <1=>PG13 <2=>PG15
#define RTE_USART6_CTS_ID               0
#if    (RTE_USART6_CTS_ID == 0)
#define RTE_USART6_CTS                  0
#elif  (RTE_USART6_CTS_ID == 1)
#define RTE_USART6_CTS                  1
#define RTE_USART6_CTS_PORT             GPIOG
#define RTE_USART6_CTS_BIT              13
#elif  (RTE_USART6_CTS_ID == 2)
#define RTE_USART6_CTS                  1
#define RTE_USART6_CTS_PORT             GPIOG
#define RTE_USART6_CTS_BIT              15
#else
#error "Invalid USART6_CTS Pin Configuration!"
#endif

//   <o> USART6_RTS Pin <0=>Not Used <1=>PG8 <2=>PG12
#define RTE_USART6_RTS_ID               0
#if    (RTE_USART6_RTS_ID == 0)
#define RTE_USART6_RTS                  0
#elif  (RTE_USART6_RTS_ID == 1)
#define RTE_USART6_RTS                  1
#define RTE_USART6_RTS_PORT             GPIOG
#define RTE_USART6_RTS_BIT              8
#elif  (RTE_USART6_RTS_ID == 2)
#define RTE_USART6_RTS                  1
#define RTE_USART6_RTS_PORT             GPIOG
#define RTE_USART6_RTS_BIT              12
#else
#error "Invalid USART6_RTS Pin Configuration!"
#endif

//   <e> DMA Rx
//     <o1> Number <2=>2
//     <i>  Selects DMA Number (only DMA2 can be used)
//     <o2> Stream <1=>1 <2=>2
//     <i>  Selects DMA Stream (only Stream 1 or 2 can be used)
//     <o3> Channel <5=>5
//     <i>  Selects DMA Channel (only Channel 5 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_USART6_RX_DMA               0
#define RTE_USART6_RX_DMA_NUMBER        2
#define RTE_USART6_RX_DMA_STREAM        1
#define RTE_USART6_RX_DMA_CHANNEL       5
#define RTE_USART6_RX_DMA_PRIORITY      0

//   <e> DMA Tx
//     <o1> Number <2=>2
//     <i>  Selects DMA Number (only DMA2 can be used)
//     <o2> Stream <6=>6 <7=>7
//     <i>  Selects DMA Stream (only Stream 6 or 7 can be used)
//     <o3> Channel <5=>5
//     <i>  Selects DMA Channel (only Channel 5 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_USART6_TX_DMA               0
#define RTE_USART6_TX_DMA_NUMBER        2
#define RTE_USART6_TX_DMA_STREAM        6
#define RTE_USART6_TX_DMA_CHANNEL       5
#define RTE_USART6_TX_DMA_PRIORITY      0

// </e>

// <e> UART7 (Universal asynchronous receiver transmitter) [Driver_USART7]
// <i> Configuration settings for Driver_USART7 in component ::CMSIS Driver:USART
#define RTE_UART7                       0

//   <o> UART7_TX Pin <0=>Not Used <1=>PF7 <2=>PE8 <3=>PA15 <4=>PB4
#define RTE_UART7_TX_ID                 0
#if    (RTE_UART7_TX_ID == 0)
#define RTE_UART7_TX                    0
#elif  (RTE_UART7_TX_ID == 1)
#define RTE_UART7_TX                    1
#define RTE_UART7_TX_PORT               GPIOF
#define RTE_UART7_TX_BIT                7
#elif  (RTE_UART7_TX_ID == 2)
#define RTE_UART7_TX                    1
#define RTE_UART7_TX_PORT               GPIOE
#define RTE_UART7_TX_BIT                8
#elif  (RTE_UART7_TX_ID == 3)
#define RTE_UART7_TX                    1
#define RTE_UART7_TX_PORT               GPIOA
#define RTE_UART7_TX_BIT                15
#elif  (RTE_UART7_TX_ID == 4)
#define RTE_UART7_TX                    1
#define RTE_UART7_TX_PORT               GPIOB
#define RTE_UART7_TX_BIT                4
#else
#error "Invalid UART7_TX Pin Configuration!"
#endif

//   <o> UART7_RX Pin <0=>Not Used <1=>PF6 <2=>PE7 <3=>PA8 <4=>PB3
#define RTE_UART7_RX_ID                 0
#if    (RTE_UART7_RX_ID == 0)
#define RTE_UART7_RX                    0
#elif  (RTE_UART7_RX_ID == 1)
#define RTE_UART7_RX                    1
#define RTE_UART7_RX_PORT               GPIOF
#define RTE_UART7_RX_BIT                6
#elif  (RTE_UART7_RX_ID == 2)
#define RTE_UART7_RX                    1
#define RTE_UART7_RX_PORT               GPIOE
#define RTE_UART7_RX_BIT                7
#elif  (RTE_UART7_RX_ID == 3)
#define RTE_UART7_RX                    1
#define RTE_UART7_RX_PORT               GPIOA
#define RTE_UART7_RX_BIT                8
#elif  (RTE_UART7_RX_ID == 4)
#define RTE_UART7_RX                    1
#define RTE_UART7_RX_PORT               GPIOB
#define RTE_UART7_RX_BIT                3
#else
#error "Invalid UART7_RX Pin Configuration!"
#endif

//   <o> UART7_CTS Pin <0=>Not Used <1=>PF9 <2=>PE10
#define RTE_UART7_CTS_ID                0
#if    (RTE_UART7_CTS_ID == 0)
#define RTE_UART7_CTS                   0
#elif  (RTE_UART7_CTS_ID == 1)
#define RTE_UART7_CTS                   1
#define RTE_UART7_CTS_PORT              GPIOF
#define RTE_UART7_CTS_BIT               9
#elif  (RTE_UART7_CTS_ID == 2)
#define RTE_UART7_CTS                   1
#define RTE_UART7_CTS_PORT              GPIOE
#define RTE_UART7_CTS_BIT               10
#else
#error "Invalid UART7_CTS Pin Configuration!"
#endif

//   <o> UART7_RTS Pin <0=>Not Used <1=>PF8 <2=>PE9
#define RTE_UART7_RTS_ID                0
#if    (RTE_UART7_RTS_ID == 0)
#define RTE_UART7_RTS                   0
#elif  (RTE_UART7_RTS_ID == 1)
#define RTE_UART7_RTS                   1
#define RTE_UART7_RTS_PORT              GPIOF
#define RTE_UART7_RTS_BIT               8
#elif  (RTE_UART7_RTS_ID == 2)
#define RTE_UART7_RTS                   1
#define RTE_UART7_RTS_PORT              GPIOE
#define RTE_UART7_RTS_BIT               9
#else
#error "Invalid UART7_RTS Pin Configuration!"
#endif

//   <e> DMA Rx
//     <o1> Number <1=>1
//     <i>  Selects DMA Number (only DMA1 can be used)
//     <o2> Stream <3=>3
//     <i>  Selects DMA Stream (only Stream 3 can be used)
//     <o3> Channel <5=>5
//     <i>  Selects DMA Channel (only Channel 5 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_UART7_RX_DMA                0
#define RTE_UART7_RX_DMA_NUMBER         1
#define RTE_UART7_RX_DMA_STREAM         3
#define RTE_UART7_RX_DMA_CHANNEL        5
#define RTE_UART7_RX_DMA_PRIORITY       0

//   <e> DMA Tx
//     <o1> Number <1=>1
//     <i>  Selects DMA Number (only DMA1 can be used)
//     <o2> Stream <1=>1
//     <i>  Selects DMA Stream (only Stream 1 can be used)
//     <o3> Channel <5=>5
//     <i>  Selects DMA Channel (only Channel 5 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_UART7_TX_DMA                0
#define RTE_UART7_TX_DMA_NUMBER         1
#define RTE_UART7_TX_DMA_STREAM         1
#define RTE_UART7_TX_DMA_CHANNEL        5
#define RTE_UART7_TX_DMA_PRIORITY       0

// </e>

// <e> UART8 (Universal asynchronous receiver transmitter) [Driver_USART8]
// <i> Configuration settings for Driver_USART8 in component ::CMSIS Driver:USART
#define RTE_UART8                       0

//   <o> UART8_TX Pin <0=>Not Used <1=>PE1
#define RTE_UART8_TX_ID                 0
#if    (RTE_UART8_TX_ID == 0)
#define RTE_UART8_TX                    0
#elif  (RTE_UART8_TX_ID == 1)
#define RTE_UART8_TX                    1
#define RTE_UART8_TX_PORT               GPIOE
#define RTE_UART8_TX_BIT                1
#else
#error "Invalid UART8_TX Pin Configuration!"
#endif

//   <o> UART8_RX Pin <0=>Not Used <1=>PE0
#define RTE_UART8_RX_ID                 0
#if    (RTE_UART8_RX_ID == 0)
#define RTE_UART8_RX                    0
#elif  (RTE_UART8_RX_ID == 1)
#define RTE_UART8_RX                    1
#define RTE_UART8_RX_PORT               GPIOE
#define RTE_UART8_RX_BIT                0
#else
#error "Invalid UART8_RX Pin Configuration!"
#endif

//   <o> UART8_CTS Pin <0=>Not Used <1=>PD14
#define RTE_UART8_CTS_ID                0
#if    (RTE_UART8_CTS_ID == 0)
#define RTE_UART8_CTS                   0
#elif  (RTE_UART8_CTS_ID == 1)
#define RTE_UART8_CTS                   1
#define RTE_UART8_CTS_PORT              GPIOD
#define RTE_UART8_CTS_BIT               14
#else
#error "Invalid UART8_CTS Pin Configuration!"
#endif

//   <o> UART8_RTS Pin <0=>Not Used <1=>PD15
#define RTE_UART8_RTS_ID                0
#if    (RTE_UART8_RTS_ID == 0)
#define RTE_UART8_RTS                   0
#elif  (RTE_UART8_RTS_ID == 1)
#define RTE_UART8_RTS                   1
#define RTE_UART8_RTS_PORT              GPIOD
#define RTE_UART8_RTS_BIT               15
#else
#error "Invalid UART8_RTS Pin Configuration!"
#endif

//   <e> DMA Rx
//     <o1> Number <1=>1
//     <i>  Selects DMA Number (only DMA1 can be used)
//     <o2> Stream <6=>6
//     <i>  Selects DMA Stream (only Stream 6 can be used)
//     <o3> Channel <5=>5
//     <i>  Selects DMA Channel (only Channel 5 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_UART8_RX_DMA                0
#define RTE_UART8_RX_DMA_NUMBER         1
#define RTE_UART8_RX_DMA_STREAM         6
#define RTE_UART8_RX_DMA_CHANNEL        5
#define RTE_UART8_RX_DMA_PRIORITY       0

//   <e> DMA Tx
//     <o1> Number <1=>1
//     <i>  Selects DMA Number (only DMA1 can be used)
//     <o2> Stream <0=>0
//     <i>  Selects DMA Stream (only Stream 0 can be used)
//     <o3> Channel <5=>5
//     <i>  Selects DMA Channel (only Channel 5 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_UART8_TX_DMA                0
#define RTE_UART8_TX_DMA_NUMBER         1
#define RTE_UART8_TX_DMA_STREAM         0
#define RTE_UART8_TX_DMA_CHANNEL        5
#define RTE_UART8_TX_DMA_PRIORITY       0

// </e>


// <e> I2C1 (Inter-integrated Circuit Interface 1) [Driver_I2C1]
// <i> Configuration settings for Driver_I2C1 in component ::CMSIS Driver:I2C
#define RTE_I2C1                        0

//   <o> I2C1_SCL Pin <0=>PB6 <1=>PB8
#define RTE_I2C1_SCL_PORT_ID            0
#if    (RTE_I2C1_SCL_PORT_ID == 0)
#define RTE_I2C1_SCL_PORT               GPIOB
#define RTE_I2C1_SCL_BIT                6
#elif  (RTE_I2C1_SCL_PORT_ID == 1)
#define RTE_I2C1_SCL_PORT               GPIOB
#define RTE_I2C1_SCL_BIT                8
#else
#error "Invalid I2C1_SCL Pin Configuration!"
#endif

//   <o> I2C1_SDA Pin <0=>PB7 <1=>PB9
#define RTE_I2C1_SDA_PORT_ID            0
#if    (RTE_I2C1_SDA_PORT_ID == 0)
#define RTE_I2C1_SDA_PORT               GPIOB
#define RTE_I2C1_SDA_BIT                7
#elif  (RTE_I2C1_SDA_PORT_ID == 1)
#define RTE_I2C1_SDA_PORT               GPIOB
#define RTE_I2C1_SDA_BIT                9
#else
#error "Invalid I2C1_SDA Pin Configuration!"
#endif

//   <e> DMA Rx
//     <o1> Number <1=>1
//     <i>  Selects DMA Number (only DMA1 can be used)
//     <o2> Stream <0=>0 <5=>5
//     <i>  Selects DMA Stream (only Stream 0 or 5 can be used)
//     <o3> Channel <1=>1
//     <i>  Selects DMA Channel (only Channel 1 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_I2C1_RX_DMA                 0
#define RTE_I2C1_RX_DMA_NUMBER          1
#define RTE_I2C1_RX_DMA_STREAM          0
#define RTE_I2C1_RX_DMA_CHANNEL         1
#define RTE_I2C1_RX_DMA_PRIORITY        0

//   <e> DMA Tx
//     <o1> Number <1=>1
//     <i>  Selects DMA Number (only DMA1 can be used)
//     <o2> Stream <6=>6 <7=>7
//     <i>  Selects DMA Stream (only Stream 6 or 7 can be used)
//     <o3> Channel <1=>1
//     <i>  Selects DMA Channel (only Channel 1 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_I2C1_TX_DMA                 0
#define RTE_I2C1_TX_DMA_NUMBER          1
#define RTE_I2C1_TX_DMA_STREAM          6
#define RTE_I2C1_TX_DMA_CHANNEL         1
#define RTE_I2C1_TX_DMA_PRIORITY        0

// </e>


// <e> I2C2 (Inter-integrated Circuit Interface 2) [Driver_I2C2]
// <i> Configuration settings for Driver_I2C2 in component ::CMSIS Driver:I2C
#define RTE_I2C2                        0

//   <o> I2C2_SCL Pin <0=>PF1 <1=>PH4 <2=>PB10
#define RTE_I2C2_SCL_PORT_ID            0
#if    (RTE_I2C2_SCL_PORT_ID == 0)
#define RTE_I2C2_SCL_PORT               GPIOF
#define RTE_I2C2_SCL_BIT                1
#elif  (RTE_I2C2_SCL_PORT_ID == 1)
#define RTE_I2C2_SCL_PORT               GPIOH
#define RTE_I2C2_SCL_BIT                4
#elif  (RTE_I2C2_SCL_PORT_ID == 2)
#define RTE_I2C2_SCL_PORT               GPIOB
#define RTE_I2C2_SCL_BIT                10
#else
#error "Invalid I2C2_SCL Pin Configuration!"
#endif

//   <o> I2C2_SDA Pin <0=>PF0 <1=>PH5 <2=>PB11
#define RTE_I2C2_SDA_PORT_ID            0
#if    (RTE_I2C2_SDA_PORT_ID == 0)
#define RTE_I2C2_SDA_PORT               GPIOF
#define RTE_I2C2_SDA_BIT                0
#elif  (RTE_I2C2_SDA_PORT_ID == 1)
#define RTE_I2C2_SDA_PORT               GPIOH
#define RTE_I2C2_SDA_BIT                5
#elif  (RTE_I2C2_SDA_PORT_ID == 2)
#define RTE_I2C2_SDA_PORT               GPIOB
#define RTE_I2C2_SDA_BIT                11
#else
#error "Invalid I2C2_SDA Pin Configuration!"
#endif

//   <e> DMA Rx
//     <o1> Number <1=>1
//     <i>  Selects DMA Number (only DMA1 can be used)
//     <o2> Stream <2=>2 <3=>3
//     <i>  Selects DMA Stream (only Stream 2 or 3 can be used)
//     <o3> Channel <7=>7
//     <i>  Selects DMA Channel (only Channel 7 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_I2C2_RX_DMA                 0
#define RTE_I2C2_RX_DMA_NUMBER          1
#define RTE_I2C2_RX_DMA_STREAM          2
#define RTE_I2C2_RX_DMA_CHANNEL         7
#define RTE_I2C2_RX_DMA_PRIORITY        0

//   <e> DMA Tx
//     <o1> Number <1=>1
//     <i>  Selects DMA Number (only DMA1 can be used)
//     <o2> Stream <7=>7
//     <i>  Selects DMA Stream (only Stream 7 can be used)
//     <o3> Channel <7=>7
//     <i>  Selects DMA Channel (only Channel 1 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_I2C2_TX_DMA                 0
#define RTE_I2C2_TX_DMA_NUMBER          1
#define RTE_I2C2_TX_DMA_STREAM          7
#define RTE_I2C2_TX_DMA_CHANNEL         7
#define RTE_I2C2_TX_DMA_PRIORITY        0

// </e>


// <e> I2C3 (Inter-integrated Circuit Interface 3) [Driver_I2C3]
// <i> Configuration settings for Driver_I2C3 in component ::CMSIS Driver:I2C
#define RTE_I2C3                        0

//   <o> I2C3_SCL Pin <0=>PH7 <1=>PA8
#define RTE_I2C3_SCL_PORT_ID            0
#if    (RTE_I2C3_SCL_PORT_ID == 0)
#define RTE_I2C3_SCL_PORT               GPIOH
#define RTE_I2C3_SCL_BIT                7
#elif  (RTE_I2C3_SCL_PORT_ID == 1)
#define RTE_I2C3_SCL_PORT               GPIOA
#define RTE_I2C3_SCL_BIT                8
#else
#error "Invalid I2C3_SCL Pin Configuration!"
#endif

//   <o> I2C3_SDA Pin <0=>PH8 <1=>PC9
#define RTE_I2C3_SDA_PORT_ID            0
#if    (RTE_I2C3_SDA_PORT_ID == 0)
#define RTE_I2C3_SDA_PORT               GPIOH
#define RTE_I2C3_SDA_BIT                8
#elif  (RTE_I2C3_SDA_PORT_ID == 1)
#define RTE_I2C3_SDA_PORT               GPIOC
#define RTE_I2C3_SDA_BIT                9
#else
#error "Invalid I2C3_SDA Pin Configuration!"
#endif

//   <e> DMA Rx
//     <o1> Number <1=>1
//     <i>  Selects DMA Number (only DMA1 can be used)
//     <o2> Stream <1=>1 <2=>2
//     <i>  Selects DMA Stream (only Stream 1 or 2 can be used)
//     <o3> Channel <1=>1 <3=>3
//     <i>  Selects DMA Channel (only Channel 1 or 3 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_I2C3_RX_DMA                 0
#define RTE_I2C3_RX_DMA_NUMBER          1
#define RTE_I2C3_RX_DMA_STREAM          2
#define RTE_I2C3_RX_DMA_CHANNEL         3
#define RTE_I2C3_RX_DMA_PRIORITY        0

//   <e> DMA Tx
//     <o1> Number <1=>1
//     <i>  Selects DMA Number (only DMA1 can be used)
//     <o2> Stream <4=>4
//     <i>  Selects DMA Stream (only Stream 4 can be used)
//     <o3> Channel <3=>3
//     <i>  Selects DMA Channel (only Channel 3 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_I2C3_TX_DMA                 0
#define RTE_I2C3_TX_DMA_NUMBER          1
#define RTE_I2C3_TX_DMA_STREAM          4
#define RTE_I2C3_TX_DMA_CHANNEL         3
#define RTE_I2C3_TX_DMA_PRIORITY        0

// </e>


// <e> I2C4 (Inter-integrated Circuit Interface 4) [Driver_I2C4]
// <i> Configuration settings for Driver_I2C4 in component ::CMSIS Driver:I2C
#define RTE_I2C4                        0

//   <o> I2C4_SCL Pin <0=>PD12 <1=>PF14 <2=>PH11 <3=>PB6 <4=>PB8
#define RTE_I2C4_SCL_PORT_ID            0
#if    (RTE_I2C4_SCL_PORT_ID == 0)
#define RTE_I2C4_SCL_PORT               GPIOD
#define RTE_I2C4_SCL_BIT                12
#elif  (RTE_I2C4_SCL_PORT_ID == 1)
#define RTE_I2C4_SCL_PORT               GPIOF
#define RTE_I2C4_SCL_BIT                14
#elif  (RTE_I2C4_SCL_PORT_ID == 2)
#define RTE_I2C4_SCL_PORT               GPIOH
#define RTE_I2C4_SCL_BIT                11
#elif  (RTE_I2C4_SCL_PORT_ID == 3)
#define RTE_I2C4_SCL_PORT               GPIOB
#define RTE_I2C4_SCL_BIT                6
#elif  (RTE_I2C4_SCL_PORT_ID == 4)
#define RTE_I2C4_SCL_PORT               GPIOB
#define RTE_I2C4_SCL_BIT                8
#else
#error "Invalid I2C4_SCL Pin Configuration!"
#endif

//   <o> I2C4_SDA Pin <0=>PD13 <1=>PF15 <2=>PH12 <3=>PB7 <4=>PB9
#define RTE_I2C4_SDA_PORT_ID            0
#if    (RTE_I2C4_SDA_PORT_ID == 0)
#define RTE_I2C4_SDA_PORT               GPIOD
#define RTE_I2C4_SDA_BIT                13
#elif  (RTE_I2C4_SDA_PORT_ID == 1)
#define RTE_I2C4_SDA_PORT               GPIOF
#define RTE_I2C4_SDA_BIT                15
#elif  (RTE_I2C4_SDA_PORT_ID == 2)
#define RTE_I2C4_SDA_PORT               GPIOH
#define RTE_I2C4_SDA_BIT                12
#elif  (RTE_I2C4_SDA_PORT_ID == 3)
#define RTE_I2C4_SDA_PORT               GPIOB
#define RTE_I2C4_SDA_BIT                7
#elif  (RTE_I2C4_SDA_PORT_ID == 4)
#define RTE_I2C4_SDA_PORT               GPIOB
#define RTE_I2C4_SDA_BIT                9
#else
#error "Invalid I2C4_SDA Pin Configuration!"
#endif

//   <e> DMA Rx
//     <o1> Number <1=>1
//     <i>  Selects DMA Number (only DMA1 can be used)
//     <o2> Stream <2=>2
//     <i>  Selects DMA Stream (only Stream 2 can be used)
//     <o3> Channel <2=>2
//     <i>  Selects DMA Channel (only Channel 2 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_I2C4_RX_DMA                 0
#define RTE_I2C4_RX_DMA_NUMBER          1
#define RTE_I2C4_RX_DMA_STREAM          2
#define RTE_I2C4_RX_DMA_CHANNEL         3
#define RTE_I2C4_RX_DMA_PRIORITY        0

//   <e> DMA Tx
//     <o1> Number <1=>1
//     <i>  Selects DMA Number (only DMA1 can be used)
//     <o2> Stream <5=>5
//     <i>  Selects DMA Stream (only Stream 5 can be used)
//     <o3> Channel <2=>2
//     <i>  Selects DMA Channel (only Channel 2 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_I2C4_TX_DMA                 0
#define RTE_I2C4_TX_DMA_NUMBER          1
#define RTE_I2C4_TX_DMA_STREAM          4
#define RTE_I2C4_TX_DMA_CHANNEL         3
#define RTE_I2C4_TX_DMA_PRIORITY        0

// </e>


// <e> SPI1 (Serial Peripheral Interface 1) [Driver_SPI1]
// <i> Configuration settings for Driver_SPI1 in component ::CMSIS Driver:SPI
#define RTE_SPI1                        0

//   <o> SPI1_MISO Pin <0=>Not Used <1=>PA6 <2=>PB4 <3=>PG9
#define RTE_SPI1_MISO_PORT_ID           0
#if    (RTE_SPI1_MISO_PORT_ID == 0)
#define RTE_SPI1_MISO                   0
#elif  (RTE_SPI1_MISO_PORT_ID == 1)
#define RTE_SPI1_MISO                   1
#define RTE_SPI1_MISO_PORT              GPIOA
#define RTE_SPI1_MISO_BIT               6
#elif  (RTE_SPI1_MISO_PORT_ID == 2)
#define RTE_SPI1_MISO                   1
#define RTE_SPI1_MISO_PORT              GPIOB
#define RTE_SPI1_MISO_BIT               4
#elif  (RTE_SPI1_MISO_PORT_ID == 3)
#define RTE_SPI1_MISO                   1
#define RTE_SPI1_MISO_PORT              GPIOG
#define RTE_SPI1_MISO_BIT               9
#else
#error "Invalid SPI1_MISO Pin Configuration!"
#endif

//   <o> SPI1_MOSI Pin <0=>Not Used <1=>PA7 <2=>PB5 <3=>PD7
#define RTE_SPI1_MOSI_PORT_ID           0
#if    (RTE_SPI1_MOSI_PORT_ID == 0)
#define RTE_SPI1_MOSI                   0
#elif  (RTE_SPI1_MOSI_PORT_ID == 1)
#define RTE_SPI1_MOSI                   1
#define RTE_SPI1_MOSI_PORT              GPIOA
#define RTE_SPI1_MOSI_BIT               7
#elif  (RTE_SPI1_MOSI_PORT_ID == 2)
#define RTE_SPI1_MOSI                   1
#define RTE_SPI1_MOSI_PORT              GPIOB
#define RTE_SPI1_MOSI_BIT               5
#elif  (RTE_SPI1_MOSI_PORT_ID == 3)
#define RTE_SPI1_MOSI                   1
#define RTE_SPI1_MOSI_PORT              GPIOD
#define RTE_SPI1_MOSI_BIT               7
#else
#error "Invalid SPI1_MOSI Pin Configuration!"
#endif

//   <o> SPI1_SCK Pin <0=>PA5 <1=>PB3 <2=>PG11
#define RTE_SPI1_SCL_PORT_ID            0
#if    (RTE_SPI1_SCL_PORT_ID == 0)
#define RTE_SPI1_SCL_PORT               GPIOA
#define RTE_SPI1_SCL_BIT                5
#elif  (RTE_SPI1_SCL_PORT_ID == 1)
#define RTE_SPI1_SCL_PORT               GPIOB
#define RTE_SPI1_SCL_BIT                3
#elif  (RTE_SPI1_SCL_PORT_ID == 2)
#define RTE_SPI1_SCL_PORT               GPIOG
#define RTE_SPI1_SCL_BIT                11
#else
#error "Invalid SPI1_SCK Pin Configuration!"
#endif

//   <o> SPI1_NSS Pin <0=>Not Used <1=>PA4 <2=>PA15 <3=>PG10
#define RTE_SPI1_NSS_PORT_ID            0
#if    (RTE_SPI1_NSS_PORT_ID == 0)
#define RTE_SPI1_NSS_PIN                0
#elif  (RTE_SPI1_NSS_PORT_ID == 1)
#define RTE_SPI1_NSS_PIN                1
#define RTE_SPI1_NSS_PORT               GPIOA
#define RTE_SPI1_NSS_BIT                4
#elif  (RTE_SPI1_NSS_PORT_ID == 2)
#define RTE_SPI1_NSS_PIN                1
#define RTE_SPI1_NSS_PORT               GPIOA
#define RTE_SPI1_NSS_BIT                15
#elif  (RTE_SPI1_NSS_PORT_ID == 3)
#define RTE_SPI1_NSS_PIN                1
#define RTE_SPI1_NSS_PORT               GPIOG
#define RTE_SPI1_NSS_BIT                10
#else
#error "Invalid SPI1_NSS Pin Configuration!"
#endif

//   <e> DMA Rx
//     <o1> Number <2=>2
//     <i>  Selects DMA Number (only DMA2 can be used)
//     <o2> Stream <0=>0 <2=>2
//     <i>  Selects DMA Stream (only Stream 0 or 2 can be used)
//     <o3> Channel <3=>3
//     <i>  Selects DMA Channel (only Channel 3 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_SPI1_RX_DMA                 0
#define RTE_SPI1_RX_DMA_NUMBER          2
#define RTE_SPI1_RX_DMA_STREAM          0
#define RTE_SPI1_RX_DMA_CHANNEL         3
#define RTE_SPI1_RX_DMA_PRIORITY        0

//   <e> DMA Tx
//     <o1> Number <2=>2
//     <i>  Selects DMA Number (only DMA2 can be used)
//     <o2> Stream <3=>3 <5=>5
//     <i>  Selects DMA Stream (only Stream 3 or 5 can be used)
//     <o3> Channel <3=>3
//     <i>  Selects DMA Channel (only Channel 3 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_SPI1_TX_DMA                 0
#define RTE_SPI1_TX_DMA_NUMBER          2
#define RTE_SPI1_TX_DMA_STREAM          5
#define RTE_SPI1_TX_DMA_CHANNEL         3
#define RTE_SPI1_TX_DMA_PRIORITY        0

// </e>


// <e> SPI2 (Serial Peripheral Interface 2) [Driver_SPI2]
// <i> Configuration settings for Driver_SPI2 in component ::CMSIS Driver:SPI
#define RTE_SPI2                        0

//   <o> SPI2_MISO Pin <0=>Not Used <1=>PB14 <2=>PC2 <3=>PI2
#define RTE_SPI2_MISO_PORT_ID           0
#if    (RTE_SPI2_MISO_PORT_ID == 0)
#define RTE_SPI2_MISO                   0
#elif  (RTE_SPI2_MISO_PORT_ID == 1)
#define RTE_SPI2_MISO                   1
#define RTE_SPI2_MISO_PORT              GPIOB
#define RTE_SPI2_MISO_BIT               14
#elif  (RTE_SPI2_MISO_PORT_ID == 2)
#define RTE_SPI2_MISO                   1
#define RTE_SPI2_MISO_PORT              GPIOC
#define RTE_SPI2_MISO_BIT               2
#elif  (RTE_SPI2_MISO_PORT_ID == 3)
#define RTE_SPI2_MISO                   1
#define RTE_SPI2_MISO_PORT              GPIOI
#define RTE_SPI2_MISO_BIT               2
#else
#error "Invalid SPI2_MISO Pin Configuration!"
#endif

//   <o> SPI2_MOSI Pin <0=>Not Used <1=>PB15 <2=>PC1 <3=>PC3 <4=>PI3
#define RTE_SPI2_MOSI_PORT_ID           0
#if    (RTE_SPI2_MOSI_PORT_ID == 0)
#define RTE_SPI2_MOSI                   0
#elif  (RTE_SPI2_MOSI_PORT_ID == 1)
#define RTE_SPI2_MOSI                   1
#define RTE_SPI2_MOSI_PORT              GPIOB
#define RTE_SPI2_MOSI_BIT               15
#elif  (RTE_SPI2_MOSI_PORT_ID == 2)
#define RTE_SPI2_MOSI                   1
#define RTE_SPI2_MOSI_PORT              GPIOC
#define RTE_SPI2_MOSI_BIT               1
#elif  (RTE_SPI2_MOSI_PORT_ID == 3)
#define RTE_SPI2_MOSI                   1
#define RTE_SPI2_MOSI_PORT              GPIOC
#define RTE_SPI2_MOSI_BIT               3
#elif  (RTE_SPI2_MOSI_PORT_ID == 4)
#define RTE_SPI2_MOSI                   1
#define RTE_SPI2_MOSI_PORT              GPIOI
#define RTE_SPI2_MOSI_BIT               3
#else
#error "Invalid SPI2_MOSI Pin Configuration!"
#endif

//   <o> SPI2_SCK Pin <0=>PA9 <1=>PB10 <2=>PB13 <3=>PD3 <4=>PI1 <5=>PA12
#define RTE_SPI2_SCL_PORT_ID            0
#if    (RTE_SPI2_SCL_PORT_ID == 0)
#define RTE_SPI2_SCL_PORT               GPIOA
#define RTE_SPI2_SCL_BIT                9
#elif  (RTE_SPI2_SCL_PORT_ID == 1)
#define RTE_SPI2_SCL_PORT               GPIOB
#define RTE_SPI2_SCL_BIT                10
#elif  (RTE_SPI2_SCL_PORT_ID == 2)
#define RTE_SPI2_SCL_PORT               GPIOB
#define RTE_SPI2_SCL_BIT                13
#elif  (RTE_SPI2_SCL_PORT_ID == 3)
#define RTE_SPI2_SCL_PORT               GPIOD
#define RTE_SPI2_SCL_BIT                3
#elif  (RTE_SPI2_SCL_PORT_ID == 4)
#define RTE_SPI2_SCL_PORT               GPIOI
#define RTE_SPI2_SCL_BIT                1
#elif  (RTE_SPI2_SCL_PORT_ID == 5)
#define RTE_SPI2_SCL_PORT               GPIOA
#define RTE_SPI2_SCL_BIT                12
#else
#error "Invalid SPI2_SCK Pin Configuration!"
#endif

//   <o> SPI2_NSS Pin <0=>Not Used <1=>PB4 <2=>PB9 <3=>PB12 <4=>PI0 <5=>PA11
#define RTE_SPI2_NSS_PORT_ID            0
#if    (RTE_SPI2_NSS_PORT_ID == 0)
#define RTE_SPI2_NSS_PIN                0
#elif  (RTE_SPI2_NSS_PORT_ID == 1)
#define RTE_SPI2_NSS_PIN                1
#define RTE_SPI2_NSS_PORT               GPIOB
#define RTE_SPI2_NSS_BIT                4
#elif  (RTE_SPI2_NSS_PORT_ID == 2)
#define RTE_SPI2_NSS_PIN                1
#define RTE_SPI2_NSS_PORT               GPIOB
#define RTE_SPI2_NSS_BIT                9
#elif  (RTE_SPI2_NSS_PORT_ID == 3)
#define RTE_SPI2_NSS_PIN                1
#define RTE_SPI2_NSS_PORT               GPIOB
#define RTE_SPI2_NSS_BIT                12
#elif  (RTE_SPI2_NSS_PORT_ID == 4)
#define RTE_SPI2_NSS_PIN                1
#define RTE_SPI2_NSS_PORT               GPIOI
#define RTE_SPI2_NSS_BIT                0
#elif  (RTE_SPI2_NSS_PORT_ID == 5)
#define RTE_SPI2_NSS_PIN                1
#define RTE_SPI2_NSS_PORT               GPIOA
#define RTE_SPI2_NSS_BIT                11
#else
#error "Invalid SPI2_NSS Pin Configuration!"
#endif

//   <e> DMA Rx
//     <o1> Number <1=>1
//     <i>  Selects DMA Number (only DMA1 can be used)
//     <o2> Stream <3=>3
//     <i>  Selects DMA Stream (only Stream 3 can be used)
//     <o3> Channel <0=>0
//     <i>  Selects DMA Channel (only Channel 0 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_SPI2_RX_DMA                 0
#define RTE_SPI2_RX_DMA_NUMBER          1
#define RTE_SPI2_RX_DMA_STREAM          3
#define RTE_SPI2_RX_DMA_CHANNEL         0
#define RTE_SPI2_RX_DMA_PRIORITY        0

//   <e> DMA Tx
//     <o1> Number <1=>1
//     <i>  Selects DMA Number (only DMA1 can be used)
//     <o2> Stream <4=>4
//     <i>  Selects DMA Stream (only Stream 4 can be used)
//     <o3> Channel <0=>0
//     <i>  Selects DMA Channel (only Channel 0 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_SPI2_TX_DMA                 0
#define RTE_SPI2_TX_DMA_NUMBER          1
#define RTE_SPI2_TX_DMA_STREAM          4
#define RTE_SPI2_TX_DMA_CHANNEL         0
#define RTE_SPI2_TX_DMA_PRIORITY        0

// </e>


// <e> SPI3 (Serial Peripheral Interface 3) [Driver_SPI3]
// <i> Configuration settings for Driver_SPI3 in component ::CMSIS Driver:SPI
#define RTE_SPI3                        0

//   <o> SPI3_MISO Pin <0=>Not Used <1=>PB4 <2=>PC11
#define RTE_SPI3_MISO_PORT_ID           0
#if    (RTE_SPI3_MISO_PORT_ID == 0)
#define RTE_SPI3_MISO                   0
#elif  (RTE_SPI3_MISO_PORT_ID == 1)
#define RTE_SPI3_MISO                   1
#define RTE_SPI3_MISO_PORT              GPIOB
#define RTE_SPI3_MISO_BIT               4
#elif  (RTE_SPI3_MISO_PORT_ID == 2)
#define RTE_SPI3_MISO                   1
#define RTE_SPI3_MISO_PORT              GPIOC
#define RTE_SPI3_MISO_BIT               11
#else
#error "Invalid SPI3_MISO Pin Configuration!"
#endif

//   <o> SPI3_MOSI Pin <0=>Not Used <1=>PB2 <2=>PB5 <3=>PC12 <4=>PD6
#define RTE_SPI3_MOSI_PORT_ID           0
#if    (RTE_SPI3_MOSI_PORT_ID == 0)
#define RTE_SPI3_MOSI                   0
#elif  (RTE_SPI3_MOSI_PORT_ID == 1)
#define RTE_SPI3_MOSI                   1
#define RTE_SPI3_MOSI_PORT              GPIOB
#define RTE_SPI3_MOSI_BIT               2
#elif  (RTE_SPI3_MOSI_PORT_ID == 2)
#define RTE_SPI3_MOSI                   1
#define RTE_SPI3_MOSI_PORT              GPIOB
#define RTE_SPI3_MOSI_BIT               5
#elif  (RTE_SPI3_MOSI_PORT_ID == 3)
#define RTE_SPI3_MOSI                   1
#define RTE_SPI3_MOSI_PORT              GPIOC
#define RTE_SPI3_MOSI_BIT               12
#elif  (RTE_SPI3_MOSI_PORT_ID == 4)
#define RTE_SPI3_MOSI                   1
#define RTE_SPI3_MOSI_PORT              GPIOD
#define RTE_SPI3_MOSI_BIT               6
#else
#error "Invalid SPI3_MOSI Pin Configuration!"
#endif

//   <o> SPI3_SCK Pin <0=>PB3 <1=>PC10
#define RTE_SPI3_SCL_PORT_ID            0
#if    (RTE_SPI3_SCL_PORT_ID == 0)
#define RTE_SPI3_SCL_PORT               GPIOB
#define RTE_SPI3_SCL_BIT                3
#elif  (RTE_SPI3_SCL_PORT_ID == 1)
#define RTE_SPI3_SCL_PORT               GPIOC
#define RTE_SPI3_SCL_BIT                10
#else
#error "Invalid SPI3_SCK Pin Configuration!"
#endif

//   <o> SPI3_NSS Pin <0=>Not Used <1=>PA4 <2=>PA15
#define RTE_SPI3_NSS_PORT_ID            0
#if    (RTE_SPI3_NSS_PORT_ID == 0)
#define RTE_SPI3_NSS_PIN                0
#elif  (RTE_SPI3_NSS_PORT_ID == 1)
#define RTE_SPI3_NSS_PIN                1
#define RTE_SPI3_NSS_PORT               GPIOA
#define RTE_SPI3_NSS_BIT                4
#elif  (RTE_SPI3_NSS_PORT_ID == 2)
#define RTE_SPI3_NSS_PIN                1
#define RTE_SPI3_NSS_PORT               GPIOA
#define RTE_SPI3_NSS_BIT                15
#else
#error "Invalid SPI3_NSS Pin Configuration!"
#endif

//   <e> DMA Rx
//     <o1> Number <1=>1
//     <i>  Selects DMA Number (only DMA1 can be used)
//     <o2> Stream <0=>0 <2=>2
//     <i>  Selects DMA Stream (only Stream 0 or 2 can be used)
//     <o3> Channel <0=>0
//     <i>  Selects DMA Channel (only Channel 0 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_SPI3_RX_DMA                 0
#define RTE_SPI3_RX_DMA_NUMBER          1
#define RTE_SPI3_RX_DMA_STREAM          0
#define RTE_SPI3_RX_DMA_CHANNEL         0
#define RTE_SPI3_RX_DMA_PRIORITY        0

//   <e> DMA Tx
//     <o1> Number <1=>1
//     <i>  Selects DMA Number (only DMA1 can be used)
//     <o2> Stream <5=>5 <7=>7
//     <i>  Selects DMA Stream (only Stream 5 or 7 can be used)
//     <o3> Channel <0=>0
//     <i>  Selects DMA Channel (only Channel 0 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_SPI3_TX_DMA                 0
#define RTE_SPI3_TX_DMA_NUMBER          1
#define RTE_SPI3_TX_DMA_STREAM          5
#define RTE_SPI3_TX_DMA_CHANNEL         0
#define RTE_SPI3_TX_DMA_PRIORITY        0

// </e>


// <e> SPI4 (Serial Peripheral Interface 4) [Driver_SPI4]
// <i> Configuration settings for Driver_SPI4 in component ::CMSIS Driver:SPI
#define RTE_SPI4                        0

//   <o> SPI4_MISO Pin <0=>Not Used <1=>PE5 <2=>PE13
#define RTE_SPI4_MISO_PORT_ID           0
#if    (RTE_SPI4_MISO_PORT_ID == 0)
#define RTE_SPI4_MISO                   0
#elif  (RTE_SPI4_MISO_PORT_ID == 1)
#define RTE_SPI4_MISO                   1
#define RTE_SPI4_MISO_PORT              GPIOE
#define RTE_SPI4_MISO_BIT               5
#elif  (RTE_SPI4_MISO_PORT_ID == 2)
#define RTE_SPI4_MISO                   1
#define RTE_SPI4_MISO_PORT              GPIOE
#define RTE_SPI4_MISO_BIT               13
#else
#error "Invalid SPI4_MISO Pin Configuration!"
#endif

//   <o> SPI4_MOSI Pin <0=>Not Used <1=>PE6 <2=>PE14
#define RTE_SPI4_MOSI_PORT_ID           0
#if    (RTE_SPI4_MOSI_PORT_ID == 0)
#define RTE_SPI4_MOSI                   0
#elif  (RTE_SPI4_MOSI_PORT_ID == 1)
#define RTE_SPI4_MOSI                   1
#define RTE_SPI4_MOSI_PORT              GPIOE
#define RTE_SPI4_MOSI_BIT               6
#elif  (RTE_SPI4_MOSI_PORT_ID == 2)
#define RTE_SPI4_MOSI                   1
#define RTE_SPI4_MOSI_PORT              GPIOE
#define RTE_SPI4_MOSI_BIT               14
#else
#error "Invalid SPI4_MOSI Pin Configuration!"
#endif

//   <o> SPI4_SCK Pin <0=>PE2 <1=>PE12
#define RTE_SPI4_SCL_PORT_ID            0
#if    (RTE_SPI4_SCL_PORT_ID == 0)
#define RTE_SPI4_SCL_PORT               GPIOE
#define RTE_SPI4_SCL_BIT                2
#elif  (RTE_SPI4_SCL_PORT_ID == 1)
#define RTE_SPI4_SCL_PORT               GPIOE
#define RTE_SPI4_SCL_BIT                12
#else
#error "Invalid SPI4_SCK Pin Configuration!"
#endif

//   <o> SPI4_NSS Pin <0=>Not Used <1=>PE4 <2=>PE11
#define RTE_SPI4_NSS_PORT_ID            0
#if    (RTE_SPI4_NSS_PORT_ID == 0)
#define RTE_SPI4_NSS_PIN                0
#elif  (RTE_SPI4_NSS_PORT_ID == 1)
#define RTE_SPI4_NSS_PIN                1
#define RTE_SPI4_NSS_PORT               GPIOE
#define RTE_SPI4_NSS_BIT                4
#elif  (RTE_SPI4_NSS_PORT_ID == 2)
#define RTE_SPI4_NSS_PIN                1
#define RTE_SPI4_NSS_PORT               GPIOE
#define RTE_SPI4_NSS_BIT                11
#else
#error "Invalid SPI4_NSS Pin Configuration!"
#endif

//   <e> DMA Rx
//     <o1> Number <2=>2
//     <i>  Selects DMA Number (only DMA2 can be used)
//     <o2> Stream <0=>0 <3=>3
//     <i>  Selects DMA Stream (only Stream 0 or 3 can be used)
//     <o3> Channel <4=>4 <5=>5
//     <i>  Selects DMA Channel (only Channel 4 or 5 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_SPI4_RX_DMA                 0
#define RTE_SPI4_RX_DMA_NUMBER          2
#define RTE_SPI4_RX_DMA_STREAM          0
#define RTE_SPI4_RX_DMA_CHANNEL         4
#define RTE_SPI4_RX_DMA_PRIORITY        0

//   <e> DMA Tx
//     <o1> Number <2=>2
//     <i>  Selects DMA Number (only DMA2 can be used)
//     <o2> Stream <1=>1 <4=>4
//     <i>  Selects DMA Stream (only Stream 1 or 4 can be used)
//     <o3> Channel <4=>4 <5=>5
//     <i>  Selects DMA Channel (only Channel 4 or 5 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_SPI4_TX_DMA                 0
#define RTE_SPI4_TX_DMA_NUMBER          2
#define RTE_SPI4_TX_DMA_STREAM          1
#define RTE_SPI4_TX_DMA_CHANNEL         4
#define RTE_SPI4_TX_DMA_PRIORITY        0

// </e>


// <e> SPI5 (Serial Peripheral Interface 5) [Driver_SPI5]
// <i> Configuration settings for Driver_SPI5 in component ::CMSIS Driver:SPI
#define RTE_SPI5                        0

//   <o> SPI5_MISO Pin <0=>Not Used <1=>PF8 <2=>PH7
#define RTE_SPI5_MISO_PORT_ID           0
#if    (RTE_SPI5_MISO_PORT_ID == 0)
#define RTE_SPI5_MISO                   0
#elif  (RTE_SPI5_MISO_PORT_ID == 1)
#define RTE_SPI5_MISO                   1
#define RTE_SPI5_MISO_PORT              GPIOF
#define RTE_SPI5_MISO_BIT               8
#elif  (RTE_SPI5_MISO_PORT_ID == 2)
#define RTE_SPI5_MISO                   1
#define RTE_SPI5_MISO_PORT              GPIOH
#define RTE_SPI5_MISO_BIT               7
#else
#error "Invalid SPI5_MISO Pin Configuration!"
#endif

//   <o> SPI5_MOSI Pin <0=>Not Used <1=>PF9 <2=>PF11
#define RTE_SPI5_MOSI_PORT_ID           0
#if    (RTE_SPI5_MOSI_PORT_ID == 0)
#define RTE_SPI5_MOSI                   0
#elif  (RTE_SPI5_MOSI_PORT_ID == 1)
#define RTE_SPI5_MOSI                   1
#define RTE_SPI5_MOSI_PORT              GPIOF
#define RTE_SPI5_MOSI_BIT               9
#elif  (RTE_SPI5_MOSI_PORT_ID == 2)
#define RTE_SPI5_MOSI                   1
#define RTE_SPI5_MOSI_PORT              GPIOF
#define RTE_SPI5_MOSI_BIT               11
#else
#error "Invalid SPI5_MOSI Pin Configuration!"
#endif

//   <o> SPI5_SCK Pin <0=>PF7 <1=>PH6
#define RTE_SPI5_SCL_PORT_ID            0
#if    (RTE_SPI5_SCL_PORT_ID == 0)
#define RTE_SPI5_SCL_PORT               GPIOF
#define RTE_SPI5_SCL_BIT                7
#elif  (RTE_SPI5_SCL_PORT_ID == 1)
#define RTE_SPI5_SCL_PORT               GPIOH
#define RTE_SPI5_SCL_BIT                6
#else
#error "Invalid SPI5_SCK Pin Configuration!"
#endif

//   <o> SPI5_NSS Pin <0=>Not Used <1=>PF6 <2=>PH5
#define RTE_SPI5_NSS_PORT_ID            0
#if    (RTE_SPI5_NSS_PORT_ID == 0)
#define RTE_SPI5_NSS_PIN                0
#elif  (RTE_SPI5_NSS_PORT_ID == 1)
#define RTE_SPI5_NSS_PIN                1
#define RTE_SPI5_NSS_PORT               GPIOF
#define RTE_SPI5_NSS_BIT                6
#elif  (RTE_SPI5_NSS_PORT_ID == 2)
#define RTE_SPI5_NSS_PIN                1
#define RTE_SPI5_NSS_PORT               GPIOH
#define RTE_SPI5_NSS_BIT                5
#else
#error "Invalid SPI5_NSS Pin Configuration!"
#endif

//   <e> DMA Rx
//     <o1> Number <2=>2
//     <i>  Selects DMA Number (only DMA2 can be used)
//     <o2> Stream <3=>3 <5=>5
//     <i>  Selects DMA Stream (only Stream 3 or 5 can be used)
//     <o3> Channel <2=>2 <7=>7
//     <i>  Selects DMA Channel (only Channel 2 or 7 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_SPI5_RX_DMA                 0
#define RTE_SPI5_RX_DMA_NUMBER          2
#define RTE_SPI5_RX_DMA_STREAM          3
#define RTE_SPI5_RX_DMA_CHANNEL         2
#define RTE_SPI5_RX_DMA_PRIORITY        0

//   <e> DMA Tx
//     <o1> Number <2=>2
//     <i>  Selects DMA Number (only DMA2 can be used)
//     <o2> Stream <4=>4 <6=>6
//     <i>  Selects DMA Stream (only Stream 4 or 6 can be used)
//     <o3> Channel <2=>2 <7=>7
//     <i>  Selects DMA Channel (only Channel 2 or 7 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_SPI5_TX_DMA                 0
#define RTE_SPI5_TX_DMA_NUMBER          2
#define RTE_SPI5_TX_DMA_STREAM          4
#define RTE_SPI5_TX_DMA_CHANNEL         2
#define RTE_SPI5_TX_DMA_PRIORITY        0

// </e>


// <e> SPI6 (Serial Peripheral Interface 6) [Driver_SPI6]
// <i> Configuration settings for Driver_SPI6 in component ::CMSIS Driver:SPI
#define RTE_SPI6                        0

//   <o> SPI6_MISO Pin <0=>Not Used <1=>PG12 <2=>PA6 <3=>PB4
#define RTE_SPI6_MISO_PORT_ID           0
#if    (RTE_SPI6_MISO_PORT_ID == 0)
#define RTE_SPI6_MISO                   0
#elif  (RTE_SPI6_MISO_PORT_ID == 1)
#define RTE_SPI6_MISO                   1
#define RTE_SPI6_MISO_PORT              GPIOG
#define RTE_SPI6_MISO_BIT               12
#elif  (RTE_SPI6_MISO_PORT_ID == 2)
#define RTE_SPI6_MISO                   1
#define RTE_SPI6_MISO_PORT              GPIOA
#define RTE_SPI6_MISO_BIT               6
#elif  (RTE_SPI6_MISO_PORT_ID == 3)
#define RTE_SPI6_MISO                   1
#define RTE_SPI6_MISO_PORT              GPIOB
#define RTE_SPI6_MISO_BIT               4
#else
#error "Invalid SPI6_MISO Pin Configuration!"
#endif

//   <o> SPI6_MOSI Pin <0=>Not Used <1=>PG14 <2=>PA7 <3=>PB5
#define RTE_SPI6_MOSI_PORT_ID           0
#if    (RTE_SPI6_MOSI_PORT_ID == 0)
#define RTE_SPI6_MOSI                   0
#elif  (RTE_SPI6_MOSI_PORT_ID == 1)
#define RTE_SPI6_MOSI                   1
#define RTE_SPI6_MOSI_PORT              GPIOG
#define RTE_SPI6_MOSI_BIT               14
#elif  (RTE_SPI6_MOSI_PORT_ID == 2)
#define RTE_SPI6_MOSI                   1
#define RTE_SPI6_MOSI_PORT              GPIOA
#define RTE_SPI6_MOSI_BIT               7
#elif  (RTE_SPI6_MOSI_PORT_ID == 3)
#define RTE_SPI6_MOSI                   1
#define RTE_SPI6_MOSI_PORT              GPIOB
#define RTE_SPI6_MOSI_BIT               5
#else
#error "Invalid SPI6_MOSI Pin Configuration!"
#endif

//   <o> SPI6_SCK Pin <0=>PG13 <1=>PA5 <2=>PB3
#define RTE_SPI6_SCL_PORT_ID            0
#if    (RTE_SPI6_SCL_PORT_ID == 0)
#define RTE_SPI6_SCL_PORT               GPIOG
#define RTE_SPI6_SCL_BIT                13
#elif  (RTE_SPI6_SCL_PORT_ID == 1)
#define RTE_SPI6_SCL_PORT               GPIOA
#define RTE_SPI6_SCL_BIT                5
#elif  (RTE_SPI6_SCL_PORT_ID == 2)
#define RTE_SPI6_SCL_PORT               GPIOB
#define RTE_SPI6_SCL_BIT                3
#else
#error "Invalid SPI6_SCK Pin Configuration!"
#endif

//   <o> SPI6_NSS Pin <0=>Not Used <1=>PG8 <2=>PA4 <3=>PA15
#define RTE_SPI6_NSS_PORT_ID            0
#if    (RTE_SPI6_NSS_PORT_ID == 0)
#define RTE_SPI6_NSS_PIN                0
#elif  (RTE_SPI6_NSS_PORT_ID == 1)
#define RTE_SPI6_NSS_PIN                1
#define RTE_SPI6_NSS_PORT               GPIOG
#define RTE_SPI6_NSS_BIT                8
#elif  (RTE_SPI6_NSS_PORT_ID == 2)
#define RTE_SPI6_NSS_PIN                1
#define RTE_SPI6_NSS_PORT               GPIOA
#define RTE_SPI6_NSS_BIT                4
#elif  (RTE_SPI6_NSS_PORT_ID == 3)
#define RTE_SPI6_NSS_PIN                1
#define RTE_SPI6_NSS_PORT               GPIOA
#define RTE_SPI6_NSS_BIT                15
#else
#error "Invalid SPI6_NSS Pin Configuration!"
#endif

//   <e> DMA Rx
//     <o1> Number <2=>2
//     <i>  Selects DMA Number (only DMA2 can be used)
//     <o2> Stream <6=>6
//     <i>  Selects DMA Stream (only Stream 6 can be used)
//     <o3> Channel <1=>1
//     <i>  Selects DMA Channel (only Channel 1 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_SPI6_RX_DMA                 0
#define RTE_SPI6_RX_DMA_NUMBER          2
#define RTE_SPI6_RX_DMA_STREAM          6
#define RTE_SPI6_RX_DMA_CHANNEL         1
#define RTE_SPI6_RX_DMA_PRIORITY        0

//   <e> DMA Tx
//     <o1> Number <2=>2
//     <i>  Selects DMA Number (only DMA2 can be used)
//     <o2> Stream <5=>5
//     <i>  Selects DMA Stream (only Stream 5 can be used)
//     <o3> Channel <1=>1
//     <i>  Selects DMA Channel (only Channel 1 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_SPI6_TX_DMA                 0
#define RTE_SPI6_TX_DMA_NUMBER          2
#define RTE_SPI6_TX_DMA_STREAM          5
#define RTE_SPI6_TX_DMA_CHANNEL         1
#define RTE_SPI6_TX_DMA_PRIORITY        0

// </e>


// <e> SAI1 (Serial Audio Interface 1) [Driver_SAI1]
// <i> Configuration settings for Driver_SAI1 in component ::CMSIS Driver:SAI
#define RTE_SAI1                        0

//   <o> SAI1_SD_A Pin <0=>Not Used <1=>PB2 <2=>PC1 <3=>PD6 <4=>PE6
#define RTE_SAI1_SD_A_PORT_ID           0
#if    (RTE_SAI1_SD_A_PORT_ID == 0)
#define RTE_SAI1_SD_A_PIN               0
#elif  (RTE_SAI1_SD_A_PORT_ID == 1)
#define RTE_SAI1_SD_A_PIN               1
#define RTE_SAI1_SD_A_PORT              GPIOB
#define RTE_SAI1_SD_A_BIT               2
#elif  (RTE_SAI1_SD_A_PORT_ID == 2)
#define RTE_SAI1_SD_A_PIN               1
#define RTE_SAI1_SD_A_PORT              GPIOC
#define RTE_SAI1_SD_A_BIT               1
#elif  (RTE_SAI1_SD_A_PORT_ID == 3)
#define RTE_SAI1_SD_A_PIN               1
#define RTE_SAI1_SD_A_PORT              GPIOD
#define RTE_SAI1_SD_A_BIT               6
#elif  (RTE_SAI1_SD_A_PORT_ID == 4)
#define RTE_SAI1_SD_A_PIN               1
#define RTE_SAI1_SD_A_PORT              GPIOE
#define RTE_SAI1_SD_A_BIT               6
#else
#error "Invalid SAI1_SD_A Pin Configuration!"
#endif

//   <o> SAI1_FS_A Pin <0=>Not Used <1=>PE4
#define RTE_SAI1_FS_A_PORT_ID           0
#if    (RTE_SAI1_FS_A_PORT_ID == 0)
#define RTE_SAI1_FS_A_PIN               0
#elif  (RTE_SAI1_FS_A_PORT_ID == 1)
#define RTE_SAI1_FS_A_PIN               1
#define RTE_SAI1_FS_A_PORT              GPIOE
#define RTE_SAI1_FS_A_BIT               4
#else
#error "Invalid SAI1_FS_A Pin Configuration!"
#endif

//   <o> SAI1_SCK_A Pin <0=>Not Used <1=>PE5
#define RTE_SAI1_SCK_A_PORT_ID          0
#if    (RTE_SAI1_SCK_A_PORT_ID == 0)
#define RTE_SAI1_SCK_A_PIN              0
#elif  (RTE_SAI1_SCK_A_PORT_ID == 1)
#define RTE_SAI1_SCK_A_PIN              1
#define RTE_SAI1_SCK_A_PORT             GPIOE
#define RTE_SAI1_SCK_A_BIT              5
#else
#error "Invalid SAI1_SCK_A Pin Configuration!"
#endif

//   <o> SAI1_MCLK_A Pin <0=>Not Used <1=>PE2 <2=>PG7
#define RTE_SAI1_MCLK_A_PORT_ID         0
#if    (RTE_SAI1_MCLK_A_PORT_ID == 0)
#define RTE_SAI1_MCLK_A_PIN             0
#elif  (RTE_SAI1_MCLK_A_PORT_ID == 1)
#define RTE_SAI1_MCLK_A_PIN             1
#define RTE_SAI1_MCLK_A_PORT            GPIOE
#define RTE_SAI1_MCLK_A_BIT             2
#elif  (RTE_SAI1_MCLK_A_PORT_ID == 2)
#define RTE_SAI1_MCLK_A_PIN             1
#define RTE_SAI1_MCLK_A_PORT            GPIOG
#define RTE_SAI1_MCLK_A_BIT             7
#else
#error "Invalid SAI1_MCLK_A Pin Configuration!"
#endif

//   <o> SAI1_SD_B Pin <0=>Not Used <1=>PE3 <2=>PF6
#define RTE_SAI1_SD_B_PORT_ID           0
#if    (RTE_SAI1_SD_B_PORT_ID == 0)
#define RTE_SAI1_SD_B_PIN               0
#elif  (RTE_SAI1_SD_B_PORT_ID == 1)
#define RTE_SAI1_SD_B_PIN               1
#define RTE_SAI1_SD_B_PORT              GPIOE
#define RTE_SAI1_SD_B_BIT               3
#elif  (RTE_SAI1_SD_B_PORT_ID == 2)
#define RTE_SAI1_SD_B_PIN               1
#define RTE_SAI1_SD_B_PORT              GPIOF
#define RTE_SAI1_SD_B_BIT               6
#else
#error "Invalid SAI1_SD_B Pin Configuration!"
#endif

//   <o> SAI1_FS_B Pin <0=>Not Used <1=>PF9
#define RTE_SAI1_FS_B_PORT_ID           0
#if    (RTE_SAI1_FS_B_PORT_ID == 0)
#define RTE_SAI1_FS_B_PIN               0
#elif  (RTE_SAI1_FS_B_PORT_ID == 1)
#define RTE_SAI1_FS_B_PIN               1
#define RTE_SAI1_FS_B_PORT              GPIOF
#define RTE_SAI1_FS_B_BIT               9
#else
#error "Invalid SAI1_FS_B Pin Configuration!"
#endif

//   <o> SAI1_SCK_B Pin <0=>Not Used <1=>PF8
#define RTE_SAI1_SCK_B_PORT_ID          0
#if    (RTE_SAI1_SCK_B_PORT_ID == 0)
#define RTE_SAI1_SCK_B_PIN              0
#elif  (RTE_SAI1_SCK_B_PORT_ID == 1)
#define RTE_SAI1_SCK_B_PIN              1
#define RTE_SAI1_SCK_B_PORT             GPIOF
#define RTE_SAI1_SCK_B_BIT              8
#else
#error "Invalid SAI1_SCK_B Pin Configuration!"
#endif

//   <o> SAI1_MCLK_B Pin <0=>Not Used <1=>PF7
#define RTE_SAI1_MCLK_B_PORT_ID         0
#if    (RTE_SAI1_MCLK_B_PORT_ID == 0)
#define RTE_SAI1_MCLK_B_PIN             0
#elif  (RTE_SAI1_MCLK_B_PORT_ID == 1)
#define RTE_SAI1_MCLK_B_PIN             1
#define RTE_SAI1_MCLK_B_PORT            GPIOF
#define RTE_SAI1_MCLK_B_BIT             7
#else
#error "Invalid SAI1_MCLK_B Pin Configuration!"
#endif

//   <e> DMA SAI1_A
//     <o1> Number <2=>2
//     <i>  Selects DMA Number (only DMA2 can be used)
//     <o2> Stream <1=>1 <3=>3
//     <i>  Selects DMA Stream (only Stream 1 or 3 can be used)
//     <o3> Channel <0=>0
//     <i>  Selects DMA Channel (only Channel 0 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_SAI1_A_DMA                  0
#define RTE_SAI1_A_DMA_NUMBER           2
#define RTE_SAI1_A_DMA_STREAM           1
#define RTE_SAI1_A_DMA_CHANNEL          0
#define RTE_SAI1_A_DMA_PRIORITY         0

//   <e> DMA SAI1_B
//     <o1> Number <2=>2
//     <i>  Selects DMA Number (only DMA2 can be used)
//     <o2> Stream <4=>4 <5=>5 <7=>7
//     <i>  Selects DMA Stream (only Stream 4, 5 or 7 can be used)
//     <o3> Channel <0=>0 <1=>1
//     <i>  Selects DMA Channel (only Channel 0 or 1 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_SAI1_B_DMA                  0
#define RTE_SAI1_B_DMA_NUMBER           2
#define RTE_SAI1_B_DMA_STREAM           5
#define RTE_SAI1_B_DMA_CHANNEL          0
#define RTE_SAI1_B_DMA_PRIORITY         0

// </e>

// <e> SAI2 (Serial Audio Interface 2) [Driver_SAI2]
// <i> Configuration settings for Driver_SAI2 in component ::CMSIS Driver:SAI
#define RTE_SAI2                        0

//   <o> SAI2_SD_A Pin <0=>Not Used <1=>PD11 <2=>PI6
#define RTE_SAI2_SD_A_PORT_ID           0
#if    (RTE_SAI2_SD_A_PORT_ID == 0)
#define RTE_SAI2_SD_A_PIN               0
#elif  (RTE_SAI2_SD_A_PORT_ID == 1)
#define RTE_SAI2_SD_A_PIN               1
#define RTE_SAI2_SD_A_PORT              GPIOD
#define RTE_SAI2_SD_A_BIT               11
#elif  (RTE_SAI2_SD_A_PORT_ID == 2)
#define RTE_SAI2_SD_A_PIN               1
#define RTE_SAI2_SD_A_PORT              GPIOI
#define RTE_SAI2_SD_A_BIT               6
#else
#error "Invalid SAI2_SD_A Pin Configuration!"
#endif

//   <o> SAI2_FS_A Pin <0=>Not Used <1=>PD12 <2=>PI7
#define RTE_SAI2_FS_A_PORT_ID           0
#if    (RTE_SAI2_FS_A_PORT_ID == 0)
#define RTE_SAI2_FS_A_PIN               0
#elif  (RTE_SAI2_FS_A_PORT_ID == 1)
#define RTE_SAI2_FS_A_PIN               1
#define RTE_SAI2_FS_A_PORT              GPIOD
#define RTE_SAI2_FS_A_BIT               12
#elif  (RTE_SAI2_FS_A_PORT_ID == 2)
#define RTE_SAI2_FS_A_PIN               1
#define RTE_SAI2_FS_A_PORT              GPIOI
#define RTE_SAI2_FS_A_BIT               7
#else
#error "Invalid SAI2_FS_A Pin Configuration!"
#endif

//   <o> SAI2_SCK_A Pin <0=>Not Used <1=>PD13 <2=>PI5
#define RTE_SAI2_SCK_A_PORT_ID          0
#if    (RTE_SAI2_SCK_A_PORT_ID == 0)
#define RTE_SAI2_SCK_A_PIN              0
#elif  (RTE_SAI2_SCK_A_PORT_ID == 1)
#define RTE_SAI2_SCK_A_PIN              1
#define RTE_SAI2_SCK_A_PORT             GPIOD
#define RTE_SAI2_SCK_A_BIT              13
#elif  (RTE_SAI2_SCK_A_PORT_ID == 2)
#define RTE_SAI2_SCK_A_PIN              1
#define RTE_SAI2_SCK_A_PORT             GPIOI
#define RTE_SAI2_SCK_A_BIT              5
#else
#error "Invalid SAI2_SCK_A Pin Configuration!"
#endif

//   <o> SAI2_MCLK_A Pin <0=>Not Used <1=>PE0 <2=>PI4
#define RTE_SAI2_MCLK_A_PORT_ID         0
#if    (RTE_SAI2_MCLK_A_PORT_ID == 0)
#define RTE_SAI2_MCLK_A_PIN             0
#elif  (RTE_SAI2_MCLK_A_PORT_ID == 1)
#define RTE_SAI2_MCLK_A_PIN             1
#define RTE_SAI2_MCLK_A_PORT            GPIOE
#define RTE_SAI2_MCLK_A_BIT             0
#elif  (RTE_SAI2_MCLK_A_PORT_ID == 2)
#define RTE_SAI2_MCLK_A_PIN             1
#define RTE_SAI2_MCLK_A_PORT            GPIOI
#define RTE_SAI2_MCLK_A_BIT             4
#else
#error "Invalid SAI2_MCLK_A Pin Configuration!"
#endif

//   <o> SAI2_SD_B Pin <0=>Not Used <1=>PA0 <2=>PE11 <3=>PF11 <4=>PG10
#define RTE_SAI2_SD_B_PORT_ID           0
#if    (RTE_SAI2_SD_B_PORT_ID == 0)
#define RTE_SAI2_SD_B_PIN               0
#elif  (RTE_SAI2_SD_B_PORT_ID == 1)
#define RTE_SAI2_SD_B_PIN               1
#define RTE_SAI2_SD_B_PORT              GPIOA
#define RTE_SAI2_SD_B_BIT               0
#elif  (RTE_SAI2_SD_B_PORT_ID == 2)
#define RTE_SAI2_SD_B_PIN               1
#define RTE_SAI2_SD_B_PORT              GPIOE
#define RTE_SAI2_SD_B_BIT               11
#elif  (RTE_SAI2_SD_B_PORT_ID == 3)
#define RTE_SAI2_SD_B_PIN               1
#define RTE_SAI2_SD_B_PORT              GPIOF
#define RTE_SAI2_SD_B_BIT               11
#elif  (RTE_SAI2_SD_B_PORT_ID == 4)
#define RTE_SAI2_SD_B_PIN               1
#define RTE_SAI2_SD_B_PORT              GPIOG
#define RTE_SAI2_SD_B_BIT               10
#else
#error "Invalid SAI2_SD_B Pin Configuration!"
#endif

//   <o> SAI2_FS_B Pin <0=>Not Used <1=>PA12 <2=>PC0 <3=>PE13 <4=>PG9
#define RTE_SAI2_FS_B_PORT_ID           0
#if    (RTE_SAI2_FS_B_PORT_ID == 0)
#define RTE_SAI2_FS_B_PIN               0
#elif  (RTE_SAI2_FS_B_PORT_ID == 1)
#define RTE_SAI2_FS_B_PIN               1
#define RTE_SAI2_FS_B_PORT              GPIOA
#define RTE_SAI2_FS_B_BIT               12
#elif  (RTE_SAI2_FS_B_PORT_ID == 2)
#define RTE_SAI2_FS_B_PIN               1
#define RTE_SAI2_FS_B_PORT              GPIOC
#define RTE_SAI2_FS_B_BIT               0
#elif  (RTE_SAI2_FS_B_PORT_ID == 3)
#define RTE_SAI2_FS_B_PIN               1
#define RTE_SAI2_FS_B_PORT              GPIOE
#define RTE_SAI2_FS_B_BIT               13
#elif  (RTE_SAI2_FS_B_PORT_ID == 4)
#define RTE_SAI2_FS_B_PIN               1
#define RTE_SAI2_FS_B_PORT              GPIOG
#define RTE_SAI2_FS_B_BIT               9
#else
#error "Invalid SAI2_FS_B Pin Configuration!"
#endif

//   <o> SAI2_SCK_B Pin <0=>Not Used <1=>PA2 <2=>PE12 <3=>PH2
#define RTE_SAI2_SCK_B_PORT_ID          0
#if    (RTE_SAI2_SCK_B_PORT_ID == 0)
#define RTE_SAI2_SCK_B_PIN              0
#elif  (RTE_SAI2_SCK_B_PORT_ID == 1)
#define RTE_SAI2_SCK_B_PIN              1
#define RTE_SAI2_SCK_B_PORT             GPIOA
#define RTE_SAI2_SCK_B_BIT              2
#elif  (RTE_SAI2_SCK_B_PORT_ID == 2)
#define RTE_SAI2_SCK_B_PIN              1
#define RTE_SAI2_SCK_B_PORT             GPIOE
#define RTE_SAI2_SCK_B_BIT              12
#elif  (RTE_SAI2_SCK_B_PORT_ID == 3)
#define RTE_SAI2_SCK_B_PIN              1
#define RTE_SAI2_SCK_B_PORT             GPIOH
#define RTE_SAI2_SCK_B_BIT              2
#else
#error "Invalid SAI2_SCK_B Pin Configuration!"
#endif

//   <o> SAI2_MCLK_B Pin <0=>Not Used <1=>PA1 <2=>PE6 <3=>PE14 <4=>PH3
#define RTE_SAI2_MCLK_B_PORT_ID         0
#if    (RTE_SAI2_MCLK_B_PORT_ID == 0)
#define RTE_SAI2_MCLK_B_PIN             0
#elif  (RTE_SAI2_MCLK_B_PORT_ID == 1)
#define RTE_SAI2_MCLK_B_PIN             1
#define RTE_SAI2_MCLK_B_PORT            GPIOA
#define RTE_SAI2_MCLK_B_BIT             1
#elif  (RTE_SAI2_MCLK_B_PORT_ID == 2)
#define RTE_SAI2_MCLK_B_PIN             1
#define RTE_SAI2_MCLK_B_PORT            GPIOE
#define RTE_SAI2_MCLK_B_BIT             6
#elif  (RTE_SAI2_MCLK_B_PORT_ID == 3)
#define RTE_SAI2_MCLK_B_PIN             1
#define RTE_SAI2_MCLK_B_PORT            GPIOE
#define RTE_SAI2_MCLK_B_BIT             14
#elif  (RTE_SAI2_MCLK_B_PORT_ID == 4)
#define RTE_SAI2_MCLK_B_PIN             1
#define RTE_SAI2_MCLK_B_PORT            GPIOH
#define RTE_SAI2_MCLK_B_BIT             3
#else
#error "Invalid SAI2_MCLK_B Pin Configuration!"
#endif

//   <e> DMA SAI2_A
//     <o1> Number <2=>2
//     <i>  Selects DMA Number (only DMA2 can be used)
//     <o2> Stream <4=>4
//     <i>  Selects DMA Stream (only Stream 4 can be used)
//     <o3> Channel <3=>3
//     <i>  Selects DMA Channel (only Channel 3 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_SAI2_A_DMA                  0
#define RTE_SAI2_A_DMA_NUMBER           2
#define RTE_SAI2_A_DMA_STREAM           4
#define RTE_SAI2_A_DMA_CHANNEL          3
#define RTE_SAI2_A_DMA_PRIORITY         0

//   <e> DMA SAI2_B
//     <o1> Number <2=>2
//     <i>  Selects DMA Number (only DMA2 can be used)
//     <o2> Stream <6=>6 <7=>7
//     <i>  Selects DMA Stream (only Stream 6 or 7 can be used)
//     <o3> Channel <0=>0 <3=>3
//     <i>  Selects DMA Channel (only Channel 0 or 3 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_SAI2_B_DMA                  0
#define RTE_SAI2_B_DMA_NUMBER           2
#define RTE_SAI2_B_DMA_STREAM           7
#define RTE_SAI2_B_DMA_CHANNEL          0
#define RTE_SAI2_B_DMA_PRIORITY         0

// </e>


// <e> SDMMC1 (SD/SDIO/MMC card host interface 1) [Driver_MCI0]
// <i> Configuration settings for Driver_MCI0 in component ::CMSIS Driver:MCI
#define RTE_SDMMC1                      0

//   <h> SDMMC Peripheral Bus
//     <o> SDMMC1_CK Pin <0=>PC12
#define   RTE_SDMMC1_CK_PORT_ID         0
#if      (RTE_SDMMC1_CK_PORT_ID == 0)
  #define RTE_SDMMC1_CK_PORT            GPIOC
  #define RTE_SDMMC1_CK_PIN             GPIO_PIN_12
#else
  #error "Invalid SDMMC1_CLK Pin Configuration!"
#endif
//     <o> SDMMC1_CMD Pin <0=>PD2
#define   RTE_SDMMC1_CMD_PORT_ID        0
#if      (RTE_SDMMC1_CMD_PORT_ID == 0)
  #define RTE_SDMMC1_CMD_PORT           GPIOD
  #define RTE_SDMMC1_CMD_PIN            GPIO_PIN_2
#else
  #error "Invalid SDMMC1_CMD Pin Configuration!"
#endif
//     <o> SDMMC1_D0 Pin <0=>PC8
#define   RTE_SDMMC1_D0_PORT_ID         0
#if      (RTE_SDMMC1_D0_PORT_ID == 0)
  #define RTE_SDMMC1_D0_PORT            GPIOC
  #define RTE_SDMMC1_D0_PIN             GPIO_PIN_8
#else
  #error "Invalid SDMMC1_D0 Pin Configuration!"
#endif
//     <e> SDMMC1_D[1 .. 3]
#define   RTE_SDMMC1_BUS_WIDTH_4        1
//       <o> SDMMC1_D1 Pin <0=>PC9
#define   RTE_SDMMC1_D1_PORT_ID         0
#if      (RTE_SDMMC1_D1_PORT_ID == 0)
  #define RTE_SDMMC1_D1_PORT            GPIOC
  #define RTE_SDMMC1_D1_PIN             GPIO_PIN_9
#else
  #error "Invalid SDMMC1_D1 Pin Configuration!"
#endif
//       <o> SDMMC1_D2 Pin <0=>PC10
#define   RTE_SDMMC1_D2_PORT_ID         0
#if      (RTE_SDMMC1_D2_PORT_ID == 0)
  #define RTE_SDMMC1_D2_PORT            GPIOC
  #define RTE_SDMMC1_D2_PIN             GPIO_PIN_10
#else
  #error "Invalid SDMMC1_D2 Pin Configuration!"
#endif
//       <o> SDMMC1_D3 Pin <0=>PC11
#define   RTE_SDMMC1_D3_PORT_ID         0
#if      (RTE_SDMMC1_D3_PORT_ID == 0)
  #define RTE_SDMMC1_D3_PORT            GPIOC
  #define RTE_SDMMC1_D3_PIN             GPIO_PIN_11
#else
  #error "Invalid SDMMC1_D3 Pin Configuration!"
#endif
//     </e> SDMMC1_D[1 .. 3]
//     <e> SDMMC1_D[4 .. 7]
#define   RTE_SDMMC1_BUS_WIDTH_8        0
//       <o> SDMMC1_D4 Pin <0=>PB8
#define   RTE_SDMMC1_D4_PORT_ID         0
#if      (RTE_SDMMC1_D4_PORT_ID == 0)
  #define RTE_SDMMC1_D4_PORT            GPIOB
  #define RTE_SDMMC1_D4_PIN             GPIO_PIN_8
#else
  #error "Invalid SDMMC1_D4 Pin Configuration!"
#endif
//       <o> SDMMC1_D5 Pin <0=>PB9
#define   RTE_SDMMC1_D5_PORT_ID         0
#if      (RTE_SDMMC1_D5_PORT_ID == 0)
  #define RTE_SDMMC1_D5_PORT            GPIOB
  #define RTE_SDMMC1_D5_PIN             GPIO_PIN_9
#else
  #error "Invalid SDMMC1_D5 Pin Configuration!"
#endif
//       <o> SDMMC1_D6 Pin <0=>PC6
#define   RTE_SDMMC1_D6_PORT_ID         0
#if      (RTE_SDMMC1_D6_PORT_ID == 0)
  #define RTE_SDMMC1_D6_PORT            GPIOC
  #define RTE_SDMMC1_D6_PIN             GPIO_PIN_6
#else
  #error "Invalid SDMMC1_D6 Pin Configuration!"
#endif
//       <o> SDMMC1_D7 Pin <0=>PC7
#define   RTE_SDMMC1_D7_PORT_ID         0
#if      (RTE_SDMMC1_D7_PORT_ID == 0)
  #define RTE_SDMMC1_D7_PORT            GPIOC
  #define RTE_SDMMC1_D7_PIN             GPIO_PIN_7
#else
  #error "Invalid SDMMC1_D7 Pin Configuration!"
#endif
//     </e> SDMMC1_D[4 .. 7]
//   </h> SDMMC Peripheral Bus

//   <e> Card Detect Pin
//   <i> Configure Pin if exists
//   <i> GPIO Pxy (x = A..H, y = 0..15) or (x = I, y = 0..11)
//     <o1> Active State <0=>Low <1=>High
//     <i>  Selects Active State Logical Level
//     <o2> Port <0=>GPIOA <1=>GPIOB <2=>GPIOC <3=>GPIOD
//               <4=>GPIOE <5=>GPIOF <6=>GPIOG <7=>GPIOH <8=>GPIOI
//     <i>  Selects Port Name
//     <o3> Bit <0-15>
//     <i>  Selects Port Bit
//     <o4> Pull Resistor <0=>Inactive <1=>Pull-up <2=>Pull-down
//     <i>  Select Pin Pull Resistor function
//   </e>
#define RTE_SDMMC1_CD_PIN_EN            0
#define RTE_SDMMC1_CD_ACTIVE            0
#define RTE_SDMMC1_CD_PORT              GPIO_PORT(0)
#define RTE_SDMMC1_CD_PIN               0
#define RTE_SDMMC1_CD_PULL              1

//   <e> Write Protect Pin
//   <i> Configure Pin if exists
//   <i> GPIO Pxy (x = A..H, y = 0..15) or (x = I, y = 0..11)
//     <o1> Active State <0=>Low <1=>High
//     <i>  Selects Active State Logical Level
//     <o2> Port <0=>GPIOA <1=>GPIOB <2=>GPIOC <3=>GPIOD
//               <4=>GPIOE <5=>GPIOF <6=>GPIOG <7=>GPIOH <8=>GPIOI
//     <i>  Selects Port Name
//     <o3> Bit <0-15>
//     <i>  Selects Port Bit
//     <o4> Pull Resistor <0=>Inactive <1=>Pull-up <2=>Pull-down
//     <i>  Select Pin Pull Resistor function
//   </e>
#define RTE_SDMMC1_WP_EN                0
#define RTE_SDMMC1_WP_ACTIVE            0
#define RTE_SDMMC1_WP_PORT              GPIO_PORT(0)
#define RTE_SDMMC1_WP_PIN               0
#define RTE_SDMMC1_WP_PULL              1

//   <e> DMA Rx
//     <o1> Number <2=>2
//     <i>  Selects DMA Number (only DMA2 can be used)
//     <o2> Stream <3=>3 <6=>6
//     <i>  Selects DMA Stream (only Stream 3 or 6 can be used)
//     <o3> Channel <4=>4
//     <i>  Selects DMA Channel (only Channel 4 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_SDMMC1_RX_DMA               1
#define RTE_SDMMC1_RX_DMA_NUMBER        2
#define RTE_SDMMC1_RX_DMA_STREAM        3
#define RTE_SDMMC1_RX_DMA_CHANNEL       4
#define RTE_SDMMC1_RX_DMA_PRIORITY      0

//   <e> DMA Tx
//     <o1> Number <2=>2
//     <i>  Selects DMA Number (only DMA2 can be used)
//     <o2> Stream <3=>3 <6=>6
//     <i>  Selects DMA Stream (only Stream 3 or 6 can be used)
//     <o3> Channel <4=>4
//     <i>  Selects DMA Channel (only Channel 4 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_SDMMC1_TX_DMA               1
#define RTE_SDMMC1_TX_DMA_NUMBER        2
#define RTE_SDMMC1_TX_DMA_STREAM        6
#define RTE_SDMMC1_TX_DMA_CHANNEL       4
#define RTE_SDMMC1_TX_DMA_PRIORITY      0

// </e>


// <e> SDMMC2 (SD/SDIO/MMC card host interface 2) [Driver_MCI1]
// <i> Configuration settings for Driver_MCI1 in component ::CMSIS Driver:MCI
#define RTE_SDMMC2                      0

//   <h> SDMMC Peripheral Bus
//     <o> SDMMC2_CK Pin <0=>PD6
#define   RTE_SDMMC2_CK_PORT_ID         0
#if      (RTE_SDMMC2_CK_PORT_ID == 0)
  #define RTE_SDMMC2_CK_PORT            GPIOD
  #define RTE_SDMMC2_CK_PIN             GPIO_PIN_6
  #define RTE_SDMMC2_CK_AF              GPIO_AF11_SDMMC2
#else
  #error "Invalid SDMMC2_CLK Pin Configuration!"
#endif
//     <o> SDMMC2_CMD Pin <0=>PD7
#define   RTE_SDMMC2_CMD_PORT_ID        0
#if      (RTE_SDMMC2_CMD_PORT_ID == 0)
  #define RTE_SDMMC2_CMD_PORT           GPIOD
  #define RTE_SDMMC2_CMD_PIN            GPIO_PIN_7
  #define RTE_SDMMC2_CMD_AF             GPIO_AF11_SDMMC2
#else
  #error "Invalid SDMMC2_CMD Pin Configuration!"
#endif
//     <o> SDMMC2_D0 Pin <0=>PB14 <1=>PG9
#define   RTE_SDMMC2_D0_PORT_ID         0
#if      (RTE_SDMMC2_D0_PORT_ID == 0)
  #define RTE_SDMMC2_D0_PORT            GPIOB
  #define RTE_SDMMC2_D0_PIN             GPIO_PIN_14
  #define RTE_SDMMC2_D0_AF              GPIO_AF10_SDMMC2
#elif    (RTE_SDMMC2_D0_PORT_ID == 1)
  #define RTE_SDMMC2_D0_PORT            GPIOG
  #define RTE_SDMMC2_D0_PIN             GPIO_PIN_9
  #define RTE_SDMMC2_D0_AF              GPIO_AF11_SDMMC2
#else
  #error "Invalid SDMMC2_D0 Pin Configuration!"
#endif
//     <e> SDMMC2_D[1 .. 3]
#define   RTE_SDMMC2_BUS_WIDTH_4        1
//       <o> SDMMC2_D1 Pin <0=>PB15 <1=>PG10
#define   RTE_SDMMC2_D1_PORT_ID         0
#if      (RTE_SDMMC2_D1_PORT_ID == 0)
  #define RTE_SDMMC2_D1_PORT            GPIOB
  #define RTE_SDMMC2_D1_PIN             GPIO_PIN_15
  #define RTE_SDMMC2_D1_AF              GPIO_AF10_SDMMC2
#elif    (RTE_SDMMC2_D1_PORT_ID == 1)
  #define RTE_SDMMC2_D1_PORT            GPIOG
  #define RTE_SDMMC2_D1_PIN             GPIO_PIN_10
  #define RTE_SDMMC2_D1_AF              GPIO_AF11_SDMMC2
#else
  #error "Invalid SDMMC2_D1 Pin Configuration!"
#endif
//       <o> SDMMC2_D2 Pin <0=>PB3 <1=>PG11
#define   RTE_SDMMC2_D2_PORT_ID         0
#if      (RTE_SDMMC2_D2_PORT_ID == 0)
  #define RTE_SDMMC2_D2_PORT            GPIOB
  #define RTE_SDMMC2_D2_PIN             GPIO_PIN_3
  #define RTE_SDMMC2_D2_AF              GPIO_AF10_SDMMC2
#elif    (RTE_SDMMC2_D2_PORT_ID == 1)
  #define RTE_SDMMC2_D2_PORT            GPIOG
  #define RTE_SDMMC2_D2_PIN             GPIO_PIN_11
  #define RTE_SDMMC2_D2_AF              GPIO_AF10_SDMMC2
#else
  #error "Invalid SDMMC2_D2 Pin Configuration!"
#endif
//       <o> SDMMC2_D3 Pin <0=>PB4 <1=>PG12
#define   RTE_SDMMC2_D3_PORT_ID         0
#if      (RTE_SDMMC2_D3_PORT_ID == 0)
  #define RTE_SDMMC2_D3_PORT            GPIOB
  #define RTE_SDMMC2_D3_PIN             GPIO_PIN_4
  #define RTE_SDMMC2_D3_AF              GPIO_AF10_SDMMC2
#elif    (RTE_SDMMC2_D3_PORT_ID == 1)
  #define RTE_SDMMC2_D3_PORT            GPIOG
  #define RTE_SDMMC2_D3_PIN             GPIO_PIN_12
  #define RTE_SDMMC2_D3_AF              GPIO_AF11_SDMMC2
#else
  #error "Invalid SDMMC2_D3 Pin Configuration!"
#endif
//     </e> SDMMC2_D[1 .. 3]
//     <e> SDMMC2_D[4 .. 7]
#define   RTE_SDMMC2_BUS_WIDTH_8        0
//       <o> SDMMC2_D4 Pin <0=>PB8
#define   RTE_SDMMC2_D4_PORT_ID         0
#if      (RTE_SDMMC2_D4_PORT_ID == 0)
  #define RTE_SDMMC2_D4_PORT            GPIOB
  #define RTE_SDMMC2_D4_PIN             GPIO_PIN_8
  #define RTE_SDMMC2_D4_AF              GPIO_AF10_SDMMC2
#else
  #error "Invalid SDMMC2_D4 Pin Configuration!"
#endif
//       <o> SDMMC2_D5 Pin <0=>PB9
#define   RTE_SDMMC2_D5_PORT_ID         0
#if      (RTE_SDMMC2_D5_PORT_ID == 0)
  #define RTE_SDMMC2_D5_PORT            GPIOB
  #define RTE_SDMMC2_D5_PIN             GPIO_PIN_9
  #define RTE_SDMMC2_D5_AF              GPIO_AF10_SDMMC2
#else
  #error "Invalid SDMMC2_D5 Pin Configuration!"
#endif
//       <o> SDMMC2_D6 Pin <0=>PC6
#define   RTE_SDMMC2_D6_PORT_ID         0
#if      (RTE_SDMMC2_D6_PORT_ID == 0)
  #define RTE_SDMMC2_D6_PORT            GPIOC
  #define RTE_SDMMC2_D6_PIN             GPIO_PIN_6
  #define RTE_SDMMC2_D6_AF              GPIO_AF10_SDMMC2
#else
  #error "Invalid SDMMC2_D6 Pin Configuration!"
#endif
//       <o> SDMMC2_D7 Pin <0=>PC7
#define   RTE_SDMMC2_D7_PORT_ID         0
#if      (RTE_SDMMC2_D7_PORT_ID == 0)
  #define RTE_SDMMC2_D7_PORT            GPIOC
  #define RTE_SDMMC2_D7_PIN             GPIO_PIN_7
  #define RTE_SDMMC2_D7_AF              GPIO_AF10_SDMMC2
#else
  #error "Invalid SDMMC2_D7 Pin Configuration!"
#endif
//     </e> SDMMC2_D[4 .. 7]
//   </h> SDMMC Peripheral Bus

//   <e> Card Detect Pin
//   <i> Configure Pin if exists
//   <i> GPIO Pxy (x = A..H, y = 0..15) or (x = I, y = 0..11)
//     <o1> Active State <0=>Low <1=>High
//     <i>  Selects Active State Logical Level
//     <o2> Port <0=>GPIOA <1=>GPIOB <2=>GPIOC <3=>GPIOD
//               <4=>GPIOE <5=>GPIOF <6=>GPIOG <7=>GPIOH <8=>GPIOI
//     <i>  Selects Port Name
//     <o3> Bit <0-15>
//     <i>  Selects Port Bit
//     <o4> Pull Resistor <0=>Inactive <1=>Pull-up <2=>Pull-down
//     <i>  Select Pin Pull Resistor function
//   </e>
#define RTE_SDMMC2_CD_PIN_EN            1
#define RTE_SDMMC2_CD_ACTIVE            0
#define RTE_SDMMC2_CD_PORT              GPIO_PORT(2)
#define RTE_SDMMC2_CD_PIN               12
#define RTE_SDMMC2_CD_PULL              1

//   <e> Write Protect Pin
//   <i> Configure Pin if exists
//   <i> GPIO Pxy (x = A..H, y = 0..15) or (x = I, y = 0..11)
//     <o1> Active State <0=>Low <1=>High
//     <i>  Selects Active State Logical Level
//     <o2> Port <0=>GPIOA <1=>GPIOB <2=>GPIOC <3=>GPIOD
//               <4=>GPIOE <5=>GPIOF <6=>GPIOG <7=>GPIOH <8=>GPIOI
//     <i>  Selects Port Name
//     <o3> Bit <0-15>
//     <i>  Selects Port Bit
//     <o4> Pull Resistor <0=>Inactive <1=>Pull-up <2=>Pull-down
//     <i>  Select Pin Pull Resistor function
//   </e>
#define RTE_SDMMC2_WP_EN                0
#define RTE_SDMMC2_WP_ACTIVE            0
#define RTE_SDMMC2_WP_PORT              GPIO_PORT(0)
#define RTE_SDMMC2_WP_PIN               0
#define RTE_SDMMC2_WP_PULL              1

//   <e> DMA Rx
//     <o1> Number <2=>2
//     <i>  Selects DMA Number (only DMA2 can be used)
//     <o2> Stream <0=>0 <5=>5
//     <i>  Selects DMA Stream (only Stream 0 or 5 can be used)
//     <o3> Channel <11=>11
//     <i>  Selects DMA Channel (only Channel 11 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_SDMMC2_RX_DMA               1
#define RTE_SDMMC2_RX_DMA_NUMBER        2
#define RTE_SDMMC2_RX_DMA_STREAM        0
#define RTE_SDMMC2_RX_DMA_CHANNEL       11
#define RTE_SDMMC2_RX_DMA_PRIORITY      0

//   <e> DMA Tx
//     <o1> Number <2=>2
//     <i>  Selects DMA Number (only DMA2 can be used)
//     <o2> Stream <0=>0 <5=>5
//     <i>  Selects DMA Stream (only Stream 0 or 5 can be used)
//     <o3> Channel <11=>11
//     <i>  Selects DMA Channel (only Channel 11 can be used)
//     <o4> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </e>
#define RTE_SDMMC2_TX_DMA               1
#define RTE_SDMMC2_TX_DMA_NUMBER        2
#define RTE_SDMMC2_TX_DMA_STREAM        5
#define RTE_SDMMC2_TX_DMA_CHANNEL       11
#define RTE_SDMMC2_TX_DMA_PRIORITY      0

// </e>


// <e> CAN1 (Controller Area Network 1) [Driver_CAN1]
// <i> Configuration settings for Driver_CAN1 in component ::CMSIS Driver:CAN
#define RTE_CAN1                        0

//   <o> CAN1_RX Pin <0=>PA11 <1=>PB8 <2=>PD0 <3=>PI9 <4=>PH14
#define RTE_CAN1_RX_PORT_ID             0
#if    (RTE_CAN1_RX_PORT_ID == 0)
#define RTE_CAN1_RX_PORT                GPIOA
#define RTE_CAN1_RX_BIT                 GPIO_PIN_11
#elif  (RTE_CAN1_RX_PORT_ID == 1)
#define RTE_CAN1_RX_PORT                GPIOB
#define RTE_CAN1_RX_BIT                 GPIO_PIN_8
#elif  (RTE_CAN1_RX_PORT_ID == 2)
#define RTE_CAN1_RX_PORT                GPIOD
#define RTE_CAN1_RX_BIT                 GPIO_PIN_0
#elif  (RTE_CAN1_RX_PORT_ID == 3)
#define RTE_CAN1_RX_PORT                GPIOI
#define RTE_CAN1_RX_BIT                 GPIO_PIN_9
#elif  (RTE_CAN1_RX_PORT_ID == 4)
#define RTE_CAN1_RX_PORT                GPIOH
#define RTE_CAN1_RX_BIT                 GPIO_PIN_14
#else
#error "Invalid CAN1_RX Pin Configuration!"
#endif

//   <o> CAN1_TX Pin <0=>PA12 <1=>PB9 <2=>PD1 <3=>PH13
#define RTE_CAN1_TX_PORT_ID             0
#if    (RTE_CAN1_TX_PORT_ID == 0)
#define RTE_CAN1_TX_PORT                GPIOA
#define RTE_CAN1_TX_BIT                 GPIO_PIN_12
#elif  (RTE_CAN1_TX_PORT_ID == 1)
#define RTE_CAN1_TX_PORT                GPIOB
#define RTE_CAN1_TX_BIT                 GPIO_PIN_9
#elif  (RTE_CAN1_TX_PORT_ID == 2)
#define RTE_CAN1_TX_PORT                GPIOD
#define RTE_CAN1_TX_BIT                 GPIO_PIN_1
#elif  (RTE_CAN1_TX_PORT_ID == 3)
#define RTE_CAN1_TX_PORT                GPIOH
#define RTE_CAN1_TX_BIT                 GPIO_PIN_13
#else
#error "Invalid CAN1_TX Pin Configuration!"
#endif

// </e>


// <e> CAN2 (Controller Area Network 2) [Driver_CAN2]
// <i> Configuration settings for Driver_CAN2 in component ::CMSIS Driver:CAN
#define RTE_CAN2                        0

//   <o> CAN2_RX Pin <0=>PB5 <1=>PB12
#define RTE_CAN2_RX_PORT_ID             0
#if    (RTE_CAN2_RX_PORT_ID == 0)
#define RTE_CAN2_RX_PORT                GPIOB
#define RTE_CAN2_RX_BIT                 GPIO_PIN_5
#elif  (RTE_CAN2_RX_PORT_ID == 1)
#define RTE_CAN2_RX_PORT                GPIOB
#define RTE_CAN2_RX_BIT                 GPIO_PIN_12
#else
#error "Invalid CAN2_RX Pin Configuration!"
#endif

//   <o> CAN2_TX Pin <0=>PB6 <1=>PB13
#define RTE_CAN2_TX_PORT_ID             0
#if    (RTE_CAN2_TX_PORT_ID == 0)
#define RTE_CAN2_TX_PORT                GPIOB
#define RTE_CAN2_TX_BIT                 GPIO_PIN_6
#elif  (RTE_CAN2_TX_PORT_ID == 1)
#define RTE_CAN2_TX_PORT                GPIOB
#define RTE_CAN2_TX_BIT                 GPIO_PIN_13
#else
#error "Invalid CAN2_TX Pin Configuration!"
#endif

// </e>


// <e> CAN3 (Controller Area Network 3) [Driver_CAN3]
// <i> Configuration settings for Driver_CAN3 in component ::CMSIS Driver:CAN
// <i> Available only on STM32F76x and STM32F77x device series
#define RTE_CAN3                        0

//   <o> CAN3_RX Pin <0=>PA8 <1=>PB3
#define RTE_CAN3_RX_PORT_ID             0
#if    (RTE_CAN3_RX_PORT_ID == 0)
#define RTE_CAN3_RX_PORT                GPIOA
#define RTE_CAN3_RX_BIT                 GPIO_PIN_8
#elif  (RTE_CAN3_RX_PORT_ID == 1)
#define RTE_CAN3_RX_PORT                GPIOB
#define RTE_CAN3_RX_BIT                 GPIO_PIN_3
#else
#error "Invalid CAN3_RX Pin Configuration!"
#endif

//   <o> CAN3_TX Pin <0=>PA15 <1=>PB4
#define RTE_CAN3_TX_PORT_ID             0
#if    (RTE_CAN3_TX_PORT_ID == 0)
#define RTE_CAN3_TX_PORT                GPIOA
#define RTE_CAN3_TX_BIT                 GPIO_PIN_15
#elif  (RTE_CAN3_TX_PORT_ID == 1)
#define RTE_CAN3_TX_PORT                GPIOB
#define RTE_CAN3_TX_BIT                 GPIO_PIN_4
#else
#error "Invalid CAN3_TX Pin Configuration!"
#endif

// </e>


// <e> ETH (Ethernet Interface) [Driver_ETH_MAC0]
// <i> Configuration settings for Driver_ETH_MAC0 in component ::CMSIS Driver:Ethernet MAC
#define RTE_ETH                         0

//   <e> MII (Media Independent Interface)
#define RTE_ETH_MII                     1

//     <o> ETH_MII_TX_CLK Pin <0=>PC3
#define RTE_ETH_MII_TX_CLK_PORT_ID      0
#if    (RTE_ETH_MII_TX_CLK_PORT_ID == 0)
#define RTE_ETH_MII_TX_CLK_PORT         GPIOC
#define RTE_ETH_MII_TX_CLK_PIN          3
#else
#error "Invalid ETH_MII_TX_CLK Pin Configuration!"
#endif
//     <o> ETH_MII_TXD0 Pin <0=>PB12 <1=>PG13
#define RTE_ETH_MII_TXD0_PORT_ID        0
#if    (RTE_ETH_MII_TXD0_PORT_ID == 0)
#define RTE_ETH_MII_TXD0_PORT           GPIOB
#define RTE_ETH_MII_TXD0_PIN            12
#elif  (RTE_ETH_MII_TXD0_PORT_ID == 1)
#define RTE_ETH_MII_TXD0_PORT           GPIOG
#define RTE_ETH_MII_TXD0_PIN            13
#else
#error "Invalid ETH_MII_TXD0 Pin Configuration!"
#endif
//     <o> ETH_MII_TXD1 Pin <0=>PB13 <1=>PG14
#define RTE_ETH_MII_TXD1_PORT_ID        0
#if    (RTE_ETH_MII_TXD1_PORT_ID == 0)
#define RTE_ETH_MII_TXD1_PORT           GPIOB
#define RTE_ETH_MII_TXD1_PIN            13
#elif  (RTE_ETH_MII_TXD1_PORT_ID == 1)
#define RTE_ETH_MII_TXD1_PORT           GPIOG
#define RTE_ETH_MII_TXD1_PIN            14
#else
#error "Invalid ETH_MII_TXD1 Pin Configuration!"
#endif
//     <o> ETH_MII_TXD2 Pin <0=>PC2
#define RTE_ETH_MII_TXD2_PORT_ID        0
#if    (RTE_ETH_MII_TXD2_PORT_ID == 0)
#define RTE_ETH_MII_TXD2_PORT           GPIOC
#define RTE_ETH_MII_TXD2_PIN            2
#else
#error "Invalid ETH_MII_TXD2 Pin Configuration!"
#endif
//     <o> ETH_MII_TXD3 Pin <0=>PB8 <1=>PE2
#define RTE_ETH_MII_TXD3_PORT_ID        0
#if    (RTE_ETH_MII_TXD3_PORT_ID == 0)
#define RTE_ETH_MII_TXD3_PORT           GPIOB
#define RTE_ETH_MII_TXD3_PIN            8
#elif  (RTE_ETH_MII_TXD3_PORT_ID == 1)
#define RTE_ETH_MII_TXD3_PORT           GPIOE
#define RTE_ETH_MII_TXD3_PIN            2
#else
#error "Invalid ETH_MII_TXD3 Pin Configuration!"
#endif
//     <o> ETH_MII_TX_EN Pin <0=>PB11 <1=>PG11
#define RTE_ETH_MII_TX_EN_PORT_ID       0
#if    (RTE_ETH_MII_TX_EN_PORT_ID == 0)
#define RTE_ETH_MII_TX_EN_PORT          GPIOB
#define RTE_ETH_MII_TX_EN_PIN           11
#elif  (RTE_ETH_MII_TX_EN_PORT_ID == 1)
#define RTE_ETH_MII_TX_EN_PORT          GPIOG
#define RTE_ETH_MII_TX_EN_PIN           11
#else
#error "Invalid ETH_MII_TX_EN Pin Configuration!"
#endif
//     <o> ETH_MII_RX_CLK Pin <0=>PA1
#define RTE_ETH_MII_RX_CLK_PORT_ID        0
#if    (RTE_ETH_MII_RX_CLK_PORT_ID == 0)
#define RTE_ETH_MII_RX_CLK_PORT         GPIOA
#define RTE_ETH_MII_RX_CLK_PIN          1
#else
#error "Invalid ETH_MII_RX_CLK Pin Configuration!"
#endif
//     <o> ETH_MII_RXD0 Pin <0=>PC4
#define RTE_ETH_MII_RXD0_PORT_ID        0
#if    (RTE_ETH_MII_RXD0_PORT_ID == 0)
#define RTE_ETH_MII_RXD0_PORT           GPIOC
#define RTE_ETH_MII_RXD0_PIN            4
#else
#error "Invalid ETH_MII_RXD0 Pin Configuration!"
#endif
//     <o> ETH_MII_RXD1 Pin <0=>PC5
#define RTE_ETH_MII_RXD1_PORT_ID        0
#if    (RTE_ETH_MII_RXD1_PORT_ID == 0)
#define RTE_ETH_MII_RXD1_PORT           GPIOC
#define RTE_ETH_MII_RXD1_PIN            5
#else
#error "Invalid ETH_MII_RXD1 Pin Configuration!"
#endif
//     <o> ETH_MII_RXD2 Pin <0=>PB0 <1=>PH6
#define RTE_ETH_MII_RXD2_PORT_ID        0
#if    (RTE_ETH_MII_RXD2_PORT_ID == 0)
#define RTE_ETH_MII_RXD2_PORT           GPIOB
#define RTE_ETH_MII_RXD2_PIN            0
#elif  (RTE_ETH_MII_RXD2_PORT_ID == 1)
#define RTE_ETH_MII_RXD2_PORT           GPIOH
#define RTE_ETH_MII_RXD2_PIN            6
#else
#error "Invalid ETH_MII_RXD2 Pin Configuration!"
#endif
//     <o> ETH_MII_RXD3 Pin <0=>PB1 <1=>PH7
#define RTE_ETH_MII_RXD3_PORT_ID        0
#if    (RTE_ETH_MII_RXD3_PORT_ID == 0)
#define RTE_ETH_MII_RXD3_PORT           GPIOB
#define RTE_ETH_MII_RXD3_PIN            1
#elif  (RTE_ETH_MII_RXD3_PORT_ID == 1)
#define RTE_ETH_MII_RXD3_PORT           GPIOH
#define RTE_ETH_MII_RXD3_PIN            7
#else
#error "Invalid ETH_MII_RXD3 Pin Configuration!"
#endif
//     <o> ETH_MII_RX_DV Pin <0=>PA7
#define RTE_ETH_MII_RX_DV_PORT_ID       0
#if    (RTE_ETH_MII_RX_DV_PORT_ID == 0)
#define RTE_ETH_MII_RX_DV_PORT          GPIOA
#define RTE_ETH_MII_RX_DV_PIN           7
#else
#error "Invalid ETH_MII_RX_DV Pin Configuration!"
#endif
//     <o> ETH_MII_RX_ER Pin <0=>Not Used <1=>PB10 <2=>PI10
#define RTE_ETH_MII_RX_ER_PORT_ID       0
#if    (RTE_ETH_MII_RX_ER_PORT_ID == 0)
#define RTE_ETH_MII_RX_ER_PORT          NULL
#elif  (RTE_ETH_MII_RX_ER_PORT_ID == 1)
#define RTE_ETH_MII_RX_ER_PORT          GPIOB
#define RTE_ETH_MII_RX_ER_PIN           10
#elif  (RTE_ETH_MII_RX_ER_PORT_ID == 2)
#define RTE_ETH_MII_RX_ER_PORT          GPIOI
#define RTE_ETH_MII_RX_ER_PIN           10
#else
#error "Invalid ETH_MII_RX_ER Pin Configuration!"
#endif
//     <o> ETH_MII_CRS Pin <0=>Not Used <1=>PA0 <2=>PH2
#define RTE_ETH_MII_CRS_PORT_ID         0
#if    (RTE_ETH_MII_CRS_PORT_ID == 0)
#define RTE_ETH_MII_CRS_PORT            NULL
#elif  (RTE_ETH_MII_CRS_PORT_ID == 1)
#define RTE_ETH_MII_CRS_PORT            GPIOA
#define RTE_ETH_MII_CRS_PIN             0
#elif  (RTE_ETH_MII_CRS_PORT_ID == 2)
#define RTE_ETH_MII_CRS_PORT            GPIOH
#define RTE_ETH_MII_CRS_PIN             2
#else
#error "Invalid ETH_MII_CRS Pin Configuration!"
#endif
//     <o> ETH_MII_COL Pin <0=>Not Used <1=>PA3 <2=>PH3
#define RTE_ETH_MII_COL_PORT_ID         0
#if    (RTE_ETH_MII_COL_PORT_ID == 0)
#define RTE_ETH_MII_COL_PORT            NULL
#elif  (RTE_ETH_MII_COL_PORT_ID == 1)
#define RTE_ETH_MII_COL_PORT            GPIOA
#define RTE_ETH_MII_COL_PIN             3
#elif  (RTE_ETH_MII_COL_PORT_ID == 2)
#define RTE_ETH_MII_COL_PORT            GPIOH
#define RTE_ETH_MII_COL_PIN             3
#else
#error "Invalid ETH_MII_COL Pin Configuration!"
#endif

//   </e>

//   <e> RMII (Reduced Media Independent Interface)
#define RTE_ETH_RMII                    0

//     <o> ETH_RMII_TXD0 Pin <0=>PB12 <1=>PG13
#define RTE_ETH_RMII_TXD0_PORT_ID       0
#if    (RTE_ETH_RMII_TXD0_PORT_ID == 0)
#define RTE_ETH_RMII_TXD0_PORT          GPIOB
#define RTE_ETH_RMII_TXD0_PIN           12
#elif  (RTE_ETH_RMII_TXD0_PORT_ID == 1)
#define RTE_ETH_RMII_TXD0_PORT          GPIOG
#define RTE_ETH_RMII_TXD0_PIN           13
#else
#error "Invalid ETH_RMII_TXD0 Pin Configuration!"
#endif
//     <o> ETH_RMII_TXD1 Pin <0=>PB13 <1=>PG14
#define RTE_ETH_RMII_TXD1_PORT_ID       0
#if    (RTE_ETH_RMII_TXD1_PORT_ID == 0)
#define RTE_ETH_RMII_TXD1_PORT          GPIOB
#define RTE_ETH_RMII_TXD1_PIN           13
#elif  (RTE_ETH_RMII_TXD1_PORT_ID == 1)
#define RTE_ETH_RMII_TXD1_PORT          GPIOG
#define RTE_ETH_RMII_TXD1_PIN           14
#else
#error "Invalid ETH_RMII_TXD1 Pin Configuration!"
#endif
//     <o> ETH_RMII_TX_EN Pin <0=>PB11 <1=>PG11
#define RTE_ETH_RMII_TX_EN_PORT_ID      0
#if    (RTE_ETH_RMII_TX_EN_PORT_ID == 0)
#define RTE_ETH_RMII_TX_EN_PORT         GPIOB
#define RTE_ETH_RMII_TX_EN_PIN          11
#elif  (RTE_ETH_RMII_TX_EN_PORT_ID == 1)
#define RTE_ETH_RMII_TX_EN_PORT         GPIOG
#define RTE_ETH_RMII_TX_EN_PIN          11
#else
#error "Invalid ETH_RMII_TX_EN Pin Configuration!"
#endif
//     <o> ETH_RMII_RXD0 Pin <0=>PC4
#define RTE_ETH_RMII_RXD0_PORT_ID       0
#if    (RTE_ETH_RMII_RXD0_PORT_ID == 0)
#define RTE_ETH_RMII_RXD0_PORT          GPIOC
#define RTE_ETH_RMII_RXD0_PIN           4
#else
#error "Invalid ETH_RMII_RXD0 Pin Configuration!"
#endif
//     <o> ETH_RMII_RXD1 Pin <0=>PC5
#define RTE_ETH_RMII_RXD1_PORT_ID       0
#if    (RTE_ETH_RMII_RXD1_PORT_ID == 0)
#define RTE_ETH_RMII_RXD1_PORT          GPIOC
#define RTE_ETH_RMII_RXD1_PIN           5
#else
#error "Invalid ETH_RMII_RXD1 Pin Configuration!"
#endif
//     <o> ETH_RMII_REF_CLK Pin <0=>PA1
#define RTE_ETH_RMII_REF_CLK_PORT_ID    0
#if    (RTE_ETH_RMII_REF_CLK_PORT_ID == 0)
#define RTE_ETH_RMII_REF_CLK_PORT       GPIOA
#define RTE_ETH_RMII_REF_CLK_PIN        1
#else
#error "Invalid ETH_RMII_REF_CLK Pin Configuration!"
#endif
//     <o> ETH_RMII_CRS_DV Pin <0=>PA7
#define RTE_ETH_RMII_CRS_DV_PORT_ID     0
#if    (RTE_ETH_RMII_CRS_DV_PORT_ID == 0)
#define RTE_ETH_RMII_CRS_DV_PORT        GPIOA
#define RTE_ETH_RMII_CRS_DV_PIN         7
#else
#error "Invalid ETH_RMII_CRS_DV Pin Configuration!"
#endif

//   </e>

//   <h> SMI (Station Management Interface)
//     <e> Hardware controlled
//     <i> Enable peripheral controlled SMI on dedicated pins
#define RTE_ETH_SMI_HW                  1
//       <o> ETH_MDC Pin <0=>PC1
#define RTE_ETH_SMI_MDC_PORT_ID         0
#if    (RTE_ETH_SMI_MDC_PORT_ID == 0)
#define RTE_ETH_SMI_MDC_PORT            GPIOC
#define RTE_ETH_SMI_MDC_PIN             1
#else
#error "Invalid ETH_MDC Pin Configuration!"
#endif
//       <o> ETH_MDIO Pin <0=>PA2
#define RTE_ETH_SMI_MDIO_PORT_ID        0
#if    (RTE_ETH_SMI_MDIO_PORT_ID == 0)
#define RTE_ETH_SMI_MDIO_PORT           GPIOA
#define RTE_ETH_SMI_MDIO_PIN            2
#else
#error "Invalid ETH_MDIO Pin Configuration!"
#endif
//     </e>

//     <e> Software controlled
//     <i> Enable software controlled SMI on arbitrary GPIO pins
#define RTE_ETH_SMI_SW                  0
//       <h> ETH_MDC Pin
//       <i> Configure arbitrary GPIO as MDC Pin
//       <i> GPIO Pxy (x = A..H, y = 0..15) or (x = I, y = 0..11)
//         <o0> Port <0=>GPIOA <1=>GPIOB <2=>GPIOC <3=>GPIOD
//                   <4=>GPIOE <5=>GPIOF <6=>GPIOG <7=>GPIOH <8=>GPIOI
//         <i>  Selects Port Name
//         <o1> Bit <0-15>
//         <i>  Selects Port Bit
#define RTE_ETH_SMI_SW_MDC_PORT         GPIO_PORT(6)
#define RTE_ETH_SMI_SW_MDC_PIN          7
//       </h>

//       <h> ETH_MDIO Pin
//       <i> Configure arbitrary GPIO as MDIO Pin
//       <i> GPIO Pxy (x = A..H, y = 0..15) or (x = I, y = 0..11)
//         <o0> Port <0=>GPIOA <1=>GPIOB <2=>GPIOC <3=>GPIOD
//                   <4=>GPIOE <5=>GPIOF <6=>GPIOG <7=>GPIOH <8=>GPIOI
//         <i>  Selects Port Name
//         <o1> Bit <0-15>
//         <i>  Selects Port Bit
#define RTE_ETH_SMI_SW_MDIO_PORT        GPIO_PORT(6)
#define RTE_ETH_SMI_SW_MDIO_PIN         6
//       </h>
//     </e>
//   </h>
//   <o> DMA Descriptor/Buffer Memory Address <0x20000000-0xE0000000>
//   <i> Configure location of the Ethernet DMA Descriptor and Buffer memory
#define RTE_ETH_DMA_MEM_ADDR            0x2000C000

// </e>


// <e> USB OTG Full-speed
#define RTE_USB_OTG_FS                  0

//   <e> Device [Driver_USBD0]
//   <i> Configuration settings for Driver_USBD0 in component ::CMSIS Driver:USB Device

#define RTE_USB_OTG_FS_DEVICE           1

//     <o0.0> VBUS Sensing Pin
//     <i> Enable or disable VBUS sensing
#define RTE_OTG_FS_VBUS_SENSING_PIN     1
//   </e>

//   <e> Host [Driver_USBH0]
//   <i> Configuration settings for Driver_USBH0 in component ::CMSIS Driver:USB Host

#define RTE_USB_OTG_FS_HOST             0

//     <e> VBUS Power On/Off Pin
//     <i> Configure Pin for driving VBUS
//     <i> GPIO Pxy (x = A..J, y = 0..15) or (x = K, y = 0..7)
//       <o1> Active State <0=>Low <1=>High
//       <i>  Selects Active State Logical Level
//       <o2> Port <0=>GPIOA <1=>GPIOB <2=>GPIOC <3=>GPIOD
//                 <4=>GPIOE <5=>GPIOF <6=>GPIOG <7=>GPIOH
//                 <8=>GPIOI <8=>GPIOJ <9=>GPIOK
//       <i>  Selects Port Name
//       <o3> Bit <0-15>
//       <i>  Selects Port Bit
//     </e>
#define RTE_OTG_FS_VBUS_PIN             0
#define RTE_OTG_FS_VBUS_ACTIVE          0
#define RTE_OTG_FS_VBUS_PORT            GPIO_PORT(0)
#define RTE_OTG_FS_VBUS_BIT             0

//     <e> Overcurrent Detection Pin
//     <i> Configure Pin for overcurrent detection
//     <i> GPIO Pxy (x = A..J, y = 0..15) or (x = K, y = 0..7)
//       <o1> Active State <0=>Low <1=>High
//       <i>  Selects Active State Logical Level
//       <o2> Port <0=>GPIOA <1=>GPIOB <2=>GPIOC <3=>GPIOD
//                 <4=>GPIOE <5=>GPIOF <6=>GPIOG <7=>GPIOH
//                 <8=>GPIOI <8=>GPIOJ <9=>GPIOK
//       <i>  Selects Port Name
//       <o3> Bit <0-15>
//       <i>  Selects Port Bit
//     </e>
#define RTE_OTG_FS_OC_PIN               0
#define RTE_OTG_FS_OC_ACTIVE            0
#define RTE_OTG_FS_OC_PORT              GPIO_PORT(0)
#define RTE_OTG_FS_OC_BIT               0
//   </e>

// </e>


// <e> USB OTG High-speed
#define RTE_USB_OTG_HS                  0

//   <h> PHY (Physical Layer)

//     <o> PHY Interface
//       <0=>On-chip full-speed PHY
//       <1=>External ULPI high-speed PHY
//       <2=>Internal UTMI high-speed PHY
#define RTE_USB_OTG_HS_PHY              1

//     <h> External ULPI Pins (UTMI+ Low Pin Interface)

//       <o> OTG_HS_ULPI_CK Pin <0=>PA5
#define RTE_USB_OTG_HS_ULPI_CK_PORT_ID  0
#if    (RTE_USB_OTG_HS_ULPI_CK_PORT_ID == 0)
#define RTE_USB_OTG_HS_ULPI_CK_PORT     GPIOA
#define RTE_USB_OTG_HS_ULPI_CK_PIN      5
#else
#error "Invalid OTG_HS_ULPI_CK Pin Configuration!"
#endif
//       <o> OTG_HS_ULPI_DIR Pin <0=>PI11 <1=>PC2
#define RTE_USB_OTG_HS_ULPI_DIR_PORT_ID 0
#if    (RTE_USB_OTG_HS_ULPI_DIR_PORT_ID == 0)
#define RTE_USB_OTG_HS_ULPI_DIR_PORT    GPIOI
#define RTE_USB_OTG_HS_ULPI_DIR_PIN     11
#elif  (RTE_USB_OTG_HS_ULPI_DIR_PORT_ID == 1)
#define RTE_USB_OTG_HS_ULPI_DIR_PORT    GPIOC
#define RTE_USB_OTG_HS_ULPI_DIR_PIN     2
#else
#error "Invalid OTG_HS_ULPI_DIR Pin Configuration!"
#endif
//       <o> OTG_HS_ULPI_STP Pin <0=>PC0
#define RTE_USB_OTG_HS_ULPI_STP_PORT_ID 0
#if    (RTE_USB_OTG_HS_ULPI_STP_PORT_ID == 0)
#define RTE_USB_OTG_HS_ULPI_STP_PORT    GPIOC
#define RTE_USB_OTG_HS_ULPI_STP_PIN     0
#else
#error "Invalid OTG_HS_ULPI_STP Pin Configuration!"
#endif
//       <o> OTG_HS_ULPI_NXT Pin <0=>PC3 <1=>PH4
#define RTE_USB_OTG_HS_ULPI_NXT_PORT_ID 1
#if    (RTE_USB_OTG_HS_ULPI_NXT_PORT_ID == 0)
#define RTE_USB_OTG_HS_ULPI_NXT_PORT    GPIOC
#define RTE_USB_OTG_HS_ULPI_NXT_PIN     3
#elif  (RTE_USB_OTG_HS_ULPI_NXT_PORT_ID == 1)
#define RTE_USB_OTG_HS_ULPI_NXT_PORT    GPIOH
#define RTE_USB_OTG_HS_ULPI_NXT_PIN     4
#else
#error "Invalid OTG_HS_ULPI_NXT Pin Configuration!"
#endif
//       <o> OTG_HS_ULPI_D0 Pin <0=>PA3
#define RTE_USB_OTG_HS_ULPI_D0_PORT_ID  0
#if    (RTE_USB_OTG_HS_ULPI_D0_PORT_ID == 0)
#define RTE_USB_OTG_HS_ULPI_D0_PORT     GPIOA
#define RTE_USB_OTG_HS_ULPI_D0_PIN      3
#else
#error "Invalid OTG_HS_ULPI_D0 Pin Configuration!"
#endif
//       <o> OTG_HS_ULPI_D1 Pin <0=>PB0
#define RTE_USB_OTG_HS_ULPI_D1_PORT_ID  0
#if    (RTE_USB_OTG_HS_ULPI_D1_PORT_ID == 0)
#define RTE_USB_OTG_HS_ULPI_D1_PORT     GPIOB
#define RTE_USB_OTG_HS_ULPI_D1_PIN      0
#else
#error "Invalid OTG_HS_ULPI_D1 Pin Configuration!"
#endif
//       <o> OTG_HS_ULPI_D2 Pin <0=>PB1
#define RTE_USB_OTG_HS_ULPI_D2_PORT_ID  0
#if    (RTE_USB_OTG_HS_ULPI_D2_PORT_ID == 0)
#define RTE_USB_OTG_HS_ULPI_D2_PORT     GPIOB
#define RTE_USB_OTG_HS_ULPI_D2_PIN      1
#else
#error "Invalid OTG_HS_ULPI_D2 Pin Configuration!"
#endif
//       <o> OTG_HS_ULPI_D3 Pin <0=>PB10
#define RTE_USB_OTG_HS_ULPI_D3_PORT_ID  0
#if    (RTE_USB_OTG_HS_ULPI_D3_PORT_ID == 0)
#define RTE_USB_OTG_HS_ULPI_D3_PORT     GPIOB
#define RTE_USB_OTG_HS_ULPI_D3_PIN      10
#else
#error "Invalid OTG_HS_ULPI_D3 Pin Configuration!"
#endif
//       <o> OTG_HS_ULPI_D4 Pin <0=>PB11
#define RTE_USB_OTG_HS_ULPI_D4_PORT_ID  0
#if    (RTE_USB_OTG_HS_ULPI_D4_PORT_ID == 0)
#define RTE_USB_OTG_HS_ULPI_D4_PORT     GPIOB
#define RTE_USB_OTG_HS_ULPI_D4_PIN      11
#else
#error "Invalid OTG_HS_ULPI_D4 Pin Configuration!"
#endif
//       <o> OTG_HS_ULPI_D5 Pin <0=>PB12
#define RTE_USB_OTG_HS_ULPI_D5_PORT_ID  0
#if    (RTE_USB_OTG_HS_ULPI_D5_PORT_ID == 0)
#define RTE_USB_OTG_HS_ULPI_D5_PORT     GPIOB
#define RTE_USB_OTG_HS_ULPI_D5_PIN      12
#else
#error "Invalid OTG_HS_ULPI_D5 Pin Configuration!"
#endif
//       <o> OTG_HS_ULPI_D6 Pin <0=>PB13
#define RTE_USB_OTG_HS_ULPI_D6_PORT_ID  0
#if    (RTE_USB_OTG_HS_ULPI_D6_PORT_ID == 0)
#define RTE_USB_OTG_HS_ULPI_D6_PORT     GPIOB
#define RTE_USB_OTG_HS_ULPI_D6_PIN      13
#else
#error "Invalid OTG_HS_ULPI_D6 Pin Configuration!"
#endif
//       <o> OTG_HS_ULPI_D7 Pin <0=>PB5
#define RTE_USB_OTG_HS_ULPI_D7_PORT_ID  0
#if    (RTE_USB_OTG_HS_ULPI_D7_PORT_ID == 0)
#define RTE_USB_OTG_HS_ULPI_D7_PORT     GPIOB
#define RTE_USB_OTG_HS_ULPI_D7_PIN      5
#else
#error "Invalid OTG_HS_ULPI_D7 Pin Configuration!"
#endif

//     </h>

//   </h>

//   <e> Device [Driver_USBD1]
//   <i> Configuration settings for Driver_USBD1 in component ::CMSIS Driver:USB Device

#define RTE_USB_OTG_HS_DEVICE           0

//     <o0.0> VBUS Sensing Pin
//     <i> Enable or disable VBUS sensing
//     <i> Relevant only if PHY Interface On-chip full-speed PHY is selected
#define RTE_OTG_HS_VBUS_SENSING_PIN     0
//   </e>

//   <e> Host [Driver_USBH1]
//   <i> Configuration settings for Driver_USBH1 in component ::CMSIS Driver:USB Host
#define RTE_USB_OTG_HS_HOST             0

//     <e> VBUS Power On/Off Pin
//     <i> Configure Pin for driving VBUS
//     <i> GPIO Pxy (x = A..J, y = 0..15) or (x = K, y = 0..7)
//       <o1> Active State <0=>Low <1=>High
//       <i>  Selects Active State Logical Level
//       <o2> Port <0=>GPIOA <1=>GPIOB <2=>GPIOC <3=>GPIOD
//                 <4=>GPIOE <5=>GPIOF <6=>GPIOG <7=>GPIOH
//                 <8=>GPIOI <8=>GPIOJ <9=>GPIOK
//       <i>  Selects Port Name
//       <o3> Bit <0-15>
//       <i>  Selects Port Bit
//     </e>
#define RTE_OTG_HS_VBUS_PIN             0
#define RTE_OTG_HS_VBUS_ACTIVE          0
#define RTE_OTG_HS_VBUS_PORT            GPIO_PORT(0)
#define RTE_OTG_HS_VBUS_BIT             0

//     <e> Overcurrent Detection Pin
//     <i> Configure Pin for overcurrent detection
//     <i> GPIO Pxy (x = A..J, y = 0..15) or (x = K, y = 0..7)
//       <o1> Active State <0=>Low <1=>High
//       <i>  Selects Active State Logical Level
//       <o2> Port <0=>GPIOA <1=>GPIOB <2=>GPIOC <3=>GPIOD
//                 <4=>GPIOE <5=>GPIOF <6=>GPIOG <7=>GPIOH
//                 <8=>GPIOI <8=>GPIOJ <9=>GPIOK
//       <i>  Selects Port Name
//       <o3> Bit <0-15>
//       <i>  Selects Port Bit
//     </e>
#define RTE_OTG_HS_OC_PIN               0
#define RTE_OTG_HS_OC_ACTIVE            0
#define RTE_OTG_HS_OC_PORT              GPIO_PORT(0)
#define RTE_OTG_HS_OC_BIT               0
//   </e>

//   <o.0> DMA
//     <i> Use dedicated DMA for transfers
//     <i> If DMA is used all USB transfer data buffers have to be 4-byte aligned.
#define RTE_OTG_HS_DMA                  0

// </e>


#endif  /* __RTE_DEVICE_H */
