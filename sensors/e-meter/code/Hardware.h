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
#ifndef HARDWARE
#define HARDWARE

#include <stdint.h>
#include "emeter-toolkit.h"
#include "emeter-3ph-neutral-6779.h"

#define MCLK_FREQ       (MCLK_DEF*1000000)
#define SMCLK_FREQ      (MCLK_DEF*1000000)
#define ACLK_FREQ       32768
#define BRCLK_FREQ      16777216

#define UCA3_BAUDRATE_CLK      SMCLK_FREQ

//#define true      (1==1)
//#define false     !true

typedef unsigned short     WORD;
typedef unsigned char      BYTE;

// Defines für RF430 (NFC-Tag)

#define LED_ON(x)       P7OUT &= ~(x)
#define LED_OFF(x)      P7OUT |= x
#define LED_TOGGLE(x)   P7OUT ^= x

#define RF430_INT0_BIT        BIT0
#define RF430_INT1_BIT        BIT1
#define RF430_INT_BITS        (RF430_INT1_BIT | RF430_INT0_BIT)

#define RF430_INT_PORT        P2IN
#define RF430_INT_DIR         P2DIR
#define RF430_IFG             P2IFG
#define RF430_IE              P2IE
#define RF430_IES             P2IES
#define RF430_REN             P2REN

#define isNFC_RF_FIELD_OFF     (RF430_INT_PORT & RF430_INT1_BIT)
#define isNFC_RF_FIELD_ON     !(isNFC_RF_FIELD_OFF)
#define isNFC_RF_INT_FLG_SET  (RF430_IFG & RF430_INT1_BIT)

#define NFC_I2C_BITS          (BIT6 | BIT7)
#define NFC_I2C_ON            P4SEL0 |=  NFC_I2C_BITS
#define NFC_I2C_OFF           P4SEL0 &= ~NFC_I2C_BITS
#define NGC_I2C_IN            P4IN

// P4OUT &= ~NFC_I2C_BITS;  P4DIR |= NFC_I2C_BITS; OUT und
#define NFC_POWER_OFF         NFC_I2C_OFF; P3OUT &= ~BIT7
#define NFC_POWER_ON          P3OUT |=  BIT7
#define NFC_POWER_DS          P3DS  |=  BIT7          // Power Drive strength

//************************************************************************************************************************
//    Hier für den RF430CL330H (NFC-Übertragung) notwendigen defines
//************************************************************************************************************************
#define RF430_WRITE_REG_ADR16(I2C_SADR, I2C_RADR, BUFFER, BLEN)     I2C_B0_WriteRegister_Adr16(I2C_SADR, I2C_RADR, BUFFER, BLEN)
#define RF430_READ_REG_ADR16(I2C_SADR, I2C_RADR, BUFFER, BLEN)      I2C_B0_ReadRegister_Adr16 (I2C_SADR, I2C_RADR, BUFFER, BLEN)
#define RF430_I2C_INIT(lBaud)                                       I2C_B0_Init(lBaud)
#define RF430_RESET_I2C       {UCB0CTLW0  |=  UCSWRST;  UCB0CTLW0  &= ~UCSWRST;}


#define RF430_I2C_ADR         0x0028

#define I2C_USE_USCI_B0
#define I2C_B0_CLK_SOURCE     SMCLK
#define I2C_B0_CLK_FREQ       SMCLK_FREQ
#define I2C_B0_SLAVE_ADR      0x10

//==============================================================================================================================================
//    Das ist die Funktion, die die Daten über SPI ans Funk-Modul übergibt
//    In diesem Fall ist das die UCA1-Schnittstelle. Mögliche andere Werte: UCA0, UCA2, UCA3, UCB0, UCB1, UCB2, UCB3
//    gehört eigentlich auch zu Hardware.h
//
//    Achtung für BLE-Modul muss Datenrate runtergesetzt werden
//    BLE       100kHz
//    HSU-Funk  1MHz
//==============================================================================================================================================


#define SPI_UCSI_A1_USED
#define SPI_UCSI_A1_BR           (SMCLK_FREQ / 1000000) /* Daniel BT-Modul 100000 HSU Funk 1000000 */
#define SPI_UCSI_A1_CLK_SRC       SMCLK
//#define SPI_UCSI_A1_CTL0_INIT    (UCCKPH | UCCKPL | UCMSB | UCMST | UCMODE_0 | UCSYNC)

#define RADIO_PUT_GET_BYTE(x)       SPI_UCSI_A1_Put_Get_Byte(x)

#define UCA1CTLW0_INIT           (UCMSB | UCMST | UCMODE_0 | UCSYNC | UCCKPH | UCCKPL)



//==============================================================================================================================================
//  Definitionen für das CS-Signal (Eigentlich in Hardware.h)
//==============================================================================================================================================
#define RADIO_CS_BIT            BIT0
#define RADIO_CS_PORT           P4OUT
#define _RadioCS_L()            RADIO_CS_PORT &= ~RADIO_CS_BIT
#define _RadioCS_H()            RADIO_CS_PORT |= RADIO_CS_BIT

#define RADIO_DATA_READY_PORT   P2IN
#define RADIO_DATA_READY_BIT    BIT2
#define RADIO_DATA_READY_IFG    P2IFG

// Data-Ready ist hier high-aktive (21.10.2015)
#define isRADIO_DATA_READY      (RADIO_DATA_READY_PORT & RADIO_DATA_READY_BIT)



/*
************************************************************************************************************************
    Auswahl Currenttransformer oder i-Flex Stromaufnehmer
    Damit kann auch ein größeres Spektrum an Transformern abgedeckt werden.

    Einstellung der Eingangsempfindlichkeit des ADC 24 für die Strommessung, betrifft dann alle Phasen
    siehe Datenblatt Prozessor. Muss ggf. für neue Typen anders eingestellt sein

      SD24GAIN_1
      SD24GAIN_2
      SD24GAIN_4
      SD24GAIN_8
      SD24GAIN_16
      SD24GAIN_32
      SD24GAIN_64
      SD24GAIN_128




    0) Einstellung Standard:
                          Stromshunt in Schaltung 0,41Ohm (2*0,82Ohm parallel)
                          EIGEN_CURRENT_PHASE_GAIN SD24GAIN_1
                          IRMS_E_GAIN_FACTOR 1                  Berechnet sich wie folgt (SD24GAIN_1 * 1)


    1) Bei 500A/100mA Transformer:
                          Stromshunt in Schaltung 0,41Ohm (2*0,82Ohm parallel)
                          EIGEN_CURRENT_PHASE_GAIN SD24GAIN_4
                          IRMS_E_GAIN_FACTOR 16                Berechnet sich wie folgt (SD24GAIN_4 * 4)

    2) Einstellung I-Flex:
                          Stromshunt in Schaltung 5,6kOhm + C 330nF  parallel
                          EIGEN_CURRENT_PHASE_GAIN SD24GAIN_8
                          IRMS_E_GAIN_FACTOR 32              Berechnet sich wie folgt (SD24GAIN_8 * 4)

************************************************************************************************************************
*/


#define CTAUSWAHL  0



#if (CTAUSWAHL == 0)
#define LCD_NK_AUSAWAHL 0
#define EIGEN_CURRENT_PHASE_GAIN SD24GAIN_1
#define IRMS_E_GAIN_FACTOR 1
#endif


#if (CTAUSWAHL == 1)
#define LCD_NK_AUSAWAHL 2
#define EIGEN_CURRENT_PHASE_GAIN SD24GAIN_4
#define IRMS_E_GAIN_FACTOR 16
#endif


#if (CTAUSWAHL == 2)
#define LCD_NK_AUSAWAHL 1
#define EIGEN_CURRENT_PHASE_GAIN SD24GAIN_8
#define IRMS_E_GAIN_FACTOR 32
#endif

//******************************************************************************
// Strom 3 Nachkommastellen
#if  (LCD_NK_AUSAWAHL == 0)
#define LCD_I_AUSGABE_NK  "I    A  %8.3f  "
#endif
// Strom 0 Nachkommastellen
#if  (LCD_NK_AUSAWAHL == 1)
#define LCD_I_AUSGABE_NK  "I    A %5.0f.0  "
#endif
// Strom 1 Nachkommastellen
#if  (LCD_NK_AUSAWAHL == 2)
#define LCD_I_AUSGABE_NK  "I    A  %6.1f   "
#endif
//******************************************************************************

//==============================================================================================================================================
//    UART UCA3 DMA
//==============================================================================================================================================

#define UCA3_UART_DMA0_CHAN                  0
#define UCA3_UART_DMA0_DMA_TSEL              DMACTL0
#define UCA3_UART_DMA0_TSEL_VAL_SET          DMA0TSEL__USCIA3RX
#define UCA3_UART_DMA0_TSEL_VAL_CLR          ~DMA0TSEL_25
#define UCA3_UART_DMA0_DMA_DA                DMA0DA
#define UCA3_UART_DMA0_DMA_SA                DMA0SA
#define UCA3_UART_DMA0_DMA_SZ                DMA0SZ
#define UCA3_UART_DMA0_DMA_CTL               DMA0CTL

#define UCA3_UART_DMA2_DMA_CHAN              2
#define UCA3_UART_DMA2_DMA_TSEL              DMACTL1
#define UCA3_UART_DMA2_TSEL_VAL_SET          DMA2TSEL__USCIA3RX
#define UCA3_UART_DMA2_DMA_DA                DMA2DA
#define UCA3_UART_DMA2_DMA_SA                DMA2SA
#define UCA3_UART_DMA2_DMA_SZ                DMA2SZ
#define UCA3_UART_DMA2_DMA_CTL               DMA2CTL
 
#endif