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
 /*********************************************************************
 * Hahn-Schickard:	API für EE893-Sensor (CO2-Sensor)
 *********************************************************************
 * FileName:         EE893.c
 * Dependencies:     See INCLUDES section below
 * Processor:        MSP430
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Bernd Ehrbrecht		03.09.2015
 ********************************************************************/
#include "Hardware.h"

#include "Globals.h"
#include "Task_Manager.h"
#include "EE893.H"

#include "I2C_USCI_Modul.h"

#define EE893_SA_SENSOR_TYPE_L   0x08
#define EE893_SA_SENSOR_SUBGROUP 0x10

#define EE893_SA_MEASUREMETS     0x18
#define EE893_SA_SENSOR_TYPE_H   0x20

#define EE893_SA_READ_CUST_ADR   0x28
#define EE893_SA_WRITE_CUST_ADR  0x28

#define EE893_SA_STATUS          0x38

#define EE893_SA_HUMIDITY_L      0x40
#define EE893_SA_HUMIDITY_H      0x48
#define EE893_SA_TEMP_L          0x50
#define EE893_SA_TEMP_H          0x58
#define EE893_SA_CO2_ACT_L       0x60
#define EE893_SA_CO2_ACT_H       0x68
#define EE893_SA_CO2_MEAN_L      0x70
#define EE893_SA_CO2_MEAN_H      0x78

//==========================================================================================================================
//    Ab hier die Modulvariablen
//==========================================================================================================================
ST_EE893_MESS_DATA mstEE893;

#define EE893_MEAS_ROWS          5
#define EE893_MEAS_COLS          3

BYTE mbMeasRecords[EE893_MEAS_ROWS][EE893_MEAS_COLS] =
{

//   { EE893_SA_SENSOR_TYPE_L   , 0x00, 0x00 },         // index 0
//   { EE893_SA_SENSOR_TYPE_H   , 0x00, 0x00 },         // index 1
//   { EE893_SA_SENSOR_SUBGROUP , 0x00, 0x00 },         // index 2
//   { EE893_SA_MEASUREMETS     , 0x00, 0x00 },         // index 3

//   { EE893_SA_HUMIDITY_L      , 0x00, 0x00 },         // index 4
//   { EE893_SA_HUMIDITY_H      , 0x00, 0x00 },         // index 5
//   { EE893_SA_TEMP_L          , 0x00, 0x00 },         // index 6
//   { EE893_SA_TEMP_H          , 0x00, 0x00 },         // index 7
   { EE893_SA_CO2_ACT_L       , 0x00, 0x00 },         // index 0
   { EE893_SA_CO2_ACT_H       , 0x00, 0x00 },         // index 1
   { EE893_SA_CO2_MEAN_L      , 0x00, 0x00 },         // index 2
   { EE893_SA_CO2_MEAN_H      , 0x00, 0x00 },         // index 3
   { EE893_SA_STATUS          , 0x00, 0x00 }          // index 4
};

BYTE mbRowIndex = 0;
BYTE mbColIndex = 0;
WORD mwErrorCounter = 0;

//==========================================================================================================================
//    Ab hier die extern verfügbaren Funktionen
//==========================================================================================================================
void EE893_Init( uint32_t ulMessInterval_ms )
{
   // ToDo: I²C Init mit 5000 Baud!!!!! -> ACKL mit 32768Hz als Basis verwenden
   I2C_B2_Init(4096);
   gstEE893_Func_Table[0].ulTime = ulMessInterval_ms;
}

void EE893_SetMessInterval(uint32_t ulMessInterval_ms )
{
   gstEE893_Func_Table[0].ulTime = ulMessInterval_ms;
   Task_InitTaskExecutionTime( gstEE893_Func_Table, 0 );    // Step 0: IDEL-State
}

ST_EE893_MESS_DATA *EE893_GetData(void)
{
   return &mstEE893;
}

//==========================================================================================================================
//    Ab hier die Modulinternen None-Task-Funktionen
//==========================================================================================================================

//==========================================================================================================================
//    Wandelt die Recorddaten in uint16 um.
//==========================================================================================================================
uint16_t EE893_RecToUInt16( uint8_t bIndex )
{
   UN_LE_2B un;
   un.b[0] = mbMeasRecords[ bIndex   ][1];
   un.b[1] = mbMeasRecords[ bIndex+1 ][1];
   return un.w;
}

//==========================================================================================================================
//    Wandelt die Recorddaten in int16 um. (+- wegen Temperatur die auch negativ werden kann)
//==========================================================================================================================
int16_t EE893_RecToInt16( uint8_t bIndex )
{
   UN_LE_2B un;
   un.b[0] = mbMeasRecords[ bIndex   ][1];
   un.b[1] = mbMeasRecords[ bIndex+1 ][1];
   return un.i;
}

//==========================================================================================================================
//    Prüft ob die Checksumme von den Empfangsdaten stimmt - true bei ok
//==========================================================================================================================
uint8_t EE893_CheckSum_OK( uint8_t bIndex )
{
   // 0x01 kommt wegen Read
   uint8_t bChkSum = (mbMeasRecords[ bIndex ][0] << 1 | 0x01) + mbMeasRecords[ bIndex ][1] ;
   return bChkSum == mbMeasRecords[ bIndex ][2];
}

//==========================================================================================================================
//    Startet die I2C - B2 Datenübertragung
//==========================================================================================================================
void EE893_I2C_Start_Read_Data()
{
   if (UCB2IE==0)
   {
      UCB2CTL1 = I2C_B2_CLK_SOURCE | UCSWRST;         // Use SMCLK, keep SW reset
      _NOP();
      _NOP();
      _NOP();
      _NOP();
      UCB2CTL1 &= ~UCSWRST;                           // Clear SW reset, resume operation
      UCB2IE   |= UCNACKIE | UCRXIE;                  // Enable RX and NACK interrupt
   }
   while (UCB2CTL1 & UCTXSTP) _NOP();                 // Ensure stop condition got sent
   UCB2I2CSA = mbMeasRecords[ mbRowIndex ][0];        // Slave-Adresse eintragen
   mbColIndex = 1;
   UCB2CTL1 |= UCTXSTT;                               // I2C start condition
}

//==========================================================================================================================
//    I2C - B2 -Interrupt-Teil
//==========================================================================================================================
#pragma vector = USCI_B2_VECTOR
__interrupt void USCI_B2_ISR(void)
{
   switch(__even_in_range(UCB2IV,12))
   {
   case  0: break;                                    // Vector  0: No interrupts
   case  2: break;                                    // Vector  2: ALIFG
   case  4:
      UCB2CTL1 |= UCTXSTP;                            // Generate I2C stop condition
      break;                                          // Vector  4: NACKIFG
   case  6: break;                                    // Vector  6: STTIFG
   case  8: break;                                    // Vector  8: STPIFG
   case 10:                                           // Vector 10: RXIFG
      if (mbColIndex == 1)
         UCB2CTL1 |= UCTXSTP;                         // Generate I2C stop condition
      mbMeasRecords[ mbRowIndex ][mbColIndex++] = UCB2RXBUF;
      break;
  case 12: break;                                     // Vector 12: TXIFG
  default: break;
  }
}

//==========================================================================================================================
//    Ab hier die Modulinternen Task-Funktionen
//==========================================================================================================================

//==========================================================================================================================
//    TASK_STEP0 = TASK_IDEL = Wartezeit bis zur nächsten Messung
//==========================================================================================================================
EN_TASK_STATUS   EE893_IDEL( void )
{
   return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP1);
}

//==========================================================================================================================
//    TASK_STEP1 = Hier Sensor einschalten ca. 20 Sekunden warten bis Messdaten vorhanden
//==========================================================================================================================
EN_TASK_STATUS   EE893_PowerUp( void )
{
   EE893_POWER_ON;
   return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP2);
}

//==========================================================================================================================
//    TASK_STEP2 = Hier das Auslesen des Sensors anstarten es. Es werden ca. 279 Bits gesendet. Bei 4096 Baud ca. 68ms
//==========================================================================================================================
EN_TASK_STATUS   EE893_Start_Read_Data( void )
{
   mbRowIndex = 0;
   mwErrorCounter = 0;
   EE893_I2C_Start_Read_Data();
   return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP3);
}

//==========================================================================================================================
//    TASK_STEP3 = Warten bis Sensordaten ausgelesen. Sensor danach abschalten und Ports entsprechend umprogrammieren
//==========================================================================================================================
EN_TASK_STATUS   EE893_End_Read_Data(void)
{
   // Defaultmäßig bleiben wir in diesem State bis alle Sensor-Daten abgerufen wurden
   EN_TASK_STATUS enRetStatus = (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP3);

   if ( EE893_CheckSum_OK( mbRowIndex ) )
      mbRowIndex++;                                   // wenn Checksumme ok dann nächsten Record verarbeiten
   else
   {
      mwErrorCounter++;                               // sonst Fehler-Counter ++
      UCB2IE = 0;                                     // I²C reset erzwingen ( in EE893_I2C_Start_Read_Data )
   }

   if (mbRowIndex == EE893_MEAS_ROWS)
   {
      mstEE893.wHumity  = 0;                          // in 1/100 %
      mstEE893.iTemp    = 2100;                       // in 1/100 °C
      mstEE893.wCO2_act = EE893_RecToUInt16( 0 );     // in ppm
      mstEE893.wCO2_avg = EE893_RecToUInt16( 2 );     // in ppm
      mstEE893.bStatus  = mbMeasRecords[4][1];

      if (mstEE893.bStatus & 0x08)                    // bei fehler in Step 1 gehen und Messung wiederholen
      {
//         mwERROR = 0x11C1;                            // Error-Code: Hier erstmal als Adresse (C1) und Checksum missbraucht
//         _NOP();
//         I2C_B2_WriteRegister(EE893_SA_WRITE_CUST_ADR, 0x00, &mwERROR, 2 );
//         I2C_B2_ReadBytes    (EE893_SA_READ_CUST_ADR, &mwERROR, 2 );
         enRetStatus = (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP2);
      }
      else                                            // Sonst die neuen Messdaten ausgeben
      {
         EE893_POWER_OFF;                             // CO2-Sensor ausschalten da alles Empfangen wurde. also jetzt in die Daten auswerten
                                                      // die Rohdaten stehen in mbMeasRecords
                                                      // die Aufbereiteten Daten stehen in mstEE893
         UCB2IE = 0;                                  // interrupt wieder abschalten
         enRetStatus = (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP4);
      }
   }
   else
   {
      EE893_I2C_Start_Read_Data();
      if (mwErrorCounter > 100)                       // nach 100 Versuchen geben wir auf - CO2 ausschalten
      {
         EE893_POWER_OFF;                             // CO2-Sensor ausschalten da alles Empfangen wurde. also jetzt in die Daten auswerten
                                                      // die Rohdaten stehen in mbMeasRecords
                                                      // die Aufbereiteten Daten stehen in mstEE893
         UCB2IE = 0;                                  // interrupt wieder abschalten
         enRetStatus = (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP0);
      }
   }
   return enRetStatus;
}

//==========================================================================================================================
//    Hier die globale Task-Tabelle für den Task-Manager
//==========================================================================================================================
ST_FUNCTION_TABLE gstEE893_Func_Table[] =
{
   { EE893_IDEL                   , 1024l*60l*30l  }, // Messung alle 30 Minuten (wird in der Init-Funktion überschrieben
   { EE893_PowerUp                , 1024l*20       }, // Spannungsversorgung für Sensor einschalten -> danach 20 Sekunden warten
   { EE893_Start_Read_Data        , 20             }, // Startet das Auslesen des Sensors -> (Zeit für 9*(3*(8+1)+4) = 279-Bits = 68ms -> 100ms warten
   { EE893_End_Read_Data          , 10             }, // Sensor-Heizer ein und warten bis stabil  -> weiter mit: Flow_MESSUNG_START

   { Task_Send_EE893_Daten        , 0              }  // Messdaten ausgeben  -> weiter mit: EE893_IDEL
};