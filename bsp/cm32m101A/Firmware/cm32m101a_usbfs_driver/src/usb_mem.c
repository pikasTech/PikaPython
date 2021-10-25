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
 * @file usb_mem.c
 * @author CMIOT
 * @version v1.0.0
 *
 * @COPYRIGHT(c) 2020, China Mobile IOT. All rights reserved.
 */
#include "usb_lib.h"
u8* EpOutDataPtrTmp;
u8* EpInDataPtrTmp;

/**
 * @brief Copy a buffer from user memory area to packet memory area (PMA)
 * @param pbUsrBuf pointer to user memory area.
 * @param wPMABufAddr address into PMA.
 * @param wNBytes no. of bytes to be copied.
 */
void USB_CopyUserToPMABuf(uint8_t* pbUsrBuf, uint16_t wPMABufAddr, uint16_t wNBytes)
{
    uint32_t n = (wNBytes + 1) >> 1; /* n = (wNBytes + 1) / 2 */
    uint32_t i, temp1, temp2;
    uint16_t* pdwVal;
    pdwVal = (uint16_t*)(wPMABufAddr * 2 + PMAAddr);
    for (i = n; i != 0; i--)
    {
        temp1 = (uint16_t)*pbUsrBuf;
        pbUsrBuf++;
        temp2     = temp1 | (uint16_t)*pbUsrBuf << 8;
        *pdwVal++ = temp2;
        pdwVal++;
        pbUsrBuf++;
        EpInDataPtrTmp = pbUsrBuf;
    }
}

/**
 * @brief Copy a buffer from user memory area to packet memory area (PMA)
 * @param pbUsrBuf    pointer to user memory area.
 * @param wPMABufAddr address into PMA.
 * @param wNBytes     no. of bytes to be copied.
 */
void USB_CopyPMAToUserBuf(uint8_t* pbUsrBuf, uint16_t wPMABufAddr, uint16_t wNBytes)
{
    uint32_t n = (wNBytes + 1) >> 1; /* /2*/
    uint32_t i;
    uint32_t* pdwVal;
    pdwVal = (uint32_t*)(wPMABufAddr * 2 + PMAAddr);
    for (i = n; i != 0; i--)
    {
        *(uint16_t*)pbUsrBuf++ = *pdwVal++;
        pbUsrBuf++;
        EpOutDataPtrTmp = pbUsrBuf;
    }
}
