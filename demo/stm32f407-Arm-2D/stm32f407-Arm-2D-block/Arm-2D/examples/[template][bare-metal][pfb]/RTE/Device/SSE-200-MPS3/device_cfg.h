/*
 * Copyright (c) 2016-2018 ARM Limited
 *
 * Licensed under the Apache License Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __ARM_LTD_DEVICE_CFG_H__
#define __ARM_LTD_DEVICE_CFG_H__

/**
 * \file device_cfg.h
 * \brief
 * This is the default device configuration file with all peripherals
 * defined and configured to be use via the non-secure base address.
 * This file is an example of how to define your own configuration file
 * with the peripherals required for your application.
 */

/* ARM GPIO */
#define ARM_GPIO0_S
#define ARM_GPIO0_NS
#define ARM_GPIO1_S
#define ARM_GPIO1_NS
#define ARM_GPIO2_S
#define ARM_GPIO2_NS
#define ARM_GPIO3_S
#define ARM_GPIO3_NS

/* ARM I2C SBCon */
#define DEFAULT_I2C_SBCON_HZ  4000000U /* 4MHz */
#define I2C0_SBCON_S
#define I2C0_SBCON_NS
#define I2C1_SBCON_S
#define I2C1_SBCON_NS
#define I2C2_SBCON_S
#define I2C2_SBCON_NS
#define I2C3_SBCON_S
#define I2C3_SBCON_NS

/* ARM MPS3 IO FPGAIO */
#define ARM_MPS3_IO_FPGAIO_S
#define ARM_MPS3_IO_FPGAIO_NS

/* ARM Memory Protection Controller (MPC) */
#define MPC_ISRAM0_S
#define MPC_ISRAM1_S
#define MPC_ISRAM2_S
#define MPC_ISRAM3_S
#define MPC_SRAM1_S
#define MPC_SRAM2_S
#define MPC_SRAM3_S

/* ARM Peripheral Protection Controllers (PPC) */
#define AHB_PPCEXP0_S
#define APB_PPC0_S
#define APB_PPC1_S
#define APB_PPCEXP0_S
#define APB_PPCEXP1_S
#define APB_PPCEXP2_S
#define APB_PPCEXP3_S

/* ARM SPI PL022 */
#define DEFAULT_SPI_SPEED_HZ  4000000U /* 4MHz */
#define SPI0_PL022_S
#define SPI0_PL022_NS
#define SPI1_PL022_S
#define SPI1_PL022_NS
#define SPI2_PL022_S
#define SPI2_PL022_NS

/* ARM UART CMSDK */
#define DEFAULT_UART_BAUDRATE  9600
#define UART0_CMSDK_S
#define UART0_CMSDK_NS
#define UART1_CMSDK_S
#define UART1_CMSDK_NS
#define UART2_CMSDK_S
#define UART2_CMSDK_NS
#define UART3_CMSDK_S
#define UART3_CMSDK_NS
#define UART4_CMSDK_S
#define UART4_CMSDK_NS
#define UART5_CMSDK_S
#define UART5_CMSDK_NS

/* ARM Watchdog */
#define APB_WATCHDOG_S
#define APB_WATCHDOG_NS
#define S32K_WATCHDOG_S

/* CMSDK Dualtimers */
#define CMSDK_DUALTIMER_S
#define CMSDK_DUALTIMER_NS

/* CMSDK Timers */
#define CMSDK_TIMER0_S
#define CMSDK_TIMER0_NS
#define CMSDK_TIMER1_S
#define CMSDK_TIMER1_NS

#endif  /* __ARM_LTD_DEVICE_CFG_H__ */
