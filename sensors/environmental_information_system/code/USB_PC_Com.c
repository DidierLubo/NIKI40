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
//************************************************************************************************************************
//*  USB_PC_com                                                                                                          *
//*  Erstellt am 27.02.2013                                                                                              *
//************************************************************************************************************************
//*  Funktion des Programms                                                                                              *
//*  Übernimmt die Kommunikation mit dem PC                                                                              *
//*  Hier werden die PC-Kommandos emfangen und ausgewertet                                                               *
//************************************************************************************************************************

#include "Hardware.h"
#include "Globals.h"

#include "USB_PC_Com.h"

#include "Radio_Task.h"
#include "IPv6.h"

#include "OLED.h"

#include "BQ27421-G1.h"

#include "Task_Manager.h"

#include "HAL_UCS.h"

#include "main.h"

#include <String.h>
#include <stdio.h>

#include "USB_Common\usb.h"                 //USB-specific functions
#include "USB\usbConstructs.h"
#include "USB_Config\descriptors.h"
#include "USB_CDC_API\UsbCdc.h"

#define MAX_CMD_LEN                    30
#define MAX_LINE_LEN                   64


static const BYTE mbDone[]            = "Done\r\n";
static const BYTE mbParError[]        = "Parameter Fehler\r\n";
static const BYTE mbCMD_UNKNOWN[]     = "CMD unknown\r\n";

WORD mwRecCounter = 0;

BYTE mbCO2_AverageLength  = 1;
WORD mwCO2_HeizerFrequenz = 120;

volatile BYTE gbCDCDataReceived_event = false;        // Flag set by event handler to indicate data has been
                                                      // received into USB buffer

volatile BYTE gbUSB_Out_Flg     = 0;                  // Flag um Sensor-Daten über USB zu senden

volatile uint8_t gbRadioStreamingMode;                // true bei streaming sonst false

//------------------------------------------------------------------------------------------------------------------------
// Holt die Zeichen vom USB-Port ab und läd diese in den übergebenen Buffer.
// Falls auf die Daten gewartet werden soll bWaitForData=true
//------------------------------------------------------------------------------------------------------------------------
BYTE USB_ReadBytes(void *pDataBuffer, BYTE bSize, BYTE bWaitForData)
{
   BYTE bByteCounter = 0;

   if (USB_connectionState() == ST_ENUM_ACTIVE)
   {
      gbCDCDataReceived_event = false;
      BYTE *pbWritePos = pDataBuffer;                    // Schreibposition
      do
      {
         BYTE bRX_Count = bByteCounter += cdcReceiveDataInBuffer (
            pbWritePos,
            bSize - bByteCounter,
            CDC0_INTFNUM);
         bByteCounter += bRX_Count;
         pbWritePos   += bRX_Count;
      }  while (bByteCounter < bSize && bWaitForData);
   }
   return bByteCounter;
}

//------------------------------------------------------------------------------------------------------------------------
// USB_ReadInt32: Liest ASCII-Zeichen von USB und wandelt diese in ein Int32 um
//------------------------------------------------------------------------------------------------------------------------
int32_t USB_ReadInt32( void )
{
   BYTE     bData;
   BYTE     bSgnFlg   = 0;
   int32_t  iRetValue = 0;

   do
   {  USB_ReadBytes(&bData, 1, true);
      if (bData>='0' && bData<='9')
      {
         iRetValue = (iRetValue * 10) + (bData - '0');
      }
      else if (bData == '-')
      { // Vorzeichen nur vor der Zahl zulässig
         if (iRetValue == 0)
            bSgnFlg = 1;
         else
            bData = 0;
      }
      else if (bData == '+')
      {  // Vorzeichen nur vor der Zahl zulässig
         if (iRetValue != 0)
            bData = 0;
      }
      else if (bData!=' ' || iRetValue > 0)
      {  // Leer-Zeichen vor Zahl sind erlaubt
         // Ende der Auswertung wenn nicht Numerisch
         bData = 0;
      }
   }  while (bData);
   return bSgnFlg ? (- iRetValue) : (iRetValue);
}

//------------------------------------------------------------------------------------------------------------------------
// USB_ReadFloat: Liest ASCII-Zeichen von USB und wandelt diese in eine float-Zahl um
//------------------------------------------------------------------------------------------------------------------------
float USB_ReadFloat( void )
{
   BYTE bBuf[15];                                     // ASCII-Buffer: max 15 Zeichen für eine Float-Zahl vorsehen
   BYTE bIndex = 0;
   BYTE bisFloatChar;
   BYTE bData;                                        // hier die USB-Daten empfangen
   float fRetValue = 0.0f;

   do
      USB_ReadBytes(&bData, 1, true);
   while (bData == ' ');                              // alle Vorlauf-Spaces ignoieren

   do
   {
      bisFloatChar = (    bData == '+' || bData == '-' || bData == 'e' || bData == 'E' || bData == '.' ||
                        ( bData >= '0' && bData <= '9' ) );
      if (bisFloatChar)
      {
         bBuf[bIndex++]  = bData;                     // Zeichen in ASCII-Buffer umbernehmen und Index erhöhen
         USB_ReadBytes(&bData, 1, true);              // nächstes Zeichen einlesen
      }
   } while (bisFloatChar && bIndex < sizeof(bBuf)-1);

   if (bIndex>0)
   {
      bBuf[ bIndex ] = 0;
      sscanf((const char*) bBuf, "%f", &fRetValue);   // ASCII-Buffer nun in float umwandeln
   }
   return fRetValue;
}

//------------------------------------------------------------------------------------------------------------------------
// USB_ReadLine: Liest ASCII-Zeichen von USB bis ein CR-Zeichen gefunden wird ein oder der übergebene Buffer voll ist
//------------------------------------------------------------------------------------------------------------------------
uint16_t USB_ReadLine( uint8_t *pbBuffer, uint16_t wBufLen )
{
   uint16_t wRetLen = 0;
   if (wBufLen)
   {
      wBufLen --;                                     // Zeichen für 0-Byte schon mal abziehen

      USB_ReadBytes(pbBuffer, 1, true);
      while (wBufLen && *pbBuffer != '\r')            // alles bis CR einlesen (solange bis wLen=0)
      {
         pbBuffer++;                                  // Zeiger auf nächste Adresse
         wRetLen++;                                   // Anzahl der Zeichen im Buffer erhöhen
         wBufLen--;                                   // Anzahl der max. noch einzulesenden Zeichen erniedrigen
         USB_ReadBytes(pbBuffer, 1, true);
      }
      *pbBuffer = 0;                                  // 0-Byte für String-Ende anfügen
   }
   return wRetLen;                                    // Anzahl der Eingelesenen Zeichen (ohne 0-Byte) zurückgeben
}

//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
void USB_Parse_IPv6( uint8_t *pbIPv6, uint16_t *pwPort )
{
   char bBuffer[70];
   USB_ReadLine( (uint8_t*)bBuffer, sizeof(bBuffer));
   IPv6_ParseString( bBuffer, pbIPv6, pwPort ) ;
}


//------------------------------------------------------------------------------------------------------------------------
// USB_Send_int32_ASCII_Data: gibt eine int32-Zahl mit Beschreibung und Einheit als ASCII-String aus
//------------------------------------------------------------------------------------------------------------------------
void USB_Send_int32_ASCII_Data(BYTE *pbDiscription, int32_t value, BYTE *pbUnit)
{
   BYTE bBuffer[80];
   int iLen = snprintf((char*) bBuffer, sizeof bBuffer,  "%s;%li;%s\r\n",
                       pbDiscription,
                       value,
                       pbUnit );

   cdcSendDataWaitTilDone( bBuffer, iLen, CDC0_INTFNUM, 1000);
}

//------------------------------------------------------------------------------------------------------------------------
// USB_Send_float_ASCII_Data: gibt eine float-Zahl mit Beschreibung und Einheit als ASCII-String aus
//------------------------------------------------------------------------------------------------------------------------
void USB_Send_float_ASCII_Data(BYTE *pbDiscription, float value, BYTE *pbUnit)
{
   BYTE bBuffer[80];
   int iLen = snprintf((char*) bBuffer, sizeof bBuffer,  "%s;%.3f;%s\r\n",
                       pbDiscription,
                       value,
                       pbUnit );

   cdcSendDataWaitTilDone( bBuffer, iLen, CDC0_INTFNUM, 1000);
}

//------------------------------------------------------------------------------------------------------------------------
// USB_Send_String: gibt einen 0-terminierten String aus
//------------------------------------------------------------------------------------------------------------------------
void USB_Send_String( BYTE *pbDiscription, BYTE *pbStr )
{
   BYTE bBuffer[100];
   int iLen = snprintf((char*) bBuffer, sizeof bBuffer,  "%s;%s\r\n", pbDiscription, pbStr);
   cdcSendDataWaitTilDone( bBuffer, iLen, CDC0_INTFNUM, 1000);
}

//------------------------------------------------------------------------------------------------------------------------
//    Ausgabe der IPv6-Adresse
//------------------------------------------------------------------------------------------------------------------------
void  USB_Send_IPv6( char *pcDescription, uint8_t *pbIPv6, uint16_t wPort)
{
   char bBuffer[70];
   uint16_t wLen = IPv6_toString( bBuffer, sizeof(bBuffer), pcDescription, pbIPv6, wPort);
   bBuffer[wLen++] = '\r';
   bBuffer[wLen++] = '\n';
   cdcSendDataWaitTilDone( (uint8_t*) bBuffer, wLen, CDC0_INTFNUM, 1000);
}

//------------------------------------------------------------------------------------------------------------------------
// USB_Send_FunkID: sendet die Funk-ID (Mac)
//------------------------------------------------------------------------------------------------------------------------
void USB_SendFunkID(void)
{
   BYTE bBuffer[80];
//   Radio_Get_FunkID( gSYS.bRadioID );                 // lese die aktuell im Funk-Modul gespeicherte Funk-ID aus
   int iLen = snprintf((char*) bBuffer, sizeof bBuffer,  "Funk ID: %02hhX:%02hhX:%02hhX:%02hhX\r\n",
                        gSYS.bRadioID[0], gSYS.bRadioID[1], gSYS.bRadioID[2], gSYS.bRadioID[3] );
   cdcSendDataWaitTilDone( bBuffer, iLen, CDC0_INTFNUM, 1000);
}

//------------------------------------------------------------------------------------------------------------------------
// USB_SetFunkID:   Setzt die Funk-ID auf die in HEX (xx:xx:xx:xx) angegebene MAC-Adresse
//------------------------------------------------------------------------------------------------------------------------
void USB_SetFunkID(void)
{
   BYTE bBuffer[80];
   USB_ReadLine(bBuffer, sizeof(bBuffer));

   int iCount = sscanf((char*) bBuffer, "%02hhX:%02hhX:%02hhX:%02hhX",
                       &gSYS.bRadioID[0], &gSYS.bRadioID[1], &gSYS.bRadioID[2], &gSYS.bRadioID[3] );
   if (iCount==4)
   {
      //Radio_Set_FunkID( gSYS.bRadioID );
      //Radio_Init();
      cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
   }
   else
      cdcSendDataWaitTilDone( (BYTE*) mbParError, sizeof(mbParError) -1, CDC0_INTFNUM, 1000);
}

int USB_FindNextFieldIndex(BYTE *pbBuffer, int iIndex, int bSize)
{
   while(iIndex<bSize && pbBuffer[iIndex] && pbBuffer[iIndex] != ';')
      iIndex++;

   if (pbBuffer[iIndex] == ';')
      iIndex++;                                       // nächster Feldanfung liegt um ein Zeichen weiter

   return iIndex;
}

//------------------------------------------------------------------------------------------------------------------------
// USB_Read_TableData:  Liest die Look-Up-Table-Daten ein und schreibt die Daten ins Flash
//------------------------------------------------------------------------------------------------------------------------
// pFunktion:           Zeiger auf die Setz-Funktion ( z.B. Flow_LoadLookUpTable)
// wMaxIndex:           Tabllengröße (z.B. FLOW_LOOK_UP_SIZE = 1024)
// bisX_Value:          true = mciFlow_X  sonst  mciFlow_Y
//------------------------------------------------------------------------------------------------------------------------
// lfx 64;32;1200;1200;1200;1200;1200;1200;1200;1200;1200;1200;1200;1200;1200;1200;1200;1200;1200;1200;1200;1200;1200;1200;1200;1200;1200;1200;1200;1200;1200;1200;1200;1200;
//------------------------------------------------------------------------------------------------------------------------
void USB_SetTableData( void (*pFunktion)(uint16_t iIndex, int16_t *bDaten, uint16_t bLen, uint8_t isX_Value), WORD wMaxIndex, uint8_t bisX_Value)
{
   BYTE bBuffer[256];
   int  iIndex = 0;
   int  iReadDataCounter = 0;
   int  iDaten[32];                                    // max. 32 Daten aus der Lookuptabelle
   WORD wIndex  = 0xFFFF;
   WORD wAnzahl = 0xFFFF;

   USB_ReadLine(bBuffer, sizeof(bBuffer));            // ganze Zeile einlesen

   sscanf((char*) &bBuffer[iIndex], "%hi;", &wIndex); // als erstes kommt der Index
   iIndex = USB_FindNextFieldIndex(bBuffer, iIndex, sizeof(bBuffer));
   sscanf((char*) &bBuffer[iIndex], "%hi;", &wAnzahl);// jetzt die Anzahl
   iIndex = USB_FindNextFieldIndex(bBuffer, iIndex, sizeof(bBuffer));

   for(WORD i=0; i<wAnzahl && iIndex>0; i++)
   {
      iReadDataCounter += sscanf((char*) &bBuffer[iIndex], "%hi;", &iDaten[i]); // jetzt die Daten
      iIndex = USB_FindNextFieldIndex(bBuffer, iIndex, sizeof(bBuffer));
   }

   if (wIndex+wAnzahl <= wMaxIndex && wAnzahl<=32 && iReadDataCounter==wAnzahl)
   {
      (*pFunktion)(wIndex, iDaten, wAnzahl, bisX_Value);          // Funktion zum Speichern aufrufen
      cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
   }
   else
      cdcSendDataWaitTilDone( (BYTE*) mbParError, sizeof(mbParError) -1, CDC0_INTFNUM, 1000);
}

void USB_Send_CO2_ASCII(void)
{

//   ST_DA_VALUES_t      *pstDA        = GetCO2_Sensor_DA_Values();
//   ST_RAW_AD_VALUES_t  *pstAD        = GetCO2_Sensor_AD_Values();
//   ST_RAW_CO2_TIMING_t *pstRawTiming = GetCO2_SensorTiming();
//
//   ST_RAW_AD_VALUES_t  *pstRec = AD_GetRawData();
//
//
//   BYTE bBuffer[180];
//
//   int iLen = snprintf((char*) bBuffer, sizeof bBuffer,  "C%u;%u ; %u;%u; %u;%u;%u ; %u;%u;%u ; %u;%u;%u; %hhu; %u;%u; %u;%u;%u;%u\r\n",
//      pstRawTiming->S1_rising_Edge ,
//      pstRawTiming->S1_falling_Edge ,
//
//      pstRawTiming->S2_rising_Edge ,
//      pstRawTiming->S2_falling_Edge ,
//
//      pstDA->iDA0, pstDA->iDA1, pstDA->iDA2,
//      pstAD->iCO2_S1_max,  pstAD->iCO2_S1_0, pstAD->iCO2_S1_min,
//      pstAD->iCO2_S2_max,  pstAD->iCO2_S2_0, pstAD->iCO2_S2_min,
//      GetCO2_SensorStatus(),
//      pstRawTiming->S1_AD_Time,                       // sollte in etwa gleich sein mit S1_rising_Edge
//      pstRawTiming->S2_AD_Time,
//
//      stRec.iDruck,
//      stRec.iTemp,
//      pstRec->iFlow_TP1_P1,
//      pstRec->iFlow_TP2_P1
//   );
//   cdcSendDataWaitTilDone( bBuffer, iLen, CDC0_INTFNUM, 1000);
}


//------------------------------------------------------------------------------------------------------------------------
//    Ausgabe der Druck/Feuchte/Temp-Sensor-Daten (BME280)
//------------------------------------------------------------------------------------------------------------------------
void  USB_Send_BME280_Data( ST_BME280_MESS_DATA *pstM )
{
   if (USB_connectionState() == ST_ENUM_ACTIVE && (gbUSB_Out_Flg & (USB_OF_BME280_PTH | USB_OF_SEND_DATA) ) == (USB_OF_BME280_PTH | USB_OF_SEND_DATA) )
   {
      BYTE bBuffer[120];
      int iLen = snprintf((char*) bBuffer, sizeof bBuffer,  "BME280: %lu;%li;%lu; %f; %f\r\n",
                          pstM->u32Pressure,
                          pstM->i32Temp,
                          pstM->u32Humity,
                          ((float) pstM->i32Temp * 0.01f ),
                          ((float) pstM->u32Humity / 1024.0f));
      cdcSendDataWaitTilDone( bBuffer, iLen, CDC0_INTFNUM, 1000);
   }
}

//------------------------------------------------------------------------------------------------------------------------
//    Ausgabe der Licht-Sensor-Daten (Si1132)
//------------------------------------------------------------------------------------------------------------------------
void USB_Send_Si1132_Data( ST_Si1132_MessData *pstM )
{
   if (USB_connectionState() == ST_ENUM_ACTIVE && (gbUSB_Out_Flg & (USB_OF_Si1132 | USB_OF_SEND_DATA) ) == (USB_OF_Si1132 | USB_OF_SEND_DATA) )
   {
      BYTE bBuffer[120];
      int iLen = snprintf((char*) bBuffer, sizeof bBuffer,  "Light: %hu;%hi;%hu ; %hu;%hi;%hu ; %hu;%hi;%hu\r\n",
                          pstM->wAvgVIS,
                          pstM->iAvgIR,
                          pstM->wAvgUV_Index,

                          pstM->wMinVIS,
                          pstM->iMinIR,
                          pstM->wMinUV_Index,

                          pstM->wMaxVIS,
                          pstM->iMaxIR,
                          pstM->wMaxUV_Index     );
      cdcSendDataWaitTilDone( bBuffer, iLen, CDC0_INTFNUM, 1000);
   }
}

//------------------------------------------------------------------------------------------------------------------------
//    Ausgabe der (2D-) Flow-Sensor-Daten
//------------------------------------------------------------------------------------------------------------------------
void USB_Send_Flow_Data( ST_FLOW_MESS_DATA *pstM )
{
   if (USB_connectionState() == ST_ENUM_ACTIVE && (gbUSB_Out_Flg & (USB_OF_2D_FLOW | USB_OF_SEND_DATA) ) == (USB_OF_2D_FLOW | USB_OF_SEND_DATA) )
   {
      BYTE bBuffer[120];
      int iLen = snprintf((char*) bBuffer, sizeof bBuffer,  "Flow: %hi;%hi ; %hi;%hi ; %hi;%hi ; %hi;%hi\r\n",
         pstM->iFlowX_avg,
         pstM->iFlowY_avg,
         pstM->iFlowX_min,
         pstM->iFlowY_min,
         pstM->iFlowX_max,
         pstM->iFlowY_max,
         pstM->iAD_X,
         pstM->iAD_Y
      );
      cdcSendDataWaitTilDone( bBuffer, iLen, CDC0_INTFNUM, 1000);
   }
}

//------------------------------------------------------------------------------------------------------------------------
//    Ausgabe der EE893-Sensor-Daten (CO2-Sensor)
//------------------------------------------------------------------------------------------------------------------------
void USB_Send_EE893_Data (ST_EE893_MESS_DATA *pstM )
{
   if (USB_connectionState() == ST_ENUM_ACTIVE && (gbUSB_Out_Flg & (USB_OF_EE893 | USB_OF_SEND_DATA) ) == (USB_OF_EE893 | USB_OF_SEND_DATA) )
   {
      BYTE bBuffer[120];
      int iLen = snprintf((char*) bBuffer, sizeof bBuffer,
         "EE893: cur. %hu ppm; avg  %hu ppm; %hhX\r\n",
         pstM->wCO2_act, pstM->wCO2_avg, pstM->bStatus );
      cdcSendDataWaitTilDone( bBuffer, iLen, CDC0_INTFNUM, 1000);
   }
}

//------------------------------------------------------------------------------------------------------------------------
//    Setzen von System-Variablen
//------------------------------------------------------------------------------------------------------------------------
void USB_SetParameter(void)
{
   char bCmdBuffer[2];
   float fValue;

   if (USB_ReadBytes(bCmdBuffer, 2, true))
   {
      if ( (bCmdBuffer[0] != 'r' && bCmdBuffer[0] != 'd' && bCmdBuffer[0] != 's') ||
           (bCmdBuffer[0] == 'r' && bCmdBuffer[1] == 'p'))
         fValue = USB_ReadFloat ();
      switch (bCmdBuffer[0])
      {
      case 'c':                                       // CO2 (sc.)
         switch(bCmdBuffer[1])
         {
         case 'i':
            gSYS.ulEE893_MessInterval = (uint32_t) fValue;
            #ifdef USE_MSP430F6658
            EE893_SetMessInterval( gSYS.ulEE893_MessInterval );
            #endif
            cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
            break;
         default:
            cdcSendDataWaitTilDone( (BYTE*) mbCMD_UNKNOWN, sizeof( mbCMD_UNKNOWN ) -1, CDC0_INTFNUM, 1000);
            break;
         }
         break;
      case 'd':
         USB_ReadLine( gSYS.bDeviceName, sizeof(gSYS.bDeviceName));
         cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
         break;
      case 'f':                                       // Flow (sf.)
         switch(bCmdBuffer[1])
         {
         case 'a':
            gSYS.bFlowMessAnzahl = (uint8_t) fValue;
            Flow_SetMessAnzahl( gSYS.bFlowMessAnzahl );
            cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
            break;
         case 'd':
            gSYS.uiFlowDAC_Heizer = (uint16_t) fValue;
            Flow_Set_Heizer_DAC( gSYS.uiFlowDAC_Heizer );
            cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
            break;
         case 'i':
            gSYS.ulFlowMessInterval = (uint32_t) fValue;
            Flow_SetMessInterval( gSYS.ulFlowMessInterval );
            cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
            break;
         default:
            cdcSendDataWaitTilDone( (BYTE*) mbCMD_UNKNOWN, sizeof( mbCMD_UNKNOWN ) -1, CDC0_INTFNUM, 1000);
            break;
         }
         break;

      case 'l':                                       // Licht (sl.)
         switch(bCmdBuffer[1])
         {
         case 'a':
            gSYS.bLightMessAnzahl = (uint8_t) fValue;
            Si1132_SetMessAnzahl( gSYS.bLightMessAnzahl );
            cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
            break;
         case 'i':
            gSYS.ulLightMessInterval = (uint32_t) fValue;
            Si1132_SetMessInterval( gSYS.ulLightMessInterval );
            cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
            break;
         case '0':
            gSYS.stSi1132_CalVal.wVIS_ADC_DarkValue = (uint16_t) fValue;
            cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
            break;
         case '1':
            gSYS.stSi1132_CalVal.wIR_ADC_DarkValue =  (uint16_t) fValue;
            cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
            break;
         case '2':
            gSYS.stSi1132_CalVal.fVIS_Scale = fValue;
            cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
            break;
         case '3':
            gSYS.stSi1132_CalVal.fIR_Scale = fValue;
            cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
            break;
         default:
            cdcSendDataWaitTilDone( (BYTE*) mbCMD_UNKNOWN, sizeof( mbCMD_UNKNOWN ) -1, CDC0_INTFNUM, 1000);
            break;
         }
         break;

      case 'p':                                       // Druck (sp.)
         gSYS.ulPressureMessInterval = (uint32_t) fValue;
         BME280_SetMessinterval( gSYS.ulPressureMessInterval );
         cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
         break;

      case 'r':
          switch(bCmdBuffer[1])
         {
         case 'a':                                    // panid setzen (sra 5633)
            gSYS.stCC1350_SET.wPANID = (uint16_t) USB_ReadInt32();
            Radio_ReInitFunkModul();
            cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
            break;
         case 'c':
            gSYS.stCC1350_SET.bCHANNEL = (uint8_t) USB_ReadInt32();
            Radio_ReInitFunkModul();
            cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
            break;
         case 'i':
            USB_Parse_IPv6( gSYS.stLWM2M_SET.bSRV_IP, &gSYS.stLWM2M_SET.wSRV_PORT );
            Radio_ReInitFunkModul();
            cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
            break;
         case 'p':
            gSYS.stLWM2M_SET.wSRV_PORT = (uint16_t) fValue;
            Radio_ReInitFunkModul();
            cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
            break;
         case 'n':
            USB_ReadBytes(bCmdBuffer, 1, true);       // zwischen CMD und String-Start ein Zeichen überspringen
            USB_ReadLine( (uint8_t*)gSYS.stLWM2M_SET.strCLI_NAME, sizeof(gSYS.stLWM2M_SET.strCLI_NAME));
            Radio_ReInitFunkModul();
            cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
            break;

         default:
            cdcSendDataWaitTilDone( (BYTE*) mbCMD_UNKNOWN, sizeof( mbCMD_UNKNOWN ) -1, CDC0_INTFNUM, 1000);
            break;
         }
         break;

      case 's':
         USB_ReadLine( gSYS.bSerienNr, sizeof(gSYS.bSerienNr));
         cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
         break;
      default:
         cdcSendDataWaitTilDone( (BYTE*) mbCMD_UNKNOWN, sizeof( mbCMD_UNKNOWN ) -1, CDC0_INTFNUM, 1000);
         break;
      }
   }
}

//------------------------------------------------------------------------------------------------------------------------
//    Lesen von System-Variablen
//------------------------------------------------------------------------------------------------------------------------
void USB_GetParameter(void)
{
   char bCmdBuffer[2];
   if (USB_ReadBytes(bCmdBuffer, 2, true))
   {
      switch (bCmdBuffer[0])
      {
      case 'c':                                       // CO2 (gc.)
         USB_Send_int32_ASCII_Data("CO2-Mess-Intervall", gSYS.ulEE893_MessInterval ,"ms");
         break;
      case 'd':
         USB_Send_String( "Device-Name", gSYS.bDeviceName );
         break;
      case 'f':                                       // Flow (gf.)
         switch (bCmdBuffer[1])
         {
         case 'a':
            USB_Send_int32_ASCII_Data("Flow-Mess-Anzahl", gSYS.bFlowMessAnzahl ,"none");
            break;
         case 'd':
            USB_Send_int32_ASCII_Data("Flow-Heizer-DAC", gSYS.uiFlowDAC_Heizer ,"none");
            break;
         case 'i':
            USB_Send_int32_ASCII_Data("Flow-Mess-Intervall", gSYS.ulFlowMessInterval ,"ms");
            break;
         default:
            cdcSendDataWaitTilDone( (BYTE*) mbCMD_UNKNOWN, sizeof( mbCMD_UNKNOWN ) -1, CDC0_INTFNUM, 1000);
         }
         break;

      case 'l':                                       // Licht (gl.)
         switch (bCmdBuffer[1])
         {
         case 'a':
            USB_Send_int32_ASCII_Data("Light-Mess-Anzahl", gSYS.bLightMessAnzahl ,"none");
            break;
         case 'i':
            USB_Send_int32_ASCII_Data("Light-Mess-Intervall", gSYS.ulLightMessInterval ,"ms");
            break;
         case '0':
            USB_Send_int32_ASCII_Data("Light-VIS-Offset", gSYS.stSi1132_CalVal.wVIS_ADC_DarkValue ,"adc");
            break;
         case '1':
            USB_Send_int32_ASCII_Data("Light-IR-Offset", gSYS.stSi1132_CalVal.wIR_ADC_DarkValue ,"adc");
            break;
         case '2':
            USB_Send_float_ASCII_Data("Light-VIS-Scale", gSYS.stSi1132_CalVal.fVIS_Scale ,"lux/adc");
            break;
         case '3':
            USB_Send_float_ASCII_Data("Light-VIS-Scale", gSYS.stSi1132_CalVal.fIR_Scale ,"lux/adc");
            break;
         default:
            cdcSendDataWaitTilDone( (BYTE*) mbCMD_UNKNOWN, sizeof( mbCMD_UNKNOWN ) -1, CDC0_INTFNUM, 1000);
         }
         break;

      case 'p':                                       // Druck (gp.)
         USB_Send_int32_ASCII_Data("Pressure-Mess-Intervall", gSYS.ulPressureMessInterval ,"ms");
         break;

      case 'r':
          switch(bCmdBuffer[1])
         {
         case 'a':
            USB_Send_int32_ASCII_Data("PANID", gSYS.stCC1350_SET.wPANID ,"");
            break;
         case 'c':
            USB_Send_int32_ASCII_Data("Radio-Channel", gSYS.stCC1350_SET.bCHANNEL ,"");
            break;
         case 'i':
            USB_Send_IPv6("Server-IPv6", gSYS.stLWM2M_SET.bSRV_IP, gSYS.stLWM2M_SET.wSRV_PORT);
            break;
         case 'p':
            USB_Send_int32_ASCII_Data("Server-Port", gSYS.stLWM2M_SET.wSRV_PORT ,"");
            break;
         case 'n':
            USB_Send_String("Client-Name", (uint8_t*) gSYS.stLWM2M_SET.strCLI_NAME );
            break;
         default:
            cdcSendDataWaitTilDone( (BYTE*) mbCMD_UNKNOWN, sizeof( mbCMD_UNKNOWN ) -1, CDC0_INTFNUM, 1000);
            break;
         }
         break;

         break;
      case 's':
         USB_Send_String("Serien-Nr", gSYS.bSerienNr );
         break;
      default:
         cdcSendDataWaitTilDone( (BYTE*) mbCMD_UNKNOWN, sizeof( mbCMD_UNKNOWN ) -1, CDC0_INTFNUM, 1000);
         break;
      }
   }
}

//------------------------------------------------------------------------------------------------------------------------
// USB_Task:   Überwacht die USB-Aktivtäten. Wird auch von anderen Modulen aufgerufen
//------------------------------------------------------------------------------------------------------------------------
BYTE USB_Task(void)
{
   static uint32_t ulStartTime = 0;                   // Wird für die Timeoutüberwachung hergenommen

   BYTE bRetValue = (USB_connectionState() == ST_ENUM_ACTIVE);

   if (bRetValue)
   {
      char bCmdBuffer[2];
      ulStartTime = 0;
      if (USB_ReadBytes(bCmdBuffer, 1, false))
      {
         switch (bCmdBuffer[0])
         {
         case '0':
            gbUSB_Out_Flg &= ~USB_OF_BME280_PTH;      // BME280-Ausgabe ausschalten
            cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
            break;
         case '1':                                    // BME280-Ausgabe einschalten
            gbUSB_Out_Flg |= USB_OF_SEND_DATA | USB_OF_BME280_PTH;
            cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
            break;
         case '2':
            gbUSB_Out_Flg &= ~USB_OF_Si1132;          // Si1132-Ausgabe ausschalten
            cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
            break;
         case '3':                                    // Si1132-Ausgabe einschalten
            gbUSB_Out_Flg |= USB_OF_SEND_DATA | USB_OF_Si1132;
            cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
            break;
         case '4':
            gbUSB_Out_Flg &= ~USB_OF_2D_FLOW;         // 2D-Flow-Ausgabe ausschalten
            cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
            break;
         case '5':                                    // 2D-Flow-Ausgabe einschalten
            gbUSB_Out_Flg |= USB_OF_SEND_DATA | USB_OF_2D_FLOW;
            cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
            break;
         case '6':
            gbUSB_Out_Flg &= ~USB_OF_EE893;            // EE893-CO2-Ausgabe stoppen
            cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
            break;
         case '7':                                    // EE893-CO2-Ausgabe einschalten
            gbUSB_Out_Flg |= USB_OF_SEND_DATA | USB_OF_EE893;
            cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
            break;
         case '8':
            Task_GoStreamingMode();
            gbUSB_Out_Flg |= USB_OF_SEND_DATA | USB_OF_Si1132 | USB_OF_2D_FLOW;
//             gbUSB_Out_Flg |= USB_OF_SEND_DATA  | USB_OF_2D_FLOW;                   // Testweise nur Flow
            cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
            break;

         case 'b':                                    // b wie Batterie
            USB_ReadBytes(bCmdBuffer, 1, true);
            switch (bCmdBuffer[0])
            {
            case 'p':
               USB_Send_int32_ASCII_Data( "Rel. Power",              // Aktueller Ladezustand in %
                                         AKKU_GET_REL_POWER(),
                                         "%" );
               break;
            case 'c':
               USB_Send_int32_ASCII_Data( "Akku-Strom",              // Aktueller Akku-Strom mA
                                         AKKU_GET_AVG_CURRENT() ,
                                         "mA" );
               break;
            case 'v':
               USB_Send_int32_ASCII_Data( "Akku-Spannung",           // Akku-Spannung in mV
                                        AKKU_GET_SPANNUNG(),
                                        "mV" );
               break;
            }
            break;

         case 'c':                                    // clear Flash Look-Up-Table
            USB_ReadBytes(bCmdBuffer, 1, true);
            switch (bCmdBuffer[0])
            {
            case 'f':                                 // cf
               Flow_ClearLookUpTables();
               cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
               break;
            default:
               cdcSendDataWaitTilDone( (BYTE*) mbCMD_UNKNOWN, sizeof( mbCMD_UNKNOWN ) -1, CDC0_INTFNUM, 1000);
            }
            break;

         case 'f':                                    // f wie Flash -> speichere die aktuelle Konfig im Flash
            StoreFlashData();
            cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
            break;

         case 'i':
            USB_SendFunkID();
            break;

         case 'l':                                    // load Flash Look-Up-Tabele
            USB_ReadBytes(bCmdBuffer, 1, true);
            if (bCmdBuffer[0] == 'f')                 // Flow-Table
            {
               USB_ReadBytes(bCmdBuffer, 1, true);
               switch (bCmdBuffer[0])
               {
               case 'c':
                  Flow_ClearLookUpTables();
                  cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
                  break;
               case 'i':
                  Flow_SetIndendityFlowTable();
                  cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
                  break;
               case 'x':                              // lfx ...
                  USB_SetTableData( Flow_LoadLookUpTable, FLOW_LOOK_UP_SIZE, 1 );
                  cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
                  break;
               case 'y':                              // lfy ...
                  USB_SetTableData( Flow_LoadLookUpTable, FLOW_LOOK_UP_SIZE, 0 );
                  cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
                  break;
               default:
                  cdcSendDataWaitTilDone( (BYTE*) mbCMD_UNKNOWN, sizeof( mbCMD_UNKNOWN ) -1, CDC0_INTFNUM, 1000);
                  break;
               }
            }
            else
               cdcSendDataWaitTilDone( (BYTE*) mbCMD_UNKNOWN, sizeof( mbCMD_UNKNOWN ) -1, CDC0_INTFNUM, 1000);
            break;

         case 'g':                                    // g wie get
            USB_GetParameter();
            break;
         case 's':                                    // s wie set
            USB_SetParameter();
            break;
         case 'S':                                    // USB-Messdatenausgaben stoppen
            gbUSB_Out_Flg &= ~USB_OF_SEND_DATA;
            Task_GoNormalMode();
            OLED_UpdateStatusTextNo(IDX_TEXT_USB_AD_Stop);
            cdcSendDataWaitTilDone( (BYTE*) mbDone, sizeof(mbDone) -1, CDC0_INTFNUM, 1000);
            break;
         case 't':
            USB_Send_String("Device-Type", "30: EIS-Temp/Feuchte/Luftdruck/2d Wind/Licht/CO2");
            break;
         case 'v':                                    //    v:         Versionsabfrage
            cdcSendDataWaitTilDone( (BYTE*) gbVersion, sizeof_gbVersion()-1, CDC0_INTFNUM, 1000);
            break;

         case '\r':
         case '\n':
            break;                                    // crlf einfach ignoieren
         default:
            // USB-Buffer leeren wenn unbekanntes CMD empfangen wurde
            while (USB_ReadBytes(bCmdBuffer, 1, false)) _NOP();
//            OLED_UpdateStatusTextNo(IDX_TEXT_USB_CMD_unkown);
            cdcSendDataWaitTilDone( (BYTE*) mbCMD_UNKNOWN, sizeof( mbCMD_UNKNOWN ) -1, CDC0_INTFNUM, 1000);
            break;
         }
      }

//      if ( (gbUSB_Out_Flg & (USB_OF_NEW_DATA | USB_OF_SEND_DATA)) == (USB_OF_NEW_DATA | USB_OF_SEND_DATA))
//      {
//         gbUSB_Out_Flg &=  ~USB_OF_NEW_DATA;          // new Data-Flag zurücksetzen
//
//         if ( gbUSB_Out_Flg & USB_OF_WM_DATA )
//         {
//            USB_Send_SpiroRec();
//         }
//         else if ( gbUSB_Out_Flg & USB_OF_AD_DATA)
//         {
//            USB_Send_AD_Data();
//         }
//         else if ( gbUSB_Out_Flg & USB_OF_AD_ASCII)
//         {
//            USB_Send_AD_ASCII_Data();
//         }
//         else if ( gbUSB_Out_Flg & USB_OF_WM_ASCII)
//         {
//            USB_Send_SpiroASCII();
//         }
//         else if ( gbUSB_Out_Flg & USB_OF_CO2_ASCII)
//         {
//            USB_Send_CO2_ASCII();
//         }
//      }
   }
   else
   {
      if (is_USB_PG)
      {
         if (ulStartTime == 0)                        // Check for Timeout für das Laden des PC / Tablet-Driver
            ulStartTime = HAL_GetTickCount();
         else if (isXT2_running)                      // Quarz wurde vom USB-Event-Handler eingeschaltet
         {
            if ( HAL_GetTickCount() - ulStartTime > 30000 )
            {
               ulStartTime = HAL_GetTickCount();      // Nach 30 Sekunden Quarz abschalten -> USB muss erneut gesteckt werden um erkannt zu werden
               UcsUse_DCO();                          // in Abhängigkeit von Quarz an/aus wird im Task_Manager in den Low-Power-Modes gegangen
               if (!isPower_Plug_Connected())
                  OLED_Off();                         // OLED bei USB-Laden auschalten
            }
         }
      }
      else
         ulStartTime = 0;
   }

   return bRetValue;                                  // True wenn PC-Treiber geladen
}
