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
 * Hahn-Schickard:	Si1132 Licht+UV-Index
 *********************************************************************
 * FileName:         Si1132.h
 * Dependencies:     See INCLUDES section below
 * Processor:        MSP430
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Bernd Ehrbrecht		14.08.2015
 ********************************************************************/

#include "Hardware.h"
#include "Si1132.h"
#include "Task_Manager.h"
#include "I2C_USCI_Modul.h"
#include "Globals.h"

//=================================================================================================================================
//    Registeradressen des Si1132
//=================================================================================================================================
#define PART_ID_ADR              0x00
#define REV_ID_ADR               0x01
#define SEQ_ID_ADR               0x02
#define INT_CFG_ADR              0x03
#define IRQ_ENABLE_ADR           0x04
#define HW_KEY_ADR               0x07
#define MEAS_RATE0_ADR           0x08
#define MEAS_RATE1_ADR           0x09

#define UCOEF0_ADR               0x13
#define UCOEF1_ADR               0x14
#define UCOEF2_ADR               0x15
#define UCOEF3_ADR               0x16

#define PARAM_WR_ADR             0x17
#define COMMAND_ADR              0x18
#define RESPONSE_ADR             0x20

#define IRQ_STATUS_ADR           0x21

#define ALS_VIS_DATA0_ADR        0x22
#define ALS_VIS_DATA1_ADR        0x23
#define ALS_IR_DATA0_ADR         0x24
#define ALS_IR_DATA1_ADR         0x25

#define AUX_DATA0_ADR            0x2C
#define AUX_DATA1_ADR            0x2D
#define UVINDEX0_ADR             0x2C
#define UVINDEX1_ADR             0x2D

#define PARAM_RD_ADR             0x2E
#define CHIP_STAT_ADR            0x30
#define ANA_IN_KEY_ADR           0x3B

//=================================================================================================================================
//    Registerinhalte / Bits
//=================================================================================================================================

#define PART_ID_VALUE            0x32
#define REV_ID_VALUE             0x00
#define SEQ_ID_VALUE             0x08

#define INT_CFG_INT_OE_BIT       BIT0
#define IRQ_ENABLE_ALS_IE_BIT    BIT0

#define HW_KEY_VALUE             0x17

#define IRQ_STATUS_CMD_INT_BIT   BIT5
#define IRQ_STATUS_CMD_ALS_BIT   BIT0

#define CHIP_STAT_RUNNING_BIT    BIT2
#define CHIP_STAT_SUSPENDED_BIT  BIT1
#define CHIP_STAT_SLEEP_BIT      BIT0

//=================================================================================================================================
//    CMD's
//=================================================================================================================================

#define CMD_PARAM_QUERY          0x80
#define CMD_PARAM_SET            0xA0
#define CMD_NOP                  0x00
#define CMD_RESET                0x01
#define CMD_GET_CAL              0x12
#define CMD_ALS_FORCE            0x06
#define CMD_ALS_PAUSE            0x0A
#define CMD_ALS_AUTO             0x0E

//=================================================================================================================================
//    Parameter RAM Adressen
//=================================================================================================================================

#define PARAM_CHLIST_ADR               0x01
#define PARAM_ALS_ENCODING_ADR         0x06


#define PARAM_CHLIST_EN_UV_BIT         BIT7
#define PARAM_CHLIST_EN_AUX_BIT        BIT6
#define PARAM_CHLIST_EN_IR_BIT         BIT5
#define PARAM_CHLIST_EN_VIS_BIT        BIT4

#define PARAM_CHLIST_DEFAULT_VALUE     0xB0           // UV / IR / VIS messen


//=================================================================================================================================
//    Modulinterne structs
//=================================================================================================================================
typedef struct
{
   uint32_t ulVIS;
   uint32_t ulIR;
   uint32_t ulUV_Index;

   uint16_t wMaxVIS;
   uint16_t wMaxIR;
   uint16_t wMaxUV_Index;

   uint16_t wMinVIS;
   uint16_t wMinIR;
   uint16_t wMinUV_Index;

   uint8_t  bAnzahl;
   uint8_t  bLoopCounter;
}  ST_Si1132_AVG;

typedef struct
{
   uint16_t wVIS;
   uint16_t wIR;
   uint16_t wUV_Index;
} ST_Si1132_SensorData;

//=================================================================================================================================
//    Modulvariablen
//=================================================================================================================================
ST_Si1132_MessData   mstSi1132_MessData;
ST_Si1132_AVG        mstSi1132_AVG;

//=================================================================================================================================
//    Prototyping
//=================================================================================================================================
EN_TASK_STATUS   Si1132_IDEL( void );

//==========================================================================================================================
// Führt ein Command aus und liefert das Ergebnis zurück
//==========================================================================================================================
// Parameter: CMD welches ausgeführt werden soll - siehe defines oben (CMD_...)
//==========================================================================================================================
// Liefert Si1132_RESPONSE_OK wenn alles OK sonst den entsprechenden Fehlercode
//==========================================================================================================================
BYTE Si1132_ExecuteCMD(BYTE bCMD)
{
   BYTE bBuf[1] = { CMD_NOP };
   Si1132_WRITE_REG(Si1132_I2C_ADR, COMMAND_ADR,  bBuf ,1 );
   Si1132_READ_REG(Si1132_I2C_ADR,  RESPONSE_ADR, bBuf ,1 );
   if (bBuf[0] != 0)
      return bBuf[0];

   bBuf[0] = bCMD;
   Si1132_WRITE_REG(Si1132_I2C_ADR, COMMAND_ADR,  bBuf ,1 );

   unsigned long ulStartTickCount = HAL_GetTickCount();
   while( HAL_GetTickCount() - ulStartTickCount < 30)
   {
      Si1132_READ_REG(Si1132_I2C_ADR,  RESPONSE_ADR, bBuf ,1 );
      if (bBuf[0] !=0)
         return bBuf[0];
   }
   return Si1132_ERR_TIME_OUT;                        // Timeout melden
}

//==========================================================================================================================
// Liest ein Paramter aus dem Speicher aus
//==========================================================================================================================
// Parameter:
// bAdr:       Parameter-Adresse
// pOutValue:  Zeiger wo der Parameter - Inhalt gespeichert werden soll
//==========================================================================================================================
// Liefert Si1132_RESPONSE_OK wenn alles OK sonst den entsprechenden Fehlercode
//==========================================================================================================================
BYTE Si1132_Get_Parameter( BYTE bAdr, BYTE *pOutValue )
{
   BYTE bRetValue = Si1132_ExecuteCMD( CMD_PARAM_QUERY | (bAdr & 0x1F) ) ;
   if  (bRetValue  == Si1132_RESPONSE_OK)
      Si1132_READ_REG(Si1132_I2C_ADR,  PARAM_RD_ADR, pOutValue ,1 );
   return bRetValue;
}

//==========================================================================================================================
// Schreibt einen Paramter in dem Speicher
//==========================================================================================================================
// Parameter:
// bAdr:          Parameter-Adresse
// pInOutValue:   Zeiger auf Wert der geschrieben werden soll.
//==========================================================================================================================
// Liefert Si1132_RESPONSE_OK wenn alles OK sonst den entsprechenden Fehlercode
//==========================================================================================================================
BYTE Si1132_Set_Parameter( BYTE bAdr, BYTE *pInOutValue )
{

   Si1132_WRITE_REG(Si1132_I2C_ADR,  PARAM_WR_ADR, pInOutValue ,1 );

   BYTE bRetValue = Si1132_ExecuteCMD( CMD_PARAM_SET | (bAdr & 0x1F) );
   if  (bRetValue  == Si1132_RESPONSE_OK)
      Si1132_READ_REG(Si1132_I2C_ADR,  PARAM_RD_ADR, pInOutValue ,1 );
   return bRetValue;
}


//==========================================================================================================================
// Sensor-Initalisiereung
// return 0=ok sonst Fehler
//==========================================================================================================================
BYTE Si1132_Init( uint32_t uiTime_ms, uint8_t bMessAnzahl )
{
   Si1132_SetMessInterval( uiTime_ms );
   mstSi1132_AVG.bAnzahl = bMessAnzahl;

   // Default Koefficenten laut Datenblatt Seite 12 oben
   const BYTE bUCOEF[4] = { 0x7B, 0x6B, 0x01, 0x00 };

   // VIS_RANGE und IR_RANGE wurden derzeit noch nicht gesetzt -> wir haben derzeit den "normalen"-Range

   BYTE bBuf[1] = { HW_KEY_VALUE };
   BYTE bRetValue = Si1132_WRITE_REG(Si1132_I2C_ADR, HW_KEY_ADR, bBuf ,1 );

   bRetValue += Si1132_READ_REG(Si1132_I2C_ADR, PART_ID_ADR, bBuf ,1 );
   if (bBuf[0] != PART_ID_VALUE)
      return 1;

   bBuf[0] = PARAM_CHLIST_DEFAULT_VALUE;
   bRetValue += Si1132_Set_Parameter(PARAM_CHLIST_ADR, bBuf );

   bRetValue += Si1132_WRITE_REG(Si1132_I2C_ADR, UCOEF0_ADR, (void*) bUCOEF ,4 );

   Si1132_IDEL();                                     // mstSi1132_AVG-Laden

   return bRetValue;
}

//==========================================================================================================================
//    Setzt das Messintervall im Task-Eintrag
//==========================================================================================================================
void Si1132_SetMessInterval( uint32_t uiTime_ms )
{
   if (uiTime_ms<10) uiTime_ms = 10;

   gstSi1132_Func_Table[0].ulTime = uiTime_ms - 10;
   Task_InitTaskExecutionTime( gstSi1132_Func_Table, 0);
}

//==========================================================================================================================
//    Setzt die Anzahl der Messungen über die gemittelt werden soll
//==========================================================================================================================
void Si1132_SetMessAnzahl( uint8_t bAnzahl )
{
   mstSi1132_AVG.bAnzahl = bAnzahl;
}

//==========================================================================================================================
//    Gibt den Zeiger auf die zuletzt gemessenen Sensor-Daten zurück
//==========================================================================================================================
ST_Si1132_MessData* Si1132_GetMessData(void)
{
   return &mstSi1132_MessData;
}

//==========================================================================================================================
//    Formel aus AN523 Seite 3:
//    Lux level = [ [(ALS visible reading) - (ALS visible dark reading)] x (ALS visible coefficient) +
//                  [ (ALSIR reading)      - (ALS IR dark reading)]      x (ALS IR coefficient) ] x gain correction
//==========================================================================================================================

//==========================================================================================================================
//    irLux =       [ (ALSIR reading)      - (ALS IR dark reading)]      x ((ALS IR coefficient)  x gain correction)
//==========================================================================================================================
int16_t Si1132_IR_Lux(uint16_t uiADC_Value )
{
   if (uiADC_Value<gSYS.stSi1132_CalVal.wIR_ADC_DarkValue)    // Keine negativen ADC-Werte zulassen
      uiADC_Value=gSYS.stSi1132_CalVal.wIR_ADC_DarkValue;
   return (int16_t)((float)(uiADC_Value - gSYS.stSi1132_CalVal.wIR_ADC_DarkValue)*gSYS.stSi1132_CalVal.fIR_Scale);
}

//==========================================================================================================================
//    Lux level = [ [(ALS visible reading) - (ALS visible dark reading)] x (ALS visible coefficient) x gain correction) + irLux
//==========================================================================================================================
uint16_t Si1132_VIS_Lux(uint16_t uiADC_Value, int16_t iIR_Lux )
{
   if (uiADC_Value<gSYS.stSi1132_CalVal.wVIS_ADC_DarkValue)    // Keine negativen ADC-Werte zulassen
      uiADC_Value=gSYS.stSi1132_CalVal.wVIS_ADC_DarkValue;
   return (uint16_t)((float)(uiADC_Value - gSYS.stSi1132_CalVal.wVIS_ADC_DarkValue)*gSYS.stSi1132_CalVal.fVIS_Scale) + iIR_Lux;
}

////==========================================================================================================================
////    Hier nun die Taskfunktionen
////==========================================================================================================================
//int Si1132_MakeMessung(void)
//{
//   Si1132_ExecuteCMD( CMD_ALS_FORCE );
//
//   HAL_Delay_ms(2, LPM3_bits);
//
//   // Lese das Messergebnis
//   int iRetValue = Si1132_READ_REG(Si1132_I2C_ADR, ALS_VIS_DATA0_ADR, &mstSi1132MessData,           4 );
//   iRetValue += Si1132_READ_REG(Si1132_I2C_ADR, AUX_DATA0_ADR,        &mstSi1132MessData.wUV_Index ,2 );
//   return iRetValue;
//}

//===============================================================================
//    TASK_IDEL:     Warte-State bis Messung durchgeführt werden soll
//===============================================================================
EN_TASK_STATUS   Si1132_IDEL( void )
{
   mstSi1132_AVG.bLoopCounter = mstSi1132_AVG.bAnzahl;
   mstSi1132_AVG.ulIR         = 0;
   mstSi1132_AVG.ulVIS        = 0;
   mstSi1132_AVG.ulUV_Index   = 0;

   mstSi1132_AVG.wMaxVIS      = 0;
   mstSi1132_AVG.wMaxIR       = 0;
   mstSi1132_AVG.wMaxUV_Index = 0;

   mstSi1132_AVG.wMinVIS      = 0xFFFF;
   mstSi1132_AVG.wMinIR       = 0xFFFF;
   mstSi1132_AVG.wMinUV_Index = 0xFFFF;

   return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP1);
}

//===============================================================================
//    TASK_STEP1:    hier die Messung einleiten (Sensoren aktivieren)
//===============================================================================
EN_TASK_STATUS   Si1132_MESSUNG_START( void )
{
   Si1132_ExecuteCMD( CMD_ALS_FORCE );
   if (isTaskInStreamingMode())
      return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP4);
   else
      return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP2);
}

//===============================================================================
//    TASK_STEP2:    hier die Sensordaten einlesen und verarbeiten (avg/min/max)
//===============================================================================
EN_TASK_STATUS   Si1132_MESSUNG_READ_SENSOR( void )
{
   // Lese das Messergebnis
   ST_Si1132_SensorData stM;
   Si1132_READ_REG(Si1132_I2C_ADR, ALS_VIS_DATA0_ADR, &stM,           4 );
   Si1132_READ_REG(Si1132_I2C_ADR, AUX_DATA0_ADR,     &stM.wUV_Index ,2 );

   mstSi1132_AVG.ulIR         += stM.wIR;
   mstSi1132_AVG.ulVIS        += stM.wVIS;
   mstSi1132_AVG.ulUV_Index   += stM.wUV_Index;

   if (mstSi1132_AVG.wMaxIR < stM.wIR)
      mstSi1132_AVG.wMaxIR = stM.wIR;
   if (mstSi1132_AVG.wMaxVIS < stM.wVIS)
      mstSi1132_AVG.wMaxVIS = stM.wVIS;
   if (mstSi1132_AVG.wMaxUV_Index < stM.wUV_Index)
      mstSi1132_AVG.wMaxUV_Index = stM.wUV_Index;

   if (mstSi1132_AVG.wMinIR > stM.wIR)
      mstSi1132_AVG.wMinIR = stM.wIR;
   if (mstSi1132_AVG.wMinVIS > stM.wVIS)
      mstSi1132_AVG.wMinVIS = stM.wVIS;
   if (mstSi1132_AVG.wMinUV_Index > stM.wUV_Index)
      mstSi1132_AVG.wMinUV_Index = stM.wUV_Index;

   if( --mstSi1132_AVG.bLoopCounter)
      return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP1);
   else
   {
      mstSi1132_MessData.iAvgIR        = Si1132_IR_Lux ((uint16_t) (mstSi1132_AVG.ulIR  / mstSi1132_AVG.bAnzahl) );
      mstSi1132_MessData.wAvgVIS       = Si1132_VIS_Lux((uint16_t) (mstSi1132_AVG.ulVIS / mstSi1132_AVG.bAnzahl), mstSi1132_MessData.iAvgIR);
      mstSi1132_MessData.wAvgUV_Index  = (uint16_t) (mstSi1132_AVG.ulUV_Index   / mstSi1132_AVG.bAnzahl);

      mstSi1132_MessData.iMinIR        = Si1132_IR_Lux ( mstSi1132_AVG.wMinIR );
      mstSi1132_MessData.wMinVIS       = Si1132_VIS_Lux( mstSi1132_AVG.wMinVIS,  mstSi1132_MessData.iMinIR );
      mstSi1132_MessData.wMinUV_Index  = mstSi1132_AVG.wMinUV_Index;

      mstSi1132_MessData.iMaxIR        = Si1132_IR_Lux ( mstSi1132_AVG.wMaxIR );
      mstSi1132_MessData.wMaxVIS       = Si1132_VIS_Lux( mstSi1132_AVG.wMaxVIS, mstSi1132_MessData.iMaxIR);
      mstSi1132_MessData.wMaxUV_Index  = mstSi1132_AVG.wMaxUV_Index;

      return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP3);
   }
}

//===============================================================================
//    TASK_STEP4:    Messdaten im Stream-Mode einlesen
//===============================================================================
EN_TASK_STATUS   Si1132_STREAM_READ_SENSOR(void)
{
   // Lese das Messergebnis
   ST_Si1132_SensorData stM;
   Si1132_READ_REG(Si1132_I2C_ADR, ALS_VIS_DATA0_ADR, &stM,           4 );
   Si1132_READ_REG(Si1132_I2C_ADR, AUX_DATA0_ADR,     &stM.wUV_Index ,2 );

   mstSi1132_AVG.ulIR         = stM.wIR;
   mstSi1132_AVG.ulVIS        = stM.wVIS;
   mstSi1132_AVG.ulUV_Index   = stM.wUV_Index;

   mstSi1132_MessData.iAvgIR        = Si1132_IR_Lux ((uint16_t) (mstSi1132_AVG.ulIR) );
   mstSi1132_MessData.wAvgVIS       = Si1132_VIS_Lux((uint16_t) (mstSi1132_AVG.ulVIS) , mstSi1132_MessData.iAvgIR);
   mstSi1132_MessData.wAvgUV_Index  = (uint16_t) (mstSi1132_AVG.ulUV_Index);

   mstSi1132_MessData.iMinIR        = mstSi1132_MessData.iAvgIR;
   mstSi1132_MessData.wMinVIS       = mstSi1132_MessData.wAvgVIS;
   mstSi1132_MessData.wMinUV_Index  = mstSi1132_MessData.wAvgUV_Index;

   mstSi1132_MessData.iMaxIR        = mstSi1132_MessData.iAvgIR;
   mstSi1132_MessData.wMaxVIS       = mstSi1132_MessData.wAvgVIS;
   mstSi1132_MessData.wMaxUV_Index  = mstSi1132_MessData.wAvgUV_Index;

   return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP5);
}

//===============================================================================
//    TASK_STEP6:    Neue Messung vorbereiten
//===============================================================================
EN_TASK_STATUS   Si1132_STREAM_SEND_RETURN(void)
{
   if (isTaskInStreamingMode())
   {
      Si1132_ExecuteCMD( CMD_ALS_FORCE );
      return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP4);
   }
   else
   {
      return (EN_TASK_STATUS) (TASK_LPM3 | TASK_IDEL);
   }
}

//===============================================================================
// Globale Tabelle mit den Task-Funktionen
//===============================================================================
ST_FUNCTION_TABLE gstSi1132_Func_Table[] =
{
   { Si1132_IDEL                    , 1024   },       // TASK_IDEL:  nichts tun                       -> weiter mit: Si1132_MESSUNG_START  hier die Messperiode eintragen
   { Si1132_MESSUNG_START           , 1      },       // TASK_STEP1: Startet die Sensormessung        -> weiter mit: Si1132_MESSUNG_READ_SENSOR
   { Si1132_MESSUNG_READ_SENSOR     , 0      },       // TASK_STEP2: hier die Sensordaten einlesen    -> weiter mit: Si1132_MESSUNG_START oder Send_Si1132__Daten
   { Task_Send_Si1132_Daten         , 0      },       // TASK_STEP3: Messdaten ausgeben               -> weiter mit: Si1132_IDEL

   { Si1132_STREAM_READ_SENSOR      , 0      },       // TASK_STEP4: hier die Sensordaten einlesen    -> weiter mit: Si1132_MESSUNG_START oder Send_Si1132__Daten
   { Task_StreamSend_Si1132_Daten   , 0      },       // TASK_STEP5: Messdaten ausgeben               -> weiter mit: Si1132_IDEL
   { Si1132_STREAM_SEND_RETURN      , 1      },       // TASK_STEP6: hier die Sensordaten einlesen    -> weiter mit: Si1132_MESSUNG_START oder Send_Si1132__Daten
                                                      //             Achtung:  define in Si1132.h Si1132_STREAM_SEND_RETURN_STEP muss diesen Step angeben
};
