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
#ifndef __LWM2M_BME280_Resources__
#define __LWM2M_BME280_Resources__

extern const ST_LWM2M_RESOURCE_ITEMS_t stBME280_Temp_Items;
extern const ST_LWM2M_RESOURCE_ITEMS_t stBME280_Pressure_Items;
extern const ST_LWM2M_RESOURCE_ITEMS_t stBME280_Humity_Items;
extern const ST_LWM2M_RESOURCE_ITEMS_t stBME280_Init_Items;

//==============================================================================================================================
// Wegen gemeinsamen Massdata-Object
//==============================================================================================================================
extern const ST_LWM2M_OBJECT_FLOAT_PARAM_t gcstBME280_Temp_Par;
extern const ST_LWM2M_OBJECT_FLOAT_PARAM_t gcstBME280_Pressure_Par;
extern const ST_LWM2M_OBJECT_FLOAT_PARAM_t gcstBME280_Humity_Par;

int32_t get_BME280_Data(uint8_t *pbBuf, uint32_t uiLen, struct ST_LWM2M_RESOURCE_ITEM const *pItem, uint32_t bInstID);

#endif