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
//************************************************************************************************************************
//*  OLED_TExte                                                                                                          *
//*  Erstellt am 02.12.2013                                                                                              *
//************************************************************************************************************************
//*                                                                                                                      *
//*  Hier werden die Texte vom Spirometer gespeichert und in einer cpbTextArray-Tabelle gespeichert                      *
//*  Damit ist zum anzeigen eines Textes nur noch dessen Index-Nummer notwendig                                          *
//*  Dies erleichtert die Übersetzung in andere Sprachen                                                                 *
//*                                                                                                                      *
//************************************************************************************************************************


//                                                  123456789012345678901 - eine Zeile hat max. 21 Zeichen
const BYTE __data20 cpbText_Main_SystemReady[]   = "System bereit";
const BYTE __data20 cpbText_Main_BT_Off[]        = "Bluetooth aus";
const BYTE __data20 cpbText_Main_BT_On[]         = "Bluetooth ein";
const BYTE __data20 cpbText_Main_BT_Error[]      = "Bluetooth error";

const BYTE __data20 cpbText_USB_AD_Start[]       = "USB-ASCII     Ausgabe";
const BYTE __data20 cpbText_USB_AD_Start1[]      = "USB-BASE128   Ausgabe";
const BYTE __data20 cpbText_USB_AD_Start2[]      = "USB-ASCII     iFiL";
const BYTE __data20 cpbText_USB_AD_Start3[]      = "USB-BASE128   iFiL";
const BYTE __data20 cpbText_USB_AD_Stop[]        = "USB-Ausgabe  gestoppt";
const BYTE __data20 cpbText_USB_CMD_unkown[]     = "USB-CMD     unbekannt";

const BYTE __data20 cpbText_BT_Version[]         = "BT: Version";
const BYTE __data20 cpbText_BT_AD_Start[]        = "BT: AD-Start";
const BYTE __data20 cpbText_BT_AD_Stop[]         = "BT: AD-Stop";
const BYTE __data20 cpbText_BT_TimeOut[]         = "BT: Timeout";

const BYTE  cpbText_Akku[]                       = "Akku:";
const BYTE  cpbText_AkkuStatus_ok[]              = "ok";
const BYTE  cpbText_AkkuStatus_laden[]           = "lädt";
const BYTE  cpbText_AkkuStatus_leer[]            = "leer";
const BYTE  cpbText_AkkuStatus_voll[]            = "voll";
const BYTE  cpbText_AkkuStatus_unknown[]         = "?";

const BYTE  cpbText_Inhale[]                     = "Inhale";
const BYTE  cpbText_Exhale[]                     = "Exhale";

const BYTE  cpbText_Pressure[]                   = "Pressure";
                                                 // 123456789012345678901 - eine Zeile hat max. 21 Zeichen
                                                 //"CO2: 5.5% Temp.:-20°C"
const BYTE  cpbText_CO2_line[]                   = "CO2: %hhi.%01hhi%% Temp.:%i°C";


//------------------------------------------------------------------------------------------------------------------------
// Achtung
//------------------------------------------------------------------------------------------------------------------------
// die Reiehnfolge im cpbTextArray muss mit den Indexnummern in OLED.h übereinstimmen!!!
//------------------------------------------------------------------------------------------------------------------------
const BYTE __data20 *cpbTextArray[] =
{                                                     // Defines für IDX_TEXT sind in OLED.h weil diese Header-Datei
                                                      // überall geladen wird, wo Display-Ausgaben erfolgen

   cpbText_Main_SystemReady,                          // IDX_TEXT_Main_SystemReady         0
   cpbText_Main_BT_Off,                               // IDX_TEXT_Main_BT_Off              1
   cpbText_Main_BT_On,                                // IDX_TEXT_Main_BT_On               2
   cpbText_Main_BT_Error,
   cpbText_USB_AD_Start,                              // IDX_TEXT_USB_AD_Start             3
   cpbText_USB_AD_Start1,                             // IDX_TEXT_USB_AD_Start1            4
   cpbText_USB_AD_Start2,                             // IDX_TEXT_USB_AD_Start2            5
   cpbText_USB_AD_Start3,                             // IDX_TEXT_USB_AD_Start3            6
   cpbText_USB_AD_Stop,                               // IDX_TEXT_USB_AD_Stop              7
   cpbText_USB_CMD_unkown,                            // IDX_TEXT_USB_CMD_unkown           8
   cpbText_BT_Version,                                // IDX_TEXT_BT_Version               9
   cpbText_BT_AD_Start,                               // IDX_TEXT_BT_AD_Start             10
   cpbText_BT_AD_Stop,                                // IDX_TEXT_BT_AD_Stop              11
   cpbText_BT_TimeOut                                 // IDX_TEXT_BT_TIME_OUT             12
};
