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
/*
;*************************************************************************************************************************
;								                                                                                                 *
;       Timer - Modul (Include-File)                                                                                     *
;	                                                                                                                      *
;*************************************************************************************************************************
*/

#include "Hardware.h"

volatile uint16_t muiTimer = 0;                       // Rückwärtszähler für Timeout

uint16_t GetTimeoutValue(void)
{
   return muiTimer;
}

void SetTimeoutValue( uint16_t uiTime_ms )
{
   muiTimer = uiTime_ms;
}

void InitTimerA0(void)
{
   TA0CCTL0 = CCIE;
   TA0CCR0  = 41;//10ms compare value
   TA0CTL   = TASSEL__ACLK | ID__8 | MC__UP | TACLR;
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0(void)
{
   if (muiTimer)
      muiTimer--;
}