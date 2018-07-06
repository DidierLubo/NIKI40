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
#include <stdint.h>

#include "Hardware.h"
#include "UART_USCI.h"

#ifdef UCA0_USED
   extern volatile ST_UART_t* UCA0_Init(long Baudrate);
#endif

#ifdef UCA1_USED
   extern volatile ST_UART_t* UCA1_Init(long Baudrate);
#endif

#ifdef UCA2_USED
   extern volatile ST_UART_t* UCA2_Init(long Baudrate);
#endif

#ifdef UCA3_USED
   extern volatile ST_UART_t* UCA3_Init(long Baudrate);
#endif

ST_UART_t* stSTDIO = 0;

//------------------------------------------------------------------------------------------------------------------------
//  Hier wird erstmal die Hardware initalisiert
//------------------------------------------------------------------------------------------------------------------------
//  bIndex:           0..3  für UCA0..UCA3
//  Baudrate:         Eine Beliebige Baurate - Standards sind mit BRxxx definiert z. B. BR9600
//  bSTD_IO:          true / false - Soll die Schnittstelle für STD-IO-Befehle printf / scanf etc genutzt werden = true
//------------------------------------------------------------------------------------------------------------------------

volatile ST_UART_t* UART_Init(BYTE bIndex, long Baudrate, BYTE bSTD_IO)
{
   volatile ST_UART_t* st = 0;

   switch(bIndex)
   {
#ifdef UCA0_USED
   case 0: st = UCA0_Init(Baudrate); break;
#endif
#ifdef UCA1_USED
   case 1: st = UCA1_Init(Baudrate); break;
#endif
#ifdef UCA2_USED
   case 2: st = UCA2_Init(Baudrate); break;
#endif
#ifdef UCA3_USED
   case 3: st = UCA3_Init(Baudrate); break;
#endif
   }
   if (bSTD_IO)
      stSTDIO = st;
   return st;
}

//------------------------------------------------------------------------------------------------------------------------
//  Damit wird ein Zeichen in den Ringbuffer übertragen. Ist der Buffer bereits voll wird gewartet, bis das zu sendende
//  Zeichen wieder Platz hat. Wenn keine Übertragung aktiv ist, wird diese damit eingeleitet
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:    Der bei Init zurückgegebene Pointer
//------------------------------------------------------------------------------------------------------------------------

void UART_Put_Byte(ST_UART_t* st, BYTE Value)
{
   if (st)
      st->Put_Byte( Value );
}

//------------------------------------------------------------------------------------------------------------------------
//  Damit wird das Senden eines Strings eingeleitet. Ist der String größer als der verbleibende Teil des Ringbvuffers wird
//  gewartet, bis dieser wieder frei ist. Ansonsten wird nach der Übtragung aller Zeichen in den Ringbuffer die Kontrolle
//  sofort an das Hauptprogramm zurückgegeben.
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:    Der bei Init zurückgegebene Pointer
//  pString:          Zeiger auf den auszugebenden String
//------------------------------------------------------------------------------------------------------------------------

void UART_Put_String(ST_UART_t* st, BYTE *pString)
{
   if (st)
      while (*pString) st->Put_Byte( *pString++ );
}

//------------------------------------------------------------------------------------------------------------------------
//  Damit wird das Senden eines Records eingeleitet. Ist die Anzahl der Datenbytesg größer als der verbleibende Teil
//  des Ringbvuffers wird gewartet, bis dieser wieder frei ist. Ansonsten wird nach der Übtragung aller Zeichen in
//  den Ringbuffer die Kontrolle sofort an das Hauptprogramm zurückgegeben.
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:    Der bei Init zurückgegebene Pointer
//  pvData:           Zeiger auf die auszugebenden Daten
//  uiLen:            Länge der Daten in Bytes
//------------------------------------------------------------------------------------------------------------------------
void UART_Put_Record(ST_UART_t* st, void *pvData, uint16_t uiLen)
{
   if (st)
   {
      uint8_t *pbBuf = (uint8_t*) pvData;
      while (uiLen)
      {
         st->Put_Byte( *pbBuf++ );
         uiLen--;
      }
   }
}

//------------------------------------------------------------------------------------------------------------------------
//  Gibt Anzahl der noch zu senden Zeichen im Ring-Buffer zurück
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:    Der bei Init zurückgegebene Pointer
//------------------------------------------------------------------------------------------------------------------------
BYTE UART_Get_TXD_Anzahl(ST_UART_t* st)
{
   if (st)
      return (st->bS_Char_Counter);
   else
      return 0;
}

//------------------------------------------------------------------------------------------------------------------------
//  Gibt Anzahl der Zeichen im Empfangsbuffer zurück
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:    Der bei Init zurückgegebene Pointer
//------------------------------------------------------------------------------------------------------------------------

BYTE UART_Get_RXD_Anzahl(ST_UART_t* st)
{
   if (st)
      return (st->bE_Char_Counter);
   else
      return 0;
}

//------------------------------------------------------------------------------------------------------------------------
//  Ringbuffer leeren. (Alle Zeichen als gelesen betrachten)
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:    Der bei Init zurückgegebene Pointer
//------------------------------------------------------------------------------------------------------------------------

__monitor void UART_Clear_E_Buffer(ST_UART_t* st)
{
   if (st)
   {
      st->pbE_Buffer_RD    = st->pbE_Buffer_WR;
      st->bE_Char_Counter  = 0;
      st->bE_Z_Status     &= Z_Status_ECHO;
   }
}

//------------------------------------------------------------------------------------------------------------------------
//  Gibt an ob z. B. CR oder LF empfangen wurde
//  Mask  gibt an welche Zeichen relevant sind (UND-Maske)
//  Value gibt an welcher Wert nach der Und-Verknüpfung mit der Maske vorkommen muss
//  z. B. gibt UART_Get_Zeichen_Status(Z_Status_CR | Z_Status_LF, Z_Status_CR)
//             nur dann 1 zurück wenn CR nicht aber LF empfangen wurde
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:    Der bei Init zurückgegebene Pointer
//------------------------------------------------------------------------------------------------------------------------

BYTE UART_Get_Zeichen_Status(ST_UART_t* st, BYTE bMask, BYTE bValue)
{
   register BYTE bStatus = 0;
   if (st)
   {
      bStatus = st->bE_Z_Status & bMask;
      bStatus = (bStatus == bValue);
      if (bStatus) st->bE_Z_Status &= ~bMask;         // Status nach Meldung zurücksetzen
   }
   return bStatus;
}

//------------------------------------------------------------------------------------------------------------------------
//  UCA0_Set_Echo:    Steuert die Echo-Ausgabe
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:    Der bei Init zurückgegebene Pointer
//------------------------------------------------------------------------------------------------------------------------
void UART_Set_Echo(ST_UART_t* st, BYTE bFlg)
{
   if (st)
   {
      switch (bFlg)
      {
      case 0: st->bE_Z_Status &= ~Z_Status_ECHO; break;           // Echo aus
      case 1: st->bE_Z_Status |=  Z_Status_ECHO; break;           // Echo an
      case 2: st->bE_Z_Status ^=  Z_Status_ECHO; break;           // Echo toggle
      }
   }
}

//------------------------------------------------------------------------------------------------------------------------
//  Übergibt ein Zeichen aus dem Empfangsbuffer - Falls keines da wird auf ein Zeichen gewartet
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:    Der bei Init zurückgegebene Pointer
//------------------------------------------------------------------------------------------------------------------------

BYTE UART_Get_Byte(ST_UART_t* st)
{
   return st->Get_Byte();
}

//------------------------------------------------------------------------------------------------------------------------
//  Übergibt ein Zeichen aus dem Empfangsbuffer - Falls keines da wird nicht gewartet
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:    Der bei Init zurückgegebene Pointer
//------------------------------------------------------------------------------------------------------------------------

BYTE UART_In_Byte(ST_UART_t* st)
{ BYTE bZeichen;

  if (st->bE_Char_Counter > 0)
     bZeichen = st->Get_Byte();
  else
     bZeichen = 0xFF;
  return bZeichen;
}

//------------------------------------------------------------------------------------------------------------------------
//  Überträgt einen Kompletten Record aus dem Ringbuffer. Falls dort zuwenig Daten sind wird gewartet bis die
//  gewünschte Anzahl Bytes empfangen wurden.
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:    Der bei Init zurückgegebene Pointer
//  pvData:           Zeiger auf die zu speichernden Empfangsdaten
//  uiLen:            Länge der Daten in Bytes
//------------------------------------------------------------------------------------------------------------------------
void UART_Get_Record(ST_UART_t* st, void *pvData, uint16_t uiLen)
{
   if (st)
   {
      uint8_t *pbBuf = (uint8_t*) pvData;
      while (uiLen)
      {
         *pbBuf++ = st->Get_Byte();
         uiLen--;
      }
   }
}

//------------------------------------------------------------------------------------------------------------------------
//  Überträgt eine komplette Zeile aus dem V24-Empfangbuffer in den Zielbuffer
//  bMaxBytes  bestimmt die Buffergröße des Zeilspeichers (Achtung Null-Byte zählt mit)
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:   Der bei Init zurückgegebene Pointer
//  bZielBuffer:     Pointer wo die Zeile hin soll
//  bMaxBytes:       Gibt an wieviele die Zeile max. enthalten darf. Werden diese errecht so wird der Teil-String
//                   zurückgebeben.
//  bCR_LF_Flg =1 entspricht LF ist Zeilenende
//  bCR_LF_Flg =0 entspricht CR ist Zeilenende
//------------------------------------------------------------------------------------------------------------------------

void UART_Get_Line(ST_UART_t* st, register BYTE *bZielBuffer, register BYTE bMaxBytes, BYTE bCR_LF_Flg)
{
   register BYTE bUCA0_Char;

   bMaxBytes--;                                                // Platz für Null-Byte schon mal abziehen

   while (bMaxBytes)
   {
      bUCA0_Char    = st->Get_Byte();
      switch (bUCA0_Char)
      {
      case '\r':
         if (!bCR_LF_Flg) bMaxBytes = 0;                       // Schon bei CR Ende Melden
         break;

      case '\n':
         if (bCR_LF_Flg) bMaxBytes = 0;                        // Erst nach CR LF bzw. LF Ende der Zeile melden
         break;

      default:
         *bZielBuffer++ = bUCA0_Char;
         bMaxBytes--;
      }
  }
  *bZielBuffer = 0;
}

//------------------------------------------------------------------------------------------------------------------------
//  Wandelt die Zeichen im V24-Buffer in eine vorzeichenlose Intergerzahl (Dezimalsystem)
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:    Der bei Init zurückgegebene Pointer
//------------------------------------------------------------------------------------------------------------------------

WORD UART_Get_Word_Field_Dez(ST_UART_t* st)
{
   register BYTE bUCA0_Char;
   register WORD wRetvalue = 0;

   do
   { bUCA0_Char    = st->Get_Byte();
      if (bUCA0_Char>='0' && bUCA0_Char<='9')
      {
         wRetvalue = (wRetvalue * 10) + (bUCA0_Char - '0');
      }
      else if (bUCA0_Char!=' ')
      { // Ende der Auswertung wenn nicht Numerisch
         bUCA0_Char = 0;
      }
   } while (bUCA0_Char);
   return (wRetvalue);
}

//------------------------------------------------------------------------------------------------------------------------
//  Wandelt die Zeichen im V24-Buffer in eine Intergerzahl (Dezimalsystem)
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:    Der bei Init zurückgegebene Pointer
//------------------------------------------------------------------------------------------------------------------------

int UART_Get_Integer_Field_Dez(ST_UART_t* st)
{
   register BYTE bSgnFlg = 0;
   register BYTE bUCA0_Char;
   register int iRetvalue = 0;

   do
   { bUCA0_Char    = st->Get_Byte();
      if (bUCA0_Char>='0' && bUCA0_Char<='9')
      {
         iRetvalue = (iRetvalue * 10) + (bUCA0_Char - '0');
      }
      else if (bUCA0_Char == '-')
      { // Vorzeichen nur vor der Zahl zulässig
         if (iRetvalue == 0)
            bSgnFlg = 1;
         else
            bUCA0_Char = 0;
      }
      else if (bUCA0_Char == '+')
      { // Vorzeichen nur vor der Zahl zulässig
         if (iRetvalue != 0)
            bUCA0_Char = 0;
      }
      else if (bUCA0_Char!=' ')
      {
         // Ende der Auswertung wenn nicht Numerisch
         bUCA0_Char = 0;
      }
   } while (bUCA0_Char);
   return bSgnFlg ? (- iRetvalue) : (iRetvalue);
}

//------------------------------------------------------------------------------------------------------------------------
//  Wandelt die Zeichen im V24-Buffer in eine Long-Intergerzahl (Dezimalsystem)
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:    Der bei Init zurückgegebene Pointer
//------------------------------------------------------------------------------------------------------------------------

long UART_Get_Long_Field_Dez(ST_UART_t* st)
{
   register BYTE bSgnFlg = 0;
   register BYTE bUCA0_Char;
   register long lRetvalue = 0;

   do
   {
      bUCA0_Char    = st->Get_Byte();
      if (bUCA0_Char>='0' && bUCA0_Char<='9')
      {
         lRetvalue = (lRetvalue * 10) + (bUCA0_Char - '0');
      }
      else if (bUCA0_Char == '-')
      { // Vorzeichen nur vor der Zahl zulässig
         if (lRetvalue == 0)
            bSgnFlg = 1;
         else
            bUCA0_Char = 0;
      }
      else if (bUCA0_Char == '+')
      {  // Vorzeichen nur vor der Zahl zulässig
         if (lRetvalue != 0) bUCA0_Char = 0;
      }
      else if (bUCA0_Char!=' ')
      { // Ende der Auswertung wenn nicht Numerisch
         bUCA0_Char = 0;
      }
   } while (bUCA0_Char);
   return bSgnFlg ? (- lRetvalue) : (lRetvalue);
}


//------------------------------------------------------------------------------------------------------------------------
//  Wandelt die Zeichen im V24-Buffer in eine Intergerzahl (Hexadezimalsystem)
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:    Der bei Init zurückgegebene Pointer
//------------------------------------------------------------------------------------------------------------------------

WORD UART_Get_Word_Field_Hex(ST_UART_t* st)
{
   register BYTE bUCA0_Char;
   register WORD wRetvalue = 0;

   do
   {
      bUCA0_Char    = st->Get_Byte();
      if (bUCA0_Char>='0' && bUCA0_Char<='9')
      {
         wRetvalue = (wRetvalue * 16) + (bUCA0_Char - '0');
      }
      else if (bUCA0_Char>='A' && bUCA0_Char<='F')
      {
         wRetvalue = (wRetvalue * 16) + (bUCA0_Char - 'A' + 10);
      }
      else if (bUCA0_Char>='a' && bUCA0_Char<='f')
      {
         wRetvalue = (wRetvalue * 16) + (bUCA0_Char - 'a' + 10);
      }
      else if (bUCA0_Char!=' ')
      { // Ende der Auswertung wenn nicht im Hex-Bereich (0-9 A-F a-f)
         bUCA0_Char = 0;
      }
   } while (bUCA0_Char);
   return wRetvalue;
}

//------------------------------------------------------------------------------------------------------------------------
//  Wandelt die Zeichen im V24-Buffer in eine Intergerzahl (Hexadezimalsystem)
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:    Der bei Init zurückgegebene Pointer
//------------------------------------------------------------------------------------------------------------------------

long UART_Get_Long_Field_Hex(ST_UART_t* st)
{
  register BYTE bUCA0_Char;
  register long lRetvalue = 0;

  do
  { bUCA0_Char    =st->Get_Byte();
    if (bUCA0_Char>='0' && bUCA0_Char<='9')
    { lRetvalue = (lRetvalue * 16) + (bUCA0_Char - '0');
    }
    else if (bUCA0_Char>='A' && bUCA0_Char<='F')
    { lRetvalue = (lRetvalue * 16) + (bUCA0_Char - 'A' + 10);
    }
    else if (bUCA0_Char>='a' && bUCA0_Char<='f')
    { lRetvalue = (lRetvalue * 16) + (bUCA0_Char - 'a' + 10);
    }
    else if (bUCA0_Char!=' ')
    { // Ende der Auswertung wenn nicht im Hex-Bereich (0-9 A-F a-f)
      bUCA0_Char = 0;
    }
  } while (bUCA0_Char);
  return (lRetvalue);
}

//------------------------------------------------------------------------------------------------------------------------
//  Empfängt einen Datensatz der wie folgt aufgebaut ist:
//  DLE STX ... Daten ... DLE ETX LRC
//  Kommt innerhalb der Daten ein DLE Zeichen vor, so wird dies wiederholt - aber nur einmal im LRC berücksichtigt
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:   Der bei Init zurückgegebene Pointer
//  *vZielBuffer     Pointer auf Zielspeicher. Dieser muss einen kompletten Datensatz aufnehmen können!!
//  bMaxBytes        Anzahl der maximal zu empfangenden Zeichen = Buffer-Länge
//------------------------------------------------------------------------------------------------------------------------
//  Liefert true wenn LRC ok und damit Datensatz vollständig empfangen wurde
//  sonst false
//------------------------------------------------------------------------------------------------------------------------

BYTE UART_GetTransparentRecord(ST_UART_t* st, void *vZielBuffer, BYTE bMaxBytes)
{
   BYTE  bCheckSum    = STX;
   BYTE *bZiel        = (BYTE*) vZielBuffer;
   BYTE  bByteCounter = bMaxBytes;
   BYTE  bV24;

   do
   {
      while (st->Get_Byte() != DLE) _NOP();           // Solange kein DLE STX empfangen wird - warten
   }  while (st->Get_Byte() != STX);

   while (bByteCounter)
   {
      bV24       = st->Get_Byte();                    // Ein Zeichen empfangen
      bCheckSum += bV24;                              // zur Checksumme dazu addieren

      if (bV24 == DLE)                                // War es ein DLE-Zeichen?
      {                                               // hier darf ein DLE oder ETX Zeichen kommen
         bV24 = st->Get_Byte();                       // Ein Zeichen empfangen
         if (bV24 == DLE)
         {
            *bZiel++ = bV24;                          // DLE-Zeichen im Datenpacket abspeichern
            bByteCounter --;
         } else if (bV24 == ETX)
         {                                            // Datensatzende über DLE ETX - Sequenz erkannt
            bCheckSum += ETX - DLE;                   // Das DLE wieder aus Checksumme entfernen und ETX addieren
            bV24 = st->Get_Byte();                    // Das ist die Checksumme
            if (bV24 == bCheckSum)
               st->Put_Byte(ACK);                     // ACK-melden bei erfolgreichem Empfang
            else
               st->Put_Byte(NAK);
            return bV24 == bCheckSum;
//------------------------------------------------------------------------------------------------------------------------
         } else if (bV24 == STX)
         {                                            // DLE STX empfangen also alles zurück auf Anfang
            bCheckSum    = STX;
            bZiel        = (BYTE*) vZielBuffer;
            bByteCounter = bMaxBytes;
         } else return false;                         // Sequenzfehler!! kein DLE ETX oder DLE DLE und kein DLE STX!!
//------------------------------------------------------------------------------------------------------------------------
      } else
      {
         *bZiel++ = bV24;                             // Zeichen abspeichern
         bByteCounter --;
      }
   }

   // Anzahl der gewünschten Datenbytes wurde empfangen -> nun muss DLE ETX kommen!!
   bV24 = st->Get_Byte();                             // Sollte das DLE Zeichen sein
   if (bV24 != DLE) return false;
//------------------------------------------------------------------------------------------------------------------------
   bV24 = st->Get_Byte();                             // Sollte das ETX Zeichen sein
   if (bV24 != ETX) return false;
//------------------------------------------------------------------------------------------------------------------------
   bCheckSum += ETX;                                  // ETX addieren
   bV24 = st->Get_Byte();                             // Hier wird die Checksumme erwartet
   if (bV24 == bCheckSum)
      st->Put_Byte(ACK);
   else
      st->Put_Byte(NAK);
   return  bV24 == bCheckSum;
}

//------------------------------------------------------------------------------------------------------------------------
//  Ermöglicht die Verwendung von printf
//------------------------------------------------------------------------------------------------------------------------

int putchar(int ch)
{
   if (stSTDIO)
      stSTDIO->Put_Byte((BYTE) ch);
   return ch;
}

//------------------------------------------------------------------------------------------------------------------------
//  Ermöglicht die Verwendung von scanf
//------------------------------------------------------------------------------------------------------------------------

int getchar(void)
{
   if (stSTDIO)
      return stSTDIO->Get_Byte();
   else
      return 0;
}

//------------------------------------------------------------------------------------------------------------------------
//  Macht den übergeben UART-Port zur Standard Ein/Ausgabe für printf und scanf
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:    Der bei Init zurückgegebene Pointer
//------------------------------------------------------------------------------------------------------------------------

void URAT_SetStandardOut(ST_UART_t* st)
{
   stSTDIO = st;
}
