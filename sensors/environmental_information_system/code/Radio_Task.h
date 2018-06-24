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
/* Hahn-Schickard:	   UART-Interface zum CC1350 (Stackforce)
 *********************************************************************
 * FileName:            UART_Task.h
 * Dependencies:        See INCLUDES section below
 * Processor:           Apollo
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Bernd Ehrbrecht		17.02.2017
 ********************************************************************/

#ifndef __UART_TASK__
#define __UART_TASK__

#include "Task_Manager.h"
#include "BME280.h"
#include "Si1132.h"
#include "2D_Flow.h"
#include "EE893.h"
                                                      // Sicherstellen dass SFD LEN CMD und CRC + reserve Platz haben
#define UART_RX_BUFFER_SIZE         (256 + 10)
#define UART_TX_BUFFER_SIZE         (256 + 10)

#define RADIO_MAX_OBJECT_SIZE        256

#define RADIO_SFD                    0xA5
#define RADIO_RESPONSE_TIME_OUT      100              // 100mal Task-Step ausführen Task-Step wird derzeit alle 10ms ausgeführt -> 1s

//===============================================================================
//    Hier die Task-Definition für den Task-Manager
//===============================================================================
extern ST_FUNCTION_TABLE gst_Radio_Func_Table[];

//===============================================================================
//    Globaler Speicher für Messdaten (LWM2M-Modul und Radio-Modul-Zugriffe)
//===============================================================================
typedef struct ST_BME280_MD
{
   ST_BME280_MESS_DATA stCur;
   ST_BME280_MESS_DATA stMin;
   ST_BME280_MESS_DATA stMax;
   uint32_t  uiStatus;
}  ST_BME280_MD;

extern ST_BME280_MD        gstBME280_MD;
extern ST_Si1132_MessData  gstSi1132_MD;
extern ST_FLOW_MESS_DATA   gstFlow_MD;

extern ST_FUNCTION_TABLE gstRadio_Task;

void Radio_Task_Init(void);
//void Radio_Get_FunkID(uint8_t *pbBuffer);
//void Radio_Set_FunkID(uint8_t *pbBuffer);

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

void Radio_BME280_ResetMinMax(void);

//===========================================================================================================
//    Initialisiert das Funk-Modul neu - z. B. weil sich Device-Name oder IP-Adresse geändert hat
//===========================================================================================================
void Radio_ReInitFunkModul(void);

//===========================================================================================================
//    Sendet einen Record über UART. (Payload muss bereits richtigen ENDIAN haben.
//    Achtung: CC1350 muss bereits eingeschalten und aktiv sein!!!
//===========================================================================================================
//    return =0 alles ok, sonst -1 wenn Daten nicht gesendet wurden (FiFo zu klein)
//===========================================================================================================
int32_t Radio_Send_Record( uint8_t bCMD, void *pvData, uint32_t uiLen );

#endif


