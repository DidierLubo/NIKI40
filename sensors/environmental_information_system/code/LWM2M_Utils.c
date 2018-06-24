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

#define TASK_MAX_STEP_TIME          0x70000000        // Maximale Zeit zwischen zwei Task-Steps

//==============================================================================================================================
// Umrechnung der internen Zeiten (ms) ins Sekunden für die Objekte
//==============================================================================================================================
const ST_LWM2M_OBJECT_FLOAT_PARAM_t cstObjectTime2Time =
{                                                     // wir rechnen mit ms objectime in sekunden
   .min        = 0.002f,                              // mindestens 2ms Idel
   .max        = 0.001f * TASK_MAX_STEP_TIME,
   .set_faktor = 1000.0f,
   .get_faktor = 0.001f,
};

//==============================================================================================================================
// Nun die Funktionen um Set/Get der Objekte und deren Umrechnung durchzuführen
//==============================================================================================================================

//==============================================================================================================================
// Umrechnung (ms in sekudnen) und Umwandlung in Float
//==============================================================================================================================
int32_t getTime(uint8_t *pbBuf, uint32_t uiLen, struct ST_LWM2M_RESOURCE_ITEM const *pItem, uint32_t bInstID)
{
   int32_t                       *pData   = (int32_t*)                        (pItem->pvData);
   ST_LWM2M_OBJECT_FLOAT_PARAM_t *pstParam = (ST_LWM2M_OBJECT_FLOAT_PARAM_t*) (pItem->pvParam);
   if ((pItem->uiTypeFLGs & LWM2M_TYPE_MASK) == (LWM2M_TYPE_FLOAT & LWM2M_TYPE_MASK))
   {
      gunAllTemp.fFloat = ((float)*pData) * pstParam->get_faktor;   // hier von uint32 nach float wandeln und mit faktor (0.001) multiplizieren
      return 4;
   }
   return -1;                                         // Falscher Datentyp
}

//==============================================================================================================================
// Umrechnung (sekudnen in ms) und Umwandlung in int32 + Range-Check
//==============================================================================================================================
int32_t setTime(uint8_t *pbBuf, uint32_t uiLen, struct ST_LWM2M_RESOURCE_ITEM const *pItem, uint32_t bInstID)
{
   int32_t                       *pData   = (int32_t*)                       (pItem->pvData);
   ST_LWM2M_OBJECT_FLOAT_PARAM_t *pstParam = (ST_LWM2M_OBJECT_FLOAT_PARAM_t*) (pItem->pvParam);

   if ((pItem->uiTypeFLGs & LWM2M_TYPE_MASK) == (LWM2M_TYPE_FLOAT & LWM2M_TYPE_MASK))
   {
      float f = gunAllTemp.fFloat;
      if (f > pstParam->max || f < pstParam->min)     // range-Check (Nach vor Skalierung da Range in Sekunden)
         return -2;                                   // Fehler melden

      f *= pstParam->set_faktor;                      // Sekunden in ms umrechnen
      *pData = (uint32_t) f;                                // hier Daten in gSYS-Struktur speichern
      return 0;
   }
   return -1;                                         // Falscher Datentyp
}

//==============================================================================================================================
// Algemeine Funktion zur Umwandlung int32 nach float mit skalierung (get-Faktor)
//==============================================================================================================================
int32_t set_float_to_uint32(uint8_t *pbBuf, uint32_t uiLen, struct ST_LWM2M_RESOURCE_ITEM const *pItem, uint32_t bInstID)
{
   int32_t *pData = (int32_t*) (pItem->pvData);       // Zeiger aufs Ziel z. B. gSYS..Setting

   ST_LWM2M_OBJECT_FLOAT_PARAM_t *stParam = (ST_LWM2M_OBJECT_FLOAT_PARAM_t *) (pItem->pvParam);

   float f = gunAllTemp.fFloat;
   if (f > stParam->max || f < stParam->min)          // range-Check (vor Skalierung)
      return -11;                                     // Fehler melden

   f *= stParam->set_faktor;                          // float-Zahl skalieren

   *pData = (uint32_t) f;                             // hier Daten als uint32 speichern (z. B. in gSYS..
   return 4;
}

//==============================================================================================================================
// Algemeine Funktion zur Umwandlung int32 nach float mit skalierung (get-Faktor)
//==============================================================================================================================
int32_t get_int32_to_float(uint8_t *pbBuf, uint32_t uiLen, struct ST_LWM2M_RESOURCE_ITEM const *pItem, uint32_t bInstID)
{
   int32_t   *pData = (int32_t*)(pItem->pvData);      // int32_t wert aus Speicher lesen
   gunAllTemp.fFloat = (float) *pData;                 // Wert in float wandeln
   if (pItem->pvParam)
   {                                                  // falls es ein Faktor zur skalierung gibt diesen jetzt anwenden
      ST_LWM2M_OBJECT_FLOAT_PARAM_t *stParam = (ST_LWM2M_OBJECT_FLOAT_PARAM_t *) (pItem->pvParam);
      gunAllTemp.fFloat *= stParam->get_faktor;
   }
   return 4;
}

int32_t get_int16_to_int32(uint8_t *pbBuf, uint32_t uiLen, struct ST_LWM2M_RESOURCE_ITEM const *pItem, uint32_t bInstID)
{
   int16_t *pData = (int16_t*) (pItem->pvData);       // 16-Bit int Daten-Zeiger
   gunAllTemp.i32Int = *pData;                         // nach gunAllTemp 32-Bit Wert laden
   return 4;
}

int32_t get_uint16_to_float(uint8_t *pbBuf, uint32_t uiLen, struct ST_LWM2M_RESOURCE_ITEM const *pItem, uint32_t bInstID)
{
   uint16_t  *pData = (uint16_t*)(pItem->pvData);     // uint16_t wert aus Speicher lesen
   gunAllTemp.fFloat = (float) *pData;                 // Wert in float wandeln
   if (pItem->pvParam)
   {                                                  // falls es ein Faktor zur skalierung gibt diesen jetzt anwenden
      ST_LWM2M_OBJECT_FLOAT_PARAM_t *stParam = (ST_LWM2M_OBJECT_FLOAT_PARAM_t *) (pItem->pvParam);
      gunAllTemp.fFloat *= stParam->get_faktor;
   }
   return 4;
}

int32_t get_int16_to_float(uint8_t *pbBuf, uint32_t uiLen, struct ST_LWM2M_RESOURCE_ITEM const *pItem, uint32_t bInstID)
{
   int16_t  *pData = (int16_t*)(pItem->pvData);       // int16_t wert aus Speicher lesen
   gunAllTemp.fFloat = (float) *pData;                 // Wert in float wandeln
   if (pItem->pvParam)
   {                                                  // falls es ein Faktor zur skalierung gibt diesen jetzt anwenden
      ST_LWM2M_OBJECT_FLOAT_PARAM_t *stParam = (ST_LWM2M_OBJECT_FLOAT_PARAM_t *) (pItem->pvParam);
      gunAllTemp.fFloat *= stParam->get_faktor;
   }
   return 4;
}
