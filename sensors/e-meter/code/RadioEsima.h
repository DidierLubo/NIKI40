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
/*******************************************************************************
*  RadioEsima.h Software- Schnittstelle zu HSU/EnOCean Funkmodulen             *
*                                                                              *
*  Kommuniaktion über SPI- Schnittstelle                                       *
*  P2.2  RF.DataReady   Port, In                                               *
*  P5.5  RF.OnOff       Port,   Out, L -> On                                   *
*  P5.6  RF.Reset       Port,                                                  *
*  P4.0  RF.CS          Port, Out                                              *
*                                                                              *
*  P4.1  RF.MISO        eUSCIA1 MISO                                           *
*  P4.2  RF.MOSI        eUSCIA1 MISO                                           *
*  P4.3  RF.CLK         eUSCIA1 CLK                                            *
*
*
*******************************************************************************/


#ifndef RADIO_ESIMA_INCLUDED
#define RADIO_ESIMA_INCLUDED

#define RADIO_CS_BIT            BIT0
#define RADIO_CS_PORT           P4OUT
#define RADIO_ONOFF_BIT         BIT5
#define RADIO_ONOFF_PORT        P5OUT
#define RADIO_RESET_BIT         BIT6
#define RADIO_RESET_PORT        P5OUT

#define RADIO_DATA_READY_PORT   P2IN
#define RADIO_DATA_READY_BIT    BIT2


#define _RadioCS_L()            RADIO_CS_PORT &= ~RADIO_CS_BIT
#define _RadioCS_H()            RADIO_CS_PORT |= RADIO_CS_BIT

#define _RadioOn()              RADIO_ONOFF_PORT &= ~RADIO_ONOFF_BIT
#define _RadioOff()             RADIO_ONOFF_PORT |=  RADIO_ONOFF_BIT

#define _RadioReset_L()         RADIO_RESET_PORT &= ~RADIO_RESET_BIT
#define _RadioReset_H()         RADIO_RESET_PORT |= RADIO_RESET_BIT

#define RADIO_PREPARED_LENGHT 15


// *** Device Definitionen
#define RADIO_DEVICE_TYPE       10

// *** Funk ID Definitionen
#define RADIO_HSU_ID            0x07            /* Modul-Kennung höchstwertiges Byte HSU        0x07 */
#define RADIO_ENOCEAN_ID        0xE0            /* Modul-Kennung höchstwertiges Byte ENOCEAN    0xE0 */

// *** Command Definitionen
#define RADIO_COM_SEND_FRAME    16
#define RADIO_COM_GET_FRAME     17
#define RADIO_COM_GET_ID        18
#define RADIO_COM_SET_ID        22




// *** E-Meter Record Definitionen
#define RADIO_RECORD_L1         20
#define RADIO_RECORD_L2         21
#define RADIO_RECORD_L3         22
#define RADIO_RECORD_LN         23
#define RADIO_RECORD_W          24
#define RADIO_RECORD_STATUS     25



typedef struct stRADIO {
  BYTE TX[RADIO_PREPARED_LENGHT];
  BYTE RX[RADIO_PREPARED_LENGHT];
} tyRADIO;

void Radio_Command(BYTE* pbBuf_TX,BYTE* pbBuf_RX,BYTE bCom);
void Radio_Prepare_Energy (BYTE* pbBuf,float fEL1,float fEL2,float fEL3);
void Radio_Prepare_Energy_Data (BYTE* pbBuf,BYTE bCom,float fEL1, float fEL2, float fEL3);
void Radio_Prepare_Phase_Data  (BYTE* pbBuf,BYTE bRecType,int iULx, float fILx, float fPLx,BYTE  bPhLx, BYTE bNb);
void Radio_Init(void);
void Radio_Task(void);
void Radio_ResetOn();
void Radio_ResetOff();
void Radio_Wakeup();
void Radio_Sleep();
void Radio_UpdateEmeter(const struct phase_parms_s pst_phase_parms[4] );
__monitor void Radio_SendGetNByte(BYTE* write_data,BYTE* read_data, BYTE data_lenght);

// return 0: ok sonst fehler
uint8_t Radio_SetID(uint8_t bID[4]);
// return 0: ok sonst fehler
uint8_t Radio_GetID(uint8_t bID[4]);

#endif