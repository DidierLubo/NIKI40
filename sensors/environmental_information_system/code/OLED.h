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
#ifndef _LCD
#define _LCD

#include "Hardware.h"
#include "BME280.h"                                   // Um BME280 Messdaten abzuholen und zu verteilen
#include "Si1132.h"                                   // Um Si1132 Messdaten abzuholen und zu verteilen
#include "2D_Flow.h"                                  // Um Flow   Messdaten abzuholen und zu verteilen
#include "EE893.h"

#define LCD_FONT_HEIGHT 8
#define LCD_FONT_WIDTH  6

#define OLED_PRINTF_BUF_SIZE           171

//------------------------------------------------------------------------------------------------------------------------
// Achtung
//------------------------------------------------------------------------------------------------------------------------
// die nachfolgenden defines müssen mit dem in OLED_Texte.h definierten cpbTextArray übereinstimmen!!!
//------------------------------------------------------------------------------------------------------------------------
#define IDX_TEXT_Main_SystemReady         0
#define IDX_TEXT_Main_BT_Off              1
#define IDX_TEXT_Main_BT_On               2
#define IDX_TEXT_Main_BT_Error            3

#define IDX_TEXT_USB_AD_Start             4
#define IDX_TEXT_USB_AD_Start1            5
#define IDX_TEXT_USB_AD_Start2            6
#define IDX_TEXT_USB_AD_Start3            7
#define IDX_TEXT_USB_AD_Stop              8
#define IDX_TEXT_USB_CMD_unkown           9

#define IDX_TEXT_BT_Version              10
#define IDX_TEXT_BT_AD_Start             11
#define IDX_TEXT_BT_AD_Stop              12

#define IDX_TEXT_BT_TIME_OUT             13

#define OLED_AKKU_LINE                    7
#define OLED_STATUS_LINE                  0

void OLED_Init();
//void OLED_Task( void );
void OLED_Clear(void);
void OLED_WriteStatusLine (const char *fmt, ...);
void OLED_UpdateStatusTextNo(BYTE bTextNumber);

void OLED_Set_Contrast(unsigned char contrast);
void OLED_Set_Position(unsigned char x, unsigned int y);
void OLED_LCD_Write(unsigned char *a, int len);
void OLED_printf (const char *fmt, ...);

void OLED_Show_EE893_Data( ST_EE893_MESS_DATA *pstM );   // Messdaten im OLED anzeigen
void OLED_Show_Flow_Data( ST_FLOW_MESS_DATA   *pstM );   // Messdaten im OLED-Display anzeigen
void OLED_Show_Si1132_Data(ST_Si1132_MessData *pstM );   // Messdaten im OLED-Display anzeigen
void OLED_Show_BME280_PTH(ST_BME280_MESS_DATA *pstM );   // Messdaten im OLED-Display anzeigen
void OLED_Show_Akku_Data( uint8_t bRelPower );           // Messdaten im OLED-Display anzeigen

//------------------------------------------------------------------------------------------------------------------------
//    OLED_Write:       Gibt einen Text ab der aktuellen Display-Position aus. Text wird entweder durch
//                      ein '0'-Zeichen (C-String-Ende) oder Länge l begrenzt
//                      Die Zeichen werden doppelt so groß wie normal ausgegeben
//------------------------------------------------------------------------------------------------------------------------
void OLED_WriteDoubleSize(BYTE __data20 *bText, int l, BYTE bFill);

void OLED_Off(void);

#define OLED_NO_TIMEOUT                0
#define OLED_NFC_TIMEOUT               30
//------------------------------------------------------------------------------------------------------------------------
//    Timeout in Sekunden. Bei OLED_NO_TIMEOUT bleibt OLED immer an
//------------------------------------------------------------------------------------------------------------------------
void OLED_On( uint16_t uTimeOutSek );

// Task nur für Timeout-Überwachung
extern ST_FUNCTION_TABLE gstOLED_Func_Table[];

#endif