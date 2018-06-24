/*
        Copyright 2016-2018 NIKI 4.0 project team
        NIKI 4.0 was financed by the Baden-Württemberg Stiftung gGmbH (www.bwstiftung.de).
        Project partners are FZI Forschungszentrum Informatik am Karlsruher
        Institut für Technologie (www.fzi.de), Hahn-Schickard-Gesellschaft
        für angewandte Forschung e.V. (www.hahn-schickard.de) and
        Hochschule Offenburg (www.hs-offenburg.de).
        This file was developed by FZI Forschungszentrum Informatik.
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
 /*********************************************************************
 * HSG-IMIT:	BQ27421-Akku-Überwachung
 * The header file joins all header files used in the project.
 *********************************************************************
 * FileName:        BQ27421.h
 * Dependencies:    See INCLUDES section below
 * Processor:       MSP430
 * Compiler:        C30/C32
 * Company:         HSG-IMIT
 *
 * Software License Agreement
 *
 * Copyright © 2008 Microchip Technology Inc.  All rights reserved.
 * Microchip licenses to you the right to use, modify, copy and distribute
 * Software only when embedded on a Microchip microcontroller or digital
 * signal controller, which is integrated into your product or third party
 * product (pursuant to the sublicense terms in the accompanying license
 * agreement).
 *
 * You should refer to the license agreement accompanying this Software
 * for additional information regarding your rights and obligations.
 *
 * SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY
 * OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR
 * PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR
 * OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,
 * BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT
 * DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL,
 * INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA,
 * COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY
 * CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF),
 * OR OTHER SIMILAR COSTS.
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Bernd Ehrbrecht		16.10.2013
 ********************************************************************/

#include "Hardware.h"
#include "I2C_USCI_Modul.h"
#include "BQ27421-G1.h"

#define BQ272421_I2C_ADR       0x55
#define Delay_us(x)           __delay_cycles((MCLK_FREQ * x) / 1000000)
#define DelayBetweenCMD       Delay_us(70)

void BQ27321_SetBatParameter(void)
{
   BYTE bBuf[2];
   bBuf[0] = 0x00;
   bBuf[1] = 0x80;
   BQ27421_WRITE_REG(BQ272421_I2C_ADR, 0x00, bBuf, 2);   // UNSEAL
   DelayBetweenCMD;
   BQ27421_WRITE_REG(BQ272421_I2C_ADR, 0x00, bBuf, 2);
   DelayBetweenCMD;
   bBuf[0] = 0x13;
   bBuf[1] = 0x00;
   BQ27421_WRITE_REG(BQ272421_I2C_ADR, 0x00, bBuf, 2);   // Send SET_CFGUPDATE subcommand, Control(0x0013).
   DelayBetweenCMD;

   do
   {
      BQ27421_READ_REG(BQ272421_I2C_ADR, 0x06, bBuf, 1);
      DelayBetweenCMD;
   } while (!(bBuf[0] & BIT4));                                   // Confirm CFGUPDATE mode by polling Flags() register until bit 4 is set. May take up to 1 second.    | rd 0x06 Flags_register;

   bBuf[0] = 0x00;
   BQ27421_WRITE_REG(BQ272421_I2C_ADR, 0x61, bBuf, 1);            // Write 0x00 using BlockDataControl() command (0x61) to enable block data memory control.            | wr 0x61 0x00;
   DelayBetweenCMD;

   bBuf[0] = 0x52;
   BQ27421_WRITE_REG(BQ272421_I2C_ADR, 0x3E, bBuf, 1);            // Write 0x52 using the DataBlockClass() command (0x3E) to access the State                           | wr 0x3E 0x52;
   DelayBetweenCMD;                                               // subclass (82 decimal, 0x52 hex) containing the Design Capacity parameter.

   bBuf[0] = 0x00;
   BQ27421_WRITE_REG(BQ272421_I2C_ADR, 0x3F, bBuf, 1);            // Write the block offset location using DataBlock() command (0x3F).                                  | wr 0x3F 0x00;
   DelayBetweenCMD;                                               // Note: To access data located at offset 0 to 31, use offset = 0x00. To access
                                                                  // data located at offset 32 to 41, use offset = 0x01.
   BYTE bOldCsum;
   BQ27421_READ_REG(BQ272421_I2C_ADR, 0x60, &bOldCsum, 1);        // Read the 1-byte checksum using the BlockDataChecksum() command (0x60).                             | rd 0x60 OLD_Csum;
   DelayBetweenCMD;                                               // Expect 0xE8 for -G1B checksum.

   BYTE bOldCap_MSB;
   BQ27421_READ_REG(BQ272421_I2C_ADR, 0x4A, &bOldCap_MSB, 1);     // Read both Design Capacity bytes starting at 0x4A (offset = 10). Block data                         | rd 0x4A OLD_DesCap_MSB;
   DelayBetweenCMD;                                               // starts at 0x40, so to read the data of a specific offset, use address 0x40 +                       | rd 0x4B OLD_DesCap_LSB;
                                                                  // mod(offset, 32). Expect 0x03 0xE8 for -G1B for a 1000-mAh default value.
                                                                  // Note: LSB byte is coincidentally the same value as the checksum.
   BYTE bOldCapLSB;
   BQ27421_READ_REG(BQ272421_I2C_ADR, 0x4B, &bOldCapLSB, 1);
   DelayBetweenCMD;

   bBuf[0] = BQ27421_AkkuCapacity >> 8;                           // Write both Design Capacity bytes starting at 0x4A (offset = 10). For this                          | wr 0x4A 0x04;
   bBuf[1] = BQ27421_AkkuCapacity & 0xFF;                         // example, the new value is 1200 mAh. (0x04B0 hex)                                                   | wr 0x4B 0xB0;
   BQ27421_WRITE_REG(BQ272421_I2C_ADR, 0x4A, bBuf, 2);
   DelayBetweenCMD;
//
//   BQ27421_WRITE_REG(BQ272421_I2C_ADR, 0x4B, &bBuf[1], 1);
//   DelayBetweenCMD;

   // Compute the new block checksum. The checksum is (255 – x) where x is the                     |   temp = mod(255 - OLD_DesCap_MSB - OLD_DesCap_LSB - OLD_Csum , 256)
   // 8-bit summation of the BlockData() (0x40 to 0x5F) on a byte-by-byte basis. A                 |
   // quick way to calculate the new checksum uses a data replacement method                       |
   // with the old and new data summation bytes. Refer to the code for the indicated               |   NEW_Csum = 255 - mod(temp + 0x04 + 0xB0, 256);
   // method.

   BYTE bTemp = 255 - bOldCap_MSB - bOldCapLSB - bOldCsum;        // Alte Werte von der Checksumme abziehen und die neuen Addieren
   bTemp = bTemp +    bBuf[0]     + bBuf[1];
   bTemp = 255 - bTemp;

   BQ27421_WRITE_REG(BQ272421_I2C_ADR, 0x60, &bTemp, 1);          // Write new checksum. The data is actually transferred to the Data Memory                            | wr 0x60 New_Csum;
   DelayBetweenCMD;                                               // when the correct checksum for the whole block (0x40 to 0x5F) is written to                         | Example: wr 0x60 0x1F
                                                                  // BlockDataChecksum() (0x60). For this example New_Csum is 0x1F.

   bBuf[0] = 0x42;
   bBuf[1] = 0x00;
   BQ27421_WRITE_REG(BQ272421_I2C_ADR, 0x00, &bBuf, 2);           // Exit CFGUPDATE mode by sending SOFT_RESET subcommand, Control(0x0042)                              | wr 0x00 0x42 0x00;
   DelayBetweenCMD;

   do
   {
      BQ27421_READ_REG(BQ272421_I2C_ADR, 0x06, bBuf, 1);
      DelayBetweenCMD;
   } while (bBuf[0] & BIT4);                                      // Confirm CFGUPDATE has been exited by polling Flags() register until bit 4 is                       | rd 0x06 Flags_register;
                                                                  // cleared. May take up to 1 second.

   bBuf[0] = 0x20;
   bBuf[1] = 0x00;
   BQ27421_WRITE_REG(BQ272421_I2C_ADR, 0x00, &bBuf, 2);           // return to SEALED mode by sending the Control(0x0020) subcommand.                                   | wr 0x00 0x20 0x00
   DelayBetweenCMD;

}

//-----------------------------------------------------------------------------------------
//	Hier die exportierten Funktionen
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
//	BQ27421_Init
//-----------------------------------------------------------------------------------------
//	Initialisiert den I2C-Bus und die beiden Sensorteile (Mag/Acc)
//-----------------------------------------------------------------------------------------
void BQ27421_Init(void)
{
	BYTE b = BQ27421_GetRelPower();
}

//-----------------------------------------------------------------------------------------
//	BQ27421_GetRelPower
//-----------------------------------------------------------------------------------------
//	Diese Funktion liest den Ladezustand in % aus
//-----------------------------------------------------------------------------------------
BYTE BQ27421_GetRelPower( void)
{
   BYTE bRetValue;
   BQ27421_READ_REG(BQ272421_I2C_ADR, 0x1C, &bRetValue, 1);
	return bRetValue;
}

//-----------------------------------------------------------------------------------------
//	BQ27421_GetAkkuSpannung
//-----------------------------------------------------------------------------------------
//	Diese Funktion liest die Akku-Spannung aus und gibt diese in mV zurück
//-----------------------------------------------------------------------------------------
int BQ27421_GetAkkuSpannung(void)
{
   WORD wRetValue;
   BQ27421_READ_REG(BQ272421_I2C_ADR, 0x04, &wRetValue, 2);
	return __swap_bytes(wRetValue);
}

//-----------------------------------------------------------------------------------------
//	BQ27421_GetAverageCurrent
//-----------------------------------------------------------------------------------------
//	Diese Funktion liest den aktuellen (über 1 Sekunde) gemittelten Strom in mA
//-----------------------------------------------------------------------------------------
int BQ27421_GetAverageCurrent(void)
{
   int iRetValue = 0;
   BQ27421_READ_REG(BQ272421_I2C_ADR, 0x10, &iRetValue, 2);
   return __swap_bytes(iRetValue);
}

