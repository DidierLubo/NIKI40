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
;*************************************************************************************************************************
;								                                                                                                 *
;       SPI - Modul (Include-File)                                                                                       *
;	                                                                                                                      *
;*************************************************************************************************************************
*/

/*
// Folgende defime's sind Hardware abhängig und sollten demzufolge in der Hardware.h definiert werden.

// Folgende define's bestimmen, welche SPI-Schnittstelle verwendet werden soll.
#define SPI_UCSI_A0_USED
#define SPI_UCSI_A1_USED
#define SPI_UCSI_A2_USED
#define SPI_UCSI_B0_USED
#define SPI_UCSI_B1_USED
#define SPI_UCSI_B2_USED

// Folgende define's bestimmen, die Bit-Rate (Geschwindigkeit) der SPI-Schnittstelle
#define SPI_UCSI_A0_BR
#define SPI_UCSI_A1_BR
#define SPI_UCSI_A2_BR
#define SPI_UCSI_B0_BR
#define SPI_UCSI_B1_BR
#define SPI_UCSI_B2_BR

// Folgende define's bestimmen, die Taktquelle (ACLK, SMCLK)
#define SPI_UCSI_A0_CLK_SRC
#define SPI_UCSI_A1_CLK_SRC
#define SPI_UCSI_A2_CLK_SRC
#define SPI_UCSI_B0_CLK_SRC
#define SPI_UCSI_B1_CLK_SRC
#define SPI_UCSI_B2_CLK_SRC

// Folgende define's bestimmen, den Initwert für das CTL0-Register
// Dadurch besteht völlige Kontrolle über das Taktverhalten ect.
#define SPI_UCSI_A0_CTL0_INIT
#define SPI_UCSI_A1_CTL0_INIT
#define SPI_UCSI_A2_CTL0_INIT
#define SPI_UCSI_B0_CTL0_INIT
#define SPI_UCSI_B1_CTL0_INIT
#define SPI_UCSI_B2_CTL0_INIT

// Beispiel für LIS3LV02DL-Sensor:
#define SPI_UCSI_B1_USED             // SPI-Modul informieren, dass SPI-B1-Schnitstelle verwendet werden soll
#define SPI_UCSI_B1_BR              0x3
#define SPI_UCSI_B1_CLK_SRC       SMCLK
#define SPI_UCSI_B1_CTL0_INIT   (UCCKPL | UCMSB | UCMST | UCMODE_0 | UCSYNC)

*/

#include "Hardware.h"

#define  ACLK  UCSSEL__ACLK
#define SMCLK  UCSSEL__SMCLK

//===========================================================================================
// Führt die Initialisierung der SPI-Schnistelle(n) durch
//===========================================================================================
void SPI_UCSI_Init(void)
{
#ifdef SPI_UCSI_A0_USED
   UCA0CTL1 = UCSWRST;

   #ifndef SPI_UCSI_A0_BR
   #warning "Fehler: No Define for SPI_UCSI_A0_BR"
   #endif
   UCA0BR0 = (SPI_UCSI_A0_BR & 0xFF);
   UCA0BR1 = (SPI_UCSI_A0_BR >> 8);

   #ifndef SPI_UCSI_A0_CTL0_INIT
   #warning "Fehler: No Define for SPI_UCSI_A0_CTL0_INIT"
   #endif
   UCA0CTL0 = SPI_UCSI_A0_CTL0_INIT;

   #ifndef SPI_UCSI_A0_CLK_SRC
   #warning "Fehler: No Define for SPI_UCSI_A0_CLK_SRC"
   #endif
   UCA0CTL1 = SPI_UCSI_A0_CLK_SRC;
   UCA0STAT = 0;
#endif

#ifdef SPI_UCSI_A1_USED
   UCA1CTL1 = UCSWRST;

   #ifndef SPI_UCSI_A1_BR
   #warning "Fehler: No Define for SPI_UCSI_A1_BR"
   #endif
   UCA1BR0 = (SPI_UCSI_A1_BR & 0xFF);
   UCA1BR1 = (SPI_UCSI_A1_BR >> 8);

   #ifndef SPI_UCSI_A1_CTL0_INIT
   #warning "Fehler: No Define for SPI_UCSI_A1_CTL0_INIT"
   #endif
   UCA1CTL0 = SPI_UCSI_A1_CTL0_INIT;

   #ifndef SPI_UCSI_A1_CLK_SRC
   #warning "Fehler: No Define for SPI_UCSI_A1_CLK_SRC"
   #endif
   UCA1CTL1 = SPI_UCSI_A1_CLK_SRC;
   UCA1STAT = 0;
#endif

#ifdef SPI_UCSI_A2_USED
   UCA2CTL1 = UCSWRST;

   #ifndef SPI_UCSI_A2_BR
   #warning "Fehler: No Define for SPI_UCSI_A2_BR"
   #endif
   UCA2BR0 = (SPI_UCSI_A2_BR & 0xFF);
   UCA2BR1 = (SPI_UCSI_A2_BR >> 8);

   #ifndef SPI_UCSI_A2_CTL0_INIT
   #warning "Fehler: No Define for SPI_UCSI_A2_CTL0_INIT"
   #endif
   UCA2CTL0 = SPI_UCSI_A2_CTL0_INIT;

   #ifndef SPI_UCSI_A2_CLK_SRC
   #warning "Fehler: No Define for SPI_UCSI_A2_CLK_SRC"
   #endif
   UCA2CTL1 = SPI_UCSI_A2_CLK_SRC;
   UCA2STAT = 0;
#endif

#ifdef SPI_UCSI_B0_USED
   UCB0CTL1 = UCSWRST;

   #ifndef SPI_UCSI_B0_BR
   #warning "Fehler: No Define for SPI_UCSI_B0_BR"
   #endif
   UCB0BR0 = (SPI_UCSI_B0_BR & 0xFF);
   UCB0BR1 = (SPI_UCSI_B0_BR >> 8);

   #ifndef SPI_UCSI_B0_CTL0_INIT
   #warning "Fehler: No Define for SPI_UCSI_B0_CTL0_INIT"
   #endif
   UCB0CTL0 = SPI_UCSI_B0_CTL0_INIT;

   #ifndef SPI_UCSI_B0_CLK_SRC
   #warning "Fehler: No Define for SPI_UCSI_B0_CLK_SRC"
   #endif
   UCB0CTL1 = SPI_UCSI_B0_CLK_SRC;
   UCB0STAT = 0;
#endif

#ifdef SPI_UCSI_B1_USED
   UCB1CTL1 = UCSWRST;

   #ifndef SPI_UCSI_B1_BR
   #warning "Fehler: No Define for SPI_UCSI_B1_BR"
   #endif
   UCB1BR0 = (SPI_UCSI_B1_BR & 0xFF);
   UCB1BR1 = (SPI_UCSI_B1_BR >> 8);

   #ifndef SPI_UCSI_B1_CTL0_INIT
   #warning "Fehler: No Define for SPI_UCSI_B1_CTL0_INIT"
   #endif
   UCB1CTL0 = SPI_UCSI_B1_CTL0_INIT;

   #ifndef SPI_UCSI_B1_CLK_SRC
   #warning "Fehler: No Define for SPI_UCSI_B1_CLK_SRC"
   #endif
   UCB1CTL1 = SPI_UCSI_B1_CLK_SRC;
   UCB1STAT = 0;
#endif

#ifdef SPI_UCSI_B2_USED
   UCB2CTL1 = UCSWRST;

   #ifndef SPI_UCSI_B2_BR
   #warning "Fehler: No Define for SPI_UCSI_B2_BR"
   #endif
   UCB2BR0 = (SPI_UCSI_B2_BR & 0xFF);
   UCB2BR1 = (SPI_UCSI_B2_BR >> 8);

   #ifndef SPI_UCSI_B2_CTL0_INIT
   #warning "Fehler: No Define for SPI_UCSI_B2_CTL0_INIT"
   #endif
   UCB2CTL0 = SPI_UCSI_B2_CTL0_INIT;

   #ifndef SPI_UCSI_B1_CLK_SRC
   #warning "Fehler: No Define for SPI_UCSI_B1_CLK_SRC"
   #endif
   UCB2CTL1 = SPI_UCSI_B2_CLK_SRC;
   UCB2STAT = 0;
#endif

}

//===========================================================================================
// Schreibt und liest ein Zeichen von der SPI-Schittstelle A0
//===========================================================================================
#ifdef SPI_UCSI_A0_USED
BYTE SPI_UCSI_A0_Put_Get_Byte(BYTE bData)
{
   UCA0TXBUF = bData;
   while (UCA0STAT & UCBUSY) _NOP();
   return UCA0RXBUF;
}
#endif

//===========================================================================================
// Schreibt und liest ein Zeichen von der SPI-Schittstelle A1
//===========================================================================================
#ifdef SPI_UCSI_A1_USED
BYTE SPI_UCSI_A1_Put_Get_Byte(BYTE bData)
{
   UCA1TXBUF = bData;
   while (UCA1STAT & UCBUSY) _NOP();
   return UCA1RXBUF;
}
#endif

//===========================================================================================
// Schreibt und liest ein Zeichen von der SPI-Schittstelle B0
//===========================================================================================
#ifdef SPI_UCSI_B0_USED
BYTE SPI_UCSI_B0_Put_Get_Byte(BYTE bData)
{
   UCB0TXBUF = bData;
   while (UCB0STAT & UCBUSY) _NOP();
   return UCB0RXBUF;
}
#endif

//===========================================================================================
// Schreibt und liest ein Zeichen von der SPI-Schittstelle B1
//===========================================================================================
#ifdef SPI_UCSI_B1_USED
BYTE SPI_UCSI_B1_Put_Get_Byte(BYTE bData)
{
   UCB1TXBUF = bData;
   while (UCB1STAT & UCBUSY) _NOP();
   return UCB1RXBUF;
}
#endif

//===========================================================================================
// Schreibt und liest ein Zeichen von der SPI-Schittstelle A2
//===========================================================================================
#ifdef SPI_UCSI_A2_USED
BYTE SPI_UCSI_A2_Put_Get_Byte(BYTE bData)
{
   UCA2TXBUF = bData;
   while (UCA2STAT & UCBUSY) _NOP();
   return UCA2RXBUF;
}
#endif

//===========================================================================================
// Schreibt und liest ein Zeichen von der SPI-Schittstelle B2
//===========================================================================================
#ifdef SPI_UCSI_B2_USED
BYTE SPI_UCSI_B2_Put_Get_Byte(BYTE bData)
{
   UCB2TXBUF = bData;
   while (UCABSTAT & UCBUSY) _NOP();
   return UCABRXBUF;
}
#endif
