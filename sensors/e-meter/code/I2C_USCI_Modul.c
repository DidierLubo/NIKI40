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

#include "Hardware.h"
#include "I2C_USCI_Modul.h"

#define SMCLK  UCSSEL_2
#define ACLK   UCSSEL_1

#define I2C_0_BUSY         BIT0
#define I2C_1_BUSY         BIT1

BYTE mbIsBusyFlg;                                     // Flag für beide I2C siehe defines

#define SET_UCB0_400kBAUD   UCB0BRW  = I2C_B0_CLK_FREQ / 4000000;   // set prescaler für 400kBit/sec default
#define SET_UCB0_100kBAUD   UCB0BRW  = I2C_B0_CLK_FREQ / 1000000;   // set prescaler für 400kBit/sec default

#define SET_UCB1_400kBAUD   UCB1BRW  = I2C_B0_CLK_FREQ / 4000000;   // set prescaler für 400kBit/sec default
#define SET_UCB1_100kBAUD   UCB1BRW  = I2C_B0_CLK_FREQ / 1000000;   // set prescaler für 400kBit/sec default

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
   UCB0CTLW0  = UCSWRST;                              // Enable SW reset
   UCB0CTLW0  = UCMST | UCMODE_3 | UCSYNC | I2C_B0_CLK_SOURCE;     // I2C Master, synchronous mode
   UCB0BRW    = I2C_B0_CLK_FREQ / BaudRate;           // set prescaler für 100kBit/sec default
   UCB0I2CSA  = I2C_B0_SLAVE_ADR;                     // set slave address
   UCB0CTLW0 &= ~UCSWRST;                             // Clear SW reset, resume operation
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
__monitor int I2C_B0_ReadRegister( char I2C_Slave_ADR, char I2C_Register, void* pvDaten, char ByteCounter)
{
   BYTE *bBuffer = (BYTE*) pvDaten;

   if (mbIsBusyFlg & I2C_0_BUSY)
      return 4;

   mbIsBusyFlg |= I2C_0_BUSY;
   int iRetValue=0;

#ifdef I2C_HS_ADR
   UCB0IFG   &= ~UCTXIFG;
   UCB0I2CSA  = I2C_HS_ADR;                           // set HS-Adress
   UCB0CTL1  |= (UCTXSTT);                            // I2C TX, start condition
   while (!(UCB0IFG & UCNACKIFG) );                   // Warte auf NAK
   UCB0IFG &= ~UCNACKIFG;
   SET_UCB0_400kBAUD;
#endif

   UCB0IFG &= ~UCTXIFG;

   UCB0I2CSA  = I2C_Slave_ADR;                        // set slave address
   UCB0CTL1  |= (UCTR | UCTXSTT);                     // I2C TX, start condition

   while (!(UCB0IFG & UCTXIFG) )                      // Warte bis Start-Kondition und Slave-Adresse gesendet
   {
      if (UCB0IFG & UCNACKIFG)
      {
         iRetValue=1;
         goto ErrorExit;
      }
   }

   UCB0TXBUF  = I2C_Register;                         // Registerdaresse senden

   while (!(UCB0IFG & UCTXIFG) )                       // Warte bis senden abgeschlossen
   {
      if (UCB0IFG & UCNACKIFG)
      {
         iRetValue=2;
         goto ErrorExit;
      }
   }

   UCB0IFG &= ~UCTXIFG;

   UCB0CTL1  &= ~UCTR ;                               // I2C Transmit-Bit löschen
   UCB0CTL1  |=  UCTXSTT ;                            // I2C TX, Restart condition

   while ( UCB0CTL1  & UCTXSTT )                      // Warte bis senden der I²C-Adresse abgeschlossen
   {
      if (UCB0IFG & UCNACKIFG)
      {
         iRetValue=3;
         goto ErrorExit;
      }
   }

   while (ByteCounter)
   {
      if (ByteCounter == 1)
         UCB0CTL1 |= (UCTXSTP);                       // Stop-Condition senden wenn letztes Byte empfangen wird

      while (!(UCB0IFG & UCRXIFG) )                   // Warte bis senden abgeschlossen
         _NOP();

      *bBuffer++ = UCB0RXBUF;                         // Zeichen Einlesen und speichern Pointer erhöhen
      ByteCounter--;
   }

   goto NormalExit;

ErrorExit:
   UCB0CTL1 |=  UCTXSTP;                              // send STOP if slave sends NACK
   UCB0IFG  &= ~UCNACKIFG;
NormalExit:
   while (UCB0CTL1 & UCTXSTP)                         // Warte bis Stop-Condition gesendet
      _NOP();
   #ifdef I2C_HS_ADR
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
__monitor int I2C_B0_WriteRegister( char I2C_Slave_ADR, char I2C_Register, void* pvDaten, char ByteCounter)
{
   if (mbIsBusyFlg & I2C_0_BUSY)
      return 4;

   mbIsBusyFlg |= I2C_0_BUSY;
   BYTE *bBuffer = (BYTE*) pvDaten;

   int iRetValue=0;

   #ifdef I2C_HS_ADR
   UCB0IFG &= ~UCTXIFG;
   UCB0I2CSA  = I2C_HS_ADR;                           // set HS-Adress
   UCB0CTL1  |= (UCTXSTT);                            // I2C TX, start condition
   while (!(UCB0IFG & UCNACKIFG) );                   // Warte auf NAK
   UCB0IFG &= ~UCNACKIFG;
   SET_UCB0_400kBAUD;
   #endif

   UCB0IFG   &= ~UCTXIFG;
   UCB0I2CSA  = I2C_Slave_ADR;                        // set slave address
   UCB0CTL1  |= UCTR | UCTXSTT;                       // I2C TX, start condition

   while (!(UCB0IFG & UCTXIFG) )                      // Warte bis Start-Kondition und Slave-Adresse gesendet
   {
      if (UCB0IFG & UCNACKIFG)
      {
         iRetValue = 1;
         goto ErrorExit;
      }
   }
   UCB0IFG &= ~UCTXIFG;

   UCB0TXBUF = I2C_Register;                          // Registerdaresse senden

   while (ByteCounter)
   {
      while (!(UCB0IFG & UCTXIFG) )                   // Warte bis senden abgeschlossen
      {
         if (UCB0IFG & UCNACKIFG)
         {
            iRetValue = 2;
            goto ErrorExit;
         }
      }

      UCB0IFG &= ~UCTXIFG;

      UCB0TXBUF = *bBuffer++;                         // Zeichen senden und Pointer erhöhen
      ByteCounter--;

   }

   while (!(UCB0IFG & UCTXIFG) )                      // Warte bis senden abgeschlossen
   {
      if (UCB0IFG & UCNACKIFG)
      {
         iRetValue = 3;
         goto ErrorExit;
      }
   }

   // hier geht es ohne Fehler weiter (iRetValue = 0)

ErrorExit:
   UCB0CTL1 |=  UCTXSTP;                              // send STOP if slave sends NACK
   UCB0IFG  &= ~UCNACKIFG;
   while (UCB0CTL1 & UCTXSTP)                         // Warte bis Stop-Condition gesendet
      _NOP();
   #ifdef I2C_HS_ADR
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
__monitor int I2C_B0_ReadRegister_Adr16( char I2C_Slave_ADR, WORD I2C_Register, void* pvDaten, char ByteCounter)
{
   BYTE *bBuffer = (BYTE*) pvDaten;

   if (mbIsBusyFlg & I2C_0_BUSY)
      return 5;

   mbIsBusyFlg |= I2C_0_BUSY;

#ifdef I2C_HS_ADR
   UCB0IFG   &= ~UCTXIFG;
   UCB0I2CSA  = I2C_HS_ADR;                           // set HS-Adress
   UCB0CTL1  |= (UCTXSTT);                            // I2C TX, start condition
   while (!(UCB0IFG & UCNACKIFG) );                   // Warte auf NAK
   UCB0IFG &= ~UCNACKIFG;
   SET_UCB0_400kBAUD;
#endif

   int iRetValue=0;

   UCB0IFG &= ~UCTXIFG;
   UCB0I2CSA  = I2C_Slave_ADR;                        // set slave address
   UCB0CTL1  |= (UCTR | UCTXSTT);                     // I2C TX, start condition

   while (!(UCB0IFG & UCTXIFG) )                      // Warte bis Start-Kondition und Slave-Adresse gesendet
   {
      if (UCB0IFG & UCNACKIFG)
      {
         iRetValue = 1;
         goto ErrorExit;
      }
   }

   UCB0TXBUF  = I2C_Register>>8;                      // Registerdaresse senden

   while (!(UCB0IFG & UCTXIFG) )                      // Warte bis senden abgeschlossen
   {
      if (UCB0IFG & UCNACKIFG)
      {
         iRetValue = 2;
         goto ErrorExit;
      }
   }

   UCB0TXBUF  = I2C_Register;                         // Registerdaresse senden

   while (!(UCB0IFG & UCTXIFG) )                      // Warte bis senden abgeschlossen
   {
      if (UCB0IFG & UCNACKIFG)
      {
         iRetValue = 3;
         goto ErrorExit;
      }
   }


   UCB0IFG &= ~UCTXIFG;

   UCB0CTL1  &= ~UCTR ;                               // I2C Transmit-Bit löschen
   UCB0CTL1  |=  UCTXSTT ;                            // I2C TX, Restart condition

   while ( UCB0CTL1  & UCTXSTT )                      // Warte bis senden der I²C-Adresse abgeschlossen
   {
      if (UCB0IFG & UCNACKIFG)
      {
         iRetValue = 4;
         goto ErrorExit;
      }
   }

   while (ByteCounter)
   {
      if (ByteCounter == 1)
         UCB0CTL1 |= (UCTXSTP);                       // Stop-Condition senden wenn letztes Byte empfangen wird

      while (!(UCB0IFG & UCRXIFG) )                   // Warte bis senden abgeschlossen
         _NOP();

      *bBuffer++ = UCB0RXBUF;                         // Zeichen Einlesen und speichern Pointer erhöhen
      ByteCounter--;
   }
   goto NormalExit;

ErrorExit:
   UCB0CTL1 |=  UCTXSTP;                              // send STOP if slave sends NACK
   UCB0IFG  &= ~UCNACKIFG;

NormalExit:
   while (UCB0CTL1 & UCTXSTP)
      _NOP();
   #ifdef I2C_HS_ADR
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
__monitor int I2C_B0_WriteRegister_Adr16( char I2C_Slave_ADR, WORD I2C_Register, void* pvDaten, char ByteCounter)
{
   if (mbIsBusyFlg & I2C_0_BUSY)
      return 5;

   mbIsBusyFlg |= I2C_0_BUSY;
   BYTE *bBuffer = (BYTE*) pvDaten;

   #ifdef I2C_HS_ADR
   UCB0IFG   &= ~UCTXIFG;
   UCB0I2CSA  = I2C_HS_ADR;                           // set HS-Adress
   UCB0CTL1  |= (UCTXSTT);                            // I2C TX, start condition
   while (!(UCB0IFG & UCNACKIFG) );                   // Warte auf NAK
   UCB0IFG &= ~UCNACKIFG;
   SET_UCB0_400kBAUD;
   #endif

   int iRetValue = 0;

   UCB0IFG   &= ~UCTXIFG;
   UCB0I2CSA  = I2C_Slave_ADR;                        // set slave address
   UCB0CTL1  |= (UCTR | UCTXSTT);                     // I2C TX, start condition

   while (!(UCB0IFG & UCTXIFG) )                      // Warte bis Start-Kondition und Slave-Adresse gesendet
   {
      if (UCB0IFG & UCNACKIFG)
      {
         iRetValue = 1;
         goto ErrorExit;
      }
   }
   UCB0IFG &= ~UCTXIFG;
   UCB0TXBUF = I2C_Register>>8;                      // High-Register-Adresse senden

   while (!(UCB0IFG & UCTXIFG) )                     // Warte bis High-Register-Adresse gesendet
   {
      if (UCB0IFG & UCNACKIFG)
      {
         iRetValue = 2;
         goto ErrorExit;
      }
   }

   UCB0IFG &= ~UCTXIFG;
   UCB0TXBUF = I2C_Register & 0xFF;                   // Low-Register-Adresse senden

   while (ByteCounter)
   {
      while (!(UCB0IFG & UCTXIFG) )                   // Warte bis senden abgeschlossen
      {
         if (UCB0IFG & UCNACKIFG)
         {
            iRetValue = 3;
            goto ErrorExit;
         }
      }

      UCB0IFG &= ~UCTXIFG;

      UCB0TXBUF = *bBuffer++;                         // Zeichen senden und Pointer erhöhen
      ByteCounter--;

   }

   while (!(UCB0IFG & UCTXIFG) )                      // Warte bis senden abgeschlossen
   {
      if (UCB0IFG & UCNACKIFG)
      {
         iRetValue = 4;
         goto ErrorExit;
      }
   }

   // Hier geht es ohne Fehler weiter - also nicht durch ErrorExit-Label iritieren lassen

ErrorExit:
   UCB0CTL1 |=  UCTXSTP;                        // send STOP if slave sends NACK
   UCB0IFG  &= ~UCNACKIFG;
   while (UCB0CTL1 & UCTXSTP)
      _NOP();
   #ifdef I2C_HS_ADR
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
   UCB1CTL1 = I2C_1_CLK_SOURCE | UCSWRST;             // Use SMCLK, keep SW reset
   UCB1BRW  = I2C_1_CLK_FREQ / BaudRate;              // set prescaler
   UCB1I2CSA = I2C_1_SLAVE_ADR;                       // set slave address
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
__monitor int I2C_B1_ReadRegister( char I2C_Slave_ADR, char I2C_Register, void* pvDaten, char ByteCounter)
{
   if (mbIsBusyFlg & I2C_1_BUSY)
      return 4;

   BYTE *bBuffer = (BYTE*) pvDaten;

   mbIsBusyFlg |= I2C_1_BUSY;

#ifdef I2C_HS_ADR
   UCB1I2CSA  = I2C_HS_ADR;                           // set HS-Adress
   UCB1CTL1  |= (UCTXSTT);                            // I2C TX, start condition
   while (!(UCB1IFG & UCNACKIFG) );                   // Warte auf NAK
   UCB1STAT &= ~UCNACKIFG;
   SET_UCB1_400kBAUD;                                 // set prescaler
#endif

   int iRetValue=0;

   UCB1I2CSA  = I2C_Slave_ADR;                        // set slave address
   UCB1CTL1  |= (UCTR | UCTXSTT);                     // I2C TX, start condition

   while (!(UCB1IFG & UCTXIFG) )                       // Warte bis Start-Kondition und Slave-Adresse gesendet
   {
      if (UCB1IFG & UCNACKIFG)
      {
         iRetValue = 1;
         goto ErrorExit;
      }
   }
   UCB1IFG &= ~UCTXIFG;
   UCB1TXBUF  = I2C_Register;                         // Registerdaresse senden

   while (!(UCB1IFG & UCTXIFG) )                      // Warte bis senden abgeschlossen
   {
      if (UCB1IFG & UCNACKIFG)
      {
         iRetValue = 2;
         goto ErrorExit;
      }
   }

   UCB1IFG &= ~UCTXIFG;

   UCB1CTL1  &= ~UCTR ;                               // I2C Transmit-Bit löschen
   UCB1CTL1  |=  UCTXSTT ;                            // I2C TX, Restart condition

   while ( UCB1CTL1  & UCTXSTT )                      // Warte bis senden der I²C-Adresse abgeschlossen
   {
      if (UCB1IFG & UCNACKIFG)
      {
         iRetValue = 3;
         goto ErrorExit;
      }
   }

   while (ByteCounter)
   {
      if (ByteCounter == 1)
         UCB1CTL1 |= (UCTXSTP);                       // Stop-Condition senden wenn letztes Byte empfangen wird

      while (!(UCB1IFG & UCRXIFG) )                   // Warte bis senden abgeschlossen
         _NOP();

      *bBuffer++ = UCB1RXBUF;                         // Zeichen Einlesen und speichern Pointer erhöhen
      ByteCounter--;
   }

   goto NormalExit;

ErrorExit:
   UCB1CTL1 |=  UCTXSTP;                              // send STOP if slave sends NACK
   UCB1IFG  &= ~UCNACKIFG;

NormalExit:
   while (UCB1CTL1 & UCTXSTP)
      _NOP();
   #ifdef I2C_HS_ADR
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
__monitor int I2C_B1_WriteRegister( char I2C_Slave_ADR, char I2C_Register, void* pvDaten, char ByteCounter)
{
   BYTE *bBuffer = (BYTE*) pvDaten;
   if (mbIsBusyFlg & I2C_1_BUSY)
     return 4;

   #ifdef I2C_HS_ADR
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

   while (!(UCB1IFG & UCTXIFG) )                      // Warte bis Start-Kondition und Slave-Adresse gesendet
   {
      if (UCB1IFG & UCNACKIFG)
      {
         iRetValue = 1;
         goto ErrorExit;
      }
   }
   UCB1IFG &= ~UCTXIFG;

   UCB1TXBUF = I2C_Register;                          // Registerdaresse senden

   while (ByteCounter)
   {
      while (!(UCB1IFG & UCTXIFG) )                   // Warte bis senden abgeschlossen
      {
         if (UCB1IFG & UCNACKIFG)
         {
            iRetValue = 2;
            goto ErrorExit;
         }
      }

      UCB1IFG &= ~UCTXIFG;

      UCB1TXBUF = *bBuffer++;                         // Zeichen senden und Pointer erhöhen
      ByteCounter--;

   }

   while (!(UCB1IFG & UCTXIFG) )                      // Warte bis senden abgeschlossen
   {
      if (UCB1IFG & UCNACKIFG)
      {
         iRetValue = 3;
         goto ErrorExit;
      }
   }
   // Hier geht es ohne Fehler weiter - also nicht durch ErrorExit-Label iritieren lassen

ErrorExit:
   UCB1CTL1 |=  UCTXSTP;                              // send STOP
   UCB1IFG  &= ~UCNACKIFG;
   while (UCB1CTL1 & UCTXSTP)
      _NOP();
   SET_UCB1_100kBAUD;                                 // set prescaler

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
__monitor int I2C_B1_ReadRegister_Adr16( char I2C_Slave_ADR, WORD I2C_Register, void* pvDaten, char ByteCounter)
{
   if (mbIsBusyFlg & I2C_1_BUSY)
      return 5;

   BYTE *bBuffer = (BYTE*) pvDaten;

   mbIsBusyFlg |= I2C_1_BUSY;

   #ifdef I2C_HS_ADR
   UCB1I2CSA  = I2C_HS_ADR;                           // set HS-Adress
   UCB1CTL1  |= (UCTXSTT);                            // I2C TX, start condition
   while (!(UCB1IFG & UCNACKIFG) );                   // Warte auf NAK
   UCB1STAT &= ~UCNACKIFG;
   SET_UCB1_400kBAUD;                                 // set prescaler
   #endif

   int iRetValue=0;

   UCB1I2CSA  = I2C_Slave_ADR;                        // set slave address
   UCB1CTL1  |= (UCTR | UCTXSTT);                     // I2C TX, start condition

   while (!(UCB1IFG & UCTXIFG) )                       // Warte bis Start-Kondition und Slave-Adresse gesendet
   {
      if (UCB1IFG & UCNACKIFG)
      {
         iRetValue = 1;
         goto ErrorExit;
      }
   }
   UCB1IFG &= ~UCTXIFG;

   UCB1TXBUF = I2C_Register>>8;                       // Registerdaresse senden
   while (!(UCB1IFG & UCTXIFG) )                      // Warte bis senden abgeschlossen
   {
      if (UCB1IFG & UCNACKIFG)
      {
         iRetValue = 2;
         goto ErrorExit;
      }
   }
   UCB1IFG &= ~UCTXIFG;

   UCB1TXBUF = I2C_Register & 0xFF;                  // Registerdaresse senden
   while (!(UCB1IFG & UCTXIFG) )                      // Warte bis senden abgeschlossen
   {
      if (UCB1IFG & UCNACKIFG)
      {
         iRetValue = 3;
         goto ErrorExit;
      }
   }

   UCB1IFG &= ~UCTXIFG;

   UCB1CTL1  &= ~UCTR ;                               // I2C Transmit-Bit löschen
   UCB1CTL1  |=  UCTXSTT ;                            // I2C TX, Restart condition

   while ( UCB1CTL1  & UCTXSTT )                      // Warte bis senden der I²C-Adresse abgeschlossen
   {
      if (UCB1IFG & UCNACKIFG)
      {
         iRetValue = 4;
         goto ErrorExit;
      }
   }

   while (ByteCounter)
   {
      if (ByteCounter == 1)
         UCB1CTL1 |= (UCTXSTP);                       // Stop-Condition senden wenn letztes Byte empfangen wird

      while (!(UCB1IFG & UCRXIFG) )                   // Warte bis senden abgeschlossen
         _NOP();

      *bBuffer++ = UCB1RXBUF;                         // Zeichen Einlesen und speichern Pointer erhöhen
      ByteCounter--;
   }
   goto NormalExit;

ErrorExit:
   UCB1CTL1 |=  UCTXSTP;                        // send STOP if slave sends NACK
   UCB1IFG  &= ~UCNACKIFG;
NormalExit:
   while (UCB1CTL1 & UCTXSTP)
      _NOP();
#ifdef I2C_HS_ADR
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
__monitor int I2C_B1_WriteRegister_Adr16( char I2C_Slave_ADR, WORD I2C_Register, void* pvDaten, char ByteCounter)
{
   BYTE *bBuffer = (BYTE*) pvDaten;
  if (mbIsBusyFlg & I2C_1_BUSY)
     return 4;

   mbIsBusyFlg |= I2C_1_BUSY;

   UCB1I2CSA  = I2C_Slave_ADR;                        // set slave address
   UCB1CTL1  |= UCTR | UCTXSTT;                       // I2C TX, start condition

   while (!(UCB1IFG & UCTXIFG) )                      // Warte bis Start-Kondition und Slave-Adresse gesendet
   {
      if (UCB1IFG & UCNACKIFG)
      {
         iRetValue = 1;
         goto ErrorExit;
      }
   }
   UCB1IFG &= ~UCTXIFG;

   UCB1TXBUF = I2C_Register>>8;                       // Registerdaresse senden
   while (!(UCB1IFG & UCTXIFG) )                      // Warte bis senden abgeschlossen
   {
      if (UCB1IFG & UCNACKIFG)
      {
         iRetValue = 2;
         goto ErrorExit;
      }
   }
   UCB1IFG &= ~UCTXIFG;

   UCB1TXBUF = I2C_Register & 0xFF;                  // Registerdaresse senden

   while (ByteCounter)
   {
      while (!(UCB1IFG & UCTXIFG) )                   // Warte bis senden abgeschlossen
      {
         if (UCB1IFG & UCNACKIFG)
         {
         iRetValue = 3;
         goto ErrorExit;
         }
      }

      UCB1IFG &= ~UCTXIFG;

      UCB1TXBUF = *bBuffer++;                         // Zeichen senden und Pointer erhöhen
      ByteCounter--;

   }

   while (!(UCB1IFG & UCTXIFG) )                      // Warte bis senden abgeschlossen
   {
      if (UCB1IFG & UCNACKIFG)
      {
         iRetValue = 4;
         goto ErrorExit;
      }
   }
   // Hier geht es ohne Fehler weiter - also nicht durch ErrorExit-Label iritieren lassen

ErrorExit:
   UCB1CTL1 |=  UCTXSTP;                              // send STOP if slave sends NACK
   UCB1IFG  &= ~UCNACKIFG;

   while (UCB1CTL1 & UCTXSTP)
      _NOP();
#ifdef I2C_HS_ADR
   SET_UCB1_100kBAUD
#endif
   mbIsBusyFlg &= ~I2C_1_BUSY;
   return iRetValue;
}

#endif                                                // I2C_USE_UCSI_B1
