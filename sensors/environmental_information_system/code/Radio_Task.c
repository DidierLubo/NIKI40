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
 * FileName:            UART_Task.c
 * Dependencies:        See INCLUDES section below
 * Processor:           Apollo
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Bernd Ehrbrecht		17.02.2017
 ********************************************************************/

#include "hardware.h"
#include "globals.h"
#include "Radio_Task.h"
#include "LWM2M.h"
#include "LWM2M_MessData_Resources.h"

#include "UART_USCI.h"

#include <stdio.h>
#include <string.h>

//===============================================================================
// GAS = Genaral API Set siehe Kapitel 3.4 in emb6_serial_api
//===============================================================================
#define GAS_RET                     0x00
#define GAS_PING                    0x10
#define GAS_CFG_SET                 0x20
#define GAS_CFG_GET                 0x21
#define GAS_CFG_RSP                 0x22
#define GAS_DEVICE_INIT             0x31
#define GAS_DEVICE_START            0x32
#define GAS_DEVICE_STOP             0x33
#define GAS_STATUS_GET              0x40
#define GAS_STATUS_RET              0x41
#define GAS_ERROR_GET               0x50
#define GAS_ERROR_RET               0x51

//===============================================================================
// GAS = Genaral API Set / Return- und ERROR-Codes
//===============================================================================
#define GAS_RET_OK	                0x00
#define GAS_RET_ERROR	            0x01
#define GAS_RET_ERROR_CMD	        0x02
#define GAS_RET_ERROR_PARAM	        0x03
#define GAS_RET_STATUS_UDEF	        0x20
#define GAS_RET_STATUS_INIT	        0x21
#define GAS_RET_STATUS_STARTED	    0x22
#define GAS_RET_STATUS_STOPPED	    0x23
#define GAS_RET_STATUS_NETWORK	    0x24
#define GAS_RET_STATUS_ERROR	    0x25

#define GAS_ERROR_NONE              0x00
#define GAS_ERROR_UNKNOWN	        0x01
#define GAS_ERROR_FATAL	            0xFF

//===============================================================================
// Interne Prototypen
//===============================================================================
//===============================================================================
// Hier die entsprechende Task aktiviern
//===============================================================================
static EN_TASK_STATUS Radio_Task_Start( void );         // vom Task-Manager aufgerufen
static EN_TASK_STATUS Radio_Task_Running( void );       // vom Task-Manager aufgerufen

static void Radio_Task_MessDataOut( void );             // von Radio_Task_Running aufgerufen um Messdaten auszugeben

                                                        // alle nachfolgenden Funktionen werden aus der Radio_Task aufgerufen
static uint16_t Radio_Task_Event();
static uint16_t Radio_Task_StartModul();
static uint16_t Radio_Task_BME280_DataOut( void );
static uint16_t Radio_Task_Si1132_DataOut( void );
static uint16_t Radio_Task_Flow_DataOut();

//===============================================================================
//  ab hier interne Prototypen nicht Task-bezogen
//===============================================================================
static void     Radio_Task_Interpret_Frame( uint32_t uiLen );
static uint16_t crc_calc(uint16_t i_init, void *pvData, uint16_t i_len);
//static uint16_t crc_calc_reverse(uint16_t i_init, void *pvData, uint16_t i_len);

//===============================================================================
// Globale Tabelle mit den Task-Funktionen
//===============================================================================
ST_FUNCTION_TABLE gst_Radio_Func_Table[] =
{
   { Radio_Task_Start,   10   },                        // Überwachung Input-Buffer und Initialisierung des Radio-Moduls
   { Radio_Task_Running, 10   },                        // Überwachung Input-Buffer und Senden der LWM2M-Messdaten-Objekte
};

//===============================================================================
// Modulvariable
//===============================================================================

#define FLAG_BME280_DATA_OUT     0x0001                 // gesetzt wenn BME280-Daten ausgegeben werden sollen
#define FLAG_Si1132_DATA_OUT     0x0002                 // gesetzt wenn Si1132-Daten ausgegeben werden sollen
#define FLAG_FLOW_DATA_OUT       0x0004
#define FLAG_LastDATA_OUT_FLG    0x0004                 // immer auf das Letzte Flag der Kette stellen

static uint16_t  muiFlags         = 0;                  // interne Flags
static ST_UART_t *mpUART;

static uint32_t muiGAS_Status    = 0xFF;

ST_BME280_MD        gstBME280_MD = {0} ;                // hier die globalen Messwert-Speicher ablegen
ST_Si1132_MessData  gstSi1132_MD = {0} ;
ST_FLOW_MESS_DATA   gstFlow_MD   = {0} ;

uint8_t mbReInitFunkModul = false;                      // damit das Funk-Modul neu initialisiert werden kann

//===============================================================================
//    Ab hier die Public Funktionen
//===============================================================================

void Radio_Task_Init()
{
   mpUART = UART_Init( RADIO_UART_INDEX, RADIO_UART_BAUDRATE, false);
   mbReInitFunkModul = true;
}

void Radio_SetStatusBit  ( uint8_t bStatus )
{
//   mbStatus |= bStatus;                             // falls es ein algemeines Status-Byte gibt hier den Status eintragen
}

void Radio_ResetStatusBit( uint8_t bStatus )
{
//   mbStatus &= ~bStatus;                            // falls es ein algemeines Status-Byte gibt hier den Status eintragen
}

//===========================================================================================================
//    ab hier die Funktionen die die Messdaten zwischenspeichern und das Senden des
//    entsprechenden LWM2M-Objekts übernehemen
//===========================================================================================================

//===========================================================================================================
//    Hier neue Messdaten entgegennehmen und eine Kopie anfertigen außerdem Erweiterung um Min/Max-Werte
//===========================================================================================================
void Radio_Send_BME280_Data( ST_BME280_MESS_DATA *pstM )
{
   muiFlags |= FLAG_BME280_DATA_OUT;
   memcpy( &gstBME280_MD.stCur , pstM, sizeof(ST_BME280_MESS_DATA) );
   // nun noch min/max bilden
   gstBME280_MD.stMin.i32Temp     = ( gstBME280_MD.stCur.i32Temp     < gstBME280_MD.stMin.i32Temp)     ?
                                      gstBME280_MD.stCur.i32Temp     : gstBME280_MD.stMin.i32Temp      ;
   gstBME280_MD.stMax.i32Temp     = ( gstBME280_MD.stCur.i32Temp     > gstBME280_MD.stMax.i32Temp)     ?
                                      gstBME280_MD.stCur.i32Temp     : gstBME280_MD.stMax.i32Temp      ;
   gstBME280_MD.stMin.u32Pressure = ( gstBME280_MD.stCur.u32Pressure < gstBME280_MD.stMin.u32Pressure) ?
                                      gstBME280_MD.stCur.u32Pressure : gstBME280_MD.stMin.u32Pressure  ;
   gstBME280_MD.stMax.u32Pressure = ( gstBME280_MD.stCur.u32Pressure > gstBME280_MD.stMax.u32Pressure) ?
                                      gstBME280_MD.stCur.u32Pressure : gstBME280_MD.stMax.u32Pressure  ;
   gstBME280_MD.stMin.u32Humity   = ( gstBME280_MD.stCur.u32Humity   < gstBME280_MD.stMin.u32Humity)   ?
                                      gstBME280_MD.stCur.u32Humity   : gstBME280_MD.stMin.u32Humity    ;
   gstBME280_MD.stMax.u32Humity   = ( gstBME280_MD.stCur.u32Humity   > gstBME280_MD.stMax.u32Humity)   ?
                                      gstBME280_MD.stCur.u32Humity   : gstBME280_MD.stMax.u32Humity    ;
}

//===========================================================================================================
//    Min/Max-Werte auf Current zurückstellen
//===========================================================================================================
void Radio_BME280_ResetMinMax(void)
{
   memcpy( &gstBME280_MD.stMin , &gstBME280_MD.stCur, sizeof(gstBME280_MD.stCur) );
   memcpy( &gstBME280_MD.stMax , &gstBME280_MD.stCur, sizeof(gstBME280_MD.stCur) );
}

//===========================================================================================================
//===========================================================================================================
void Radio_Send_Si1132_Data( ST_Si1132_MessData *pstM  )
{
   muiFlags |= FLAG_Si1132_DATA_OUT;
   memcpy( &gstSi1132_MD, pstM, sizeof(ST_Si1132_MessData) );
}

//===========================================================================================================
//===========================================================================================================
void Radio_Send_Flow_Data  ( ST_FLOW_MESS_DATA *pstM   )
{
   muiFlags |= FLAG_FLOW_DATA_OUT;
   memcpy( &gstFlow_MD, pstM, sizeof(ST_FLOW_MESS_DATA) );
}

//===========================================================================================================
//===========================================================================================================
void Radio_Send_EE893_Data ( ST_EE893_MESS_DATA *pstM  )
{
}

//===========================================================================================================
//===========================================================================================================
void Radio_Send_Akku_Data  ( uint8_t bAkkuValue        )
{
}

//===========================================================================================================
//===========================================================================================================
void Radio_ReInitFunkModul(void)
{
   mbReInitFunkModul = true;                          // damit das Funk-Modul neu initialisiert werden kann
}

//===========================================================================================================
//    Sendet einen Record über UART. (Payload muss bereits richtigen ENDIAN haben.
//===========================================================================================================
//    return =0 alles ok, sonst -1 wenn Daten nicht gesendet wurden (FiFo zu klein)
//===========================================================================================================
int32_t Radio_Send_Record( uint8_t bCMD, void *pvData, uint32_t uiLen )
{
   uint16_t uiSpaceLeft = RADIO_TX_BUF_LEN - UART_Get_TXD_Anzahl(mpUART);
   uint16_t uiTotalLen  =  uiLen + 8;                   // SFD +2 len + 2 len invertiert + cmd + 2 crc = 8 Bytes  uiLen
   if ( uiSpaceLeft >= uiTotalLen )
   {                                                    // damit nicht halbe Rekords gesendet werden senden nur beginnen, wenn genügent FiFo vorhanden ist
      uiLen++;                                          // CMD in gesendeder Länge berücksichtgen
      UART_Put_Byte( mpUART, RADIO_SFD );               // header senden
      UART_Put_Byte( mpUART, uiLen >> 8 );
      UART_Put_Byte( mpUART, uiLen );

      UART_Put_Byte( mpUART, (~uiLen) >> 8 );
      UART_Put_Byte( mpUART,  ~uiLen );

      UART_Put_Byte( mpUART, bCMD  );
      uiLen--;

      if (uiLen && pvData)                              // nur wenn Zeiger auf Daten übergeben wurde und die Länge >0 ist
         UART_Put_Record(mpUART, pvData, uiLen );

      uint16_t crc=0;                                   // nun noch das CRC berechnen und senden
      crc = crc_calc( crc, &bCMD, 1 );
      if (uiLen && pvData)
         crc = crc_calc( crc, pvData, uiLen );
      crc = ~crc;
      UART_Put_Byte( mpUART, crc >> 8 );
      UART_Put_Byte( mpUART, crc );
      return 0;
   }
   return -1;
}

//===============================================================================
// ab hier nur noch modulinterne Funktionen
//===============================================================================

static EN_TASK_STATUS Radio_Task_Start( void )
{
   uint16_t iRetValue = Radio_Task_Event();             // erstmal alles was mit Empfang zu tun hat abarbeiten

   iRetValue = Radio_Task_StartModul();                 // hier das Funkmodul initialisieren

   if (iRetValue==0)
      return TASK_STEP1;                                // nachdem das Modul initialisiert ist mit dem 'normalen' Aufgaben weitermachen
   else
      return TASK_STEP0;
}

//=========================================================================================================================================
//  Hier kommen wir alle 10ms hin. Instanze-Write aber max. alle 1Sekunde absetzen = Counter=100
//=========================================================================================================================================
static EN_TASK_STATUS Radio_Task_Running( void )
{
    static uint16_t wCount = 100;                       // bei jedem 100mal auf neue Messdaten prüfen und dann als Instance-Write senden

    if ( mbReInitFunkModul )
    {
        return Radio_Task_Start();                      // Funkmodul neu initialisieren falls notwendig
    }

    Radio_Task_Event();                                 // Dann die Empfangsdaten abarbeiten

    if ( wCount )
    {
        wCount--;
    }
    else
    {
        if ( muiFlags )
        {
            muiFlags = 0;                               // OUT-Flags zurücksetzen
            wCount = 100;                               // nächste Asugabe in 100*10ms = einer Sekunde
            Radio_Task_MessDataOut();
        }
    }
    return TASK_STEP1;
}

////===============================================================================
////===============================================================================
//static EN_TASK_STATUS Radio_Task_Running( void )
//{
//   static uint16_t wStep = 0;
//   static uint16_t wNextOutFlagMask = 0x0001;
//
//   uint16_t wTemp;
//
//   if ( mbReInitFunkModul )
//   {
//      return Radio_Task_Start();
//   }
//
//   uint16_t iRetValue = Radio_Task_Event();             // erstmal alles was mit Empfang zu tun hat abarbeiten
//   switch(wStep)
//   {
//   case 0:                                            // hier prüfen ob es etwas zu senden gibt
//      iRetValue = 1;
//      wTemp = muiFlags & wNextOutFlagMask;
//      if (wTemp & FLAG_BME280_DATA_OUT)
//         wStep = 1;
//      else if (wTemp & FLAG_Si1132_DATA_OUT)
//         wStep = 2;
//      else if (wTemp & FLAG_FLOW_DATA_OUT)
//         wStep = 3;
//      else
//      {
//         wNextOutFlagMask <<= 1;                      // Maske zum nächsten Flag bewegen - dadurch sicherstellen, dass jede Ausgabe einmal dran kommt
//         if (wNextOutFlagMask > FLAG_LastDATA_OUT_FLG)
//            wNextOutFlagMask = 1;                     // wieder beim ersten Flag beginnen
//      }
//      break;
//
//   case 1:
//      iRetValue = Radio_Task_BME280_DataOut();        // BME280-Daten in LWM2M-Objekten ausgeben
//      break;
//
//   case 2:
//      iRetValue = Radio_Task_Si1132_DataOut();        // Si1132-Daten in LWM2M-Objekten ausgeben
//      break;
//
//   case 3:
//      iRetValue = Radio_Task_Flow_DataOut();          // Si1132-Daten in LWM2M-Objekten ausgeben
//      break;
//   default:
//      iRetValue = 0;
//   }
//
//   if (iRetValue==0)
//   {
//      muiFlags &= ~wNextOutFlagMask;                  // Ausgabe beendet -> Flag zurücksetzen
//      wStep = 0;
//   }
//   return TASK_STEP1;
//}

//===============================================================================
//    Hier die Verarbeitung der RX-Daten
//===============================================================================
static uint16_t Radio_Task_Event()
{
   static uint16_t uiStep=0;
   static uint16_t uiLen=0;
   uint16_t uiLenInv;

   uint16_t uiBytesInFiFo = UART_Get_RXD_Anzahl( mpUART );
   uint16_t uiWasEnoughBytes = true;

   while ( uiBytesInFiFo && uiWasEnoughBytes )
   {
      switch (uiStep)
      {
      case 0:                                           // suche SFD
         if ( UART_Get_Byte( mpUART ) == RADIO_SFD)
            uiStep++;
         break;
      case 1:                                           // lese Länge (Big-Endian = High-Byte-first)
         if (uiBytesInFiFo >= 4 )
         {
            uiLen   = UART_Get_Byte( mpUART );
            uiLen <<= 8;
            uiLen  |= UART_Get_Byte( mpUART );

            uiLenInv   = UART_Get_Byte( mpUART );
            uiLenInv <<= 8;
            uiLenInv  |= UART_Get_Byte( mpUART );

            uiLen += 2;                                 // um die 2 CRC-Bytes auch mit zur berücksichtigen
            if (uiLen>128 && uiLen != ~uiLenInv)
               uiStep = 0;                              // unsere Datensätze sind kleiner als 128 -> Datensatz ignoieren
            else
               uiStep++;                                // SFD + Len (2Byte) sind bereits eingelesen
         }
         else
            uiWasEnoughBytes = false;                   // war ein Zeichen zu wenig also nicht genug Zeichen->verlasse schleife
         break;
      case 2:
         if ( uiBytesInFiFo >= uiLen )                  // warte bis Anzahl Bytes im FiFo
         {
            Radio_Task_Interpret_Frame( uiLen );        // Verarbeite diese und sende Antwort
            uiStep=0;
         }
         else
            uiWasEnoughBytes = false;                   // war ein Zeichen zu wenig also nicht genug Zeichen->verlasse schleife
         break;
      }
      uiBytesInFiFo = UART_Get_RXD_Anzahl( mpUART );
   }
   return 0;
}

//===============================================================================
//    LWM2M-Funk-Modul initialisieren
//===============================================================================
static uint16_t Radio_Task_StartModul(void)
{
   uint16_t wRetValue = 1;
   static uint16_t uiStartUpStep = 0;
   static uint16_t uiCounter     = 0;
   switch (uiStartUpStep)
   {
   case 0:
      LED_GE_ON;
      RADIO_ON;
      RADIO_RESET_ON;
      uiStartUpStep++;
      break;
   case 1:
      RADIO_RESET_OFF;
      muiGAS_Status = 0xFF;
      uiStartUpStep++;
      break;

   case 2:
      if (muiGAS_Status == 0x31)                      // warte auf Status 31 (Einschaltmeldung von Funk-Modul)
      {
         muiGAS_Status = 0xFF;
         LWM2M_GAS_DeviceStop();                      // Device Stoppen
         uiStartUpStep++;
      }
      break;

   case 3:
      if (muiGAS_Status == 0x30)                      // warte auf Status 30
      {
         mbReInitFunkModul = false;                   // hier erfolgt die erste Übertragung von Parameter ans Funkmodul -> wenn jetzt etwas an IP / Port /Client-Name
         muiGAS_Status = 0xFF;
         LWM2M_GAS_SetChannel();
         uiStartUpStep++;
//         uiStartUpStep++;                             // nächsten Schritt Setzen der PANID testweise übersprigen
      }
      break;

   case 4:
      if (muiGAS_Status == 0x00)                      // warte auf Status 00
      {
         muiGAS_Status = 0xFF;
         LWM2M_GAS_SetPANID();
         uiStartUpStep++;
      }
      break;

   case 5:
      if (muiGAS_Status == 0x00)                      // warte auf Status 00
      {
         muiGAS_Status = 0xFF;
         LWM2M_GAS_DeviceStart();
         uiStartUpStep++;
      }
      break;

   case 6:
      if (muiGAS_Status == 0x31)                      // warte auf Status 31
      {
         guiLWM2M_Status = 0xFF;                      // geändert wird ist eine erneueter Aufruf der Initialisierung notwendig (Running-Task)
         LWM2M_Stop();
         uiStartUpStep++;
      }
      break;

   case 7:
      if (guiLWM2M_Status == 0x00)                     // warte auf Status 31
      {
         guiLWM2M_Status = 0xFF;                      // geändert wird ist eine erneueter Aufruf der Initialisierung notwendig (Running-Task)
         LWM2M_SET_SRV_IP( false );
         uiStartUpStep++;
      }
      break;

   case 8:
      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
      {
         guiLWM2M_Status = 0xFF;
         LWM2M_SET_SRV_PORT( false );
         uiStartUpStep++;
      }
      break;

   case 9:
      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
      {
         guiLWM2M_Status = 0xFF;
         LWM2M_SET_CLI_NAME( false );
         uiStartUpStep++;
      }
      break;

   case 10:
      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
      {
         LWM2M_startCreateObjects();                  // hier nur die Modul-Variablen (Index/Pointer) richtig setzen
         uiStartUpStep++;
      }                                               // bewusst kein break;
      else
         break;                                       // break nur solange wie auf status 0 gewartet wird

   case 11:
      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
      {
         guiLWM2M_Status = 0xFF;
         uiStartUpStep += LWM2M_CreateNextObject();   // hier ein Object nach dem anderen anlegen bis alle Objecte aktiv sind
      }
      break;

   case 12:
      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
      {
         guiLWM2M_Status = 0xFF;
         uiStartUpStep+=LWM2M_CreateNextResource();   // wird eine Resource nach der anderen für das aktuelle Objekt angelegt;
      }
      break;

   case 13:
      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
      {
         guiLWM2M_Status = 0xFF;
         LWM2M_Start();                               // LWM2M-Level sarten
         uiStartUpStep++;
      }
      break;
   case 14:
      if (guiLWM2M_Status ==0x32)                     // warte auf Status 32
      {
         guiLWM2M_Status = 0x00;                      // Damit erste LEM2M-Resource initialisiert wird
         uiStartUpStep++;
         uiCounter = 10;
      }
      break;

   case 15:
      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
      {
         if (uiCounter-- == 0)
         {
            // alle resourcen einmalig initialisieren ?!?
            guiLWM2M_Status = 0xFF;
            uiStartUpStep+=LWM2M_InitNextResource();
            uiCounter = 10;
         }
      }
      break;

   case 16:
      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
      {
         LED_GE_OFF;
         wRetValue = 0;                               // nun sind wir fertig -> 0 zurückgeben
         uiStartUpStep = 0;
      }
      break;

   default:
     uiStartUpStep=0;
   }

   return wRetValue;                                  // wenn wir noch nicht fertig sind 1 zurückgeben sonst 0
}

//===========================================================================================================
//  Instance-Write nur noch eine Ausgabe für alle Resourcen
//===========================================================================================================
static void Radio_Task_MessDataOut()
{
    static const uint16_t ui_RES_IDs[] =
    {
        LWM2M_MESS_DATA_VIS_DATA_RES_ID,
        LWM2M_MESS_DATA_TEMP_DATA_RES_ID,
        LWM2M_MESS_DATA_HUMITY_DATA_RES_ID,
        LWM2M_MESS_DATA_PRESS_DATA_RES_ID,
        LWM2M_MESS_DATA_FLOW_X_DATA_RES_ID,
        LWM2M_MESS_DATA_FLOW_Y_DATA_RES_ID
    };

    uint8_t   bBuf[64];                                 // 6 Resourcen*7-Bytes  + 5 Header-Bytes = min 47 Bytes

    int16_t iRetVal = LWM2M_BuildInstanceWR_UartBuf(bBuf, sizeof(bBuf), true, LWM2M_MESS_DATA_OBJ_ID, 0, ui_RES_IDs, 6 );
    Radio_Send_Record( LWM2M_LWM2M_CMD, bBuf, iRetVal );
}

//===========================================================================================================
//    Task-Step6: BME280 Objektdatenausgabe
//===========================================================================================================
static uint16_t Radio_Task_BME280_DataOut( void )
{
   static uint16_t uiStep = 0;
   static  int16_t  iTimeOutCounter = RADIO_RESPONSE_TIME_OUT;

   uint8_t   bBuf[15];                                // jeweils nur eine Objekt-Resource setzen
   uint8_t *pbBuf = bBuf;

   *pbBuf++ = LWM2M_WR_REQ;
   int16_t iBufLen = sizeof(bBuf) -1;

   int16_t iRetVal=-1;

   switch(uiStep)
   {
   case 0:                                            // im ersten Step ist Funk-Status egal deshalb in allen andsern steps auf Status prüfen
      LED_GE_ON;
      iRetVal = LWM2M_BuildObjektUartBuf( pbBuf, iBufLen, BME280_TEMP_OBJ_ID, 0, LWM2M_CUR_SENS_VALUE );
      break;
   case 1:
      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
         iRetVal = LWM2M_BuildObjektUartBuf( pbBuf, iBufLen, BME280_TEMP_OBJ_ID, 0, LWM2M_MIN_SENS_VALUE );
      break;
   case 2:
      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
         iRetVal = LWM2M_BuildObjektUartBuf( pbBuf, iBufLen, BME280_TEMP_OBJ_ID, 0, LWM2M_MAX_SENS_VALUE );
      break;
   case 3:
      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
         iRetVal = LWM2M_BuildObjektUartBuf( pbBuf, iBufLen, BME280_HUMITY_OBJ_ID, 0, LWM2M_CUR_SENS_VALUE );
      break;
   case 4:
      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
         iRetVal = LWM2M_BuildObjektUartBuf( pbBuf, iBufLen, BME280_HUMITY_OBJ_ID, 0, LWM2M_MIN_SENS_VALUE );
      break;
   case 5:
      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
         iRetVal = LWM2M_BuildObjektUartBuf( pbBuf, iBufLen, BME280_HUMITY_OBJ_ID, 0, LWM2M_MAX_SENS_VALUE );
      break;
   case 6:
      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
         iRetVal = LWM2M_BuildObjektUartBuf( pbBuf, iBufLen, BME280_PRESSURE_OBJ_ID, 0, LWM2M_CUR_SENS_VALUE );
      break;
   case 7:
      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
         iRetVal = LWM2M_BuildObjektUartBuf( pbBuf, iBufLen, BME280_PRESSURE_OBJ_ID, 0, LWM2M_MIN_SENS_VALUE );
      break;
   case 8:
      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
         iRetVal = LWM2M_BuildObjektUartBuf( pbBuf, iBufLen, BME280_PRESSURE_OBJ_ID, 0, LWM2M_MAX_SENS_VALUE );
      break;
   case 9:
//      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
//         iRetVal = LWM2M_BuildObjektUartBuf( pbBuf, iBufLen, BME280_INI_OBJ_ID, 0, LWM2M_STATUS_RES_ID );
//      break;
   case 10:
      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
         uiStep = 0xFF;                               // Ausgabe-Task nun beendet
      break;
   }

   if (iRetVal>0)
   {                                                  // es wurde etwas in Out-Buffer gelegt
      guiLWM2M_Status  = 0xFFFF;                      // also Status erstmal ungültig machen Status ist (uit8_t) auf Funk-Seite
      iTimeOutCounter  = RADIO_RESPONSE_TIME_OUT;      // Anzahl der Response-Warte-Steps setzen
                                                      // Daten nun abschicken
      Radio_Send_Record( LWM2M_LWM2M_CMD, bBuf, iRetVal+1 );
      uiStep++;                                       // weiter mit nächsten step
   }
   else if (uiStep == 0xFF          ||                               // Letzter Step erreicht               -> Senden beenden
             iTimeOutCounter-- <  0 ||                               // Warte-Zeit aus Status abgelaufen    -> Senden beenden
           (guiLWM2M_Status !=0xFFFF && guiLWM2M_Status !=0x00) )    // Falscher Funk-Status erkannt        -> Senden beenden
   {                                                  // Blaue LED wieder ausschalten
      LED_GE_OFF;
      uiStep = 0;
      return 0;
   }
   return 1;
}

//===========================================================================================================
//    Si1132 Objektdatenausgabe
//===========================================================================================================
static uint16_t Radio_Task_Si1132_DataOut( void )
{
   static uint16_t uiStep = 0;
   static  int16_t uiTimeOutCounter = RADIO_RESPONSE_TIME_OUT;

   uint8_t   bBuf[15];                                // jeweils nur eine Objekt-Resource setzen
   uint8_t *pbBuf = bBuf;

   *pbBuf++ = LWM2M_WR_REQ;
   int32_t iBufLen = sizeof(bBuf) -1;

   int32_t iRetVal=-1;
   switch(uiStep)
   {
   case 0:
      LED_GE_ON;
      iRetVal = LWM2M_BuildObjektUartBuf( pbBuf, iBufLen, Si1132_VIS_OBJ_ID, 0, LWM2M_CUR_SENS_VALUE );
      break;
   case 1:
      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
         iRetVal = LWM2M_BuildObjektUartBuf( pbBuf, iBufLen, Si1132_VIS_OBJ_ID, 0, LWM2M_MIN_SENS_VALUE );
      break;
   case 2:
      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
         iRetVal = LWM2M_BuildObjektUartBuf( pbBuf, iBufLen, Si1132_VIS_OBJ_ID, 0, LWM2M_MAX_SENS_VALUE );
      break;
   case 3:
//      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
//         iRetVal = LWM2M_BuildObjektUartBuf( pbBuf, iBufLen, Si1132_IR_OBJ_ID, 0, LWM2M_CUR_SENS_VALUE );
//      break;
//   case 4:
//      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
//         iRetVal = LWM2M_BuildObjektUartBuf( pbBuf, iBufLen, Si1132_IR_OBJ_ID, 0, LWM2M_MIN_SENS_VALUE );
//      break;
//   case 5:
//      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
//         iRetVal = LWM2M_BuildObjektUartBuf( pbBuf, iBufLen, Si1132_IR_OBJ_ID, 0, LWM2M_MAX_SENS_VALUE );
//      break;
//   case 6:
//      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
//         iRetVal = LWM2M_BuildObjektUartBuf( pbBuf, iBufLen, Si1132_UV_OBJ_ID, 0, LWM2M_CUR_SENS_VALUE );
//      break;
//   case 7:
//      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
//         iRetVal = LWM2M_BuildObjektUartBuf( pbBuf, iBufLen, Si1132_UV_OBJ_ID, 0, LWM2M_MIN_SENS_VALUE );
//      break;
//   case 8:
//      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
//         iRetVal = LWM2M_BuildObjektUartBuf( pbBuf, iBufLen, Si1132_UV_OBJ_ID, 0, LWM2M_MAX_SENS_VALUE );
//      break;
//   case 9:
//      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
//         iRetVal = LWM2M_BuildObjektUartBuf( pbBuf, iBufLen, Si1132_INI_OBJ_ID, 0, LWM2M_STATUS_RES_ID );
//      break;
   case 10:
      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
         uiStep = 0xFF;                               // Ausgabe-Task nun beendet
      break;
   }

   if (iRetVal>0)
   {                                                  // es wurde etwas in Out-Buffer gelegt
      guiLWM2M_Status  = 0xFFFF;                      // also Status erstmal ungültig machen Status ist (uit8_t) auf Funk-Seite
      uiTimeOutCounter = RADIO_RESPONSE_TIME_OUT;      // Anzahl der Response-Warte-Steps setzen
                                                      // Daten nun abschicken
      Radio_Send_Record( LWM2M_LWM2M_CMD, bBuf, iRetVal+1 );
      uiStep++;                                       // weiter mit nächsten step
   }
   else if (uiStep == 0xFF         ||                               // Letzter Step erreicht               -> Senden beenden
            uiTimeOutCounter-- < 0 ||                               // Warte-Zeit aus Status abgelaufen    -> Senden beenden
           (guiLWM2M_Status !=0xFFFF && guiLWM2M_Status !=0x00 ) )  // Falscher Funk-Status erkannt        -> Senden beenden
   {                                                  // Blaue LED wieder ausschalten
      uiStep = 0;
      LED_GE_OFF;
      return 0;
   }
   return 1;
}


//===========================================================================================================
//    Flow Objektdatenausgabe
//===========================================================================================================
static uint16_t Radio_Task_Flow_DataOut()
{
      static uint16_t uiStep = 0;
   static  int16_t uiTimeOutCounter = RADIO_RESPONSE_TIME_OUT;

   uint8_t   bBuf[15];                                // jeweils nur eine Objekt-Resource setzen
   uint8_t *pbBuf = bBuf;

   *pbBuf++ = LWM2M_WR_REQ;
   int32_t iBufLen = sizeof(bBuf) -1;

   int32_t iRetVal=-1;
   switch(uiStep)
   {
   case 0:
      LED_GE_ON;
      iRetVal = LWM2M_BuildObjektUartBuf( pbBuf, iBufLen, FLOW_X_OBJ_ID, 0, LWM2M_CUR_SENS_VALUE );
      break;
   case 1:
      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
         iRetVal = LWM2M_BuildObjektUartBuf( pbBuf, iBufLen, FLOW_X_OBJ_ID, 0, LWM2M_MIN_SENS_VALUE );
      break;
   case 2:
      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
         iRetVal = LWM2M_BuildObjektUartBuf( pbBuf, iBufLen, FLOW_X_OBJ_ID, 0, LWM2M_MAX_SENS_VALUE );
      break;
   case 3:
      iRetVal = LWM2M_BuildObjektUartBuf( pbBuf, iBufLen, FLOW_Y_OBJ_ID, 0, LWM2M_CUR_SENS_VALUE );
      break;
   case 4:
      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
         iRetVal = LWM2M_BuildObjektUartBuf( pbBuf, iBufLen, FLOW_Y_OBJ_ID, 0, LWM2M_MIN_SENS_VALUE );
      break;
   case 5:
      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
         iRetVal = LWM2M_BuildObjektUartBuf( pbBuf, iBufLen, FLOW_Y_OBJ_ID, 0, LWM2M_MAX_SENS_VALUE );
      break;
   case 6:
      if (guiLWM2M_Status ==0x00)                     // warte auf Status 00
         uiStep = 0xFF;                               // Ausgabe-Task nun beendet
      break;
   }

   if (iRetVal>0)
   {                                                  // es wurde etwas in Out-Buffer gelegt
      guiLWM2M_Status  = 0xFFFF;                      // also Status erstmal ungültig machen Status ist (uit8_t) auf Funk-Seite
      uiTimeOutCounter = RADIO_RESPONSE_TIME_OUT;     // Anzahl der Response-Warte-Steps setzen
                                                      // Daten nun abschicken
      Radio_Send_Record( LWM2M_LWM2M_CMD, bBuf, iRetVal+1 );
      uiStep++;                                       // weiter mit nächsten step
   }
   else if (uiStep == 0xFF         ||                               // Letzter Step erreicht               -> Senden beenden
            uiTimeOutCounter-- < 0 ||                               // Warte-Zeit aus Status abgelaufen    -> Senden beenden
           (guiLWM2M_Status !=0xFFFF && guiLWM2M_Status !=0x00 ) )  // Falscher Funk-Status erkannt        -> Senden beenden
   {                                                  // Blaue LED wieder ausschalten
      uiStep = 0;
      LED_GE_OFF;
      return 0;
   }
   return 1;
}

//===========================================================================================================
// Interpretiere die CFG-Response auf unsere Anfrage und stelle evtl.  die nächste Anfrage
//===========================================================================================================
void GAS_InterpretCFG_RSP(uint8_t *pbBuf, uint32_t uiLen)
{
   switch(pbBuf[1])
   {
   case GAS_CFG_ID_MACADDR:
      memcpy( gSYS.stCC1350_SET.bMAC_ADR, &pbBuf[2], sizeof(gSYS.stCC1350_SET.bMAC_ADR));
      break;
   case GAS_CFG_ID_PAN:
      gSYS.stCC1350_SET.wPANID  = pbBuf[2];
      gSYS.stCC1350_SET.wPANID <<= 8;
      gSYS.stCC1350_SET.wPANID |= pbBuf[3];
      break;
   case GAS_CFG_ID_OPMODE:
      gSYS.stCC1350_SET.bOPMODE  = pbBuf[2];
      break;
   case GAS_CFG_ID_CHANNEL:
      gSYS.stCC1350_SET.bCHANNEL = pbBuf[2];
      break;
   }
}

//===========================================================================================================
//    CRC-Berechnung für normale Daten (little Endian)
//===========================================================================================================
/*! x^16 + x^13 + x^12 + x^11 + x^10 + x^8 + x^6 + x^5 + x^2 + 1
 required when manually computing */
#define POLYNOMAL                     0x3D65U
static uint16_t crc_calc(uint16_t i_init, void *pvData, uint16_t i_len)
{
  /* Temporary variables */
  uint32_t i, j, c;
  uint8_t *pcData = (uint8_t*) pvData;

  /* Stores the current calculated crc value */
  uint32_t i_crc;
  i_crc = i_init;

  for(i = i_len; i; i--)
  {
    c = *pcData++;

    c <<= 8U;
    i_crc ^= c;

    for(j = 0U;j < 8U;j++)
    {
      if(i_crc & 0x8000U)
        i_crc = (i_crc << 1U) ^ POLYNOMAL;
      else
         i_crc <<= 1U;
    }
    i_crc &= 0xFFFFU;
  }
  return (uint16_t) i_crc;
}

//===============================================================================
// Verarbeite diese (Sende Antwort)
//===============================================================================
void Radio_Task_Interpret_Frame( uint32_t uiLen )
{
   uint8_t bBuf[ RADIO_MAX_OBJECT_SIZE ];

   if (uiLen > RADIO_MAX_OBJECT_SIZE)
      uiLen = RADIO_MAX_OBJECT_SIZE;                  // längenbegrenzung um Stack-Überlauf zu verhindern

   // bei big Endian:
   UART_Get_Record( mpUART, bBuf, uiLen);             // CMD+Payload+CRC

   uint16_t uiCRC_ist=crc_calc( 0, bBuf, uiLen-2 ) ^0xFFFF;

   uint16_t uiCRC_soll = bBuf[uiLen-2];               // soll laut Datensatz Big Endian [uiLen-1], big Endian [uiLen-2]
   uiCRC_soll <<= 8;
   uiCRC_soll  |= bBuf[uiLen-1];

   uiLen -= 2;                                        // CRC nicht mehr in uiLen führen

//   Testweise auskommentiert um mit H-Term die CMD's zu testen ohne sich ums CRC zu kümmern
   if (uiCRC_soll == uiCRC_ist)
   {                                                  // Daten wurden alle empfangen crc ist geprüft nun Daten verarbeiten
      switch(bBuf[0])
      {
      case LWM2M_GAS_RET:                             // Status-Ret
          muiGAS_Status = bBuf[1];                  // der GAS-Status ist uns egal
         break;
      case GAS_PING:                                  // Antort auf PING ist ein Ping
         Radio_Send_Record( GAS_PING, 0, 0 );
         break;
      case LWM2M_GAS_CFG_SET:
         LWM2M_GAS_Interpret_CFG_SET( &bBuf[1], uiLen-1);
         break;
      case LWM2M_GAS_CFG_GET:
         LWM2M_GAS_Interpret_CFG_GET( &bBuf[1], uiLen-1);
         break;

      case GAS_STATUS_RET:                            // Status-Ret
         muiGAS_Status = bBuf[1];                   // der GAS-Status ist uns egal
         break;
      case GAS_ERROR_RET:                             // Error-Ret
         break;
      case LWM2M_LWM2M_CMD:
         LWM2M_InterpretCMD( &bBuf[1], uiLen-1 );
         break;
      //case LWM2M_WR_RSP:
      //   break;
      }
   }
}