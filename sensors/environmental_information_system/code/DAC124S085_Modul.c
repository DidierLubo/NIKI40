/*
        Copyright 2016-2018 NIKI 4.0 project team
        NIKI 4.0 was financed by the Baden-Württemberg Stiftung gGmbH (www.bwstiftung.de).
        Project partners are FZI Forschungszentrum Informatik am Karlsruher
        Institut für Technologie (www.fzi.de), Hahn-Schickard-Gesellschaft
        für angewandte Forschung e.V. (www.hahn-schickard.de) and
        Hochschule Offenburg (www.hs-offenburg.de).
        This file was developed by Hahn-Schickard.
        Licensed under the Apache License, Version 2.0 (the "License");
        you may not use this file except in compliance with the License.
        You may obtain a copy of the License at
        http://www.apache.org/licenses/LICENSE-2.0
        Unless required by applicable law or agreed to in writing, software
        distributed under the License is distributed on an "AS IS" BASIS,
        WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
        See the License for the specific language governing permissions and
        limitations under the License.
*/
/*
;**************************************************************************************************************************
;								                                                                                                  *
;	      DAC124S085:    Kommunikation zwischen MSP und DAC                                                                *
;								                                                                                                  *
;**************************************************************************************************************************
;								                                                                                                  *
;        Erstellt am 25.08.2014                                                                                           *
;								                                                                                                  *
;			Das Modul nutzt die unter DAC_PUT_GET_BYTE definierteSPI-Schnittstelle (0..3) um mit dem DAC zu                  *
;			kommunizieren                                                                                                    *
;								                                                                                                  *
;**************************************************************************************************************************
*/

#include "Hardware.h"
#include "SPI_UCSI_Modul.h"

void DAC_Init( void )
{
   DAC_SYNC_LOW;
}

void DAC_SetValueDirect( BYTE bChannelNo, WORD wValue )
{
   const WORD wChannelCodes[4] =
   {
      0x1000,                                         // Write to specified register and update outputs
      0x5000,
      0x9000,
      0xD000
   };

   DAC_SYNC_HIGH;
   bChannelNo &= 0x03;                                // Channel-No auf den erlaubten Bereich begrenzen
   wValue     &= 0x0FFF;                              // DAC-Value  auf den erlaubten Bereich begrenzen
   wValue     |= wChannelCodes[bChannelNo];
   DAC_SYNC_LOW;

   DAC_PUT_GET_BYTE(wValue >> 8);
   DAC_PUT_GET_BYTE(wValue & 0xFF);
}

void DAC_SetValueOnly( BYTE bChannelNo, WORD wValue )
{
   const WORD wChannelCodes[4] =
   {
      0x0000,                                         // Write to specified register but do not update outputs
      0x4000,
      0x8000,
      0xC000
   };

   DAC_SYNC_HIGH;
   bChannelNo &= 0x03;                                // Channel-No auf den erlaubten Bereich begrenzen
   wValue     &= 0x0FFF;                              // DAC-Value  auf den erlaubten Bereich begrenzen
   wValue     |= wChannelCodes[bChannelNo];
   DAC_SYNC_LOW;

   DAC_PUT_GET_BYTE(wValue >> 8);
   DAC_PUT_GET_BYTE(wValue & 0xFF);
}

void DAC_GoSleep(void)
{
   DAC_SYNC_HIGH;
   _NOP();
   _NOP();
   _NOP();
   DAC_SYNC_LOW;

   DAC_PUT_GET_BYTE(0xB0);                            // 100 kOhm to GND
   DAC_PUT_GET_BYTE(0x00);

}