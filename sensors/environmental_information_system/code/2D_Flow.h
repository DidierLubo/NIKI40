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

#ifndef __2D_Flow__
#define __2D_Flow__

#include <stdint.h>
#include "Task_Manager.h"


//------------------------------------------------------------------------------------------------------------------------
// Nachfolgendes Define bestimmt, in welchem Step in der ST_FUNCTION_TABLE nach dem Senden der Daten weiter gemacht wird
// Dieser Wert muss mit der Tabelle: ST_FUNCTION_TABLE gstFlow_Func_Table[] mit dem Flow_Send_Return Step übereinstimmen
//------------------------------------------------------------------------------------------------------------------------
#define FLOW_STREAM_SEND_RETURN_STEP   (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP8)

//------------------------------------------------------------------------------------------------------------------------
//    Achtung mit den längen wegen der Adressvergabe und den 512-Bytes Segtorgröße beim löschen
//------------------------------------------------------------------------------------------------------------------------
#define FLOW_LOOK_UP_SIZE      1024                    // länge der Flow-Table Look-Up-Tabelle

typedef struct
{
   int16_t   iFlowX_avg;
   int16_t   iFlowY_avg;
   int16_t   iFlowX_min;
   int16_t   iFlowY_min;
   int16_t   iFlowX_max;
   int16_t   iFlowY_max;
   int16_t   iAD_X;
   int16_t   iAD_Y;
   uint8_t   bStatus;
} ST_FLOW_MESS_DATA;

void Flow_Init( uint32_t ulTime_ms , uint8_t bMessAnzahl, uint16_t uiDAC_Heizer );

void Flow_SetMessInterval( uint32_t ulTime_ms );
void Flow_SetMessAnzahl( uint8_t bAnzahl );
void Flow_Set_Heizer_DAC( uint16_t uiDAC );

void Flow_ClearLookUpTables( void );
void Flow_LoadLookUpTable  ( uint16_t iIndex, int16_t *bDaten, uint16_t wLen, uint8_t isX_Value );
void Flow_SetIndendityFlowTable ( void );

ST_FLOW_MESS_DATA * Flow_GetMessData(void);

extern ST_FUNCTION_TABLE gstFlow_Func_Table[];

#endif