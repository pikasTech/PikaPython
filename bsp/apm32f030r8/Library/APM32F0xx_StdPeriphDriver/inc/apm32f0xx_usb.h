/*!
 * @file        apm32f10x_usb.h
 *
 * @brief       This file contains all the prototypes,enumeration and macros for USB peripheral     
 *
 * @version     V1.0.1
 *
 * @date        2021-07-01
 *
 */

#ifndef __USB_H
#define __USB_H

#include "apm32f0xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup USB_Driver  USB Driver
  @{
*/

/** @addtogroup  USB_Enumerations Enumerations
  @{
*/

/**
 * @brief   USB Endpoint register bit definition
 */
typedef enum
{
    USB_EP_BIT_ADDR     = (uint32_t)(BIT0 | BIT1 | BIT2 | BIT3),         
    USB_EP_BIT_TXSTS    = (uint32_t)(BIT4 | BIT5),
    USB_EP_BIT_TXDTOG   = (uint32_t)(BIT6),
    USB_EP_BIT_CTFT     = (uint32_t)(BIT7),
    USB_EP_BIT_KIND     = (uint32_t)(BIT8),
    USB_EP_BIT_TYPE     = (uint32_t)(BIT9 | BIT10),
    USB_EP_BIT_SETUP    = (uint32_t)(BIT11),
    USB_EP_BIT_RXSTS    = (uint32_t)(BIT12 | BIT13),
    USB_EP_BIT_RXDTOG   = (uint32_t)(BIT14),  
    USB_EP_BIT_CTFR     = (uint32_t)(BIT15)
}USB_EP_BIT_T;

/**
 * @brief   Endpoint id
 */
typedef enum
{
    USB_EP_0,
    USB_EP_1,
    USB_EP_2,
    USB_EP_3,
    USB_EP_4,
    USB_EP_5,
    USB_EP_6,
    USB_EP_7,
}USB_EP_T;

/**
 * @brief   Endpoint status
 */
typedef enum
{
    USB_EP_STATUS_DISABLE   = ((uint32_t)0),
    USB_EP_STATUS_STALL     = ((uint32_t)1),
    USB_EP_STATUS_NAK       = ((uint32_t)2),
    USB_EP_STATUS_VALID     = ((uint32_t)3),
}USB_EP_STATUS_T;

/**
 * @brief   USB Endpoint type
 */
typedef enum
{
    USB_EP_TYPE_BULK,
    USB_EP_TYPE_CONTROL,
    USB_EP_TYPE_ISOCHRONOUS,
    USB_EP_TYPE_INTERRUPT
}USB_EP_TYPE_T;

/*!
 * @brief       Set CTRL register
 *
 * @param       val:    Register value
 *
 * @retval      None
 *     
 */
#define USB_SetRegCTRL(val)         (USB->CTRL = val)

/*!
 * @brief       Set INTSTS register
 *
 * @param       val:    Register value
 *
 * @retval      None      
 */
#define USB_SetRegINTSTS(val)       (USB->INTSTS = val)

/*!
 * @brief       Set force reset
 *
 * @param       None
 *
 * @retval      None     
 */
#define USB_SetForceReset()         (USB->CTRL_B.FORRST = BIT_SET)

/*!
 * @brief       Reset force reset
 *
 * @param       None
 *
 * @retval      None     
 */
#define USB_ResetForceReset()       (USB->CTRL_B.FORRST = BIT_RESET)

/*!
 * @brief       Set power down
 *
 * @param       None
 *
 * @retval      None     
 */
#define USB_SetPowerDown()          (USB->CTRL_B.PWRDOWN = BIT_SET)

/*!
 * @brief       Reset power down
 *
 * @param       None
 *
 * @retval      None     
 */
#define USB_ResetPowerDown()        (USB->CTRL_B.PWRDOWN = BIT_RESET)

/*!
 * @brief       Set low power mode
 *
 * @param       None
 *
 * @retval      None     
 */
#define USB_SetLowerPowerMode()     (USB->CTRL_B.LPWREN = BIT_SET)

/*!
 * @brief       Ret low power mode
 *
 * @param       None
 *
 * @retval      None     
 */
#define USB_ResetLowerPowerMode()   (USB->CTRL_B.LPWREN = BIT_RESET)

/*!
 * @brief       Set force suspend
 *
 * @param       None
 *
 * @retval      None     
 */
#define USB_SetForceSuspend()       (USB->CTRL_B.FORSUS = BIT_SET)

/*!
 * @brief       Reset force suspend
 *
 * @param       None
 *
 * @retval      None     
 */
#define USB_ResetForceSuspend()     (USB->CTRL_B.FORSUS = BIT_RESET)

/*!
 * @brief       Read force suspend status
 *
 * @param       None
 *
 * @retval      None     
 */
#define USB_ReadForceSuspend()      (USB->CTRL_B.FORSUS)

/*!
 * @brief       Set resume
 *
 * @param       None
 *
 * @retval      None     
 */
#define USB_SetResume()             (USB->CTRL_B.WKUPREQ = BIT_SET)

/*!
 * @brief       Reset resume
 *
 * @param       None
 *
 * @retval      None     
 */
#define USB_ResetResume()           (USB->CTRL_B.WKUPREQ = BIT_RESET)

/*!
 * @brief       Enable interrupt 
 *
 * @param       int:    Interrupt source
 *
 * @retval      None     
 */
#define USB_EnableInterrupt(int)    (USB->CTRL |= int)

/*!
 * @brief       Disable interrupt 
 *
 * @param       int:    Interrupt source
 *
 * @retval      None     
 */
#define USB_DisableInterrupt(int)   (USB->CTRL &= (uint32_t)~int)

/*!
 * @brief       Read the specified interrupt flag status 
 *
 * @param       int:    Interrupt source
 *
 * @retval      Flag status.0 or not 0     
 */
#define USB_ReadIntFlag(int)        (USB->INTSTS & int)

/*!
 * @brief       Clear the specified interrupt flag status 
 *
 * @param       int:    Interrupt source
 *
 * @retval      None     
 */
#define USB_ClearIntFlag(int)       (USB->INTSTS &= (uint32_t)~int)

/*!
 * @brief       Read DOT field value in INTSTS rigister  
 *
 * @param       None
 *
 * @retval      DOT field value     
 */
#define USB_ReadDir()               (USB->INTSTS_B.DOT)

/*!
 * @brief       Read EPID field value in INTSTS rigister  
 *
 * @param       None
 *
 * @retval      EPIDfield value     
 */
#define USB_ReadEP()                ((USB_EP_T)(USB->INTSTS_B.EPID))

/*!
 * @brief       Read EP type
 *
 * @param       ep:     EP number
 *
 * @retval      EP type 
 */
#define USB_ReadEPType(ep)          (USB->EP[ep].EP_B.TYPE)

/*!
 * @brief       Read EP Tx status
 *
 * @param       ep:     EP number
 *
 * @retval      EP Tx status       
 */
#define USB_ReadEPTxStatus(ep)      ((USB_EP_STATUS_T)(USB->EP[ep].EP_B.TXSTS))     

/*!
 * @brief       Read EP Rx status
 *
 * @param       ep:     EP number
 *
 * @retval      EP Rx status   
 */
#define USB_ReadEPRxStatus(ep)      ((USB_EP_STATUS_T)(USB->EP[ep].EP_B.RXSTS))

/*!
 * @brief       Read EP Tx address pointer  
 *
 * @param       ep:     EP number
 *
 * @retval      EP Tx address pointer      
 */
#define USB_ReadEPTxAddrPointer(ep) (uint16_t *)((USB->BUFTB + ep * 8)/* * 2*/ + USB_PMA_ADDR)          


/*!
 * @brief       Read EP Tx count pointer 
 *
 * @param       ep:     EP number
 *
 * @retval      EP Tx count pointer      
 */
#define USB_ReadEPTxCntPointer(ep)  (uint16_t *)((USB->BUFTB + ep * 8 + 2)/* * 2*/ + USB_PMA_ADDR)  

/*!
 * @brief       Read EP Rx address pointer 
 *
 * @param       ep:     EP number
 *
 * @retval      EP Rx address pointer      
 */
#define USB_ReadEPRxAddrPointer(ep) (uint16_t *)((USB->BUFTB + ep * 8 + 4)/* * 2*/ + USB_PMA_ADDR)

/*!
 * @brief       Read EP Rx count pointer
 *
 * @param       ep:     EP number
 *
 * @retval      EP Rx count pointer      
 */
#define USB_ReadEPRxCntPointer(ep)  (uint16_t *)((USB->BUFTB + ep * 8 + 6)/* * 2*/ + USB_PMA_ADDR) 

/*!
 * @brief       Set EP Tx addr
 *
 * @param       ep:     EP number
 *
 * @param       addr:   Tx addr
 *
 * @retval      None    
 */
#define USB_SetEPTxAddr(ep, addr)   (*USB_ReadEPTxAddrPointer(ep) = (addr >> 1) << 1)

/*!
 * @brief       Set EP Rx addr
 *
 * @param       ep:     EP number
 *
 * @param       addr:   Rx addr
 *
 * @retval      None    
 */
#define USB_SetEPRxAddr(ep, addr)   (*USB_ReadEPRxAddrPointer(ep) = (addr >> 1) << 1)

/*!
 * @brief       Read EP Tx addr
 *
 * @param       ep:     EP number
 *
 * @retval      EP Tx addr       
 */
#define USB_ReadEPTxAddr(ep)        ((uint16_t)*USB_ReadEPTxAddrPointer(ep))

/*!
 * @brief       Read EP Rx addr
 *
 * @param       ep:     EP number
 *
 * @retval      EP Rx addr     
 */
#define USB_ReadEPRxAddr(ep)        ((uint16_t)*USB_ReadEPRxAddrPointer(ep))

/*!
 * @brief       Set EP Rx Count
 *
 * @param       ep:     EP number
 *
 * @param       cnt:    Tx count
 *
 * @retval      None     
 */
#define USB_SetEPTxCnt(ep, cnt)     (*USB_ReadEPTxCntPointer(ep) = cnt)

/*!
 * @brief       Read EP Tx count
 *
 * @param       ep:     EP number
 *
 * @retval      EP Tx count     
 */
#define USB_ReadEPTxCnt(ep)        ((uint16_t)*USB_ReadEPTxCntPointer(ep) & 0x3ff)

/*!
 * @brief       Read EP Rx count
 *
 * @param       ep:     EP number
 *
 * @retval      EP Rx count       
 */
#define USB_ReadEPRxCnt(ep)        ((uint16_t)*USB_ReadEPRxCntPointer(ep) & 0x3ff)

/*!
 * @brief       Read SETUP field value in EP register
 *
 * @param       ep:     EP number
 *
 * @retval      SETUP field value      
 */
#define USB_ReadEPSetup(ep)         (USB->EP[ep].EP_B.SETUP)

/*!
 * @brief       Set buffer table value
 *
 * @param       tab:    Buffer table value
 *
 * @retval      None      
 */
#define USB_SetBufferTable(tab)     (USB->BUFTB_B.BUFFTB = tab)

/*!
 * @brief       Set device address
 *
 * @param       addr:   Device address
 *
 * @retval      None      
 */
#define USB_SetDeviceAddr(addr)     (USB->ADDR_B.ADDR = addr)

/*!
 * @brief       Read CTFR field value in EP register
 *
 * @param       ep: Endpoint number
 *
 * @retval      CTFR field value      
 */
#define USB_ReadEPRxFlag(ep)        (USB->EP[ep].EP_B.CTFR)

/*!
 * @brief       Read CTFT field value in EP register
 *
 * @param       ep: Endpoint number
 *
 * @retval      CTFT field value      
 */
#define USB_ReadEPTxFlag(ep)        (USB->EP[ep].EP_B.CTFT)

/*!
 * @brief       Enable USB peripheral
 *
 * @param       None
 *
 * @retval      None      
 */
#define USB_Enable()                (USB->ADDR_B.USBDEN = BIT_SET)

/*!
 * @brief       Disable USB peripheral
 *
 * @param       None
 *
 * @retval      None      
 */
#define USB_Disable()               (USB->ADDR_B.USBDEN = BIT_RESET)
                                    
/*!
 * @brief       Enable USB2 peripheral
 *
 * @param       None
 *
 * @retval      None      
 */
#define USB2_Enable()               (USB->SW = BIT_SET)
                                    
/*!
 * @brief       Disable USB2 peripheral
 *
 * @param       None
 *
 * @retval      None      
 */
#define USB2_Disable()              (USB->SW = BIT_RESET)
                                    
/*!
 * @brief       Read RXDPSTS field value in FRANUM register
 *
 * @param       None
 *
 * @retval      RXDPSTS field value      
 */
#define USB_ReadRDPS()              (USB->FRANUM_B.RXDPSTS)

/*!
 * @brief       Read RXDMSTS field value in FRANUM register
 *
 * @param       None
 *
 * @retval      RXDMSTS field value      
 */
#define USB_ReadRDMS()              (USB->FRANUM_B.RXDPSTS)

/*!
 * @brief       Read LOCK field value in FRANUM register
 *
 * @param       None
 *
 * @retval      LOCK field value      
 */
#define USB_ReadLOCK()              (USB->FRANUM_B.LOCK)

/*!
 * @brief       Read LSOFNUM field value in FRANUM register
 *
 * @param       None
 *
 * @retval      LSOFNUM field value      
 */
#define USB_ReadLSOF()              (USB->FRANUM_B.LSOFNUM)
                                    
/*!
 * @brief       Read FRANUM field value in FRANUM register
 *
 * @param       None
 *
 * @retval      FRANUM field value      
 */
#define USB_ReadFRANUM()            (USB->FRANUM_B.FRANUM)
#define USB_SetBESL(val)            (USB->LPMCTRLSTS_B.BESL = val) 
#define USB_SetRemoteWakeVal(val)   (USB->LPMCTRLSTS_B.REMWAKE = val)
#define USB_EnableAckLPM()          (USB->LPMCTRLSTS_B.LPMACKEN = 1)
#define USB_DisableAckLPM()         (USB->LPMCTRLSTS_B.LPMACKEN = 0)
#define USB_EnableLPM()             (USB->LPMCTRLSTS_B.LPMEN = 1)
#define USB_DisableLPM()            (USB->LPMCTRLSTS_B.LPMEN = 0)
#define USB_EnablePullUpDP()        (USB->BCD_B.DPPUCTRL = 1)
#define USB_DisablePullUpDP()       (USB->BCD_B.DPPUCTRL = 0)
#define USB_DMPullUpStatus()        (USB->BCD_B.DMPUDFLG)
#define USB_SDStatus()              (USB->BCD_B.SDFLG)
#define USB_PDStatus()              (USB->BCD_B.PDFLG)
#define USB_DCDStatus()             (USB->BCD_B.DCDFLG)
#define USB_EnablePDMode()          (USB->BCD_B.PDEN = 1)
#define USB_DisablePDMode()         (USB->BCD_B.PDEN = 0)
#define USB_EnableSDMode()          (USB->BCD_B.SDEN = 1)
#define USB_DisableSDMode()         (USB->BCD_B.SDEN = 0)
#define USB_EnableDCDMode()         (USB->BCD_B.DCDEN = 1)
#define USB_DisableDCDMode()        (USB->BCD_B.DCDEN = 0)
#define USB_EnableBCD()             (USB->BCD_B.BCDEN = 2)
#define USB_DisableBCD()            (USB->BCD_B.BCDEN = 0)

/** @addtogroup ADC_Macros Macros
  @{
*/

/** USB packet memory area base address */
#define USB_PMA_ADDR            (0x40006000L)

/** Endpoint register mask value default */
#define USB_EP_MASK_DEFAULT     (USB_EP_BIT_CTFR | USB_EP_BIT_SETUP | USB_EP_BIT_TYPE | USB_EP_BIT_KIND | USB_EP_BIT_CTFT |USB_EP_BIT_ADDR)

/**
 * @brief   USB interrupt source
 */
#define USB_INT_ESOF            0X100
#define USB_INT_SOF             0X200
#define USB_INT_RESET           0X400
#define USB_INT_SUSPEND         0x800
#define USB_INT_WAKEUP          0X1000
#define USB_INT_ERROR           0X2000
#define USB_INT_PMAOU           0X4000
#define USB_INT_CT              0X8000
#define USB_INT_ALL             0XFF00    

/**@} end of group ADC_Macros*/

/** @addtogroup  USB_Fuctions Fuctions
  @{
*/
void USB_SetEPType(USB_EP_T ep, USB_EP_TYPE_T type);

void USB_SetEPKind(USB_EP_T ep);
void USB_ResetEPKind(USB_EP_T ep);

void USB_ResetEPRxFlag(USB_EP_T ep);
void USB_ResetEPTxFlag(USB_EP_T ep);

void USB_ToggleTx(USB_EP_T ep);
void USB_ToggleRx(USB_EP_T ep);
void USB_ResetTxToggle(USB_EP_T ep);
void USB_ResetRxToggle(USB_EP_T ep);

void USB_SetEpAddr(USB_EP_T ep, uint8_t addr);

void USB_SetEPTxStatus(USB_EP_T ep, USB_EP_STATUS_T status);
void USB_SetEPRxStatus(USB_EP_T ep, USB_EP_STATUS_T status);
void USB_SetEPRxTxStatus(USB_EP_T ep, USB_EP_STATUS_T txStatus, USB_EP_STATUS_T rxStatus);

void USB_SetEPRxCnt(USB_EP_T ep, uint32_t cnt);

void USB_WriteDataToEP(USB_EP_T ep, uint8_t *wBuf, uint32_t wLen);
void USB_ReadDataFromEP(USB_EP_T ep, uint8_t *rBuf, uint32_t rLen);

/**@} end of group USB_Fuctions*/
/**@} end of group USB_Driver */
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /* __USB_H */
