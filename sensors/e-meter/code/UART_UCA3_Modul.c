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
;	      UART-Modul:    Untersützt UCA3...UCA3                                                                            *
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

#include "UART_USCI.h"
#include "LWM2M_App.h"
//#include "msp430f67791.h"

#include <String.h>
#include <stdlib.h>

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

#ifdef UCA3_USED

   #ifndef UCA3_LPM
      #warning "Kein Low-Power-Mode für UCA3-Modul angegeben. Default wird verwendet."
      #define UCA3_LPM             LPM0
   #endif

   #ifndef UCA3_LPM_EXIT
      #warning "Kein Low-Power-Exit-Mode für UCA3-Modul angegeben. Default wird verwendet."
      #define UCA3_LPM_EXIT        LPM0_EXIT
   #endif

   #ifndef UCA3_CLK_Source                            // Wenn keine Taktquelle angegeben dann SMCLK wählen (Kompatibilität)
     #define UCA3_CLK_Source SMCLK
   #endif

   const BYTE UCA3_S_Buffer_len = 32;
   const BYTE UCA3_E_Buffer_len = 256;

   BYTE bUCA3_S_Buffer[32];         // Ringbuffer für das Senden
   BYTE bUCA3_E_Buffer[256];         // Ringbuffer für den Empfang

   ST_UART_t stUCA3;
   
#ifdef UART_UCA3_DMA_ENABLE
    int8_t dma_sz = sizeof(bUCA3_E_Buffer);
    uint8_t storageIdx = 0;

    static void UCA3_UART_RX_DMA_Enable(void);
#endif

static uint16_t UCA3_getUCBRS(float fraction);
void UCA3_Put_Byte(register BYTE bZeichen);
BYTE UCA3_Get_Byte(void);

//------------------------------------------------------------------------------------------------------------------------
//  Hier wird erstmal die Hardware initalisiert
//------------------------------------------------------------------------------------------------------------------------

volatile ST_UART_t* UCA3_Init(long Baudrate)
{
   int16_t x = 0;
   uint16_t ucbrs = 0;
   float n = 0.0f;

   UCA3CTLW0 |= UCSWRST;
   UCA3CTLW0 |= UCSSEL_2;
   UCA3CTLW1 = 0;

   n = (float)BRCLK_FREQ / Baudrate; // Teilerfaktor berechnen
   ucbrs = UCA3_getUCBRS((float)(n - (uint16_t)n));
   
   if (n >= 16) // ja - n >= 16 - Oversampling Mode
   {
     UCA3BRW = (unsigned int)(n/16);

     //caluclate UCBRF
     x = (int16_t)(((n/16) - (int16_t)(n/16))*16);
     
     UCA3MCTLW |= ucbrs | ((int16_t)(x)<<4);
     UCA3MCTLW |= UCOS16; //UCOS16 = 1
   }
   else // nein Low-Frequenz Mode
   {
     UCA3BRW = (unsigned int)n;
     UCA3MCTLW = 0;

     //caluclate UCBRS
     UCA3MCTLW |= ucbrs | ((int16_t)(x)<<4);
   }

    UCA3CTLW0 &= ~UCSWRST;

    stUCA3.pbE_Buffer_WR = bUCA3_E_Buffer; // Zeiger für Interruptprogramm setzen
    stUCA3.pbE_Buffer_RD = bUCA3_E_Buffer;
    stUCA3.pbS_Buffer_WR = bUCA3_S_Buffer;
    stUCA3.pbS_Buffer_RD = bUCA3_S_Buffer;
    stUCA3.bE_Char_Counter = 0; // Keine Zeichen empfangen
    stUCA3.bS_Char_Counter = 0; // Keine Zeichen zu senden
    stUCA3.bE_Z_Status     = 0;

    stUCA3.Put_Byte = UCA3_Put_Byte;
    stUCA3.Get_Byte = UCA3_Get_Byte;

    UCA3IE |= UCRXIE;                           // RX interrupt UCA3 nun zulassen

#ifdef UART_UCA3_DMA_ENABLE
  UCA3IE &= ~UCRXIE;                            // RX interrupt UCA3 nun disablen
  UCA3_UART_RX_DMA_Enable();
#endif

   return &stUCA3;
}



uint16_t UCA3_getUCBRS(float fraction)
{ 
  uint16_t ucbrs_value = 0x00;
  
  if(fraction >= 0.0000f && fraction < 0.0529f)  
    ucbrs_value = 0x0000;
  else if(fraction >= 0.0529f && fraction < 0.0715f)
    ucbrs_value = 0x0100;
  else if(fraction >= 0.0715f && fraction < 0.0835f)
    ucbrs_value = 0x0200;
  else if(fraction >= 0.0835f && fraction < 0.1001f)
    ucbrs_value = 0x0400;
  else if(fraction >= 0.1001f && fraction < 0.1252f)
    ucbrs_value = 0x0800;
  else if(fraction >= 0.1252f && fraction < 0.1430f)
    ucbrs_value = 0x1000;
  else if(fraction >= 0.1430f && fraction < 0.1670f)
    ucbrs_value = 0x2000;
  else if(fraction >= 0.1670f && fraction < 0.2147f)
    ucbrs_value = 0x1100;
  else if(fraction >= 0.2147f && fraction < 0.2224f)
    ucbrs_value = 0x2100;    
  else if(fraction >= 0.2224f && fraction < 0.2503f)
    ucbrs_value = 0x2200;
  else if(fraction >= 0.2503f && fraction < 0.3000f)
    ucbrs_value = 0x4400;
  else if(fraction >= 0.3000f && fraction < 0.3335f)
    ucbrs_value = 0x2500;
  else if(fraction >= 0.3335f && fraction < 0.3575f)
    ucbrs_value = 0x4900;
  else if(fraction >= 0.3575f && fraction < 0.3753f)
    ucbrs_value = 0x4A00;
  else if(fraction >= 0.3753f && fraction < 0.4003f)
    ucbrs_value = 0x5200;
  else if(fraction >= 0.4003f && fraction < 0.4286f)
    ucbrs_value = 0x9200;
  else if(fraction >= 0.4286f && fraction < 0.4378f)
    ucbrs_value = 0x5300;
  else if(fraction >= 0.4378f && fraction < 0.5002f)
    ucbrs_value = 0x5500;
  else if(fraction >= 0.5002f && fraction < 0.5715f)
    ucbrs_value = 0xAA00;
  else if(fraction >= 0.5715f && fraction < 0.6003f)
    ucbrs_value = 0x6B00;
  else if(fraction >= 0.6003f && fraction < 0.6254f)
    ucbrs_value = 0xAD00;
  else if(fraction >= 0.6254f && fraction < 0.6432f)
    ucbrs_value = 0xB500;
  else if(fraction >= 0.6432f && fraction < 0.6667f)
    ucbrs_value = 0xB600;
  else if(fraction >= 0.6667f && fraction < 0.7001f)
    ucbrs_value = 0xD600;
  else if(fraction >= 0.7001f && fraction < 0.7147f)
    ucbrs_value = 0xB700;
  else if(fraction >= 0.7147f && fraction < 0.7503f)
    ucbrs_value = 0xDD00;    
  else if(fraction >= 0.7503f && fraction < 0.7861f)
    ucbrs_value = 0xED00;
  else if(fraction >= 0.7861f && fraction < 0.8004f)
    ucbrs_value = 0xEE00;
  else if(fraction >= 0.8004f && fraction < 0.8333f)
    ucbrs_value = 0xBF00;
  else if(fraction >= 0.8333f && fraction < 0.8464f)
    ucbrs_value = 0xDF00;
  else if(fraction >= 0.8464f && fraction < 0.8527f)
    ucbrs_value = 0xEF00;
  else if(fraction >= 0.8527f && fraction < 0.8751f)
    ucbrs_value = 0xF700;
  else if(fraction >= 0.8751f && fraction < 0.9004f)
    ucbrs_value = 0xFB00;
  else if(fraction >= 0.9004f && fraction < 0.9170f)
    ucbrs_value = 0xFD00;
  else if(fraction >= 0.9170f && fraction <= 0.9288f)
    ucbrs_value = 0xFE00;
  
  return ucbrs_value;
}

#ifdef UART_UCA3_DMA_ENABLE

static void UCA3_UART_RX_DMA_Enable(void)
{
   UCA3_UART_DMA0_DMA_TSEL |= UCA3_UART_DMA0_TSEL_VAL_SET;           // DMA is triggert bei UART
   UCA3_UART_DMA2_DMA_TSEL |= UCA3_UART_DMA2_TSEL_VAL_SET;           // DMA is triggert bei UART

   UCA3_UART_DMA0_DMA_SA  = (void*) &UCA3RXBUF;             // set source for DMA
   UCA3_UART_DMA0_DMA_DA  = bUCA3_E_Buffer;                 // set destination for DMA
   UCA3_UART_DMA0_DMA_SZ  = UCA3_E_Buffer_len;              // Länge des Buffers
   UCA3_UART_DMA2_DMA_SA  = (void*) &UCA3RXBUF;             // set source for DMA
   UCA3_UART_DMA2_DMA_DA  = bUCA3_E_Buffer;                 // set destination for DMA
   UCA3_UART_DMA2_DMA_SZ  = UCA3_E_Buffer_len;              // Länge des Buffers

   UCA3_UART_DMA0_DMA_CTL = DMADT_4      |                  // Repeated single transfer
                            DMADSTINCR_3 |                  // DST-Pointer + 1
                            DMASRCINCR_0 |                  // SCR-Pointer unverändert lassen
                            DMASBDB      |                  // BYTE to BYTE-Transfer
                            DMAEN;
   
   UCA3_UART_DMA2_DMA_CTL = DMADT_4      |                  // Repeated single transfer
                            DMADSTINCR_3 |                  // DST-Pointer + 1
                            DMASRCINCR_0 |                  // SCR-Pointer unverändert lassen
                            DMASBDB      |                  // BYTE to BYTE-Transfer
                            DMAEN;
}



void UCA3_UART_RX_DMA_Read_Data(uint8_t* buffer, uint8_t len)
{
    int8_t szDiff = UCA3_UART_DMA0_DMA_SZ - len;

    if(szDiff < 0)
    {
        while(UCA3_UART_DMA0_DMA_SZ > 0);
        memcpy(buffer, bUCA3_E_Buffer + storageIdx, (len+szDiff));
        UCA3_UART_DMA0_DMA_SZ = UCA3_E_Buffer_len;
        storageIdx = 0;
        dma_sz = UCA3_E_Buffer_len;
        while(UCA3_UART_DMA0_DMA_SZ > (dma_sz + szDiff));
        memcpy(buffer+(len+szDiff), bUCA3_E_Buffer + storageIdx, abs(szDiff));
    }
    else
    {
        while((dma_sz - UCA3_UART_DMA0_DMA_SZ) < len);
        memcpy(buffer, bUCA3_E_Buffer + storageIdx, len);
        storageIdx = storageIdx + len;
    }

    dma_sz = UCA3_UART_DMA0_DMA_SZ;
}



BYTE UCA3_UART_RX_DMA_get_ByteCount(void)
{
    return dma_sz - UCA3_UART_DMA0_DMA_SZ;
}

#endif

//------------------------------------------------------------------------------------------------------------------------
//  Damit wird ein Zeichen in den Ringbuffer übertragen. Ist der Buffer bereits voll wird gewartet, bis das zu sendende
//  Zeichen wieder Platz hat. Wenn keine Übertragung aktiv ist, wird diese damit eingeleitet
//------------------------------------------------------------------------------------------------------------------------

void UCA3_Put_Byte(register BYTE bZeichen)
{
   register BYTE *pbRingbuffer;
                                                      // Warte bis im Ringbuffer Platz für ein neues Zeichen ist (hier ist IRQ noch an
   while (stUCA3.bS_Char_Counter == UCA3_S_Buffer_len)
      UCA3_LPM;

   pbRingbuffer = (BYTE*) stUCA3.pbS_Buffer_WR;
   *pbRingbuffer = bZeichen;

   if ( pbRingbuffer  == &bUCA3_S_Buffer[UCA3_S_Buffer_len -1])   // Wenn buffer überundet wird, dann von vornen anfangem
       pbRingbuffer  =   (BYTE*) bUCA3_S_Buffer;
   else pbRingbuffer++;                               // Buffer erhöhen

   stUCA3.pbS_Buffer_WR = pbRingbuffer;               // Neue Schreibposition des WR Pointers übertragen
   stUCA3.bS_Char_Counter++;                          // Zeichenzähler erhöhen

   UCA3IE   |= UCTXIE;                                // Interrupt freigeben, es wird sofort in den Interrupt verzweigt
}

//------------------------------------------------------------------------------------------------------------------------
//  Übergibt ein Zeichen aus dem Empfangsbuffer - Falls keines da wird auf ein Zeichen gewartet
//------------------------------------------------------------------------------------------------------------------------

BYTE UCA3_Get_Byte(void)
{
  BYTE bZeichen;

  while(!stUCA3.bE_Char_Counter)                      // Warte bis ein Byte da
     UCA3_LPM;

  bZeichen = *stUCA3.pbE_Buffer_RD;                   // Zeichen auslesen

  stUCA3.bE_Char_Counter--;
  if  (stUCA3.pbE_Buffer_RD == &bUCA3_E_Buffer[UCA3_E_Buffer_len-1]) // Lespointer erhöhen (Ringbuffer)
       stUCA3.pbE_Buffer_RD = (BYTE*) bUCA3_E_Buffer;
  else stUCA3.pbE_Buffer_RD++;

  if (stUCA3.bE_Z_Status & Z_Status_ECHO)
    UCA3_Put_Byte(bZeichen);

  return (bZeichen);
}

//------------------------------------------------------------------------------------------------------------------------
//  Sende IRQ-Programm-Teil
//------------------------------------------------------------------------------------------------------------------------
#pragma vector=USCI_A3_VECTOR
__interrupt void UCA3_Sende_IRQ( void )
{
   if( UCA3IFG & UCTXIFG )
   {
     if (stUCA3.bS_Char_Counter == 0)
      {
         UCA3IE &= ~UCTXIE;                           // TX interrupt sperren wenn alle Zeichen gesendet
      }
      else
      {
         UCA3TXBUF = *stUCA3.pbS_Buffer_RD;           // Zeichen Senden
         if ( stUCA3.pbS_Buffer_RD == &bUCA3_S_Buffer[UCA3_S_Buffer_len-1])             // Lessebuffer anpassen
            stUCA3.pbS_Buffer_RD =  bUCA3_S_Buffer;
         else stUCA3.pbS_Buffer_RD++;
            stUCA3.bS_Char_Counter--;                 // Zeichenzähler verringern
         UCA3_LPM_EXIT;
      }
   }

   if( UCA3IFG & UCRXIFG )
   {
      register BYTE *pbE_Buffer = stUCA3.pbE_Buffer_WR;
      register BYTE bZeichen;
      
      UCA3STATW = 0;
      bZeichen = UCA3RXBUF;
      
      *pbE_Buffer = bZeichen;

      if ( pbE_Buffer == &bUCA3_E_Buffer[UCA3_E_Buffer_len-1])
           pbE_Buffer =  bUCA3_E_Buffer;              // Wenn Ringbufferende dann an den Ringbufferanfang stellen
      else pbE_Buffer++;                              // sonst Zeiger einfach um eins erhöhen.


      if ( pbE_Buffer !=  stUCA3.pbE_Buffer_RD)       // Wenn nicht Empfangsbuffer überlauf dann Zeiger auf nächste
      { stUCA3.pbE_Buffer_WR = pbE_Buffer;            // Bufferposition setzen
         stUCA3.bE_Char_Counter++;
      }
      
      if(stUCA3.bE_Char_Counter == 5)
      {
        if(bZeichen == 0x80 || //Resource Read Request
           bZeichen == 0x82 || //Resource Write Request
           bZeichen == 0x90 || //Instance Read Request 
           bZeichen == 0x92 ||
           bZeichen == 0x32)   //LWM2M Status Started
        {
          LWM2M_Task |= LWM2M_TASK_UART_RX;
        }
      }
      
      UCA3_LPM_EXIT;
   }
}

#ifdef UCA3_SelbstTest
void UCA3_Selftest(void)
{
  BYTE Buffer[E_Buffer_len];
  register BYTE i;

  UCA3_Init(BR115200);
  UCA3_Put_String((BYTE*) "\n\rV24 Selbstest - Bitte 2 mal eine Taste drücken:\r\n");

  UCA3_Get_Byte();

  i = '0';
  while (UCA3_Get_RXD_Anzahl()==0)                    // Sende solange Zeichen bis ein Zeichen empfangen wurde
  { UCA3_Put_Byte(i);
    i++;
    if (i=='z') i='0';
  }
  UCA3_Put_String((BYTE*) "\n\rFolgendes Zeichen wurde erkannt:");
  UCA3_Put_Byte(UCA3_Get_Byte());
  UCA3_Put_String((BYTE*) "\n\rBitte geben Sie einen Text ein - Dieser sollte nach einem Return sichtbar werden.\r\n");

  // Warte bis CR LF erkannt
  while (UCA3_Get_Zeichen_Status((Z_Status_CR | Z_Status_LF), (Z_Status_CR | Z_Status_LF))) ;

  UCA3_Get_Line(Buffer, E_Buffer_len, 1);
  UCA3_Put_String(Buffer);
  UCA3_Put_String((BYTE*) "\r\nSelbsttest-Ende");

  // Warte bis alles gesendet
  while (UCA3_Get_TXD_Anzahl()) ;
}
#endif
#endif                                                // ifdef UCA3_USED