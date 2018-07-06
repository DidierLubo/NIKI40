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
#ifndef RF430
#define RF430

#include "BME280.h"
#include "Si1132.h"
#include "2D_Flow.h"
#include "EE893.h"

//define the values for Granite's registers we want to access
#define CONTROL_REG 		         0xFFFE
#define STATUS_REG			      0xFFFC
#define INT_ENABLE_REG		      0xFFFA
#define INT_FLAG_REG		         0xFFF8
#define CRC_RESULT_REG		      0xFFF6
#define CRC_LENGTH_REG		      0xFFF4
#define CRC_START_ADDR_REG	      0xFFF2
#define COMM_WD_CTRL_REG	      0xFFF0
#define VERSION_REG			      0xFFEE               //contains the software version of the ROM
#define TEST_FUNCTION_REG        0xFFE2
#define TEST_MODE_REG		      0xFFE0

//define the different virtual register bits
//CONTROL_REG bits
#define SW_RESET		            BIT0
#define RF_ENABLE		            BIT1
#define INT_ENABLE		         BIT2
#define INTO_HIGH		            BIT3
#define INTO_DRIVE		         BIT4
#define BIP8_ENABLE		         BIT5
#define STANDBY_ENABLE	         BIT6
#define TEST430_ENABLE	         BIT7

//STATUS_REG bits
#define READY			            BIT0
#define CRC_ACTIVE		         BIT1
#define RF_BUSY			         BIT2

//INT_ENABLE_REG bits
#define EOR_INT_ENABLE		      BIT1
#define EOW_INT_ENABLE		      BIT2
#define CRC_INT_ENABLE		      BIT3
#define BIP8_ERROR_INT_ENABLE		BIT4
#define NDEF_ERROR_INT_ENABLE		BIT5
#define GENERIC_ERROR_INT_ENABLE	BIT7

//INT_FLAG_REG bits
#define EOR_INT_FLAG	            BIT1
#define EOW_INT_FLAG	            BIT2
#define CRC_INT_FLAG	            BIT3
#define BIP8_ERROR_INT_FLAG	   BIT4
#define NDEF_ERROR_INT_FLAG	   BIT5
#define GENERIC_ERROR_INT_FLAG	BIT7

//COMM_WD_CTRL_REG bits
#define WD_ENABLE	               BIT0
#define TIMEOUT_PERIOD_2_SEC	   0
#define TIMEOUT_PERIOD_32_SEC	   BIT1
#define TIMEOUT_PERIOD_8_5_MIN	BIT2
#define TIMEOUT_PERIOD_MASK		BIT1 + BIT2 + BIT3

#define TEST_MODE_KEY 0x004E

#define NDEF_FLAG_MB             BIT7                 // Message Begin
#define NDEF_FLAG_ME             BIT6                 // Message End
#define NDEF_FLAG_MB_ME          (BIT7 | BIT6)
#define NDEF_FLAG_NONE           0                    // Für zwischen drin

void RF430_Init(void);

void RF430_Send_BME280_Data( ST_BME280_MESS_DATA  *pstM );
void RF430_Send_Si1132_Data( ST_Si1132_MessData   *pstM );
void RF430_Send_Flow_Data  ( ST_FLOW_MESS_DATA    *pstM );
void RF430_Send_Akku_Data  ( uint8_t bAkkuValue );
void RF430_Send_EE893_Data (ST_EE893_MESS_DATA *pstM );

extern ST_FUNCTION_TABLE gstRF430_Func_Table[];
#endif