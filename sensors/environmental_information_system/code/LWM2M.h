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
 * FileName:            LWM2M.h
 * Dependencies:        See INCLUDES section below
 * Processor:           Apollo
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Bernd Ehrbrecht		08.03.2017
 ********************************************************************/

#ifndef __LWM2M__
#define __LWM2M__

#define LWM2M_MAX_OBJECT_SIZE       (5+32+10)         // 5 Bytes-Objekt-ID's, 32 max-Payload-Bytes, 10 Bytes reserve

                                    //bbbb::86eb:18ff:feed:7e67
                                    //1     2     3     4     5     6     7     8     9     10    11    12    13    14    15    16
#define LWM2M_DEFAULT_BS_SRV_IP     { 0xbb, 0xbb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x86, 0xeb, 0x18, 0xff, 0xfe, 0xca, 0x7e, 0x67 }
#define LWM2M_DEFAULT_SRV_IP        { 0xbb, 0xbb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0xA3, 0x16, 0xff, 0xfe, 0xb5, 0xd1, 0xA6 }

//                                   12345678901234567890123456789012
#define LWM2M_DEFAULT_CLI_NAME      "emb6.lwm2m_eis_bernd"           // max 32-Stellen

#define LWM2M_DEFAULT_BS_SRV_PORT      5683
//#define LWM2M_DEFAULT_SRV_PORT         OPC_UA_SVR_PORT           // LESHAN_SVR_PORT or OPC_UA_SVR_PORT
#define LWM2M_DEFAULT_SRV_PORT         LESHAN_SVR_PORT           // LESHAN_SVR_PORT or OPC_UA_SVR_PORT

//#define LWM2M_DEFAULT_SRV_PORT         0xDD07         // Max 16 stellen

#define CC1350_DEFAULT_MAC_ADR         { 0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x00 }
#define CC1350_DEFAULT_PANID           0xABCD
#define CC1350_DEFAULT_OPMODE          0x01
#define CC1350_DEFAULT_CHANNEL         26


#define OPC_UA_SVR_PORT                5683           // 0x1633
#define LESHAN_SVR_PORT                56583          // 0xDD07

//===========================================================================================================
//   LWM2M-Object-ID's
//===========================================================================================================
#define Si1132_VIS_OBJ_ID               3301
#define BME280_TEMP_OBJ_ID              3303
#define BME280_HUMITY_OBJ_ID            3304
#define BME280_PRESSURE_OBJ_ID          3315

#define FLOW_X_OBJ_ID                   3601
#define FLOW_Y_OBJ_ID                   3602

#define BME280_INI_OBJ_ID              10010
#define Si1132_INI_OBJ_ID              10011

#define Si1132_IR_OBJ_ID               10013
#define Si1132_UV_OBJ_ID               10014

//===========================================================================================================
//   LWM2M-Resource-IDs
//===========================================================================================================
#define LWM2M_MIN_SENS_VALUE            5601
#define LWM2M_MAX_SENS_VALUE            5602
#define LWM2M_MIN_SENS_RANGE            5603
#define LWM2M_MAX_SENS_RANGE            5604

#define LWM2M_CUR_SENS_VALUE            5700
#define FESTO_INDEX_RES_ID              5700

#define MinLogValue_RES_ID              9980
#define MaxLogValue_RES_ID              9981
#define DifLogValue_RES_ID              9982
#define LWM2M_IDEL_TIME_RES_ID          9983
#define LWM2M_STATUS_RES_ID             9984

#define Si1132_AVG_RES_ID               9985          // Anzahl der Werte über die gemittelt werden soll

#define FESTO_SET_MODE_RES_ID          10000
#define FESTO_SET_TEACH_CYCLE_RES_ID   10001          // Anzahl der Teach-Cycles
#define FESTO_SET_CHECKS_ADAY_RES_ID   10002          // 10 Checks a Day
#define FESTO_SET_DATA_POINTS_RES_ID   10003          // Data-Poinst = Samples pro Kolbenhub 500 programmed
#define FESTO_SET_SAMPLE_RATE_RES_ID   10004          // 1000 Hz
#define FESTO_SET_COMP_POS1_RES_ID     10005          // Komperator-Setting für Pos1
#define FESTO_SET_COMP_POS2_RES_ID     10006          // Komperator-Setting für Pos2

//#define FESTO_SET_L2R_HA1_IDX_RES_ID   10007
//#define FESTO_SET_L2R_HA2_IDX_RES_ID   10008
//#define FESTO_SET_L2R_HA3_IDX_RES_ID   10009
//
//#define FESTO_SET_R2L_HA1_IDX_RES_ID   10010
//#define FESTO_SET_R2L_HA2_IDX_RES_ID   10011
//#define FESTO_SET_R2L_HA3_IDX_RES_ID   10012

#define FESTO_EXEC_PROMGRAMM_RES_ID    10015

#define FESTO_STATUS_RES_ID            10016

#define FESTO_GET_L2R_MIN_RES_ID       10020
#define FESTO_GET_L2R_MAX_RES_ID       10021
#define FESTO_GET_R2L_MIN_RES_ID       10022
#define FESTO_GET_R2L_MAX_RES_ID       10023

#define FESTO_POLY_OFFSET_RES_ID       10025
#define FESTO_POLY_LINEAR_RES_ID       10026
#define FESTO_POLY_QUADRAT_RES_ID      10027
#define FESTO_POLY_CUBIC_RES_ID        10028
#define FESTO_POLY_X4_RES_ID           10029
#define FESTO_POLY_X5_RES_ID           10030
#define FESTO_POLY_ADC_MIN_RES_ID      10031
#define FESTO_POLY_ADC_MAX_RES_ID      10032

//===========================================================================================================
// GAS = Genaral API Set siehe Kapitel 3.4 in emb6_serial_api TBD: habe mal Dummywerte genommen
//===========================================================================================================
//#define LWM2M_INIT                     0xF3
#define LWM2M_GAS_RET                  0x00
#define LWM2M_GAS_PING                 0x10

#define LWM2M_GAS_CFG_SET              0x20
#define LWM2M_GAS_CFG_GET              0x21
#define LWM2M_GAS_CFG_RES              0x22

#define LWM2M_GAS_DEVICE_STOP          0x30
#define LWM2M_GAS_DEVICE_START         0x31

#define LWM2M_GAS_STATUS_RET           0x41

//===========================================================================================================
//===========================================================================================================
#define LWM2M_RETURN                   0x00

#define LWM2M_CFG_SET                  0x20
#define LWM2M_CFG_GET                  0x21
#define LWM2M_CFG_RES                  0x22

#define LWM2M_STOP                     0x30
#define LWM2M_START                    0x31

#define LWM2M_STATUS_RET               0x41

#define LWM2M_OBJ_CREATE              (0x60)
#define LWM2M_OBJ_CREATE_XML          (0x61)
#define LWM2M_OBJ_RET                 (0x62)
#define LWM2M_OBJ_DEL                 (0x63)
#define LWM2M_RES_CREATE              (0x70)
#define LWM2M_RES_RET                 (0x72)
#define LWM2M_RES_DEL                 (0x73)

#define LWM2M_LWM2M_CMD                0xE1

#define LWM2M_RD_REQ                   0x80           // war 0xFC
#define LWM2M_RD_RSP                   0x81           // war 0xFD
#define LWM2M_WR_REQ                   0x82           // war 0xFE
#define LWM2M_WR_RSP                   0x83           // war 0xFF

#define LWM2M_INST_RD_REQ              0x90           // war 0xFC
#define LWM2M_INST_RD_RSP              0x91           // war 0xFD
#define LWM2M_INST_WR_REQ              0x92           // war 0xFE
#define LWM2M_INST_WR_RSP              0x93           // war 0xFF


//===========================================================================================================
//    GAS-Config-ID's
//===========================================================================================================
#define GAS_CFG_ID_MACADDR             0x00
#define GAS_CFG_ID_PAN                 0x01
#define GAS_CFG_ID_OPMODE              0x02
#define GAS_CFG_ID_CHANNEL             0x03

//===========================================================================================================
//    LWM2M-CFG-ID's
//===========================================================================================================
#define LWM2M_CFG_ID_BS_SVR_IP         0x00
#define LWM2M_CFG_ID_BS_SVR_PORT       0x01
#define LWM2M_CFG_ID_SVR_IP            0x02
#define LWM2M_CFG_ID_SVR_PORT          0x03
#define LWM2M_CFG_ID_CLI_NAME          0x04

//===========================================================================================================
//   Status and Return Codes
//===========================================================================================================
#define LWM2M_ERROR_OK                 0x00
#define LWM2M_ERROR_ERROR              0x01
#define LWM2M_ERROR_CMD                0x02
#define LWM2M_ERROR_PARAM              0x03
#define LWM2M_STATUS_STOPPED           0x30
#define LWM2M_STATUS_STARTED           0x31
#define LWM2M_STATUS_NETWORK           0x32
#define LWM2M_STATUS_ERROR             0x3E
#define LWM2M_STATUS_UDEF              0x3F

typedef struct ST_LWM2M_CFG
{
   uint8_t  bBS_SRV_IP[16];                           // 2001::AA IP address of the LWM2M bootstrap server.*
   uint16_t wBS_SRV_PORT;                             // 5683 Port of the LWM2M bootstrap server.*
   uint8_t  bSRV_IP[16];                              // 2001::BB IP address of the LWM2M bootstrap server.*
   uint16_t wSRV_PORT;                                // 5683 Port of the LWM2M bootstrap server.*
   char     strCLI_NAME[33];                          // Name of the LWM2M client. The server instance will use this name for identification
}  ST_LWM2M_CFG_t;

typedef struct ST_CC1350_GENERAL_API_SET
{
   uint8_t  bMAC_ADR[6];                              // GAS-Parameter
   uint16_t wPANID;
   uint8_t  bOPMODE;
   uint8_t  bCHANNEL;
}  ST_CC1350_GENERAL_API_SET_t;

// Entfernt da in IPSO nicht vorhanden
//#define LWM2M_TYPE_UINT8            0x00000011
//#define LWM2M_TYPE_UINT16           0x00000022
//#define LWM2M_TYPE_UINT32           0x00000043
//#define LWM2M_TYPE_INT8             0x00000014
//#define LWM2M_TYPE_INT16            0x00000025
//#define LWM2M_TYPE_DOUBLE           0x00000088

#define LWM2M_TYPE_INT32            0x00000041
#define LWM2M_TYPE_FLOAT            0x00000042
#define LWM2M_TYPE_STRING           0x00000003
#define LWM2M_TYPE_UNKNOWN          0x0000000F
#define LWM2M_TYPE_MASK             0x0000000F
#define LWM2M_LEN_MASK              0x000000F0

#define LWM2M_ACCESS_MASK           0x00000F00
#define LWM2M_ACCESS_RD             0x00000100
#define LWM2M_ACCESS_WR             0x00000200
#define LWM2M_ACCESS_RD_WR          0x00000300
#define LWM2M_ACCESS_EX             0x00000400        // pvSet -> Pointer zu der aufzurufenden Funktion

#define LWM2M_EXECUTE_BEFORE_MASK   0x00003000
#define LWM2M_EXECUTE_SET_BEFORE    0x00001000
#define LWM2M_EXECUTE_GET_BEFORE    0x00002000

#define LWM2M_EXECUTE_AFTER_MASK    0x0000C000
#define LWM2M_EXECUTE_SET_AFTER     0x00004000
#define LWM2M_EXECUTE_GET_AFTER     0x00008000

#define LWM2M_USE_TEMP_DATA         0x00010000        // ist dieses Flag gesetzt, so wird der Temp-Speicher-Platz benutzt
#define LWM2M_STORE_IN_FLASH        0x00020000        // ist dieses Flag gesetzt, so wird gSYS im Flash gespeichert (nach Set-Funktion)

#define LWM2M_IGNORE_RESOURCE       0x00300000
#define LWM2M_DONT_CREATE_RESOURCE  0x00100000        // verhindert, dass Resource im Funk-Modul erstellt wird (PC-Only)
#define LWM2M_DONT_INIT_RESOURCE    0x00200000        // verhindert, dass Resource im Funk-Modul initialisiert wird

#define LWM2M_MAX_STRING_LEN_MASK   0xFF000000
#define LWM2M_MAX_STRING_LEN_SHIFT  24
#define LWM2M_MAX_STRING_LEN(x)     ( x >> 24 )


typedef struct ST_LWM2M_RESOURCE_ITEM
{
   uint32_t uiTypeFLGs;                               // Flags
   uint16_t wResourceID;                              // LWM2M Resource-ID
   void     *pvData;                                  // Pointer auf Daten
   int32_t (*pvSet)( uint8_t *pbBuf, uint32_t uiLen, struct ST_LWM2M_RESOURCE_ITEM const *pItem, uint32_t bInstID ); // Pointer zu Set-Funktion
   int32_t (*pvGet)( uint8_t *pbBuf, uint32_t uiLen, struct ST_LWM2M_RESOURCE_ITEM const *pItem, uint32_t bInstID ); // Pointer zu Lese-Funktion
   void     *pvParam;                                 // Zeiger auf evtl. zusätzliche Parameter
} ST_LWM2M_RESOURCE_ITEM_t;

typedef struct ST_LWM2M_RESOURCE_ITEMS
{
   uint16_t uiItemCount;
   const ST_LWM2M_RESOURCE_ITEM_t *pstItems;
} ST_LWM2M_RESOURCE_ITEMS_t;

typedef struct ST_LWM2M_OBJECT_FLOAT_PARAM
{
   float min;
   float max;
   float set_faktor;
   float get_faktor;
   const uint8_t *pbUnit;
} ST_LWM2M_OBJECT_FLOAT_PARAM_t;

typedef struct ST_LWM2M_OBJECT_INT32_PARAM
{
   int32_t min;
   int32_t max;
   float set_faktor;
   float get_faktor;
} ST_LWM2M_OBJECT_INT32_PARAM_t;

typedef struct ST_LWM2M_OBJECT_INT16_PARAM
{
   int16_t min;
   int16_t max;
   float set_faktor;
   float get_faktor;
} ST_LWM2M_OBJECT_INT16_PARAM_t;

typedef union
{
   uint8_t  bByte;
   uint16_t ui16Int;
   uint32_t ui32Int;
   int8_t   cChar;
   int16_t  i16Int;
   int32_t  i32Int;
   float    fFloat;
   double   dDouble;
   uint8_t  bArray[8];
} UN_ALL_DATA_t;

#define LWM2M_OBJ_FLAG_NORMAL_OBJ  0x000000000        // Normales Objekt - wird beim starten im Funkmodul angelegt
#define LWM2M_OBJ_FLAG_IGNORE_OBJ  0x000000001        // Object nicht im Funkmodul erzeugen

typedef struct ST_LWM2M_OBJECT
{
   uint16_t wObjectID;                                // Object-ID
   uint8_t  bMaxInstID;                               // Max.Instanze-ID
   uint32_t uiObjFlags;                               // Steuerflags
   const ST_LWM2M_RESOURCE_ITEMS_t *pItems;           // Pointer auf die Items
}  ST_LWM2M_OBJECT_t;

typedef struct ST_LWM2M_IDs
{
   uint16_t wObjectID;                                // Object-ID
   uint8_t  bInstanceID;                              // Instance-ID
   uint16_t wResourceID;                              // Resource-ID
}  ST_LWM2M_IDs_t;


// Ablauf LWM2M_RD_REQ:
// Interpreter sucht Resource-ID
// prüft auf Get-Funktion und Before-Mask
//    ruft die Get-Funktion auf welche die Daten z. B. nach fTemp speichert (z.B. Umrechnung / Typ-Konvertierung)
// prüft auf Get-Funktion und After-Mask
//    Get-Funktion füllt Buffer mit Daten und gibt zurück wiviel Daten es war
// else
//    LWM2M_WR_REQ  baut aus pvDaten und Typ das Sende-Datenpacket zusammen

// Beispiel:
// pvData -> zeigt auf Messwert mit dem richtigen Daten-Type und Skalierung
//    keine pvSet und pvGet-Funktion notwendig
//    Interpreter holt Daten aus pvData und bereitet den Sende-Buffer mit den Daten auf

// pvData -> zeigt auf Messwert Skalierung / Datentyp stimmt nicht
//    pvGet-Funktion notwendig: Flags: LWM2M_EXECUTE_GET_BEFORE, LWM2M_USE_TEMP_DATA
//       Get-Funktion berechnet aus pvDaten den Type-Richtigen Wert aus und legt in munTempData ab
//    Interpreter holt Daten aus munTempData und bereitet den Sende-Buffer mit den Daten auf

// pvData -> zeigt auf unbekannten Datentype
//    pvGet-Funktion notwendig: Flags: LWM2M_EXECUTE_GET_AFTER
//       Getfunktion übernimmt die Datenaufbereitung im übergebenen Buffer

//

//==============================================================================================================================
// Dies Funktion bereitet Objekt Daten auf und speichert diese im UART-Buffer so dass dieser gesendet werden kann
//==============================================================================================================================
//  stIDs:                 Zeiger auf die Object/Instance/Resource-IDs
//  pbBuf:                 hier gehen die aufbereiteten Daten hin
//  uiLen:                 größe des übergebenen Buffers
//==============================================================================================================================
//  return:                Anzahl der übertragenen Bytes oder negativ wenn Fehler
//==============================================================================================================================
int32_t LWM2M_GetItem( ST_LWM2M_IDs_t *stIDs, uint8_t *pbBuf, uint32_t uiLen );

int32_t LWM2M_SetItem( ST_LWM2M_IDs_t *stIDs, uint8_t *pbBuf, uint32_t uiLen );


//==============================================================================================================================
// Dies Funktion berarbeitet den LWM2M_RD_RQ
//==============================================================================================================================
//  pbBuf:                 stehen die vom RD-Rquest empfangenen Daten
//  uiLen:                 größe des übergebenen Buffers
//==============================================================================================================================
void LWM2M_InterpretRD_REQ(uint8_t *pbBuf, uint32_t uiLen);

//==============================================================================================================================
// Dies Funktion berarbeitet den LWM2M_WR_RQ
//==============================================================================================================================
//  pbBuf:                 stehen die vom RD-Rquest empfangenen Daten
//  uiLen:                 größe des übergebenen Buffers
//==============================================================================================================================
void LWM2M_InterpretWR_REQ(uint8_t *pbBuf, uint32_t uiLen);

//==============================================================================================================================
//   Interpret LWM2M Next-Layer CMD
//==============================================================================================================================
void LWM2M_InterpretCMD( uint8_t *pbBuf, uint32_t uiLen );

void LWM2M_GAS_Interpret_CFG_GET( uint8_t *pbBuf, uint32_t uiLen );
void LWM2M_GAS_Interpret_CFG_SET( uint8_t *pbBuf, uint32_t uiLen );

//==============================================================================================================================
// gibt die ADC-Objecte aus
//==============================================================================================================================
//void LWM2M_FestoADC_RecordOut(uint32_t uiIndex);
//
//void LWM2M_FestoStatusOut(void);
//
//void LWM2M_BME280_DataOut( void );
//void LWM2M_Si1132_DataOut( void );

//==============================================================================================================================
// Vor Set den LWM2M-Layer stoppen (Device-Stop)
//==============================================================================================================================
void LWM2M_GAS_DeviceStart( void );                   // Erst nach E1 41 30 ist der Stack gestoppt
void LWM2M_GAS_DeviceStop( void );

//==============================================================================================================================
//    Sendet die neuen Configurationsdaten - Device muss dazu gestoppt sein
//==============================================================================================================================
void LWM2M_GAS_CFG_SET_MACADDR( uint32_t bIsResponse );
void LWM2M_GAS_CFG_SET_PANID  ( uint32_t bIsResponse );
void LWM2M_GAS_CFG_SET_OPMODE ( uint32_t bIsResponse );
void LWM2M_GAS_CFG_SET_CHANNEL( uint32_t bIsResponse );

void LWM2M_GAS_CFG_GET_MACADDR( void );
void LWM2M_GAS_CFG_GET_PANID  ( void );
void LWM2M_GAS_CFG_GET_OPMODE ( void );
void LWM2M_GAS_CFG_GET_CHANNEL( void );

void LWM2M_GAS_SetMAC      ( void );
void LWM2M_GAS_SetPANID    ( void );
void LWM2M_GAS_SetOPMODE   ( void );
void LWM2M_GAS_SetChannel  ( void );

void LWM2M_Stop(void);                                // LWM2M-Schicht stoppen
void LWM2M_Start(void);                               // LWM2m-Schicht straten

//==============================================================================================================================
//    Sendet die Aufforderung an CC1350 die Daten zu senden
//==============================================================================================================================
void LWM2M_SET_BS_SRV_IP   ( uint32_t bIsResponse );
void LWM2M_SET_BS_SRV_PORT ( uint32_t bIsResponse );
void LWM2M_SET_SRV_IP      ( uint32_t bIsResponse );
void LWM2M_SET_SRV_PORT    ( uint32_t bIsResponse );
void LWM2M_SET_CLI_NAME    ( uint32_t bIsResponse );

void LWM2M_GET_BS_SRV_IP   ( void );
void LWM2M_GET_BS_SRV_PORT ( void );
void LWM2M_GET_SRV_IP      ( void );
void LWM2M_GET_SRV_PORT    ( void );
void LWM2M_GET_CLI_NAME    ( void );

//==============================================================================================================================
//==============================================================================================================================
int32_t LWM2M_BuildObjektUartBuf( uint8_t *pbBuf, uint32_t uiLen, uint16_t uiObjID, uint8_t uiInstID, uint16_t uiResID );

//==============================================================================================================================
//  Minimale Buffer-Größe: Header 5-Bytes, n*(3+Resource-Daten-Länge)
//==============================================================================================================================
int16_t LWM2M_BuildInstanceWR_UartBuf( uint8_t *pbBuf, int16_t iLen, uint8_t bGenHeader, uint16_t uiObjID, uint8_t uiInstID,
                                       const uint16_t uiResIDs[], uint16_t uiNumResources );

//==============================================================================================================================
//    setzt den SampleIndex für die Festo Daten-Ausgabe - genutzt aus UART-Task
//==============================================================================================================================
void LWM2M_SetFestoSampleIndex(uint32_t uiIndex );

//==============================================================================================================================
// Setzt die Modulvariablen fürs Anlegen der Objekte entsprechend
//==============================================================================================================================
void LWM2M_startCreateObjects(void);

//==============================================================================================================================
// Hier werden die Objecte im Funkmodul angelegt.  Return-Wert für Step-Änderung der State-Maschine in UART_Task verwendet
//==============================================================================================================================
//    Ret = 0     => weitere Objecte vorhanden
//    Ret = 1     => Object wurde Angelegt -> Resourcen anlegen
//    Ret = 2     => keine weiteren Objecte vorhanden
//==============================================================================================================================
int32_t LWM2M_CreateNextObject();                     // hier ein Object nach dem anderen anlegen bis alle Objecte aktiv sind

//==============================================================================================================================
// Hier werden die Resourcen zum Object im Funkmodul angelegt
// Return-Wert für Step-Änderung der State-Maschine in UART_Task verwendet
//==============================================================================================================================
//    Ret = 0     => weitere Resourcen vorhanden
//    Ret = -1    => alle Resourcen des Objects angelegt
//    Ret = 1     => alle Resourcen des Objects angelegt - keine weiteren Objecte/Instanzen vorhanden
//==============================================================================================================================
int32_t LWM2M_CreateNextResource();

//==============================================================================================================================
//    Hier alle Resourcen initilisieren.
//    Return-Wert für Step-Änderung der State-Maschine in UART_Task verwendet
//    muiObjectIndex, mbInstanceIndex,  muiResourceIndex sollten beim ersten Aufruf 0 sein
//==============================================================================================================================
//    Ret = 0     => weitere Resourcen vorhanden
//    Ret = 1     => alle Resourcen wurden initialisiert
//==============================================================================================================================
int32_t  LWM2M_InitNextResource();

//===========================================================================================================
//    Globale Variable
//===========================================================================================================
extern uint32_t guiLWM2M_Status;
extern UN_ALL_DATA_t gunAllTemp;
#endif