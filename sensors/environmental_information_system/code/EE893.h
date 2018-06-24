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
 * Hahn-Schickard:	API für EE893-Sensor (CO2-Sensor)
 *********************************************************************
 * FileName:         EE893.h
 * Dependencies:     See INCLUDES section below
 * Processor:        MSP430
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Bernd Ehrbrecht		03.09.2015
 ********************************************************************/

#ifndef __EE893__
#define __EE893__

#include <stdint.h>

typedef struct
{
   uint16_t  wCO2_act;
   uint16_t  wCO2_avg;
   uint16_t  wHumity;
   int16_t   iTemp;

   uint8_t   bStatus;
}  ST_EE893_MESS_DATA;

void EE893_Init( uint32_t ulMessInterval_ms );
void EE893_SetMessInterval(uint32_t ulMessInterval_ms );

ST_EE893_MESS_DATA *EE893_GetData(void);

extern ST_FUNCTION_TABLE gstEE893_Func_Table[];
#endif