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
#ifndef  _UART__USCI
#define  _UART__USCI

#include "Hardware.h"
//------------------------------------------------------------------------------------------------------------------------
#ifndef BYTE
   #define BYTE unsigned char
   #define WORD unsigned int
#endif
//------------------------------------------------------------------------------------------------------------------------

// Standard Steuerzeichen
#define SOH 0x01
#define STX 0x02
#define ETX 0x03
#define EOT 0x04
#define DLE 0x10
#define DC3 0x13
#define ACK 0x06
#define NAK 0x15

#define Z_Status_CR   0x01
#define Z_Status_LF   0x02
#define Z_Status_ECHO 0x80

#define BR1200         1200
#define BR2400         2400
#define BR4800         4800
#define BR9600         9600
#define BR19200       19200
#define BR38400       38400
#define BR57600       57600
#define BR76800       76800
#define BR115200     115200
#define BR460800     460800

typedef volatile struct ST_UART_T
{
   BYTE  *pbE_Buffer_WR;                              // Für Empfang des Zeichens (IRQ) und übertragen in den E-Buffer
   BYTE  *pbE_Buffer_RD;                              // Wird beim Auslesen des E-Buffers verwendet
   BYTE    bE_Char_Counter;                           // Hier wird die Anzahl der empfangenen Zeichen angegeben
   BYTE    bE_Z_Status;                               // Hier werden gewisse Zeichen signalisiert

   BYTE  *pbS_Buffer_WR;                              // Beim übertragen des Zeichens in den S-Buffer verwendet
   BYTE  *pbS_Buffer_RD;                              // Wird beim Auslesen des S-Buffers im IRQ verwendet
   BYTE    bS_Char_Counter;                           // Hier wird die Anzahl der noch zu sendenden Zeichen angegeben

   void (*Put_Byte)(register BYTE bZeichen);
   BYTE (*Get_Byte)(void);

} ST_UART_t;

//------------------------------------------------------------------------------------------------------------------------
//  Hier wird erstmal die Hardware initalisiert
//------------------------------------------------------------------------------------------------------------------------
//  bIndex:          0..3  für UCA0..UCA3
//  Baudrate:        Eine Beliebige Baurate - Standards sind mit BRxxx definiert z. B. BR9600
//  bSTD_IO:         true / false - Soll die Schnittstelle für STD-IO-Befehle printf / scanf etc genutzt werden = true
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t*       gibt einen Zeiger auf den UART-Buffer zurück
//------------------------------------------------------------------------------------------------------------------------
extern volatile ST_UART_t* UART_Init(BYTE bIndex, long Baudrate, BYTE bSTD_IO);

//------------------------------------------------------------------------------------------------------------------------
//  Damit wird ein Zeichen in den Ringbuffer übertragen. Ist der Buffer bereits voll wird gewartet, bis das zu sendende
//  Zeichen wieder Platz hat. Wenn keine Übertragung aktiv ist, wird diese damit eingeleitet
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:    Der bei Init zurückgegebene Pointer
//------------------------------------------------------------------------------------------------------------------------
extern void UART_Put_Byte(ST_UART_t* st, BYTE Value);

extern void UART_Put_ByteArray(ST_UART_t* st, BYTE len, BYTE* txData);

//------------------------------------------------------------------------------------------------------------------------
//  Damit wird das Senden eines Strings eingeleitet. Ist der String größer als der verbleibende Teil des Ringbvuffers wird
//  gewartet, bis dieser wieder frei ist. Ansonsten wird nach der Übtragung aller Zeichen in den Ringbuffer die Kontrolle
//  sofort an das Hauptprogramm zurückgegeben.
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:    Der bei Init zurückgegebene Pointer
//  pString:          Zeiger auf den auszugebenden String
//------------------------------------------------------------------------------------------------------------------------
extern void UART_Put_String(ST_UART_t* st, BYTE *pString); // Gibt einen String  auf V24 aus

//------------------------------------------------------------------------------------------------------------------------
//  Gibt Anzahl der noch zu senden Zeichen im Ring-Buffer zurück
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:    Der bei Init zurückgegebene Pointer
//------------------------------------------------------------------------------------------------------------------------
extern BYTE UART_Get_TXD_Anzahl(ST_UART_t* st);       // Gibt die Anzahl der noch zu sendenden Zeichen zurück

//------------------------------------------------------------------------------------------------------------------------
//  Gibt Anzahl der Zeichen im Empfangsbuffer zurück
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:    Der bei Init zurückgegebene Pointer
//------------------------------------------------------------------------------------------------------------------------
extern inline BYTE UART_Get_RXD_Anzahl(ST_UART_t* st);       // Gibt Anzahl der Zeichen im Empfangsbuffer zurück

//------------------------------------------------------------------------------------------------------------------------
//  Lerrt den Empfangsbuffer
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:    Der bei Init zurückgegebene Pointer
//------------------------------------------------------------------------------------------------------------------------
extern __monitor void UART_Clear_E_Buffer(ST_UART_t* st);   // Ringbuffer leeren. (Alle Zeichen als gelesen betrachten)

//------------------------------------------------------------------------------------------------------------------------
//  Gibt an ob z. B. CR oder LF empfangen wurde
//  Mask  gibt an welche Zeichen relevant sind (UND-Maske)
//  Value gibt an welcher Wert nach der Und-Verknüpfung mit der Maske vorkommen muss
//  z. B. gibt UART_Get_Zeichen_Status(Z_Status_CR | Z_Status_LF, Z_Status_CR)
//             nur dann 1 zurück wenn CR nicht aber LF empfangen wurde
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:   Der bei Init zurückgegebene Pointer
//------------------------------------------------------------------------------------------------------------------------
extern BYTE UART_Get_Zeichen_Status(ST_UART_t* st, BYTE bMask, BYTE Value);

//------------------------------------------------------------------------------------------------------------------------
//  UCA0_Set_Echo:   Steuert die Echo-Ausgabe
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:   Der bei Init zurückgegebene Pointer
//  bFlg:            eines der unten stehen #define-Werte
//------------------------------------------------------------------------------------------------------------------------
#define UART_Set_Echo_OFF    0
#define UART_Set_Echo_ON     1
#define UART_Set_Echo_TOGGLE 2
void UART_Set_Echo(ST_UART_t* st, BYTE bFlg);

//------------------------------------------------------------------------------------------------------------------------
//  Übergibt ein Zeichen aus dem Empfangsbuffer - Falls keines da wird auf ein Zeichen gewartet
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:   Der bei Init zurückgegebene Pointer
//------------------------------------------------------------------------------------------------------------------------
extern BYTE UART_Get_Byte(ST_UART_t* st);            // Liest ein Zeichen aus dem Ringbuffer bzw. wartet bis eines da

extern void UART_Get_ByteArray(ST_UART_t* st, register BYTE *bZielBuffer, register BYTE bMaxBytes);


//------------------------------------------------------------------------------------------------------------------------
//  Übergibt ein Zeichen aus dem Empfangsbuffer - Falls keines da wird nicht gewartet
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:   Der bei Init zurückgegebene Pointer
//------------------------------------------------------------------------------------------------------------------------
extern BYTE UART_In_Byte(ST_UART_t* st);

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
extern void UART_Get_Line(ST_UART_t* st, register BYTE *bZielBuffer, register BYTE bMaxBytes, BYTE bCR_LF_Flg);

//------------------------------------------------------------------------------------------------------------------------
//  Wandelt die Zeichen im V24-Buffer in eine vorzeichenlose Intergerzahl (Dezimalsystem)
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:    Der bei Init zurückgegebene Pointer
//------------------------------------------------------------------------------------------------------------------------
extern WORD UART_Get_Word_Field_Dez(ST_UART_t* st);

//------------------------------------------------------------------------------------------------------------------------
//  Wandelt die Zeichen im V24-Buffer in eine Intergerzahl (Dezimalsystem)
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:    Der bei Init zurückgegebene Pointer
//------------------------------------------------------------------------------------------------------------------------
extern int  UART_Get_Integer_Field_Dez(ST_UART_t* st);

//------------------------------------------------------------------------------------------------------------------------
//  Wandelt die Zeichen im V24-Buffer in eine Long-Intergerzahl (Dezimalsystem)
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:    Der bei Init zurückgegebene Pointer
//------------------------------------------------------------------------------------------------------------------------
extern long UART_Get_Long_Field_Dez(ST_UART_t* st);

//------------------------------------------------------------------------------------------------------------------------
//  Wandelt die Zeichen im V24-Buffer in eine Intergerzahl (Hexadezimalsystem)
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:    Der bei Init zurückgegebene Pointer
//------------------------------------------------------------------------------------------------------------------------
extern WORD UART_Get_Word_Field_Hex(ST_UART_t* st);
//------------------------------------------------------------------------------------------------------------------------
//  Wandelt die Zeichen im V24-Buffer in eine Intergerzahl (Hexadezimalsystem)
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:    Der bei Init zurückgegebene Pointer
//------------------------------------------------------------------------------------------------------------------------
extern long UART_Get_Long_Field_Hex(ST_UART_t* st);

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
extern BYTE UART_GetTransparentRecord(ST_UART_t* st, void *vZielBuffer, BYTE bMaxBytes);

//------------------------------------------------------------------------------------------------------------------------
//  Macht den übergeben UART-Port zur Standard Ein/Ausgabe für printf und scanf
//------------------------------------------------------------------------------------------------------------------------
//  ST_UART_t* st:    Der bei Init zurückgegebene Pointer
//------------------------------------------------------------------------------------------------------------------------
extern void UART_SetStandardOut(ST_UART_t* st);

#ifdef UART_UCA3_DMA_ENABLE
extern void UCA3_UART_RX_DMA_Read_Data(BYTE* buffer, BYTE len);
BYTE UCA3_UART_RX_DMA_get_ByteCount(void);

#endif

#endif