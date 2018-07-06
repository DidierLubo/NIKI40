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
//*  Vorlageprojekt MSP430F5528 mit USB, abgeleitet aus ??                                                               *
//*  Erstellt am 03.03.2016                                                                                              *
//************************************************************************************************************************
//*                                                                                                                      *
//*  EIS - Enviroment Information System                                                                                 *
//*                                                                                                                      *
//*  Misst:                                                                                                              *
//*  Druck / Feuchte / Temperatur (BME280)                                                                               *
//*  CO2 (2000ppm) (EE893)                                                                                               *
//*  Licht Helligkeit in LUX + Infrarot + UV-Index (Si1132)                                                              *
//*  2D-Flow [m/s] Hahn-Schickard-Sensor                                                                                 *
//*                                                                                                                      *
//*  Ausgabe der Werte über USB / NFC (RF430) / HSU-Radio / OLED-Display                                                 *
//*                                                                                                                      *
//*  Belegte DMA-Kanäle:                                                                                                 *
//*  DMA0: USB                                                                                                           *
//*  DMA4: HSU-Radio TX                                                                                                  *
//*  DMA5: HSU-Radio RX                                                                                                  *
//*                                                                                                                      *
//*  Timer:                                                                                                              *
//*  TA0: für LED-Blinken über CCRx Register (PWM) vorgesehen.                                                           *
//*  TA1: liefert ca. 1ms-Tick-Counter für HAL_GetTickCount                                                              *
//*                                                                                                                      *
//*  I2C:                                                                                                                *
//*  I2C_B0:  BQ27200 / BME280 / Si1132                                                                                  *
//*  I2C_B1:  RF430                                                                                                      *
//*  I2C_B2:  EE893 - wegen besonders langsamen I2C-Schnittstelle und den vielen Adressen                                *
//*                                                                                                                      *
//*  SPI:                                                                                                                *
//*  SPI_A2:  HSU-Funkmodul. Nutzt DMA um die Daten zu senden/empfangen - SOI_USCI_Modul.c nur für init genutzt          *
//*                                                                                                                      *
//*  ADC12 / DAC12                                                                                                       *
//*  ADC-0..3 FLow-Sensor Signale                                                                                        *
//*  DAC-0    FLow-Sensor Heizer                                                                                         *
//*                                                                                                                      *
//************************************************************************************************************************
//*                                                                                                                      *
//*  Wünsche in neuer Hard-Ware-Config:                                                                                  *
//*                                                                                                                      *
//*  WPG Wireless-Power-Good-Signal um NFC bei Wireless-Power nicht einzuschalten                                        *
//*  Schalter am Power-Stecker nutzen um Lader auf voll Power zu schalten                                                *
//*                                                                                                                      *
//************************************************************************************************************************
//*                                                                                                                      *
//*  Version 1.2 vom 7.7.2016                                                                                            *
//*  OLED-Flow-Anzeige hatte in Version 1.1 einen um Faktor 10 zu hohen Wert angezeigt. Dies wurde hier behoben          *
//*                                                                                                                      *
//************************************************************************************************************************

#include "Hardware.h"
#include "Globals.h"

// hier die Projektspezifischen includ's

#include "OLED.h"
#include "I2C_USCI_Modul.h"

#include "RF430.h"
#include "SPI_UCSI_Modul.h"
#include "BQ27421-G1.h"

#include "BME280.h"
#include "Si1132.h"
#include "2D_Flow.h"

#include "LWM2M.h"
#include "Radio_Task.h"

#include "Task_Manager.h"

#include <String.h>
#include <stdio.h>

#include "USB_PC_Com.h"                               // USB-PC- Com- Interpreter
#include "USB_Common/usb.h"                           // USB-specific functions

#include "HAL_FLASH.h"
#include "HAL_UCS.h"
#include "HAL_PMM.h"



/* The following parameters are used when configuring HCILL Mode.    */
#define HCILL_MODE_INACTIVITY_TIMEOUT              (500)
#define HCILL_MODE_RETRANSMIT_TIMEOUT              (100)

ST_SYS gSYS;                                          // Konfigurationsbereich im RAM
__no_init ST_SYS mFlashSYS @ "INFO" ;                 // Konfigurationsbereich im Flash

ST_SYS mstSYS_DefaultValue =
{
   .ulPressureMessInterval    = 1024l*2,              // Testweise alle 2 Sekunden

   .ulLightMessInterval       = 1024,                 // Messung jede Sekunde
   .bLightMessAnzahl          = 40,
   .stSi1132_CalVal =
   {
      .fIR_Scale              =  0.08f,               // Werte aus AN523  Seite 4
      .fVIS_Scale             =  5.41f,
      .wIR_ADC_DarkValue      =  251,
      .wVIS_ADC_DarkValue     =  258,
   },

   .ulFlowMessInterval        = 1024l * 3l,           // Messung alle 3 Sekunden
   .bFlowMessAnzahl           = 40,
   .uiFlowDAC_Heizer          = 3400,

   .ulEE893_MessInterval      = 1024l*60l*30l,        // Alle 30 Minuten Messen

   //               1234567890
   .bDeviceName =  "EIS-HS-01",
   .bSerienNr   =  "EIS-00001",

   .bRadioType = RADIO_TYPE_HSU,

   .stLWM2M_SET =
   {
      .bBS_SRV_IP       = LWM2M_DEFAULT_BS_SRV_IP,    // 2001::AA IP address of the LWM2M bootstrap server.*
      .wBS_SRV_PORT     = LWM2M_DEFAULT_BS_SRV_PORT,  // 5683 Port of the LWM2M bootstrap server.*
      .bSRV_IP          = LWM2M_DEFAULT_SRV_IP,       // 2001::BB IP address of the LWM2M bootstrap server.*
      .wSRV_PORT        = LWM2M_DEFAULT_SRV_PORT,     // 5683 Port of the LWM2M bootstrap server.*
      .strCLI_NAME      = LWM2M_DEFAULT_CLI_NAME,
   },

   .stCC1350_SET =
   {
      .bMAC_ADR         = CC1350_DEFAULT_MAC_ADR,     // GAS-Parameter
      .wPANID           = CC1350_DEFAULT_PANID,
      .bOPMODE          = CC1350_DEFAULT_OPMODE,
      .bCHANNEL         = CC1350_DEFAULT_CHANNEL,
   }
} ;

volatile BYTE mbFlashCounter  = 0;                    // Speicher um Blinkzeichen zu geben
         BYTE mbFlashLED_Mask = 0;

int      ServerPortID;                                // Variable which contains the Server-ID bei Init_SPP_Spirometer


const BYTE gbVersion[] = "Esima: EIS - Version 2.3 vom " __DATE__ " time " __TIME__ "\r\n";
BYTE sizeof_gbVersion (void)
{
   return sizeof(gbVersion);
}

//------------------------------------------------------------------------------------------------------------------------
//    LED_flash:           Lässt die in der 'bLED_Mask' angegeben LED's 'bFlashCount' mal blinken.
//------------------------------------------------------------------------------------------------------------------------
//    bLED_Mask:           LED_GREEN | LED_ORANGE | LED_RED oder beliebe Kombination daraus
//------------------------------------------------------------------------------------------------------------------------
void LED_flash(BYTE bLED_Mask, BYTE bFlashCount)
{
   mbFlashCounter = (bFlashCount << 1) -1;
   mbFlashLED_Mask    = bLED_Mask & LED_MASK;
   LED_PORT &= ~mbFlashLED_Mask;

   TA0CCTL0 = CCIE;                                   // CCR0 interrupt enabled
   TA0CCR0  = (WORD) (ACLK_FREQ * 0.3);               // 0,3s ~ 1,5Hz
   TA0CTL   = TASSEL_1 | MC_1 | TACLR;                // ACLK, Up-Mode
}

//------------------------------------------------------------------------------------------------------------------------
//    TIMER0_A0:          Timer-IRQ um LED's blinken zu lasssen
//------------------------------------------------------------------------------------------------------------------------
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER0_A0(void)
{
   if( mbFlashCounter )
   {
      mbFlashCounter--;                               // Blickzähler erniedrigen
      LED_PORT ^=  mbFlashLED_Mask;                   // LED's blinken deruch XOR mit Maske
   }
   else
   {
      LED_PORT &=  ~mbFlashLED_Mask;                   // Alle LED's aus
      TA0CCTL0  = 0;                                  // IRQ aus
      TA0CTL    = 0;                                  // Timer aus
      LPM4_EXIT;                                      // um in main() evtl. in LPM4 gehen zu können
   }
}

//------------------------------------------------------------------------------------------------------------------------
// Ports nach Systemstart initialisieren
//------------------------------------------------------------------------------------------------------------------------
void PortInit(void)
{

   P1OUT = DP1OUT;
   P1DIR = DP1DIR;
   P1REN = DP1REN;
   P1DS  = DP1DS;
   P1SEL = DP1SEL;
   P1IFG = 0x00;
   P1IES = DP1IES;
   P1IE  = DP1IE;

   PMAPKEYID = PMAPKEY;
   P2MAP0 = P2MAP0_VAL;
   P2MAP1 = P2MAP1_VAL;
   P2MAP2 = P2MAP2_VAL;
   P2MAP3 = P2MAP3_VAL;
   P2MAP4 = P2MAP4_VAL;
   P2MAP5 = P2MAP5_VAL;
   P2MAP6 = P2MAP6_VAL;
   P2MAP7 = P2MAP7_VAL;
   PMAPKEYID = 0;   // Illegal Key = Write-Protection on siehe recommendadtion Family-Handbook 11.2.1 page 334

   P2OUT = DP2OUT;
   P2DIR = DP2DIR;
   P2REN = DP2REN;
   P2DS  = DP2DS;
   P2SEL = DP2SEL;
   P2IFG = 0x00;
   P2IES = DP2IES;
   P2IE  = DP2IE;

   P3OUT = DP3OUT;
   P3DIR = DP3DIR;
   P3REN = DP3REN;
   P3DS  = DP3DS;
   P3SEL = DP3SEL;
   P3IFG = 0x00;
   P3IES = DP3IES;
   P3IE  = DP3IE;

   P4OUT = DP4OUT;
   P4DIR = DP4DIR;
   P4REN = DP4REN;
   P4DS  = DP4DS;
   P4SEL = DP4SEL;
   P4IFG = 0x00;
   P4IES = DP4IES;
   P4IE  = DP4IE;

   P5OUT = DP5OUT;
   P5DIR = DP5DIR;
   P5REN = DP5REN;
   P5DS  = DP5DS;
   P5SEL = DP5SEL;

   P6OUT = DP6OUT;
   P6DIR = DP6DIR;
   P6REN = DP6REN;
   P6DS  = DP6DS;
   P6SEL = DP6SEL;

   P7OUT = DP7OUT;
   P7DIR = DP7DIR;
   P7REN = DP7REN;
   P7DS  = DP7DS;
   P7SEL = DP7SEL;

   P8OUT = DP8OUT;
   P8DIR = DP8DIR;
   P8REN = DP8REN;
   P8DS  = DP8DS;
   P8SEL = DP8SEL;

   P9OUT = DP9OUT;
   P9DIR = DP9DIR;
   P9REN = DP9REN;
   P9DS  = DP9DS;
   P9SEL = DP9SEL;

   PJOUT = DPJOUT;
   PJDIR = DPJDIR;
   PJREN = DPJREN;
   PJDS  = DPJDS;
}

//------------------------------------------------------------------------------------------------------------------------
// Berechnet das CRC-Zeichen
//------------------------------------------------------------------------------------------------------------------------
// pData             Zeiger auf die Daten
// bLen              Anzahl der Bytes
//------------------------------------------------------------------------------------------------------------------------
WORD get_CRC(void __data20 *pData, BYTE bLen)
{
   BYTE __data20 *pB  = pData;
   CRCINIRES = CRC_START_VALUE;
   while (bLen)
   {
      CRCDI = *pB++;
      bLen--;
   }
   return CRCINIRES;
}

//------------------------------------------------------------------------------------------------------------------------
//  Speichert die aktuellen RAM-Daten im Flash-Speicher ab
//------------------------------------------------------------------------------------------------------------------------
void StoreFlashData(void)
{
   gSYS.wCRC = get_CRC ((BYTE __data20*)&gSYS, sizeof(gSYS)-2 );  // Benutzt Hardware CRC- Modul
   uint8_t *pbSegment = (uint8_t*) &mFlashSYS;
   uint8_t *pbEnd     = (uint8_t*) &mFlashSYS + sizeof(mFlashSYS);
   while(pbSegment < pbEnd)
   {
      Flash_SegmentErase( pbSegment );                // alle von mFlashSYS belegten Segmente löschen
      pbSegment+=64;                                  // Achtung max. 256-Byte = 4 Segmente!!!!
   }
   Flash_Write ( &gSYS, &mFlashSYS, sizeof(gSYS) );   // Und nun die Parameter in den Flash-Speicher übertragen
}

//------------------------------------------------------------------------------------------------------------------------
//  Flash-Speicher ins Ram laden
//------------------------------------------------------------------------------------------------------------------------
void init_SystemParameter (void)
{
   if (get_CRC(&mFlashSYS, sizeof(mFlashSYS)-2) == mFlashSYS.wCRC)
   {                                                  // Wenns CRC stimmt dann die Daten aus dem Flash-Speicher ins RAM kopieren
      memcpy ( &gSYS , &mFlashSYS, sizeof(gSYS));
   }
   else
   {                                                  // Ok im Flash-Speicher steht ein Schrott - Standardwerte setzen
      memcpy ( &gSYS , &mstSYS_DefaultValue, sizeof(gSYS));
   }
}

//------------------------------------------------------------------------------------------------------------------------
// Main-Programm: Inialisiert alles, checkt auf UART-Befehle und geht evtl. in Automatik-Messmode
//------------------------------------------------------------------------------------------------------------------------
__noreturn main( void )
{
   // Stop watchdog timer to prevent time out reset
   WDTCTL = WDTPW + WDTHOLD;

   PortInit();

   __delay_cycles( 100000 );

   Ucs_Start_XT1();
   UcsUse_DCO();

   init_SystemParameter();

   __delay_cycles(MCLK_FREQ * 0.1);                   // 0,1sec = 100ms warten bis 3V Spannung stabil

   Init_Task_Manager();
   __enable_interrupt() ;

   LED_ALL_ON;

   OLED_Init();
   OLED_Set_Position(0, 0);
   //                     12345  1234567890
   OLED_WriteDoubleSize( "Hahn-\rSchickard", 255, false );
   OLED_Set_Position(0, 5);
   OLED_WriteDoubleSize( "ESIMA-EIS", 255, false );
   //__delay_cycles(MCLK_FREQ * 3);                     // Text 3 Sekunden anzeigen

   OLED_Clear();

   LED_GE_OFF;

   I2C_B0_Init(100000l);
   AKKU_INIT();                                       // Akku-Überwachung

   BME280_Init( gSYS.ulPressureMessInterval );
   Si1132_Init( gSYS.ulLightMessInterval, gSYS.bLightMessAnzahl );
   Flow_Init  ( gSYS.ulFlowMessInterval,  gSYS.bFlowMessAnzahl, gSYS.uiFlowDAC_Heizer);

#ifdef USE_MSP430F6658
   EE893_Init ( gSYS.ulEE893_MessInterval );
#endif

   RF430_Init();                                      // I2C init aber Ports auf OUT-LOW um Strom zu sparen

//   P9SEL = 0x6E;
   SPI_UCSI_Init();

   Radio_Task_Init();
//   if (gSYS.bRadioType == RADIO_TYPE_HSU)
//   {
//      Radio_Set_FunkID( gSYS.bRadioID );              // Zunächst die Funkid-Setzen und dann zurücklesen
//      Radio_Get_FunkID( gSYS.bRadioID );
//   }

   LED_ALL_OFF;

//
//   // Flowsensor ein:
//   //CHIP_HEIZUNG_ON;
//   FLOW_HEIZER_ON;
//
   OLED_UpdateStatusTextNo(IDX_TEXT_Main_SystemReady);

   USB_init();

   //Enable various USB event handling routines
   USB_setEnabledEvents(
        kUSB_VbusOnEvent       + kUSB_VbusOffEvent    + kUSB_receiveCompletedEvent
      + kUSB_dataReceivedEvent + kUSB_UsbSuspendEvent + kUSB_UsbResumeEvent
      + kUSB_UsbResetEvent ) ;

   //See if we're already attached physically to USB, and if so, connect to it
   //Normally applications don't invoke the event handlers, but this is an exception.
   if (USB_connectionInfo() & kUSB_vbusPresent)
   {
      UcsUse_XTAL2(XT2DRIVE_3);                       // 19,2MHz Quarz anschalten
      if (USB_enable() == kUSB_succeed)
      {
         USB_reset();
         USB_connect();
         OLED_On( OLED_NO_TIMEOUT );
      }
   }

   Task_Manager();
}
