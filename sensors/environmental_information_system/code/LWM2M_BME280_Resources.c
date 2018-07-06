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
#include "BME280.h"

//==============================================================================================================================
// Hier alle für die Scalierung der BME280 notwendigen strukturen
//==============================================================================================================================

const uint8_t cBLE280_TempUnit[] = "°C";
const ST_LWM2M_OBJECT_FLOAT_PARAM_t gcstBME280_Temp_Par =
{
   .min        =   -10.0f,
   .max        =    60.0f,
   .set_faktor =   100.0f,
   .get_faktor =     0.01f,
   .pbUnit     =   cBLE280_TempUnit,
};

const uint8_t cBLE280_PressureUnit[] = "hPas";
const ST_LWM2M_OBJECT_FLOAT_PARAM_t gcstBME280_Pressure_Par =
{
   .min        =   850.0f,
   .max        =  1100.0f,
   .set_faktor =   100.0f,
   .get_faktor =     0.01f,
   .pbUnit     =   cBLE280_PressureUnit,
};

const uint8_t cBLE280_HumityUnit[] = "%";
const ST_LWM2M_OBJECT_FLOAT_PARAM_t gcstBME280_Humity_Par =
{
   .min        =          0.0f,                       // min   0% Luftfeuchte
   .max        =        100.0f,                       // max 100% Luftfeuchte
   .set_faktor =       1024.0f,
   .get_faktor =  1.0f/1024.0f,
   .pbUnit     =   cBLE280_HumityUnit,
};


//==============================================================================================================================
// hier die Funktionen zur einstellung der Idel-Zeiten der Tasks damit die Einstellung sofort wirksam wird
//==============================================================================================================================
static int32_t setBME280_IdelTime(uint8_t *pbBuf, uint32_t uiLen, struct ST_LWM2M_RESOURCE_ITEM const *pItem, uint32_t bInstID)
{
   uint32_t uiResult = setTime( pbBuf, uiLen, pItem, bInstID);
   if (uiResult==0)
      BME280_SetMessinterval( gSYS.ulPressureMessInterval );
   return uiResult;
}

int32_t get_BME280_Data(uint8_t *pbBuf, uint32_t uiLen, struct ST_LWM2M_RESOURCE_ITEM const *pItem, uint32_t bInstID)
{
   return get_int32_to_float( pbBuf, uiLen, pItem, bInstID );
}

static int32_t exec_BME280MinMaxReset(uint8_t *pbBuf, uint32_t uiLen, struct ST_LWM2M_RESOURCE_ITEM const *pItem, uint32_t bInstID)
{
   Radio_BME280_ResetMinMax();
   return 0;
}

//==============================================================================================================================
// hier nun die BLE280-Items
//==============================================================================================================================

static const ST_LWM2M_RESOURCE_ITEM_t stTemp_items[] =
{
   {                                                  // Derzeitiger Messwert
      LWM2M_TYPE_FLOAT           |
      LWM2M_ACCESS_RD            |
      LWM2M_EXECUTE_GET_BEFORE   |
      LWM2M_USE_TEMP_DATA,
      LWM2M_CUR_SENS_VALUE,                           // resource-ID
      &(gstBME280_MD.stCur.i32Temp),
      0,                                              // setzen geht nicht
      get_BME280_Data,                                // beim lesen int32 in float umwandeln
      (void*)&gcstBME280_Temp_Par,                     // Parameter für Umwandlung (faktor 0.01)
   },
   {                                                  // Minimaler Messwert seit letztem Reset
      LWM2M_TYPE_FLOAT           |
      LWM2M_ACCESS_RD            |
      LWM2M_EXECUTE_GET_BEFORE   |
      LWM2M_USE_TEMP_DATA,
      LWM2M_MIN_SENS_VALUE,                           // resource-ID
      &(gstBME280_MD.stMin.i32Temp),
      0,                                              // setzen geht nicht
      get_BME280_Data,                                // beim lesen int32 in float umwandeln
      (void*)&gcstBME280_Temp_Par,                     // Parameter für Umwandlung (faktor 0.01)
   },
   {                                                  // Maximaler Messwert seit letztem Reset
      LWM2M_TYPE_FLOAT           |
      LWM2M_ACCESS_RD            |
      LWM2M_EXECUTE_GET_BEFORE   |
      LWM2M_USE_TEMP_DATA,
      LWM2M_MAX_SENS_VALUE,                           // resource-ID
      &(gstBME280_MD.stMax.i32Temp),
      0,                                              // setzen geht nicht
      get_BME280_Data,                                // beim lesen int32 in float umwandeln
      (void*)&gcstBME280_Temp_Par,                     // Parameter für Umwandlung (faktor 0.01)
   },
   //=================================================// ab hier die Sensorabhängigen Konstanten
   {                                                  // Unterer Messbereich des Sensors
      LWM2M_TYPE_FLOAT           |
      LWM2M_ACCESS_RD            ,
      LWM2M_MIN_SENS_RANGE,                           // resource-ID
      (void*)&gcstBME280_Temp_Par.min,
      0,                                              // no set
      0,                                              // no get
      0,                                              // no parameter
   },
   {                                                  // Oberer Messbereich des Sensors
      LWM2M_TYPE_FLOAT           |
      LWM2M_ACCESS_RD            ,
      LWM2M_MAX_SENS_RANGE,                           // resource-ID
      (void*)&gcstBME280_Temp_Par.max,
      0,                                              // no set
      0,                                              // no get
      0,                                              // no parameter
   },
   {                                                  // Units
      LWM2M_TYPE_STRING          |
      LWM2M_ACCESS_RD            ,
      5701,                                           // resource-ID
      (void*)cBLE280_TempUnit,
      0,                                              // no set
      0,                                              // no get
      0,                                              // no parameter
   },
};

static const ST_LWM2M_RESOURCE_ITEM_t stPressure_items[] =
{
   {
      LWM2M_TYPE_FLOAT           |
      LWM2M_ACCESS_RD            |
      LWM2M_EXECUTE_GET_BEFORE   |
      LWM2M_USE_TEMP_DATA,
      LWM2M_CUR_SENS_VALUE,                           // resource-ID
      &(gstBME280_MD.stCur.u32Pressure),
      0,                                              // setzen geht nicht
      get_BME280_Data,                                // beim lesen int32 in float umwandeln
      (void*)&gcstBME280_Pressure_Par,                 // Parameter für Umwandlung (faktor 0.01)
   },
   {
      LWM2M_TYPE_FLOAT           |
      LWM2M_ACCESS_RD            |
      LWM2M_EXECUTE_GET_BEFORE   |
      LWM2M_USE_TEMP_DATA,
      LWM2M_MIN_SENS_VALUE,                           // resource-ID
      &(gstBME280_MD.stMin.u32Pressure),
      0,                                              // setzen geht nicht
      get_BME280_Data,                                // beim lesen int32 in float umwandeln
      (void*)&gcstBME280_Pressure_Par,                 // Parameter für Umwandlung (faktor 0.01)
   },
   {
      LWM2M_TYPE_FLOAT           |
      LWM2M_ACCESS_RD            |
      LWM2M_EXECUTE_GET_BEFORE   |
      LWM2M_USE_TEMP_DATA,
      LWM2M_MAX_SENS_VALUE,                           // resource-ID
      &(gstBME280_MD.stMax.u32Pressure),
      0,                                              // setzen geht nicht
      get_BME280_Data,                                // beim lesen int32 in float umwandeln
      (void*)&gcstBME280_Pressure_Par,                 // Parameter für Umwandlung (faktor 0.01)
   },
   //=================================================// ab hier die Sensorabhängigen Konstanten
   {                                                  // Unterer Messbereich des Sensors
      LWM2M_TYPE_FLOAT           |
      LWM2M_ACCESS_RD            ,
      LWM2M_MIN_SENS_RANGE,                           // resource-ID
      (void*)&gcstBME280_Pressure_Par.min,
      0,                                              // no set
      0,                                              // no get
      0,                                              // no parameter
   },
   {                                                  // Oberer Messbereich des Sensors
      LWM2M_TYPE_FLOAT           |
      LWM2M_ACCESS_RD            ,
      LWM2M_MAX_SENS_RANGE,                           // resource-ID
      (void*)&gcstBME280_Pressure_Par.max,
      0,                                              // no set
      0,                                              // no get
      0,                                              // no parameter
   },
   {                                                  // Units
      LWM2M_TYPE_STRING          |
      LWM2M_ACCESS_RD            ,
      5701,                                           // resource-ID
      (void*)cBLE280_PressureUnit,
      0,                                              // no set
      0,                                              // no get
      0,                                              // no parameter
   },
};

const ST_LWM2M_RESOURCE_ITEM_t stHumity_items[] =
{
   {
      LWM2M_TYPE_FLOAT           |
      LWM2M_ACCESS_RD            |
      LWM2M_EXECUTE_GET_BEFORE   |
      LWM2M_USE_TEMP_DATA,
      LWM2M_CUR_SENS_VALUE,                           // resource-ID
      &(gstBME280_MD.stCur.u32Humity),
      0,                                              // setzen geht nicht
      get_int32_to_float,                                 // beim lesen int32 in float umwandeln
      (void*)&gcstBME280_Humity_Par,                   // Parameter für Umwandlung (faktor 1/1024)
   },
   {
      LWM2M_TYPE_FLOAT           |
      LWM2M_ACCESS_RD            |
      LWM2M_EXECUTE_GET_BEFORE   |
      LWM2M_USE_TEMP_DATA,
      LWM2M_MIN_SENS_VALUE,                           // resource-ID
      &(gstBME280_MD.stMin.u32Humity),
      0,                                              // setzen geht nicht
      get_BME280_Data,                                // beim lesen int32 in float umwandeln
      (void*)&gcstBME280_Humity_Par,                   // Parameter für Umwandlung (faktor 1/1024)
   },
   {
      LWM2M_TYPE_FLOAT           |
      LWM2M_ACCESS_RD            |
      LWM2M_EXECUTE_GET_BEFORE   |
      LWM2M_USE_TEMP_DATA,
      LWM2M_MAX_SENS_VALUE,                           // resource-ID
      &(gstBME280_MD.stMax.u32Humity),
      0,                                              // setzen geht nicht
      get_BME280_Data,                                // beim lesen int32 in float umwandeln
      (void*)&gcstBME280_Humity_Par,                   // Parameter für Umwandlung (faktor 1/1024)
   },
   //=================================================// ab hier die Sensorabhängigen Konstanten
   {                                                  // Unterer Messbereich des Sensors
      LWM2M_TYPE_FLOAT           |
      LWM2M_ACCESS_RD            ,
      LWM2M_MIN_SENS_RANGE,                           // resource-ID
      (void*)&gcstBME280_Humity_Par.min,
      0,                                              // no set
      0,                                              // no get
      0,                                              // no parameter
   },
   {                                                  // Oberer Messbereich des Sensors
      LWM2M_TYPE_FLOAT           |
      LWM2M_ACCESS_RD            ,
      LWM2M_MAX_SENS_RANGE,                           // resource-ID
      (void*)&gcstBME280_Humity_Par.max,
      0,                                              // no set
      0,                                              // no get
      0,                                              // no parameter
   },
   {                                                  // Units
      LWM2M_TYPE_STRING          |
      LWM2M_ACCESS_RD            ,
      5701,                                           // resource-ID
      (void*)cBLE280_HumityUnit,
      0,                                              // no set
      0,                                              // no get
      0,                                              // no parameter
   },
};

static const ST_LWM2M_RESOURCE_ITEM_t stInitItems[] =
{
   {                                                  // Reset-Funktion um Min/Max-Werte auf aktuellen Messwert zu setzen
      LWM2M_EXECUTE_SET_BEFORE   |
      LWM2M_ACCESS_EX,
      5605,                                           // resource-ID
      0,
      exec_BME280MinMaxReset,                         // no set
      0,                                              // no get
      0,                                              // no parameter
   },
   {
      LWM2M_TYPE_FLOAT           |
      LWM2M_ACCESS_RD_WR         |
      LWM2M_EXECUTE_GET_BEFORE   |
      LWM2M_EXECUTE_SET_AFTER    |
      LWM2M_USE_TEMP_DATA,
      LWM2M_IDEL_TIME_RES_ID,                         // resource-ID
      &(gSYS.ulPressureMessInterval),
      setBME280_IdelTime,                             // setzen die BLE280-Idel-Time
      getTime,                                        // beim lesen int32 in float umwandeln und skalieren
      (void*)&cstObjectTime2Time,                     // Parameter für Skalierung
   },
//   {
//      LWM2M_TYPE_INT32           |
//      LWM2M_ACCESS_RD_WR,
//      LWM2M_STATUS_RES_ID,                            // resource-ID
//      &(gSYS.stBME280_SET.uiStatusFlags),
//      0,                                              // keine Set-Funktion
//      0,                                              // keine Get-Funktion
//      0,                                              // keine Parameter
//   },
};

const ST_LWM2M_RESOURCE_ITEMS_t stBME280_Temp_Items =
{
   .uiItemCount = GET_ITEM_COUNT(stTemp_items),
   .pstItems    = stTemp_items,
};

const ST_LWM2M_RESOURCE_ITEMS_t stBME280_Pressure_Items =
{
   .uiItemCount = GET_ITEM_COUNT(stPressure_items),
   .pstItems    = stPressure_items,
};

const ST_LWM2M_RESOURCE_ITEMS_t stBME280_Humity_Items =
{
   .uiItemCount = GET_ITEM_COUNT(stHumity_items),
   .pstItems    = stHumity_items,
};

const ST_LWM2M_RESOURCE_ITEMS_t stBME280_Init_Items =
{
   .uiItemCount = GET_ITEM_COUNT(stInitItems),
   .pstItems    = stInitItems,
};
