/*
        Copyright 2016-2018 NIKI 4.0 project team
        NIKI 4.0 was financed by the Baden-W�rttemberg Stiftung gGmbH (www.bwstiftung.de).
        Project partners are FZI Forschungszentrum Informatik am Karlsruher
        Institut f�r Technologie (www.fzi.de), Hahn-Schickard-Gesellschaft
        f�r angewandte Forschung e.V. (www.hahn-schickard.de) and
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

#ifndef __TIMER_MODUL__
#define __TIMER_MODUL__


//========================================================================================================================
//    Das ist der ms-Timer z. B. f�r Timeouts
//========================================================================================================================
void InitTimerA0(void);

void SetTimeoutValue( uint16_t uiTime_ms );
uint16_t GetTimeoutValue(void);

#endif