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
/*
;**************************************************************************************************************************
;								                                                                                                  *
;	      UART-Modul:    Untersützt UCA2...UCA3                                                                            *
;								                                                                                                  *
;**************************************************************************************************************************
;								                                                                                                  *
;        Achtung: Die Ports-Select und -Dir müssen außerhalb gesetzt werden ( init-Funktion )                             *
;								                                                                                                  *
;**************************************************************************************************************************
;                                                                                                                         *
;        Neue Defines ab dem 13.10.2008                                                                                   *
;								                                                                                                  *
;**************************************************************************************************************************
*/

#include <stdint.h>
#include "Hardware.h"
#include "UART_USCI.h"

// Taktquellen für den Baudratengenerator
#define SMCLK  UCSSEL_2
//#define MCLK   0x20
#define ACLK   UCSSEL_1
#define UCLKI  UCSSEL_0                               // Externer Clock (ULCKI)

// Diese Defines sollten in der Hardware.h gemacht werden
// Sie bestimmen das Verhalten, wenn auf den Empfang eines V24-Zeichens gewartet wird
// bzw. wenn der Sendebuffer voll ist und gewartet wird, bis ein neues Zeichen darin platz hat.
// Für alte Module können die Warnings ignoiert werden. Sie stellen nur ein Hinweis dar,
// dass diese Definitionen nicht gemacht wurden.
// Default-Mäßig wird nur die CPU angehalten (LPM0). Das ist in allen Fällen ok da der MCLK nicht als
// Taktquelle für den UART gewählt werden kann.
// Nur ist es in manchen Applicationen möglich noch weitere CPU-Teile schlafen zu
// legen um Strom zu sparen. Dafür sind diese Defines gedacht.
// Der LMP-mode der gewählt werden kann, hängt von der Taktquelle des UART's ab.
// Es muss sichergestellt sein, dass diese läuft - sonst bleibt das Programm stehen!!

#ifdef UCA2_USED

   #ifndef UCA2_LPM
      #warning "Kein Low-Power-Mode für UCA2-Modul angegeben. Default wird verwendet."
      #define UCA2_LPM             LPM0
   #endif

   #ifndef UCA2_LPM_EXIT
      #warning "Kein Low-Power-Exit-Mode für UCA2-Modul angegeben. Default wird verwendet."
      #define UCA2_LPM_EXIT        LPM0_EXIT
   #endif

   #ifndef UCA2_CLK_Source                            // Wenn keine Taktquelle angegeben dann SMCLK wählen (Kompatibilität)
     #define UCA2_CLK_Source SMCLK
   #endif

   BYTE    bUCA2_S_Buffer[UCA2_S_Buffer_len];         // Ringbuffer für das Senden
   BYTE    bUCA2_E_Buffer[UCA2_E_Buffer_len];         // Ringbuffer für den Empfang

   ST_UART_t stUCA2;


void UCA2_Put_Byte(register BYTE bZeichen);
BYTE UCA2_Get_Byte(void);

//------------------------------------------------------------------------------------------------------------------------
//  Hier wird erstmal die Hardware initalisiert
//------------------------------------------------------------------------------------------------------------------------

volatile ST_UART_t* UCA2_Init(long Baudrate)
{

   UCA2CTL0 = 0x00;                                   // No Parity, 8 Datenbits, 1 Stopbit, Asynchron Moder
   UCA2CTL1 = UCSWRST;                                // Rücksetzen der UART A0
   UCA2CTL1 = UCA2_CLK_Source;                        // Taktquelle muss mit BAUDRATE_CLK betrieben werden

   float n,x;
   n = (float)UCA2_BAUDRATE_CLK / (float)Baudrate;    // Teilerfaktor berechnen


   if (n >= 32)                                       // ja - n >= 32 - Oversampling Mode
   {

     UCA2BRW = (unsigned int)(n/16) % 256;
     UCA2MCTL = 0;

     //caluclate UCBRF
     x = (int)(((n/16 - (int)(n/16))*16) + 0.5);
     UCA2MCTL |= (int)(x)<<4;
     UCA2MCTL |= UCOS16;                                //UCOS16 = 1
   }
   else                                                 // nein Low-Frequenz Mode
   {
     UCA2BRW = (unsigned int)n;
      UCA2MCTL = 0;
     //caluclate UCBRS
     x = (int)((n-(int)(n))*8+0.5);
     UCA2MCTL |= (int)(x)<<1;
   }


   stUCA2.pbE_Buffer_WR = bUCA2_E_Buffer;             // Zeiger für Interruptprogramm setzen
   stUCA2.pbE_Buffer_RD = bUCA2_E_Buffer;
   stUCA2.pbS_Buffer_WR = bUCA2_S_Buffer;
   stUCA2.pbS_Buffer_RD = bUCA2_S_Buffer;

   stUCA2.bE_Char_Counter = 0;                        // Keine Zeichen empfangen
   stUCA2.bS_Char_Counter = 0;                        // Keine Zeichen zu senden

   stUCA2.bE_Z_Status     = 0;


   stUCA2.Put_Byte = UCA2_Put_Byte;
   stUCA2.Get_Byte = UCA2_Get_Byte;
   UCA2IE |= UCRXIE | UCTXIE;                         // RX interrupt UCA2 nun zulassen

   return &stUCA2;
}

//------------------------------------------------------------------------------------------------------------------------
//  Damit wird ein Zeichen in den Ringbuffer übertragen. Ist der Buffer bereits voll wird gewartet, bis das zu sendende
//  Zeichen wieder Platz hat. Wenn keine Übertragung aktiv ist, wird diese damit eingeleitet
//------------------------------------------------------------------------------------------------------------------------

void UCA2_Put_Byte(register BYTE bZeichen)
{
   register BYTE *pbRingbuffer;
                                                      // Warte bis im Ringbuffer Platz für ein neues Zeichen ist (hier ist IRQ noch an
   while (stUCA2.bS_Char_Counter == UCA2_S_Buffer_len)
      UCA2_LPM;

   pbRingbuffer = (BYTE*) stUCA2.pbS_Buffer_WR;
   *pbRingbuffer = bZeichen;

   if ( pbRingbuffer  == &bUCA2_S_Buffer[UCA2_S_Buffer_len -1])   // Wenn buffer überundet wird, dann von vornen anfangem
       pbRingbuffer  =   (BYTE*) bUCA2_S_Buffer;
   else pbRingbuffer++;                               // Buffer erhöhen

   stUCA2.pbS_Buffer_WR = pbRingbuffer;               // Neue Schreibposition des WR Pointers übertragen
   stUCA2.bS_Char_Counter++;                          // Zeichenzähler erhöhen

   UCA2IE   |= UCTXIE;                                // Interrupt freigeben, es wird sofort in den Interrupt verzweigt

}

//------------------------------------------------------------------------------------------------------------------------
//  Übergibt ein Zeichen aus dem Empfangsbuffer - Falls keines da wird auf ein Zeichen gewartet
//------------------------------------------------------------------------------------------------------------------------

BYTE UCA2_Get_Byte(void)
{ BYTE bZeichen;

  while(!stUCA2.bE_Char_Counter)                      // Warte bis ein Byte da
     UCA2_LPM;

  bZeichen = *stUCA2.pbE_Buffer_RD;                   // Zeichen auslesen

  stUCA2.bE_Char_Counter--;
  if  (stUCA2.pbE_Buffer_RD == &bUCA2_E_Buffer[UCA2_E_Buffer_len-1]) // Lespointer erhöhen (Ringbuffer)
       stUCA2.pbE_Buffer_RD = (BYTE*) bUCA2_E_Buffer;
  else stUCA2.pbE_Buffer_RD++;

  if (stUCA2.bE_Z_Status & Z_Status_ECHO)
    UCA2_Put_Byte(bZeichen);

  return (bZeichen);
}

//------------------------------------------------------------------------------------------------------------------------
//  Sende IRQ-Programm-Teil
//------------------------------------------------------------------------------------------------------------------------
#pragma vector=USCI_A2_VECTOR
__interrupt void UCA2_Sende_IRQ( void )
{
   if( UCA2IFG & UCTXIFG )
   {
      if (stUCA2.bS_Char_Counter == 0)
      {
         UCA2IE   &= ~UCTXIE;                         // TX interrupt sperren wenn alle Zeichen gesendet
      }
      else
      {
         UCA2TXBUF = *stUCA2.pbS_Buffer_RD;           // Zeichen Senden
         if ( stUCA2.pbS_Buffer_RD == &bUCA2_S_Buffer[UCA2_S_Buffer_len-1])             // Lessebuffer anpassen
            stUCA2.pbS_Buffer_RD =  bUCA2_S_Buffer;
         else stUCA2.pbS_Buffer_RD++;
            stUCA2.bS_Char_Counter--;                 // Zeichenzähler verringern
         UCA2_LPM_EXIT;
      }
   }
   if( UCA2IFG & UCRXIFG )
   {
      register BYTE *pbE_Buffer = stUCA2.pbE_Buffer_WR;
      register BYTE bZeichen;

      UCA2STAT = 0;
      bZeichen = UCA2RXBUF;
      if (bZeichen == '\r') stUCA2.bE_Z_Status |= Z_Status_CR;
      if (bZeichen == '\n') stUCA2.bE_Z_Status |= Z_Status_LF;

      *pbE_Buffer = bZeichen;

      if ( pbE_Buffer == &bUCA2_E_Buffer[UCA2_E_Buffer_len-1])
           pbE_Buffer =  bUCA2_E_Buffer;              // Wenn Ringbufferende dann an den Ringbufferanfang stellen
      else pbE_Buffer++;                              // sonst Zeiger einfach um eins erhöhen.


      if ( pbE_Buffer !=  stUCA2.pbE_Buffer_RD)       // Wenn nicht Empfangsbuffer überlauf dann Zeiger auf nächste
      { stUCA2.pbE_Buffer_WR = pbE_Buffer;            // Bufferposition setzen
         stUCA2.bE_Char_Counter++;
      }
      UCA2_LPM_EXIT;
      UCA2IFG &= ~UCRXIFG;
   }
}

#ifdef UCA2_SelbstTest
void UCA2_Selftest(void)
{
  BYTE Buffer[E_Buffer_len];
  register BYTE i;

  UCA2_Init(BR115200);
  UCA2_Put_String((BYTE*) "\n\rV24 Selbstest - Bitte 2 mal eine Taste drücken:\r\n");

  UCA2_Get_Byte();

  i = '0';
  while (UCA2_Get_RXD_Anzahl()==0)                    // Sende solange Zeichen bis ein Zeichen empfangen wurde
  { UCA2_Put_Byte(i);
    i++;
    if (i=='z') i='0';
  }
  UCA2_Put_String((BYTE*) "\n\rFolgendes Zeichen wurde erkannt:");
  UCA2_Put_Byte(UCA2_Get_Byte());
  UCA2_Put_String((BYTE*) "\n\rBitte geben Sie einen Text ein - Dieser sollte nach einem Return sichtbar werden.\r\n");

  // Warte bis CR LF erkannt
  while (UCA2_Get_Zeichen_Status((Z_Status_CR | Z_Status_LF), (Z_Status_CR | Z_Status_LF))) ;

  UCA2_Get_Line(Buffer, E_Buffer_len, 1);
  UCA2_Put_String(Buffer);
  UCA2_Put_String((BYTE*) "\r\nSelbsttest-Ende");

  // Warte bis alles gesendet
  while (UCA2_Get_TXD_Anzahl()) ;
}
#endif
#endif                                                // ifdef UCA2_USED