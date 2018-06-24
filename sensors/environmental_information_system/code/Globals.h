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
//*  Globale Strukturen "globals.h"                                                                                      *
//*  Erstellt am 3.07.2013                                                                                              *
//************************************************************************************************************************

#ifndef __GLOBALS__
#define __GLOBALS__

#include <stdint.h>

#include "Si1132.h"
#include "LWM2M.h"

typedef union
{
   uint8_t    b[4];
   int16_t    i[2];
   uint16_t   w[2];
   uint32_t   dw;
   int32_t    l;
   float      f;
}  UN_LE_4B;

typedef union
{
   uint8_t    b[2];
   int16_t    i;
   uint16_t   w;
}  UN_LE_2B;

#define RADIO_TYPE_HSU        0
#define RADIO_TYPE_ENOCEAN    1

typedef struct                                        // Konfigurationsvariablen
{
   uint32_t ulPressureMessInterval;                   // BME280:  Messinterval für Druck/Temperatur/Feuchte

   uint32_t ulLightMessInterval;                      // Si1132:  Messinterval für Licht
   uint8_t  bLightMessAnzahl;                         // Si1132:  Anzahl der Lichtmessungen über die gemittelt werden soll
   ST_Si1132_CAL_VALUES stSi1132_CalVal;              // Si1132:  Cal-Values für den Sensor (Dark-Offset-Values / Skalenfaktor / ..)

   uint32_t ulFlowMessInterval;                       // 2D-Flow: Messintervall
   uint8_t  bFlowMessAnzahl;                          // 2D-Flow: Anzahl der Messungen über die gemittelt werden soll
   uint16_t uiFlowDAC_Heizer;                         // 2D-Flow: Heizer DAC-Wert

   uint32_t ulEE893_MessInterval;                     // EE893:   Messintervall

   BYTE bDeviceName[10];
   BYTE bSerienNr[10];                                // Seriennummer
   BYTE bRadioID[4];                                  // Radio-Mac-Adresse (ID) ab 22.2.16 hier gespeichert
   BYTE bRadioType;                                   //

   ST_LWM2M_CFG_t                stLWM2M_SET;
   ST_CC1350_GENERAL_API_SET_t   stCC1350_SET;

   WORD wCRC;                                         // CRC- Check zum Speichercheck
}  ST_SYS;

extern ST_SYS gSYS;

#define USB_OF_BME280_PTH              BIT0           // gesetzt: BME280-Daten über USB ausgeben
#define USB_OF_Si1132                  BIT1           // gesetzt: Si1132-Daten über USB ausgeben
#define USB_OF_2D_FLOW                 BIT2           // gesetzt: 2D-Flow-Daten über USB ausgeben
#define USB_OF_EE893                   BIT3           // gesetzt: EE893-Daten ausgeben

#define USB_OF_SEND_DATA               BIT7           // Gesetzt -> IRQ Ausgabe aktivieren

extern volatile BYTE gbUSB_Out_Flg;                   // Nach globals gebracht, damit usbEventHandling.c die Ausgabe der Daten steuern kann

#define OLED_OF_NEW_DATA              BIT0            // Gesetzt -> neue Messdaten zur Anzeige bereit
#define OLED_OF_DISP_DATA             BIT7            // Gesetzt -> OLED-Ausgabe aktivieren
extern volatile BYTE gbOLED_Out_Flg;

extern const BYTE gbVersion[];
BYTE sizeof_gbVersion (void);

#endif