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

//==============================================================================================================================
// Hier alle für den 2d-Flow
//==============================================================================================================================
static const uint8_t cFlow_X_Unit[] = "x m/s";
static const uint8_t cFlow_Y_Unit[] = "y m/s";
const ST_LWM2M_OBJECT_FLOAT_PARAM_t gcstFlow_Par =
{
   .min        =         -5.0f,                       // min -5,0 m/s
   .max        =          5.0f,                       // max  5,0 m/s
   .set_faktor =           100,
   .get_faktor =          0.01,
   .pbUnit     =   cFlow_X_Unit,
};

//==============================================================================================================================
//    Umrechnung von int16 in float unter Berücksichtigung der Einheit (int16=1/100m/s -> float=m/s)
//==============================================================================================================================
int32_t get_Flow_Data(uint8_t *pbBuf, uint32_t uiLen, struct ST_LWM2M_RESOURCE_ITEM const *pItem, uint32_t bInstID)
{
   return get_int16_to_float( pbBuf, uiLen, pItem, bInstID );
}

//==============================================================================================================================
//    Flow-Objecte
//==============================================================================================================================
static const ST_LWM2M_RESOURCE_ITEM_t stX_Items[] =
{
   {
      LWM2M_TYPE_FLOAT           |
      LWM2M_ACCESS_RD            |
      LWM2M_EXECUTE_GET_BEFORE   |
      LWM2M_USE_TEMP_DATA,
      LWM2M_CUR_SENS_VALUE,                           // resource-ID
      &(gstFlow_MD.iFlowX_avg),
      0,                                              // setzen geht nicht
      get_Flow_Data,                                  // beim lesen int16 in float umwandeln
      (void*)&gcstFlow_Par,                            // Parameter für Umwandlung (faktor 0.01)
   },
   {
      LWM2M_TYPE_FLOAT           |
      LWM2M_ACCESS_RD            |
      LWM2M_EXECUTE_GET_BEFORE   |
      LWM2M_USE_TEMP_DATA,
      LWM2M_MIN_SENS_VALUE,                           // resource-ID
      &(gstFlow_MD.iFlowX_min),
      0,                                              // setzen geht nicht
      get_Flow_Data,                                  // beim lesen int16 in float umwandeln
      (void*)&gcstFlow_Par,                            // Parameter für Umwandlung (faktor 0.01)
   },
   {
      LWM2M_TYPE_FLOAT           |
      LWM2M_ACCESS_RD            |
      LWM2M_EXECUTE_GET_BEFORE   |
      LWM2M_USE_TEMP_DATA,
      LWM2M_MAX_SENS_VALUE,                           // resource-ID
      &(gstFlow_MD.iFlowX_max),
      0,                                              // setzen geht nicht
      get_Flow_Data,                                  // beim lesen int16 in float umwandeln
      (void*)&gcstFlow_Par,                            // Parameter für Umwandlung (faktor 0.01)
   },
   //=================================================// ab hier die Sensorabhängigen Konstanten
   {                                                  // Unterer Messbereich des Sensors
      LWM2M_TYPE_FLOAT           |
      LWM2M_ACCESS_RD            ,
      LWM2M_MIN_SENS_RANGE,                           // resource-ID
      (void*)&gcstFlow_Par.min,
      0,                                              // no set
      0,                                              // no get
      0,                                              // no parameter
   },
   {                                                  // Oberer Messbereich des Sensors
      LWM2M_TYPE_FLOAT           |
      LWM2M_ACCESS_RD            ,
      LWM2M_MAX_SENS_RANGE,                           // resource-ID
      (void*)&gcstFlow_Par.max,
      0,                                              // no set
      0,                                              // no get
      0,                                              // no parameter
   },
   {                                                  // Units
      LWM2M_TYPE_STRING          |
      LWM2M_ACCESS_RD            ,
      5701,                                           // resource-ID
      (void*)cFlow_X_Unit,
      0,                                              // no set
      0,                                              // no get
      0,                                              // no parameter
   },
};

static const ST_LWM2M_RESOURCE_ITEM_t stY_Items[] =
{
   {
      LWM2M_TYPE_FLOAT           |
      LWM2M_ACCESS_RD            |
      LWM2M_EXECUTE_GET_BEFORE   |
      LWM2M_USE_TEMP_DATA,
      LWM2M_CUR_SENS_VALUE,                           // resource-ID
      &(gstFlow_MD.iFlowY_avg),
      0,                                              // setzen geht nicht
      get_Flow_Data,                                  // beim lesen int16 in float umwandeln
      (void*)&gcstFlow_Par,                            // Parameter für Umwandlung (faktor 0.01)
   },
   {
      LWM2M_TYPE_FLOAT           |
      LWM2M_ACCESS_RD            |
      LWM2M_EXECUTE_GET_BEFORE   |
      LWM2M_USE_TEMP_DATA,
      LWM2M_MIN_SENS_VALUE,                           // resource-ID
      &(gstFlow_MD.iFlowY_min),
      0,                                              // setzen geht nicht
      get_Flow_Data,                                  // beim lesen int16 in float umwandeln
      (void*)&gcstFlow_Par,                            // Parameter für Umwandlung (faktor 0.01)
   },
   {
      LWM2M_TYPE_FLOAT           |
      LWM2M_ACCESS_RD            |
      LWM2M_EXECUTE_GET_BEFORE   |
      LWM2M_USE_TEMP_DATA,
      LWM2M_MAX_SENS_VALUE,                           // resource-ID
      &(gstFlow_MD.iFlowY_max),
      0,                                              // setzen geht nicht
      get_Flow_Data,                                  // beim lesen int16 in float umwandeln
      (void*)&gcstFlow_Par,                            // Parameter für Umwandlung (faktor 0.01)
   },
   //=================================================// ab hier die Sensorabhängigen Konstanten
   {                                                  // Unterer Messbereich des Sensors
      LWM2M_TYPE_FLOAT           |
      LWM2M_ACCESS_RD            ,
      LWM2M_MIN_SENS_RANGE,                           // resource-ID
      (void*)&gcstFlow_Par.min,
      0,                                              // no set
      0,                                              // no get
      0,                                              // no parameter
   },
   {                                                  // Oberer Messbereich des Sensors
      LWM2M_TYPE_FLOAT           |
      LWM2M_ACCESS_RD            ,
      LWM2M_MAX_SENS_RANGE,                           // resource-ID
      (void*)&gcstFlow_Par.max,
      0,                                              // no set
      0,                                              // no get
      0,                                              // no parameter
   },
   {                                                  // Units
      LWM2M_TYPE_STRING          |
      LWM2M_ACCESS_RD            ,
      5701,                                           // resource-ID
      (void*)cFlow_Y_Unit,
      0,                                              // no set
      0,                                              // no get
      0,                                              // no parameter
   },
};


const ST_LWM2M_RESOURCE_ITEMS_t stFlow_X_Items =
{
   .uiItemCount = GET_ITEM_COUNT(stX_Items),
   .pstItems    = stX_Items,
};

const ST_LWM2M_RESOURCE_ITEMS_t stFlow_Y_Items =
{
   .uiItemCount = GET_ITEM_COUNT(stY_Items),
   .pstItems    = stY_Items,
};

