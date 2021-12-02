/*!
 * @file        apm32f10x_usb.c
 *
 * @brief       This file contains all the functions for the USB peripheral 
 *
 * @version     V1.0.1
 *
 * @date        2021-07-01
 *
 */
 
#include "apm32f0xx_usb.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup USB_Driver  USB Driver
  @{
*/

/** @addtogroup  USB_Fuctions Fuctions
  @{
*/

/*!
 * @brief       Set Endpoint type
 *
 * @param       ep:     Endpoint number
 *
 * @param       type:   Endpoint type
 *
 * @retval      None      
 */
void USB_SetEPType(USB_EP_T ep, USB_EP_TYPE_T type)
{
    __IOM uint32_t reg;

    reg = USB->EP[ep].EP;

    reg &= (uint32_t)(USB_EP_MASK_DEFAULT);
    reg &= ~USB_EP_BIT_TYPE;
    reg |= type << 9;
    
    USB->EP[ep].EP = reg;
}

/*!
 * @brief       Set EP kind
 *
 * @param       ep:     Endpoint number
 *
 * @retval      None       
 */
void USB_SetEPKind(USB_EP_T ep)
{
    __IOM uint32_t reg;

    reg = USB->EP[ep].EP;

    reg &= (uint32_t)(USB_EP_MASK_DEFAULT);
    reg |= USB_EP_BIT_KIND;
    
    USB->EP[ep].EP = reg;
}

/*!
 * @brief       Reset EP kind 
 *
 * @param       ep:     Endpoint number
 *
 * @retval      None      
 */
void USB_ResetEPKind(USB_EP_T ep)
{
    __IOM uint32_t reg;

    reg = USB->EP[ep].EP;

    reg &= (uint32_t)(USB_EP_MASK_DEFAULT);
    reg &= ~USB_EP_BIT_KIND;
    
    USB->EP[ep].EP = reg;
}


/*!
 * @brief       Reset EP CTFR bit
 *
 * @param       ep: Endpoint number
 *
 * @retval      None      
 */
void USB_ResetEPRxFlag(USB_EP_T ep)
{
    __IOM uint32_t reg;

    reg = USB->EP[ep].EP;

    reg &= (uint32_t)(USB_EP_MASK_DEFAULT);
    reg &= ~USB_EP_BIT_CTFR;
    
    USB->EP[ep].EP = reg;
}

/*!
 * @brief       Reset EP CTFT bit
 *
 * @param       ep: Endpoint number
 *
 * @retval      None      
 */
void USB_ResetEPTxFlag(USB_EP_T ep)
{
    __IOM uint32_t reg;

    reg = USB->EP[ep].EP;

    reg &= (uint32_t)(USB_EP_MASK_DEFAULT);
    reg &= ~USB_EP_BIT_CTFT;
    
    USB->EP[ep].EP = reg;
}

/*!
 * @brief       Toggle Tx DTOG
 *
 * @param       ep: Endpoint number
 *
 * @retval      None      
 */
void USB_ToggleTx(USB_EP_T ep)
{
    __IOM uint32_t reg;

    reg = USB->EP[ep].EP;
    
    reg &= (uint32_t)(USB_EP_MASK_DEFAULT);
    reg |= USB_EP_BIT_TXDTOG;
    
    USB->EP[ep].EP = reg;
}

/*!
 * @brief       Toggle Rx DTOG
 *
 * @param       ep: Endpoint number
 *
 * @retval      None       
 */
void USB_ToggleRx(USB_EP_T ep)
{
    __IOM uint32_t reg;

    reg = USB->EP[ep].EP;
    
    reg &= (uint32_t)(USB_EP_MASK_DEFAULT);
    reg |= USB_EP_BIT_RXDTOG;
    
    USB->EP[ep].EP = reg;
}

/*!
 * @brief       Reset Toggle Tx DTOG
 *
 * @param       ep: Endpoint number
 *
 * @retval      None       
 */
void USB_ResetTxToggle(USB_EP_T ep)
{
    if(USB->EP[ep].EP_B.TXDTOG)
    {
        USB_ToggleTx(ep);
    }
}

/*!
 * @brief       Reset Toggle Rx DTOG
 *
 * @param       ep: Endpoint number
 *
 * @retval      None       
 */
void USB_ResetRxToggle(USB_EP_T ep)
{
    if(USB->EP[ep].EP_B.RXDTOG)
    {
        USB_ToggleRx(ep);
    }
}

/*!
 * @brief       Set EP address
 *
 * @param       ep:     Endpoint number
 *
 * @param       addr:   Address
 *
 * @retval      None     
 */
void USB_SetEpAddr(USB_EP_T ep, uint8_t addr)
{
    __IOM uint32_t reg;

    reg = USB->EP[ep].EP;

    reg &= (uint32_t)(USB_EP_MASK_DEFAULT);
    reg &= ~USB_EP_BIT_ADDR;
    reg |= addr;
    
    USB->EP[ep].EP = reg;
}

/*!
 * @brief       Set EP Tx status
 *
 * @param       ep:     Endpoint number
 *
 * @param       status: status
 *
 * @retval      None      
 */
void USB_SetEPTxStatus(USB_EP_T ep, USB_EP_STATUS_T status)
{
    __IOM uint32_t reg;

    status <<= 4;
    
    reg = USB->EP[ep].EP;
    
    reg &= (uint32_t)(USB_EP_MASK_DEFAULT | USB_EP_BIT_TXSTS);
    reg ^= (status & USB_EP_BIT_TXSTS);

    USB->EP[ep].EP = reg;
}

/*!
 * @brief       Set EP Rx status
 *
 * @param       ep:     Endpoint number
 *
 * @param       status: status
 *
 * @retval      None      
 */
void USB_SetEPRxStatus(USB_EP_T ep, USB_EP_STATUS_T status)
{
    __IOM uint32_t reg;
    uint32_t tmp;
    
    tmp = status << 12;
    
    reg = USB->EP[ep].EP;
    
    reg &= (uint32_t)(USB_EP_MASK_DEFAULT | USB_EP_BIT_RXSTS);
    reg ^= (tmp & USB_EP_BIT_RXSTS);

    USB->EP[ep].EP = reg;
}


/*!
 * @brief       Set EP Rx and Txstatus
 *
 * @param       ep:     Endpoint number
 *
 * @param       status: status
 *
 * @retval      None      
 */
void USB_SetEPRxTxStatus(USB_EP_T ep, USB_EP_STATUS_T txStatus, USB_EP_STATUS_T rxStatus)
{
    __IOM uint32_t reg;
    uint32_t tmp;

    reg = USB->EP[ep].EP;
    
    reg &= (uint32_t)(USB_EP_MASK_DEFAULT | USB_EP_BIT_RXSTS | USB_EP_BIT_TXSTS);
    
    tmp = rxStatus << 12;
    reg ^= (tmp & USB_EP_BIT_RXSTS);
    
    tmp = txStatus << 4;
    reg ^= (tmp & USB_EP_BIT_TXSTS);
    
    USB->EP[ep].EP = reg;
}

/*!
 * @brief       Set EP Rx Count 
 *
 * @param       ep:     Endpoint number
 *
 * @param       cnt:    Rx count
 *
 * @retval      None      
 */
void USB_SetEPRxCnt(USB_EP_T ep, uint32_t cnt)
{
    __IOM uint16_t *p;
    __IOM uint32_t block = 0;
    
    p = USB_ReadEPRxCntPointer(ep);

    if(cnt > 62)
    {
        block = cnt >> 5;
        
        if(!(cnt & 0x1f))
        {
            block -= 1;
        }

        *p = (block << 10) | 0x8000;
    }
    else
    {
        block = cnt >> 1;
        
        if(cnt & 0x01)
        {
            block += 1;
        }

        *p = (block << 10);
    }
}

/*!
 * @brief       Write a buffer of data to a selected endpoint
 *
 * @param       ep:     Endpoint number
 *
 * @retval      wBuf:   The pointer to the buffer of data to be written to the endpoint
 *
 * @param       wLen:   Number of data to be written (in bytes)
 *
 * @retval      None    
 */
void USB_WriteDataToEP(USB_EP_T ep, uint8_t *wBuf, uint32_t wLen)
{
    uint32_t i;
    uint16_t *addrEP;
    uint16_t tmp;    
    wLen = (wLen + 1) >> 1;

    addrEP = (uint16_t *)(USB_ReadEPTxAddr(ep) + USB_PMA_ADDR);

    for(i = 0; i < wLen; i++)
    {
        tmp = *wBuf++;
        tmp = ((*wBuf++) << 8) | tmp;

        *addrEP++ = tmp;
    }
}

/*!
 * @brief       Read a buffer of data to a selected endpoint
 *
 * @param       ep:     Endpoint number
 *
 * @retval      wBuf:   The pointer to the buffer of data to be read to the endpoint
 *
 * @param       wLen:   Number of data to be read (in bytes)
 *
 * @retval      None     
 */
void USB_ReadDataFromEP(USB_EP_T ep, uint8_t *rBuf, uint32_t rLen)
{
    uint32_t i;
    uint16_t *addrEP;
    uint16_t tmp;
    
    rLen = (rLen + 1) >> 1;

    addrEP = (uint16_t *)(USB_ReadEPRxAddr(ep) + USB_PMA_ADDR);
    
    for(i = 0; i < rLen; i++)
    {
        tmp = *addrEP++;
        *rBuf++ = tmp & 0XFF;
        *rBuf++ = (tmp >> 8) & 0xff; 
    }
}

/**@} end of group USB_Fuctions*/
/**@} end of group USB_Driver */
/**@} end of group Peripherals_Library*/
