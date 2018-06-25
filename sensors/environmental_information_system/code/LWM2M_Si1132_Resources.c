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
#include "LWM2M_Utils.h"
#include "Si1132.h"

//==============================================================================================================================
// Hier alle für den Si1132 notwendigen Strukturen
//==============================================================================================================================
static const uint8_t cSi1132_VIS_Unit[] = "lux";
static const ST_LWM2M_OBJECT_INT32_PARAM_t cstSi1132_VIS_Par =
{
   .min        =             0,                       // min     0 lux
   .max        =         65535,                       // max 65535 lux
   .set_faktor =             1,                       // keine umrechnungfaktoren
   .get_faktor =             1,
};

static const uint8_t cSi1132_IR_Unit[] = "lux";
static const ST_LWM2M_OBJECT_INT32_PARAM_t cstSi1132_IR_Par =
{
   .min        =             0,                       // min     0 lux
   .max        =         65535,                       // max 65535 lux
   .set_faktor =             1,                       // keine umrechnungfaktoren
   .get_faktor =             1,
};

static const uint8_t cSi1132_UV_Unit[] = "none";
static const ST_LWM2M_OBJECT_FLOAT_PARAM_t cstSi1132_UV_Par =
{
   .min        =          0.0f,                       // min  0 (UV-Index)
   .max        =         50.0f,                       // max 50 (UV-Index)
   .set_faktor =           100,
   .get_faktor =          0.01,
};


//==============================================================================================================================
//    Setzen der Idel-Time in der Si1132-Task
//==============================================================================================================================
static int32_t setSi1132_IdelTime(uint8_t *pbBuf, uint32_t uiLen, struct ST_LWM2M_RESOURCE_ITEM const *pItem, uint32_t bInstID)
{
   uint32_t uiResult = setTime( pbBuf, uiLen, pItem, bInstID);
   if (uiResult==0)
      Si1132_SetMessInterval( gSYS.ulLightMessInterval );
   return uiResult;
}

//int32_t exec_Si1132MinMaxReset(uint8_t *pbBuf, uint32_t uiLen, struct ST_LWM2M_OBJECT_ITEM const *pItem, uint32_t bInstID)
//{
//   Si1132_Task_ResetMinMax();
//   return 0;
//}

//==============================================================================================================================
//    Si1132-Objecte
//==============================================================================================================================
static const ST_LWM2M_RESOURCE_ITEM_t stVIS_items[] =
{
   {
      LWM2M_TYPE_FLOAT           |
      LWM2M_EXECUTE_GET_BEFORE   |
      LWM2M_USE_TEMP_DATA        |
      LWM2M_ACCESS_RD            ,
      LWM2M_CUR_SENS_VALUE,                           // resource-ID
      &(gstSi1132_MD.wAvgVIS),
      0,                                              // setzen geht nicht
      get_uint16_to_float,                             // 16-Bit nach 32-Bit wandeln
      0,                                              // keine Parameter
   },
   {
      LWM2M_TYPE_FLOAT           |
      LWM2M_EXECUTE_GET_BEFORE   |
      LWM2M_USE_TEMP_DATA        |
      LWM2M_ACCESS_RD            ,
      LWM2M_MIN_SENS_VALUE,                           // resource-ID
      &(gstSi1132_MD.wMinVIS),
      0,                                              // setzen geht nicht
      get_uint16_to_float,                             // 16-Bit nach 32-Bit wandeln
      0,                                              // keine Parameter
   },
   {
      LWM2M_TYPE_FLOAT           |
      LWM2M_EXECUTE_GET_BEFORE   |
      LWM2M_USE_TEMP_DATA        |
      LWM2M_ACCESS_RD            ,
      LWM2M_MAX_SENS_VALUE,                           // resource-ID
      &(gstSi1132_MD.wMaxVIS),
      0,                                              // setzen geht nicht
      get_uint16_to_float,                             // 16-Bit nach 32-Bit wandeln
      0,                                              // keine Parameter
   },
   //=================================================// ab hier die Sensorabhängigen Konstanten
   {                                                  // Unterer Messbereich des Sensors
      LWM2M_TYPE_FLOAT           |
      LWM2M_ACCESS_RD            ,
      LWM2M_MIN_SENS_RANGE,                           // resource-ID
      (void*)&cstSi1132_VIS_Par.min,
      0,                                              // no set
      get_uint16_to_float,                            // 16-Bit nach 32-Bit wandeln
      0,                                              // no parameter
   },
   {                                                  // Oberer Messbereich des Sensors
      LWM2M_TYPE_FLOAT           |
      LWM2M_ACCESS_RD            ,
      LWM2M_MAX_SENS_RANGE,                           // resource-ID
      (void*)&cstSi1132_VIS_Par.max,
      0,                                              // no set
      get_uint16_to_float,                            // 16-Bit nach 32-Bit wandeln
      0,                                              // no parameter
   },
   {                                                  // Units
      LWM2M_TYPE_STRING          |
      LWM2M_ACCESS_RD            ,
      5701,                                           // resource-ID
      (void*)cSi1132_VIS_Unit,
      0,                                              // no set
      0,                                              // 16-Bit nach 32-Bit wandeln
      0,                                              // no parameter
   },
//   //=================================================// ab hier die Definitionen fürs Log-Buch
//   {
//      LWM2M_TYPE_INT32          |
//      LWM2M_ACCESS_RD_WR         ,
//      MaxLogValue_RES_ID,                             // resource-ID
//      &(gSYS.stSi1132_SET.stMax.wVIS),
//      0,                                              // keine Umrechnung -> keine Funktion
//      0,                                              // keine Umrechnung -> keine Funktion
//      (void*)&cstSi1132_VIS_Par,                      // Beim Setzen min/max beachten
//   },
//   {
//      LWM2M_TYPE_INT32          |
//      LWM2M_ACCESS_RD_WR         ,
//      MinLogValue_RES_ID,                             // resource-ID
//      &(gSYS.stSi1132_SET.stMin.wVIS),
//      0,                                              // keine Umrechnung -> keine Funktion
//      0,                                              // keine Umrechnung -> keine Funktion
//      (void*)&cstSi1132_VIS_Par,                      // Beim Setzen min/max beachten
//   },
//   {
//      LWM2M_TYPE_INT32          |
//      LWM2M_ACCESS_RD_WR         ,
//      DifLogValue_RES_ID,                             // resource-ID
//      &(gSYS.stSi1132_SET.stDif.wVIS),
//      0,                                              // keine Umrechnung -> keine Set-Funktion
//      0,                                              // keine Umrechnung -> keine Get-Funktion
//      (void*)&cstSi1132_VIS_Par,                      // Beim Setzen min/max beachten
//   },
};

//const ST_LWM2M_OBJECT_ITEM_t stSi1132_IR_items[] =
//{
//   {
//      LWM2M_TYPE_INT32          |
//      LWM2M_ACCESS_RD            ,
//      LWM2M_CUR_SENS_VALUE,                           // resource-ID
//      &(gstSi1132_MD.iAvgIR),
//      0,                                              // setzen geht nicht
//      0,                                              // keine Lese-Funktion notwendig
//      0,                                              // keine Parameter
//   },
//   {
//      LWM2M_TYPE_INT32          |
//      LWM2M_ACCESS_RD            ,
//      LWM2M_MIN_SENS_VALUE,                           // resource-ID
//      &(gstSi1132_MD.iMinIR),
//      0,                                              // setzen geht nicht
//      0,                                              // keine Lese-Funktion notwendig
//      0,                                              // keine Parameter
//   },
//   {
//      LWM2M_TYPE_INT32          |
//      LWM2M_ACCESS_RD            ,
//      LWM2M_MAX_SENS_VALUE,                           // resource-ID
//      &(gstSi1132_MD.iMaxIR),
//      0,                                              // setzen geht nicht
//      0,                                              // keine Lese-Funktion notwendig
//      0,                                              // keine Parameter
//   },
//   //=================================================// ab hier die Sensorabhängigen Konstanten
//   {                                                  // Unterer Messbereich des Sensors
//      LWM2M_TYPE_INT32          |
//      LWM2M_ACCESS_RD            ,
//      LWM2M_MIN_SENS_RANGE,                           // resource-ID
//      (void*)&cstSi1132_IR_Par.min,
//      0,                                              // no set
//      0,                                              // no get
//      0,                                              // no parameter
//   },
//   {                                                  // Oberer Messbereich des Sensors
//      LWM2M_TYPE_INT32          |
//      LWM2M_ACCESS_RD            ,
//      LWM2M_MAX_SENS_RANGE,                           // resource-ID
//      (void*)&cstSi1132_IR_Par.max,
//      0,                                              // no set
//      0,                                              // no get
//      0,                                              // no parameter
//   },
//   {                                                  // Units
//      LWM2M_TYPE_STRING          |
//      LWM2M_ACCESS_RD            ,
//      5701,                                           // resource-ID
//      (void*)cSi1132_IR_Unit,
//      0,                                              // no set
//      0,                                              // no get
//      0,                                              // no parameter
//   },
//   //=================================================// ab hier die Definitionen fürs Log-Buch
//   {
//      LWM2M_TYPE_INT32          |
//      LWM2M_ACCESS_RD_WR         ,
//      MinLogValue_RES_ID,                             // resource-ID
//      &(gSYS.stSi1132_SET.stMin.iIR),
//      0,                                              // keine Umrechnung -> keine Funktion
//      0,                                              // keine Umrechnung -> keine Funktion
//      (void*)&cstSi1132_IR_Par,                       // Beim Setzen min/max beachten
//   },
//   {
//      LWM2M_TYPE_INT32          |
//      LWM2M_ACCESS_RD_WR         ,
//      MaxLogValue_RES_ID,                             // resource-ID
//      &(gSYS.stSi1132_SET.stMax.iIR),
//      0,                                              // keine Umrechnung -> keine Funktion
//      0,                                              // keine Umrechnung -> keine Funktion
//      (void*)&cstSi1132_IR_Par,                       // Beim Setzen min/max beachten
//   },
//   {
//      LWM2M_TYPE_INT32          |
//      LWM2M_ACCESS_RD_WR         ,
//      DifLogValue_RES_ID,                             // resource-ID
//      &(gSYS.stSi1132_SET.stDif.iIR),
//      0,                                              // keine Umrechnung -> keine Set-Funktion
//      0,                                              // keine Umrechnung -> keine Get-Funktion
//      (void*)&cstSi1132_IR_Par,                       // Beim Setzen min/max beachten
//   },
//};

//const ST_LWM2M_OBJECT_ITEM_t stSi1132_UV_items[] =
//{
//   {
//      LWM2M_TYPE_FLOAT           |
//      LWM2M_ACCESS_RD            |
//      LWM2M_EXECUTE_GET_BEFORE   |
//      LWM2M_USE_TEMP_DATA,
//      LWM2M_CUR_SENS_VALUE,                           // resource-ID
//      &(gstSi1132_MD.wAvgUV_Index),
//      0,                                              // setzen geht nicht
//      get_int32_to_float,                             // Funktion um uint32 in float zu andeln und mit 0.01 zu multiplizierten
//      (void*)&cstSi1132_UV_Par,                       // UV-Parameter für Umrechnung der Werte
//   },
//   {
//      LWM2M_TYPE_FLOAT           |
//      LWM2M_ACCESS_RD            |
//      LWM2M_EXECUTE_GET_BEFORE   |
//      LWM2M_USE_TEMP_DATA,
//      LWM2M_MIN_SENS_VALUE,                           // resource-ID
//      &(gstSi1132_MD.wMinUV_Index),
//      0,                                              // setzen geht nicht
//      get_int32_to_float,                             // Funktion um uint32 in float zu andeln und mit 0.01 zu multiplizierten
//      (void*)&cstSi1132_UV_Par,                       // UV-Parameter für Umrechnung der Werte
//   },
//   {
//      LWM2M_TYPE_FLOAT           |
//      LWM2M_ACCESS_RD            |
//      LWM2M_EXECUTE_GET_BEFORE   |
//      LWM2M_USE_TEMP_DATA,
//      LWM2M_MAX_SENS_VALUE,                           // resource-ID
//      &(gstSi1132_MD.wMaxUV_Index),
//      0,                                              // setzen geht nicht
//      get_int32_to_float,                             // Funktion um uint32 in float zu andeln und mit 0.01 zu multiplizierten
//      (void*)&cstSi1132_UV_Par,                       // UV-Parameter für Umrechnung der Werte
//   },
//   //=================================================// ab hier die Sensorabhängigen Konstanten
//   {                                                  // Unterer Messbereich des Sensors
//      LWM2M_TYPE_FLOAT           |
//      LWM2M_ACCESS_RD            |
//      LWM2M_EXECUTE_GET_BEFORE   |
//      LWM2M_USE_TEMP_DATA,
//      LWM2M_MIN_SENS_RANGE,                           // resource-ID
//      (void*)&cstSi1132_UV_Par.min,
//      0,                                              // setzen geht nicht
//      get_int32_to_float,                             // Funktion um uint32 in float zu andeln und mit 0.01 zu multiplizierten
//      (void*)&cstSi1132_UV_Par,                       // UV-Parameter für Umrechnung der Werte
//   },
//   {                                                  // Oberer Messbereich des Sensors
//      LWM2M_TYPE_FLOAT           |
//      LWM2M_ACCESS_RD            |
//      LWM2M_EXECUTE_GET_BEFORE   |
//      LWM2M_USE_TEMP_DATA,
//      LWM2M_MAX_SENS_RANGE,                           // resource-ID
//      (void*)&cstSi1132_UV_Par.max,
//      0,                                              // setzen geht nicht
//      get_int32_to_float,                             // Funktion um uint32 in float zu andeln und mit 0.01 zu multiplizierten
//      (void*)&cstSi1132_UV_Par,                       // UV-Parameter für Umrechnung der Werte
//   },
//   {                                                  // Units
//      LWM2M_TYPE_FLOAT           |
//      LWM2M_ACCESS_RD            ,
//      5701,                                           // resource-ID
//      (void*)cSi1132_UV_Unit,
//      0,                                              // setzen geht nicht
//      0,                                              // Funktion um uint32 in float zu andeln und mit 0.01 zu multiplizierten
//      0,                                              // UV-Parameter für Umrechnung der Werte
//   },
//   //=================================================// ab hier die Definitionen fürs Log-Buch
//   {
//      LWM2M_TYPE_FLOAT           |
//      LWM2M_ACCESS_RD_WR         |
//      LWM2M_EXECUTE_GET_BEFORE   |
//      LWM2M_EXECUTE_SET_AFTER    |
//      LWM2M_USE_TEMP_DATA,
//      MinLogValue_RES_ID,                             // resource-ID
//      &(gSYS.stSi1132_SET.stMin.wUV_Index),
//      set_float_to_uint32,                            // skaliert und setzt die float-Zahl als uint32
//      get_int32_to_float,                             // beim lesen int32 in float umwandeln
//      (void*)&cstSi1132_UV_Par,                       // Beim Setzen min/max beachten
//   },
//   {
//      LWM2M_TYPE_FLOAT           |
//      LWM2M_ACCESS_RD_WR         |
//      LWM2M_EXECUTE_GET_BEFORE   |
//      LWM2M_EXECUTE_SET_AFTER    |
//      LWM2M_USE_TEMP_DATA,
//      MaxLogValue_RES_ID,                             // resource-ID
//      &(gSYS.stSi1132_SET.stMax.wUV_Index),
//      set_float_to_uint32,                            // skaliert und setzt die float-Zahl als uint32
//      get_int32_to_float,                             // beim lesen int32 in float umwandeln
//      (void*)&cstSi1132_UV_Par,                       // Beim Setzen min/max beachten
//   },
//   {
//      LWM2M_TYPE_FLOAT           |
//      LWM2M_ACCESS_RD_WR         |
//      LWM2M_EXECUTE_GET_BEFORE   |
//      LWM2M_EXECUTE_SET_AFTER    |
//      LWM2M_USE_TEMP_DATA,
//      DifLogValue_RES_ID,                             // resource-ID
//      &(gSYS.stSi1132_SET.stDif.wUV_Index),
//      set_float_to_uint32,                            // skaliert und setzt die float-Zahl als uint32
//      get_int32_to_float,                             // beim lesen int32 in float umwandeln
//      (void*)&cstSi1132_UV_Par,                       // Beim Setzen min/max beachten
//   },
//};

static const ST_LWM2M_RESOURCE_ITEM_t stInitItems[] =
{
//   {                                                  // Reset-Funktion um Min/Max-Werte auf aktuellen Messwert zu setzen
//      LWM2M_EXECUTE_SET_BEFORE   |
//      LWM2M_ACCESS_EX,
//      5605,                                           // resource-ID
//      0,
//      exec_Si1132MinMaxReset,                         // no set
//      0,                                              // no get
//      0,                                              // no parameter
//   },
   {
      LWM2M_TYPE_FLOAT           |
      LWM2M_ACCESS_RD_WR         |
      LWM2M_EXECUTE_GET_BEFORE   |
      LWM2M_EXECUTE_SET_AFTER    |
      LWM2M_USE_TEMP_DATA,
      LWM2M_IDEL_TIME_RES_ID,                         // resource-ID
      &(gSYS.ulLightMessInterval),
      setSi1132_IdelTime,                             // setzen die BLE280-Idel-Time
      getTime,                                        // beim lesen int32 in float umwandeln und skalieren
      (void*)&cstObjectTime2Time,                     // Parameter für Skalierung
   },
//   {
//      LWM2M_TYPE_INT32           |
//      LWM2M_ACCESS_RD_WR,
//      LWM2M_STATUS_RES_ID,                            // resource-ID
//      &(gSYS.stSi1132_SET.bStatus),
//      0,                                              // keine Set-Funktion
//      0,                                              // keine Get-Funktion
//      0,                                              // keine Parameter
//   },
   {
      LWM2M_TYPE_INT32           |
      LWM2M_ACCESS_RD_WR,
      Si1132_AVG_RES_ID,                              // Anzahl der Werte über die gemittelt werden soll
      &(gSYS.bLightMessAnzahl),
      0,                                              // keine Set-Funktion
      0,                                              // keine Get-Funktion
      0,                                              // keine Parameter
   },
};

const ST_LWM2M_RESOURCE_ITEMS_t stSi1132_VIS_Items =
{
   .uiItemCount = GET_ITEM_COUNT(stVIS_items),
   .pstItems    = stVIS_items,
};

const ST_LWM2M_RESOURCE_ITEMS_t stSi1132_Init_Items =
{
   .uiItemCount = GET_ITEM_COUNT(stInitItems),
   .pstItems    = stInitItems,
};
