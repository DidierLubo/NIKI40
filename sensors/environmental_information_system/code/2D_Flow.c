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
 * Hahn-Schickard:	2D-Flow
 *********************************************************************
 * FileName:         2D_Flow.h
 * Dependencies:     See INCLUDES section below
 * Processor:        MSP430
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Bernd Ehrbrecht		03.09.2015
 ********************************************************************/

#include "Hardware.h"
#include "2D_Flow.h"
#include "HAL_FLASH.h"
#include "Task_Manager.h"

#define ADC_FLOW_X_VALUE         ADC12MEM0
#define ADC_FLOW_Y_VALUE         ADC12MEM1
#define ADC_FLOW_XpVALUE         ADC12MEM2
#define ADC_FLOW_YpVALUE         ADC12MEM3

//=================================================================================================================================
//    Modulinterne structs
//=================================================================================================================================
typedef struct
{
   int32_t lFlowX_avg;
   int32_t lFlowY_avg;
   int32_t lAD_X_avg;
   int32_t lAD_Y_avg;

   int16_t iFlowX_min;
   int16_t iFlowY_min;

   int16_t iFlowX_max;
   int16_t iFlowY_max;

   uint8_t  bAnzahl;
   uint8_t  bLoopCounter;
}  ST_FLOW_AVG;

//==========================================================================================================================
//    Hier die Modul-Internen Variablen
//==========================================================================================================================
ST_FLOW_MESS_DATA mstFlow_Data;
ST_FLOW_AVG       mstFlow_AVG;

// LookUpTable für Flow_X für 4096-mögliche AD-Werte sind hier 1024 gespeichert -> cm/s
static const int16_t mciFlow_X[FLOW_LOOK_UP_SIZE]  @ 0x10000  =
#include "FlowX_Values.h"


// LookUpTable für Flow_Y für 4096-mögliche AD-Werte sind hier 1024 gespeichert -> cm/s
static const int16_t mciFlow_Y[FLOW_LOOK_UP_SIZE] @ 0x10000 + sizeof(mciFlow_X) =
#include "FlowY_Values.h"


//==========================================================================================================================
//    Interner Prototyp
//==========================================================================================================================
void FillFlowLookUpIndendity(const int16_t __data20 *mciFlow);
EN_TASK_STATUS   Flow_IDEL( void ) ;

//==========================================================================================================================
//    Hier nun die externen benötigten Funktionen
//==========================================================================================================================

//==========================================================================================================================
//    Initialisierung des 2D-Flow-Sensors
//==========================================================================================================================
void Flow_Init( uint32_t ulTime_ms , uint8_t bMessAnzahl, uint16_t uiDAC_Heizer )
{
   ADC12CTL0 &=~ ADC12ENC;                            // disable conversion

   ADC12MCTL0 = ADC12SREF_7 | ADC_FLOW_X ;            // VeRef+ VeRef- , ADC_FLOW_X  -> Flow in X-Richtung
   ADC12MCTL1 = ADC12SREF_7 | ADC_FLOW_Y ;            // VeRef+ VeRef- , ADC_FLOW_X  -> Flow in Y-Richtung
   ADC12MCTL2 = ADC12SREF_7 | ADC_FLOW_Xp ;           // VeRef+ VeRef- , ADC_FLOW_Xp -> Arbeitspunkt des X-Flow-Sensors
   ADC12MCTL3 = ADC12SREF_7 | ADC_FLOW_Yp  | ADC12EOS;// VeRef+ VeRef- , ADC_FLOW_Yp -> Arbeitspunkt des Y-Flow-Sensors

   ADC12CTL0  =   ADC12SHT0_2       |                 // 16 Sample-Takte (Channel 0..7)
                  ADC12MSC          ;                 // Gleich alle Channels Samplen

   ADC12CTL1  = ADC12CSTARTADD_0  |                   // Alle Kanäle messen
                ADC12SHS_0        |                   // Sample and Hold Source = ADC12SC
                ADC12SHP          |                   // Sample and Hold Source über Timer
                ADC12DIV_3        |                   // ADCCLK12 / 4
                ADC12SSEL_0       |                   // ADC12OSC als Clock wählen
                ADC12CONSEQ_1     ;                   // single sequence of channels

   DAC12_1CTL0 = DAC12LSEL_0 | DAC12IR |              // OPS = 0, REF=0, 12Bit, (SEL=0) Direct Load, CALON=0,
                 DAC12AMP_1;                          // DAC12AMP_1 = DAC-Out = 0V, DAC12DF=0 Straight binary,
   DAC12_1CTL1 = 0;

   gstFlow_Func_Table[0].ulTime = ulTime_ms;
   mstFlow_AVG.bAnzahl          = bMessAnzahl;
   DAC12_1DAT                   = uiDAC_Heizer;

   if (mciFlow_X[0] == 0xFFFF && mciFlow_X[FLOW_LOOK_UP_SIZE-1] == 0xFFFF)
      FillFlowLookUpIndendity(mciFlow_X);

   if (mciFlow_Y[0] == 0xFFFF && mciFlow_Y[FLOW_LOOK_UP_SIZE-1] == 0xFFFF)
      FillFlowLookUpIndendity(mciFlow_Y);

   Flow_IDEL();                                       // mstFlow_AVG-Struktur laden
}

//==========================================================================================================================
//    Messintervall setzen
//==========================================================================================================================
void Flow_SetMessInterval( uint32_t ulTime_ms )
{
   gstFlow_Func_Table[0].ulTime = ulTime_ms;
   Task_InitTaskExecutionTime( gstFlow_Func_Table, 0 );    // Step 0: IDEL-State
}

//==========================================================================================================================
//    Anzahl der Messungen über die gemittelt werden soll setzen
//==========================================================================================================================
void Flow_SetMessAnzahl( uint8_t bAnzahl )
{
   mstFlow_AVG.bAnzahl = bAnzahl;
}

//==========================================================================================================================
//    DAC-Wert für Heizer-Ausgang setzen
//==========================================================================================================================
void Flow_Set_Heizer_DAC( uint16_t uiDAC )
{
   DAC12_1DAT = uiDAC;
}

//==========================================================================================================================
//    Funktionen um Look-Up-Tabellen zu löschen
//==========================================================================================================================
void Flow_ClearLookUpTables ( void )
{
   for (uint16_t wIndex=0; wIndex < FLOW_LOOK_UP_SIZE; wIndex+=256)    // Segmentsize = 512 Byte = 256 Array-Einträge
   {
      Flash_SegmentErase( (void __data20 *) &mciFlow_X[wIndex] );
      Flash_SegmentErase( (void __data20 *) &mciFlow_Y[wIndex] );
   }
}

void Flow_SetIndendityFlowTable ( void )
{
   FillFlowLookUpIndendity( mciFlow_X );
   FillFlowLookUpIndendity( mciFlow_Y );
}

//==========================================================================================================================
//    Funktionen um Look-Up-Tabellen zu laden
//==========================================================================================================================
void Flow_LoadLookUpTable ( uint16_t iIndex, int16_t *bDaten, uint16_t wLen, uint8_t isX_Value )
{
   if  (isX_Value)
      Flash_Write( bDaten, (void __data20 *) &mciFlow_X[ iIndex ], wLen<<1);
   else
      Flash_Write( bDaten, (void __data20 *) &mciFlow_Y[ iIndex ], wLen<<1);
}

void FillFlowLookUpIndendity(const int16_t __data20 *mciFlow)
{
   int16_t iIndex = 0;
   int16_t iData[64];
   do
   {
      int16_t iStartIndex = iIndex;
      for(int i=0; i<sizeof(iData)/sizeof(int16_t); i++)
      {
         iData[i] = iIndex++;
      }
      Flash_Write( iData, (void __data20 *)&mciFlow[ iStartIndex ], sizeof(iData));
   } while (iIndex < FLOW_LOOK_UP_SIZE) ;
}

//==========================================================================================================================
//    Gibt einen Zeiger auf die letzten Messdaten zurück
//==========================================================================================================================
ST_FLOW_MESS_DATA * Flow_GetMessData(void)
{
   return &mstFlow_Data;
}

//==========================================================================================================================
//    Hier nun die Taskfunktionen
//==========================================================================================================================

//==========================================================================================================================
// Step 0: (TASK_IDEL)  Warte-State bis Messung durchgeführt werden soll
//==========================================================================================================================
EN_TASK_STATUS   Flow_IDEL( void )
{
   mstFlow_AVG.bLoopCounter = mstFlow_AVG.bAnzahl;

   mstFlow_AVG.lFlowX_avg = 0;
   mstFlow_AVG.lFlowY_avg = 0;
   mstFlow_AVG.lAD_X_avg  = 0;
   mstFlow_AVG.lAD_Y_avg  = 0;

   mstFlow_AVG.iFlowX_min = 0x7FFF;
   mstFlow_AVG.iFlowY_min = 0x7FFF;

   mstFlow_AVG.iFlowX_max = 0x8000;
   mstFlow_AVG.iFlowY_max = 0x8000;

   return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP1);
}

//==========================================================================================================================
// Step 1: hier die Messung einleiten -> ADREF einschalten
//==========================================================================================================================
EN_TASK_STATUS Flow_ADREF_On( void )
{
   REFCTL0 =  REFMSTR | REFVSEL_2 | REFOUT | REFON;   // Ref-Master, 2,5V, Ref-Buffer-on, Ref-on

   return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP2);
}

//==========================================================================================================================
// Step 2: hier die Messung einleiten -> Sensor-Heizer einschalten
//==========================================================================================================================
EN_TASK_STATUS Flow_Heizer_On( void )
{
   DAC12_1CTL0 = DAC12LSEL_0 | DAC12IR |              // OPS = 0, REF=0, 12Bit, (SEL=0) Direct Load, CALON=0,
                 DAC12AMP_2;                          // DAC12AMP_1 = DAC-Out an, DAC12DF=0 Straight binary,

   FLOW_HEIZER_ON;                                    // OP des Heizers an

   ADC12CTL0 |= ADC12ON;                              // ADC on
   return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP3);
}

//==========================================================================================================================
// Step 3: hier die Messung einleiten (Sensoren aktivieren)
//==========================================================================================================================
EN_TASK_STATUS   Flow_MESSUNG_START( void )
{
   ADC12CTL0 &= ~ADC12ENC;                            // disable conversion
   ADC12CTL0 |= (ADC12ENC | ADC12SC);                 // enable conversion and start sampling

   if (isTaskInStreamingMode())
      return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP6);
   else
      return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP4);
}

//==========================================================================================================================
// Step 4: hier die Sensordaten einlesen und verarbeiten (avg/min/max)
//==========================================================================================================================
EN_TASK_STATUS   Flow_MESSUNG_READ_SENSOR( void )
{
   ADC12CTL0 &= ~ADC12ENC;                            // disable conversion

   int16_t iFlowX = mciFlow_X[ ADC_FLOW_X_VALUE >> 2 ];
   int16_t iFlowY = mciFlow_Y[ ADC_FLOW_Y_VALUE >> 2 ];

   mstFlow_AVG.lFlowX_avg += iFlowX;
   mstFlow_AVG.lFlowY_avg += iFlowY;
   mstFlow_AVG.lAD_X_avg  += ADC_FLOW_XpVALUE;
   mstFlow_AVG.lAD_Y_avg  += ADC_FLOW_YpVALUE;

   if ( mstFlow_AVG.iFlowX_min > iFlowX)
      mstFlow_AVG.iFlowX_min = iFlowX;

   if ( mstFlow_AVG.iFlowY_min > iFlowY)
      mstFlow_AVG.iFlowY_min = iFlowY;

   if ( mstFlow_AVG.iFlowX_max < iFlowX)
      mstFlow_AVG.iFlowX_max = iFlowX;

   if ( mstFlow_AVG.iFlowY_max < iFlowY)
      mstFlow_AVG.iFlowY_max = iFlowY;

   if( --mstFlow_AVG.bLoopCounter)
      return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP3);
   else
   {
      REFCTL0    =  REFMSTR | REFVSEL_2;              // Ref-Master, 2,5V, Ref-Buffer-off, Ref-off
      ADC12CTL0 &= ~ADC12ON;                          // ADC off
      DAC12_1CTL0 = DAC12LSEL_0 | DAC12IR |           // OPS = 0, REF=0, 12Bit, (SEL=0) Direct Load, CALON=0,
                    DAC12AMP_1;                       // DAC12AMP_1 = DAC-Out = 0V, DAC12DF=0 Straight binary,

      FLOW_HEIZER_OFF;                                // Heizer aus

      mstFlow_Data.iFlowX_avg = (int16_t) (mstFlow_AVG.lFlowX_avg / mstFlow_AVG.bAnzahl);
      mstFlow_Data.iFlowY_avg = (int16_t) (mstFlow_AVG.lFlowY_avg / mstFlow_AVG.bAnzahl);
      mstFlow_Data.iAD_X      = (int16_t) (mstFlow_AVG.lAD_X_avg  / mstFlow_AVG.bAnzahl);
      mstFlow_Data.iAD_Y      = (int16_t) (mstFlow_AVG.lAD_Y_avg  / mstFlow_AVG.bAnzahl);

      mstFlow_Data.iFlowX_min = mstFlow_AVG.iFlowX_min;
      mstFlow_Data.iFlowY_min = mstFlow_AVG.iFlowY_min;

      mstFlow_Data.iFlowX_max = mstFlow_AVG.iFlowX_max;
      mstFlow_Data.iFlowY_max = mstFlow_AVG.iFlowY_max;

      return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP5);
   }
}

//==========================================================================================================================
// hier die Streaming-Task-Steps
//==========================================================================================================================

//==========================================================================================================================
// Step 6: Flow_Stream_Read_Sensor:
//==========================================================================================================================
EN_TASK_STATUS  Flow_Stream_Read_Sensor( void )
{
   ADC12CTL0 &= ~ADC12ENC;                            // disable conversion

   int16_t iFlowX = mciFlow_X[ ADC_FLOW_X_VALUE >> 2 ];
   int16_t iFlowY = mciFlow_Y[ ADC_FLOW_Y_VALUE >> 2 ];

   mstFlow_Data.iFlowX_max =
   mstFlow_Data.iFlowX_min =
   mstFlow_Data.iFlowX_avg = iFlowX;

   mstFlow_Data.iFlowY_max =
   mstFlow_Data.iFlowY_min =
   mstFlow_Data.iFlowY_avg = iFlowY;
   mstFlow_Data.iAD_X      = ADC_FLOW_XpVALUE;
   mstFlow_Data.iAD_Y      = ADC_FLOW_YpVALUE;

   return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP7);
}

//==========================================================================================================================
// Step 8: Flow_Send_Return:
//==========================================================================================================================
EN_TASK_STATUS  Flow_Send_Return( void )
{
   if (isTaskInStreamingMode())
   {                                                  // im streaming-mode weitere AD-Wandlung starten
      ADC12CTL0 &= ~ADC12ENC;                         // disable conversion
      ADC12CTL0 |= (ADC12ENC | ADC12SC);              // enable conversion and start sampling
      return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP6);
   }
   else
   {  // Streaming-Mode beendet
      REFCTL0    =  REFMSTR | REFVSEL_2;              // Ref-Master, 2,5V, Ref-Buffer-off, Ref-off
      ADC12CTL0 &= ~ADC12ON;                          // ADC off
      DAC12_1CTL0 = DAC12LSEL_0 | DAC12IR |           // OPS = 0, REF=0, 12Bit, (SEL=0) Direct Load, CALON=0,
                    DAC12AMP_1;                       // DAC12AMP_1 = DAC-Out = 0V, DAC12DF=0 Straight binary,

      FLOW_HEIZER_OFF;                                // Heizer aus
      return (EN_TASK_STATUS) (TASK_LPM3 | TASK_IDEL);
   }
}

//==========================================================================================================================
// Funktionstabelle für Taskmanager
//==========================================================================================================================
ST_FUNCTION_TABLE gstFlow_Func_Table[] =
{
   { Flow_IDEL                    , 1024   },         // Step 0: nichts tun                               -> weiter mit: Step 1:    Flow_ADREF_On  hier die Messperiode eintragen
   { Flow_ADREF_On                , 250    },         // Step 1: ADREF-Einschalten und warten bis stabil  -> weiter mit: Step 2:    Flow_Heizer_On
   { Flow_Heizer_On               , 10     },         // Step 2: Sensor-Heizer ein und warten bis stabil  -> weiter mit: Step 3:    Flow_MESSUNG_START
   { Flow_MESSUNG_START           , 1      },         // Step 3: Startet die eigentliche Sensormessung    -> weiter mit: Step 4:    Flow_MESSUNG_READ_SENSOR
   { Flow_MESSUNG_READ_SENSOR     , 0      },         // Step 4: hier die Sensordaten einlesen            -> weiter mit: Step 4/5:  Flow_MESSUNG_START oder Task_Send_Flow_Daten
   { Task_Send_Flow_Daten         , 0      },         // Step 5: Messdaten ausgeben                       -> weiter mit: Step 0:    Flow_IDEL

   { Flow_Stream_Read_Sensor      , 0      },         // Step 6: Messdaten ausgeben                       -> weiter mit: Step 7
   { Task_StreamSend_Flow_Daten   , 0      },         // Step 7: Messdaten ausgeben                       -> weiter mit: Step 8
   { Flow_Send_Return             , 1      }          // Step 8: Messdaten ausgeben                       -> weiter mit: Step 6/0   Flow_IDEL
                                                      //         Achtung:  define in 2D_Flow.h FLOW_STREAM_SEND_RETURN_STEP muss diesen Step angeben
};
