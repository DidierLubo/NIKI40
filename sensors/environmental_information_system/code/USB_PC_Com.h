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
//*  USB_PC_com                                                                                                          *
//*  Erstellt am 27.02.2013                                                                                              *
//************************************************************************************************************************
//*  Funktion des Programms                                                                                              *
//*  Übernimmt die Kommunikation mit dem PC                                                                              *
//*  Hier werden die PC-Kommandos emfangen und ausgewertet                                                               *
//************************************************************************************************************************

#ifndef __USB_PC_COM__
#define __USB_PC_COM__

#include "BME280.h"
#include "Si1132.h"
#include "2D_Flow.h"
#include "EE893.h"

extern volatile BYTE gbCDCDataReceived_event;         // Flag set by event handler to indicate data has been
                                                      // received into USB buffer
extern volatile BYTE gbUSB_SendResultFlg;             // Flag um Sensor-Daten über USB zu senden
extern BYTE gbUSB_Buffer[256];                        // Zwischenspeicher um Messdaten über USB zu übertragen

//------------------------------------------------------------------------------------------------------------------------
// Aufruf durch ADXL362 wenn neue Messdaten vorhanden sind
//------------------------------------------------------------------------------------------------------------------------
void USB_SendResult(int iResult[3]);

//------------------------------------------------------------------------------------------------------------------------
// USB_Task:   Überwacht die USB-Aktivtäten. Wird auch von anderen Modulen aufgerufen
//------------------------------------------------------------------------------------------------------------------------
// return true wenn PC-Treiber geladen
//------------------------------------------------------------------------------------------------------------------------
BYTE USB_Task(void)     ;

//------------------------------------------------------------------------------------------------------------------------
// Aufruf durch Task-Manager wenn neue Mess-Daten vorhanden sind
//------------------------------------------------------------------------------------------------------------------------
void USB_Send_BME280_Data( ST_BME280_MESS_DATA *stM );
void USB_Send_Si1132_Data( ST_Si1132_MessData *pstM );
void USB_Send_Flow_Data( ST_FLOW_MESS_DATA *pstM );
void USB_Send_EE893_Data (ST_EE893_MESS_DATA *pstM );

#endif