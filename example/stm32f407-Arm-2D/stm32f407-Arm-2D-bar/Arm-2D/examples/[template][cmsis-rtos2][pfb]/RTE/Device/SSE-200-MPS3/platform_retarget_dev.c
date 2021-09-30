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

/**
 * \file platform_retarget_dev.c
 * \brief This file defines exports the structures based on the peripheral
 * definitions from device_cfg.h.
 * This retarget file is meant to be used as a helper for baremetal
 * applications and/or as an example of how to configure the generic
 * driver structures.
 */

#include "platform_retarget_dev.h"
#include "platform_retarget.h"
#include "system_cmsdk_mps3_sse_200.h"
#include "mps3_time.h" /* Import mps3_sleepus function */

/* ARM GPIO driver structures */
#ifdef ARM_GPIO0_S
static const struct arm_gpio_dev_cfg_t ARM_GPIO0_DEV_CFG_S = {
    .base = GPIO0_BASE_S};
static struct arm_gpio_dev_data_t ARM_GPIO0_DEV_DATA_S = {
    .state = 0,
    .port_mask = DEFAULT_PORT_MASK};
struct arm_gpio_dev_t ARM_GPIO0_DEV_S = {&(ARM_GPIO0_DEV_CFG_S),
                                         &(ARM_GPIO0_DEV_DATA_S)};
#endif
#ifdef ARM_GPIO0_NS
static const struct arm_gpio_dev_cfg_t ARM_GPIO0_DEV_CFG_NS = {
    .base = GPIO0_BASE_NS};
static struct arm_gpio_dev_data_t ARM_GPIO0_DEV_DATA_NS = {
    .state = 0,
    .port_mask = DEFAULT_PORT_MASK};
struct arm_gpio_dev_t ARM_GPIO0_DEV_NS = {&(ARM_GPIO0_DEV_CFG_NS),
                                          &(ARM_GPIO0_DEV_DATA_NS)};
#endif

#ifdef ARM_GPIO1_S
static const struct arm_gpio_dev_cfg_t ARM_GPIO1_DEV_CFG_S = {
    .base = GPIO1_BASE_S};
static struct arm_gpio_dev_data_t ARM_GPIO1_DEV_DATA_S = {
    .state = 0,
    .port_mask = DEFAULT_PORT_MASK};
struct arm_gpio_dev_t ARM_GPIO1_DEV_S = {&(ARM_GPIO1_DEV_CFG_S),
                                         &(ARM_GPIO1_DEV_DATA_S)};
#endif
#ifdef ARM_GPIO1_NS
static const struct arm_gpio_dev_cfg_t ARM_GPIO1_DEV_CFG_NS = {
    .base = GPIO1_BASE_NS};
static struct arm_gpio_dev_data_t ARM_GPIO1_DEV_DATA_NS = {
    .state = 0,
    .port_mask = DEFAULT_PORT_MASK};
struct arm_gpio_dev_t ARM_GPIO1_DEV_NS = {&(ARM_GPIO1_DEV_CFG_NS),
                                          &(ARM_GPIO1_DEV_DATA_NS)};
#endif

#ifdef ARM_GPIO2_S
static const struct arm_gpio_dev_cfg_t ARM_GPIO2_DEV_CFG_S = {
    .base = GPIO2_BASE_S};
static struct arm_gpio_dev_data_t ARM_GPIO2_DEV_DATA_S = {
    .state = 0,
    .port_mask = DEFAULT_PORT_MASK};
struct arm_gpio_dev_t ARM_GPIO2_DEV_S = {&(ARM_GPIO2_DEV_CFG_S),
                                         &(ARM_GPIO2_DEV_DATA_S)};
#endif
#ifdef ARM_GPIO2_NS
static const struct arm_gpio_dev_cfg_t ARM_GPIO2_DEV_CFG_NS = {
    .base = GPIO2_BASE_NS};
static struct arm_gpio_dev_data_t ARM_GPIO2_DEV_DATA_NS = {
    .state = 0,
    .port_mask = DEFAULT_PORT_MASK};
struct arm_gpio_dev_t ARM_GPIO2_DEV_NS = {&(ARM_GPIO2_DEV_CFG_NS),
                                          &(ARM_GPIO2_DEV_DATA_NS)};
#endif

#ifdef ARM_GPIO3_S
static const struct arm_gpio_dev_cfg_t ARM_GPIO3_DEV_CFG_S = {
    .base = GPIO3_BASE_S};
static struct arm_gpio_dev_data_t ARM_GPIO3_DEV_DATA_S = {
    .state = 0,
    .port_mask = DEFAULT_PORT_MASK};
struct arm_gpio_dev_t ARM_GPIO3_DEV_S = {&(ARM_GPIO3_DEV_CFG_S),
                                         &(ARM_GPIO3_DEV_DATA_S)};
#endif
#ifdef ARM_GPIO3_NS
static const struct arm_gpio_dev_cfg_t ARM_GPIO3_DEV_CFG_NS = {
    .base = GPIO3_BASE_NS};
static struct arm_gpio_dev_data_t ARM_GPIO3_DEV_DATA_NS = {
    .state = 0,
    .port_mask = DEFAULT_PORT_MASK};
struct arm_gpio_dev_t ARM_GPIO3_DEV_NS = {&(ARM_GPIO3_DEV_CFG_NS),
                                          &(ARM_GPIO3_DEV_DATA_NS)};
#endif

/* I2C SBCon driver structures */
#ifdef I2C0_SBCON_S
static const struct i2c_sbcon_dev_cfg_t I2C0_SBCON_DEV_CFG_S = {
    .base = I2C0_SBCON_BASE_S,
    .sleep_us = mps3_sleepus,
    .default_freq_hz = DEFAULT_I2C_SBCON_HZ};
static struct i2c_sbcon_dev_data_t I2C0_SBCON_DEV_DATA_S = {
    .freq_us = 0,
    .sys_clk = 0,
    .state = 0};
struct i2c_sbcon_dev_t I2C0_SBCON_DEV_S = {
    &(I2C0_SBCON_DEV_CFG_S),
    &(I2C0_SBCON_DEV_DATA_S)};
#endif
#ifdef I2C0_SBCON_NS
static const struct i2c_sbcon_dev_cfg_t I2C0_SBCON_DEV_CFG_NS = {
    .base = I2C0_SBCON_BASE_NS,
    .sleep_us = mps3_sleepus,
    .default_freq_hz = DEFAULT_I2C_SBCON_HZ};
static struct i2c_sbcon_dev_data_t I2C0_SBCON_DEV_DATA_NS = {
    .freq_us = 0,
    .sys_clk = 0,
    .state = 0};
struct i2c_sbcon_dev_t I2C0_SBCON_DEV_NS = {
    &(I2C0_SBCON_DEV_CFG_NS),
    &(I2C0_SBCON_DEV_DATA_NS)};
#endif

#ifdef I2C1_SBCON_S
static const struct i2c_sbcon_dev_cfg_t I2C1_SBCON_DEV_CFG_S = {
    .base = I2C1_SBCON_BASE_S,
    .sleep_us = mps3_sleepus,
    .default_freq_hz = DEFAULT_I2C_SBCON_HZ};
static struct i2c_sbcon_dev_data_t I2C1_SBCON_DEV_DATA_S = {
    .freq_us = 0,
    .sys_clk = 0,
    .state = 0};
struct i2c_sbcon_dev_t I2C1_SBCON_DEV_S = {
    &(I2C1_SBCON_DEV_CFG_S),
    &(I2C1_SBCON_DEV_DATA_S)};
#endif
#ifdef I2C1_SBCON_NS
static const struct i2c_sbcon_dev_cfg_t I2C1_SBCON_DEV_CFG_NS = {
    .base = I2C1_SBCON_BASE_NS,
    .sleep_us = mps3_sleepus,
    .default_freq_hz = DEFAULT_I2C_SBCON_HZ};
static struct i2c_sbcon_dev_data_t I2C1_SBCON_DEV_DATA_NS = {
    .freq_us = 0,
    .sys_clk = 0,
    .state = 0};
struct i2c_sbcon_dev_t I2C1_SBCON_DEV_NS = {
    &(I2C1_SBCON_DEV_CFG_NS),
    &(I2C1_SBCON_DEV_DATA_NS)};
#endif

#ifdef I2C2_SBCON_S
static const struct i2c_sbcon_dev_cfg_t I2C2_SBCON_DEV_CFG_S = {
    .base = I2C2_SBCON_BASE_S,
    .sleep_us = mps3_sleepus,
    .default_freq_hz = DEFAULT_I2C_SBCON_HZ};
static struct i2c_sbcon_dev_data_t I2C2_SBCON_DEV_DATA_S = {
    .freq_us = 0,
    .sys_clk = 0,
    .state = 0};
struct i2c_sbcon_dev_t I2C2_SBCON_DEV_S = {
    &(I2C2_SBCON_DEV_CFG_S),
    &(I2C2_SBCON_DEV_DATA_S)};
#endif
#ifdef I2C2_SBCON_NS
static const struct i2c_sbcon_dev_cfg_t I2C2_SBCON_DEV_CFG_NS = {
    .base = I2C2_SBCON_BASE_NS,
    .sleep_us = mps3_sleepus,
    .default_freq_hz = DEFAULT_I2C_SBCON_HZ};
static struct i2c_sbcon_dev_data_t I2C2_SBCON_DEV_DATA_NS = {
    .freq_us = 0,
    .sys_clk = 0,
    .state = 0};
struct i2c_sbcon_dev_t I2C2_SBCON_DEV_NS = {
    &(I2C2_SBCON_DEV_CFG_NS),
    &(I2C2_SBCON_DEV_DATA_NS)};
#endif

#ifdef I2C3_SBCON_S
static const struct i2c_sbcon_dev_cfg_t I2C3_SBCON_DEV_CFG_S = {
    .base = I2C3_SBCON_BASE_S,
    .sleep_us = mps3_sleepus,
    .default_freq_hz = DEFAULT_I2C_SBCON_HZ};
static struct i2c_sbcon_dev_data_t I2C3_SBCON_DEV_DATA_S = {
    .freq_us = 0,
    .sys_clk = 0,
    .state = 0};
struct i2c_sbcon_dev_t I2C3_SBCON_DEV_S = {
    &(I2C3_SBCON_DEV_CFG_S),
    &(I2C3_SBCON_DEV_DATA_S)};
#endif
#ifdef I2C3_SBCON_NS
static const struct i2c_sbcon_dev_cfg_t I2C3_SBCON_DEV_CFG_NS = {
    .base = I2C3_SBCON_BASE_NS,
    .sleep_us = mps3_sleepus,
    .default_freq_hz = DEFAULT_I2C_SBCON_HZ};
static struct i2c_sbcon_dev_data_t I2C3_SBCON_DEV_DATA_NS = {
    .freq_us = 0,
    .sys_clk = 0,
    .state = 0};
struct i2c_sbcon_dev_t I2C3_SBCON_DEV_NS = {
    &(I2C3_SBCON_DEV_CFG_NS),
    &(I2C3_SBCON_DEV_DATA_NS)};
#endif

/* ARM MPS3 IO FPGAIO driver structures */
#ifdef ARM_MPS3_IO_FPGAIO_S
static const struct arm_mps3_io_dev_cfg_t ARM_MPS3_IO_FPGAIO_DEV_CFG_S = {
    .base = MPS3_IO_FPGAIO_BASE_S};
struct arm_mps3_io_dev_t ARM_MPS3_IO_FPGAIO_DEV_S =
                                              {&(ARM_MPS3_IO_FPGAIO_DEV_CFG_S)};
#endif
#ifdef ARM_MPS3_IO_FPGAIO_NS
static const struct arm_mps3_io_dev_cfg_t ARM_MPS3_IO_FPGAIO_DEV_CFG_NS = {
    .base = MPS3_IO_FPGAIO_BASE_NS};
struct arm_mps3_io_dev_t ARM_MPS3_IO_FPGAIO_DEV_NS =
                                             {&(ARM_MPS3_IO_FPGAIO_DEV_CFG_NS)};
#endif

/* UART CMSDK driver structures */
#ifdef UART0_CMSDK_S
static const struct uart_cmsdk_dev_cfg_t UART0_CMSDK_DEV_CFG_S = {
    .base = UART0_CMSDK_BASE_S,
    .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct uart_cmsdk_dev_data_t UART0_CMSDK_DEV_DATA_S = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0};
struct uart_cmsdk_dev_t UART0_CMSDK_DEV_S = {&(UART0_CMSDK_DEV_CFG_S),
                                         &(UART0_CMSDK_DEV_DATA_S)};
#endif
#ifdef UART0_CMSDK_NS
static const struct uart_cmsdk_dev_cfg_t UART0_CMSDK_DEV_CFG_NS = {
    .base = UART0_CMSDK_BASE_NS,
    .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct uart_cmsdk_dev_data_t UART0_CMSDK_DEV_DATA_NS = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0};
struct uart_cmsdk_dev_t UART0_CMSDK_DEV_NS = {&(UART0_CMSDK_DEV_CFG_NS),
                                          &(UART0_CMSDK_DEV_DATA_NS)};
#endif

#ifdef UART1_CMSDK_S
static const struct uart_cmsdk_dev_cfg_t UART1_CMSDK_DEV_CFG_S = {
    .base = UART1_CMSDK_BASE_S,
    .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct uart_cmsdk_dev_data_t UART1_CMSDK_DEV_DATA_S = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0};
struct uart_cmsdk_dev_t UART1_CMSDK_DEV_S = {&(UART1_CMSDK_DEV_CFG_S),
                                         &(UART1_CMSDK_DEV_DATA_S)};
#endif
#ifdef UART1_CMSDK_NS
static const struct uart_cmsdk_dev_cfg_t UART1_CMSDK_DEV_CFG_NS = {
    .base = UART1_CMSDK_BASE_NS,
    .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct uart_cmsdk_dev_data_t UART1_CMSDK_DEV_DATA_NS = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0};
struct uart_cmsdk_dev_t UART1_CMSDK_DEV_NS = {&(UART1_CMSDK_DEV_CFG_NS),
                                          &(UART1_CMSDK_DEV_DATA_NS)};
#endif

#ifdef UART2_CMSDK_S
static const struct uart_cmsdk_dev_cfg_t UART2_CMSDK_DEV_CFG_S = {
    .base = UART2_CMSDK_BASE_S,
    .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct uart_cmsdk_dev_data_t UART2_CMSDK_DEV_DATA_S = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0};
struct uart_cmsdk_dev_t UART2_CMSDK_DEV_S = {&(UART2_CMSDK_DEV_CFG_S),
                                         &(UART2_CMSDK_DEV_DATA_S)};
#endif
#ifdef UART2_CMSDK_NS
static const struct uart_cmsdk_dev_cfg_t UART2_CMSDK_DEV_CFG_NS = {
    .base = UART2_CMSDK_BASE_NS,
    .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct uart_cmsdk_dev_data_t UART2_CMSDK_DEV_DATA_NS = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0};
struct uart_cmsdk_dev_t UART2_CMSDK_DEV_NS = {&(UART2_CMSDK_DEV_CFG_NS),
                                          &(UART2_CMSDK_DEV_DATA_NS)};
#endif

#ifdef UART3_CMSDK_S
static const struct uart_cmsdk_dev_cfg_t UART3_CMSDK_DEV_CFG_S = {
    .base = UART3_CMSDK_BASE_S,
    .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct uart_cmsdk_dev_data_t UART3_CMSDK_DEV_DATA_S = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0};
struct uart_cmsdk_dev_t UART3_CMSDK_DEV_S = {&(UART3_CMSDK_DEV_CFG_S),
                                         &(UART3_CMSDK_DEV_DATA_S)};
#endif
#ifdef UART3_CMSDK_NS
static const struct uart_cmsdk_dev_cfg_t UART3_CMSDK_DEV_CFG_NS = {
    .base = UART3_CMSDK_BASE_NS,
    .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct uart_cmsdk_dev_data_t UART3_CMSDK_DEV_DATA_NS = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0};
struct uart_cmsdk_dev_t UART3_CMSDK_DEV_NS = {&(UART3_CMSDK_DEV_CFG_NS),
                                          &(UART3_CMSDK_DEV_DATA_NS)};
#endif

#ifdef UART4_CMSDK_S
static const struct uart_cmsdk_dev_cfg_t UART4_CMSDK_DEV_CFG_S = {
    .base = UART4_CMSDK_BASE_S,
    .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct uart_cmsdk_dev_data_t UART4_CMSDK_DEV_DATA_S = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0};
struct uart_cmsdk_dev_t UART4_CMSDK_DEV_S = {&(UART4_CMSDK_DEV_CFG_S),
                                         &(UART4_CMSDK_DEV_DATA_S)};
#endif
#ifdef UART4_CMSDK_NS
static const struct uart_cmsdk_dev_cfg_t UART4_CMSDK_DEV_CFG_NS = {
    .base = UART4_CMSDK_BASE_NS,
    .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct uart_cmsdk_dev_data_t UART4_CMSDK_DEV_DATA_NS = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0};
struct uart_cmsdk_dev_t UART4_CMSDK_DEV_NS = {&(UART4_CMSDK_DEV_CFG_NS),
                                          &(UART4_CMSDK_DEV_DATA_NS)};
#endif

#ifdef UART5_CMSDK_S
static const struct uart_cmsdk_dev_cfg_t UART5_CMSDK_DEV_CFG_S = {
    .base = UART5_CMSDK_BASE_S,
    .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct uart_cmsdk_dev_data_t UART5_CMSDK_DEV_DATA_S = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0};
struct uart_cmsdk_dev_t UART5_CMSDK_DEV_S = {&(UART5_CMSDK_DEV_CFG_S),
                                         &(UART5_CMSDK_DEV_DATA_S)};
#endif
#ifdef UART5_CMSDK_NS
static const struct uart_cmsdk_dev_cfg_t UART5_CMSDK_DEV_CFG_NS = {
    .base = UART5_CMSDK_BASE_NS,
    .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct uart_cmsdk_dev_data_t UART5_CMSDK_DEV_DATA_NS = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0};
struct uart_cmsdk_dev_t UART5_CMSDK_DEV_NS = {&(UART5_CMSDK_DEV_CFG_NS),
                                          &(UART5_CMSDK_DEV_DATA_NS)};
#endif

/* ARM Watchdog driver structures */
#ifdef APB_WATCHDOG_S
static const struct arm_watchdog_dev_cfg_t APB_WATCHDOG_DEV_CFG_S = {
    .base = APB_WATCHDOG_BASE_S};
static struct arm_watchdog_dev_data_t APB_WATCHDOG_DEV_DATA_S = {
    .state = 0,
    .timeout = 0};
struct arm_watchdog_dev_t APB_WATCHDOG_DEV_S = {
    &(APB_WATCHDOG_DEV_CFG_S),
    &(APB_WATCHDOG_DEV_DATA_S)};
#endif
#ifdef APB_WATCHDOG_NS
static const struct arm_watchdog_dev_cfg_t APB_WATCHDOG_DEV_CFG_NS = {
    .base = APB_WATCHDOG_BASE_NS};
static struct arm_watchdog_dev_data_t APB_WATCHDOG_DEV_DATA_NS = {
    .state = 0,
    .timeout = 0};
struct arm_watchdog_dev_t APB_WATCHDOG_DEV_NS = {
    &(APB_WATCHDOG_DEV_CFG_NS),
    &(APB_WATCHDOG_DEV_DATA_NS)};
#endif

#ifdef S32K_WATCHDOG_S
static const struct arm_watchdog_dev_cfg_t S32K_WATCHDOG_DEV_CFG_S = {
    .base = S32K_WATCHDOG_BASE_S};
static struct arm_watchdog_dev_data_t S32K_WATCHDOG_DEV_DATA_S = {
    .state = 0,
    .timeout = 0};
struct arm_watchdog_dev_t S32K_WATCHDOG_DEV_S = {
    &(S32K_WATCHDOG_DEV_CFG_S),
    &(S32K_WATCHDOG_DEV_DATA_S)};
#endif

/* CMSDK Dualtimer driver structures */
#ifdef CMSDK_DUALTIMER_S
static const struct dualtimer_cmsdk_dev_cfg_t CMSDK_DUALTIMER_DEV_CFG_S = {
    .base = CMSDK_DUALTIMER_BASE_S};
static struct dualtimer_cmsdk_dev_data_t CMSDK_DUALTIMER_DEV_DATA_S = {
    .is_initialized = 0};
struct dualtimer_cmsdk_dev_t CMSDK_DUALTIMER_DEV_S = {
    &(CMSDK_DUALTIMER_DEV_CFG_S),
    &(CMSDK_DUALTIMER_DEV_DATA_S)};
#endif
#ifdef CMSDK_DUALTIMER_NS
static const struct dualtimer_cmsdk_dev_cfg_t CMSDK_DUALTIMER_DEV_CFG_NS = {
    .base = CMSDK_DUALTIMER_BASE_NS};
static struct dualtimer_cmsdk_dev_data_t CMSDK_DUALTIMER_DEV_DATA_NS = {
    .is_initialized = 0};
struct dualtimer_cmsdk_dev_t CMSDK_DUALTIMER_DEV_NS = {
    &(CMSDK_DUALTIMER_DEV_CFG_NS),
    &(CMSDK_DUALTIMER_DEV_DATA_NS)};
#endif

/* ARM PPC SIE 200 driver structures */
#ifdef AHB_PPC0_S
static struct ppc_sse200_dev_cfg_t AHB_PPC0_DEV_CFG_S = {
    .spctrl_base  = CMSDK_SPCTRL_BASE_S,
    .nspctrl_base = CMSDK_NSPCTRL_BASE_NS };
static struct ppc_sse200_dev_data_t AHB_PPC0_DEV_DATA_S = {
    .p_ns_ppc  = 0,
    .p_sp_ppc  = 0,
    .p_nsp_ppc = 0,
    .int_bit_mask = 0,
    .state = 0 };
struct ppc_sse200_dev_t AHB_PPC0_DEV_S = {
    &AHB_PPC0_DEV_CFG_S, &AHB_PPC0_DEV_DATA_S };
#endif

#ifdef AHB_PPCEXP0_S
static struct ppc_sse200_dev_cfg_t AHB_PPCEXP0_DEV_CFG_S = {
    .spctrl_base  = CMSDK_SPCTRL_BASE_S,
    .nspctrl_base = CMSDK_NSPCTRL_BASE_NS };
static struct ppc_sse200_dev_data_t AHB_PPCEXP0_DEV_DATA_S = {
    .p_ns_ppc  = 0,
    .p_sp_ppc  = 0,
    .p_nsp_ppc = 0,
    .int_bit_mask = 0,
    .state = 0 };
struct ppc_sse200_dev_t AHB_PPCEXP0_DEV_S = {
    &AHB_PPCEXP0_DEV_CFG_S, &AHB_PPCEXP0_DEV_DATA_S };
#endif

#ifdef AHB_PPCEXP1_S
static struct ppc_sse200_dev_cfg_t AHB_PPCEXP1_DEV_CFG_S = {
    .spctrl_base  = CMSDK_SPCTRL_BASE_S,
    .nspctrl_base = CMSDK_NSPCTRL_BASE_NS };
static struct ppc_sse200_dev_data_t AHB_PPCEXP1_DEV_DATA_S = {
    .p_ns_ppc  = 0,
    .p_sp_ppc  = 0,
    .p_nsp_ppc = 0,
    .int_bit_mask = 0,
    .state = 0 };
struct ppc_sie200_dev_t AHB_PPCEXP1_DEV_S = {
    &AHB_PPCEXP1_DEV_CFG_S, &AHB_PPCEXP1_DEV_DATA_S };
#endif

#ifdef AHB_PPCEXP2_S
static struct ppc_sse200_dev_cfg_t AHB_PPCEXP2_DEV_CFG_S = {
    .spctrl_base  = CMSDK_SPCTRL_BASE_S,
    .nspctrl_base = CMSDK_NSPCTRL_BASE_NS };
static struct ppc_sse200_dev_data_t AHB_PPCEXP2_DEV_DATA_S = {
    .p_ns_ppc  = 0,
    .p_sp_ppc  = 0,
    .p_nsp_ppc = 0,
    .int_bit_mask = 0,
    .state = 0 };
struct ppc_sse200_dev_t AHB_PPCEXP2_DEV_S = {
    &AHB_PPCEXP2_DEV_CFG_S, &AHB_PPCEXP2_DEV_DATA_S };
#endif

#ifdef AHB_PPCEXP3_S
static struct ppc_sse200_dev_cfg_t AHB_PPCEXP3_DEV_CFG_S = {
    .spctrl_base  = CMSDK_SPCTRL_BASE_S,
    .nspctrl_base = CMSDK_NSPCTRL_BASE_NS };
static struct ppc_sse200_dev_data_t AHB_PPCEXP3_DEV_DATA_S = {
    .p_ns_ppc  = 0,
    .p_sp_ppc  = 0,
    .p_nsp_ppc = 0,
    .int_bit_mask = 0,
    .state = 0 };
struct ppc_sse200_dev_t AHB_PPCEXP3_DEV_S = {
    &AHB_PPCEXP3_DEV_CFG_S, &AHB_PPCEXP3_DEV_DATA_S };
#endif

#ifdef APB_PPC0_S
static struct ppc_sse200_dev_cfg_t APB_PPC0_DEV_CFG_S = {
    .spctrl_base  = CMSDK_SPCTRL_BASE_S,
    .nspctrl_base = CMSDK_NSPCTRL_BASE_NS };
static struct ppc_sse200_dev_data_t APB_PPC0_DEV_DATA_S = {
    .p_ns_ppc  = 0,
    .p_sp_ppc  = 0,
    .p_nsp_ppc = 0,
    .int_bit_mask = 0,
    .state = 0 };
struct ppc_sse200_dev_t APB_PPC0_DEV_S = {
    &APB_PPC0_DEV_CFG_S, &APB_PPC0_DEV_DATA_S };
#endif

#ifdef APB_PPC1_S
static struct ppc_sse200_dev_cfg_t APB_PPC1_DEV_CFG_S = {
    .spctrl_base  = CMSDK_SPCTRL_BASE_S,
    .nspctrl_base = CMSDK_NSPCTRL_BASE_NS };
static struct ppc_sse200_dev_data_t APB_PPC1_DEV_DATA_S = {
    .p_ns_ppc  = 0,
    .p_sp_ppc  = 0,
    .p_nsp_ppc = 0,
    .int_bit_mask = 0,
    .state = 0 };
struct ppc_sse200_dev_t APB_PPC1_DEV_S = {
    &APB_PPC1_DEV_CFG_S, &APB_PPC1_DEV_DATA_S};
#endif

#ifdef APB_PPCEXP0_S
static struct ppc_sse200_dev_cfg_t APB_PPCEXP0_DEV_CFG_S = {
    .spctrl_base  = CMSDK_SPCTRL_BASE_S,
    .nspctrl_base = CMSDK_NSPCTRL_BASE_NS };
static struct ppc_sse200_dev_data_t APB_PPCEXP0_DEV_DATA_S = {
    .p_ns_ppc  = 0,
    .p_sp_ppc  = 0,
    .p_nsp_ppc = 0,
    .int_bit_mask = 0,
    .state = 0 };
struct ppc_sse200_dev_t APB_PPCEXP0_DEV_S = {
    &APB_PPCEXP0_DEV_CFG_S, &APB_PPCEXP0_DEV_DATA_S };
#endif

#ifdef APB_PPCEXP1_S
static struct ppc_sse200_dev_cfg_t APB_PPCEXP1_DEV_CFG = {
    .spctrl_base  = CMSDK_SPCTRL_BASE_S,
    .nspctrl_base = CMSDK_NSPCTRL_BASE_NS };
static struct ppc_sse200_dev_data_t APB_PPCEXP1_DEV_DATA_S = {
    .p_ns_ppc  = 0,
    .p_sp_ppc  = 0,
    .p_nsp_ppc = 0,
    .int_bit_mask = 0,
    .state = 0 };
struct ppc_sse200_dev_t APB_PPCEXP1_DEV_S = {
    &APB_PPCEXP1_DEV_CFG, &APB_PPCEXP1_DEV_DATA_S };
#endif

#ifdef APB_PPCEXP2_S
static struct ppc_sse200_dev_cfg_t APB_PPCEXP2_DEV_CFG = {
    .spctrl_base  = CMSDK_SPCTRL_BASE_S,
    .nspctrl_base = CMSDK_NSPCTRL_BASE_NS };
static struct ppc_sse200_dev_data_t APB_PPCEXP2_DEV_DATA_S = {
    .p_ns_ppc  = 0,
    .p_sp_ppc  = 0,
    .p_nsp_ppc = 0,
    .int_bit_mask = 0,
    .state = 0 };
struct ppc_sse200_dev_t APB_PPCEXP2_DEV_S = {
    &APB_PPCEXP2_DEV_CFG, &APB_PPCEXP2_DEV_DATA_S };
#endif

#ifdef APB_PPCEXP3_S
static struct ppc_sse200_dev_cfg_t APB_PPCEXP3_DEV_CFG = {
    .spctrl_base  = CMSDK_SPCTRL_BASE_S,
    .nspctrl_base = CMSDK_NSPCTRL_BASE_NS };
static struct ppc_sse200_dev_data_t APB_PPCEXP3_DEV_DATA_S = {
    .p_ns_ppc  = 0,
    .p_sp_ppc  = 0,
    .p_nsp_ppc = 0,
    .int_bit_mask = 0,
    .state = 0 };
struct ppc_sse200_dev_t APB_PPCEXP3_DEV_S = {
    &APB_PPCEXP3_DEV_CFG, &APB_PPCEXP3_DEV_DATA_S };
#endif

/* CMSDK Timer driver structures */
#ifdef CMSDK_TIMER0_S
static const struct timer_cmsdk_dev_cfg_t CMSDK_TIMER0_DEV_CFG_S = {
    .base = CMSDK_TIMER0_BASE_S};
static struct timer_cmsdk_dev_data_t CMSDK_TIMER0_DEV_DATA_S = {
    .is_initialized = 0};
struct timer_cmsdk_dev_t CMSDK_TIMER0_DEV_S = {&(CMSDK_TIMER0_DEV_CFG_S),
                                               &(CMSDK_TIMER0_DEV_DATA_S)};
#endif
#ifdef CMSDK_TIMER0_NS
static const struct timer_cmsdk_dev_cfg_t CMSDK_TIMER0_DEV_CFG_NS = {
    .base = CMSDK_TIMER0_BASE_NS};
static struct timer_cmsdk_dev_data_t CMSDK_TIMER0_DEV_DATA_NS = {
    .is_initialized = 0};
struct timer_cmsdk_dev_t CMSDK_TIMER0_DEV_NS = {&(CMSDK_TIMER0_DEV_CFG_NS),
                                                &(CMSDK_TIMER0_DEV_DATA_NS)};
#endif

#ifdef CMSDK_TIMER1_S
static const struct timer_cmsdk_dev_cfg_t CMSDK_TIMER1_DEV_CFG_S = {
    .base = CMSDK_TIMER1_BASE_S};
static struct timer_cmsdk_dev_data_t CMSDK_TIMER1_DEV_DATA_S = {
    .is_initialized = 0};
struct timer_cmsdk_dev_t CMSDK_TIMER1_DEV_S = {&(CMSDK_TIMER1_DEV_CFG_S),
                                               &(CMSDK_TIMER1_DEV_DATA_S)};
#endif
#ifdef CMSDK_TIMER1_NS
static const struct timer_cmsdk_dev_cfg_t CMSDK_TIMER1_DEV_CFG_NS = {
    .base = CMSDK_TIMER1_BASE_NS};
static struct timer_cmsdk_dev_data_t CMSDK_TIMER1_DEV_DATA_NS = {
    .is_initialized = 0};
struct timer_cmsdk_dev_t CMSDK_TIMER1_DEV_NS = {&(CMSDK_TIMER1_DEV_CFG_NS),
                                                &(CMSDK_TIMER1_DEV_DATA_NS)};
#endif

/* ARM MPC SSE 200 driver structures */
#ifdef MPC_ISRAM0_S
static const struct mpc_sie200_dev_cfg_t MPC_ISRAM0_DEV_CFG_S = {
    .base = MPC_ISRAM0_BASE_S};
static struct mpc_sie200_dev_data_t MPC_ISRAM0_DEV_DATA_S = {
    .range_list = 0,
    .nbr_of_ranges = 0,
    .state = 0,
    .reserved = 0};
struct mpc_sie200_dev_t MPC_ISRAM0_DEV_S = {
    &(MPC_ISRAM0_DEV_CFG_S),
    &(MPC_ISRAM0_DEV_DATA_S)};
#endif

#ifdef MPC_ISRAM1_S
static const struct mpc_sie200_dev_cfg_t MPC_ISRAM1_DEV_CFG_S = {
    .base = MPC_ISRAM1_BASE_S};
static struct mpc_sie200_dev_data_t MPC_ISRAM1_DEV_DATA_S = {
    .range_list = 0,
    .nbr_of_ranges = 0,
    .state = 0,
    .reserved = 0};
struct mpc_sie200_dev_t MPC_ISRAM1_DEV_S = {
    &(MPC_ISRAM1_DEV_CFG_S),
    &(MPC_ISRAM1_DEV_DATA_S)};
#endif

#ifdef MPC_ISRAM2_S
static const struct mpc_sie200_dev_cfg_t MPC_ISRAM2_DEV_CFG_S = {
    .base = MPC_ISRAM2_BASE_S};
static struct mpc_sie200_dev_data_t MPC_ISRAM2_DEV_DATA_S = {
    .range_list = 0,
    .nbr_of_ranges = 0,
    .state = 0,
    .reserved = 0};
struct mpc_sie200_dev_t MPC_ISRAM2_DEV_S = {
    &(MPC_ISRAM2_DEV_CFG_S),
    &(MPC_ISRAM2_DEV_DATA_S)};
#endif

#ifdef MPC_ISRAM3_S
static const struct mpc_sie200_dev_cfg_t MPC_ISRAM3_DEV_CFG_S = {
    .base = MPC_ISRAM3_BASE_S};
static struct mpc_sie200_dev_data_t MPC_ISRAM3_DEV_DATA_S = {
    .range_list = 0,
    .nbr_of_ranges = 0,
    .state = 0,
    .reserved = 0};
struct mpc_sie200_dev_t MPC_ISRAM3_DEV_S = {
    &(MPC_ISRAM3_DEV_CFG_S),
    &(MPC_ISRAM3_DEV_DATA_S)};
#endif

#ifdef MPC_CODE_SRAM1_S
static const struct mpc_sie200_dev_cfg_t MPC_CODE_SRAM1_DEV_CFG_S = {
    .base = MPC_CODE_SRAM1_BASE_S};
static struct mpc_sie200_dev_data_t MPC_CODE_SRAM1_DEV_DATA_S = {
    .range_list = 0,
    .nbr_of_ranges = 0,
    .state = 0,
    .reserved = 0};
struct mpc_sie200_dev_t MPC_CODE_SRAM1_DEV_S = {
    &(MPC_CODE_SRAM1_DEV_CFG_S),
    &(MPC_CODE_SRAM1_DEV_DATA_S)};
#endif

#ifdef MPC_CODE_SRAM2_S
static const struct mpc_sie200_dev_cfg_t MPC_CODE_SRAM2_DEV_CFG_S = {
    .base = MPC_CODE_SRAM2_BASE_S};
static struct mpc_sie200_dev_data_t MPC_CODE_SRAM2_DEV_DATA_S = {
    .range_list = 0,
    .nbr_of_ranges = 0,
    .state = 0,
    .reserved = 0};
struct mpc_sie200_dev_t MPC_CODE_SRAM2_DEV_S = {
    &(MPC_CODE_SRAM2_DEV_CFG_S),
    &(MPC_CODE_SRAM2_DEV_DATA_S)};
#endif

#ifdef MPC_CODE_SRAM3_S
static const struct mpc_sie200_dev_cfg_t MPC_CODE_SRAM3_DEV_CFG_S = {
    .base = MPC_CODE_SRAM3_BASE_S};
static struct mpc_sie200_dev_data_t MPC_CODE_SRAM3_DEV_DATA_S = {
    .range_list = 0,
    .nbr_of_ranges = 0,
    .state = 0,
    .reserved = 0};
struct mpc_sie200_dev_t MPC_CODE_SRAM3_DEV_S = {
    &(MPC_CODE_SRAM3_DEV_CFG_S),
    &(MPC_CODE_SRAM3_DEV_DATA_S)};
#endif

/* SPI PL022 driver structures */
#ifdef SPI0_PL022_S
static const struct spi_pl022_dev_cfg_t SPI0_PL022_DEV_CFG_S = {
    .base = SSP0_BASE_S,
    .default_ctrl_cfg = {
       .spi_mode = SPI_PL022_MASTER_SELECT,
       .frame_format = SPI_PL022_CFG_FRF_MOT,
       .word_size = 8,
       .bit_rate = DEFAULT_SPI_SPEED_HZ
    }};
static struct spi_pl022_dev_data_t SPI0_PL022_DEV_DATA_S = {
    .state = 0,
    .sys_clk = 0,
    .ctrl_cfg = {
       .spi_mode = 0,
       .frame_format = 0,
       .word_size = 0,
       .bit_rate = 0
     }};
struct spi_pl022_dev_t SPI0_PL022_DEV_S = {
  &(SPI0_PL022_DEV_CFG_S),
  &(SPI0_PL022_DEV_DATA_S)};
#endif
#ifdef SPI0_PL022_NS
static const struct spi_pl022_dev_cfg_t SPI0_PL022_DEV_CFG_NS = {
    .base = SSP0_BASE_NS,
    .default_ctrl_cfg = {
       .spi_mode = SPI_PL022_MASTER_SELECT,
       .frame_format = SPI_PL022_CFG_FRF_MOT,
       .word_size = 8,
       .bit_rate = DEFAULT_SPI_SPEED_HZ
    }};
static struct spi_pl022_dev_data_t SPI0_PL022_DEV_DATA_NS = {
    .state = 0,
    .sys_clk = 0,
    .ctrl_cfg = {
       .spi_mode = 0,
       .frame_format = 0,
       .word_size = 0,
       .bit_rate = 0
     }};
struct spi_pl022_dev_t SPI0_PL022_DEV_NS = {
  &(SPI0_PL022_DEV_CFG_NS),
  &(SPI0_PL022_DEV_DATA_NS)};
#endif

#ifdef SPI1_PL022_S
static const struct spi_pl022_dev_cfg_t SPI1_PL022_DEV_CFG_S = {
    .base = SSP1_BASE_S,
    .default_ctrl_cfg = {
       .spi_mode = SPI_PL022_MASTER_SELECT,
       .frame_format = SPI_PL022_CFG_FRF_MOT,
       .word_size = 8,
       .bit_rate = DEFAULT_SPI_SPEED_HZ
    }};
static struct spi_pl022_dev_data_t SPI1_PL022_DEV_DATA_S = {
    .state = 0,
    .sys_clk = 0,
    .ctrl_cfg = {
       .spi_mode = 0,
       .frame_format = 0,
       .word_size = 0,
       .bit_rate = 0
     }};
struct spi_pl022_dev_t SPI1_PL022_DEV_S = {
  &(SPI1_PL022_DEV_CFG_S),
  &(SPI1_PL022_DEV_DATA_S)};
#endif
#ifdef SPI1_PL022_NS
static const struct spi_pl022_dev_cfg_t SPI1_PL022_DEV_CFG_NS = {
    .base = SSP1_BASE_NS,
    .default_ctrl_cfg = {
       .spi_mode = SPI_PL022_MASTER_SELECT,
       .frame_format = SPI_PL022_CFG_FRF_MOT,
       .word_size = 8,
       .bit_rate = DEFAULT_SPI_SPEED_HZ
    }};
static struct spi_pl022_dev_data_t SPI1_PL022_DEV_DATA_NS = {
    .state = 0,
    .sys_clk = 0,
    .ctrl_cfg = {
       .spi_mode = 0,
       .frame_format = 0,
       .word_size = 0,
       .bit_rate = 0
     }};
struct spi_pl022_dev_t SPI1_PL022_DEV_NS = {
    &(SPI1_PL022_DEV_CFG_NS),
    &(SPI1_PL022_DEV_DATA_NS)};
#endif

#ifdef SPI2_PL022_S
static const struct spi_pl022_dev_cfg_t SPI2_PL022_DEV_CFG_S = {
    .base = SSP2_BASE_S,
    .default_ctrl_cfg = {
       .spi_mode = SPI_PL022_MASTER_SELECT,
       .frame_format = SPI_PL022_CFG_FRF_MOT,
       .word_size = 8,
       .bit_rate = DEFAULT_SPI_SPEED_HZ
    }};
static struct spi_pl022_dev_data_t SPI2_PL022_DEV_DATA_S = {
    .state = 0,
    .sys_clk = 0,
    .ctrl_cfg = {
       .spi_mode = 0,
       .frame_format = 0,
       .word_size = 0,
       .bit_rate = 0
     }};
struct spi_pl022_dev_t SPI2_PL022_DEV_S = {
  &(SPI2_PL022_DEV_CFG_S),
  &(SPI2_PL022_DEV_DATA_S)};
#endif
#ifdef SPI2_PL022_NS
static const struct spi_pl022_dev_cfg_t SPI2_PL022_DEV_CFG_NS = {
    .base = SSP2_BASE_NS,
    .default_ctrl_cfg = {
       .spi_mode = SPI_PL022_MASTER_SELECT,
       .frame_format = SPI_PL022_CFG_FRF_MOT,
       .word_size = 8,
       .bit_rate = DEFAULT_SPI_SPEED_HZ
    }};
static struct spi_pl022_dev_data_t SPI2_PL022_DEV_DATA_NS = {
    .state = 0,
    .sys_clk = 0,
    .ctrl_cfg = {
       .spi_mode = 0,
       .frame_format = 0,
       .word_size = 0,
       .bit_rate = 0
     }};
struct spi_pl022_dev_t SPI2_PL022_DEV_NS = {
    &(SPI2_PL022_DEV_CFG_NS),
    &(SPI2_PL022_DEV_DATA_NS)};
#endif
