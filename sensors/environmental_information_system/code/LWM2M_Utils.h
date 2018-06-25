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

#ifndef __LWM2M_UTILS__
#define __LWM2M_UTILS__

#include "hardware.h"
#include "stdint.h"
#include "globals.h"
#include "LWM2M.h"
#include "Radio_Task.h"

#define GET_ITEM_COUNT(x) (sizeof(x) / sizeof(ST_LWM2M_RESOURCE_ITEM_t))

extern const ST_LWM2M_OBJECT_FLOAT_PARAM_t cstObjectTime2Time;

//==============================================================================================================================
// Umrechnung (ms in sekudnen) und Umwandlung in Float
//==============================================================================================================================
int32_t getTime(uint8_t *pbBuf, uint32_t uiLen, struct ST_LWM2M_RESOURCE_ITEM const *pItem, uint32_t bInstID);

//==============================================================================================================================
// Umrechnung (sekudnen in ms) und Umwandlung in int32 + Range-Check
//==============================================================================================================================
int32_t setTime(uint8_t *pbBuf, uint32_t uiLen, struct ST_LWM2M_RESOURCE_ITEM const *pItem, uint32_t bInstID);

//==============================================================================================================================
// Algemeine Funktion zur Umwandlung int32 nach float mit skalierung (get-Faktor)
//==============================================================================================================================
int32_t set_float_to_uint32(uint8_t *pbBuf, uint32_t uiLen, struct ST_LWM2M_RESOURCE_ITEM const *pItem, uint32_t bInstID);

//==============================================================================================================================
// Algemeine Funktion zur Umwandlung int32 nach float mit skalierung (get-Faktor)
//==============================================================================================================================
int32_t get_int32_to_float(uint8_t *pbBuf, uint32_t uiLen, struct ST_LWM2M_RESOURCE_ITEM const *pItem, uint32_t bInstID);

int32_t get_int16_to_int32(uint8_t *pbBuf, uint32_t uiLen, struct ST_LWM2M_RESOURCE_ITEM const *pItem, uint32_t bInstID);

int32_t get_uint16_to_float(uint8_t *pbBuf, uint32_t uiLen, struct ST_LWM2M_RESOURCE_ITEM const *pItem, uint32_t bInstID);

int32_t get_int16_to_float(uint8_t *pbBuf, uint32_t uiLen, struct ST_LWM2M_RESOURCE_ITEM const *pItem, uint32_t bInstID);
#endif