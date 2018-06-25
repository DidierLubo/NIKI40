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
#ifndef RTC_CE_INCLUDED

#define RTC_CE_INCLUDED

typedef struct rtc_s
{
  uint8_t second;
  uint8_t minute;
  uint8_t hour;
  uint8_t dow;
  uint8_t day;
  uint8_t month;
  uint16_t year;
} rtc_t;


extern rtc_t gstrRTC_Time;

extern __monitor rtc_t *RTC_CE_Get_Time(void);
extern void RTC_CE_Set_Time(rtc_t *lptrNewTime);
extern void RTC_CE_Init(void);


#endif











