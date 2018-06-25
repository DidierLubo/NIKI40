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
//*  Vorlageprojekt mit USB Hardwaredefinitionen aufgebaut aus Schocklogger                                              *
//*  Erstellt am 11.04.2013                                                                                              *
//************************************************************************************************************************
//*                                                                                                                      *
//*   Projekt 313409: MetaSensController iFiL_MetaSens_Electronic                                                        *
//*                                                                                                                      *
//*   Erfasst Spirometerdaten, liniarisiert diese und gibt das Ergebnis über USB und später auch über                    *
//*   Bluetooth (PAN1323-Modul) aus                                                                                      *
//*                                                                                                                      *
//*                                                                                                                      *
//*                                                                                                                      *
//*                                                                                                                      *
//*                                                                                                                      *
//************************************************************************************************************************

#ifndef _HARDWARE_INCLUDED
#define _HARDWARE_INCLUDED

//************************************************************************************************************************
//    Auswahl Registerformat
//************************************************************************************************************************

#include <msp430.h>

#include "USB_Common\types.h"                         //Basic Type declarations
#include "HAL.h"

//************************************************************************************************************************
//    Grunddefinitionen
//************************************************************************************************************************

   /* Definitions for compilers that required structure to be explicitly*/
   /* declared as packed.                                               */
#define __PACKED_STRUCT_BEGIN__

#define __PACKED_STRUCT_END__

#define Byte_t    BYTE
#define Word_t    WORD

#define true  1
#define false 0

//************************************************************************************************************************
//    Takterzeugung
//************************************************************************************************************************

#define XT2_FREQ        19200000l                     // Quarz-Oszillator 19.2MHz
#define MCLK_FREQ       XT2_FREQ
#define SMCLK_FREQ     (XT2_FREQ / 2)                 // z. B. I2C / Timer etc. 9.6 MHz
#define ACLK_FREQ          32768                      // Für Interner Oszillator

#define TIMER_B_CLK_SOURCE    TBSSEL__SMCLK
#define TIMER_B_CLK_FREQ      SMCLK_FREQ

#define isXT2_running   ( !(UCSCTL6 & XT2OFF) )
#define isXT2_off       (UCSCTL6 & XT2OFF)

#define is_USB_PG       (USBPWRCTL & USBBGVBV)

#define MSP430_TICK_RATE_HZ      1024

//************************************************************************************************************************
//    CRC Definitionen
//************************************************************************************************************************

#define CRC_START_VALUE  0xAA55                       // CRC Startrwert

//************************************************************************************************************************
//    Port 1 Einstellungen: (Status-Enable für PULL-UP der Akku-Statusleitungen)
//    Akku_Status1; Akku_Status2; Lade-Power-Good; Status-Enable; Lade-Mode; Flow HTS-PWM; Flow HTS SD;  USB-Strom_Limit
//    in;           in;           in;              out;           out;       out;          out;          out
//************************************************************************************************************************
#define DP1OUT 0x00
#define DP1DIR 0x1F
#define DP1REN 0x00
#define DP1DS  0x00
#define DP1SEL 0x00
#define DP1IES 0x00
#define DP1IE  0x00

//************************************************************************************************************************
//    Port 2 Einstellungen
//    UCB0_SDA;   UCB0_SCL;   PRES_INT1;  PRES_INT2;  CO2_Hz_dig; CO2_S2_dig;         CO2_S1_dig;         Flow_HTC_ON
//    I²C;        I²C;        in;         in;         TBCCR4 out; out/TBCCR3 Cap in;  out/TBCCR2 Cap in;  Out
//************************************************************************************************************************
#define DP2OUT 0x00
#define DP2DIR 0x0F
#define DP2REN 0x00
#define DP2DS  0x00
#define DP2SEL 0xC0
#define DP2IES 0x00
#define DP2IE  0x00

#define P2MAP0_VAL      PM_NONE
#define P2MAP1_VAL      PM_NONE
#define P2MAP2_VAL      PM_NONE
#define P2MAP3_VAL      PM_NONE
#define P2MAP4_VAL      PM_NONE
#define P2MAP5_VAL      PM_NONE
#define P2MAP6_VAL      PM_UCB0SCL
#define P2MAP7_VAL      PM_UCB0SDA

//******************************************************* ****************************************************************
//    Port 3 Einstellungen:
//    OLED-Data-Port
//    out; out; out; out; out; out; out; out;
//************************************************************************************************************************
#define DP3OUT 0x00
#define DP3DIR 0xFF
#define DP3REN 0x00
#define DP3DS  0x00
#define DP3SEL 0x00
#define DP3IES 0x00
#define DP3IE  0x00

//************************************************************************************************************************
//    Port 4 Einstellungen (Für Bluetooth) Um leichert löten zu können immer zwei Pins zusammen
//    RF-Data-Ready; Light-IRQ;  NFC INT2;   NFC INT1;   OLED_CS;    OLED_POWER_Enable;   TBCCR1;        TP101
//    in;            in;         in;         in;         out;        out;                 out TB0_CCR1;  out
//************************************************************************************************************************
#define DP4OUT 0x88
#define DP4DIR 0x0F
#define DP4REN 0x80
#define DP4DS  0x00
#define DP4SEL 0x02
#define DP4IES 0x20
#define DP4IE  0x00

//************************************************************************************************************************
//    Port 5 Einstellungen
//    TP100;   DMAE0;   LED-GE;  LED-GN;  RF.OnOff; RF.Reset;   VREF-;   VREF+
//    out;     in;      out;     out;     out;      out;        in;      out;
//************************************************************************************************************************
#define DP5OUT 0x00
#define DP5DIR 0xBD
#define DP5REN 0x00
#define DP5DS  0x00
#define DP5SEL 0x43

//************************************************************************************************************************
//    Port 6 Einstellungen
//    Flow_HTS_DAC;  CO2_S2_an;  CO2_S1_an;  CO2_HZ_Strom;  Flow_Temp_Diode;  Flow_TPT1;  Flow_TP2;   Flow_TP1
//    in A7;         in A6;      in A5;      in A4;         in A3;            in A2;      in A1;      in A0;
//************************************************************************************************************************
#define DP6OUT 0x00
#define DP6DIR 0x00
#define DP6REN 0x00
#define DP6DS  0x00
#define DP6SEL 0xFF                                   // ADC-Eingänge P6.0 ... P6.7 (HTS_DAC als Eingang UREF als Heizervorgabe)

//************************************************************************************************************************
//    Port 7 Einstellungen
//    OLED_RD#; OLED_WR#; OLED_CMD_D#; OLED_RESET#;   XT2OUT;  XT2IN;   ?;    ?;
//    out;      out;      out;         out;           SEL;     SEL;
//************************************************************************************************************************
#define DP7OUT 0x00
#define DP7DIR 0xFB
#define DP7REN 0x00
#define DP7DS  0x00
#define DP7SEL 0x0C

//************************************************************************************************************************
//    Port 8 Einstellungen                      nur interner PullUp/PullDown
//    PowerPlugIn; UCB1SCL; UCB1SDA; NFC_SW_3V; PowerPlugEn; DAC_UCA1SIMO;  DAC_UCA1CLK; DAC_LOAD;
//    in pull-up;  sel;     sel;     out;       in;         out+sel;       out+sel;     out;
//************************************************************************************************************************
#define DP8OUT 0x00
#define DP8DIR 0x17
#define DP8REN 0x80
#define DP8DS  0x00
#define DP8SEL 0x66

//************************************************************************************************************************
//    Port 9 Einstellungen
//    CO2 Enable; UCB2SCL; UCB2SDA; WirlesPowerGood; RF.MISO; RF.MOSI; RF.SCLK; RF.CS
//                                                   UART-RXD UART-TXD
//    out         sel      sel      in               in+sel   out+sel  out      out
//************************************************************************************************************************
#define DP9OUT 0x00
#define DP9DIR 0x87
#define DP9REN 0x00
#define DP9DS  0x00
#define DP9SEL 0x6C

//************************************************************************************************************************
//    Port J Einstellungen
//************************************************************************************************************************
#define DPJOUT 0x00
#define DPJDIR 0x00
#define DPJREN 0x00
#define DPJDS  0x00
//************************************************************************************************************************

//************************************************************************************************************************
//
//************************************************************************************************************************
//************************************************************************************************************************
//    FlowSensorHeizer
//************************************************************************************************************************
#define FLOW_HEIZER_ON        P1OUT &= ~BIT1
#define FLOW_HEIZER_OFF       P1OUT |=  BIT1

//************************************************************************************************************************
//    LED-Defines
//************************************************************************************************************************
#define LED_PORT              P5OUT
#define LED_GN                BIT4
#define LED_GE                BIT5

#define LED_GN_OFF            LED_PORT &= ~LED_GN
#define LED_GN_ON             LED_PORT |=  LED_GN
#define LED_GN_TOGGLE         LED_PORT ^=  LED_GN

#define LED_GE_OFF            LED_PORT &= ~LED_GE
#define LED_GE_ON             LED_PORT |=  LED_GE
#define LED_GE_TOGGLE         LED_PORT ^=  LED_GE

#define LED_MASK              (LED_GE | LED_GN)

#define LED_ALL_ON            LED_PORT |=  (LED_GE | LED_GN)
#define LED_ALL_OFF           LED_PORT &= ~(LED_GE | LED_GN)

//************************************************************************************************************************
//    ADC-Kanal defines
//************************************************************************************************************************
#define ADC_FLOW_X            ADC12INCH_0
#define ADC_FLOW_Y            ADC12INCH_1
#define ADC_FLOW_Xp           ADC12INCH_2
#define ADC_FLOW_Yp           ADC12INCH_3

#define ADC_CO2_HZ            ADC12INCH_4
#define ADC_CO2_S1            ADC12INCH_5
#define ADC_CO2_S2            ADC12INCH_6

#define ADC_TBCCR1_DMA_CHAN   2

//#define ADC_TBCCR1_DMA_TSEL            DMACTL1
//#define ADC_TBCCR1_DMA_TSEL_VAL_SET    DMA2TSEL__DMAE0
//#define ADC_TBCCR1_DMA_TSEL_VAL_CLR    ~DMA2TSEL_31
//#define ADC_TBCCR1_DMA_DA              DMA2DA
//#define ADC_TBCCR1_DMA_SA              DMA2SA
//#define ADC_TBCCR1_DMA_SZ              DMA2SZ
//#define ADC_TBCCR1_DMA_CTL             DMA2CTL

//************************************************************************************************************************
//    OLED defines
//************************************************************************************************************************
#define OLED_WIDTH            128
#define OLED_HEIGHT           64
#define OLED_LINES            (OLED_HEIGHT / 8)

#define OLED_DATA_IN          P3IN
#define OLED_DATA_OUT         P3OUT
#define OLED_DATA_DIR         P3DIR

#define OLED_CTRL_PORT        P4OUT
#define OLED_POWER_ENABLE_BIT BIT2
#define OLED_CS_BIT           BIT3

#define OLED_CTRL_PORT1       P7OUT

#define OLED_RESET_BIT        BIT4
#define OLED_D_CMD_BIT        BIT5
#define OLED_WR_BIT           BIT6
#define OLED_RD_BIT           BIT7

#define OLED_POWER_ON         OLED_CTRL_PORT |=  OLED_POWER_ENABLE_BIT
#define OLED_POWER_OFF        OLED_CTRL_PORT &= ~OLED_POWER_ENABLE_BIT
#define isOLED_POWER_ON       (OLED_CTRL_PORT & OLED_POWER_ENABLE_BIT)

#define OLED_CS_LOW           OLED_CTRL_PORT &= ~OLED_CS_BIT
#define OLED_CS_HIGH          OLED_CTRL_PORT |=  OLED_CS_BIT

#define OLED_SET_DATA_MODE    OLED_CTRL_PORT1 |=  OLED_D_CMD_BIT
#define OLED_SET_CMD_MODE     OLED_CTRL_PORT1 &= ~OLED_D_CMD_BIT

#define OLED_RESET_LOW        OLED_CTRL_PORT1 &= ~OLED_RESET_BIT
#define OLED_RESET_HIGH       OLED_CTRL_PORT1 |=  OLED_RESET_BIT

#define OLED_WR_LOW           OLED_CTRL_PORT1 &= ~OLED_WR_BIT
#define OLED_WR_HIGH          OLED_CTRL_PORT1 |=  OLED_WR_BIT

#define OLED_RD_LOW           OLED_CTRL_PORT1 &= ~OLED_RD_BIT
#define OLED_RD_HIGH          OLED_CTRL_PORT1 |=  OLED_RD_BIT


//************************************************************************************************************************
//    Alle 3 I2C-Module werden benötigt
//************************************************************************************************************************
#define I2C_USE_USCI_B0                               // Akku : BQ27421 | Licht : Si1132 | Feuchte / Druck / Temp : BME280
#define I2C_USE_USCI_B1                               // NFC  : RF430
#define I2C_USE_USCI_B2                               // CO2  : EE893

#define I2C_B0_CLK_SOURCE        UCSSEL__SMCLK
#define I2C_B1_CLK_SOURCE        UCSSEL__SMCLK
#define I2C_B2_CLK_SOURCE        UCSSEL__ACLK

#define I2C_B0_CLK_FREQ          SMCLK_FREQ
#define I2C_B1_CLK_FREQ          SMCLK_FREQ
#define I2C_B2_CLK_FREQ          ACLK_FREQ

#define I2C_B0_SLAVE_ADR         0x02
#define I2C_B1_SLAVE_ADR         0x02
#define I2C_B2_SLAVE_ADR         0x02

#define I2C_B0_TIME_OUT_FUNC     HAL_GetTickCount
#define I2C_B0_TIME_OUT_VALUE    3
#define I2C_B1_TIME_OUT_FUNC     HAL_GetTickCount
#define I2C_B1_TIME_OUT_VALUE    3
//#define I2C_B2_TIME_OUT_FUNC     HAL_GetTickCount
//#define I2C_B2_TIME_OUT_VALUE    5

//************************************************************************************************************************
//    Hier für den BQ27421 (Akku-Überwachung) notwendigen defines
//************************************************************************************************************************
#ifdef EIS_V1_1

#include "BQ27200.h"

#define BQ27200_RS_in_mOhm       20.0f                // mOhm
#define BQ27200_AkkuCapacity     4000.0f              // mAh

#define BQ27200_WRITE_REG(I2C_SADR, I2C_RADR, BUFFER, BLEN)     I2C_B0_WriteRegister(I2C_SADR, I2C_RADR, BUFFER, BLEN)
#define BQ27200_READ_REG(I2C_SADR, I2C_RADR, BUFFER, BLEN)      I2C_B0_ReadRegister (I2C_SADR, I2C_RADR, BUFFER, BLEN)

#define AKKU_INIT()             BQ27200_Init()
#define AKKU_GET_REL_POWER()    BQ27200_GetRelPower()
#define AKKU_GET_SPANNUNG()     BQ27200_GetAkkuSpannung()
#define AKKU_GET_AVG_CURRENT()  BQ27200_GetAverageCurrent()

#else

#include "BQ27421-G1.h"

#define BQ27421_WRITE_REG(I2C_SADR, I2C_RADR, BUFFER, BLEN)     I2C_B0_WriteRegister(I2C_SADR, I2C_RADR, BUFFER, BLEN)
#define BQ27421_READ_REG(I2C_SADR, I2C_RADR, BUFFER, BLEN)      I2C_B0_ReadRegister (I2C_SADR, I2C_RADR, BUFFER, BLEN)

#define AKKU_INIT()             BQ27421_Init()
#define AKKU_GET_REL_POWER      BQ27421_GetRelPower
#define AKKU_GET_SPANNUNG()     BQ27421_GetAkkuSpannung()
#define AKKU_GET_AVG_CURRENT()  BQ27421_GetAverageCurrent()

#define BQ27421_AkkuCapacity     4000                 // mAh
#endif

//************************************************************************************************************************
//    Hier für den BQ24070 (Akku-Ladung) notwendigen defines
//************************************************************************************************************************
#define AKKU_LADE_LIMIT_100mA    P1OUT &= ~(BIT3 | BIT0)
#define AKKU_LADE_LIMIT_500mA    P1OUT &= ~ BIT3; P1OUT |=  BIT0

#define isAKKU_IN_USB_LADE_MODE  (!(P1IN  & BIT3))
#define AKKU_USB_LADE_MODE       P1OUT &= ~BIT3
#define AKKU_POWER_LADE_MODE     P1OUT |=  BIT3

#define AKKU_LADE_POWER_GOOD     BIT5
#define AKKU_STATUS_STAT1_BIT    BIT7
#define AKKU_STATUS_STAT2_BIT    BIT6

#define AKKU_STATUS_PRECHARGE      0
#define AKKU_STATUS_CHARGE         AKKU_STATUS_STAT2_BIT
#define AKKU_STATUS_CHARGE_DONE    AKKU_STATUS_STAT1_BIT
#define AKKU_STATUS_SLEEP         (AKKU_STATUS_STAT1_BIT | AKKU_STATUS_STAT2_BIT)
#define AKKU_STATUS_ERROR         (AKKU_STATUS_STAT1_BIT | AKKU_STATUS_STAT2_BIT)

#define AKKU_get_STATUS          (P1IN & (AKKU_STATUS_STAT1_BIT | AKKU_STATUS_STAT2_BIT))
#define AKKU_is_LADE_POWER_GOOD  (P1IN &  AKKU_LADE_POWER_GOOD)

#define AKKU_STATUS_ENABLE       P1OUT |= BIT4
#define AKKU_STATUS_DISABLE      P1OUT &= ~BIT4

//************************************************************************************************************************
//    Hier Definitionen für die Lade-Rundstecker Überwachung
//************************************************************************************************************************
#define POWER_PLUG_BIT           BIT7
inline BYTE isPower_Plug_Connected(void)
{
   BYTE bRetValue;
   P8OUT |= POWER_PLUG_BIT;                           // Pull-Up enablen
   _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP();    // ein bischen Warten um Port Zeit zu geben auf high zu gehen
   bRetValue = P8IN & POWER_PLUG_BIT;                 // Port abfragen
   P8OUT &= ~POWER_PLUG_BIT;                          // PulUp zu PullDown werden lassen um strom zu sparen
   return bRetValue;
}

//#define LADE_STECKER_PULL_UP_ON  P8OUT |=  BIT3
//#define LADE_STECKER_PULL_UP_OFF P8OUT &= ~BIT3
//#define isLADE_STECKER_CONNECTED (!(P8IN & BIT7))

//************************************************************************************************************************
//    Hier für den RF430CL330H (NFC-Übertragung) notwendigen defines
//************************************************************************************************************************
#define RF430_WRITE_REG_ADR16(I2C_SADR, I2C_RADR, BUFFER, BLEN)     I2C_B1_WriteRegister_Adr16(I2C_SADR, I2C_RADR, BUFFER, BLEN)
#define RF430_READ_REG_ADR16(I2C_SADR, I2C_RADR, BUFFER, BLEN)      I2C_B1_ReadRegister_Adr16 (I2C_SADR, I2C_RADR, BUFFER, BLEN)
#define RF430_I2C_INIT(lBaud)                                       I2C_B1_Init(lBaud)
#define RF430_RESET_I2C       {UCB1CTLW0  |=  UCSWRST;  UCB1CTLW0  &= ~UCSWRST;}

#define RF430_I2C_ADR         0x28

#define RF430_INT0_BIT        BIT4
#define RF430_INT1_BIT        BIT5
#define RF430_INT_BITS        (RF430_INT1_BIT | RF430_INT0_BIT)

#define RF430_INT_PORT        P4IN
#define RF430_INT_DIR         P4DIR
#define RF430_INT_OUT         P4OUT
#define RF430_IFG             P4IFG
#define RF430_IE              P4IE
#define RF430_IES             P4IES
#define RF430_REN             P4REN

#define isNFC_RF_FIELD_OFF     (RF430_INT_PORT & RF430_INT1_BIT)
#define isNFC_RF_FIELD_ON     !(isNFC_RF_FIELD_OFF)
#define isNFC_RF_INT_FLG_SET  (RF430_IFG &   RF430_INT1_BIT)
#define NFC_RF_INT_FLG_CLR     RF430_IFG &= ~RF430_INT1_BIT

#define RF430_I2C_BITS          (BIT5 | BIT6)
#define RF430_I2C_ON            P8SEL |=  RF430_I2C_BITS
#define RF430_I2C_OFF           P8SEL &= ~RF430_I2C_BITS
#define RF430_I2C_IN            P8IN
#define RF430_POWER_ON          {RF430_I2C_ON  ; P8OUT |=  BIT4;}
#define RF430_POWER_OFF         {RF430_I2C_OFF ; P8OUT &= ~BIT4;}

//========================================================================================================================
// Achtung Hardware-Änderungen notwendig: WP-Stecker - PIN 11 mit TP104 (P8.7) verbinden
// Sicherheitshalber R100 (neben der LED) von WP-Platine entfernen damit 5V von USB und Externem Netzteil
// nicht auf MSP-Port gelangt
//========================================================================================================================
#define isWP_OFF              (P9IN & BIT4)
#define isWP_ON               (!isWP_OFF)

//************************************************************************************************************************
//    Hier für den BME280 (Druck / Feuchte / Temperatur - Sensor)
//************************************************************************************************************************

#define BME280_WRITE_REG(I2C_SADR, I2C_RADR, BUFFER, BLEN)     I2C_B0_WriteRegister(I2C_SADR, I2C_RADR, BUFFER, BLEN)
#define BME280_READ_REG(I2C_SADR, I2C_RADR, BUFFER, BLEN)      I2C_B0_ReadRegister (I2C_SADR, I2C_RADR, BUFFER, BLEN)
#define BME280_I2C_ADR           0x76
#define BME280_VAL_CTRL_HUM      ( BME280_VAL_CH_OVERSAMPLING16 )
#define BME280_VAL_CTRL_MEAS     ( BME280_VAL_CM_TEMP_OVERSAMPLING16 | BME280_VAL_CM_PRESS_OVERSAMPLING16 )
#define BME280_VAL_CONFIG        ( BME280_VAL_CON_IA_1000ms          | BME280_VAL_CON_FC_2 )

//************************************************************************************************************************
//    Hier für den RF430CL330H (NFC-Übertragung) notwendigen defines
//************************************************************************************************************************

#define Si1132_WRITE_REG(I2C_SADR, I2C_RADR, BUFFER, BLEN)     I2C_B0_WriteRegister(I2C_SADR, I2C_RADR, BUFFER, BLEN)
#define Si1132_READ_REG(I2C_SADR, I2C_RADR, BUFFER, BLEN)      I2C_B0_ReadRegister (I2C_SADR, I2C_RADR, BUFFER, BLEN)
#define Si1132_I2C_ADR        0x60

#define Si1132_IRQ_PORT_BIT   BIT6
#define Si1132_IFG            P4IFG
#define Si1132_IE             P4IE

#define isSi1132_IFG_SET      (Si1132_IFG & Si1132_IRQ_PORT_BIT)
#define Si1132_CLR_IFG         Si1132_IFG &= ~Si1132_IRQ_PORT_BIT

#define Si1132_ENABLE_IRQ      Si1132_IE |=  Si1132_IRQ_PORT_BIT
#define Si1132_DISABLE_IRQ     Si1132_IE &= ~Si1132_IRQ_PORT_BIT

//************************************************************************************************************************
//    Hier für den DAC124S08
//************************************************************************************************************************

#define SPI_UCSI_A1_USED
#define SPI_UCSI_A1_BR        (SMCLK_FREQ / 4000000)
#define SPI_UCSI_A1_CLK_SRC   SMCLK
// UCCKPH UCCKPL
#define SPI_UCSI_A1_CTL0_INIT (UCMSB | UCMST | UCMODE_0 | UCSYNC)

#define DAC_PUT_GET_BYTE(x)   SPI_UCSI_A1_Put_Get_Byte(x)

#define DAC_SENSOR1_WINDOW    0
#define DAC_SENSOR2_WINDOW    1
#define DAC_HEIZER_HIGH       2
#define DAC_HEIZER_LOW        3

#define DAC_SYNC_HIGH         P8OUT |=  BIT0;
#define DAC_SYNC_LOW          P8OUT &= ~BIT0;

//************************************************************************************************************************
//    Hier Definitionen für die Testpunkte
//************************************************************************************************************************

#define TP100_CLR             P5OUT &= ~BIT7
#define TP100_SET             P5OUT |=  BIT7
#define TP100_TOGGLE          P5OUT ^=  BIT7

#define TP101_CLR             P4OUT &= ~BIT0
#define TP101_SET             P4OUT |=  BIT0
#define TP101_TOGGLE          P4OUT ^=  BIT0

#define TP102_CLR             P4OUT &= ~BIT1
#define TP102_SET             P4OUT |=  BIT1
#define TP102_TOGGLE          P4OUT ^=  BIT1

//#define TP103_CLR             P8OUT &= ~BIT3
//#define TP103_SET             P8OUT |=  BIT3
//#define TP103_TOGGLE          P8OUT ^=  BIT3
//
//#define TP105_CLR             P9OUT &= ~BIT4
//#define TP105_SET             P9OUT |=  BIT4
//#define TP105_TOGGLE          P9OUT ^=  BIT4

//************************************************************************************************************************
//    Hier Definitionen für das Funk-Modul
//************************************************************************************************************************

#define RADIO_CS_BIT            BIT0
#define RADIO_CS_PORT           P9OUT
#define RADIO_ONOFF_BIT         BIT3
#define RADIO_ONOFF_PORT        P5OUT
#define RADIO_RESET_BIT         BIT2
#define RADIO_RESET_PORT        P5OUT

//#define RADIO_DATA_READY_BIT           BIT7         // nicht mehr benutzt beim LWM2M-Modul
//#define RADIO_DATA_READY_PORT          P4IN

#define RADIO_CS_LOW            RADIO_CS_PORT &= ~RADIO_CS_BIT
#define RADIO_CS_HIGH           RADIO_CS_PORT |= RADIO_CS_BIT
#define isRADIO_CS_HIGH        (RADIO_CS_PORT &  RADIO_CS_BIT )
#define isRADIO_CS_LOW         (!isRADIO_CS_HIGH)

#define RADIO_ON                RADIO_ONOFF_PORT &= ~RADIO_ONOFF_BIT
#define RADIO_OFF               RADIO_ONOFF_PORT |=  RADIO_ONOFF_BIT

#define RADIO_RESET_ON          RADIO_RESET_PORT &= ~RADIO_RESET_BIT
#define RADIO_RESET_OFF         RADIO_RESET_PORT |= RADIO_RESET_BIT

// Data-Ready ist hier high-aktive
#define isRADIO_DATA_READY      (RADIO_DATA_READY_PORT & RADIO_DATA_READY_BIT)

#define RADIO_RX_BUF_LEN         255                  // da Byte-Zeichen-Zähler ist 255 die max. Buffergröße
#define RADIO_TX_BUF_LEN         255
#define RADIO_UART_INDEX         2
#define RADIO_UART_BAUDRATE      BR115200


//==============================================================================================================================================
//    Define für UART-Radio
//==============================================================================================================================================
#define UCA2_USED
#define UCA2_LPM                 LPM3
#define UCA2_LPM_EXIT            LPM3_EXIT
#define UCA2_CLK_Source          SMCLK
#define UCA2_BAUDRATE_CLK        SMCLK_FREQ

#define UCA2_S_Buffer_len        RADIO_TX_BUF_LEN
#define UCA2_E_Buffer_len        RADIO_RX_BUF_LEN

//==============================================================================================================================================
// Das Radio_Task_DMA.c nutzt DMA4 und DMA5 um mit dem HSU-Funk-Modul über SPI zu kommunizieren
//==============================================================================================================================================
// nachfolgender Define hat keine Bedeutung mehr, da das Modul auf DMA-Ausgabe umgestellt wurde
//#define RADIO_PUT_GET_BYTE(x)   SPI_UCSI_A2_Put_Get_Byte(x)

// nun die Definitionen für die fürs Radio verwendeten SPI-Schnittstelle
// Achtung nur 250kBit/sec
//#define SPI_UCSI_A2_USED
//#define SPI_UCSI_A2_BR          (SMCLK_FREQ / 100000) /* 800000 für HSU-Funk */
//#define SPI_UCSI_A2_CLK_SRC     SMCLK
//#define SPI_UCSI_A2_CTL0_INIT   (UCCKPH | UCCKPL | UCMSB | UCMST | UCMODE_0 | UCSYNC)

//************************************************************************************************************************
//    Hier Definitionen für den EE893-Sensor (CO2) (UCB2)
//************************************************************************************************************************
#define EE893_POWER_BIT          BIT7
#define EE893_POWER_PORT         P9OUT
#define EE893_POWER_ON           EE893_POWER_PORT |=  EE893_POWER_BIT
#define EE893_POWER_OFF          EE893_POWER_PORT &= ~EE893_POWER_BIT




#endif /* _HARDWARE_INCLUDED */
