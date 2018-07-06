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
*  P4.2  RF.MOSI        eUSCIA1 MOSI                                           *
*  P4.3  RF.CLK         eUSCIA1 CLK                                            *
*
*
*******************************************************************************/
#include "msp430.h"
#include "Hardware.h"
#include "emeter-structs.h"
#include "RadioEsima.h"
#include "Timer_Modul.h"

#include "SPI_UCSI_Modul.h"
#include <string.h>

#define RADIO_IDEL                     0
#define RADIO_SEND_PHASE1              1
#define RADIO_SEND_PHASE2              2
#define RADIO_SEND_PHASE3              3
#define RADIO_SEND_NEUTRAL             4

#define RADIO_SEND_PHASE1_ONE_TIME     5
#define RADIO_SEND_PHASE2_ONE_TIME     6
#define RADIO_SEND_PHASE3_ONE_TIME     7
#define RADIO_SEND_NEUTRAL_ONE_TIME    8
#define RADIO_SEND_ENERGIE_ONE_TIME    9

#define RADIO_SEND_CMD                 10

#define RADIO_RECEIVE                  11             // hier daten einlesen
#define RADIO_RECEIVE_READY            12             // hier wird in RadioTask die Daten verarbeitet

#define RADIO_GET_RADIO_ID_SEND_CMD    13             // leitet die Abfrage der Funk-ID mit CMD 12 ein
#define RADIO_GET_RADIO_ID_GET_DATA    14             // holt die Funk-ID ab
#define RADIO_GET_RADIO_ID_GOT_DATA    15             // hier kann die Funk-ID abgeholt werden

#define RADIO_SET_RADIO_ID_SEND_CMD    16             // leitet die Abfrage der Funk-ID mit CMD 12 ein


volatile uint8_t  mbRadioStatus = RADIO_IDEL;

BYTE Radio_Tx_Data[RADIO_PREPARED_LENGHT];
BYTE Radio_Rx_Data[RADIO_PREPARED_LENGHT];

////******************************************************************************
//// Init USCIA1 as SPI-Interface for ESIMA radio
////******************************************************************************
//void Radio_Init(void)
//{
//  // Initialize USCI_B1 for SPI Master operation
//  // Put state machine in reset
//  UCA1CTLW0 = UCSWRST;
//  //3-pin, 8-bit SPI master
//  UCA1CTLW0 |= (UCMSB | UCMST | UCMODE_0 | UCSYNC | UCCKPH | UCCKPL);
//  // Clock phase - data captured first edge, change second edge
//  // MSB
//  // Use SMCLK, keep RESET
//  UCA1CTLW0 |= UCSSEL_2;
//  UCA1BR0 = 0x22;
//  UCA1BR1 = 0;
//  // Release USCI state machine
//  UCA1CTLW0 &= ~UCSWRST;
//  UCA1IFG &= ~UCRXIFG;
//}
//
//
////******************************************************************************
//// Send/Get  n nytes (USCIA1) to ESIMA radio
//// Byte* write_data
//// Byte* ptr read_data
//// Byte* data_lenght
////******************************************************************************
//__monitor void Radio_SendGetNByte(BYTE* write_data,BYTE* read_data, BYTE data_lenght)
//{
//  _RadioCS_L();
//      while (data_lenght)
//      {
//        // USCI_A1 TX buffer ready?
//        while (!(UCA1IFG & UCTXIFG));
//        // Transmit data
//        UCA1TXBUF = *write_data;
//        // Increment the pointer on the array
//        // USCI_A1 RX buffer ready?
//
//        while (!(UCA1IFG & UCRXIFG));
//
//        *read_data = UCA1RXBUF;
//        // Increment the pointers read/write on the array
//        ++write_data;
//        ++read_data;
//        // Decrement the Byte counter
//        --data_lenght;
//      }
//  _RadioCS_H();
//}
//
//
//
////******************************************************************************
//// Prepare single phase information
//// Commands   RADIO_COM_SEND_FRAME
////
//// Record     BYTE  bREcType ( RADIO_RECORD_L1, RADIO_RECORD_L2, RADIO_RECORD_L3)
//// Voltage    int   iULx
//// Current    float fILx
//// Power      float fPLx
//// Phase      BYTE  bPhLx
//// Counter    BYTE  bnB
////******************************************************************************
//void Radio_Prepare_Phase_Data
//(BYTE* pbBuf,BYTE bRecType,int iULx, float fILx, float fPLx,BYTE  bPhLx, BYTE bNb)
//{
//  *pbBuf++ = RADIO_COM_SEND_FRAME;               // Prepare command
//  *pbBuf++ = RADIO_DEVICE_TYPE;                  // Prepare device Type
//  *pbBuf++ = bRecType;                           // Prepare Record Type
//  memcpy (pbBuf,(BYTE*)&iULx,sizeof(int));       // Prepare voltage
//  pbBuf += sizeof(int);
//  memcpy (pbBuf,(BYTE*)&fILx,sizeof(float));     // Prepare current
//  pbBuf += sizeof(float);
//  memcpy (pbBuf,(BYTE*)&fPLx,sizeof(float));     // Prepare power
//  pbBuf += sizeof(float);
//  *pbBuf++ =  bPhLx;                            // Prepare phase
//  *pbBuf   =  bNb;                              // Prepare measuering number
//}
//
//void Radio_Prepare_Energy (BYTE* pbBuf,float fEL1,float fEL2,float fEL3)
//{
//  *pbBuf++ = RADIO_COM_SEND_FRAME;               // Prepare command
//  *pbBuf++ = RADIO_DEVICE_TYPE;                  // Prepare device type
//  *pbBuf++ = RADIO_RECORD_W;                      // Prepare record type
//  memcpy (pbBuf,(BYTE*)&fEL1,sizeof(float));     // Prepare energy L1
//  pbBuf += sizeof(float);
//  memcpy (pbBuf,(BYTE*)&fEL2,sizeof(float));     // Prepare energy L2
//  pbBuf += sizeof(float);
//  memcpy (pbBuf,(BYTE*)&fEL3,sizeof(float));     // Prepare energy L3
//  pbBuf += sizeof(float);
//}
//
//
//
////******************************************************************************
//// Send radio command and get answer
//// *TX pbBuf_TX
//// *RX pbBuf_RX
//// BYTE Command
////******************************************************************************
//void Radio_Command(BYTE* pbBuf_TX,BYTE* pbBuf_RX,BYTE bCom)
// {
//      *pbBuf_TX = RADIO_COM_GET_ID;                                          // Prepare command
//       Radio_SendGetNByte(pbBuf_TX, pbBuf_RX,1);                               // Send command
//       __delay_cycles((long int)(MCLK_FREQ * 0.0005));                                        // Wait 1ms, time for slave to decode command
//       memset(pbBuf_TX,0x00,RADIO_PREPARED_LENGHT);                             // Clear TX buffer
//       memset(pbBuf_RX,0x00,RADIO_PREPARED_LENGHT);                             // Clear RX buffer
//      Radio_SendGetNByte(pbBuf_TX,pbBuf_RX,RADIO_PREPARED_LENGHT);
//
//}

//==============================================================================================================================================
// Hier die Bernd-Version:
//==============================================================================================================================================


//==============================================================================================================================================
//    Ab hier der durch das Tool zu erzeugende Code
//==============================================================================================================================================


//==============================================================================================================================================

//==============================================================================================================================================
//    Defines für CMD's, Geräte-Typ und Record-Typen
//==============================================================================================================================================
#define RADIO_CMD_SEND              0x10
#define RADIO_CMD_RECEIVE           0x11
#define RADIO_CMD_SET_ID            0x16
#define RADIO_CMD_GET_ID            0x12

#define RADIO_PC_CMD_SEND_ENERGY    0x01
#define RADIO_PC_CMD_SEND_PAR1      0x02
#define RADIO_PC_CMD_SEND_PAR2      0x03

#define RADIO_DEVICE_TYPE_VALUE     10                // Gerätertyp

#define RADIO_REC_TYP_L1_VALUE       1
#define RADIO_REC_TYP_L2_VALUE       2
#define RADIO_REC_TYP_L3_VALUE       3
#define RADIO_REC_TYP_N_VALUE        4
#define RADIO_REC_TYP_ENERGY_VALUE   5

#define SA_CONNECTED                 0xAA
#define SA_DISCONNECTED              0x00

//==============================================================================================================================================
//    hier die definitionen der sende-record-strukturen - siehe excel-File
//    diese sollten durch das Tool erzeugt werden
//==============================================================================================================================================
#pragma pack(1)                                       // Erzwingen, dass Daten genau so im speicher abgelegt werden wie nachfolgend definiert

typedef struct                                        // Struktur der die Ünertragung der "normalen Messdaten" - wird für alle 3 Phasen verwendet
{
   uint8_t bDummy;                                    // dies ist dem Alignment auf even-adresse geschuldet
   uint8_t bRadioCMD;
   uint8_t bDeviceType;
   uint8_t bRecordType;
   int16_t iU;                                        // Spannung in V
   float   fI;                                        // Strom in A
   float   fE;                                        // Energie in Ws
   int8_t  cCosPhi;                                   // 100 = cos 0° = 1
   uint8_t bRecordNo;                                 // bRecordNo = Datensatznummer um Daten später zusammen führen zu können
}  ST_RADIO_MessData1;

typedef struct                                        // Struktur für die Übertragung der Null-Leiter-Messung
{
   uint8_t  bDummy;                                   // dies ist dem Alignment auf even-adresse geschuldet
   uint8_t  bRadioCMD;
   uint8_t  bDeviceType;
   uint8_t  bRecordType;
   uint16_t uiDummy;
   float    fI;                                       // Strom im Nulleiter in A
   uint32_t  uiDummy1;
   int8_t   cCosPhi;                                  // 100 = cos 0° = 1
   uint8_t  bRecordNo;                                // bRecordNo = Datensatznummer um Daten später zusammen führen zu können
}  ST_RADIO_MessData2;

typedef struct                                        // Struktur für die Übertragung der Energie-Daten
{
   uint8_t  bDummy;                                   // dies ist dem Alignment auf even-adresse geschuldet
   uint8_t  bRadioCMD;
   uint8_t  bDeviceType;
   uint8_t  bRecordType;
   float    fE_L1;                                    // in kWh
   float    fE_L2;                                    // in kWh
   float    fE_L3;                                    // in kWh
}  ST_RADIO_Energy;

typedef struct                                        // Struktur für die Übertragung der Energie-Daten
{
   uint8_t  bDummy;                                   // dies ist dem Alignment auf even-adresse geschuldet uiIndex/fValue liegen nun richtig im speicher
   uint8_t  bDeviceType;
   uint8_t  bRecordType;
   uint8_t  bCMD;                                     // CMD von PC / Datenbank
   uint16_t uiIndex;                                  // iIndex
   float    fValue;                                   // Wert falls Parameter
}  ST_RADIO_ReceiveRec;

typedef struct                                        // Struktur für die Berechnung der Energie-Daten
{
   double   dE_L1;                                    // in kWh
   double   dE_L2;                                    // in kWh
   double   dE_L3;                                    // in kWh
}  ST_CALC_Energy;

ST_CALC_Energy mstEnergy;
struct phase_readings_s mstReadings[4];
uint8_t mbRecordNo = 0;
uint8_t mbRadio_Has_Received_Data = false;

#pragma data_alignment=2                                                  // Sicherstellen, dass stRecData an gerader Adresse anfängt
ST_RADIO_ReceiveRec mstRecData;

uint8_t mbMyFunkID[4];

//==============================================================================================================================================
//    Hier findet die Übertragung der Daten an das Funk-Modul statt
//    __monitor:  Compileranweisung dass Interrupts während der Ausführung dieser Funktion gesperrt sind
//                Am Ende der Funktion wird der Interrupt-Enable-Zustand, der beim Aufruf der Funktion geherrscht hat, wieder hergestellt
//                Dies ist in sofern wichtig, dass sonst durch Sensor-IRQ's diese Funktion unterbrochen werden könnte, die dann selbst
//                diese SPI-Schnittstelle benutzen würden. Das würde zu einem Daten-Mix an der SPI-Schnittstelle führen. ...
//==============================================================================================================================================
__monitor void Radio_SendRecord(void* write_data, BYTE data_lenght)
{
   uint8_t bReceiveBuffer[15];
   uint8_t *pbReceiveBufferPointer = bReceiveBuffer;

   uint8_t *pbData = write_data;
   pbData++;                                          // bDummy überspringen
   data_lenght--;                                     // und deshalb auch das längen-Byte korrigieren

   _RadioCS_L();                                      // CS auf Low
   while (data_lenght)
   {
      *pbReceiveBufferPointer++ = RADIO_PUT_GET_BYTE( *pbData++ );   // Sende Daten an das Radio und speicher die Antwort in bReceiveBufer ab
      data_lenght--;                                  // Decrement the Byte counter
   }
   _RadioCS_H();                                      // CS auf High

   switch (bReceiveBuffer[0])
   {
   case SA_CONNECTED:
      _NOP();                                         // Conenction to the Gateway is establisched. The data can be send.
      break;
   case SA_DISCONNECTED:
      _NOP();                                         // Conenction to the Gateway is not establisched. The data can not be send.
      break;
   case RADIO_CMD_RECEIVE:                            // Rf Transceiver received a new data paket => Get_Frame request
      mbRadio_Has_Received_Data = true;
   default:
      break;
   }

}

__monitor void Radio_ReceiveRecord(void* read_data, BYTE data_lenght)
{
   uint8_t *pbData = read_data;
   data_lenght--;

   _RadioCS_L();                                      // CS auf Low
   *pbData++ = RADIO_PUT_GET_BYTE( RADIO_CMD_RECEIVE );           // Sende Receive-CMD an das Radio-Modul
   while (data_lenght)
   {
      *pbData++ = RADIO_PUT_GET_BYTE( 0x00 );         // Empfange die Daten vom Radio
      data_lenght--;                                  // Decrement the Byte counter
   }
   _RadioCS_H();                                      // CS auf High
}

//==============================================================================================================================================
//    Funktion, welche die Messdaten für L1..L3 ensprechend der drei Mess-Phasen formatiert und an das Funkmodul übergibt
//==============================================================================================================================================
void Radio_SendMessdata1(const struct phase_readings_s *pP_Data, uint8_t bRecordType, uint8_t bRecordNo)
{
   #pragma data_alignment=2
   ST_RADIO_MessData1 st;
   st.bRadioCMD   = RADIO_CMD_SEND;
   st.bDeviceType = RADIO_DEVICE_TYPE_VALUE;
   st.bRecordType = bRecordType;
   st.iU          = (int16_t)((pP_Data->V_rms +50)    / 100);           // +50 wegen Rundung / 100 weil V_rms in 10mV / 1/100V vorliegt
   st.fI          = (float)    pP_Data->I_rms         / 1000.0f;        // Umwandlung mA in A
   st.fE          = (float)    pP_Data->active_power  / 100.0f;         // Umwandlung 10mWs in Ws
   st.cCosPhi     =  pP_Data->power_factor            / 100;            // 0 .. 10000 in 0 .. 100 umwandeln
   st.bRecordNo   =  bRecordNo;

   Radio_SendRecord( &st, sizeof(st) );               // Hier den Datensatz ans Funkmodul übergeben
}

//==============================================================================================================================================
//    Funktion, welche die Messdaten für den Null-Leiter-Strom ensprechend formatiert und an das Funkmodul übergibt
//==============================================================================================================================================
void Radio_SendMessdata2(const struct phase_readings_s *pP_Data, uint8_t bRecordType, uint8_t bRecordNo)
{
   #pragma data_alignment=2
   ST_RADIO_MessData2 st;
   st.bRadioCMD   = RADIO_CMD_SEND;
   st.bDeviceType = RADIO_DEVICE_TYPE_VALUE;
   st.bRecordType = bRecordType;
   st.fI          = (float) pP_Data->I_rms         / 1000.0f;        // Umwandlung mA in A
   st.cCosPhi     =  pP_Data->active_power         / 100;            // 0 .. 10000 in 0 .. 100 umwandeln
   st.bRecordNo   = bRecordNo;

   Radio_SendRecord( &st, sizeof(st) );               // Hier den Datensatz ans Funkmodul übergeben
}

void Radio_SendEnergy(void)
{
   #pragma data_alignment=2
   ST_RADIO_Energy st;                                // hier die Daten speichern, da Daten nur auf Abruf gesendet werden
   st.bRadioCMD   = RADIO_CMD_SEND;
   st.bDeviceType = RADIO_DEVICE_TYPE_VALUE;
   st.bRecordType = RADIO_REC_TYP_ENERGY_VALUE;
   st.fE_L1 = (float) mstEnergy.dE_L1;                // aus double wird float beim senden
   st.fE_L2 = (float) mstEnergy.dE_L2;                // aus double wird float beim senden
   st.fE_L3 = (float) mstEnergy.dE_L3;                // aus double wird float beim senden

   Radio_SendRecord( &st, sizeof(st) );               // Hier den Datensatz ans Funkmodul übergeben
}

void Radio_SEND_GetID_CMD( void )
{
   uint8_t bBuffer[15];
   bBuffer[0] = RADIO_COM_GET_ID;
   for (int i=14; i; i--)
      bBuffer[i] = 0;
   Radio_SendRecord( bBuffer, sizeof(bBuffer) );
}

void Radio_SEND_GetCMD( void )
{
   uint8_t bBuffer[15];
   Radio_ReceiveRecord(bBuffer, sizeof(bBuffer) );
   if ( bBuffer[0] == RADIO_COM_GET_ID)
   {
      mbMyFunkID[0] = bBuffer[1];
      mbMyFunkID[1] = bBuffer[2];
      mbMyFunkID[2] = bBuffer[3];
      mbMyFunkID[3] = bBuffer[4];
   }
}

void Radio_SEND_SetID_CMD( void )
{
   uint8_t bBuffer[16];                               // Ein Byte mehr weil normale sende-struktur ein Dummy-Byte enthält
   bBuffer[1] = RADIO_COM_SET_ID;
   bBuffer[2] = mbMyFunkID[0];
   bBuffer[3] = mbMyFunkID[1];
   bBuffer[4] = mbMyFunkID[2];
   bBuffer[5] = mbMyFunkID[3];
   for (int i=15; i>4; i--)
      bBuffer[i] = 0;
   Radio_SendRecord( bBuffer, sizeof(bBuffer) );
}

#pragma vector=PORT2_VECTOR
__interrupt void Radio_ISR(void)
{
   RADIO_DATA_READY_IFG &= ~RADIO_DATA_READY_BIT;
   if (!isRADIO_DATA_READY)
      return;                                         // wir kommen zu spät -> Warten auf das nächste mal

   switch (mbRadioStatus)
   {
   case RADIO_IDEL:                                   // nichts tun
      break;

   // Durch Radio_UpdateEmeter veranlasste State
   case RADIO_SEND_PHASE1:                            // Phase 1 senden
      Radio_SendMessdata1( &mstReadings[0], RADIO_REC_TYP_L1_VALUE, mbRecordNo);
      mbRadioStatus = RADIO_SEND_PHASE2;
      break;
   case RADIO_SEND_PHASE2:                            // Phase 2 senden
      Radio_SendMessdata1( &mstReadings[1], RADIO_REC_TYP_L2_VALUE, mbRecordNo);
      mbRadioStatus = RADIO_SEND_PHASE3;
      break;
   case RADIO_SEND_PHASE3:                            // Phase 3 senden
      Radio_SendMessdata1( &mstReadings[2], RADIO_REC_TYP_L3_VALUE, mbRecordNo);
      #if defined(NEUTRAL_MONITOR_SUPPORT)
         mbRadioStatus = RADIO_SEND_NEUTRAL;
      #else
         if (mbRadio_Has_Received_Data)
            mbRadioStatus = RADIO_RECEIVE;
         else
            mbRadioStatus = RADIO_IDEL;
      #endif
      break;

   case RADIO_SEND_NEUTRAL:                           // Neutral-Leiterdaten senden
      Radio_SendMessdata2( &mstReadings[3], RADIO_REC_TYP_N_VALUE,  mbRecordNo);
      if (mbRadio_Has_Received_Data)
         mbRadioStatus = RADIO_RECEIVE;
      else
         mbRadioStatus = RADIO_IDEL;
      break;
   case RADIO_RECEIVE:                                // Daten von Funkmodul empfangen
      Radio_ReceiveRecord( &mstRecData, sizeof(mstRecData) );
      mbRadioStatus = RADIO_RECEIVE;
      break;
   case RADIO_RECEIVE_READY:
      break;                                          // In diesem State stehen bleiben - Radio_Task prüft diesen state und verarbeitet die Daten

   // Durch Radio_Task veranlasste States
   case RADIO_SEND_PHASE1_ONE_TIME :                  // Phase1-Daten senden
      Radio_SendMessdata1( &mstReadings[0], RADIO_REC_TYP_L1_VALUE, mbRecordNo);
      mbRadioStatus = RADIO_IDEL;
      break;
   case RADIO_SEND_PHASE2_ONE_TIME:                   // Phase2-Daten senden
      Radio_SendMessdata1( &mstReadings[1], RADIO_REC_TYP_L2_VALUE, mbRecordNo);
      mbRadioStatus = RADIO_IDEL;
      break;
   case RADIO_SEND_PHASE3_ONE_TIME:                   // Phase3-Daten senden
      Radio_SendMessdata1( &mstReadings[2], RADIO_REC_TYP_L3_VALUE, mbRecordNo);
      mbRadioStatus = RADIO_IDEL;
      break;
   case RADIO_SEND_NEUTRAL_ONE_TIME:                  // Neutral-Leiter-Daten senden
      Radio_SendMessdata2( &mstReadings[3], RADIO_REC_TYP_N_VALUE,  mbRecordNo);
      mbRadioStatus = RADIO_IDEL;
      break;
   case RADIO_SEND_ENERGIE_ONE_TIME:
      Radio_SendEnergy();                             // Energiewerte senden
      mbRadioStatus = RADIO_IDEL;
      break;

   // Durch externe Funktionen veranlasste States
   case RADIO_GET_RADIO_ID_SEND_CMD:
      Radio_SEND_GetID_CMD();
      mbRadioStatus = RADIO_GET_RADIO_ID_GET_DATA;
      break;
   case RADIO_GET_RADIO_ID_GET_DATA:                  // Sendet das Kommando um die Funk-ID auszulesen (0x12)
      Radio_SEND_GetCMD();
      mbRadioStatus = RADIO_GET_RADIO_ID_GOT_DATA;
      break;
   case RADIO_GET_RADIO_ID_GOT_DATA:                  // Sendet das Lese-Kommando und liest die Daten  (0x11)
      break;                                          // hier dem Radio_GetID melden dass Funk-ID-Daten nun eingelesen sind

   case RADIO_SET_RADIO_ID_SEND_CMD:                  // leitet die Abfrage der Funk-ID mit CMD 12 ein
      Radio_SEND_SetID_CMD();
      mbRadioStatus = RADIO_IDEL;
      break;

   default:
      break;
   }
}

uint8_t WaiteForState( uint8_t bState, uint32_t ulTimeOut)
{
   SetTimeoutValue( ulTimeOut );
   while ( GetTimeoutValue() )
   {
      if (mbRadioStatus == bState)
         return 0;                                    // 0 = alles ok - gewünschter State ist erreicht
   }
   return 1;                                          // 1 = Timeout abgelaufen
}

//==============================================================================================================================================
//    Diese Funktion wird einmal pro Sekunde aufgerufen, wenn neue Messdaten vorliegen. Der Aufruf erfolgt in der Main-loop
//==============================================================================================================================================
void Radio_UpdateEmeter(const struct phase_parms_s pst_phase_parms[4] )
{
   memcpy( &mstReadings[0], &pst_phase_parms[0].readings, sizeof(struct phase_readings_s));
   memcpy( &mstReadings[1], &pst_phase_parms[1].readings, sizeof(struct phase_readings_s));
   memcpy( &mstReadings[2], &pst_phase_parms[2].readings, sizeof(struct phase_readings_s));
   #if defined(NEUTRAL_MONITOR_SUPPORT)
   memcpy( &mstReadings[3], &pst_phase_parms[3].readings, sizeof(struct phase_readings_s));
   #endif
   mbRecordNo++;

   if (mbRadioStatus == RADIO_IDEL)
      mbRadioStatus = RADIO_SEND_PHASE1;

   // double ist notwendig, da sehr kleine Werte (Ws) mit sehr großen Werten (kWh) addiert werden
   mstEnergy.dE_L1 += (double) pst_phase_parms[0].readings.active_power / 3600000.0;      // Ws ind kWh umrechnen
   mstEnergy.dE_L2 += (double) pst_phase_parms[1].readings.active_power / 3600000.0;      // Ws ind kWh umrechnen
   mstEnergy.dE_L3 += (double) pst_phase_parms[2].readings.active_power / 3600000.0;      // Ws ind kWh umrechnen

}

void Radio_Task(void)
{
//   if( mbRadioStatus == RADIO_SEND_PHASE1 && isRADIO_DATA_READY )
//      RADIO_DATA_READY_IFG |= RADIO_DATA_READY_BIT;

   if( mbRadioStatus == RADIO_RECEIVE_READY )
   {
      mbRadioStatus = RADIO_IDEL;
      switch (mstRecData.bCMD)
      {
      case RADIO_PC_CMD_SEND_ENERGY:
         switch(mstRecData.uiIndex)
         {
         case RADIO_REC_TYP_L1_VALUE:
            mbRadioStatus = RADIO_SEND_PHASE1_ONE_TIME;
            break;
         case RADIO_REC_TYP_L2_VALUE:
            mbRadioStatus = RADIO_SEND_PHASE2_ONE_TIME;
            break;
         case RADIO_REC_TYP_L3_VALUE:
            mbRadioStatus = RADIO_SEND_PHASE3_ONE_TIME;
            break;
         case RADIO_REC_TYP_N_VALUE:
            mbRadioStatus = RADIO_SEND_NEUTRAL_ONE_TIME;
            break;
         case RADIO_REC_TYP_ENERGY_VALUE:
            mbRadioStatus = RADIO_SEND_ENERGIE_ONE_TIME;
            break;
         }
         break;
      case RADIO_PC_CMD_SEND_PAR1:
         mstEnergy.dE_L1 = 0.0;                       // Energie-Werte auf 0 stellen
         mstEnergy.dE_L2 = 0.0;
         mstEnergy.dE_L3 = 0.0;

         break;
      case RADIO_PC_CMD_SEND_PAR2:
         break;
      }
   }
}

void Radio_PowerInit()
{
   mstEnergy.dE_L1 = 0.0;                             // Energie-Werte auf 0 stellen
   mstEnergy.dE_L2 = 0.0;
   mstEnergy.dE_L3 = 0.0;
   // ToDo weitere initatlierungen?
}

//
void Radio_ResetOn()
{
  // ToDo: bei Radio Reset aktivierung
  _RadioReset_L();
}

void Radio_ResetOff()
{
  // ToDo: bei Radio Reset deaktivierung
  _RadioReset_H();
}

void Radio_Wakeup()
{
   // ToDo: Radio einschalten, warten bis Spannung stabil, Ports auf high und auf SPI-Funktion umschalten
  _RadioOn();
}
void Radio_Sleep()
{
   // ToDo: Ports auf low bzw auf Output statt SPI-Funktion, Power off
  _RadioOff();
}


uint8_t Radio_SetID(uint8_t bID[4])
{
   uint8_t bRetValue=0;
   bRetValue  = (WaiteForState( RADIO_IDEL , 100)) ? 1 : 0 ;
   if  (bRetValue == 0)
   {
      mbMyFunkID[0] = bID[0];
      mbMyFunkID[1] = bID[1];
      mbMyFunkID[2] = bID[2];
      mbMyFunkID[3] = bID[3];
      mbRadioStatus = RADIO_SET_RADIO_ID_SEND_CMD;
      bRetValue |= (WaiteForState( RADIO_IDEL , 100)) ? 2 : 0 ;
   }
   mbRadioStatus = RADIO_IDEL;
   return bRetValue;
}

uint8_t Radio_GetID(uint8_t bID[4])
{
   uint8_t bRetValue=0;
   // erstmal warten, dass die State-Maschine in den IDEL-Mode geht
   bRetValue  = (WaiteForState( RADIO_IDEL , 100)) ? 1 : 0 ;
   if  (bRetValue == 0)
   {
      // Abfrage durch setzen des Status starten
      mbRadioStatus = RADIO_GET_RADIO_ID_SEND_CMD;
      // Warten bis die Daten gekommen sind
      bRetValue |= (WaiteForState( RADIO_GET_RADIO_ID_GOT_DATA , 100)) ? 2 : 0 ;

      bID[0] = mbMyFunkID[0];
      bID[1] = mbMyFunkID[1];
      bID[2] = mbMyFunkID[2];
      bID[3] = mbMyFunkID[3];
   }
   mbRadioStatus = RADIO_IDEL;
   return  bRetValue;
}