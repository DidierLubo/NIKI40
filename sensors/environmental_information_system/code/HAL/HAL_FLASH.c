/*******************************************************************************
 *
 * HAL_FLASH.c
 * Flash Library for flash memory controller of MSP430F5xx/6xx family
 *
 *
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Created: Version 1.0 11/24/2009
 * Updated: Version 2.0 01/18/2011
 *
 ******************************************************************************/

#include "msp430.h"
#include "HAL_FLASH.h"

__monitor void Flash_SegmentErase( void __data20 *pFlash_ptr)
{
   uint16_t __data20 *pData = pFlash_ptr;

   FCTL3 = FWKEY;                                     // Clear Lock bit
   FCTL1 = FWKEY + ERASE;                             // Set Erase bit
   while (FCTL3 & BUSY);                              // test busy
   *pData = 0xFF;                                     // Dummy write to erase Flash seg
   while (FCTL3 & BUSY);                              // test busy
   FCTL1 = FWKEY;                                     // Clear WRT bit
   FCTL3 = FWKEY + LOCK;                              // Set LOCK bit
}

__ramfunc __monitor void Flash_BankErase( void __data20 *pFlash_ptr )
{
   uint16_t __data20 *pData = pFlash_ptr;

   FCTL3 = FWKEY;                                     // Clear Lock bit
   FCTL1 = FWKEY + MERAS;                             // Set Erase bit
   while (FCTL3 & BUSY);                              // test busy
   *pData = 0xFF;                                     // Dummy write to erase Flash seg
   while (FCTL3 & BUSY);                              // test busy
   FCTL1 = FWKEY;                                     // Clear WRT bit
   FCTL3 = FWKEY + LOCK;                              // Set LOCK bit
}

uint8_t Flash_EraseCheck( void __data20 *pFlash_ptr, uint16_t len)
{
   uint8_t __data20 *pData = pFlash_ptr;
   pData = pFlash_ptr;
   uint16_t i;

   for (i = 0; i < len; i++) {                        // was erasing successfull?
      if (*(pData + i) != 0xFF)
      {
         return FLASH_STATUS_ERROR;
      }
   }

   return FLASH_STATUS_OK;
}

__monitor void Flash_Write(void *Data_ptr, void __data20 *Flash_ptr, uint16_t wByteCount)
{
   uint8_t __data20 *pDst = Flash_ptr;
   uint8_t  *pSrc = Data_ptr;

   pDst = Flash_ptr;

   FCTL3 = FWKEY;                                     // Clear Lock bit
   FCTL1 = FWKEY+WRT;                                 // Enable byte/word write mode

   while (wByteCount > 0)
   {
      while (FCTL3 & BUSY);                           // test busy
      *pDst++ = *pSrc++;                              // Write to Flash
      wByteCount--;
   }

   FCTL1 = FWKEY;                                     // Clear write bit
   FCTL3 = FWKEY + LOCK;                              // Set LOCK bit
}
