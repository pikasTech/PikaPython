/*----------------------------------------------------------------------------
 * Name:    SMM_MPS2.h
 * Purpose: SMM MPS2 definitions
 *----------------------------------------------------------------------------*/

/* Copyright (c) 2011 - 2017 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/

#ifndef SMM_MPS2_H_
#define SMM_MPS2_H_

#include "Device.h"                         /* device specific header file */

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/*----------------------------------------------------------------------------
  FPGA System Register declaration
 *----------------------------------------------------------------------------*/
typedef struct                   /* Document SMM_M3.doc */
{
  __IO uint32_t LED;             /* Offset: 0x000 (R/W)  LED connections */
                                 /*                         [31:2] : Reserved */
                                 /*                          [1:0] : LEDs */
       uint32_t RESERVED0[1];
  __IO uint32_t BUTTON;          /* Offset: 0x008 (R/W)  Buttons */
                                 /*                         [31:2] : Reserved */
                                 /*                          [1:0] : Buttons */
       uint32_t RESERVED1[1];
  __IO uint32_t CLK1HZ;          /* Offset: 0x010 (R/W)  1Hz up counter */
  __IO uint32_t CLK100HZ;        /* Offset: 0x014 (R/W)  100Hz up counter */
  __IO uint32_t COUNTER;         /* Offset: 0x018 (R/W)  Cycle Up Counter */
                                 /*                         Increments when 32-bit prescale counter reach zero */
       uint32_t RESERVED2[1];
  __IO uint32_t PRESCALE;        /* Offset: 0x020 (R/W)  Prescaler */
                                 /*                         Bit[31:0] : reload value for prescale counter */
  __IO uint32_t PSCNTR;          /* Offset: 0x024 (R/W)  32-bit Prescale counter */
                                 /*                         current value of the pre-scaler counter */
								 /*                         The Cycle Up Counter increment when the prescale down counter reach 0 */
								 /*                         The pre-scaler counter is reloaded with PRESCALE after reaching 0 */
       uint32_t RESERVED3[9];
  __IO uint32_t MISC;            /* Offset: 0x04C (R/W)  Misc control */
                                 /*                         [31:7] : Reserved */
                                 /*                            [6] : CLCD_BL_CTRL */
                                 /*                            [5] : CLCD_RD */
                                 /*                            [4] : CLCD_RS */
                                 /*                            [3] : CLCD_RESET */
                                 /*                            [2] : RESERVED */
                                 /*                            [1] : SPI_nSS */
                                 /*                            [0] : CLCD_CS */
} MPS2_FPGAIO_TypeDef;


/*----------------------------------------------------------------------------
  SCC Register declaration
 *----------------------------------------------------------------------------*/
typedef struct                   /* Document SMM_M3.doc */
{
  __IO uint32_t CFG_REG0;        /* Offset: 0x000 (R/W)  Remaps block RAM to ZBT */
                                 /*                         [31:1] : Reserved */
                                 /*                            [0] 1 : REMAP BlockRam to ZBT */
  __IO uint32_t CFG_REG1;        /* Offset: 0x004 (R/W)  Controls the MCC user LEDs */
                                 /*                         [31:8] : Reserved */
                                 /*                          [7:0] : MCC LEDs */
  __I  uint32_t CFG_REG2;        /* Offset: 0x008 (R/ )  Denotes the state of the MCC user switches */
                                 /*                         [31:8] : Reserved */
                                 /*                          [7:0] : These bits indicate state of the MCC switches */
  __I  uint32_t CFG_REG3;        /* Offset: 0x00C (R/ )  Denotes the board revision */
                                 /*                         [31:4] : Reserved */
                                 /*                          [3:0] : Used by the MCC to pass PCB revision. 0 = A 1 = B */
       uint32_t RESERVED0[36];
  __IO uint32_t SYS_CFGDATA_RTN; /* Offset: 0x0A0 (R/W)  User data register */
                                 /*                         [31:0] : Data */
  __IO uint32_t SYS_CFGDATA_OUT; /* Offset: 0x0A4 (R/W)  User data register */
                                 /*                         [31:0] : Data */
  __IO uint32_t SYS_CFGCTRL;     /* Offset: 0x0A8 (R/W)  Control register */
                                 /*                           [31] : Start (generates interrupt on write to this bit) */
                                 /*                           [30] : R/W access */
                                 /*                        [29:26] : Reserved */
                                 /*                        [25:20] : Function value */
                                 /*                        [19:12] : Reserved */
                                 /*                         [11:0] : Device (value of 0/1/2 for supported clocks) */
  __IO uint32_t SYS_CFGSTAT;     /* Offset: 0x0AC (R/W)  Contains status information */
                                 /*                         [31:2] : Reserved */
                                 /*                            [1] : Error */
                                 /*                            [0] : Complete */
  __IO uint32_t RESERVED1[20];
  __IO uint32_t SCC_DLL;         /* Offset: 0x100 (R/W)  DLL Lock Register */
                                 /*                        [31:24] : DLL LOCK MASK[7:0] - Indicate if the DLL locked is masked */
                                 /*                        [23:16] : DLL LOCK MASK[7:0] - Indicate if the DLLs are locked or unlocked */
                                 /*                         [15:1] : Reserved */
                                 /*                            [0] : This bit indicates if all enabled DLLs are locked */
       uint32_t RESERVED2[957];
  __I  uint32_t SCC_AID;         /* Offset: 0xFF8 (R/ )  SCC AID Register */
                                 /*                        [31:24] : FPGA build number */
                                 /*                        [23:20] : V2M-MPS2 target board revision (A = 0, B = 1) */
                                 /*                        [19:11] : Reserved */
                                 /*                           [10] : if “1” SCC_SW register has been implemented */
                                 /*                            [9] : if “1” SCC_LED register has been implemented */
                                 /*                            [8] : if “1” DLL lock register has been implemented */
                                 /*                          [7:0] : number of SCC configuration register */
  __I  uint32_t SCC_ID;          /* Offset: 0xFFC (R/ )  Contains information about the FPGA image */
                                 /*                        [31:24] : Implementer ID: 0x41 = ARM */
                                 /*                        [23:20] : Application note IP variant number */
                                 /*                        [19:16] : IP Architecture: 0x4 =AHB */
                                 /*                         [11:4] : Primary part number: 386 = AN386 */
                                 /*                          [3:0] : Application note IP revision number */
} MPS2_SCC_TypeDef;


/*----------------------------------------------------------------------------
  SSP Peripheral declaration  (Document DDI0194G_ssp_pl022_r1p3_trm.pdf)
 *----------------------------------------------------------------------------*/
typedef struct                   /* Document DDI0194G_ssp_pl022_r1p3_trm.pdf */
{
  __IO uint32_t CR0;             /* Offset: 0x000 (R/W)  Control register 0 */
                                 /*                        [31:16] : Reserved */
                                 /*                         [15:8] : Serial clock rate */
                                 /*                            [7] : SSPCLKOUT phase,    applicable to Motorola SPI frame format only */
                                 /*                            [6] : SSPCLKOUT polarity, applicable to Motorola SPI frame format only */
                                 /*                          [5:4] : Frame format */
                                 /*                          [3:0] : Data Size Select */
  __IO uint32_t CR1;             /* Offset: 0x004 (R/W)  Control register 1 */
                                 /*                         [31:4] : Reserved */
                                 /*                            [3] : Slave-mode output disable */
                                 /*                            [2] : Master or slave mode select */
                                 /*                            [1] : Synchronous serial port enable */
                                 /*                            [0] : Loop back mode */
  __IO uint32_t DR;              /* Offset: 0x008 (R/W)  Data register */
                                 /*                        [31:16] : Reserved */
                                 /*                         [15:0] : Transmit/Receive FIFO */
  __I  uint32_t SR;              /* Offset: 0x00C (R/ )  Status register */
                                 /*                         [31:5] : Reserved */
                                 /*                            [4] : PrimeCell SSP busy flag */
                                 /*                            [3] : Receive FIFO full */
                                 /*                            [2] : Receive FIFO not empty */
                                 /*                            [1] : Transmit FIFO not full */
                                 /*                            [0] : Transmit FIFO empty */
  __IO uint32_t CPSR;            /* Offset: 0x010 (R/W)  Clock prescale register */
                                 /*                         [31:8] : Reserved */
                                 /*                          [8:0] : Clock prescale divisor */
  __IO uint32_t IMSC;            /* Offset: 0x014 (R/W)  Interrupt mask set or clear register */
                                 /*                         [31:4] : Reserved */
                                 /*                            [3] : Transmit FIFO interrupt mask */
                                 /*                            [2] : Receive FIFO interrupt mask */
                                 /*                            [1] : Receive timeout interrupt mask */
                                 /*                            [0] : Receive overrun interrupt mask */
  __I  uint32_t RIS;             /* Offset: 0x018 (R/ )  Raw interrupt status register */
                                 /*                         [31:4] : Reserved */
                                 /*                            [3] : raw interrupt state, prior to masking, of the SSPTXINTR interrupt */
                                 /*                            [2] : raw interrupt state, prior to masking, of the SSPRXINTR interrupt */
                                 /*                            [1] : raw interrupt state, prior to masking, of the SSPRTINTR interrupt */
                                 /*                            [0] : raw interrupt state, prior to masking, of the SSPRORINTR interrupt */
  __I  uint32_t MIS;             /* Offset: 0x01C (R/ )  Masked interrupt status register */
                                 /*                         [31:4] : Reserved */
                                 /*                            [3] : transmit FIFO masked interrupt state, after masking, of the SSPTXINTR interrupt */
                                 /*                            [2] : receive FIFO masked interrupt state, after masking, of the SSPRXINTR interrupt */
                                 /*                            [1] : receive timeout masked interrupt state, after masking, of the SSPRTINTR interrupt */
                                 /*                            [0] : receive over run masked interrupt status, after masking, of the SSPRORINTR interrupt */
  __O  uint32_t ICR;             /* Offset: 0x020 ( /W)  Interrupt clear register */
                                 /*                         [31:2] : Reserved */
                                 /*                            [1] : Clears the SSPRTINTR interrupt */
                                 /*                            [0] : Clears the SSPRORINTR interrupt */
  __IO uint32_t DMACR;           /* Offset: 0x024 (R/W)  DMA control register */
                                 /*                         [31:2] : Reserved */
                                 /*                            [1] : Transmit DMA Enable */
                                 /*                            [0] : Receive DMA Enable */
} MPS2_SSP_TypeDef;


/*----------------------------------------------------------------------------
  I2C Peripheral declaration
 *----------------------------------------------------------------------------*/
typedef struct                   /* Document ? */
{
  union {
  __O  uint32_t CONTROLS;        /* Offset: 0x000 ( /W)  CONTROL Set Register */
  __I  uint32_t CONTROL;         /* Offset: 0x000 (R/ )  CONTROL Status Register */
  };
                                 /*                         [31:2] : Reserved */
                                 /*                            [1] : SDA */
                                 /*                            [0] : SCL */
  __O    uint32_t  CONTROLC;     /* Offset: 0x004 ( /W)  CONTROL Clear Register */
} MPS2_I2C_TypeDef;



/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/

#define MPS2_FPGAIO_BASE        (0x40028000ul)       /*!< FPGAIO Base Address */
#define MPS2_SCC_BASE           (0x4002F000ul)       /*!< SCC Base Address    */
#define MPS2_SSP0_BASE          (0x40021000ul)       /*!< SSP0 Base Address   */
#define MPS2_I2C0_BASE          (0x40022000ul)       /*!< I2C0 Base Address   */


/******************************************************************************/
/*                         Peripheral declaration                             */
/******************************************************************************/

#define MPS2_I2C0               ((MPS2_I2C_TypeDef      *) MPS2_I2C0_BASE )
#define MPS2_FPGAIO             ((MPS2_FPGAIO_TypeDef   *) MPS2_FPGAIO_BASE )
#define MPS2_SCC                ((MPS2_SCC_TypeDef      *) MPS2_SCC_BASE )
#define MPS2_SSP0               ((MPS2_SSP_TypeDef      *) MPS2_SSP0_BASE )

#endif /* SMM_MPS2_H_ */
