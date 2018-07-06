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
#include "Hardware.h"
#include "Globals.h"

#include "Hal.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "font.h"
#include "OLED.h"
#include "OLED_Texte.h"

BYTE mbLineNo = 0;
BYTE mbColNo  = 0;
uint16_t muiTimeOutSek = 0;                           // hier die Timeout-Zeit in Sekunden bis OLED ausgeschaltet wird. Bei 0 erfolgt keine Timeout-Überwachung

volatile BYTE gbOLED_Out_Flg = OLED_OF_DISP_DATA;     // Achtung globaler Zugriff!!

//unsigned long mulStartTicksCount = 0;                 // TickCount bei Text-Anzeige
void OLED_On( uint16_t uiTimeoutSek );

void OLED_WR_CMD(BYTE bCMD)
{
   OLED_DATA_DIR = 0xFF;                              // Port auf Ausgabe schalten
   OLED_DATA_OUT = bCMD;
   OLED_SET_CMD_MODE;                                 // CMD-Mode wählen
   OLED_WR_LOW;                                       // WR auf low
   OLED_CS_LOW;                                       // CS auf low
   _NOP();
   OLED_WR_HIGH;                                      // WR-Signal high setzen und damit die Datenübernahme an den SSD1309-Controller durchführen
   _NOP();
   OLED_CS_HIGH;                                      // CS-Signal high setzen und damit die Kommunikation abschließen
}

void OLED_WR_Data(BYTE bDATA)
{
   OLED_DATA_DIR = 0xFF;                              // Port auf Ausgabe schalten
   OLED_DATA_OUT = bDATA;
   OLED_SET_DATA_MODE;                                // DATA-Mode wählen
   OLED_WR_LOW;                                       // WR auf low
   OLED_CS_LOW;                                       // CS auf low
   _NOP();
   OLED_WR_HIGH;                                      // WR-Signal high setzen und damit die Datenübernahme an den SSD1309-Controller durchführen
   _NOP();
   OLED_CS_HIGH;                                      // CS-Signal high setzen und damit die Kommunikation abschließen
}

BYTE OLED_RD_Status(void)
{
   BYTE bRetValue;

   OLED_DATA_DIR = 0x00;                              // Port auf Eingabe schalten
   OLED_SET_CMD_MODE;                                 // CMD-Mode wählen
   OLED_RD_LOW;                                       // RD auf low
   OLED_CS_LOW;                                       // CS auf low
   _NOP();
   bRetValue = OLED_DATA_IN;
   OLED_RD_HIGH;                                      // WR-Signal high setzen und damit die Datenübernahme an den SSD1309-Controller durchführen
   _NOP();
   OLED_CS_HIGH;                                      // CS-Signal high setzen und damit die Kommunikation abschließen
   return bRetValue;
}

BYTE OLED_RD_Data(void)
{
   BYTE bRetValue;

   OLED_DATA_DIR = 0x00;                              // Port auf Eingabe schalten
   OLED_SET_DATA_MODE;                                // Data-Mode wählen
   OLED_RD_LOW;                                       // RD auf low
   OLED_CS_LOW;                                       // CS auf low
   _NOP();
   bRetValue = OLED_DATA_IN;                          // Daten des SSD1309-Controllers sollten nun stabil anliegen
   OLED_RD_HIGH;                                      // RD-Signal high setzen
   _NOP();
   OLED_CS_HIGH;                                      // CS-Signal high setzen und damit die Kommunikation abschließen
   return bRetValue;
}

//------------------------------------------------------------------------------------------------------------------------
//    OLED_Init:        Initialisiert das Display und schaltet es ein
//------------------------------------------------------------------------------------------------------------------------
void OLED_Init()
{
   const BYTE bInitSeq[] =
   {
      0xFD,                                           // Command Un Lock
      0x12,
      0xAE,                                           // Display off
      0xD5,                                           // Set Display Clock Divide Ratio / Oscillator Frequency
      0xA0,
      0xA8,                                           // Set Multiplex-Ratio
      0x3F,
      0xD3,                                           // Set Display Offset
      0x00,
      0x40,                                           // Set Display Start-Line
//      0xA1,                                           // Set Segment Remap                          ( Text wie in Spirometer )
      0xA0,                                           // Set Segment Remap                            ( Text wie in EIS )
//      0xC8,                                           // Set COM Output Scan Direction (Remapped)   ( Text wie in Spirometer )
      0xC0,                                           // Set COM Output Scan Direction (Normal)       ( Text wie in EIS )

      0xDA,                                           // Set COM-Pins Hardware Configuration
      0x12,                                           // Set COL Output Scan Direction (Remapped)   ( Text wie in Spirometer )

      0x81,                                           // Set Current Control
      0xE3,
      0xD9,                                           // Set Pre-Charge Period
      0x19,
      0xDB,                                           // Set VCOMH Deselect Level
      0x34,
      0xA4,                                           // Set Entire Display on
      0xA6                                            // Set Normal Display (not inverse)
//      0x20                                            // Set Horizontal Addressing Mode   (geht nicht da sonst Set_Position nicht funktioniert
   };

   OLED_RESET_LOW;
   __delay_cycles(MCLK_FREQ * 0.00001f);              // 10µs low
   OLED_RESET_HIGH;
   __delay_cycles(MCLK_FREQ * 0.00001f);              // 10µs high bevor es weiter geht

   for (BYTE i=0; i<sizeof(bInitSeq); i++)
      OLED_WR_CMD( bInitSeq[i] );

   OLED_Clear();

   OLED_On(30);

   __delay_cycles(MCLK_FREQ * 0.1);                   // 0,1sec = 100ms warten (Recommended Delay)

}

void OLED_On( uint16_t uiTimeoutSek )
{
   muiTimeOutSek = uiTimeoutSek;
   OLED_POWER_ON;                                     // VCC=12,5V einschalten
   __delay_cycles(MCLK_FREQ * 0.01);                  // 0,01sec = 10ms warten
   OLED_WR_CMD( 0xAF );                               // Set Display on
}

void OLED_Off(void)
{
   OLED_WR_CMD( 0xAE );                               // Set Display off
   OLED_POWER_OFF;                                    // VCC=12,5V abschalten
}

//void OLED_Set_Contrast(BYTE contrast)
//{
//   BYTE bCMD[4] = { 0x80, 0x81, 0x80, 0 };
//   bCMD[3]      = contrast;
//   SendeI2C_Data( bCMD, sizeof bCMD, 0, 0);
//}

//------------------------------------------------------------------------------------------------------------------------
//    OLED_Set_Position:   Setzt die als nächstes zu benutzende Display-Position
//------------------------------------------------------------------------------------------------------------------------
void OLED_Set_Position(unsigned char x, unsigned int y)
{
   y &= 0x07;                                         // begrenze y auf den gültigen Wertebereich (0..7)
   OLED_WR_CMD( 0xB0 | y);                            // Setze die Page-Adresse

   x &= 0x7F;                                         // begrenze x auf den gültigen Wertebereich (0..127)
   OLED_WR_CMD( 0x00 | (x&0x0F));                     // Setze lower Column
   OLED_WR_CMD( 0x10 | (x>>4)  );                     // Setze upper Column

   mbLineNo = y;
   mbColNo  = x;
}

//------------------------------------------------------------------------------------------------------------------------
//    OLED_Clear:          Löscht das ganze Display
//------------------------------------------------------------------------------------------------------------------------
void OLED_Clear()
{
   for (BYTE line=0; line<OLED_LINES; line++)
   {
      OLED_WR_CMD( 0xB0 | line);                      // Setze die Page-Adresse

      for (BYTE col=0; col < OLED_WIDTH; col++)
         OLED_WR_Data( 0 );
   }
   OLED_Set_Position( 0, 0);
}

//------------------------------------------------------------------------------------------------------------------------
//    OLED_ClearFromTo:    Löscht einen Display-Bereich. y bezieht sich auf ganze Zeilen
//------------------------------------------------------------------------------------------------------------------------
void OLED_ClearFromTo(BYTE x_start, BYTE x_end, BYTE y_start,  BYTE y_end)
{
   for (BYTE bLine=y_start ; bLine<=y_end; bLine++)
   {
      OLED_Set_Position( x_start, bLine);

      for (BYTE col=x_start; col <= x_end; col++)
         OLED_WR_Data( 0 );
   }
   OLED_Set_Position( x_start, y_start );
}

//------------------------------------------------------------------------------------------------------------------------
//    OLED_Write:       Gibt einen Text ab der aktuellen Display-Position aus. Text wird entweder durch
//                      ein '0'-Zeichen (C-String-Ende) oder Länge l begrenzt
//                      Die Zeichen werden doppelt so groß wie normal ausgegeben
//------------------------------------------------------------------------------------------------------------------------
void OLED_WriteDoubleSize(BYTE __data20 *bText, int l, BYTE bFill)
{
   const BYTE bDoubleNibble[16] = { 0x00, 0x03, 0x0C, 0x0F, 0x30, 0x33, 0x3C, 0x3F,
                                    0xC0, 0xC3, 0xCC, 0xCF, 0xF0, 0xF3, 0xFC, 0xFF };

   BYTE i;

   while (*bText && l)                                // Ende durch 0-Byte im String oder surch erreichen der Länge l
   {
      if (*bText == '\r' )
      {
         OLED_Set_Position( 0, mbLineNo+2 );
         bText++;
         continue;
      }

      l--;                                            // nur druckbare Zeichen zählen - Steuerzeichen ausblenden

      if ( mbColNo > OLED_WIDTH-10 )                  // Passt das Zeichen noch auf die Zeile?
      {
         OLED_Set_Position( 0, mbLineNo+2 );          // nein dann gehe auf die nächste Zeile
      }

      BYTE bFontIndex = *bText;

      if (bFontIndex<='z')
         bFontIndex -= 32;                            // Zeichen -32 da Zeichensatz erst mit Space = 32 beginnt
      else
      {
         switch(bFontIndex)                           // hier Sonderbehandlung der nicht ASCII-Zeichen
         {
         case '°': bFontIndex=91; break;
         case 'Ä': bFontIndex=93; break;
         case 'Ö': bFontIndex=94; break;
         case 'Ü': bFontIndex=95; break;
         case 'ä': bFontIndex=96; break;
         case 'ö': bFontIndex=97; break;
         case 'ü': bFontIndex=98; break;
         case 'ß': bFontIndex=99; break;
         default:  bFontIndex='?'-32; break;          // hier ein ? ausgeben
         }
      }

      // Gebe low-Nibble aus
      for (i=0; i<5; i++)
      {
         BYTE bOut = bDoubleNibble[ FontLookup[bFontIndex][i] & 0x0F ];
         OLED_WR_Data( bOut );
         OLED_WR_Data( bOut );
      }

      OLED_Set_Position( mbColNo, mbLineNo+1 );       // gehe in die nächste Zeile auf gleiche Spaltenposition
      // Gebe high-Nibble aus
      for (i=0; i<5; i++)
      {
         BYTE bOut = bDoubleNibble[ FontLookup[bFontIndex][i] >> 4 ];
         OLED_WR_Data( bOut );
         OLED_WR_Data( bOut );
      }

      mbColNo +=12;
      if ( mbColNo >= OLED_WIDTH )
      {                                               // "Zeilen-Vorschhub" durchführen da mbColNo am Zeilen-Ende angekommen ist
         OLED_Set_Position( 0, mbLineNo+1 );          // dann gehe auf die nächste Zeile
      }
      else
      {
         OLED_WR_Data( 0 );                           // einen leer-Raum zwischen den Zeichen lassen (untere Zeile)
         OLED_WR_Data( 0 );
         OLED_Set_Position( mbColNo-2, mbLineNo-1 );  // gehe auf obere Zeile zurück
         OLED_WR_Data( 0 );                           // einen leer-Raum zwischen den Zeichen lassen (obere Zeile)
         OLED_WR_Data( 0 );
         mbColNo +=2;
      }
      bText++;
   }

   if (bFill)
   {
      l *= 12;                                        // Jedes Zeichen hat 6 Spalten => wird verdoppelt (12) => Umrechnung von Zeichen auf Spalten
      i  = l;
      while (l)
      {
         OLED_WR_Data( 0 );                           // Rest der oberen Zeile mit leerzeichen füllen
         l--;
      }
      OLED_Set_Position( mbColNo, mbLineNo-1 );
      l = i;
      while (l)
      {
         OLED_WR_Data( 0 );                           // Rest der unteren Zeile mit leerzeichen füllen
         l--;
      }
      OLED_Set_Position( mbColNo+i, mbLineNo+1 );
   }
}

//------------------------------------------------------------------------------------------------------------------------
//    OLED_Write:       Gibt einen Text ab der aktuellen Display-Position aus. Text wird entweder durch
//                      ein '0'-Zeichen (C-String-Ende) oder Länge l begrenzt
//------------------------------------------------------------------------------------------------------------------------
void OLED_Write(BYTE __data20 *a, int l, BYTE bFill)
{
   BYTE i;

   while (*a && l)                                    // Ende durch 0-Byte im String oder surch erreichen der Länge l
   {
      if (*a == '\r' )
      {
         OLED_Set_Position( 0, mbLineNo+1 );
         a++;
         continue;
      }

      l--;                                            // nur druckbare Zeichen zählen - Steuerzeichen ausblenden

      if ( mbColNo > OLED_WIDTH-5 )                   // Passt das Zeichen noch auf die Zeile?
      {
         OLED_Set_Position( 0, mbLineNo+1 );          // nein dann gehe auf die nächste Zeile
      }

      BYTE bFontIndex = *a;

      if (bFontIndex<='z')
         bFontIndex -= 32;                            // Zeichen -32 da Zeichensatz erst mit Space = 32 beginnt
      else
      {
         switch(bFontIndex)                           // hier Sonderbehandlung der nicht ASCII-Zeichen
         {
         case '°': bFontIndex=91; break;
         case 'Ä': bFontIndex=93; break;
         case 'Ö': bFontIndex=94; break;
         case 'Ü': bFontIndex=95; break;
         case 'ä': bFontIndex=96; break;
         case 'ö': bFontIndex=97; break;
         case 'ü': bFontIndex=98; break;
         case 'ß': bFontIndex=99; break;
         default:  bFontIndex='?'-32; break;          // hier ein ? ausgeben
         }
      }

      for (i=0; i<5; i++)
         OLED_WR_Data( FontLookup[bFontIndex][i] );

      mbColNo +=6;
      if ( mbColNo >= OLED_WIDTH )
      {                                               // "Zeilen-Vorschhub" durchführen da mbColNo am Zeilen-Ende angekommen ist
         OLED_Set_Position( 0, mbLineNo+1 );          // nein dann gehe auf die nächste Zeile
      }
      else
         OLED_WR_Data( 0 );                           // einen leer-Raum zwischen den Zeichen lassen
      a++;
   }

   if (bFill)
   {
      l *= 6;                                         // Jedes Zeichen hat 6 Spalten => Umrechnung von Zeichen auf Spalten
      while (l)
      {
         OLED_WR_Data( 0 );                           // Rest der Zeile mit leerzeichen füllen
         l--;
      }
   }
}

//------------------------------------------------------------------------------------------------------------------------
//    OLED_printf:      Formatiert einen Text laut dem fmt-String und gibt diesen ab der aktuellen Display-Position aus.
//------------------------------------------------------------------------------------------------------------------------
void OLED_printf (const char *fmt, ...)
{
   va_list valist;
   char buf[OLED_PRINTF_BUF_SIZE];

   va_start (valist, fmt);
   int len = vsnprintf (buf, sizeof (buf), fmt, valist);
   va_end (valist);
   OLED_Write ((BYTE __data20*) buf, len, false);
}

//------------------------------------------------------------------------------------------------------------------------
//    OLED_printf:      Formatiert einen Text laut dem fmt-String und gibt diesen ab der aktuellen Display-Position aus.
//------------------------------------------------------------------------------------------------------------------------
void OLED_print_line (uint8_t bLine, const char *fmt, ...)
{
   va_list valist;
   char buf[OLED_PRINTF_BUF_SIZE];

   va_start (valist, fmt);
   int len = vsnprintf (buf, sizeof (buf), fmt, valist);
   va_end (valist);

   OLED_Set_Position( 0, bLine );
   OLED_Write ((BYTE __data20*) buf, 21, true);
}

//------------------------------------------------------------------------------------------------------------------------
//    OLED_WriteStatusLine:      Formatiert einen Text laut dem fmt-String und gibt diesen ab der aktuellen Display-Position aus.
//------------------------------------------------------------------------------------------------------------------------
void OLED_WriteStatusLine (const char *fmt, ...)
{
   va_list valist;
   char buf[21];

   va_start (valist, fmt);
   int len = vsnprintf (buf, sizeof (buf), fmt, valist);
   va_end (valist);

   OLED_Set_Position( 0, OLED_STATUS_LINE );
   OLED_Write ((BYTE __data20*) buf, 21, true);
}

//------------------------------------------------------------------------------------------------------------------------
//    OLED_DrawTank:    Zeichent einen Akku der entsprechend gefüllt ist. bRelPower: 0..100%
//------------------------------------------------------------------------------------------------------------------------
void OLED_DrawTank(BYTE bRelPower, BYTE x, BYTE y)
{

   BYTE bMask;

   bRelPower >>= 2;                                   // 0..100% in 0..25 Pixel umwandeln

   OLED_Set_Position( x, y );

   OLED_WR_Data( 0xFE );
   OLED_WR_Data( 0x82 );                              // Tank Anfang zeichnen

   for (BYTE bPos=0; bPos<25; bPos++)
   {
      bMask = (bPos<bRelPower) ? 0xBA : 0x82;         // Tank und Schrift xxx% mit exor verknüpfen -> quasie inverse Schrift bei gefülltem Tank
      OLED_WR_Data (bMask);
   }

   OLED_WR_Data( 0x82 );                              // Tank Ende zeichnen
   OLED_WR_Data( 0xC6 );
   OLED_WR_Data( 0x44 );
   OLED_WR_Data( 0x44 );
   OLED_WR_Data( 0x7C );
}


void OLED_UpdateStatusTextNo(BYTE bTextNumber)
{
   static BYTE bActualTextNumber = 255;

   if ( bActualTextNumber != bTextNumber )
   {
      bActualTextNumber = bTextNumber;                // Wenn sich die Text-Nummer geändert hat

      OLED_Set_Position( 0, OLED_STATUS_LINE );
      OLED_Write ( (BYTE __data20 *) cpbTextArray[bTextNumber], 21, true);    // genau 21 Zeichen darstellen
   }
}

//------------------------------------------------------------------------------------------------------------------------
//    OLED_Show_Si1132_Data:    Zeigt die EE893 Daten im OLED an
//------------------------------------------------------------------------------------------------------------------------
void OLED_Show_EE893_Data( ST_EE893_MESS_DATA *pstM )
{
}

//------------------------------------------------------------------------------------------------------------------------
//    OLED_Show_Si1132_Data:    Zeigt die Flow Daten im OLED an
//------------------------------------------------------------------------------------------------------------------------
void OLED_Show_Flow_Data( ST_FLOW_MESS_DATA   *pstM )
{
   if (isOLED_POWER_ON)
   {
      int16_t iFlowH =  pstM->iFlowX_avg/100;
      int16_t iFlowL = (pstM->iFlowX_avg/10 )%10;
      if (iFlowL<0) iFlowL = -iFlowL;

      OLED_print_line(1, "Flow X:  %3i,%0i m/s", iFlowH, iFlowL);

      iFlowH =  pstM->iFlowY_avg/100;
      iFlowL = (pstM->iFlowY_avg/10 )%10;
      if (iFlowL<0) iFlowL = -iFlowL;
      OLED_print_line(2, "Flow Y:  %3i,%0i m/s", iFlowH, iFlowL);
   }
}

//------------------------------------------------------------------------------------------------------------------------
//    OLED_Show_Si1132_Data:    Zeigt die Si1132 Daten im OLED an
//------------------------------------------------------------------------------------------------------------------------
void OLED_Show_Si1132_Data(ST_Si1132_MessData *pstM )
{
   if (isOLED_POWER_ON)
   {
      OLED_print_line( 3, "Licht: %5u   lux", pstM->wAvgVIS);
   }
}

//------------------------------------------------------------------------------------------------------------------------
//    OLED_Show_BME280_PTH:    Zeigt die BME280 Daten  im OLED an
//------------------------------------------------------------------------------------------------------------------------
void OLED_Show_BME280_PTH(ST_BME280_MESS_DATA *pstM )
{
   if (isOLED_POWER_ON)
   {
      int16_t iH = pstM->u32Pressure / 100;
      int16_t iL = (pstM->u32Pressure / 10)%10;
      OLED_print_line(4, "Druck:  %4u,%0u mBar" , iH, iL);

      iH = pstM->i32Temp / 100;
      iL = (pstM->i32Temp / 10) % 10;
      if (iL<0) iL = -iL;                             // Bei der Nachkommestelle kein neg. Vorzeichen zulassen
      OLED_print_line(5, "Temp:    %+3i,%0i °C" , iH, iL);

      iH = pstM->u32Humity>>10;
      iL = (pstM->u32Humity/102) % 10;
      OLED_print_line(6, "Feuchte: %3u,%0u %%" , iH, iL);
   }
}

//------------------------------------------------------------------------------------------------------------------------
//    OLED_Show_Akku_Data:    Schreibt die Akku-Status-Zeile mit Tank und ob laed/ok/leer/voll
//------------------------------------------------------------------------------------------------------------------------
void OLED_Show_Akku_Data( uint8_t bRelPower )         // Akku-Zustand im OLED anzeigen
{
   static BYTE bLastRelPower = 255;

   if ( bRelPower<bLastRelPower -4 || bRelPower>bLastRelPower +4 || (bRelPower!=bLastRelPower && bRelPower>96))
   {
      // Wenn sich RelPower um 4% unterscheidet oder über 96% beträgt

      AKKU_STATUS_ENABLE;                             // Hier die Pull-Ups für die Charger-Statusleitungen einschalten

      bLastRelPower = bRelPower;

      OLED_Set_Position( 0, OLED_AKKU_LINE );
      OLED_Write( (BYTE*) cpbText_Akku, 6, true );    // Immer 6 Zeichen ausgeben

      OLED_DrawTank(bRelPower, 29, OLED_AKKU_LINE);

      const BYTE *bStatusText = cpbText_AkkuStatus_unknown;

      if (is_USB_PG)
         if (AKKU_get_STATUS == AKKU_STATUS_CHARGE_DONE)
            bStatusText = cpbText_AkkuStatus_voll;
         else if(AKKU_get_STATUS == AKKU_STATUS_CHARGE)
            bStatusText = cpbText_AkkuStatus_laden;
      else
         if (bRelPower<25)
             bStatusText = cpbText_AkkuStatus_leer;
         else
             bStatusText = cpbText_AkkuStatus_ok;

      AKKU_STATUS_DISABLE;                            // Hier die Pull-Ups für die Charger-Statusleitungen ausschalten
      OLED_Set_Position( 64, OLED_AKKU_LINE );
      OLED_Write( (BYTE __data20 *) bStatusText, 10, true );

      OLED_Set_Position( 104, OLED_AKKU_LINE );
      char bBuffer[5];
      snprintf (bBuffer, sizeof (bBuffer), "%3hhi%%" , bRelPower);
      OLED_Write( (BYTE __data20 *) bBuffer, 4, true );
   }
}

EN_TASK_STATUS OLED_Task(void)
{
   if (muiTimeOutSek)                                 // prüfe ob Timeout-Überwachung aktiv
   {
      muiTimeOutSek--;                                // Anzahl der verbleibenden Sekunden erniedrigen
      if (muiTimeOutSek == 0)
         OLED_Off();                                  // Zeit abgelaufen -> OLED-ausschalten
   }
   return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP0);
}


//==========================================================================================================================
//    Hier die globale Task-Tabelle für den Task-Manager
//==========================================================================================================================
ST_FUNCTION_TABLE gstOLED_Func_Table[] =
{
   { OLED_Task                   , 1024l  }           // Task alle Sekunde aufrufen um zu prüfen ob OLED ausgeschaltet werden soll
};
