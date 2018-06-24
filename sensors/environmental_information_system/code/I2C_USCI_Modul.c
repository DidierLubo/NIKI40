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
//************************************************************************************************************************
//    I²C-Modul                                                                                                          *
//************************************************************************************************************************
//                                                                                                                       *
//    Erstellt am       28.05.2013                                                                                       *
//    Erstellt durch:   Bernd Ehrbrecht                                                                                  *
//                                                                                                                       *
//    Interrupts wurden nicht genutzt, da Sensordaten durch einen anderen IRQ-Programmteil aufgerufen wird.              *
//    Dadurch soll verhindert werden, dass ein Stacküberlauf durch nested Interrupts auftreten kann.                     *
//                                                                                                                       *
//************************************************************************************************************************
//                                                                                                                       *
// in Hardware.h zu machende Definitionen:  (für alle benutzten I²C-Module (B0/B1/B2))                                   *
//                                                                                                                       *
// #define I2C_USE_USCI_B0                                                                                               *
// #define I2C_B0_CLK_SOURCE        SMCLK                                                                                *
// #define I2C_B0_CLK_FREQ          SMCLK_FREQ                                                                           *
// #define I2C_B0_SLAVE_ADR         0x02                                                                                 *
// #define I2C_B0_HS_ADR                                                                                                 *
//                                                                                                                       *
// neu ab 27.04.2016                                                                                                     *
//                                                                                                                       *
// #define I2C_B0_TIME_OUT_FUNC     HAL_GetTickCount  diese Funktion muss einen Timer-Wert zurückliefern (ms)            *
// #define I2C_B0_TIME_OUT_VALUE    3                 im Beispiel tritt der Timeout nach 3-4ms ein                       *
//                                                                                                                       *
//************************************************************************************************************************

#include "Hardware.h"
#include "I2C_USCI_Modul.h"

#if defined(I2C_B0_TIME_OUT_FUNC) && !defined(I2C_B0_TIME_OUT_VALUE)
#error "I2C_B0_TIME_OUT_FUNC ohne define für I2C_B0_TIME_OUT_VALUE geht nicht
#endif

#if defined(I2C_B1_TIME_OUT_FUNC) && !defined(I2C_B1_TIME_OUT_VALUE)
#error "I2C_B1_TIME_OUT_FUNC ohne define für I2C_B1_TIME_OUT_VALUE geht nicht
#endif

#if defined(I2C_B2_TIME_OUT_FUNC) && !defined(I2C_B2_TIME_OUT_VALUE)
#error "I2C_B2_TIME_OUT_FUNC ohne define für I2C_B2_TIME_OUT_VALUE geht nicht
#endif

#define SMCLK  UCSSEL_2
#define ACLK   UCSSEL_1

#define I2C_0_BUSY         BIT0
#define I2C_1_BUSY         BIT1
#define I2C_2_BUSY         BIT2

BYTE mbIsBusyFlg;                                     // Flag für beide I2C siehe defines

#define SET_UCB0_400kBAUD   UCB0BRW  = I2C_B0_CLK_FREQ / 4000000;   // set prescaler für 400kBit/sec default
#define SET_UCB0_100kBAUD   UCB0BRW  = I2C_B0_CLK_FREQ / 1000000;   // set prescaler für 400kBit/sec default

#define SET_UCB1_400kBAUD   UCB1BRW  = I2C_B1_CLK_FREQ / 4000000;   // set prescaler für 400kBit/sec default
#define SET_UCB1_100kBAUD   UCB1BRW  = I2C_B1_CLK_FREQ / 1000000;   // set prescaler für 400kBit/sec default

#define SET_UCB2_400kBAUD   UCB1BRW  = I2C_B2_CLK_FREQ / 4000000;   // set prescaler für 400kBit/sec default
#define SET_UCB2_100kBAUD   UCB1BRW  = I2C_B2_CLK_FREQ / 1000000;   // set prescaler für 400kBit/sec default

//------------------------------------------------------------------------------------------------------------------------
//    I2C_Error: nur zum Debuggen gedacht
//------------------------------------------------------------------------------------------------------------------------
void I2C_Error(int iError)
{
   while (iError)
   {
      LED_GE_TOGGLE;                                  // hier Breakpoint setzen
      __delay_cycles(10);
      LED_GE_TOGGLE;
      iError--;
   }
}

#ifdef I2C_USE_USCI_B0

//------------------------------------------------------------------------------------------------------------------------
//    I2C_Init:
//------------------------------------------------------------------------------------------------------------------------
//    Initialisiert die UCSI-Schnittstelle und verwendet die angegebene Bausrate (100000 / 400000)
//------------------------------------------------------------------------------------------------------------------------
//    BaudRate:      (100000 / 400000)
//------------------------------------------------------------------------------------------------------------------------
void I2C_B0_Init(long BaudRate)
{
   UCB0CTL1 = UCSWRST;                                // Enable SW reset
   UCB0CTL0 = UCMST | UCMODE_3  | UCSYNC;             // I2C Master, synchronous mode
   UCB0CTL1 = I2C_B0_CLK_SOURCE | UCSWRST;            // Use SMCLK, keep SW reset
   UCB0BRW  = I2C_B0_CLK_FREQ / BaudRate;             // set prescaler für 100kBit/sec default

   UCB0I2CSA = I2C_B0_SLAVE_ADR;                      // set slave address
   UCB0CTL1 &= ~UCSWRST;                              // Clear SW reset, resume operation
   mbIsBusyFlg &= ~I2C_0_BUSY;
}

//------------------------------------------------------------------------------------------------------------------------
//    I2C_ReadRegister:
//------------------------------------------------------------------------------------------------------------------------
//    Liest ein Register von der angegebenen I²C-Slave-Adresse und der angegeben Register-Adresse
//------------------------------------------------------------------------------------------------------------------------
//    I2C_Slave_ADR:       I²C-Slave-Adresse (Device-Adresse) ohne RD/WR-Bit
//    I2C_Register:        Register-Adresse
//    bBuffer:             Pointer wo die Daten hin sollen
//    ByteCount:           Anzahl der zu lesenden Bytes
//------------------------------------------------------------------------------------------------------------------------
//    Return int:          0 Alles ok sonst Fehlernummer
//------------------------------------------------------------------------------------------------------------------------
//__monitor int I2C_B0_ReadRegister( char I2C_Slave_ADR, char I2C_Register, void* pvDaten, char ByteCounter)
// Testweise ohne monitor
int I2C_B0_ReadRegister( char I2C_Slave_ADR, char I2C_Register, void* pvDaten, char ByteCounter)
{
   BYTE *bBuffer = (BYTE*) pvDaten;

   if (mbIsBusyFlg & I2C_0_BUSY)
      return 4;

   mbIsBusyFlg |= I2C_0_BUSY;
   int iRetValue=0;

#ifdef I2C_B0_HS_ADR
   UCB0I2CSA  = I2C_HS_ADR;                           // set HS-Adress
   UCB0CTL1  |= (UCTXSTT);                            // I2C TX, start condition
   while (!(UCB0IFG & UCNACKIFG) );                   // Warte auf NAK
   UCB0IFG &= ~UCNACKIFG;
   SET_UCB0_400kBAUD;
#endif

#ifdef I2C_B0_TIME_OUT_FUNC
   unsigned long ulStartTime = I2C_B0_TIME_OUT_FUNC();
#endif

   UCB0I2CSA  = I2C_Slave_ADR;                        // set slave address
   UCB0CTL1  |= (UCTR | UCTXSTT);                     // I2C TX, start condition

   while (!(UCB0IFG & UCTXIFG) )                      // Warte bis Start-Kondition und Slave-Adresse gesendet
   {
#ifdef I2C_B0_TIME_OUT_FUNC
      if ((UCB0IFG & UCNACKIFG) || (I2C_B0_TIME_OUT_FUNC() - ulStartTime) > I2C_B0_TIME_OUT_VALUE)
#else
      if (UCB0IFG & UCNACKIFG)
#endif
      {
         iRetValue=1;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }
   }

   UCB0TXBUF  = I2C_Register;                         // Registerdaresse senden

#ifdef I2C_B0_TIME_OUT_FUNC
   ulStartTime = I2C_B0_TIME_OUT_FUNC();
#endif

   while (!(UCB0IFG & UCTXIFG) )                      // Warte bis senden abgeschlossen
   {
#ifdef I2C_B0_TIME_OUT_FUNC
      if ((UCB0IFG & UCNACKIFG) || (I2C_B0_TIME_OUT_FUNC() - ulStartTime) > I2C_B0_TIME_OUT_VALUE)
#else
      if (UCB0IFG & UCNACKIFG)
#endif
      {
         iRetValue=2;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }
   }

   UCB0IFG &= ~UCTXIFG;

   UCB0CTL1  &= ~UCTR ;                               // I2C Transmit-Bit löschen
   UCB0CTL1  |=  UCTXSTT ;                            // I2C TX, Restart condition

#ifdef I2C_B0_TIME_OUT_FUNC
   ulStartTime = I2C_B0_TIME_OUT_FUNC();
#endif
   while ( UCB0CTL1  & UCTXSTT )                      // Warte bis senden der I²C-Adresse abgeschlossen
   {
#ifdef I2C_B0_TIME_OUT_FUNC
      if ((UCB0IFG & UCNACKIFG) || (I2C_B0_TIME_OUT_FUNC() - ulStartTime) > I2C_B0_TIME_OUT_VALUE)
#else
      if (UCB0IFG & UCNACKIFG)
#endif
      {
         iRetValue=3;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }
   }

   while (ByteCounter)
   {
      if (ByteCounter == 1)
         UCB0CTL1 |= (UCTXSTP);                       // Stop-Condition senden wenn letztes Byte empfangen wird

#ifdef I2C_B0_TIME_OUT_FUNC
      ulStartTime = I2C_B0_TIME_OUT_FUNC();
#endif
      while (!(UCB0IFG & UCRXIFG) )                   // Warte bis senden abgeschlossen
      {
#ifdef I2C_B0_TIME_OUT_FUNC
         if ((I2C_B0_TIME_OUT_FUNC() - ulStartTime) > I2C_B0_TIME_OUT_VALUE)
         {
            iRetValue=4;
            I2C_Error( iRetValue );
            goto ErrorExit;
         }
#else
         _NOP();
#endif
      }
      *bBuffer++ = UCB0RXBUF;                         // Zeichen Einlesen und speichern Pointer erhöhen
      ByteCounter--;
   }

   goto NormalExit;

ErrorExit:
   UCB0CTL1 |=  UCTXSTP;                              // send STOP if slave sends NACK
   UCB0IFG  &= ~UCNACKIFG;
NormalExit:
#ifdef I2C_B0_TIME_OUT_FUNC
   ulStartTime = I2C_B0_TIME_OUT_FUNC();
#endif
   while (UCB0CTL1 & UCTXSTP)                         // Warte bis Stop-Condition gesendet
#ifdef I2C_B0_TIME_OUT_FUNC
      if ((I2C_B0_TIME_OUT_FUNC() - ulStartTime) > I2C_B0_TIME_OUT_VALUE)
      {
         iRetValue=5;
         I2C_Error( iRetValue );
         UCB0CTL1 |=  UCSWRST;                        // Clear SW reset, resume operation
         _NOP();
         UCB0CTL1 &= ~UCSWRST;                        // Clear SW reset, resume operation
         break;
      }
#else
      _NOP();
#endif

#ifdef I2C_B0_HS_ADR
   SET_UCB0_100kBAUD;                                 // High-Speed-Mode beenden
   #endif
   mbIsBusyFlg &= ~I2C_0_BUSY;
   return iRetValue;
}

//------------------------------------------------------------------------------------------------------------------------
//    I2C_WriteRegister:
//------------------------------------------------------------------------------------------------------------------------
//    Liest ein Register von der angegebenen I²C-Slave-Adresse und der angegeben Register-Adresse
//------------------------------------------------------------------------------------------------------------------------
//    I2C_Slave_ADR:       I²C-Slave-Adresse (Device-Adresse) ohne RD/WR-Bit
//    I2C_Register:        Register-Adresse
//    bBuffer:             Pointer wo die Daten hin sollen
//    ByteCount:           Anzahl der zu lesenden Bytes
//------------------------------------------------------------------------------------------------------------------------
//    Return int:          0 Alles ok sonst Fehlernummer
//------------------------------------------------------------------------------------------------------------------------
// Testweise ohne monitor
int I2C_B0_WriteRegister( char I2C_Slave_ADR, char I2C_Register, void* pvDaten, char ByteCounter)
{
   if (mbIsBusyFlg & I2C_0_BUSY)
      return 4;

   mbIsBusyFlg |= I2C_0_BUSY;
   BYTE *bBuffer = (BYTE*) pvDaten;

   int iRetValue=0;

   #ifdef I2C_B0_HS_ADR
   UCB0I2CSA  = I2C_HS_ADR;                           // set HS-Adress
   UCB0CTL1  |= (UCTXSTT);                            // I2C TX, start condition
   while (!(UCB0IFG & UCNACKIFG) );                   // Warte auf NAK
   UCB0IFG &= ~UCNACKIFG;
   SET_UCB0_400kBAUD;
   #endif

   UCB0I2CSA  = I2C_Slave_ADR;                        // set slave address
   UCB0CTL1  |= UCTR | UCTXSTT;                       // I2C TX, start condition

#ifdef I2C_B0_TIME_OUT_FUNC
   unsigned long ulStartTime = I2C_B0_TIME_OUT_FUNC();
#endif

   while (!(UCB0IFG & UCTXIFG) )                      // Warte bis Start-Kondition und Slave-Adresse gesendet
   {
#ifdef I2C_B0_TIME_OUT_FUNC
      if ((UCB0IFG & UCNACKIFG) || (I2C_B0_TIME_OUT_FUNC() - ulStartTime) > I2C_B0_TIME_OUT_VALUE)
#else
      if (UCB0IFG & UCNACKIFG)
#endif
      {
         iRetValue = 1;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }
   }
   UCB0IFG &= ~UCTXIFG;

   UCB0TXBUF = I2C_Register;                          // Registerdaresse senden

   while (ByteCounter)
   {
#ifdef I2C_B0_TIME_OUT_FUNC
      ulStartTime = I2C_B0_TIME_OUT_FUNC();
#endif
      while (!(UCB0IFG & UCTXIFG) )                   // Warte bis senden abgeschlossen
      {
#ifdef I2C_B0_TIME_OUT_FUNC
      if ((UCB0IFG & UCNACKIFG) || (I2C_B0_TIME_OUT_FUNC() - ulStartTime) > I2C_B0_TIME_OUT_VALUE)
#else
      if (UCB0IFG & UCNACKIFG)
#endif
         {
            iRetValue = 2;
            I2C_Error( iRetValue );
            goto ErrorExit;
         }
      }

      UCB0IFG &= ~UCTXIFG;

      UCB0TXBUF = *bBuffer++;                         // Zeichen senden und Pointer erhöhen
      ByteCounter--;

   }

#ifdef I2C_B0_TIME_OUT_FUNC
      ulStartTime = I2C_B0_TIME_OUT_FUNC();
#endif
   while (!(UCB0IFG & UCTXIFG) )                      // Warte bis senden des letzten Bytes abgeschlossen
   {
#ifdef I2C_B0_TIME_OUT_FUNC
      if ((UCB0IFG & UCNACKIFG) || (I2C_B0_TIME_OUT_FUNC() - ulStartTime) > I2C_B0_TIME_OUT_VALUE)
#else
      if (UCB0IFG & UCNACKIFG)
#endif
      {
         iRetValue = 3;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }
   }

   // hier geht es ohne Fehler weiter (iRetValue = 0)

ErrorExit:
   UCB0CTL1 |=  UCTXSTP;                              // send STOP if slave sends NACK
   UCB0IFG  &= ~UCNACKIFG;
#ifdef I2C_B0_TIME_OUT_FUNC
   ulStartTime = I2C_B0_TIME_OUT_FUNC();
#endif
   while (UCB0CTL1 & UCTXSTP)                         // Warte bis Stop-Condition gesendet
#ifdef I2C_B0_TIME_OUT_FUNC
      if ((I2C_B0_TIME_OUT_FUNC() - ulStartTime) > I2C_B0_TIME_OUT_VALUE)
      {
         iRetValue=5;
         I2C_Error( iRetValue );
         UCB0CTL1 |=  UCSWRST;                        // Clear SW reset, resume operation
         _NOP();
         UCB0CTL1 &= ~UCSWRST;                        // Clear SW reset, resume operation
         break;
      }
#else
      _NOP();
#endif
   #ifdef I2C_B0_HS_ADR
   SET_UCB0_100kBAUD;                                 // wieder auf Low-Speed zurückschalten
   #endif
   mbIsBusyFlg &= ~I2C_0_BUSY;
   return iRetValue;
}

//------------------------------------------------------------------------------------------------------------------------
//    I2C_ReadRegister:
//------------------------------------------------------------------------------------------------------------------------
//    Liest ein Register von der angegebenen I²C-Slave-Adresse und der angegeben Register-Adresse
//------------------------------------------------------------------------------------------------------------------------
//    I2C_Slave_ADR:       I²C-Slave-Adresse (Device-Adresse) ohne RD/WR-Bit
//    I2C_Register:        Register-Adresse
//    bBuffer:             Pointer wo die Daten hin sollen
//    ByteCount:           Anzahl der zu lesenden Bytes
//------------------------------------------------------------------------------------------------------------------------
//    Return int:          0 Alles ok sonst Fehlernummer
//------------------------------------------------------------------------------------------------------------------------
// Testweise ohne monitor
int I2C_B0_ReadRegister_Adr16( char I2C_Slave_ADR, WORD I2C_Register, void* pvDaten, char ByteCounter)
{
   BYTE *bBuffer = (BYTE*) pvDaten;

   if (mbIsBusyFlg & I2C_0_BUSY)
      return 5;

   mbIsBusyFlg |= I2C_0_BUSY;

#ifdef I2C_B0_HS_ADR
   UCB0I2CSA  = I2C_HS_ADR;                           // set HS-Adress
   UCB0CTL1  |= (UCTXSTT);                            // I2C TX, start condition
   while (!(UCB0IFG & UCNACKIFG) );                   // Warte auf NAK
   UCB0IFG &= ~UCNACKIFG;
   SET_UCB0_400kBAUD;
#endif

   int iRetValue=0;

#ifdef I2C_B0_TIME_OUT_FUNC
   unsigned long ulStartTime = I2C_B0_TIME_OUT_FUNC();
#endif

   UCB0I2CSA  = I2C_Slave_ADR;                        // set slave address
   UCB0CTL1  |= (UCTR | UCTXSTT);                     // I2C TX, start condition

   while (!(UCB0IFG & UCTXIFG) )                      // Warte bis Start-Kondition und Slave-Adresse gesendet
   {
#ifdef I2C_B0_TIME_OUT_FUNC
      if ((UCB0IFG & UCNACKIFG) || (I2C_B0_TIME_OUT_FUNC() - ulStartTime) > I2C_B0_TIME_OUT_VALUE)
#else
      if (UCB0IFG & UCNACKIFG)
#endif
      {
         iRetValue = 1;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }
   }

   UCB0TXBUF  = I2C_Register>>8;                      // Registerdaresse senden
#ifdef I2C_B0_TIME_OUT_FUNC
      ulStartTime = I2C_B0_TIME_OUT_FUNC();
#endif

   while (!(UCB0IFG & UCTXIFG) )                      // Warte bis senden abgeschlossen
   {
#ifdef I2C_B0_TIME_OUT_FUNC
      if ((UCB0IFG & UCNACKIFG) || (I2C_B0_TIME_OUT_FUNC() - ulStartTime) > I2C_B0_TIME_OUT_VALUE)
#else
      if (UCB0IFG & UCNACKIFG)
#endif
      {
         iRetValue = 2;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }
   }

   UCB0TXBUF  = I2C_Register;                         // Registerdaresse senden
#ifdef I2C_B0_TIME_OUT_FUNC
      ulStartTime = I2C_B0_TIME_OUT_FUNC();
#endif

   while (!(UCB0IFG & UCTXIFG) )                      // Warte bis senden abgeschlossen
   {
#ifdef I2C_B0_TIME_OUT_FUNC
      if ((UCB0IFG & UCNACKIFG) || (I2C_B0_TIME_OUT_FUNC() - ulStartTime) > I2C_B0_TIME_OUT_VALUE)
#else
      if (UCB0IFG & UCNACKIFG)
#endif
      {
         iRetValue = 3;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }
   }


   UCB0IFG &= ~UCTXIFG;

   UCB0CTL1  &= ~UCTR ;                               // I2C Transmit-Bit löschen
   UCB0CTL1  |=  UCTXSTT ;                            // I2C TX, Restart condition

#ifdef I2C_B0_TIME_OUT_FUNC
      ulStartTime = I2C_B0_TIME_OUT_FUNC();
#endif

   while ( UCB0CTL1  & UCTXSTT )                      // Warte bis senden der I²C-Adresse abgeschlossen
   {
#ifdef I2C_B0_TIME_OUT_FUNC
      if ((UCB0IFG & UCNACKIFG) || (I2C_B0_TIME_OUT_FUNC() - ulStartTime) > I2C_B0_TIME_OUT_VALUE)
#else
      if (UCB0IFG & UCNACKIFG)
#endif
      {
         iRetValue = 4;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }
   }

   while (ByteCounter)
   {
      if (ByteCounter == 1)
         UCB0CTL1 |= (UCTXSTP);                       // Stop-Condition senden wenn letztes Byte empfangen wird
#ifdef I2C_B0_TIME_OUT_FUNC
      ulStartTime = I2C_B0_TIME_OUT_FUNC();
#endif
      while (!(UCB0IFG & UCRXIFG) )                   // Warte bis senden abgeschlossen
      {
#ifdef I2C_B0_TIME_OUT_FUNC
         if ((I2C_B0_TIME_OUT_FUNC() - ulStartTime) > I2C_B0_TIME_OUT_VALUE)
         {
            iRetValue=5;
            I2C_Error( iRetValue );
            goto ErrorExit;
         }
#else
         _NOP();
#endif
      }

      *bBuffer++ = UCB0RXBUF;                         // Zeichen Einlesen und speichern Pointer erhöhen
      ByteCounter--;
   }
   goto NormalExit;

ErrorExit:
   UCB0CTL1 |=  UCTXSTP;                              // send STOP if slave sends NACK
   UCB0IFG  &= ~UCNACKIFG;

NormalExit:
#ifdef I2C_B0_TIME_OUT_FUNC
   ulStartTime = I2C_B0_TIME_OUT_FUNC();
#endif
   while (UCB0CTL1 & UCTXSTP)                         // Warte bis Stop-Condition gesendet
#ifdef I2C_B0_TIME_OUT_FUNC
      if ((I2C_B0_TIME_OUT_FUNC() - ulStartTime) > I2C_B0_TIME_OUT_VALUE)
      {
         iRetValue=5;
         I2C_Error( iRetValue );
         UCB0CTL1 |=  UCSWRST;                        // Clear SW reset, resume operation
         _NOP();
         UCB0CTL1 &= ~UCSWRST;                        // Clear SW reset, resume operation
         break;
      }
#else
      _NOP();
#endif
   #ifdef I2C_B0_HS_ADR
   SET_UCB0_100kBAUD
   #endif
   mbIsBusyFlg &= ~I2C_0_BUSY;
   return iRetValue;

}

//------------------------------------------------------------------------------------------------------------------------
//    I2C_WriteRegister:
//------------------------------------------------------------------------------------------------------------------------
//    Liest ein Register von der angegebenen I²C-Slave-Adresse und der angegeben Register-Adresse
//------------------------------------------------------------------------------------------------------------------------
//    I2C_Slave_ADR:       I²C-Slave-Adresse (Device-Adresse) ohne RD/WR-Bit
//    I2C_Register:        Register-Adresse
//    bBuffer:             Pointer wo die Daten hin sollen
//    ByteCount:           Anzahl der zu lesenden Bytes
//------------------------------------------------------------------------------------------------------------------------
//    Return int:          0 Alles ok sonst Fehlernummer
//------------------------------------------------------------------------------------------------------------------------
// Testweise ohne monitor
int I2C_B0_WriteRegister_Adr16( char I2C_Slave_ADR, WORD I2C_Register, void* pvDaten, char ByteCounter)
{
   if (mbIsBusyFlg & I2C_0_BUSY)
      return 5;

   mbIsBusyFlg |= I2C_0_BUSY;
   BYTE *bBuffer = (BYTE*) pvDaten;

   #ifdef I2C_B0_HS_ADR
   UCB0I2CSA  = I2C_HS_ADR;                           // set HS-Adress
   UCB0CTL1  |= (UCTXSTT);                            // I2C TX, start condition
   while (!(UCB0IFG & UCNACKIFG) );                   // Warte auf NAK
   UCB0IFG &= ~UCNACKIFG;
   SET_UCB0_400kBAUD;
   #endif

   int iRetValue = 0;

   UCB0I2CSA  = I2C_Slave_ADR;                        // set slave address
   UCB0CTL1  |= UCTR | UCTXSTT;                       // I2C TX, start condition

#ifdef I2C_B0_TIME_OUT_FUNC
   unsigned long ulStartTime = I2C_B0_TIME_OUT_FUNC();
#endif
   while (!(UCB0IFG & UCTXIFG) )                     // Warte bis Start-Kondition und Slave-Adresse gesendet
   {
#ifdef I2C_B0_TIME_OUT_FUNC
      if ((UCB0IFG & UCNACKIFG) || (I2C_B0_TIME_OUT_FUNC() - ulStartTime) > I2C_B0_TIME_OUT_VALUE)
#else
      if (UCB0IFG & UCNACKIFG)
#endif
      {
         iRetValue = 1;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }
   }
   UCB0IFG &= ~UCTXIFG;
   UCB0TXBUF = I2C_Register>>8;                      // High-Register-Adresse senden
#ifdef I2C_B0_TIME_OUT_FUNC
      ulStartTime = I2C_B0_TIME_OUT_FUNC();
#endif
   while (!(UCB0IFG & UCTXIFG) )                     // Warte bis High-Register-Adresse gesendet
   {
#ifdef I2C_B0_TIME_OUT_FUNC
      if ((UCB0IFG & UCNACKIFG) || (I2C_B0_TIME_OUT_FUNC() - ulStartTime) > I2C_B0_TIME_OUT_VALUE)
#else
      if (UCB0IFG & UCNACKIFG)
#endif
      {
         iRetValue = 2;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }
   }

   UCB0IFG &= ~UCTXIFG;
   UCB0TXBUF = I2C_Register & 0xFF;                   // Low-Register-Adresse senden

   while (ByteCounter)
   {
#ifdef I2C_B0_TIME_OUT_FUNC
      ulStartTime = I2C_B0_TIME_OUT_FUNC();
#endif
      while (!(UCB0IFG & UCTXIFG) )                   // Warte bis senden abgeschlossen
      {
#ifdef I2C_B0_TIME_OUT_FUNC
      if ((UCB0IFG & UCNACKIFG) || (I2C_B0_TIME_OUT_FUNC() - ulStartTime) > I2C_B0_TIME_OUT_VALUE)
#else
      if (UCB0IFG & UCNACKIFG)
#endif
         {
            iRetValue = 3;
            I2C_Error( iRetValue );
            goto ErrorExit;
         }
      }

      UCB0IFG &= ~UCTXIFG;

      UCB0TXBUF = *bBuffer++;                         // Zeichen senden und Pointer erhöhen
      ByteCounter--;

   }

#ifdef I2C_B0_TIME_OUT_FUNC
   ulStartTime = I2C_B0_TIME_OUT_FUNC();
#endif
   while (!(UCB0IFG & UCTXIFG) )                      // Warte bis senden abgeschlossen
   {
#ifdef I2C_B0_TIME_OUT_FUNC
      if ((UCB0IFG & UCNACKIFG) || (I2C_B0_TIME_OUT_FUNC() - ulStartTime) > I2C_B0_TIME_OUT_VALUE)
#else
      if (UCB0IFG & UCNACKIFG)
#endif
      {
         iRetValue = 4;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }
   }

   // Hier geht es ohne Fehler weiter - also nicht durch ErrorExit-Label iritieren lassen

ErrorExit:
   UCB0CTL1 |=  UCTXSTP;                        // send STOP if slave sends NACK
   UCB0IFG  &= ~UCNACKIFG;
#ifdef I2C_B0_TIME_OUT_FUNC
   ulStartTime = I2C_B0_TIME_OUT_FUNC();
#endif
   while (UCB0CTL1 & UCTXSTP)                         // Warte bis Stop-Condition gesendet
#ifdef I2C_B0_TIME_OUT_FUNC
      if ((I2C_B0_TIME_OUT_FUNC() - ulStartTime) > I2C_B0_TIME_OUT_VALUE)
      {
         iRetValue=5;
         I2C_Error( iRetValue );
         UCB0CTL1 |=  UCSWRST;                        // Clear SW reset, resume operation
         _NOP();
         UCB0CTL1 &= ~UCSWRST;                        // Clear SW reset, resume operation
         break;
      }
#else
      _NOP();
#endif
   #ifdef I2C_B0_HS_ADR
   SET_UCB0_100kBAUD
   #endif
   mbIsBusyFlg &= ~I2C_0_BUSY;
   return iRetValue;
}

#endif                                                // I2C_USE_UCSI_B0

#ifdef I2C_USE_USCI_B1

//------------------------------------------------------------------------------------------------------------------------
//    I2C_Init:
//------------------------------------------------------------------------------------------------------------------------
//    Initialisiert die UCSI-Schnittstelle und verwendet die angegebene Bausrate (100000 / 400000)
//------------------------------------------------------------------------------------------------------------------------
//    BaudRate:      (100000 / 400000)
//------------------------------------------------------------------------------------------------------------------------
void I2C_B1_Init(long BaudRate)
{
   UCB1CTL1 = UCSWRST;                                // Enable SW reset
   UCB1CTL0 = UCMST | UCMODE_3 | UCSYNC;              // I2C Master, synchronous mode
   UCB1CTL1 = I2C_B1_CLK_SOURCE | UCSWRST;             // Use SMCLK, keep SW reset
   UCB1BRW  = I2C_B1_CLK_FREQ / BaudRate;              // set prescaler
   UCB1I2CSA = I2C_B1_SLAVE_ADR;                       // set slave address
   UCB1CTL1 &= ~UCSWRST;                              // Clear SW reset, resume operation
   mbIsBusyFlg &= ~I2C_1_BUSY;
}

//------------------------------------------------------------------------------------------------------------------------
//    I2C_ReadRegister:
//------------------------------------------------------------------------------------------------------------------------
//    Liest ein Register von der angegebenen I²C-Slave-Adresse und der angegeben Register-Adresse
//------------------------------------------------------------------------------------------------------------------------
//    I2C_Slave_ADR:       I²C-Slave-Adresse (Device-Adresse) ohne RD/WR-Bit
//    I2C_Register:        Register-Adresse
//    bBuffer:             Pointer wo die Daten hin sollen
//    ByteCount:           Anzahl der zu lesenden Bytes
//------------------------------------------------------------------------------------------------------------------------
//    Return int:          0 Alles ok sonst Fehlernummer
//------------------------------------------------------------------------------------------------------------------------
// Testweise ohne monitor
int I2C_B1_ReadRegister( char I2C_Slave_ADR, char I2C_Register, void* pvDaten, char ByteCounter)
{
   if (mbIsBusyFlg & I2C_1_BUSY)
      return 4;

   BYTE *bBuffer = (BYTE*) pvDaten;

   mbIsBusyFlg |= I2C_1_BUSY;

#ifdef I2C_B1_HS_ADR
   UCB1I2CSA  = I2C_HS_ADR;                           // set HS-Adress
   UCB1CTL1  |= (UCTXSTT);                            // I2C TX, start condition
   while (!(UCB1IFG & UCNACKIFG) );                   // Warte auf NAK
   UCB1STAT &= ~UCNACKIFG;
   SET_UCB1_400kBAUD;                                 // set prescaler
#endif

   int iRetValue=0;

   UCB1I2CSA  = I2C_Slave_ADR;                        // set slave address
   UCB1CTL1  |= (UCTR | UCTXSTT);                     // I2C TX, start condition

#ifdef I2C_B1_TIME_OUT_FUNC
    unsigned long  ulStartTime = I2C_B1_TIME_OUT_FUNC();
#endif
   while (!(UCB1IFG & UCTXIFG) )                       // Warte bis Start-Kondition und Slave-Adresse gesendet
   {
#ifdef I2C_B1_TIME_OUT_FUNC
      if ((UCB1IFG & UCNACKIFG) || (I2C_B1_TIME_OUT_FUNC() - ulStartTime) > I2C_B1_TIME_OUT_VALUE)
#else
      if (UCB1IFG & UCNACKIFG)
#endif
      {
         iRetValue = 1;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }
   }
   UCB1IFG &= ~UCTXIFG;
   UCB1TXBUF  = I2C_Register;                         // Registerdaresse senden
#ifdef I2C_B1_TIME_OUT_FUNC
    ulStartTime = I2C_B1_TIME_OUT_FUNC();
#endif
   while (!(UCB1IFG & UCTXIFG) )                      // Warte bis senden abgeschlossen
   {
#ifdef I2C_B1_TIME_OUT_FUNC
      if ((UCB1IFG & UCNACKIFG) || (I2C_B1_TIME_OUT_FUNC() - ulStartTime) > I2C_B1_TIME_OUT_VALUE)
#else
      if (UCB1IFG & UCNACKIFG)
#endif
      {
         iRetValue = 2;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }
   }

   UCB1IFG &= ~UCTXIFG;

   UCB1CTL1  &= ~UCTR ;                               // I2C Transmit-Bit löschen
   UCB1CTL1  |=  UCTXSTT ;                            // I2C TX, Restart condition
#ifdef I2C_B1_TIME_OUT_FUNC
    ulStartTime = I2C_B1_TIME_OUT_FUNC();
#endif
   while ( UCB1CTL1  & UCTXSTT )                      // Warte bis senden der I²C-Adresse abgeschlossen
   {
#ifdef I2C_B1_TIME_OUT_FUNC
      if ((UCB1IFG & UCNACKIFG) || (I2C_B1_TIME_OUT_FUNC() - ulStartTime) > I2C_B1_TIME_OUT_VALUE)
#else
      if (UCB1IFG & UCNACKIFG)
#endif
      {
         iRetValue = 3;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }
   }

   while (ByteCounter)
   {
      if (ByteCounter == 1)
         UCB1CTL1 |= (UCTXSTP);                       // Stop-Condition senden wenn letztes Byte empfangen wird
#ifdef I2C_B1_TIME_OUT_FUNC
      ulStartTime = I2C_B1_TIME_OUT_FUNC();
#endif
      while (!(UCB1IFG & UCRXIFG) )                   // Warte bis senden abgeschlossen
      {
#ifdef I2C_B1_TIME_OUT_FUNC
         if ((I2C_B1_TIME_OUT_FUNC() - ulStartTime) > I2C_B1_TIME_OUT_VALUE)
         {
            iRetValue=4;
            I2C_Error( iRetValue );
            goto ErrorExit;
         }
#else
         _NOP();
#endif
      }

      *bBuffer++ = UCB1RXBUF;                         // Zeichen Einlesen und speichern Pointer erhöhen
      ByteCounter--;
   }

   goto NormalExit;

ErrorExit:
   UCB1CTL1 |=  UCTXSTP;                              // send STOP if slave sends NACK
   UCB1IFG  &= ~UCNACKIFG;

NormalExit:
#ifdef I2C_B1_TIME_OUT_FUNC
   ulStartTime = I2C_B1_TIME_OUT_FUNC();
#endif
   while (UCB0CTL1 & UCTXSTP)                         // Warte bis Stop-Condition gesendet
#ifdef I2C_B1_TIME_OUT_FUNC
      if ((I2C_B1_TIME_OUT_FUNC() - ulStartTime) > I2C_B1_TIME_OUT_VALUE)
      {
         iRetValue=5;
         I2C_Error( iRetValue );
         UCB1CTL1 |=  UCSWRST;                        // Clear SW reset, resume operation
         _NOP();
         UCB1CTL1 &= ~UCSWRST;                        // Clear SW reset, resume operation
         break;
      }
#else
      _NOP();
#endif
   #ifdef I2C_B1_HS_ADR
   SET_UCB1_100kBAUD
   #endif
   mbIsBusyFlg &= ~I2C_1_BUSY;
   return iRetValue;
}

//------------------------------------------------------------------------------------------------------------------------
//    I2C_WriteRegister:
//------------------------------------------------------------------------------------------------------------------------
//    Liest ein Register von der angegebenen I²C-Slave-Adresse und der angegeben Register-Adresse
//------------------------------------------------------------------------------------------------------------------------
//    I2C_Slave_ADR:       I²C-Slave-Adresse (Device-Adresse) ohne RD/WR-Bit
//    I2C_Register:        Register-Adresse
//    bBuffer:             Pointer wo die Daten hin sollen
//    ByteCount:           Anzahl der zu lesenden Bytes
//------------------------------------------------------------------------------------------------------------------------
//    Return int:          0 Alles ok sonst Fehlernummer
//------------------------------------------------------------------------------------------------------------------------
// Testweise ohne monitor
int I2C_B1_WriteRegister( char I2C_Slave_ADR, char I2C_Register, void* pvDaten, char ByteCounter)
{
   BYTE *bBuffer = (BYTE*) pvDaten;
   if (mbIsBusyFlg & I2C_1_BUSY)
     return 4;

   #ifdef I2C_B1_HS_ADR
   UCB1I2CSA  = I2C_HS_ADR;                           // set HS-Adress
   UCB1CTL1  |= (UCTXSTT);                            // I2C TX, start condition
   while (!(UCB1IFG & UCNACKIFG) );                   // Warte auf NAK
   UCB1STAT &= ~UCNACKIFG;
   SET_UCB1_400kBAUD;                                 // set prescaler
   #endif

   int iRetValue=0;

   mbIsBusyFlg |= I2C_1_BUSY;

   UCB1I2CSA  = I2C_Slave_ADR;                        // set slave address
   UCB1CTL1  |= UCTR | UCTXSTT;                       // I2C TX, start condition

#ifdef I2C_B1_TIME_OUT_FUNC
    unsigned long  ulStartTime = I2C_B1_TIME_OUT_FUNC();
#endif
   while (!(UCB1IFG & UCTXIFG) )                      // Warte bis Start-Kondition und Slave-Adresse gesendet
   {
#ifdef I2C_B1_TIME_OUT_FUNC
      if ((UCB1IFG & UCNACKIFG) || (I2C_B1_TIME_OUT_FUNC() - ulStartTime) > I2C_B1_TIME_OUT_VALUE)
#else
      if (UCB1IFG & UCNACKIFG)
#endif
      {
         iRetValue = 1;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }
   }
   UCB1IFG &= ~UCTXIFG;

   UCB1TXBUF = I2C_Register;                          // Registerdaresse senden

#ifdef I2C_B1_TIME_OUT_FUNC
    ulStartTime = I2C_B1_TIME_OUT_FUNC();
#endif
   while (ByteCounter)
   {
      while (!(UCB1IFG & UCTXIFG) )                   // Warte bis senden abgeschlossen
      {
#ifdef I2C_B1_TIME_OUT_FUNC
         if ((UCB1IFG & UCNACKIFG) || (I2C_B1_TIME_OUT_FUNC() - ulStartTime) > I2C_B1_TIME_OUT_VALUE)
#else
         if (UCB1IFG & UCNACKIFG)
#endif
         {
            iRetValue = 2;
            I2C_Error( iRetValue );
            goto ErrorExit;
         }
      }

      UCB1IFG &= ~UCTXIFG;

      UCB1TXBUF = *bBuffer++;                         // Zeichen senden und Pointer erhöhen
      ByteCounter--;

   }

#ifdef I2C_B1_TIME_OUT_FUNC
    ulStartTime = I2C_B1_TIME_OUT_FUNC();
#endif
   while (!(UCB1IFG & UCTXIFG) )                      // Warte bis senden abgeschlossen
   {
#ifdef I2C_B1_TIME_OUT_FUNC
      if ((UCB1IFG & UCNACKIFG) || (I2C_B1_TIME_OUT_FUNC() - ulStartTime) > I2C_B1_TIME_OUT_VALUE)
#else
      if (UCB1IFG & UCNACKIFG)
#endif
      {
         iRetValue = 3;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }
   }
   // Hier geht es ohne Fehler weiter - also nicht durch ErrorExit-Label iritieren lassen

ErrorExit:
   UCB1CTL1 |=  UCTXSTP;                              // send STOP
   UCB1IFG  &= ~UCNACKIFG;
#ifdef I2C_B1_TIME_OUT_FUNC
   ulStartTime = I2C_B1_TIME_OUT_FUNC();
#endif
   while (UCB0CTL1 & UCTXSTP)                         // Warte bis Stop-Condition gesendet
#ifdef I2C_B1_TIME_OUT_FUNC
      if ((I2C_B1_TIME_OUT_FUNC() - ulStartTime) > I2C_B1_TIME_OUT_VALUE)
      {
         iRetValue=5;
         I2C_Error( iRetValue );
         UCB1CTL1 |=  UCSWRST;                        // Clear SW reset, resume operation
         _NOP();
         UCB1CTL1 &= ~UCSWRST;                        // Clear SW reset, resume operation
         break;
      }
#else
      _NOP();
#endif
#ifdef I2C_B1_HS_ADR
   SET_UCB1_100kBAUD;                                 // set prescaler
#endif
   mbIsBusyFlg &= ~I2C_1_BUSY;
   return iRetValue;
}

//------------------------------------------------------------------------------------------------------------------------
//    I2C_ReadRegister:
//------------------------------------------------------------------------------------------------------------------------
//    Liest ein Register von der angegebenen I²C-Slave-Adresse und der angegeben Register-Adresse
//------------------------------------------------------------------------------------------------------------------------
//    I2C_Slave_ADR:       I²C-Slave-Adresse (Device-Adresse) ohne RD/WR-Bit
//    I2C_Register:        Register-Adresse
//    bBuffer:             Pointer wo die Daten hin sollen
//    ByteCount:           Anzahl der zu lesenden Bytes
//------------------------------------------------------------------------------------------------------------------------
//    Return int:          0 Alles ok sonst Fehlernummer
//------------------------------------------------------------------------------------------------------------------------
//__monitor int I2C_B1_ReadRegister_Adr16( char I2C_Slave_ADR, WORD I2C_Register, void* pvDaten, char ByteCounter)
// Testweise ohne monitor
int I2C_B1_ReadRegister_Adr16( char I2C_Slave_ADR, WORD I2C_Register, void* pvDaten, char ByteCounter)
{
   if (mbIsBusyFlg & I2C_1_BUSY)
      return 5;

   BYTE *bBuffer = (BYTE*) pvDaten;

   mbIsBusyFlg |= I2C_1_BUSY;

   #ifdef I2C_B1_HS_ADR
   UCB1I2CSA  = I2C_HS_ADR;                           // set HS-Adress
   UCB1CTL1  |= (UCTXSTT);                            // I2C TX, start condition
   while (!(UCB1IFG & UCNACKIFG) );                   // Warte auf NAK
   UCB1STAT &= ~UCNACKIFG;
   SET_UCB1_400kBAUD;                                 // set prescaler
   #endif

   int iRetValue=0;

   UCB1I2CSA  = I2C_Slave_ADR;                        // set slave address
   UCB1CTL1  |= (UCTR | UCTXSTT);                     // I2C TX, start condition

#ifdef I2C_B1_TIME_OUT_FUNC
    unsigned long  ulStartTime = I2C_B1_TIME_OUT_FUNC();
#endif
   while (!(UCB1IFG & UCTXIFG) )                       // Warte bis Start-Kondition und Slave-Adresse gesendet
   {
#ifdef I2C_B1_TIME_OUT_FUNC
      if ((UCB1IFG & UCNACKIFG) || (I2C_B1_TIME_OUT_FUNC() - ulStartTime) > I2C_B1_TIME_OUT_VALUE)
#else
      if (UCB1IFG & UCNACKIFG)
#endif
      {
         iRetValue = 1;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }
   }
   UCB1IFG &= ~UCTXIFG;

   UCB1TXBUF = I2C_Register>>8;                       // Registerdaresse senden

#ifdef I2C_B1_TIME_OUT_FUNC
    ulStartTime = I2C_B1_TIME_OUT_FUNC();
#endif
   while (!(UCB1IFG & UCTXIFG) )                      // Warte bis senden abgeschlossen
   {
#ifdef I2C_B1_TIME_OUT_FUNC
      if ((UCB1IFG & UCNACKIFG) || (I2C_B1_TIME_OUT_FUNC() - ulStartTime) > I2C_B1_TIME_OUT_VALUE)
#else
      if (UCB1IFG & UCNACKIFG)
#endif
      {
         iRetValue = 2;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }
   }
   UCB1IFG &= ~UCTXIFG;

   UCB1TXBUF = I2C_Register & 0xFF;                  // Registerdaresse senden
   while (!(UCB1IFG & UCTXIFG) )                      // Warte bis senden abgeschlossen
   {
#ifdef I2C_B1_TIME_OUT_FUNC
      if ((UCB1IFG & UCNACKIFG) || (I2C_B1_TIME_OUT_FUNC() - ulStartTime) > I2C_B1_TIME_OUT_VALUE)
#else
      if (UCB1IFG & UCNACKIFG)
#endif
      {
         iRetValue = 3;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }
   }

   UCB1IFG &= ~UCTXIFG;

   UCB1CTL1  &= ~UCTR ;                               // I2C Transmit-Bit löschen
   UCB1CTL1  |=  UCTXSTT ;                            // I2C TX, Restart condition

#ifdef I2C_B1_TIME_OUT_FUNC
    ulStartTime = I2C_B1_TIME_OUT_FUNC();
#endif
   while ( UCB1CTL1  & UCTXSTT )                      // Warte bis senden der I²C-Adresse abgeschlossen
   {
#ifdef I2C_B1_TIME_OUT_FUNC
      if ((UCB1IFG & UCNACKIFG) || (I2C_B1_TIME_OUT_FUNC() - ulStartTime) > I2C_B1_TIME_OUT_VALUE)
#else
      if (UCB1IFG & UCNACKIFG)
#endif
      {
         iRetValue = 4;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }
   }

   while (ByteCounter)
   {
      if (ByteCounter == 1)
         UCB1CTL1 |= (UCTXSTP);                       // Stop-Condition senden wenn letztes Byte empfangen wird

#ifdef I2C_B1_TIME_OUT_FUNC
      ulStartTime = I2C_B1_TIME_OUT_FUNC();
#endif
      while (!(UCB1IFG & UCRXIFG) )                   // Warte bis senden abgeschlossen
      {
#ifdef I2C_B1_TIME_OUT_FUNC
         if ((I2C_B1_TIME_OUT_FUNC() - ulStartTime) > I2C_B1_TIME_OUT_VALUE)
         {
            iRetValue=5;
            I2C_Error( iRetValue );
            goto ErrorExit;
         }
#else
         _NOP();
#endif
      }
      *bBuffer++ = UCB1RXBUF;                         // Zeichen Einlesen und speichern Pointer erhöhen
      ByteCounter--;
   }
   goto NormalExit;

ErrorExit:
   UCB1CTL1 |=  UCTXSTP;                        // send STOP if slave sends NACK
   UCB1IFG  &= ~UCNACKIFG;
NormalExit:
#ifdef I2C_B1_TIME_OUT_FUNC
   ulStartTime = I2C_B1_TIME_OUT_FUNC();
#endif
   while (UCB0CTL1 & UCTXSTP)                         // Warte bis Stop-Condition gesendet
#ifdef I2C_B1_TIME_OUT_FUNC
      if ((I2C_B1_TIME_OUT_FUNC() - ulStartTime) > I2C_B1_TIME_OUT_VALUE)
      {
         iRetValue=5;
         I2C_Error( iRetValue );
         UCB1CTL1 |=  UCSWRST;                        // Clear SW reset, resume operation
         _NOP();
         UCB1CTL1 &= ~UCSWRST;                        // Clear SW reset, resume operation
         break;
      }
#else
      _NOP();
#endif
#ifdef I2C_B1_HS_ADR
   SET_UCB1_100kBAUD
#endif
   mbIsBusyFlg &= ~I2C_1_BUSY;
   return iRetValue;
}

//------------------------------------------------------------------------------------------------------------------------
//    I2C_WriteRegister:
//------------------------------------------------------------------------------------------------------------------------
//    Liest ein Register von der angegebenen I²C-Slave-Adresse und der angegeben Register-Adresse
//------------------------------------------------------------------------------------------------------------------------
//    I2C_Slave_ADR:       I²C-Slave-Adresse (Device-Adresse) ohne RD/WR-Bit
//    I2C_Register:        Register-Adresse
//    bBuffer:             Pointer wo die Daten hin sollen
//    ByteCount:           Anzahl der zu lesenden Bytes
//------------------------------------------------------------------------------------------------------------------------
//    Return int:          0 Alles ok sonst Fehlernummer
//------------------------------------------------------------------------------------------------------------------------
// Testweise ohne monitor
int I2C_B1_WriteRegister_Adr16( char I2C_Slave_ADR, WORD I2C_Register, void* pvDaten, char ByteCounter)
{
   BYTE *bBuffer = (BYTE*) pvDaten;
   if (mbIsBusyFlg & I2C_1_BUSY)
      return 4;

   mbIsBusyFlg |= I2C_1_BUSY;

   UCB1I2CSA  = I2C_Slave_ADR;                        // set slave address
   UCB1CTL1  |= UCTR | UCTXSTT;                       // I2C TX, start condition

   int iRetValue = 0;

   #ifdef I2C_B1_HS_ADR
   UCB1I2CSA  = I2C_HS_ADR;                           // set HS-Adress
   UCB1CTL1  |= (UCTXSTT);                            // I2C TX, start condition
   while (!(UCB1IFG & UCNACKIFG) );                   // Warte auf NAK
   UCB1STAT &= ~UCNACKIFG;
   SET_UCB1_400kBAUD;                                 // set prescaler
   #endif

#ifdef I2C_B1_TIME_OUT_FUNC
    unsigned long  ulStartTime = I2C_B1_TIME_OUT_FUNC();
#endif
   while (!(UCB1IFG & UCTXIFG) )                      // Warte bis Start-Kondition und Slave-Adresse gesendet
   {
#ifdef I2C_B1_TIME_OUT_FUNC
      if ((UCB1IFG & UCNACKIFG) || (I2C_B1_TIME_OUT_FUNC() - ulStartTime) > I2C_B1_TIME_OUT_VALUE)
#else
      if (UCB1IFG & UCNACKIFG)
#endif
      {
         iRetValue = 1;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }
   }
   UCB1IFG &= ~UCTXIFG;

   UCB1TXBUF = I2C_Register>>8;                       // Registerdaresse senden
#ifdef I2C_B1_TIME_OUT_FUNC
    ulStartTime = I2C_B1_TIME_OUT_FUNC();
#endif
   while (!(UCB1IFG & UCTXIFG) )                      // Warte bis senden abgeschlossen
   {
#ifdef I2C_B1_TIME_OUT_FUNC
      if ((UCB1IFG & UCNACKIFG) || (I2C_B1_TIME_OUT_FUNC() - ulStartTime) > I2C_B1_TIME_OUT_VALUE)
#else
      if (UCB1IFG & UCNACKIFG)
#endif
      {
         iRetValue = 2;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }
   }
   UCB1IFG &= ~UCTXIFG;

   UCB1TXBUF = I2C_Register & 0xFF;                   // Registerdaresse senden

   while (ByteCounter)
   {
#ifdef I2C_B1_TIME_OUT_FUNC
      ulStartTime = I2C_B1_TIME_OUT_FUNC();
#endif
      while (!(UCB1IFG & UCTXIFG) )                   // Warte bis senden abgeschlossen
      {
#ifdef I2C_B1_TIME_OUT_FUNC
         if ((UCB1IFG & UCNACKIFG) || (I2C_B1_TIME_OUT_FUNC() - ulStartTime) > I2C_B1_TIME_OUT_VALUE)
#else
         if (UCB1IFG & UCNACKIFG)
#endif
         {
            iRetValue = 3;
            I2C_Error( iRetValue );
            goto ErrorExit;
         }
      }

      UCB1IFG &= ~UCTXIFG;

      UCB1TXBUF = *bBuffer++;                         // Zeichen senden und Pointer erhöhen
      ByteCounter--;

   }

#ifdef I2C_B1_TIME_OUT_FUNC
    ulStartTime = I2C_B1_TIME_OUT_FUNC();
#endif
   while (!(UCB1IFG & UCTXIFG) )                      // Warte bis senden abgeschlossen
   {
#ifdef I2C_B1_TIME_OUT_FUNC
      if ((UCB1IFG & UCNACKIFG) || (I2C_B1_TIME_OUT_FUNC() - ulStartTime) > I2C_B1_TIME_OUT_VALUE)
#else
      if (UCB1IFG & UCNACKIFG)
#endif
      {
         iRetValue = 4;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }
   }
   // Hier geht es ohne Fehler weiter - also nicht durch ErrorExit-Label iritieren lassen

ErrorExit:
   UCB1CTL1 |=  UCTXSTP;                              // send STOP if slave sends NACK
   UCB1IFG  &= ~UCNACKIFG;

#ifdef I2C_B1_TIME_OUT_FUNC
   ulStartTime = I2C_B1_TIME_OUT_FUNC();
#endif
   while (UCB1CTL1 & UCTXSTP)                         // Warte bis Stop-Condition gesendet
#ifdef I2C_B1_TIME_OUT_FUNC
      if ((I2C_B1_TIME_OUT_FUNC() - ulStartTime) > 3)
      {
         iRetValue=5;
         I2C_Error( iRetValue );
         UCB1CTL1 |=  UCSWRST;                        // Clear SW reset, resume operation
         _NOP();
         UCB1CTL1 &= ~UCSWRST;                        // Clear SW reset, resume operation
         break;
      }
#else
      _NOP();
#endif

#ifdef I2C_B1_HS_ADR
   SET_UCB1_100kBAUD
#endif
   mbIsBusyFlg &= ~I2C_1_BUSY;
   return iRetValue;
}

#endif                                                // I2C_USE_UCSI_B1

#ifdef I2C_USE_USCI_B2

#ifdef USE_MSP430F6658
//------------------------------------------------------------------------------------------------------------------------
//    I2C_Init:
//------------------------------------------------------------------------------------------------------------------------
//    Initialisiert die UCSI-Schnittstelle und verwendet die angegebene Bausrate (100000 / 400000)
//------------------------------------------------------------------------------------------------------------------------
//    BaudRate:      (100000 / 400000)
//------------------------------------------------------------------------------------------------------------------------
void I2C_B2_Init(long BaudRate)
{
   UCB2CTL1 = UCSWRST;                                // Enable SW reset
   UCB2CTL0 = UCMST | UCMODE_3 | UCSYNC;              // I2C Master, synchronous mode
   UCB2CTL1 = I2C_B2_CLK_SOURCE | UCSWRST;            // Use SMCLK, keep SW reset
   UCB2BRW  = I2C_B2_CLK_FREQ / BaudRate;             // set prescaler
   UCB2I2CSA = I2C_B2_SLAVE_ADR;                      // set slave address
   UCB2CTL1 &= ~UCSWRST;                              // Clear SW reset, resume operation
   mbIsBusyFlg &= ~I2C_2_BUSY;
}

//------------------------------------------------------------------------------------------------------------------------
//    I2C_ReadBytes:
//------------------------------------------------------------------------------------------------------------------------
//    Liest Daten vom I2C-Bus ein.
//------------------------------------------------------------------------------------------------------------------------
//    I2C_Slave_ADR:       I²C-Slave-Adresse (Device-Adresse) ohne RD/WR-Bit
//    bBuffer:             Pointer wo die Daten hin sollen
//    ByteCount:           Anzahl der zu lesenden Bytes
//------------------------------------------------------------------------------------------------------------------------
//    Return int:          0 Alles ok sonst Fehlernummer
//------------------------------------------------------------------------------------------------------------------------
// Testweise ohne monitor
int I2C_B2_ReadBytes( char I2C_Slave_ADR, void* pvDaten, char ByteCounter)
{
   if (mbIsBusyFlg & I2C_2_BUSY)
      return 4;

   BYTE *bBuffer = (BYTE*) pvDaten;
   mbIsBusyFlg |= I2C_2_BUSY;

   int iRetValue=0;

   UCB2I2CSA  = I2C_Slave_ADR;                        // set slave address
   UCB2CTL1  &= ~UCTR ;                               // I2C Transmit-Bit löschen
   UCB2CTL1  |=  UCTXSTT ;                            // I2C TX, Restart condition

#ifdef I2C_B2_TIME_OUT_FUNC
    unsigned long  ulStartTime = I2C_B2_TIME_OUT_FUNC();
#endif
   while ( UCB2CTL1  & UCTXSTT )                      // Warte bis senden der I²C-Adresse abgeschlossen
   {
#ifdef I2C_B2_TIME_OUT_FUNC
      if ((UCB2IFG & UCNACKIFG) || (I2C_B2_TIME_OUT_FUNC() - ulStartTime) > I2C_B2_TIME_OUT_VALUE)
#else
      if (UCB2IFG & UCNACKIFG)
#endif
      {
         iRetValue = 1;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }
   }

   while (ByteCounter)
   {
      if (ByteCounter == 1)
         UCB2CTL1 |= (UCTXSTP);                       // Stop-Condition senden wenn letztes Byte empfangen wird

      while (!(UCB2IFG & UCRXIFG) )                   // Warte bis senden abgeschlossen
#ifdef I2C_B2_TIME_OUT_FUNC
         if ((I2C_B2_TIME_OUT_FUNC() - ulStartTime) > I2C_B2_TIME_OUT_VALUE)
         {
            iRetValue=2;
            I2C_Error( iRetValue );
            goto ErrorExit;
         }
#else
         _NOP();
#endif

      *bBuffer++ = UCB2RXBUF;                         // Zeichen Einlesen und speichern Pointer erhöhen
      ByteCounter--;
   }
   goto NormalExit;

ErrorExit:
   UCB2CTL1 |=  UCTXSTP;                              // send STOP if slave sends NACK
   UCB2IFG  &= ~UCNACKIFG;
NormalExit:
#ifdef I2C_B2_TIME_OUT_FUNC
   ulStartTime = I2C_B2_TIME_OUT_FUNC();
#endif
   while (UCB2CTL1 & UCTXSTP)                         // Warte bis Stop-Condition gesendet
#ifdef I2C_B2_TIME_OUT_FUNC
      if ((I2C_B2_TIME_OUT_FUNC() - ulStartTime) > I2C_B2_TIME_OUT_VALUE)
      {
         iRetValue=3;
         I2C_Error( iRetValue );
         UCB2CTL1 |=  UCSWRST;                        // Clear SW reset, resume operation
         _NOP();
         UCB2CTL1 &= ~UCSWRST;                        // Clear SW reset, resume operation
         break;
      }
#else
      _NOP();
#endif

#ifdef I2C_B2_HS_ADR
   SET_UCB2_100kBAUD
#endif
   mbIsBusyFlg &= ~I2C_2_BUSY;
   return iRetValue;
}

//------------------------------------------------------------------------------------------------------------------------
//    I2C_ReadRegister:
//------------------------------------------------------------------------------------------------------------------------
//    Liest ein Register von der angegebenen I²C-Slave-Adresse und der angegeben Register-Adresse
//------------------------------------------------------------------------------------------------------------------------
//    I2C_Slave_ADR:       I²C-Slave-Adresse (Device-Adresse) ohne RD/WR-Bit
//    I2C_Register:        Register-Adresse
//    bBuffer:             Pointer wo die Daten hin sollen
//    ByteCount:           Anzahl der zu lesenden Bytes
//------------------------------------------------------------------------------------------------------------------------
//    Return int:          0 Alles ok sonst Fehlernummer
//------------------------------------------------------------------------------------------------------------------------
// Testweise ohne monitor
int I2C_B2_ReadRegister( char I2C_Slave_ADR, char I2C_Register, void* pvDaten, char ByteCounter)
{
   if (mbIsBusyFlg & I2C_2_BUSY)
      return 4;

   BYTE *bBuffer = (BYTE*) pvDaten;

   mbIsBusyFlg |= I2C_2_BUSY;

#ifdef I2C_B2_HS_ADR
   UCB2I2CSA  = I2C_HS_ADR;                           // set HS-Adress
   UCB2CTL1  |= (UCTXSTT);                            // I2C TX, start condition
   while (!(UCB2IFG & UCNACKIFG) );                   // Warte auf NAK
   UCB2STAT &= ~UCNACKIFG;
   SET_UCB2_400kBAUD;                                 // set prescaler
#endif

#ifdef I2C_B2_TIME_OUT_FUNC
   unsigned long  ulStartTime = I2C_B2_TIME_OUT_FUNC();
#endif

   int iRetValue=0;

   UCB2I2CSA  = I2C_Slave_ADR;                        // set slave address

   while (!(UCB2IFG & UCTXIFG) )                       // Warte bis Start-Kondition und Slave-Adresse gesendet
   {
#ifdef I2C_B2_TIME_OUT_FUNC
      if ((UCB2IFG & UCNACKIFG) || (I2C_B2_TIME_OUT_FUNC() - ulStartTime) > I2C_B2_TIME_OUT_VALUE)
#else
      if (UCB2IFG & UCNACKIFG)
#endif
      {
         iRetValue = 1;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }
   }
   UCB2IFG &= ~UCTXIFG;
   UCB2TXBUF  = I2C_Register;                         // Registerdaresse senden
#ifdef I2C_B2_TIME_OUT_FUNC
   ulStartTime = I2C_B2_TIME_OUT_FUNC();
#endif
   while (!(UCB2IFG & UCTXIFG) )                      // Warte bis senden abgeschlossen
   {
#ifdef I2C_B2_TIME_OUT_FUNC
      if ((UCB2IFG & UCNACKIFG) || (I2C_B2_TIME_OUT_FUNC() - ulStartTime) > I2C_B2_TIME_OUT_VALUE)
#else
      if (UCB2IFG & UCNACKIFG)
#endif
      {
         iRetValue = 2;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }
   }

   UCB2IFG &= ~UCTXIFG;

   UCB2CTL1  &= ~UCTR ;                               // I2C Transmit-Bit löschen
   UCB2CTL1  |=  UCTXSTT ;                            // I2C TX, Restart condition

#ifdef I2C_B2_TIME_OUT_FUNC
   ulStartTime = I2C_B2_TIME_OUT_FUNC();
#endif
   while ( UCB2CTL1  & UCTXSTT )                      // Warte bis senden der I²C-Adresse abgeschlossen
   {
#ifdef I2C_B2_TIME_OUT_FUNC
      if ((UCB2IFG & UCNACKIFG) || (I2C_B2_TIME_OUT_FUNC() - ulStartTime) > I2C_B2_TIME_OUT_VALUE)
#else
      if (UCB2IFG & UCNACKIFG)
#endif
      {
         iRetValue = 3;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }
   }

   while (ByteCounter)
   {
      if (ByteCounter == 1)
         UCB2CTL1 |= (UCTXSTP);                       // Stop-Condition senden wenn letztes Byte empfangen wird

#ifdef I2C_B2_TIME_OUT_FUNC
      ulStartTime = I2C_B2_TIME_OUT_FUNC();
#endif
      while (!(UCB2IFG & UCRXIFG) )                   // Warte bis senden abgeschlossen
#ifdef I2C_B2_TIME_OUT_FUNC
         if ((I2C_B2_TIME_OUT_FUNC() - ulStartTime) > I2C_B2_TIME_OUT_VALUE)
         {
            iRetValue=4;
            I2C_Error( iRetValue );
            goto ErrorExit;
         }
#else
         _NOP();
#endif

      *bBuffer++ = UCB2RXBUF;                         // Zeichen Einlesen und speichern Pointer erhöhen
      ByteCounter--;
   }

   goto NormalExit;

ErrorExit:
   UCB2CTL1 |=  UCTXSTP;                              // send STOP if slave sends NACK
   UCB2IFG  &= ~UCNACKIFG;

NormalExit:
#ifdef I2C_B2_TIME_OUT_FUNC
   ulStartTime = I2C_B2_TIME_OUT_FUNC();
#endif
   while (UCB2CTL1 & UCTXSTP)                         // Warte bis Stop-Condition gesendet
#ifdef I2C_B2_TIME_OUT_FUNC
      if ((I2C_B2_TIME_OUT_FUNC() - ulStartTime) > I2C_B2_TIME_OUT_VALUE)
      {
         iRetValue=5;
         I2C_Error( iRetValue );
         UCB2CTL1 |=  UCSWRST;                        // Clear SW reset, resume operation
         _NOP();
         UCB2CTL1 &= ~UCSWRST;                        // Clear SW reset, resume operation
         break;
      }
#else
      _NOP();
#endif

#ifdef I2C_B2_HS_ADR
   SET_UCB2_100kBAUD
#endif
   mbIsBusyFlg &= ~I2C_2_BUSY;
   return iRetValue;
}

//------------------------------------------------------------------------------------------------------------------------
//    I2C_WriteRegister:
//------------------------------------------------------------------------------------------------------------------------
//    Liest ein Register von der angegebenen I²C-Slave-Adresse und der angegeben Register-Adresse
//------------------------------------------------------------------------------------------------------------------------
//    I2C_Slave_ADR:       I²C-Slave-Adresse (Device-Adresse) ohne RD/WR-Bit
//    I2C_Register:        Register-Adresse
//    bBuffer:             Pointer wo die Daten hin sollen
//    ByteCount:           Anzahl der zu lesenden Bytes
//------------------------------------------------------------------------------------------------------------------------
//    Return int:          0 Alles ok sonst Fehlernummer
//------------------------------------------------------------------------------------------------------------------------
// Testweise ohne monitor
int I2C_B2_WriteRegister( char I2C_Slave_ADR, char I2C_Register, void* pvDaten, char ByteCounter)
{
   BYTE *bBuffer = (BYTE*) pvDaten;
   if (mbIsBusyFlg & I2C_2_BUSY)
     return 4;

   #ifdef I2C_B2_HS_ADR
   UCB2I2CSA  = I2C_HS_ADR;                           // set HS-Adress
   UCB2CTL1  |= (UCTXSTT);                            // I2C TX, start condition
   while (!(UCB2IFG & UCNACKIFG) );                   // Warte auf NAK
   UCB2STAT &= ~UCNACKIFG;
   SET_UCB2_400kBAUD;                                 // set prescaler
   #endif

   int iRetValue=0;

   mbIsBusyFlg |= I2C_2_BUSY;

   UCB2I2CSA  = I2C_Slave_ADR;                        // set slave address
   UCB2CTL1  |= UCTR | UCTXSTT;                       // I2C TX, start condition

#ifdef I2C_B2_TIME_OUT_FUNC
   unsigned long  ulStartTime = I2C_B2_TIME_OUT_FUNC();
#endif
  while (!(UCB2IFG & UCTXIFG) )                      // Warte bis Start-Kondition und Slave-Adresse gesendet
   {
#ifdef I2C_B2_TIME_OUT_FUNC
      if ((UCB2IFG & UCNACKIFG) || (I2C_B2_TIME_OUT_FUNC() - ulStartTime) > I2C_B2_TIME_OUT_VALUE)
#else
      if (UCB2IFG & UCNACKIFG)
#endif
      {
         iRetValue = 1;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }
   }
   UCB2IFG &= ~UCTXIFG;

   UCB2TXBUF = I2C_Register;                          // Registerdaresse senden

   while (ByteCounter)
   {
#ifdef I2C_B2_TIME_OUT_FUNC
      ulStartTime = I2C_B2_TIME_OUT_FUNC();
#endif
      while (!(UCB2IFG & UCTXIFG) )                   // Warte bis senden abgeschlossen
      {
#ifdef I2C_B2_TIME_OUT_FUNC
         if ((UCB2IFG & UCNACKIFG) || (I2C_B2_TIME_OUT_FUNC() - ulStartTime) > I2C_B2_TIME_OUT_VALUE)
#else
         if (UCB2IFG & UCNACKIFG)
#endif
         {
            iRetValue = 2;
            I2C_Error( iRetValue );
            goto ErrorExit;
         }
      }

      UCB2IFG &= ~UCTXIFG;

      UCB2TXBUF = *bBuffer++;                         // Zeichen senden und Pointer erhöhen
      ByteCounter--;

   }

#ifdef I2C_B2_TIME_OUT_FUNC
   ulStartTime = I2C_B2_TIME_OUT_FUNC();
#endif
   while (!(UCB2IFG & UCTXIFG) )                      // Warte bis senden abgeschlossen
   {
#ifdef I2C_B2_TIME_OUT_FUNC
      if ((UCB2IFG & UCNACKIFG) || (I2C_B2_TIME_OUT_FUNC() - ulStartTime) > I2C_B2_TIME_OUT_VALUE)
#else
      if (UCB2IFG & UCNACKIFG)
#endif
      {
         iRetValue = 3;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }
   }
   // Hier geht es ohne Fehler weiter - also nicht durch ErrorExit-Label iritieren lassen

ErrorExit:
   UCB2CTL1 |=  UCTXSTP;                              // send STOP
   UCB2IFG  &= ~UCNACKIFG;
#ifdef I2C_B2_TIME_OUT_FUNC
   ulStartTime = I2C_B2_TIME_OUT_FUNC();
#endif
   while (UCB2CTL1 & UCTXSTP)                         // Warte bis Stop-Condition gesendet
#ifdef I2C_B2_TIME_OUT_FUNC
      if ((I2C_B2_TIME_OUT_FUNC() - ulStartTime) > I2C_B2_TIME_OUT_VALUE)
      {
         iRetValue=5;
         I2C_Error( iRetValue );
         UCB2CTL1 |=  UCSWRST;                        // Clear SW reset, resume operation
         _NOP();
         UCB2CTL1 &= ~UCSWRST;                        // Clear SW reset, resume operation
         break;
      }
#else
      _NOP();
#endif

#ifdef I2C_B2_HS_ADR
   SET_UCB2_100kBAUD;                                 // set prescaler
#endif

   mbIsBusyFlg &= ~I2C_2_BUSY;
   return iRetValue;
}

//------------------------------------------------------------------------------------------------------------------------
//    I2C_ReadRegister:
//------------------------------------------------------------------------------------------------------------------------
//    Liest ein Register von der angegebenen I²C-Slave-Adresse und der angegeben Register-Adresse
//------------------------------------------------------------------------------------------------------------------------
//    I2C_Slave_ADR:       I²C-Slave-Adresse (Device-Adresse) ohne RD/WR-Bit
//    I2C_Register:        Register-Adresse
//    bBuffer:             Pointer wo die Daten hin sollen
//    ByteCount:           Anzahl der zu lesenden Bytes
//------------------------------------------------------------------------------------------------------------------------
//    Return int:          0 Alles ok sonst Fehlernummer
//------------------------------------------------------------------------------------------------------------------------
// Testweise ohne monitor
int I2C_B2_ReadRegister_Adr16( char I2C_Slave_ADR, WORD I2C_Register, void* pvDaten, char ByteCounter)
{
   if (mbIsBusyFlg & I2C_2_BUSY)
      return 5;

   BYTE *bBuffer = (BYTE*) pvDaten;

   mbIsBusyFlg |= I2C_2_BUSY;

   #ifdef I2C_B2_HS_ADR
   UCB2I2CSA  = I2C_HS_ADR;                           // set HS-Adress
   UCB2CTL1  |= (UCTXSTT);                            // I2C TX, start condition
   while (!(UCB2IFG & UCNACKIFG) );                   // Warte auf NAK
   UCB2STAT &= ~UCNACKIFG;
   SET_UCB2_400kBAUD;                                 // set prescaler
   #endif

   int iRetValue=0;

   UCB2I2CSA  = I2C_Slave_ADR;                        // set slave address
   UCB2CTL1  |= (UCTR | UCTXSTT);                     // I2C TX, start condition

#ifdef I2C_B2_TIME_OUT_FUNC
   unsigned long  ulStartTime = I2C_B2_TIME_OUT_FUNC();
#endif
   while (!(UCB2IFG & UCTXIFG) )                       // Warte bis Start-Kondition und Slave-Adresse gesendet
   {
#ifdef I2C_B2_TIME_OUT_FUNC
      if ((UCB2IFG & UCNACKIFG) || (I2C_B2_TIME_OUT_FUNC() - ulStartTime) > I2C_B2_TIME_OUT_VALUE)
#else
      if (UCB2IFG & UCNACKIFG)
#endif
      {
         iRetValue = 1;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }

   }
   UCB2IFG &= ~UCTXIFG;
#ifdef I2C_B2_TIME_OUT_FUNC
   ulStartTime = I2C_B2_TIME_OUT_FUNC();
#endif
   UCB2TXBUF = I2C_Register>>8;                       // Registerdaresse senden
   while (!(UCB2IFG & UCTXIFG) )                      // Warte bis senden abgeschlossen
   {
#ifdef I2C_B2_TIME_OUT_FUNC
      if ((UCB2IFG & UCNACKIFG) || (I2C_B2_TIME_OUT_FUNC() - ulStartTime) > I2C_B2_TIME_OUT_VALUE)
#else
      if (UCB2IFG & UCNACKIFG)
#endif
      {
         iRetValue = 2;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }

   }
   UCB2IFG &= ~UCTXIFG;
#ifdef I2C_B2_TIME_OUT_FUNC
   ulStartTime = I2C_B2_TIME_OUT_FUNC();
#endif
   UCB2TXBUF = I2C_Register & 0xFF;                   // Registerdaresse senden
   while (!(UCB2IFG & UCTXIFG) )                      // Warte bis senden abgeschlossen
   {
#ifdef I2C_B2_TIME_OUT_FUNC
      if ((UCB2IFG & UCNACKIFG) || (I2C_B2_TIME_OUT_FUNC() - ulStartTime) > I2C_B2_TIME_OUT_VALUE)
#else
      if (UCB2IFG & UCNACKIFG)
#endif
      {
         iRetValue = 3;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }

   }

   UCB2IFG &= ~UCTXIFG;

   UCB2CTL1  &= ~UCTR ;                               // I2C Transmit-Bit löschen
   UCB2CTL1  |=  UCTXSTT ;                            // I2C TX, Restart condition
#ifdef I2C_B2_TIME_OUT_FUNC
   ulStartTime = I2C_B2_TIME_OUT_FUNC();
#endif
   while ( UCB2CTL1  & UCTXSTT )                      // Warte bis senden der I²C-Adresse abgeschlossen
   {
#ifdef I2C_B2_TIME_OUT_FUNC
      if ((UCB2IFG & UCNACKIFG) || (I2C_B2_TIME_OUT_FUNC() - ulStartTime) > I2C_B2_TIME_OUT_VALUE)
#else
      if (UCB2IFG & UCNACKIFG)
#endif
      {
         iRetValue = 4;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }

   }

   while (ByteCounter)
   {
      if (ByteCounter == 1)
         UCB2CTL1 |= (UCTXSTP);                       // Stop-Condition senden wenn letztes Byte empfangen wird

#ifdef I2C_B2_TIME_OUT_FUNC
      ulStartTime = I2C_B2_TIME_OUT_FUNC();
#endif
      while (!(UCB2IFG & UCRXIFG) )                   // Warte bis senden abgeschlossen
#ifdef I2C_B2_TIME_OUT_FUNC
         if ((I2C_B2_TIME_OUT_FUNC() - ulStartTime) > I2C_B2_TIME_OUT_VALUE)
         {
            iRetValue=5;
            I2C_Error( iRetValue );
            goto ErrorExit;
         }
#else
         _NOP();
#endif

      *bBuffer++ = UCB2RXBUF;                         // Zeichen Einlesen und speichern Pointer erhöhen
      ByteCounter--;
   }
   goto NormalExit;

ErrorExit:
   UCB2CTL1 |=  UCTXSTP;                              // send STOP if slave sends NACK
   UCB2IFG  &= ~UCNACKIFG;
NormalExit:
#ifdef I2C_B2_TIME_OUT_FUNC
   ulStartTime = I2C_B2_TIME_OUT_FUNC();
#endif
   while (UCB2CTL1 & UCTXSTP)                         // Warte bis Stop-Condition gesendet
#ifdef I2C_B2_TIME_OUT_FUNC
      if ((I2C_B2_TIME_OUT_FUNC() - ulStartTime) > I2C_B2_TIME_OUT_VALUE)
      {
         iRetValue=5;
         I2C_Error( iRetValue );
         UCB2CTL1 |=  UCSWRST;                        // Clear SW reset, resume operation
         _NOP();
         UCB2CTL1 &= ~UCSWRST;                        // Clear SW reset, resume operation
         break;
      }
#else
      _NOP();
#endif

#ifdef I2C_B2_HS_ADR
   SET_UCB2_100kBAUD
#endif
   mbIsBusyFlg &= ~I2C_2_BUSY;
   return iRetValue;
}

//------------------------------------------------------------------------------------------------------------------------
//    I2C_WriteRegister:
//------------------------------------------------------------------------------------------------------------------------
//    Liest ein Register von der angegebenen I²C-Slave-Adresse und der angegeben Register-Adresse
//------------------------------------------------------------------------------------------------------------------------
//    I2C_Slave_ADR:       I²C-Slave-Adresse (Device-Adresse) ohne RD/WR-Bit
//    I2C_Register:        Register-Adresse
//    bBuffer:             Pointer wo die Daten hin sollen
//    ByteCount:           Anzahl der zu lesenden Bytes
//------------------------------------------------------------------------------------------------------------------------
//    Return int:          0 Alles ok sonst Fehlernummer
//------------------------------------------------------------------------------------------------------------------------
// Testweise ohne monitor
int I2C_B2_WriteRegister_Adr16( char I2C_Slave_ADR, WORD I2C_Register, void* pvDaten, char ByteCounter)
{
   BYTE *bBuffer = (BYTE*) pvDaten;
   if (mbIsBusyFlg & I2C_2_BUSY)
      return 4;

   mbIsBusyFlg |= I2C_1_BUSY;

   #ifdef I2C_B2_HS_ADR
   UCB2I2CSA  = I2C_HS_ADR;                           // set HS-Adress
   UCB2CTL1  |= (UCTXSTT);                            // I2C TX, start condition
   while (!(UCB2IFG & UCNACKIFG) );                   // Warte auf NAK
   UCB2STAT &= ~UCNACKIFG;
   SET_UCB2_400kBAUD;                                 // set prescaler
   #endif

   UCB2I2CSA  = I2C_Slave_ADR;                        // set slave address
   UCB2CTL1  |= UCTR | UCTXSTT;                       // I2C TX, start condition

   int iRetValue = 0;

#ifdef I2C_B2_TIME_OUT_FUNC
   unsigned long  ulStartTime = I2C_B2_TIME_OUT_FUNC();
#endif
   while (!(UCB2IFG & UCTXIFG) )                      // Warte bis Start-Kondition und Slave-Adresse gesendet
   {
#ifdef I2C_B2_TIME_OUT_FUNC
      if ((UCB2IFG & UCNACKIFG) || (I2C_B2_TIME_OUT_FUNC() - ulStartTime) > I2C_B2_TIME_OUT_VALUE)
#else
      if (UCB2IFG & UCNACKIFG)
#endif
      {
         iRetValue = 1;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }

   }
   UCB2IFG &= ~UCTXIFG;

#ifdef I2C_B2_TIME_OUT_FUNC
    ulStartTime = I2C_B2_TIME_OUT_FUNC();
#endif
   UCB2TXBUF = I2C_Register>>8;                       // Registerdaresse senden
   while (!(UCB2IFG & UCTXIFG) )                      // Warte bis senden abgeschlossen
   {
#ifdef I2C_B2_TIME_OUT_FUNC
      if ((UCB2IFG & UCNACKIFG) || (I2C_B2_TIME_OUT_FUNC() - ulStartTime) > I2C_B2_TIME_OUT_VALUE)
#else
      if (UCB2IFG & UCNACKIFG)
#endif
      {
         iRetValue = 2;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }

   }
   UCB2IFG &= ~UCTXIFG;

   UCB2TXBUF = I2C_Register & 0xFF;                   // Registerdaresse senden

   while (ByteCounter)
   {
#ifdef I2C_B2_TIME_OUT_FUNC
      ulStartTime = I2C_B2_TIME_OUT_FUNC();
#endif
      while (!(UCB2IFG & UCTXIFG) )                   // Warte bis senden abgeschlossen
      {
#ifdef I2C_B2_TIME_OUT_FUNC
         if ((UCB2IFG & UCNACKIFG) || (I2C_B2_TIME_OUT_FUNC() - ulStartTime) > I2C_B2_TIME_OUT_VALUE)
#else
         if (UCB2IFG & UCNACKIFG)
#endif
         {
            iRetValue = 3;
            I2C_Error( iRetValue );
            goto ErrorExit;
         }

      }

      UCB2IFG &= ~UCTXIFG;

      UCB2TXBUF = *bBuffer++;                         // Zeichen senden und Pointer erhöhen
      ByteCounter--;

   }
#ifdef I2C_B2_TIME_OUT_FUNC
   ulStartTime = I2C_B2_TIME_OUT_FUNC();
#endif
   while (!(UCB2IFG & UCTXIFG) )                      // Warte bis senden abgeschlossen
   {
#ifdef I2C_B2_TIME_OUT_FUNC
      if ((UCB2IFG & UCNACKIFG) || (I2C_B2_TIME_OUT_FUNC() - ulStartTime) > I2C_B2_TIME_OUT_VALUE)
#else
      if (UCB2IFG & UCNACKIFG)
#endif
      {
         iRetValue = 4;
         I2C_Error( iRetValue );
         goto ErrorExit;
      }

   }
   // Hier geht es ohne Fehler weiter - also nicht durch ErrorExit-Label iritieren lassen

ErrorExit:
   UCB2CTL1 |=  UCTXSTP;                              // send STOP if slave sends NACK
   UCB2IFG  &= ~UCNACKIFG;

#ifdef I2C_B2_TIME_OUT_FUNC
   ulStartTime = I2C_B2_TIME_OUT_FUNC();
#endif
   while (UCB2CTL1 & UCTXSTP)                         // Warte bis Stop-Condition gesendet
#ifdef I2C_B2_TIME_OUT_FUNC
      if ((I2C_B2_TIME_OUT_FUNC() - ulStartTime) > I2C_B2_TIME_OUT_VALUE)
      {
         iRetValue=5;
         I2C_Error( iRetValue );
         UCB2CTL1 |=  UCSWRST;                        // Clear SW reset, resume operation
         _NOP();
         UCB2CTL1 &= ~UCSWRST;                        // Clear SW reset, resume operation
         break;
      }
#else
      _NOP();
#endif

#ifdef I2C_B2_HS_ADR
   SET_UCB2_100kBAUD
#endif
   mbIsBusyFlg &= ~I2C_2_BUSY;
   return iRetValue;
}

#endif                                                // I2C_USE_UCSI_B3
#endif // USE_MSP430F6658