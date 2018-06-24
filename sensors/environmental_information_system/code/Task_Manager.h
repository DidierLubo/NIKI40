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
//*  Taskmanager                                                                                                         *
//************************************************************************************************************************
//*                                                                                                                      *
//*  Aufgaben:                                                                                                           *
//*                                                                                                                      *
//*  Alle Task's zu verwalten                                                                                            *
//*  Low-Power aktivieren wenn möglich                                                                                   *
//*  Verwendet nur den Timer A1                                                                                          *
//*                                                                                                                      *
//************************************************************************************************************************

#ifndef __TASK_MANAGER__
#define __TASK_MANAGER__

#include  "stdint.h"

typedef enum
{
   TASK_IDEL                     = 0x00,              // Initialisiert alles (z. B Auswerte Variablen) und geht dann in den Zustand TASK_WAIT_FOR_MESS_START

   TASK_WAIT_FOR_MESS_START      = 0x01,              // Vorbereitungen für die Messung: z. B. ADC-Ref einschalten, Heizer einschalten, ...
   TASK_WAIT_FOR_MESS_START1     = 0x02,              // Evtl. weitere Vorbereitungen zum Messen

   TASK_MESSUNG_START            = 0x03,              // hier die Messung einleiten (Sensoren aktivieren, ...) ADC-IRQ, ...
   TASK_MESSUNG_READ_SENSOR      = 0x04,              // hier die Sensordaten einlesen min/max/avg bilden etc. falls notwendig eine neue Sensormessung beginnen

   TASK_MESSUNG_READY            = 0x05,              // Messdaten nun in interner structur speichern
   TASK_SEND_DATA                = 0x06,              // Hier sollen die Messdaten vom Taskmanger abgeholt und an Funk/USB/RF430/OLED etc. verteilt werden
                                                      // Weitere States nur bis 0x0F möglich, da es sonst zu Überlappung mit den LPM-Bits kommt
   TASK_FUNC_INDEX_MASK          = 0x0F,

   TASK_STEP0                    = 0x00,
   TASK_STEP1                    = 0x01,
   TASK_STEP2                    = 0x02,
   TASK_STEP3                    = 0x03,
   TASK_STEP4                    = 0x04,
   TASK_STEP5                    = 0x05,
   TASK_STEP6                    = 0x06,
   TASK_STEP7                    = 0x07,
   TASK_STEP8                    = 0x08,
   TASK_STEP9                    = 0x09,
   TASK_STEP10                   = 0x0A,
   TASK_STEP11                   = 0x0B,
   TASK_STEP12                   = 0x0C,
   TASK_STEP13                   = 0x0D,
   TASK_STEP14                   = 0x0E,
   TASK_STEP15                   = 0x0F,

   TASK_NO_LPM                   = 0x00,
   TASK_LPM0                     = LPM0_bits,         // die LPM-bits liegen zwischen 0x10 und 0x80 also im oberen Nibble eines Bytes
   TASK_LPM1                     = LPM1_bits,
   TASK_LPM2                     = LPM2_bits,
   TASK_LPM3                     = LPM3_bits,
   TASK_LPM4                     = LPM4_bits,
   TASK_LPM_BIT_MASK             = LPM4_bits
} EN_TASK_STATUS;

typedef struct
{
   EN_TASK_STATUS (*pFunc)(void);                     // Zeiger auf Funktion
   uint32_t       ulTime;                             // Zeit die anschließend vergeht, bis zum nächsten Eintrag gewechselt wird
} ST_FUNCTION_TABLE;


extern volatile uint32_t mulMSP430Ticks;
inline uint32_t HAL_GetTickCount(void)                // Wegen der Timersteuerung hier her verlegt
{
   return  mulMSP430Ticks;
}

void HAL_Delay_ms( uint32_t ulTime_ms, uint16_t uiLPM_Bits  );              // Wegen der Timersteuerung hierher verlegt

//------------------------------------------------------------------------------------------------------------------------
//    Initialisiert die Zeit bis zur nächsten Task-Ausführung neu, wenn sich die Task im angegebenen Step befindet
//------------------------------------------------------------------------------------------------------------------------
//    pstFT:                  Pointer auf Task-Tabelle
//    bStep:                  Step der Task = Index in der Task-Tabelle
//------------------------------------------------------------------------------------------------------------------------
void Task_InitTaskExecutionTime( ST_FUNCTION_TABLE *pstFT, BYTE bStep);

void Init_Task_Manager(void);
__noreturn void Task_Manager(void);

EN_TASK_STATUS Task_Send_BME280_Daten( void );
EN_TASK_STATUS Task_Send_Si1132_Daten( void );
EN_TASK_STATUS Task_Send_Flow_Daten(void);
EN_TASK_STATUS Task_Send_EE893_Daten(void);

EN_TASK_STATUS Task_StreamSend_Flow_Daten(void);
EN_TASK_STATUS Task_StreamSend_Si1132_Daten( void );

extern volatile uint8_t gbRadioStreamingMode;

inline void Task_GoNormalMode(void)
{
   gbRadioStreamingMode = false;
}

void Task_GoStreamingMode(void);

inline uint8_t isTaskInStreamingMode()
{
   return gbRadioStreamingMode;
}

inline uint8_t isTaskInNormalMode()
{
   return gbRadioStreamingMode==0;
}

#endif