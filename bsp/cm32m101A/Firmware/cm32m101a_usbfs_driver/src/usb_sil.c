/*
  ******************************************************************************
  *
  * COPYRIGHT(c) 2020, China Mobile IOT
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of China Mobile IOT nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 

/**
 * @file usb_sil.c
 * @author CMIOT
 * @version v1.0.0
 *
 * @COPYRIGHT(c) 2020, China Mobile IOT. All rights reserved.
 */
#include "usb_lib.h"

/**
 * @brief Initialize the USB Device IP and the Endpoint 0.
 * @return Status.
 */
uint32_t USB_SilInit(void)
{
    /* USB interrupts initialization */
    /* clear pending interrupts */
    _SetISTR(0);
    wInterrupt_Mask = IMR_MSK;
    /* set interrupts mask */
    _SetCNTR(wInterrupt_Mask);
    return 0;
}

/**
 * @brief Write a buffer of data to a selected endpoint.
 * @param bEpAddr The address of the non control endpoint.
 * @param pBufferPointer The pointer to the buffer of data to be written to the endpoint.
 * @param wBufferSize Number of data to be written (in bytes).
 * @return Status.
 */
uint32_t USB_SilWrite(uint8_t bEpAddr, uint8_t* pBufferPointer, uint32_t wBufferSize)
{
    /* Use the memory interface function to write to the selected endpoint */
    USB_CopyUserToPMABuf(pBufferPointer, USB_GetEpTxAddr(bEpAddr & 0x7F), wBufferSize);
    /* Update the data length in the control register */
    USB_SetEpTxCnt((bEpAddr & 0x7F), wBufferSize);
    return 0;
}

/**
 * @brief Write a buffer of data to a selected endpoint.
 * @param bEpAddr The address of the non control endpoint.
 * @param pBufferPointer The pointer to which will be saved the received data buffer.
 * @return Number of received data (in Bytes).
 */
uint32_t USB_SilRead(uint8_t bEpAddr, uint8_t* pBufferPointer)
{
    uint32_t DataLength = 0;
    /* Get the number of received data on the selected Endpoint */
    DataLength = USB_GetEpRxCnt(bEpAddr & 0x7F);
    /* Use the memory interface function to write to the selected endpoint */
    USB_CopyPMAToUserBuf(pBufferPointer, USB_GetEpRxAddr(bEpAddr & 0x7F), DataLength);
    /* Return the number of received data */
    return DataLength;
}
