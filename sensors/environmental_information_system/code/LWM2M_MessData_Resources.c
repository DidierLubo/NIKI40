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
#include "LWM2M_Utils.h"

#include "LWM2M_MessData_Resources.h"
#include "LWM2M_Flow_Resources.h"
#include "LWM2M_BME280_Resources.h"

static const ST_LWM2M_RESOURCE_ITEM_t stResourceItems[] =
{
    {
        LWM2M_TYPE_FLOAT           |
        LWM2M_ACCESS_RD            |
        LWM2M_EXECUTE_GET_BEFORE   |
        LWM2M_USE_TEMP_DATA,
        LWM2M_MESS_DATA_VIS_DATA_RES_ID,                // resource-ID
        &(gstSi1132_MD.wAvgVIS),
        0,                                              // setzen geht nicht
        get_uint16_to_float,                            // 16-Bit nach 32-Bit wandeln
        0,                                              // keine Parameter
    },
    {                                                   // Derzeitiger Messwert
        LWM2M_TYPE_FLOAT           |
        LWM2M_ACCESS_RD            |
        LWM2M_EXECUTE_GET_BEFORE   |
        LWM2M_USE_TEMP_DATA,
        LWM2M_MESS_DATA_TEMP_DATA_RES_ID,               // resource-ID
        &(gstBME280_MD.stCur.i32Temp),
        0,                                              // setzen geht nicht
        get_BME280_Data,                                // beim lesen int32 in float umwandeln
        (void*)&gcstBME280_Temp_Par,                    // Parameter für Umwandlung (faktor 0.01)
    },
    {
        LWM2M_TYPE_FLOAT           |
        LWM2M_ACCESS_RD            |
        LWM2M_EXECUTE_GET_BEFORE   |
        LWM2M_USE_TEMP_DATA,
        LWM2M_MESS_DATA_HUMITY_DATA_RES_ID,             // resource-ID
        &(gstBME280_MD.stCur.u32Humity),
        0,                                              // setzen geht nicht
        get_int32_to_float,                             // beim lesen int32 in float umwandeln
        (void*)&gcstBME280_Humity_Par,                  // Parameter für Umwandlung (faktor 1/1024)
    },
    {
        LWM2M_TYPE_FLOAT           |
        LWM2M_ACCESS_RD            |
        LWM2M_EXECUTE_GET_BEFORE   |
        LWM2M_USE_TEMP_DATA,
        LWM2M_MESS_DATA_PRESS_DATA_RES_ID,              // resource-ID
        &(gstBME280_MD.stCur.u32Pressure),
        0,                                              // setzen geht nicht
        get_BME280_Data,                                // beim lesen int32 in float umwandeln
        (void*)&gcstBME280_Pressure_Par,                // Parameter für Umwandlung (faktor 0.01)
    },
    {
        LWM2M_TYPE_FLOAT           |
        LWM2M_ACCESS_RD            |
        LWM2M_EXECUTE_GET_BEFORE   |
        LWM2M_USE_TEMP_DATA,
        LWM2M_MESS_DATA_FLOW_X_DATA_RES_ID,             // resource-ID
        &(gstFlow_MD.iFlowX_avg),
        0,                                              // setzen geht nicht
        get_Flow_Data,                                  // beim lesen int16 in float umwandeln
        (void*)&gcstFlow_Par,                           // Parameter für Umwandlung (faktor 0.01)
    },
    {
        LWM2M_TYPE_FLOAT           |
        LWM2M_ACCESS_RD            |
        LWM2M_EXECUTE_GET_BEFORE   |
        LWM2M_USE_TEMP_DATA,
        LWM2M_MESS_DATA_FLOW_Y_DATA_RES_ID,             // resource-ID
        &(gstFlow_MD.iFlowY_avg),
        0,                                              // setzen geht nicht
        get_Flow_Data,                                  // beim lesen int16 in float umwandeln
        (void*)&gcstFlow_Par,                           // Parameter für Umwandlung (faktor 0.01)
    },
};

const ST_LWM2M_RESOURCE_ITEMS_t stMessDataResources =
{
   .uiItemCount = GET_ITEM_COUNT(stResourceItems),
   .pstItems    = stResourceItems,
};