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
 * Hahn-Schickard:	Radio-Modul
 *********************************************************************
 * FileName:         Radio_Task.h
 * Dependencies:     See INCLUDES section below
 * Processor:        MSP430
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Bernd Ehrbrecht		14.08.2015
 ********************************************************************/

#ifndef __Radio_Task__
#define __Radio_Task__

#include "stdint.h"

#include "BME280.h"
#include "Si1132.h"
#include "2D_Flow.h"
#include "EE893.h"

#include "Task_Manager.h"

void Radio_Init(void);
void Radio_Get_FunkID(uint8_t *pbBuffer);
void Radio_Set_FunkID(uint8_t *pbBuffer);


#define RADIO_SENSOR_STATUS_OK               0
#define RADIO_SENSOR_STATUS_BME280_ERR      BIT0
#define RADIO_SENSOR_STATUS_Si1132_ERR      BIT1
#define RADIO_SENSOR_STATUS_FLOW_ERR        BIT2
#define RADIO_SENSOR_STATUS_EE893_ERR       BIT3
#define RADIO_SENSOR_STATUS_AKKU_ERR        BIT4
#define RADIO_SENSOR_STATUS_NFC_ERR         BIT5

#define RADIO_SENSOR_STATUS_RESET           BIT7

void Radio_SetStatusBit  ( uint8_t bStatus );
void Radio_ResetStatusBit( uint8_t bStatus );

void Radio_Send_BME280_Data( ST_BME280_MESS_DATA *pstM );
void Radio_Send_Si1132_Data( ST_Si1132_MessData *pstM  );
void Radio_Send_Flow_Data  ( ST_FLOW_MESS_DATA *pstM   );
void Radio_Send_EE893_Data ( ST_EE893_MESS_DATA *pstM  );
void Radio_Send_Akku_Data  ( uint8_t bAkkuValue        ) ;
//extern ST_FUNCTION_TABLE gstRadio_Func_Table[];

#endif