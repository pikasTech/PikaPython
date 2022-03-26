/********************************** (C) COPYRIGHT  *******************************
 * File Name          : core_riscv.h
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2021/10/28
 * Description        : CH583 RISC-V Core Peripheral Access Layer Header File
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/
#ifndef __CORE_RV3A_H__
#define __CORE_RV3A_H__

#ifdef __cplusplus
extern "C" {
#endif

/* IO definitions */
#ifdef __cplusplus
  #define __I    volatile  /*!< defines 'read only' permissions      */
#else
  #define __I    volatile const /*!< defines 'read only' permissions     */
#endif
#define __O                 volatile  /*!< defines 'write only' permissions     */
#define __IO                volatile  /*!< defines 'read / write' permissions   */
#define RV_STATIC_INLINE    static inline

//typedef enum {SUCCESS = 0, ERROR = !SUCCESS} ErrorStatus;

typedef enum
{
    DISABLE = 0,
    ENABLE = !DISABLE
} FunctionalState;
typedef enum
{
    RESET = 0,
    SET = !RESET
} FlagStatus, ITStatus;

/* memory mapped structure for Program Fast Interrupt Controller (PFIC) */
typedef struct __attribute__((packed))
{
    __I uint32_t  ISR[8];           // 0
    __I uint32_t  IPR[8];           // 20H
    __IO uint32_t ITHRESDR;         // 40H
    uint8_t       RESERVED[4];      // 44H
    __O uint32_t  CFGR;             // 48H
    __I uint32_t  GISR;             // 4CH
    __IO uint8_t  IDCFGR[4];        // 50H
    uint8_t       RESERVED0[0x0C];  // 54H
    __IO uint32_t FIADDRR[4];       // 60H
    uint8_t       RESERVED1[0x90];  // 70H
    __O uint32_t  IENR[8];          // 100H
    uint8_t       RESERVED2[0x60];  // 120H
    __O uint32_t  IRER[8];          // 180H
    uint8_t       RESERVED3[0x60];  // 1A0H
    __O uint32_t  IPSR[8];          // 200H
    uint8_t       RESERVED4[0x60];  // 220H
    __O uint32_t  IPRR[8];          // 280H
    uint8_t       RESERVED5[0x60];  // 2A0H
    __IO uint32_t IACTR[8];         // 300H
    uint8_t       RESERVED6[0xE0];  // 320H
    __IO uint8_t  IPRIOR[256];      // 400H
    uint8_t       RESERVED7[0x810]; // 500H
    __IO uint32_t SCTLR;            // D10H
} PFIC_Type;

/* memory mapped structure for SysTick */
typedef struct __attribute__((packed))
{
    __IO uint32_t CTLR;
    __IO uint32_t SR;
    __IO uint64_t CNT;
    __IO uint64_t CMP;
} SysTick_Type;

#define PFIC                    ((PFIC_Type *)0xE000E000)
#define SysTick                 ((SysTick_Type *)0xE000F000)

#define PFIC_KEY1               ((uint32_t)0xFA050000)
#define PFIC_KEY2               ((uint32_t)0xBCAF0000)
#define PFIC_KEY3               ((uint32_t)0xBEEF0000)

/* ##########################   define  #################################### */
#define __nop()                 asm volatile("nop")

#define read_csr(reg)           ({unsigned long __tmp;                        \
     asm volatile ("csrr %0, " #reg : "=r"(__tmp));                 \
         __tmp; })

#define write_csr(reg, val)     ({                                      \
    if (__builtin_constant_p(val) && (unsigned long)(val) < 32)    \
      asm volatile ("csrw  " #reg ", %0" :: "i"(val));              \
    else                                                            \
      asm volatile ("csrw  " #reg ", %0" :: "r"(val)); })

#define PFIC_EnableAllIRQ()     write_csr(0x800, 0x88)
#define PFIC_DisableAllIRQ()    write_csr(0x800, 0x80)
/* ##########################   PFIC functions  #################################### */

/*******************************************************************************
 * @fn      PFIC_EnableIRQ
 *
 * @brief   Enable Interrupt
 *
 * @param   IRQn    - Interrupt Numbers
 */
RV_STATIC_INLINE void PFIC_EnableIRQ(IRQn_Type IRQn)
{
    PFIC->IENR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn)&0x1F));
}

/*******************************************************************************
 * @fn      PFIC_DisableIRQ
 *
 * @brief   Disable Interrupt
 *
 * @param   IRQn    - Interrupt Numbers
 */
RV_STATIC_INLINE void PFIC_DisableIRQ(IRQn_Type IRQn)
{
    PFIC->IRER[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn)&0x1F));
    __nop();
    __nop();
}

/*******************************************************************************
 * @fn      PFIC_GetStatusIRQ
 *
 * @brief   Get Interrupt Enable State
 *
 * @param   IRQn    - Interrupt Numbers
 *
 * @return  1: Interrupt Enable
 *          0: Interrupt Disable
 */
RV_STATIC_INLINE uint32_t PFIC_GetStatusIRQ(IRQn_Type IRQn)
{
    return ((uint32_t)((PFIC->ISR[(uint32_t)(IRQn) >> 5] & (1 << ((uint32_t)(IRQn)&0x1F))) ? 1 : 0));
}

/*******************************************************************************
 * @fn      PFIC_GetPendingIRQ
 *
 * @brief   Get Interrupt Pending State
 *
 * @param   IRQn    - Interrupt Numbers
 *
 * @return  1: Interrupt Pending Enable
 *          0: Interrupt Pending Disable
 */
RV_STATIC_INLINE uint32_t PFIC_GetPendingIRQ(IRQn_Type IRQn)
{
    return ((uint32_t)((PFIC->IPR[(uint32_t)(IRQn) >> 5] & (1 << ((uint32_t)(IRQn)&0x1F))) ? 1 : 0));
}

/*******************************************************************************
 * @fn      PFIC_SetPendingIRQ
 *
 * @brief   Set Interrupt Pending
 *
 * @param   IRQn    - Interrupt Numbers
 */
RV_STATIC_INLINE void PFIC_SetPendingIRQ(IRQn_Type IRQn)
{
    PFIC->IPSR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn)&0x1F));
}

/*******************************************************************************
 * @fn      PFIC_ClearPendingIRQ
 *
 * @brief   Clear Interrupt Pending
 *
 * @param   IRQn    - Interrupt Numbers
 */
RV_STATIC_INLINE void PFIC_ClearPendingIRQ(IRQn_Type IRQn)
{
    PFIC->IPRR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn)&0x1F));
}

/*******************************************************************************
 * @fn      PFIC_GetActive
 *
 * @brief   Get Interrupt Active State
 *
 * @param   IRQn    - Interrupt Numbers
 *
 * @return  1: Interrupt Active
 *          0: Interrupt No Active.
 */
RV_STATIC_INLINE uint32_t PFIC_GetActive(IRQn_Type IRQn)
{
    return ((uint32_t)((PFIC->IACTR[(uint32_t)(IRQn) >> 5] & (1 << ((uint32_t)(IRQn)&0x1F))) ? 1 : 0));
}

/*******************************************************************************
 * @fn      PFIC_SetPriority
 *
 * @brief   Set Interrupt Priority
 *
 * @param   IRQn        - Interrupt Numbers
 * @param   priority    - bit7:         pre-emption priority
 *                        bit6-bit4:    subpriority
 */
RV_STATIC_INLINE void PFIC_SetPriority(IRQn_Type IRQn, uint8_t priority)
{
    PFIC->IPRIOR[(uint32_t)(IRQn)] = priority;
}

/*********************************************************************
 * @fn      PFIC_EnableFastINT0
 *
 * @brief   Set fast Interrupt 0
 *
 * @param   IRQn  - Interrupt Numbers
 * @param   addr  - interrupt service addr
 */
RV_STATIC_INLINE void PFIC_EnableFastINT0(IRQn_Type IRQn, uint32_t addr)
{
    PFIC->IDCFGR[0] = IRQn;
    PFIC->FIADDRR[0] = (addr & 0xFFFFFFFE) | 1;
}

/*********************************************************************
 * @fn      PFIC_EnableFastINT1
 *
 * @brief   Set fast Interrupt 1
 *
 * @param   IRQn  - Interrupt Numbers
 * @param   addr  - interrupt service addr
 */
RV_STATIC_INLINE void PFIC_EnableFastINT1(IRQn_Type IRQn, uint32_t addr)
{
    PFIC->IDCFGR[1] = IRQn;
    PFIC->FIADDRR[1] = (addr & 0xFFFFFFFE) | 1;
}

/*********************************************************************
 * @fn      PFIC_EnableFastINT2
 *
 * @brief   Set fast Interrupt 2
 *
 * @param   IRQn  - Interrupt Numbers
 * @param   addr  - interrupt service addr
 */
RV_STATIC_INLINE void PFIC_EnableFastINT2(IRQn_Type IRQn, uint32_t addr)
{
    PFIC->IDCFGR[2] = IRQn;
    PFIC->FIADDRR[2] = (addr & 0xFFFFFFFE) | 1;
}

/*********************************************************************
 * @fn      PFIC_EnableFastINT3
 *
 * @brief   Set fast Interrupt 3
 *
 * @param   IRQn  - Interrupt Numbers
 * @param   addr  - interrupt service addr
 */
RV_STATIC_INLINE void PFIC_EnableFastINT3(IRQn_Type IRQn, uint32_t addr)
{
    PFIC->IDCFGR[3] = IRQn;
    PFIC->FIADDRR[3] = (addr & 0xFFFFFFFE) | 1;
}

/*********************************************************************
 * @fn      PFIC_DisableFastINT0
 *
 * @brief   Disable fast Interrupt 0
 */
RV_STATIC_INLINE void PFIC_DisableFastINT0(void)
{
    PFIC->FIADDRR[0] = PFIC->FIADDRR[0] & 0xFFFFFFFE;
}

/*********************************************************************
 * @fn      PFIC_DisableFastINT1
 *
 * @brief   Disable fast Interrupt 1
 */
RV_STATIC_INLINE void PFIC_DisableFastINT1(void)
{
    PFIC->FIADDRR[1] = PFIC->FIADDRR[1] & 0xFFFFFFFE;
}

/*********************************************************************
 * @fn      PFIC_DisableFastINT2
 *
 * @brief   Disable fast Interrupt 2
 */
RV_STATIC_INLINE void PFIC_DisableFastINT2(void)
{
    PFIC->FIADDRR[2] = PFIC->FIADDRR[2] & 0xFFFFFFFE;
}

/*********************************************************************
 * @fn      PFIC_DisableFastINT3
 *
 * @brief   Disable fast Interrupt 3
 */
RV_STATIC_INLINE void PFIC_DisableFastINT3(void)
{
    PFIC->FIADDRR[3] = PFIC->FIADDRR[3] & 0xFFFFFFFE;
}

/*********************************************************************
 * @fn      __SEV
 *
 * @brief   Wait for Events
 */
__attribute__((always_inline)) RV_STATIC_INLINE void __SEV(void)
{
    PFIC->SCTLR |= (1 << 3);
}

/*********************************************************************
 * @fn      __WFI
 *
 * @brief   Wait for Interrupt
 */
__attribute__((always_inline)) RV_STATIC_INLINE void __WFI(void)
{
    PFIC->SCTLR &= ~(1 << 3); // wfi
    asm volatile("wfi");
}

/*********************************************************************
 * @fn      __WFE
 *
 * @brief   Wait for Events
 */
__attribute__((always_inline)) RV_STATIC_INLINE void __WFE(void)
{
    PFIC->SCTLR |= (1 << 3) | (1 << 5); // (wfi->wfe)+(__sev)
    asm volatile("wfi");
    PFIC->SCTLR |= (1 << 3);
    asm volatile("wfi");
}

/*********************************************************************
 * @fn      PFIC_SystemReset
 *
 * @brief   Initiate a system reset request
 */
RV_STATIC_INLINE void PFIC_SystemReset(void)
{
    PFIC->CFGR = PFIC_KEY3 | (1 << 7);
}

#define SysTick_LOAD_RELOAD_Msk    (0xFFFFFFFFFFFFFFFF)
#define SysTick_CTLR_INIT          (1 << 5)
#define SysTick_CTLR_MODE          (1 << 4)
#define SysTick_CTLR_STRE          (1 << 3)
#define SysTick_CTLR_STCLK         (1 << 2)
#define SysTick_CTLR_STIE          (1 << 1)
#define SysTick_CTLR_STE           (1 << 0)

#define SysTick_SR_CNTIF           (1 << 0)

RV_STATIC_INLINE uint32_t SysTick_Config(uint64_t ticks)
{
    if((ticks - 1) > SysTick_LOAD_RELOAD_Msk)
        return (1); /* Reload value impossible */

    SysTick->CMP = ticks - 1; /* set reload register */
    PFIC_EnableIRQ(SysTick_IRQn);
    SysTick->CTLR = SysTick_CTLR_INIT |
                    SysTick_CTLR_STRE |
                    SysTick_CTLR_STCLK |
                    SysTick_CTLR_STIE |
                    SysTick_CTLR_STE; /* Enable SysTick IRQ and SysTick Timer */
    return (0);                       /* Function successful */
}

#ifdef __cplusplus
}
#endif

#endif /* __CORE_RV3A_H__ */
