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
 * Hahn-Schickard:	BME280 von Bosch - Druck / Feuchte / Temp-Sensor
 *********************************************************************
 * FileName:         BME280.h
 * Dependencies:     See INCLUDES section below
 * Processor:        MSP430
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Bernd Ehrbrecht		25.08.2015
 ********************************************************************/

#ifndef  __BME280__
#define  __BME280__

#include <stdint.h>

#include "Task_Manager.h"


typedef struct
{
   int32_t    i32Temp;                                // Wert in 1/100°C: Output value of “5123” equals 51.23 DegC.
   uint32_t   u32Pressure;                            // Wert in Pa: value of “96386” equals 96386 Pa = 963.86 hPa
   uint32_t   u32Humity;                              // Q22.10-Format: value of “47445” represents 47445/1024 = 46.333 %RH
   uint8_t    bStatus;
}  ST_BME280_MESS_DATA;

uint8_t BME280_Init( uint32_t uiTime_ms );

void BME280_SetMessinterval( uint32_t uiTime_ms );

ST_BME280_MESS_DATA *BME280_GetLastMessdata(void);

//================================================================================================================================
//    Hier die (implementierten) Task-Funktionen
//================================================================================================================================

EN_TASK_STATUS   BME280_IDEL( void );                 // Initialisiert alles (z. B Auswerte Variablen) und geht dann in den Zustand TASK_WAIT_FOR_MESS_START
EN_TASK_STATUS   BME280_MESSUNG_START( void );        // hier die Messung einleiten (Sensoren aktivieren,
EN_TASK_STATUS   BME280_MESSUNG_READ_SENSOR( void );  // hier die Sensordaten einlesen min/max/avg bilden etc. falls notwendig eine neue Sensormessung beginnen

extern ST_FUNCTION_TABLE gstBME280_Func_Table[];

#endif