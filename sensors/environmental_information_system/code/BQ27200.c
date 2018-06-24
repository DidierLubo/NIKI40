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
 * HSG-IMIT:	BQ27200-Akku-Überwachung
 * The header file joins all header files used in the project.
 *********************************************************************
 * FileName:        BQ27200.h
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
#include "Globals.h"

#include "I2C_USCI_Modul.h"
#include "BQ27200.h"

#define BQ27200_I2C_ADR       0x55

#define bq27x10CMD_CTRL      0x00
#define bq27x10CMD_MODE      0x01
#define bq27x10CMD_AR_LSB    0x02
#define bq27x10CMD_AR_MSB    0x03
#define bq27x10CMD_ARTTE_LSB 0x04
#define bq27x10CMD_ARTTE_MSB 0x05
#define bq27x10CMD_TEMP_LSB  0x06
#define bq27x10CMD_TEMP_MSB  0x07
#define bq27x10CMD_VOLT_LSB  0x08
#define bq27x10CMD_VOLT_MSB  0x09
#define bq27x10CMD_FLAGS     0x0A
#define bq27x10CMD_RSOC      0x0B
#define bq27x10CMD_NAC_LSB   0x0C
#define bq27x10CMD_NAC_MSB   0x0D
#define bq27x10CMD_LMD_LSB   0x0E
#define bq27x10CMD_LMD_MSB   0x0F
#define bq27x10CMD_CAC_LSB   0x10
#define bq27x10CMD_CAC_MSB   0x11
#define bq27x10CMD_FCAC_LSB  0x12
#define bq27x10CMD_FCAC_MSB  0x13
#define bq27x10CMD_AI_LSB    0x14
#define bq27x10CMD_AI_MSB    0x15
#define bq27x10CMD_TTE_LSB   0x16
#define bq27x10CMD_TTE_MSB   0x17
#define bq27x10CMD_TTF_LSB   0x18
#define bq27x10CMD_TTF_MSB   0x19
#define bq27x10CMD_SI_LSB    0x1A
#define bq27x10CMD_SI_MSB    0x1B
#define bq27x10CMD_STTE_LSB  0x1C
#define bq27x10CMD_STTE_MSB  0x1D
#define bq27x10CMD_CEDV_LSB  0x20
#define bq27x10CMD_CEDV_MSB  0x21
#define bq27x10CMD_TTECP_LSB 0x26
#define bq27x10CMD_TTECP_MSB 0x27
#define bq27x10CMD_CYCL_LSB  0x28
#define bq27x10CMD_CYCL_MSB  0x29
#define bq27x10CMD_CYCT_LSB  0x2A
#define bq27x10CMD_CYCT_MSB  0x2B
#define bq27x10CMD_CSOC      0x2C
#define bq27x10CMD_EE_EN     0x6E
#define bq27x10CMD_ILMD      0x76
#define bq27x10CMD_SEDVF     0x77
#define bq27x10CMD_SEDV1     0x78
#define bq27x10CMD_ISLC_EDVT 0x79
#define bq27x10CMD_DMFSD     0x7A
#define bq27x10CMD_TAPER     0x7B
#define bq27x10CMD_PKCFG     0x7C
#define bq27x10CMD_GAF_DEDV  0x7D
#define bq27x10CMD_DCOMP     0x7E
#define bq27x10CMD_TCOMP     0x7F

// When CTRL COMMAND KEY = 0xA9 ...
#define bq27x10REG_MODE_GPIEN  BIT7
#define bq27x10REG_MODE_GPSTAT BIT6
#define bq27x10REG_MODE_WRTNAC BIT5
#define bq27x10REG_MODE_DONE   BIT4
#define bq27x10REG_MODE_PRST   BIT3
#define bq27x10REG_MODE_INIT   BIT2
#define bq27x10REG_MODE_FRST   BIT1
#define bq27x10REG_MODE_SHIP   BIT0

// When CTRL COMMAND KEY = 0x56 ...
#define bq27x10REG_MODE_GPIEN  BIT7
#define bq27x10REG_MODE_GPSTAT BIT6
#define bq27x10REG_MODE_CEO    BIT5
#define bq27x10REG_MODE_CIO    BIT4
#define bq27x10REG_MODE_WNACCI BIT3
#define bq27x10REG_MODE_INIT   BIT2
#define bq27x10REG_MODE_WRTCYC BIT1
#define bq27x10REG_MODE_WRTLMD BIT0

// When CTRL COMMAND KEY = 0xC5 ...
#define bq27x10REG_MODE_GPIEN  BIT7
#define bq27x10REG_MODE_GPSTAT BIT6
#define bq27x10REG_MODE_UPDC   BIT5
#define bq27x10REG_MODE_UPEDVI BIT4
#define bq27x10REG_MODE_UPDMF  BIT3
#define bq27x10REG_MODE_INIT   BIT2
#define bq27x10REG_MODE_UPCFG  BIT1
#define bq27x10REG_MODE_UPCOMP BIT0

// Status Flag Register (FLAGS)
#define bq27x10REG_FLAGS_CHGS  BIT7
#define bq27x10REG_FLAGS_NOACT BIT6
#define bq27x10REG_FLAGS_IMIN  BIT5
#define bq27x10REG_FLAGS_CI    BIT4
#define bq27x10REG_FLAGS_CALIP BIT3
#define bq27x10REG_FLAGS_VDQ   BIT2
#define bq27x10REG_FLAGS_EDV1  BIT1
#define bq27x10REG_FLAGS_EDVF  BIT0

//-----------------------------------------------------------------------------------------
//	Hier die exportierten Funktionen
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
//	BQ27200_Init
//-----------------------------------------------------------------------------------------
//	Initialisiert den I2C-Bus und die beiden Sensorteile (Mag/Acc)
//-----------------------------------------------------------------------------------------
void BQ27200_Init(void)
{
	BYTE bRetValue;
   if (BQ27200_READ_REG(BQ27200_I2C_ADR, bq27x10CMD_FLAGS, &bRetValue, 1))
   {
      bRetValue = bq27x10REG_MODE_FRST;
      BQ27200_WRITE_REG( BQ27200_I2C_ADR, bq27x10CMD_MODE, &bRetValue, 1);
      bRetValue = 0xA9;
      BQ27200_WRITE_REG( BQ27200_I2C_ADR, bq27x10CMD_CTRL, &bRetValue, 1);
      BQ27200_READ_REG(BQ27200_I2C_ADR, bq27x10CMD_FLAGS, &bRetValue, 1);
   }

   if ( bRetValue & bq27x10REG_FLAGS_CI)
   {                                                  // Hier kommen wir hin, wenn der BQ27210 noch keine Kapazität ermittelt hat
      UN_LE_2B un;
      // 3.57µVh / 20mOhm * Digits = Kapazität in mAh -> Da wir die Kapazität in mAh und den Messwiderstand (20mOhm) kennen
      // berechnen wir die Digits im BQ27210
      un.w = (uint16_t) (BQ27200_AkkuCapacity * BQ27200_RS_in_mOhm / 3.57f);
      BQ27200_WRITE_REG( BQ27200_I2C_ADR, bq27x10CMD_AR_LSB, &un.b[0], 1);
      BQ27200_WRITE_REG( BQ27200_I2C_ADR, bq27x10CMD_AR_MSB, &un.b[1], 1);

      // Werte in NAC (Nominal Available Capacity) und LMD (Last Measured Discharge) übernhemen
      bRetValue = bq27x10REG_MODE_WRTNAC | bq27x10REG_MODE_DONE;
      BQ27200_WRITE_REG( BQ27200_I2C_ADR, bq27x10CMD_MODE, &bRetValue, 1);
      bRetValue = 0xA9;
      BQ27200_WRITE_REG( BQ27200_I2C_ADR, bq27x10CMD_CTRL, &bRetValue, 1);
   }
}

//-----------------------------------------------------------------------------------------
//	BQ27200_GetRelPower
//-----------------------------------------------------------------------------------------
//	Diese Funktion liest den Ladezustand in % aus
//-----------------------------------------------------------------------------------------
BYTE BQ27200_GetRelPower( void)
{
   BYTE bRetValue = 255;

   for (BYTE i=5; i && bRetValue>100; i--)            // Maximal 5 Versuche um Rel.Power zu lesen
   {
      BQ27200_READ_REG(BQ27200_I2C_ADR, bq27x10CMD_CSOC, &bRetValue, 1);
   }

   if (bRetValue > 100)
      bRetValue = 100;
	return bRetValue;
}

//-----------------------------------------------------------------------------------------
//	BQ27200_GetAkkuSpannung
//-----------------------------------------------------------------------------------------
//	Diese Funktion liest die Akku-Spannung aus und gibt diese in mV zurück
//-----------------------------------------------------------------------------------------
int BQ27200_GetAkkuSpannung(void)
{
   WORD wRetValue;
   BQ27200_READ_REG(BQ27200_I2C_ADR, bq27x10CMD_VOLT_LSB, &wRetValue, 2);
	return wRetValue;
}

//-----------------------------------------------------------------------------------------
//	BQ27200_GetAverageCurrent
//-----------------------------------------------------------------------------------------
//	Diese Funktion liest den aktuellen (über 5.12 Sekunden) gemittelten Strom in 0.01 mA
// (max)                                                                      320.00 mA
//-----------------------------------------------------------------------------------------
int BQ27200_GetAverageCurrent(void)
{
   long lRetValue = 0;
   BQ27200_READ_REG(BQ27200_I2C_ADR, bq27x10CMD_AI_LSB, &lRetValue, 2);
	// 3.57µV Auflösung von AD-Wandler µV / mOhm = mA
   // da wir 357 als Konstante verwenden bekommen wir den Strom in 10µA => max 327mA (wegen int)
   lRetValue = (lRetValue * 357) / (int16_t) BQ27200_RS_in_mOhm;

   BYTE bFlags;
   BQ27200_READ_REG(BQ27200_I2C_ADR, bq27x10CMD_FLAGS, &bFlags, 1);
   if (!(bFlags & bq27x10REG_FLAGS_CHGS))
      lRetValue = -lRetValue;

   return lRetValue;
}

