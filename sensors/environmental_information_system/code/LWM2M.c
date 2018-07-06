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
/* Hahn-Schickard:	   LWM2M
 *********************************************************************
 * FileName:            LWM2M.c
 * Dependencies:        See INCLUDES section below
 * Processor:           Apollo
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Bernd Ehrbrecht		08.03.2017
 ********************************************************************/

#include <string.h>

#include "hardware.h"
#include "stdint.h"
#include "LWM2M.h"

#include "globals.h"
#include "Radio_Task.h"
#include "main.h"

#include "LWM2M_BME280_Resources.h"
#include "LWM2M_Si1132_Resources.h"
#include "LWM2M_Flow_Resources.h"
#include "LWM2M_MessData_Resources.h"


//ST_FESTO_DATA_t mstFestoData;
uint32_t muiIndex;
uint32_t muiObjectIndex   = 0;
uint8_t  mbInstanceIndex  = 0;
uint32_t muiResourceIndex = 0;
uint32_t muiResourceCounter=0;                        // nur zu debug-zwecken

//==============================================================================================================================
// das ist die universelle Temp-Variable für Datenumwandlung/Skalierung des Moduls
//==============================================================================================================================
UN_ALL_DATA_t gunAllTemp;
uint32_t guiLWM2M_Status = 0xFF;

//==============================================================================================================================
// hier nun die Object-Liste
//==============================================================================================================================
#define LWM2M_ObjList_COUNT   (sizeof(stLWM2M_ObjList) / sizeof(ST_LWM2M_OBJECT_t))
const ST_LWM2M_OBJECT_t stLWM2M_ObjList[] =
//ST_LWM2M_OBJECT_t stLWM2M_ObjList[] =
{
   //===========================================================================================================================
   {                                                  // BME280-Object
      .wObjectID   = BME280_TEMP_OBJ_ID,              // Object-ID
      .bMaxInstID  = 0,
      .uiObjFlags  = LWM2M_OBJ_FLAG_NORMAL_OBJ,
      .pItems      = &stBME280_Temp_Items,            // Pointer auf die Items
   },
   {                                                  // BME280-Object
      .wObjectID   = BME280_HUMITY_OBJ_ID,            // Object-ID
      .bMaxInstID  = 0,
      .uiObjFlags  = LWM2M_OBJ_FLAG_NORMAL_OBJ,
      .pItems      = &stBME280_Humity_Items,          // Pointer auf die Items
   },
   {                                                  // BME280-Object
      .wObjectID   = BME280_PRESSURE_OBJ_ID,          // Object-ID
      .bMaxInstID  = 0,
      .uiObjFlags  = LWM2M_OBJ_FLAG_NORMAL_OBJ,
      .pItems      = &stBME280_Pressure_Items,        // Pointer auf die Items
   },
//   {                                                  // BME280-Init-Object
//      .wObjectID   = BME280_INI_OBJ_ID,               // Object-ID
//      .bMaxInstID  = 0,
//      .uiObjFlags  = LWM2M_OBJ_FLAG_NORMAL_OBJ,
//      .uiItemCount = GET_ITEM_COUNT( stBME280_InitItems ),   // Anzahl der Items des Objects
//      .pItems      = &stBME280_InitItems[0],          // Pointer auf die Items
//   },
   //===========================================================================================================================
   {                                                  // Si1132-Object
      .wObjectID   = Si1132_VIS_OBJ_ID,               // Object-ID
      .bMaxInstID  = 0,
      .uiObjFlags  = LWM2M_OBJ_FLAG_NORMAL_OBJ,
      .pItems      = &stSi1132_VIS_Items,             // Pointer auf die Items
   },
//   {                                                  // Si1132-Object
//      .wObjectID   = Si1132_IR_OBJ_ID,                // Object-ID
//      .bMaxInstID  = 0,
//      .pItems      = &stSi1132_IR_items[0],         // Pointer auf die Items
//   },
//   {                                                  // Si1132-Object
//      .wObjectID   = Si1132_UV_OBJ_ID,                // Object-ID
//      .bMaxInstID  = 0,
//      .pItems      = &stSi1132_UV_items[0],     // Pointer auf die Items
//   },
//   {                                                  // Si1132-Object
//      .wObjectID   = Si1132_INI_OBJ_ID,               // Object-ID
//      .bMaxInstID  = 0,
//      .uiObjFlags  = LWM2M_OBJ_FLAG_NORMAL_OBJ,
//      .pItems      = &stSi1132_InitItems[0],          // Pointer auf die Items
//   },
   {
      .wObjectID   = FLOW_X_OBJ_ID,                   // Object-ID
      .bMaxInstID  = 0,
      .uiObjFlags  = LWM2M_OBJ_FLAG_NORMAL_OBJ,
      .pItems      = &stFlow_X_Items,                 // Pointer auf die Items
   },
   {
      .wObjectID   = FLOW_Y_OBJ_ID,                   // Object-ID
      .bMaxInstID  = 0,
      .uiObjFlags  = LWM2M_OBJ_FLAG_NORMAL_OBJ,
      .pItems      = &stFlow_Y_Items,                 // Pointer auf die Items
   },
   {
      .wObjectID   = LWM2M_MESS_DATA_OBJ_ID,            // neues Mass-Data-Object-ID um mit Instance-Write alle Messdaten zu übertragen
      .bMaxInstID  = 0,
      .uiObjFlags  = LWM2M_OBJ_FLAG_NORMAL_OBJ,
      .pItems      = &stMessDataResources,              // Pointer auf die Items
   },
};

//==============================================================================================================================
// Private-Funktionen
//==============================================================================================================================


//==============================================================================================================================
//==============================================================================================================================
static int32_t LWM2M_FindObjectIndex( ST_LWM2M_IDs_t *stIDs )
{
   for(uint32_t i=0; i<LWM2M_ObjList_COUNT; i++)
   {
      if (stLWM2M_ObjList[i].wObjectID == stIDs->wObjectID)
         return i;                                    // Object-ID gefunden -> Index zurückgeben
   }
   return -1;                                         // nicht gefunden
}

//==============================================================================================================================
//==============================================================================================================================
static int32_t LWM2M_FindItemIndex( ST_LWM2M_IDs_t *stIDs, int32_t iObjectIndex )
{
   const ST_LWM2M_RESOURCE_ITEM_t *pItem = stLWM2M_ObjList[iObjectIndex].pItems->pstItems;
   uint32_t uiItemCount = stLWM2M_ObjList[iObjectIndex].pItems->uiItemCount;
   for(uint32_t i=0; i<uiItemCount; i++)
   {
      if (pItem->wResourceID == stIDs->wResourceID)
         return i;                                    // Resource-ID gefunden -> Index zurückgeben
      pItem++;
   }
   return -1;                                         // nicht gefunden
}

//==============================================================================================================================
// Dies Funktion bereitet Objekt Daten auf und speichert diese im UART-Buffer so dass dieser gesendet werden kann
//==============================================================================================================================
//  stIDs:                 Zeiger auf die Object/Instance/Resource-IDs
//  bBuf:                  hier gehen die aufbereiteten Daten hin
//  uiLen:                 größe des übergebenen Buffers
//==============================================================================================================================
//  return:                Anzahl der übertragenen Bytes oder negativ wenn Fehler
//==============================================================================================================================
int32_t LWM2M_GetItem( ST_LWM2M_IDs_t *stIDs, uint8_t *bBuf, uint32_t uiLen )
{
   int32_t iObjectIndex = LWM2M_FindObjectIndex( stIDs );
   if (iObjectIndex<0)
      return -1;
                                                      // prüfe ob Instanze-ID im gültigen Bereich falls nicht return
   if ( stLWM2M_ObjList[iObjectIndex].bMaxInstID  < stIDs->bInstanceID)
      return -2;

   int32_t iItemIndex = LWM2M_FindItemIndex( stIDs, iObjectIndex );
   if (iItemIndex<0)
      return -3;
                                                      // ok Object und Item gefunden nun verarbeiten
   const ST_LWM2M_RESOURCE_ITEM_t *pItem = &stLWM2M_ObjList[iObjectIndex].pItems->pstItems[ iItemIndex ];

   if ((pItem->uiTypeFLGs & LWM2M_ACCESS_RD) == 0)
      return -4;                                      // no read access

   int32_t iRetValue;
   if ( pItem->uiTypeFLGs & LWM2M_EXECUTE_GET_BEFORE && pItem->pvGet )
   {
      iRetValue =
         pItem->pvGet( bBuf, uiLen,
                       pItem, stIDs->bInstanceID);    // die Get-Funktion skaliert nur die daten und legt Ergebniss in gunAllTemp ab
      if (iRetValue<0)                                // sinnvoll also nur wenn auch LWM2M_USE_TEMP_DATA gesetzt ist
         return iRetValue*100;                        // fehler Melden
   }

   uint8_t *pItemValue = 0;
   if (pItem->uiTypeFLGs & LWM2M_USE_TEMP_DATA)
      pItemValue = (uint8_t *) &gunAllTemp;
   else
   {
      pItemValue  = (uint8_t *) pItem->pvData;
      // LEN_Mask>>4 = Datenlänge in Bytes * [INST_ID] Inst-ID ist Array-Index
      pItemValue += ((pItem->uiTypeFLGs & LWM2M_LEN_MASK)>>4) * stIDs->bInstanceID;
   }

   if (pItemValue)
   {
      switch (pItem->uiTypeFLGs & LWM2M_LEN_MASK)
      {
      case 0x80:                                      // double übertragen
         if (uiLen<8)
            return -4;
         *bBuf++ = pItemValue[7];
         *bBuf++ = pItemValue[6];
         *bBuf++ = pItemValue[5];
         *bBuf++ = pItemValue[4];
         *bBuf++ = pItemValue[3];
         *bBuf++ = pItemValue[2];
         *bBuf++ = pItemValue[1];
         *bBuf++ = pItemValue[0];
         break;
      case 0x40:                                      // hier float/uint32_t/int32_t Werte übertragen
         if (uiLen<4)
            return -5;
         *bBuf++ = pItemValue[3];
         *bBuf++ = pItemValue[2];
         *bBuf++ = pItemValue[1];
         *bBuf++ = pItemValue[0];
         break;
      case 0x20:                                      // hier uint16_t/int16_t Werte übertragen
         if (uiLen<2)
            return -6;
         *bBuf++ = pItemValue[1];
         *bBuf++ = pItemValue[0];
      case 0x10:                                      // hier char und uint8_t Werte übertragen
         if (uiLen==0)
            return -7;
         *bBuf++ = pItemValue[0];
         break;
      case 0x00:
         if( (pItem->uiTypeFLGs & LWM2M_TYPE_MASK) == LWM2M_TYPE_STRING )
         {
            uint32_t uiStrLen = strlen( (const char*) pItemValue );
            if ( uiStrLen > uiLen )
               uiStrLen = uiLen;                      // wenn weniger Zeichen im Buffer Platz haben diese darauf begrenzen
            memcpy( bBuf, pItemValue, uiStrLen );
            return uiStrLen;
         }
      }
   }
   if ( pItem->uiTypeFLGs & LWM2M_EXECUTE_GET_AFTER && pItem->pvGet )
      return pItem->pvGet( bBuf, uiLen,
                           pItem, stIDs->bInstanceID ); // die Get-Funktion übernimmt selbst die Aufbereitung der Daten

   return (pItem->uiTypeFLGs & LWM2M_LEN_MASK)>>4;    // Länge in Bytes zurückgeben
}

//==============================================================================================================================
// Dies Funktion bereitet Objekt Daten auf und speichert diese im UART-Buffer so dass dieser gesendet werden kann
//==============================================================================================================================
//  stIDs:                 Zeiger auf die Object/Instance/Resource-IDs
//  bBuf:                  hier stehen die auszuwertenden Daten drin, die ins Objegt geschrieben werden sollen
//  uiLen:                 größe des übergebenen Buffers
//==============================================================================================================================
//  return:                Anzahl der ausgewerteten Bytes oder negativ wenn Fehler
//==============================================================================================================================
int32_t LWM2M_SetItem( ST_LWM2M_IDs_t *stIDs, uint8_t *bBuf, uint32_t uiLen )
{
   int32_t iObjectIndex = LWM2M_FindObjectIndex( stIDs );
   if (iObjectIndex<0)
      return -1;
                                                      // prüfe ob Instanze-ID im gültigen Bereich falls nicht return
   if ( stLWM2M_ObjList[iObjectIndex].bMaxInstID  < stIDs->bInstanceID)
      return -2;

   int32_t iItemIndex = LWM2M_FindItemIndex( stIDs, iObjectIndex );
   if (iItemIndex<0)
      return -3;
                                                      // ok Object und Item gefunden nun verarbeiten
   const ST_LWM2M_RESOURCE_ITEM_t *pItem = &stLWM2M_ObjList[iObjectIndex].pItems->pstItems[ iItemIndex ];

   if ((pItem->uiTypeFLGs & (LWM2M_ACCESS_WR | LWM2M_ACCESS_EX)) == 0)
      return -4;                                      // no write access / no execute access

   if ( pItem->uiTypeFLGs & LWM2M_EXECUTE_SET_BEFORE && pItem->pvSet )
      return pItem->pvSet( bBuf, uiLen, pItem,
                           stIDs->bInstanceID);       // Die Set-Funktion holt selbst die Daten aus dem bBuf ab und macht den Rest

   uint8_t *pItemValue = 0;
   if (pItem->uiTypeFLGs & LWM2M_USE_TEMP_DATA)
      pItemValue = (uint8_t *) &gunAllTemp;
   else
   {
      pItemValue = (uint8_t *) pItem->pvData;
      // LEN_Mask>>4 = Datenlänge in Bytes * [INST_ID] Inst-ID ist Array-Index
      pItemValue += ((pItem->uiTypeFLGs & LWM2M_LEN_MASK)>>4) * stIDs->bInstanceID;
   }

   if (pItemValue)
   {
      switch (pItem->uiTypeFLGs & LWM2M_LEN_MASK)
      {
      case 0x80:                                         // double übertragen
         if (uiLen<8)
            return -4;
         pItemValue[7] = *bBuf++;
         pItemValue[6] = *bBuf++;
         pItemValue[5] = *bBuf++;
         pItemValue[4] = *bBuf++;
         pItemValue[3] = *bBuf++;
         pItemValue[2] = *bBuf++;
         pItemValue[1] = *bBuf++;
         pItemValue[0] = *bBuf;
         uiLen = 8;
         break;
      case 0x40:                                         // hier float/uint32_t/int32_t Werte übertragen
         if (uiLen<4)
            return -5;
         pItemValue[3] = *bBuf++;
         pItemValue[2] = *bBuf++;
         pItemValue[1] = *bBuf++;
         pItemValue[0] = *bBuf;
         uiLen = 4;
         break;
      case 0x20:                                         // hier uint16_t/int16_t Werte übertragen
         if (uiLen<2)
            return -6;
         pItemValue[1] = *bBuf++;
         pItemValue[0] = *bBuf;
         uiLen = 2;
         break;
      case 0x10:                                         // hier char und uint8_t Werte übertragen
         if (uiLen==0)
            return -7;
         pItemValue[0] = *bBuf;
         uiLen = 1;
         break;
      case 0x00:
         if( (pItem->uiTypeFLGs & LWM2M_TYPE_MASK) == LWM2M_TYPE_STRING )
         {
            uint32_t uiStrLen = LWM2M_MAX_STRING_LEN( pItem->uiTypeFLGs )-1;  // wegen 0-Byte
            pItemValue = (uint8_t *) pItem->pvData;

            if ( uiStrLen > uiLen )
               uiStrLen = uiLen;                         // es passen mehr Zeichen in String-Buffer als gesendet wurden

            memcpy( pItemValue, bBuf, uiStrLen );
            pItemValue[uiStrLen] = 0;                    // 0-Byte anhängen
            return uiStrLen;
         }
      }
   }
   // Daten sind entweder bereits an ort und stelle (z. B. gSYS) oder sie wurden nach gunAllTemp geladen

   if ( pItem->uiTypeFLGs & LWM2M_EXECUTE_SET_AFTER && pItem->pvSet )
      uiLen = pItem->pvSet( bBuf, uiLen, pItem,
                            stIDs->bInstanceID );     // Die Set-Funktion holt selbst die Daten aus dem bBuf ab und macht den Rest
                                                      // die Set-Funktion um den neuen Parameter an Sensoren/Task zu übertragen
                                                      // z. B. wenn gSYS. ..setings..IDEL-Time neu gesetzt wurde muss eine Task-Funktion (SetIdelTime) aufgerufen werden
                                                      // damit dieser Parameter sofort wirksam wird. Ähnliches gilt für Sensoren wenn Messbereich etc. verändert wurde

   if(pItem->uiTypeFLGs & LWM2M_STORE_IN_FLASH)
      StoreFlashData();                               // Daten werden sofort im Flash gespeichert wenn LWM2M_STORE_IN_FLASH-Bit gesetzt ist

   return uiLen;                                      // länge der ausgewerteten Byte zurückgeben
}

//==============================================================================================================================
// Public-Funktionen
//==============================================================================================================================
void LWM2M_GAS_DeviceStart( void )
{
   Radio_Send_Record(LWM2M_GAS_DEVICE_START, 0, 0);
}

void LWM2M_GAS_DeviceStop( void )
{
   Radio_Send_Record(LWM2M_GAS_DEVICE_STOP, 0, 0);
}

void LWM2M_GAS_SetMAC( void )
{
   uint8_t bBuffer[7];
   bBuffer[0] = GAS_CFG_ID_MACADDR;
   memcpy( &bBuffer[1], gSYS.stCC1350_SET.bMAC_ADR, sizeof(bBuffer) -1);
   Radio_Send_Record(LWM2M_GAS_CFG_SET, bBuffer, sizeof(bBuffer));
}

void LWM2M_GAS_SetPANID( void )
{
   uint8_t bBuffer[3];
   bBuffer[0] = GAS_CFG_ID_PAN;
   bBuffer[1] = gSYS.stCC1350_SET.wPANID >> 8;
   bBuffer[2] = gSYS.stCC1350_SET.wPANID;
   Radio_Send_Record(LWM2M_GAS_CFG_SET, bBuffer, sizeof(bBuffer));
}

void LWM2M_GAS_SetOPMODE( void )
{
   uint8_t bBuffer[2];
   bBuffer[0] = GAS_CFG_ID_OPMODE;
   bBuffer[1] = gSYS.stCC1350_SET.bOPMODE;
   Radio_Send_Record(LWM2M_GAS_CFG_SET, bBuffer, sizeof(bBuffer));
}

void LWM2M_GAS_SetChannel( void )
{
   uint8_t bBuffer[2];
   bBuffer[0] = GAS_CFG_ID_CHANNEL;
   bBuffer[1] = gSYS.stCC1350_SET.bCHANNEL;
   Radio_Send_Record(LWM2M_GAS_CFG_SET, bBuffer, sizeof(bBuffer));
}

void LWM2M_Stop(void)
{
   uint8_t bBuffer[1];
   bBuffer[0] = LWM2M_STOP;
   Radio_Send_Record(LWM2M_LWM2M_CMD, bBuffer, sizeof(bBuffer));
}

void LWM2M_Start(void)
{
   uint8_t bBuffer[1];
   bBuffer[0] = LWM2M_START;
   Radio_Send_Record(LWM2M_LWM2M_CMD, bBuffer, sizeof(bBuffer));
}

//==============================================================================================================================
//    Hier die SET-LWM2M-Parameter an CC1350 übertagen  entweder als response auf ein LWM2M_GAS_CFG_GET oder als
//    Befehl von UHF-RFID oder NFC
//==============================================================================================================================
void LWM2M_SET_BS_SRV_IP   ( uint32_t bIsResponse )
{
   uint8_t bTxBuf[2+sizeof(gSYS.stLWM2M_SET.bBS_SRV_IP)];
   bTxBuf[0] = (bIsResponse) ? LWM2M_GAS_CFG_RES : LWM2M_GAS_CFG_SET;
   bTxBuf[1] = LWM2M_CFG_ID_BS_SVR_IP;
   memcpy( &bTxBuf[2], gSYS.stLWM2M_SET.bBS_SRV_IP, sizeof(gSYS.stLWM2M_SET.bBS_SRV_IP) );
   Radio_Send_Record( LWM2M_LWM2M_CMD, bTxBuf, sizeof(bTxBuf) );
}

void LWM2M_SET_BS_SRV_PORT ( uint32_t bIsResponse )
{
   uint8_t bTxBuf[2+2];
   bTxBuf[0] = (bIsResponse) ? LWM2M_GAS_CFG_RES : LWM2M_GAS_CFG_SET;
   bTxBuf[1] = LWM2M_CFG_ID_BS_SVR_PORT;
   bTxBuf[2] = gSYS.stLWM2M_SET.wBS_SRV_PORT>>8;
   bTxBuf[3] = gSYS.stLWM2M_SET.wBS_SRV_PORT;
   Radio_Send_Record( LWM2M_LWM2M_CMD, bTxBuf, sizeof(bTxBuf) );
}

void LWM2M_SET_SRV_IP      ( uint32_t bIsResponse )
{
   uint8_t bTxBuf[2+sizeof(gSYS.stLWM2M_SET.bBS_SRV_IP)];
   bTxBuf[0] = (bIsResponse) ? LWM2M_GAS_CFG_RES : LWM2M_GAS_CFG_SET;
   bTxBuf[1] = LWM2M_CFG_ID_SVR_IP;
   memcpy( &bTxBuf[2], gSYS.stLWM2M_SET.bSRV_IP, sizeof(gSYS.stLWM2M_SET.bSRV_IP) );
   Radio_Send_Record( LWM2M_LWM2M_CMD, bTxBuf, sizeof(bTxBuf) );
}

void LWM2M_SET_SRV_PORT    ( uint32_t bIsResponse )
{
   uint8_t bTxBuf[2+2];
   bTxBuf[0] = (bIsResponse) ? LWM2M_GAS_CFG_RES : LWM2M_GAS_CFG_SET;
   bTxBuf[1] = LWM2M_CFG_ID_SVR_PORT;
   bTxBuf[2] = gSYS.stLWM2M_SET.wSRV_PORT>>8;
   bTxBuf[3] = gSYS.stLWM2M_SET.wSRV_PORT;
   Radio_Send_Record( LWM2M_LWM2M_CMD, bTxBuf, sizeof(bTxBuf) );
}

void LWM2M_SET_CLI_NAME    ( uint32_t bIsResponse )
{
   uint8_t bTxBuf[2+sizeof(gSYS.stLWM2M_SET.strCLI_NAME) ];
   bTxBuf[0] = (bIsResponse) ? LWM2M_GAS_CFG_RES : LWM2M_GAS_CFG_SET;
   bTxBuf[1] = LWM2M_CFG_ID_CLI_NAME;
   uint32_t uiLen = strlen(gSYS.stLWM2M_SET.strCLI_NAME);
   if (uiLen > 32) uiLen = 32;
   memcpy( &bTxBuf[2], gSYS.stLWM2M_SET.strCLI_NAME, uiLen );
   Radio_Send_Record( LWM2M_LWM2M_CMD, bTxBuf, uiLen+2 );
}

//==============================================================================================================================
//    Hier die LWM2M-Parameter von CC1350 abfragen - z. B. wegen Anfrage durch UHF-RFID oder NFC
//==============================================================================================================================
void LWM2M_GET_BS_SRV_IP( void )
{
   uint8_t bTxBuf[2] = { LWM2M_GAS_CFG_GET, LWM2M_CFG_ID_BS_SVR_IP };
   Radio_Send_Record( LWM2M_LWM2M_CMD, bTxBuf, sizeof(bTxBuf) );
}

void LWM2M_GET_BS_SRV_PORT( void )
{
   uint8_t bTxBuf[2] = { LWM2M_GAS_CFG_GET, LWM2M_CFG_ID_BS_SVR_PORT};
   Radio_Send_Record( LWM2M_LWM2M_CMD, bTxBuf, sizeof(bTxBuf) );
}

void LWM2M_GET_SRV_IP( void )
{
   uint8_t bTxBuf[2] = { LWM2M_GAS_CFG_GET, LWM2M_CFG_ID_SVR_IP };
   Radio_Send_Record( LWM2M_LWM2M_CMD, bTxBuf, sizeof(bTxBuf) );
}

void LWM2M_GET_SRV_PORT( void )
{
   uint8_t bTxBuf[2] = { LWM2M_GAS_CFG_GET, LWM2M_CFG_ID_SVR_PORT };
   Radio_Send_Record( LWM2M_LWM2M_CMD, bTxBuf, sizeof(bTxBuf) );
}

void LWM2M_GET_CLI_NAME( void )
{
   uint8_t bTxBuf[2] = { LWM2M_GAS_CFG_GET, LWM2M_CFG_ID_CLI_NAME };
   Radio_Send_Record( LWM2M_LWM2M_CMD, bTxBuf, sizeof(bTxBuf) );
}

//==============================================================================================================================
//    Hier wurden neue LWM2M-Parameter von CC1350 gesendet. Diese nun in gSYS eintragen und Response senden
//==============================================================================================================================
void LWM2M_CC1350_SET_SendResponse( uint8_t bError )
{
   uint8_t bTxBuf[2] = {LWM2M_GAS_CFG_RES, bError};
   Radio_Send_Record( LWM2M_LWM2M_CMD, bTxBuf, sizeof(bTxBuf) );
}

void LWM2M_CC1350_SET_BS_SRV_IP( uint8_t *pbBuf, uint32_t uiLen )
{
   if (uiLen!= sizeof(gSYS.stLWM2M_SET.bBS_SRV_IP))
   {
      LWM2M_CC1350_SET_SendResponse( LWM2M_ERROR_PARAM );
      return;
   }

   memcpy( gSYS.stLWM2M_SET.bBS_SRV_IP, pbBuf, sizeof(gSYS.stLWM2M_SET.bBS_SRV_IP) );
   LWM2M_CC1350_SET_SendResponse( LWM2M_ERROR_OK );
}

void LWM2M_CC1350_SET_BS_SRV_PORT( uint8_t *pbBuf, uint32_t uiLen )
{
   if (uiLen!= sizeof(gSYS.stLWM2M_SET.wBS_SRV_PORT))
   {
      LWM2M_CC1350_SET_SendResponse( LWM2M_ERROR_PARAM );
      return;
   }
   gSYS.stLWM2M_SET.wBS_SRV_PORT   = *pbBuf++;
   gSYS.stLWM2M_SET.wBS_SRV_PORT <<= 8;
   gSYS.stLWM2M_SET.wBS_SRV_PORT  |= *pbBuf;
   LWM2M_CC1350_SET_SendResponse( LWM2M_ERROR_OK );
}

void LWM2M_CC1350_SET_SRV_IP( uint8_t *pbBuf, uint32_t uiLen )
{
   if (uiLen!= sizeof(gSYS.stLWM2M_SET.bBS_SRV_IP))
   {
      LWM2M_CC1350_SET_SendResponse( LWM2M_ERROR_PARAM );
      return;
   }

   memcpy( gSYS.stLWM2M_SET.bBS_SRV_IP, pbBuf, sizeof(gSYS.stLWM2M_SET.bBS_SRV_IP) );
   LWM2M_CC1350_SET_SendResponse( LWM2M_ERROR_OK );
}

void LWM2M_CC1350_SET_SRV_PORT( uint8_t *pbBuf, uint32_t uiLen )
{
   if (uiLen!= sizeof(gSYS.stLWM2M_SET.wBS_SRV_PORT))
   {
      LWM2M_CC1350_SET_SendResponse( LWM2M_ERROR_PARAM );
      return;
   }
   gSYS.stLWM2M_SET.wSRV_PORT   = *pbBuf++;
   gSYS.stLWM2M_SET.wSRV_PORT <<= 8;
   gSYS.stLWM2M_SET.wSRV_PORT  |= *pbBuf;
   LWM2M_CC1350_SET_SendResponse( LWM2M_ERROR_OK );
}

void LWM2M_CC1350_SET_CLI_NAME( uint8_t *pbBuf, uint32_t uiLen )
{
   if (uiLen>= sizeof(gSYS.stLWM2M_SET.strCLI_NAME))
   {
      LWM2M_CC1350_SET_SendResponse( LWM2M_ERROR_PARAM );
      return;
   }
   pbBuf[uiLen] = 0;                                  // Null-Byte als String-Ende anhängen
   memcpy( gSYS.stLWM2M_SET.strCLI_NAME, pbBuf, uiLen+1 );
   LWM2M_CC1350_SET_SendResponse( LWM2M_ERROR_OK );
}


//==============================================================================================================================
//    Get-CMD von CC1350: Daten aus gSYS auslesen und an CC1350 übertragen
//==============================================================================================================================
void LWM2M_Interpret_CFG_GET( uint8_t *pbBuf, uint32_t uiLen)
{
   switch ( *pbBuf++ )
   {
   case LWM2M_CFG_ID_BS_SVR_IP:
      LWM2M_SET_BS_SRV_IP( true );
      break;
   case LWM2M_CFG_ID_BS_SVR_PORT:
      LWM2M_SET_BS_SRV_PORT( true );
      break;
   case LWM2M_CFG_ID_SVR_IP:
      LWM2M_SET_SRV_IP( true );
      break;
   case LWM2M_CFG_ID_SVR_PORT:
      LWM2M_SET_SRV_PORT( true );
      break;
   case LWM2M_CFG_ID_CLI_NAME:
      LWM2M_SET_CLI_NAME( true );
      break;
   }
}

//==============================================================================================================================
//    Set-CMD von CC1350: Daten in gSYS speichern und response gernerieren
//==============================================================================================================================
void LWM2M_Interpret_CFG_SET( uint8_t *pbBuf, uint32_t uiLen)
{
   switch ( *pbBuf++ )
   {
   case LWM2M_CFG_ID_BS_SVR_IP:
      LWM2M_CC1350_SET_BS_SRV_IP( pbBuf, uiLen -1 );
      break;
   case LWM2M_CFG_ID_BS_SVR_PORT:
      LWM2M_CC1350_SET_BS_SRV_PORT( pbBuf, uiLen -1 );
      break;
   case LWM2M_CFG_ID_SVR_IP:
      LWM2M_CC1350_SET_SRV_IP( pbBuf, uiLen -1 );
      break;
   case LWM2M_CFG_ID_SVR_PORT:
      LWM2M_CC1350_SET_SRV_PORT( pbBuf, uiLen -1 );
      break;
   case LWM2M_CFG_ID_CLI_NAME:
      LWM2M_CC1350_SET_CLI_NAME( pbBuf, uiLen -1 );
      break;
   }
}

//==============================================================================================================================
// Dies Funktion berarbeitet den LWM2M_RD_RQ
//==============================================================================================================================
//  pbBuf:                 stehen die vom RD-Rquest empfangenen Daten
//  uiLen:                 größe des übergebenen Buffers
//==============================================================================================================================
void LWM2M_InterpretRD_REQ(uint8_t *pbBuf, uint32_t uiLen)
{

   ST_LWM2M_IDs_t stID;

   uint8_t   mybTxBuf[ LWM2M_MAX_OBJECT_SIZE ];
   uint8_t  *mypTxBuf = mybTxBuf;

   int32_t   iTxLen = 1;                              // TX-Länge ist mindestens 1
   *mypTxBuf++      = LWM2M_WR_REQ;                   // E1 Next-Layer CMD hinzufügen

   int32_t   iLen   = -1;
//   pbBuf++;                                           // CMD ist bereits interpretiert und wird nun übersprungen
//   uiLen--;

   while(uiLen && iTxLen<LWM2M_MAX_OBJECT_SIZE-6)      // solange weitere Objekt-anfragen im RxBuffer sind und noch Platz im TxBuffer ist
   {
      memcpy( mypTxBuf, pbBuf, 5);                    // Erstmal OBJ-ID, INST-ID, RES-ID in tx-Buffer kopieren
      iTxLen   += 5;
      mypTxBuf += 5;

      stID.wObjectID     = *pbBuf++;                  // MSB
      stID.wObjectID   <<= 8;
      stID.wObjectID    |= *pbBuf++;                  // LSB

      stID.bInstanceID   = *pbBuf++;                  // nur ein Byte

      stID.wResourceID   = *pbBuf++;                  // MSB
      stID.wResourceID <<= 8;
      stID.wResourceID  |= *pbBuf++;                  // LSB

      uiLen -= 5;

      iLen = LWM2M_GetItem( &stID, mypTxBuf, LWM2M_MAX_OBJECT_SIZE - iTxLen );
      if (iLen>0)
      {
         iTxLen   += iLen;
         mypTxBuf += iLen;
      }
      else
      {
         iTxLen   -= 5;
         mypTxBuf -= 5;
         break;                                       // Objekt nicht gefunden oder Fehler bei der Datenaufbereitung (z. B. TX-Buffer zu klein)
      }
   }

   if (iTxLen>0)
   {                                                  // Antwort senden (steht im gleichen Buffer, der vom Empfang zur Verfügung gestellt wurde)
      while ( Radio_Send_Record( LWM2M_WR_REQ, mybTxBuf, iTxLen ) )
         __no_operation();                            // senden wiederholen bis Datensatz in FiFo passt ?!?
   }
   else
   {
      mybTxBuf[0] = LWM2M_RD_RSP;
      mybTxBuf[1] = iLen;                             // Fehler zurückmelden
      Radio_Send_Record( LWM2M_LWM2M_CMD, mybTxBuf, 1 );
   }
}

//==============================================================================================================================
// Dies Funktion berarbeitet den LWM2M_WR_RQ
//==============================================================================================================================
//  pbBuf:                 stehen die vom WD-Rquest empfangenen Daten
//  uiLen:                 größe des übergebenen Buffers
//==============================================================================================================================
void LWM2M_InterpretWR_REQ(uint8_t *pbBuf, uint32_t uiLen)
{
   ST_LWM2M_IDs_t stID;
   int32_t iSetResultVal = 0;

//   pbBuf++;                                           // CMD ist bereits interpretiert und wird nun übersprungen
//   uiLen--;
//
   while(uiLen && iSetResultVal >=0 )                 // solange weitere Objekt-Daten im RxBuffer sind und es kein Fehler gegeben hat
   {
      stID.wObjectID     = *pbBuf++;                  // MSB
      stID.wObjectID   <<= 8;
      stID.wObjectID    |= *pbBuf++;                  // LSB

      stID.bInstanceID   = *pbBuf++;                  // nur ein Byte

      stID.wResourceID   = *pbBuf++;                  // MSB
      stID.wResourceID <<= 8;
      stID.wResourceID  |= *pbBuf++;                  // LSB

      uiLen -= 5;
                                                      // Zurückgegeben wird die Länge der im Buf verarbeiteten Zeichen. Zusätzlich wird in uiError ein Fehlerkode zurückgegeben
      iSetResultVal = LWM2M_SetItem( &stID, pbBuf, uiLen );
      if ( iSetResultVal > 0 )
      {
         uiLen -= iSetResultVal;                      // Anzahl der bereits verarbeiteten Zeichen aus dem pbBuf abziehen
         pbBuf += iSetResultVal;                      // pbBuf auf nächsten Teil stellen
      }
   }

   if (iSetResultVal>0)
      iSetResultVal = 0;                              // alles ok Melden

   uint8_t mybTxBuf[1] = { iSetResultVal };           // iSetResultVal: 0=ok sonst Fehler-Nr in Antwort einbauen
   Radio_Send_Record( LWM2M_WR_RSP, mybTxBuf, 1 );
}

//==============================================================================================================================
//   LWM2M Next-Layer CMD
//==============================================================================================================================
void LWM2M_InterpretCMD( uint8_t *pbBuf, uint32_t uiLen )
{
   uiLen--;                                           // wir interpretieren gerade ein Zeichen dieses also von uiLen abziehen
   switch (*pbBuf++)
   {
   case LWM2M_RETURN:
      guiLWM2M_Status = *pbBuf;
      break;

   case LWM2M_CFG_SET:                                // wir sollen neue CC1350-Parameter in gSYS-Speichern -> Response mit Error-Code zusammenbauen
      LWM2M_Interpret_CFG_SET( pbBuf, uiLen);
      break;
   case LWM2M_CFG_GET:                                // wir sollen die derzeitigen Setting's senden -> Response mit Daten zusammenbauen
      LWM2M_Interpret_CFG_GET( pbBuf, uiLen);
      break;

   case LWM2M_STATUS_RET:
      guiLWM2M_Status = *pbBuf;
      break;

   case LWM2M_OBJ_RET:
      guiLWM2M_Status = pbBuf[3];
      break;

   case LWM2M_RES_RET:
      guiLWM2M_Status = pbBuf[5];
      break;

   case LWM2M_RD_REQ:
      LWM2M_InterpretRD_REQ(pbBuf, uiLen);            // wir erhalten einen RD-Request und reagieren mit einem WR-Request
      break;
   //case LWM2M_RD_RSP:

   case LWM2M_WR_REQ:                                 // Wir erhalten einen WR-Request und generieren einen Status-Response
      LWM2M_InterpretWR_REQ(pbBuf, uiLen);
      break;
   }
}

//==============================================================================================================================
//    Hier die GAS-Config-SET-Funktionen
//==============================================================================================================================
void LWM2M_GAS_CFG_SET_MACADDR( uint32_t bIsResponse )
{
   uint8_t bTxBuf[1+sizeof(gSYS.stCC1350_SET.bMAC_ADR)];
   bTxBuf[0] = GAS_CFG_ID_MACADDR;
   memcpy( &bTxBuf[1], gSYS.stCC1350_SET.bMAC_ADR, sizeof(gSYS.stCC1350_SET.bMAC_ADR) );
   uint8_t bCMD = (bIsResponse) ? LWM2M_GAS_CFG_RES : LWM2M_GAS_CFG_SET;
   Radio_Send_Record( bCMD, bTxBuf, sizeof(bTxBuf) );
}

void LWM2M_GAS_CFG_SET_PANID  ( uint32_t bIsResponse )
{
   uint8_t bTxBuf[1+2];
   bTxBuf[0] = GAS_CFG_ID_PAN;
   bTxBuf[1] = gSYS.stCC1350_SET.wPANID >> 8;
   bTxBuf[2] = gSYS.stCC1350_SET.wPANID ;
   uint8_t bCMD = (bIsResponse) ? LWM2M_GAS_CFG_RES : LWM2M_GAS_CFG_SET;
   Radio_Send_Record( bCMD, bTxBuf, sizeof(bTxBuf) );
}

void LWM2M_GAS_CFG_SET_OPMODE ( uint32_t bIsResponse )
{
   uint8_t bTxBuf[2];
   bTxBuf[0] = GAS_CFG_ID_OPMODE;
   bTxBuf[1] = gSYS.stCC1350_SET.bOPMODE;
   uint8_t bCMD = (bIsResponse) ? LWM2M_GAS_CFG_RES : LWM2M_GAS_CFG_SET;
   Radio_Send_Record( bCMD, bTxBuf, sizeof(bTxBuf) );
}

void LWM2M_GAS_CFG_SET_CHANNEL( uint32_t bIsResponse )
{
   uint8_t bTxBuf[2];
   bTxBuf[0] = GAS_CFG_ID_CHANNEL;
   bTxBuf[1] = gSYS.stCC1350_SET.bCHANNEL;
   uint8_t bCMD = (bIsResponse) ? LWM2M_GAS_CFG_RES : LWM2M_GAS_CFG_SET;
   Radio_Send_Record( bCMD, bTxBuf, sizeof(bTxBuf) );
}

//==============================================================================================================================
//    CC1310 hat neue Config-Daten,  diese nun speichern und antwort generieren
//==============================================================================================================================
void LWM2M_GAS_NEW_CFG_Response( uint8_t bError )
{
   Radio_Send_Record( LWM2M_GAS_CFG_RES, &bError, 1 );
}

void LWM2M_GAS_NEW_CFG_MACADDR( uint8_t *pbBuf, uint32_t uiLen )
{
   if ( uiLen != sizeof(gSYS.stCC1350_SET.bMAC_ADR) )
   {
      LWM2M_GAS_NEW_CFG_Response( LWM2M_ERROR_PARAM );
      return;
   }

   memcpy( gSYS.stCC1350_SET.bMAC_ADR, pbBuf, sizeof(gSYS.stCC1350_SET.bMAC_ADR) );
   LWM2M_GAS_NEW_CFG_Response( LWM2M_ERROR_OK );
}

void LWM2M_GAS_NEW_CFG_PANID( uint8_t *pbBuf, uint32_t uiLen )
{
   if ( uiLen != sizeof(gSYS.stCC1350_SET.wPANID) )
   {
      LWM2M_GAS_NEW_CFG_Response( LWM2M_ERROR_PARAM );
      return;
   }

   gSYS.stCC1350_SET.wPANID   = *pbBuf++;
   gSYS.stCC1350_SET.wPANID <<= 8;
   gSYS.stCC1350_SET.wPANID  |= *pbBuf;
   LWM2M_GAS_NEW_CFG_Response( LWM2M_ERROR_OK );
}

void LWM2M_GAS_NEW_CFG_OPMODE( uint8_t *pbBuf, uint32_t uiLen )
{
   gSYS.stCC1350_SET.bOPMODE = *pbBuf;
   LWM2M_GAS_NEW_CFG_Response( LWM2M_ERROR_OK );
}

void LWM2M_GAS_NEW_CFG_CHANNEL( uint8_t *pbBuf, uint32_t uiLen )
{
   gSYS.stCC1350_SET.bCHANNEL = *pbBuf;
   LWM2M_GAS_NEW_CFG_Response( LWM2M_ERROR_OK );
}

//==============================================================================================================================
//    Hier die GAS-Config-GET-Funktionen antriggern (Anfrage an CC1310)
//==============================================================================================================================
void LWM2M_GAS_CFG_GET_MACADDR( void )
{
   uint8_t bTxBuf[1];
   bTxBuf[0] = GAS_CFG_ID_MACADDR;
   Radio_Send_Record( LWM2M_GAS_CFG_GET, bTxBuf, sizeof(bTxBuf) );
}

void LWM2M_GAS_CFG_GET_PANID  ( void )
{
   uint8_t bTxBuf[1];
   bTxBuf[0] = GAS_CFG_ID_PAN;
   Radio_Send_Record( LWM2M_GAS_CFG_GET, bTxBuf, sizeof(bTxBuf) );
}

void LWM2M_GAS_CFG_GET_OPMODE ( void )
{
   uint8_t bTxBuf[1];
   bTxBuf[0] = GAS_CFG_ID_OPMODE;
   Radio_Send_Record( LWM2M_GAS_CFG_GET, bTxBuf, sizeof(bTxBuf) );
}

void LWM2M_GAS_CFG_GET_CHANNEL( void )
{
   uint8_t bTxBuf[1];
   bTxBuf[0] = GAS_CFG_ID_CHANNEL;
   Radio_Send_Record( LWM2M_GAS_CFG_GET, bTxBuf, sizeof(bTxBuf) );
}

//==============================================================================================================================
//  GAS: GET/RET-Interpreter GET-Anfrage mit Response und Daten antworten
//==============================================================================================================================
void LWM2M_GAS_Interpret_CFG_GET( uint8_t *pbBuf, uint32_t uiLen)
{
   switch (*pbBuf)
   {
   case GAS_CFG_ID_MACADDR:
      LWM2M_GAS_CFG_SET_MACADDR( true );
      break;
   case GAS_CFG_ID_PAN:
      LWM2M_GAS_CFG_SET_PANID( true );
      break;
   case GAS_CFG_ID_OPMODE:
      LWM2M_GAS_CFG_SET_OPMODE( true );
      break;
   case GAS_CFG_ID_CHANNEL:
      LWM2M_GAS_CFG_SET_CHANNEL( true );
      break;
   }
}

//==============================================================================================================================
//  GAS: SET-Interpreter SET-mit neuen Config-Daten: Diese nun speichern und Response generieren
//==============================================================================================================================
void LWM2M_GAS_Interpret_CFG_SET( uint8_t *pbBuf, uint32_t uiLen)
{
   switch (*pbBuf++)
   {
   case GAS_CFG_ID_MACADDR:
      LWM2M_GAS_NEW_CFG_MACADDR( pbBuf, uiLen-1 );
      break;
   case GAS_CFG_ID_PAN:
      LWM2M_GAS_NEW_CFG_PANID( pbBuf, uiLen-1 );
      break;
   case GAS_CFG_ID_OPMODE:
      LWM2M_GAS_NEW_CFG_OPMODE( pbBuf, uiLen-1 );
      break;
   case GAS_CFG_ID_CHANNEL:
      LWM2M_GAS_NEW_CFG_CHANNEL(pbBuf, uiLen-1 );
      break;
   }
}

//==============================================================================================================================
//==============================================================================================================================
int32_t LWM2M_BuildObjektUartBuf( uint8_t *pbBuf, uint32_t uiLen, uint16_t uiObjID, uint8_t uiInstID, uint16_t uiResID )
{
   if (uiLen<7)
      return -10;

   ST_LWM2M_IDs_t stID;
   stID.wObjectID    = uiObjID;
   stID.bInstanceID  = uiInstID;
   stID.wResourceID  = uiResID;
   *pbBuf++ = stID.wObjectID   >> 8;
   *pbBuf++ = stID.wObjectID;
   *pbBuf++ = stID.bInstanceID;
   *pbBuf++ = stID.wResourceID >> 8;
   *pbBuf++ = stID.wResourceID;
   uiLen -= 5;

   int32_t iRetVal = LWM2M_GetItem(&stID, pbBuf, uiLen);
   if (iRetVal < 0)
      return iRetVal;
   else
      return iRetVal +5;
}



//==============================================================================================================================
//  Minimale Buffer-Größe: Header 5-Bytes, n*(3+Resource-Daten-Länge)
//==============================================================================================================================
int16_t LWM2M_BuildInstanceWR_UartBuf( uint8_t *pbBuf, int16_t iLen, uint8_t bGenHeader, uint16_t uiObjID, uint8_t uiInstID,
                                       const uint16_t uiResIDs[], uint16_t uiNumResources )
{
    int16_t iByteCount = 0;                             // hier die Bytes im Buffer zählen

    if (bGenHeader)
    {
        if (iLen<5)
            return -1;

        *pbBuf++ = LWM2M_INST_WR_REQ;
        *pbBuf++ = uiObjID >> 8;
        *pbBuf++ = uiObjID;
        *pbBuf++ = uiInstID;
        *pbBuf++ = uiNumResources;
        iByteCount = 5;                                 // bereits 5 Zeichen in Buffer übertragen
    }

    ST_LWM2M_IDs_t stID;
    stID.wObjectID    = uiObjID;
    stID.bInstanceID  = uiInstID;

    for (uint16_t i=0;
        i            <  uiNumResources &&               // solange noch Resourcen zu übertragen sind
        iByteCount+4 <  iLen           &&               // solange noch Platz im übergebenen Buffer ist
        iByteCount   >= 0              ;                // solange das vorrige übertragen der Daten geklappt hat
        i++)                                            // am Ende der Schleife i = Resourcen-Index erhöhen
    {
        stID.wResourceID = uiResIDs[i];
                                                        // länge der resource in iRetVal speichern
        int16_t iRetVal = LWM2M_GetItem(&stID, &pbBuf[3], iLen-iByteCount-3);
        if (iRetVal>=0)
        {                                               // wenn das übertragen der Resource-Daten geklappt, hat dann den Resourcen-Header hinzufügen
            *pbBuf++    = stID.wResourceID >> 8;
            *pbBuf++    = stID.wResourceID;
            *pbBuf++    = iRetVal;                      // Anzahl der Bytes dieser Resource in Buffer übertragen
            pbBuf      += iRetVal;                      // Zeiger um die Anzahl der Bytes der Resource weiter verschieben
            iByteCount += iRetVal + 3;
        }
        else
           iByteCount = -2;
    }
    return iByteCount;
}

//==============================================================================================================================
// Setzt die Modulvariablen fürs Anlegen der Objekte entsprechend
//==============================================================================================================================
void LWM2M_startCreateObjects(void)
{
   muiObjectIndex     = 0;
   mbInstanceIndex    = 0;
   muiResourceIndex   = 0;
   muiResourceCounter = 0;
}

//==============================================================================================================================
// Hier werden die Objecte im Funkmodul angelegt.
//==============================================================================================================================
//    Ret = 0     => weitere Objecte vorhanden
//    Ret = 1     => Object wurde Angelegt -> Resourcen anlegen
//    Ret = 2     => keine weiteren Objecte vorhanden
//==============================================================================================================================
int32_t LWM2M_CreateNextObject()
{
   if ( mbInstanceIndex > stLWM2M_ObjList[muiObjectIndex].bMaxInstID)
   {
      muiResourceIndex = 0;
      mbInstanceIndex  = 0;
      muiObjectIndex++;
   }

   if ( muiObjectIndex>=LWM2M_ObjList_COUNT )
   {
      muiObjectIndex   = 0;
      mbInstanceIndex  = 0;
      muiResourceIndex = 0;
      guiLWM2M_Status  = 0;
      return 2;
   }

   if(stLWM2M_ObjList[muiObjectIndex].uiObjFlags == LWM2M_OBJ_FLAG_NORMAL_OBJ)
   {
      uint8_t bBuffer[4];

      bBuffer[0] = LWM2M_OBJ_CREATE;
      bBuffer[1] = (uint8_t)(stLWM2M_ObjList[muiObjectIndex].wObjectID >> 8);
      bBuffer[2] = (uint8_t) stLWM2M_ObjList[muiObjectIndex].wObjectID;
      bBuffer[3] = mbInstanceIndex;
      Radio_Send_Record( LWM2M_LWM2M_CMD, bBuffer, sizeof(bBuffer) );
   }
   else
   {
      guiLWM2M_Status  = 0;
      muiResourceIndex = 0;
      mbInstanceIndex  = 0;
      muiObjectIndex++;
      if ( muiObjectIndex>=LWM2M_ObjList_COUNT )
      {
         muiObjectIndex=0;
         return 2;                                    // alle Objekte/Instanzen wurden angelegt -> nächster Schritt in State-Maschine
      }
      else
      {
         return 0;
      }
   }
   return 1;
}

//==============================================================================================================================
//  Hier werden die Resourcen zum Object im Funkmodul angelegt
//==============================================================================================================================
//    Ret = 0     => weitere Resourcen vorhanden
//    Ret = -1    => alle Resourcen des Objects angelegt - weitere Objecte/Instanzen vorhanden
//    Ret = 1     => alle Resourcen des Objects angelegt - keine weiteren Objecte/Instanzen vorhanden
//==============================================================================================================================
int32_t LWM2M_CreateNextResource()
{
   uint8_t bBuffer[9];

   if ((stLWM2M_ObjList[muiObjectIndex].pItems->pstItems[muiResourceIndex].uiTypeFLGs & LWM2M_DONT_CREATE_RESOURCE)==0)
   {
      bBuffer[0] = LWM2M_RES_CREATE;
      bBuffer[1] = (uint8_t)(stLWM2M_ObjList[muiObjectIndex].wObjectID >> 8);
      bBuffer[2] = (uint8_t) stLWM2M_ObjList[muiObjectIndex].wObjectID;
      bBuffer[3] = mbInstanceIndex;
      bBuffer[4] = (uint8_t)(stLWM2M_ObjList[muiObjectIndex].pItems->pstItems[muiResourceIndex].wResourceID >> 8);
      bBuffer[5] = (uint8_t) stLWM2M_ObjList[muiObjectIndex].pItems->pstItems[muiResourceIndex].wResourceID ;
      bBuffer[6] =           stLWM2M_ObjList[muiObjectIndex].pItems->pstItems[muiResourceIndex].uiTypeFLGs & LWM2M_TYPE_MASK;

      if(bBuffer[6] != LWM2M_TYPE_STRING)
      {
         bBuffer[7] = (stLWM2M_ObjList[muiObjectIndex].pItems->pstItems[muiResourceIndex].uiTypeFLGs & LWM2M_LEN_MASK) >> 4;
      }
      else
      {
         bBuffer[7] = LWM2M_MAX_STRING_LEN(stLWM2M_ObjList[muiObjectIndex].pItems->pstItems[muiResourceIndex].uiTypeFLGs);
         if ( bBuffer[7]==0 )
         {
            bBuffer[7] = strlen((const char*)(stLWM2M_ObjList[muiObjectIndex].pItems->pstItems[muiResourceIndex].pvData)) + 1;
         }
      }
      bBuffer[8] = ((stLWM2M_ObjList[muiObjectIndex].pItems->pstItems[muiResourceIndex].uiTypeFLGs & LWM2M_ACCESS_MASK) >> 8);
      Radio_Send_Record( LWM2M_LWM2M_CMD, bBuffer, sizeof(bBuffer) );
      muiResourceCounter++;
   }
   else
      guiLWM2M_Status = 0;

   muiResourceIndex++;
   if (muiResourceIndex >= stLWM2M_ObjList[muiObjectIndex].pItems->uiItemCount)
   {
      muiResourceIndex = 0;
      mbInstanceIndex++;
      if ( mbInstanceIndex > stLWM2M_ObjList[muiObjectIndex].bMaxInstID)
      {
         mbInstanceIndex = 0;
         muiObjectIndex++;
         if ( muiObjectIndex>=LWM2M_ObjList_COUNT )
         {
            muiObjectIndex  = 0;
            return 1;                                 // alle Objekte/Instanzen wurden angelegt -> nächster Schritt in State-Maschine
         }
      }
      return -1;                                      // weitere Objecte/Instanzen vorhanden -> Step-Back to create Object/Instanze
   }
   return 0;
}

//==============================================================================================================================
//    Hier alle Resourcen initilisieren.
//    muiObjectIndex, mbInstanceIndex,  muiResourceIndex sollten beim ersten Aufruf 0 sein
//==============================================================================================================================
//    Ret = 0     => weitere Resourcen vorhanden
//    Ret = 1     => alle Resourcen wurden initialisiert
//==============================================================================================================================
int32_t  LWM2M_InitNextResource()
{
   uint8_t bBuffer[255];

   bBuffer[0] = LWM2M_WR_REQ;                         // erstmal das schreib-CMD in Buffer

   int32_t iLen=0;
   int32_t iRetValue=0;
   do
   {
      if( (stLWM2M_ObjList[muiObjectIndex].uiObjFlags == LWM2M_OBJ_FLAG_NORMAL_OBJ) &&
          (stLWM2M_ObjList[muiObjectIndex].pItems->pstItems[muiResourceIndex].uiTypeFLGs & LWM2M_DONT_INIT_RESOURCE)==0)
         iLen = LWM2M_BuildObjektUartBuf( &bBuffer[1], sizeof(bBuffer)-1,
                                      stLWM2M_ObjList[muiObjectIndex].wObjectID, mbInstanceIndex,
                                      stLWM2M_ObjList[muiObjectIndex].pItems->pstItems[muiResourceIndex].wResourceID);
      muiResourceIndex++;
      if (muiResourceIndex >= stLWM2M_ObjList[muiObjectIndex].pItems->uiItemCount)
      {
         muiResourceIndex = 0;
         mbInstanceIndex++;
         if ( mbInstanceIndex > stLWM2M_ObjList[muiObjectIndex].bMaxInstID)
         {
            mbInstanceIndex = 0;
            muiObjectIndex++;

            while ( muiObjectIndex<LWM2M_ObjList_COUNT  &&
                    stLWM2M_ObjList[muiObjectIndex].uiObjFlags != LWM2M_OBJ_FLAG_NORMAL_OBJ )
               muiObjectIndex++;

            if ( muiObjectIndex>=LWM2M_ObjList_COUNT )
            {
               muiObjectIndex=0;
               iRetValue = 1;
               break;                                 // alle Resourcen nun mit Werten geladen -> Schleife verlassen
            }
         }
      }
   }  while (iLen<1);

   if (iLen>0)
   {                                                   // wegen schreib-CMD iRetVal+1
      Radio_Send_Record(LWM2M_LWM2M_CMD, bBuffer, iLen+1);
   }
   return iRetValue;
}
