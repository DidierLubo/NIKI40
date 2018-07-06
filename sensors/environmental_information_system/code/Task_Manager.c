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
//************************************************************************************************************************
//*  Taskmanager                                                                                                         *
//************************************************************************************************************************
//*                                                                                                                      *
//*  Aufgaben:                                                                                                           *
//*                                                                                                                      *
//*  Alle Task's zu verwalten                                                                                            *
//*  Low-Power aktivieren wenn möglich                                                                                   *
//*  Verwendet nur den Timer A1                                                                                          *
//*                                                                                                                      *
//************************************************************************************************************************

#include "Hardware.h"
#include "Task_Manager.h"

#include "OLED.h"
//#include "BQ27421-G1.h"                               // Um Akku-Stand abzufragen und zu verteilen
//#include "BME280.h"                                   // Um BME280 Messdaten abzuholen und zu verteilen
//#include "Si1132.h"                                   // Um Si1132 Messdaten abzuholen und zu verteilen
//#include "2D_Flow.h"                                  // Um Flow   Messdaten abzuholen und zu verteilen
//#include "EE893.h"

#include "USB_PC_Com.h"                               // USB-PC-Com-Interpreter immer aufrufen wenn Taskmanger wach wird und USB aktiv ist
                                                      // Außerdem sind hier die Funktionen deklariert um Messdaten über USB zu senden
#include "RF430.h"                                    // Deklarartion der Funktionen zum Senden der Messdaten und Task-Liste

#include "Radio_Task.h"                              // Deklarartion der Funktionen zum Senden der Messdaten

//------------------------------------------------------------------------------------------------------------------------
//    Hier die Prototypen für die Modulinternen Funktionen
//------------------------------------------------------------------------------------------------------------------------
EN_TASK_STATUS Task_Send_Akku_Daten(void);
EN_TASK_STATUS Task_CheckPowerPlug(void);

//------------------------------------------------------------------------------------------------------------------------
// Hier die Datentypen, die nur innerhalb dieses Moduls benötigt werden
//------------------------------------------------------------------------------------------------------------------------
typedef struct ST_TASK
{  // Alle Zeiten sind in 1/1024 sekunden anzugeben ~1ms. Zeit-Wert 1024 entspricht demnach 1ms
   EN_TASK_STATUS enStatus;
   const ST_FUNCTION_TABLE *pstFuncTable;             // Zeiger auf die Function-Table
   uint32_t lTaskNextExecuteTime;                     // Zeit wann diese Task ausgeführt werden soll
   EN_TASK_STATUS enStreamStartStep;
} ST_TASK_T;

//------------------------------------------------------------------------------------------------------------------------
//    Hier die Modulvariablen
//------------------------------------------------------------------------------------------------------------------------
#define TASK_COUNT      (sizeof(mstTaskArray) / sizeof(ST_TASK_T))

ST_FUNCTION_TABLE myAkkuTask[] =
{
   { Task_Send_Akku_Daten  ,    1024l*60l*15l }       // alle 15 Minuten = 60*15 Sekunden mal den Akku abfragen
};

ST_FUNCTION_TABLE myPowerPlugTask[] =
{
   { Task_CheckPowerPlug,    1024l*1 },               // Jede Sekunde Power-Plug checken
   { Task_CheckPowerPlug,    10 }                     // alle 10ms auf DisConnect prüfen
};

ST_TASK_T mstTaskArray[] =
{
   { TASK_STEP1 , gstBME280_Func_Table, 0 , TASK_STEP1 },
   { TASK_STEP1 , gstSi1132_Func_Table, 0 , TASK_STEP1 },
   { TASK_STEP1 , gstFlow_Func_Table,   0 , TASK_STEP1 },
#ifdef USE_MSP430F6658
   { TASK_STEP1 , gstEE893_Func_Table,  0 , TASK_STEP1 },
#endif
   { TASK_IDEL  , gstRF430_Func_Table,  0 , TASK_IDEL  },
   { TASK_IDEL  , gstOLED_Func_Table,   0 , TASK_IDEL  },
   { TASK_IDEL  , myAkkuTask,           0 , TASK_IDEL  },
   { TASK_IDEL  , myPowerPlugTask,      0 , TASK_IDEL  },
   { TASK_STEP0 , gst_Radio_Func_Table, 0 , TASK_IDEL  },
};

volatile unsigned long mulMSP430Ticks=0;

//------------------------------------------------------------------------------------------------------------------------
//    nun die Modulinternen Funktionen
//------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------
//    Timer A1-Interrupt über TA1CCR0
//------------------------------------------------------------------------------------------------------------------------
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER_INTERRUPT(void)
{
   uint16_t uiTemp = TA1CCR0;
//   TA1CCR0 = 0xFFFF;                                  // nächster IRQ durch Task-Manger oder nach 65sec
   mulMSP430Ticks += uiTemp;
   LPM3_EXIT;                                         // Exit from LPM if necessary (this statement will have no effect if we are not currently in low power mode).
}

//------------------------------------------------------------------------------------------------------------------------
//    Sendet die Akku-Daten an alle Ausgabemodule
//------------------------------------------------------------------------------------------------------------------------
EN_TASK_STATUS Task_Send_Akku_Daten(void)
{
   BYTE bRelPower = AKKU_GET_REL_POWER();
//   USB_Send_Flow_Data( pstM );                      // deaktiviert da dort nicht benötigt. (Es gibt eigne Funktionen um Akku-Status abzufragen)
   RF430_Send_Akku_Data( bRelPower );                 // Messdaten an NFC-Interface ausgeben
   Radio_Send_Akku_Data( bRelPower );                 // Messdaten über Funk versenden
   OLED_Show_Akku_Data( bRelPower );                  // Messdaten im OLED-Display anzeigen
   return (EN_TASK_STATUS) (TASK_LPM3 | TASK_IDEL);
}

//------------------------------------------------------------------------------------------------------------------------
//    nun die Extern aufgerufenen Funktionen
//------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------
//    Sendet die BME280-Daten an alle Ausgabe-Module
//------------------------------------------------------------------------------------------------------------------------
EN_TASK_STATUS Task_Send_BME280_Daten( void )
{
   ST_BME280_MESS_DATA *pstM = BME280_GetLastMessdata();
   USB_Send_BME280_Data( pstM );                      // Messdaten über USB ausgeben
   RF430_Send_BME280_Data( pstM );                    // Messdaten an NFC-Interface ausgeben
   Radio_Send_BME280_Data( pstM );                    // Messdaten über Funk versenden
   OLED_Show_BME280_PTH( pstM );                      // Messdaten im OLED-Display anzeigen
   return (EN_TASK_STATUS) (TASK_LPM3 | TASK_IDEL);
}

//------------------------------------------------------------------------------------------------------------------------
//    Sendet die Si1132-Daten an alle Ausgabe-Module
//------------------------------------------------------------------------------------------------------------------------
EN_TASK_STATUS Task_Send_Si1132_Daten( void )
{
   ST_Si1132_MessData *pstM = Si1132_GetMessData();
   USB_Send_Si1132_Data( pstM );                      // Messdaten über USB ausgeben
   Radio_Send_Si1132_Data( pstM );                    // Messdaten über Funk versenden

   OLED_Show_Si1132_Data( pstM );                     // Messdaten im OLED-Display anzeigen
   RF430_Send_Si1132_Data( pstM );                    // Messdaten an NFC-Interface ausgeben

   return (EN_TASK_STATUS) (TASK_LPM3 | TASK_IDEL);
}

//------------------------------------------------------------------------------------------------------------------------
//    Sendet die Streaming Si1132-Daten an alle Ausgabe-Module
//------------------------------------------------------------------------------------------------------------------------
EN_TASK_STATUS Task_StreamSend_Si1132_Daten( void )
{
   ST_Si1132_MessData *pstM = Si1132_GetMessData();
   USB_Send_Si1132_Data( pstM );                      // Messdaten über USB ausgeben
   Radio_Send_Si1132_Data( pstM );                    // Messdaten über Funk versenden
   return Si1132_STREAM_SEND_RETURN_STEP;
}

//------------------------------------------------------------------------------------------------------------------------
//    Sendet die (2D-) Flow-Daten an alle Ausgabe-Module
//------------------------------------------------------------------------------------------------------------------------
EN_TASK_STATUS Task_Send_Flow_Daten(void)
{
   ST_FLOW_MESS_DATA *pstM = Flow_GetMessData();
   USB_Send_Flow_Data( pstM );                        // Messdaten über USB ausgeben
   Radio_Send_Flow_Data( pstM );                      // Messdaten über Funk versenden

   OLED_Show_Flow_Data( pstM );                       // Messdaten im OLED-Display anzeigen
   RF430_Send_Flow_Data( pstM );                      // Messdaten an NFC-Interface ausgeben

   return (EN_TASK_STATUS) (TASK_LPM3 | TASK_IDEL);
}

//------------------------------------------------------------------------------------------------------------------------
//    Sendet die Streaming Si1132-Daten an alle Ausgabe-Module
//------------------------------------------------------------------------------------------------------------------------
EN_TASK_STATUS Task_StreamSend_Flow_Daten(void)
{
   ST_FLOW_MESS_DATA *pstM = Flow_GetMessData();
   USB_Send_Flow_Data( pstM );                        // Messdaten über USB ausgeben
   Radio_Send_Flow_Data( pstM );                      // Messdaten über Funk versenden

   return FLOW_STREAM_SEND_RETURN_STEP;
}

//------------------------------------------------------------------------------------------------------------------------
//    Sendet die EE893-Daten an alle Ausgabe-Module
//------------------------------------------------------------------------------------------------------------------------
EN_TASK_STATUS Task_Send_EE893_Daten(void)
{
   ST_EE893_MESS_DATA *pstM = EE893_GetData();
   USB_Send_EE893_Data( pstM );                       // Messdaten über USB ausgeben
   Radio_Send_EE893_Data( pstM );                     // Messdaten über Funk versenden
   OLED_Show_EE893_Data( pstM );                      // Messdaten im OLED anzeigen
   RF430_Send_EE893_Data( pstM );                     // Messdaten an NFC-Interface ausgeben
   return (EN_TASK_STATUS) (TASK_LPM3 | TASK_IDEL);
}

//------------------------------------------------------------------------------------------------------------------------
//    Zeitverzögerung: sollte nur in der Init-Phase verwendet werden -> sonst Task-Step einführen
//------------------------------------------------------------------------------------------------------------------------
void HAL_Delay_ms( uint32_t ulTime_ms , uint16_t uiLPM_Bits)  // Wegen der Timersteuerung hier her verlegt
{
   if (ulTime_ms<2) ulTime_ms = 2;                    // min. 1ms warten (genaue Zeit liegt zwischen 1ms und 2ms
   uint32_t ulStartTime = mulMSP430Ticks;
   while ( (mulMSP430Ticks - ulStartTime) < ulTime_ms )
   {
      TA1CCR0 = TA1R+2;
      __bis_SR_register( uiLPM_Bits );
   }
}

//------------------------------------------------------------------------------------------------------------------------
// This function is called to configure the System Timer, i.e TA1.
// This timer is used for all system time scheduling.
//------------------------------------------------------------------------------------------------------------------------
void Init_Task_Manager(void)
{
   TA1CTL = 0;                                        // Ensure the timer is stopped.

   TA1CTL = TASSEL_1 | ID_3;                          // Run the timer off of the ACLK / 32 durch ID3 = / 8 und TAIDEX_3 / 4
   TA1EX0 = TAIDEX_3;

   TA1CTL |= TACLR;                                   // Clear everything to start with.

   TA1CCR0 = 1;                                       // Set the compare match value according to the tick rate we want.  (ACLK/32) ~ 1ms

   TA1CCTL0 = CCIE;                                   // Enable the interrupts.

   TA1CTL |= MC_1;                                    // ab nun Up mode. (Zähle bis CCR0)
}

//------------------------------------------------------------------------------------------------------------------------
//    Initialisiert die Zeit bis zur nächsten Task-Ausführung neu, wenn sich die Task im angegebenen Step befindet
//------------------------------------------------------------------------------------------------------------------------
//    pstFT:                  Pointer auf Task-Tabelle
//    bStep:                  Step der Task = Index in der Task-Tabelle
//------------------------------------------------------------------------------------------------------------------------
void Task_InitTaskExecutionTime( ST_FUNCTION_TABLE *pstFT, BYTE bStep )
{
   uint32_t ulTime;

   for (uint16_t uiTaskNo = 0 ; uiTaskNo < TASK_COUNT ; uiTaskNo++)
   {
      bStep &= TASK_FUNC_INDEX_MASK;
      if ( mstTaskArray[uiTaskNo].pstFuncTable == pstFT &&
           mstTaskArray[uiTaskNo].enStatus & TASK_FUNC_INDEX_MASK == bStep )
      {
         // Hier ist es die richtige Task im richten Step -> lTaskNextExecuteTime neu setzen
         ulTime = mstTaskArray[uiTaskNo].pstFuncTable[bStep].ulTime;
         mstTaskArray[uiTaskNo].lTaskNextExecuteTime = mulMSP430Ticks + ulTime;
      }
   }
}

//------------------------------------------------------------------------------------------------------------------------
void Task_GoStreamingMode(void)
{
   gbRadioStreamingMode = true;

   // So und nun noch bei allen IDEL-Tasks die Zeit ablaufen lassen
   for (uint16_t uiTaskNo = 0 ; uiTaskNo < TASK_COUNT ; uiTaskNo++)
   {
      if (  (mstTaskArray[uiTaskNo].enStatus & TASK_FUNC_INDEX_MASK) <= mstTaskArray[uiTaskNo].enStreamStartStep )
      {
         // Task befindet sich im IDEL-Mode oder Step 1 ->
         // Zeit nun ablaufen lassen und sicherstellen, dass es mit Step1 weitergeht
         mstTaskArray[uiTaskNo].enStatus =  mstTaskArray[uiTaskNo].enStreamStartStep;
         mstTaskArray[uiTaskNo].lTaskNextExecuteTime = mulMSP430Ticks;
      }
   }
}

////------------------------------------------------------------------------------------------------------------------------
////    Hier den Akku-Lade-Mode überprüfen -
////    wenn Lade-Stecker gesteckt dann Ladestrom nicht begrenzen sonst
////    den Ladestrom auf USB-Nivou absenken
////------------------------------------------------------------------------------------------------------------------------
//void CheckLadeMode(void)
//{
//   LADE_STECKER_PULL_UP_ON;                           // hier den Pull-Up enablen - Abfrage erfolgt später um Zeit für das Laden der Eingangs-Kapazität zu geben
//   __delay_cycles(10);
//   if (isLADE_STECKER_CONNECTED)                      // nur über Rund-Stecker voller Ladestrom
//      AKKU_POWER_LADE_MODE;
//   else
//      AKKU_USB_LADE_MODE;
//   LADE_STECKER_PULL_UP_OFF;
//}

EN_TASK_STATUS Task_CheckPowerPlug(void)
{
   EN_TASK_STATUS st=(EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP0);

   if (isPower_Plug_Connected())
   {
      st =(EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP1);  // 10ms Task um ausstecken schnell zu erkennen
      AKKU_POWER_LADE_MODE;
   }
   else
   {
      AKKU_USB_LADE_MODE;
      st = (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP0); // nur jede Sekunde prüfen ob Power-Plug gesteckt wurde
   }
   return st;
}

//------------------------------------------------------------------------------------------------------------------------
// This function is called from main und wird nie mehr verlassen
// Sie übernimmt das abarbeiten aller (Mess-) Tasks
//------------------------------------------------------------------------------------------------------------------------
__noreturn void Task_Manager(void)
{
   static uint8_t bUSBOldConnectionStatus = 0;
   while(1)
   {
      if (isXT2_running)                              // USB-Task nur ausführen wenn Quarz läuft (einstecken erkannt und ENUM innerhalb 30 Sek durchgeführt)
      {
         USB_Task();
         if (!bUSBOldConnectionStatus)
         {
            bUSBOldConnectionStatus = 1;
            OLED_WriteStatusLine( "USB-verbunden" );
         }
      }
      else
      {
         if (bUSBOldConnectionStatus)
         {
            bUSBOldConnectionStatus = 0;
            OLED_WriteStatusLine( "USB-getrennt" );
         }
      }

      uint16_t uiFunktIndex;

      EN_TASK_STATUS enLPM = TASK_LPM3;               // auf höchst möglichen LPM-Stellen (LPM4 geht nicht da dann auch der ACLK-steht der zum Aufwachen benötigt wird)
      uint32_t ulTimeDif   = 0xFFFFFFFF;              // auf höchsten Wert stellen
      uint32_t ulTime;

      for (uint16_t uiTaskNo = 0 ; uiTaskNo < TASK_COUNT ; uiTaskNo++)
      {
         if (mulMSP430Ticks >= mstTaskArray[uiTaskNo].lTaskNextExecuteTime )
         {
            uiFunktIndex = mstTaskArray[uiTaskNo].enStatus & TASK_FUNC_INDEX_MASK ;

            EN_TASK_STATUS enStatus = mstTaskArray[uiTaskNo].pstFuncTable[uiFunktIndex].pFunc();
            // in  enStatus sind die LPM-Bits und der nächste Funktion-Index kodiert
            mstTaskArray[uiTaskNo].enStatus = enStatus;
            enLPM &= enStatus;                        // durch and-Operation verschwinden LPM-Bits so dass nur der kleinste LPM-mode übrig bleibt

            ulTime = mstTaskArray[uiTaskNo].pstFuncTable[uiFunktIndex].ulTime;
            mstTaskArray[uiTaskNo].lTaskNextExecuteTime = mulMSP430Ticks + ulTime;
         }
         else
         {
            ulTime = mstTaskArray[uiTaskNo].lTaskNextExecuteTime - mulMSP430Ticks;
         }
         if (ulTimeDif > ulTime)
            ulTimeDif  = ulTime;                      // Die kürzeste Zeit zählt

      }
      // OK alle Tasks wurden ausgeführt

      if ( ulTimeDif > 60000 )
         ulTimeDif = 60000;                           // max. 60 Sekunden schlafen
      else if (ulTimeDif < 2)
         ulTimeDif = 2;                               // min 2ms Sekunden schlafen

      _DINT();                                        // Interrupts sperren um nicht zwischen setzen von TA1CTL und __bis_SR_register unterbrochen zu werden
      TA1CCR0 = ulTimeDif;                            // die gewünschte Anzahl ms schlafen
      TA1CTL = TASSEL_1 | MC_1 | ID_3 | TACLR;        // Timer wieder laufen lassen

      if (!isXT2_running && (enLPM & TASK_LPM_BIT_MASK) && ulTimeDif && isRADIO_CS_HIGH)
      {
         // ToDo: Weitere Perepherie-Module abschalten
         // !isXT2_running:               hier besteht keine USB-Verbindung -> schlafen möglich
         // (enLPM & TASK_LPM_BIT_MASK)   es sind auch LPM-Bits gesetzt     -> mindestens LPM0 möglich
         // ulTimeDif > 0                                                   -> mindestens 1ms schlafen gewünscht

         // so nun endlich schlafen legen -> sicher stellen, dass GIE enabled
         __bis_SR_register( (enLPM & TASK_LPM_BIT_MASK) | GIE );

         // ToDo: obige Perepherie-Module wieder einschalten
      }
      else
         __bis_SR_register( LPM0_bits | GIE );
   }
}

