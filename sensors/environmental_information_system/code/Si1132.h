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

#ifndef Si1132_
#define Si1132_

#include <stdint.h>

#include "Task_Manager.h"

//------------------------------------------------------------------------------------------------------------------------
// Nachfolgendes Define bestimmt, in welchem Step in der ST_FUNCTION_TABLE nach dem Senden der Daten weiter gemacht wird
// Dieser Wert muss mit der Tabelle: ST_FUNCTION_TABLE gstSi1132_Func_Table[] mit dem Si1132_STREAM_SEND_RETURN Step
// übereinstimmen
//------------------------------------------------------------------------------------------------------------------------
#define Si1132_STREAM_SEND_RETURN_STEP   (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP6)

#define Si1132_RESPONSE_OK                   0x01

#define Si1132_ERR_IVALID_SETTING            0x80
#define Si1132_ERR_ALS_VIS_ADC_OVERFLOW      0x8C
#define Si1132_ERR_ALS_IR_ADC_OVERFLOW       0x8D
#define Si1132_ERR_ALS_AUX_ADC_OVERFLOW      0x8E
#define Si1132_ERR_TIME_OUT                  0x8F


typedef struct
{
   uint16_t   wMinVIS;                                // lux
   int16_t    iMinIR;                                 // lux
   uint16_t   wMinUV_Index;                           // uv-Index * 100

   uint16_t   wMaxVIS;
   int16_t    iMaxIR;
   uint16_t   wMaxUV_Index;

   uint16_t   wAvgVIS;
   int16_t    iAvgIR;
   uint16_t   wAvgUV_Index;

   uint8_t    bStatus;                                // Sensorstatus
}  ST_Si1132_MessData;

//==========================================================================================================================
//    Hier die struct um die Si1132-ADC-Daten in Lux umrechnen zu können. Muss in gSYS gespeichert werden
//==========================================================================================================================
typedef struct
{
   uint16_t   wVIS_ADC_DarkValue;                     // ADC-Dark-Value vom sichbaren Licht
   uint16_t   wIR_ADC_DarkValue;                      // ADC-Dark-Value vom infrarotem Licht
   float      fVIS_Scale;                             // ALS visible coefficient * gain correction
   float      fIR_Scale;                              // ALS IR coefficient      * gain correction
}  ST_Si1132_CAL_VALUES;

//==========================================================================================================================
// Sensor-Initalisiereung
// return 0=ok sonst Fehler
//==========================================================================================================================
BYTE Si1132_Init( uint32_t ulTime_ms , uint8_t bMessAnzahl );
void Si1132_SetMessInterval( uint32_t ulTime_ms );
void Si1132_SetMessAnzahl( uint8_t bAnzahl );

ST_Si1132_MessData * Si1132_GetMessData(void);

//int  Si1132_MakeMessung(void);

extern ST_FUNCTION_TABLE gstSi1132_Func_Table[];

#endif