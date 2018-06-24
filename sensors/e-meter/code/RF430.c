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
#include "Hardware.h"
#include "RF430.h"
#include "I2C_USCI_Modul.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

uint8_t  mbNFC_IsInitialised=0;
uint16_t mwNDEF_Pos;
uint8_t  mbRecCounter = 0;
uint8_t  mbOutFlag = true;

const uint8_t mcbNDEF_Header_Data[] =
{
   0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01,          // NDEF Tag Application Name
   0xE1, 0x03, 											   	// Capability Container ID
   0x00, 0x0F,	                                      // CCLEN
   0x20,		                                          // Mapping version 2.0
   0x00, 0xF9,	                                      // MLe (49 bytes); Maximum R-APDU data size
   0x00, 0xF6,                                        // MLc (52 bytes); Maximum C-APDU data size
   0x04, 		                                       // Tag, File Control TLV (4 = NDEF file)
   0x06, 		                                       // Length, File Control TLV (6 = 6 bytes of data for this tag)
   0xE1, 0x04,	                                       // File Identifier
   0x0B, 0xDF,                                        // Max NDEF size (3037 bytes of useable memory)
   0x00, 		                                       // NDEF file read access condition, read access without any security
   0x00,  		                                       // NDEF file write access condition; write access without any security

   0xE1, 0x04,                                        // NDEF File ID
   0x00, 37,                                          // NDEF Length: D1...'F' =  37bytes
   0x95, 0x00, 0x01, 0x00                             // Binär-Daten länge 1 (letzte 0x00)
};

#define NDEF_TEXT_HEADER_TEXT_LEN_POS     2
#define NDEF_TEXT_HEADER_TEXT_LEN_OFFSET  3

const uint8_t mcbNDEF_Text_Header[] =
{
   0xD1, 0x01, 33, 		    									// NDEF Header MB=1, ME=1, CF=0, SR=1, IL=1, TNF=1 (Well-Known type); TypeLen=1Byte, 33 Datenbytes (33 = 1+2+13+17)
   'T',                                               // T = text
   0x02, 															// Länge der Länderkennung
   'e', 'n',                                          // Länderkennung = "en"
};

const uint8_t mcbNDEF_SN_ID[] =                        // SN_ID = Sensor-Netzwerk-ID = MAC / IP-Adresse
{
   0xE1, 0x04,                                        // NDEF File ID
   0x00, 37,                                          // NDEF Length: D1...'F' =  37bytes
   0xD1, 0x01, 33, 		    									// NDEF Header MB=1, ME=1, CF=0, SR=1, IL=1, TNF=1 (Well-Known type); TypeLen=1Byte, 33 Datenbytes (33 = 1+2+13+17)
   'T',                                               // T = text
   0x02, 															// Länge der Länderkennung
   'e', 'n',                                          // Länderkennung = "en"
   // emeter.BTADR als Stringkennung = als Feldtrenner dann Bluetoothadresse
   //1	2    3    4    5    6    7    8    9    10   11   12    13
   'e', 'm', 'e', 't', 'e', 'r', '.', 'B', 'T', 'A', 'D', 'R',  '=',
   // 17 Stellen: "00:17:E9:62:29:1F"                 // Bluetoothaddresse
   '0', '0', ':', '1', '7', ':', 'E', '9', ':', '6', '2', ':', '2', '9', ':', '1', 'F',
};

const uint8_t mcbNDEF_AAR[] =
{
   0xE1, 0x04, 	/* NDEF File ID */ 									               \
   0x00, 31,      /* NLEN; NDEF length 2+1+15+13 */ 					   			\
   0xD4,                                              // NDEF Header MB=1, ME=1, CF=0, SR=1, IL=1, TNF=4 (NFC-Forum external Type);
   15, 13,                                            // 15 Stellen für "android.com:pkg";  13 Stellen für "de.hsg.emeter"
   //1	2    3    4    5    6    7    8    9   10   11   12   13   14   15
   'a', 'n', 'd', 'r', 'o', 'i', 'd', '.', 'c', 'o', 'm', ':', 'p', 'k', 'g',
   'd', 'e', '.', 'h', 's', 'g', '.', 'e', 'm', 'e', 't', 'e', 'r'
};

// Reads the register at reg_addr, returns the result
unsigned int Read_Register(unsigned int reg_addr)
{
   BYTE bBuffer[] = {0, 0};

   RF430_READ_REG_ADR16(RF430_I2C_ADR, reg_addr, bBuffer, sizeof(bBuffer));

	return bBuffer[1] << 8 | bBuffer[0];
}

//writes the register at reg_addr with value
void Write_Register(unsigned int reg_addr, unsigned int value)
{
   BYTE TxData[2];
	TxData[1] = value >> 8;
	TxData[0] = value & 0xFF;

   RF430_WRITE_REG_ADR16(RF430_I2C_ADR, reg_addr, TxData, sizeof(TxData));
}

void Read_Continuous(unsigned int reg_addr, void *bBuffer, unsigned int data_length)
{
   RF430_READ_REG_ADR16(RF430_I2C_ADR, reg_addr, (BYTE*) bBuffer, data_length);
}

//writes the register at reg_addr and incrementing addresses with the data at "write_data" of length data_length
void Write_Continuous(unsigned int reg_addr, const void* write_data, unsigned int data_length)
{
   RF430_WRITE_REG_ADR16(RF430_I2C_ADR, reg_addr, (BYTE*) write_data, data_length);
}

void RF430_WriteNdefHeader()
{
   Write_Continuous(0, mcbNDEF_Header_Data, sizeof(mcbNDEF_Header_Data));
   mwNDEF_Pos = sizeof(mcbNDEF_Header_Data);
}

void RF430_SetNdefLen()
{
   uint8_t bBuf[] = { 0x00, 37,                       // NDEF Length: D1...'F' =  37bytes
                      0x95, 0x00, 0x01, 0x00 };       // Binär-Daten länge 1 (letzte 0x00)

   uint16_t wLen = mwNDEF_Pos - sizeof(mcbNDEF_Header_Data)+4;
   bBuf[0] = wLen >> 8;                               // Länge in NDEF-Daten eintragen
   bBuf[1] = wLen &  0xFF;
   mbRecCounter++;
   mbRecCounter &= 0x7F;
   bBuf[5] = mbRecCounter;                            // Oberstes Bit heißt Reader hat was neues in die NDEF-Daten geschrieben

   Write_Continuous(sizeof(mcbNDEF_Header_Data) -sizeof(bBuf), &bBuf, sizeof(bBuf));

   // 25.02.2016 ToDo: Testweise mal eingebaut - wieder entfernen wenn es nichts bringt
   Write_Continuous(2048, bBuf, 1);
}

void RF430_SetFirstNDEF_Rec_Pos(void)
{
   mwNDEF_Pos = sizeof(mcbNDEF_Header_Data);
}

void RF430_WriteText( const char *pText, uint8_t bNDEF_Flags )
{
   uint8_t bNDEF_Text_Header[ sizeof (mcbNDEF_Text_Header) ];
   uint16_t uiLen = strlen( (const char*) pText );

   memcpy( bNDEF_Text_Header, mcbNDEF_Text_Header, sizeof (mcbNDEF_Text_Header) );

   bNDEF_Text_Header[0] &= ~0xC0;
   bNDEF_Flags          &=  0xC0;
   bNDEF_Text_Header[0] |= bNDEF_Flags;

//   bNDEF_Text_Header[NDEF_TEXT_HEADER_LEN_POS]      = NDEF_TEXT_HEADER_LEN_OFFSET      + uiLen;
   bNDEF_Text_Header[NDEF_TEXT_HEADER_TEXT_LEN_POS] = NDEF_TEXT_HEADER_TEXT_LEN_OFFSET + uiLen;

   Write_Continuous(mwNDEF_Pos, bNDEF_Text_Header, sizeof(bNDEF_Text_Header));
   mwNDEF_Pos += sizeof(bNDEF_Text_Header);

   Write_Continuous(mwNDEF_Pos, pText, uiLen);
   mwNDEF_Pos += uiLen;
}


void RF430_WriteSingleText( const char *pText)
{
   Write_Register(CONTROL_REG, 0);
   RF430_WriteNdefHeader();
   RF430_WriteText( pText, NDEF_FLAG_ME);
   RF430_SetNdefLen();
   Write_Register(INT_FLAG_REG, 0xFF);                // CLR all pending interrupts
   Write_Register(CONTROL_REG, RF_ENABLE);
}

int RF430_ReadText( int iNDEF_Pos, BYTE *pbBuffer, int iBufLen)
{
   BYTE bBuf[5];
   int iLen = 0;
   mwNDEF_Pos = sizeof(mcbNDEF_Header_Data) + iNDEF_Pos;
   Read_Continuous(mwNDEF_Pos, bBuf, 5);              // Nach den Binärdaten erwarten wir das Text-Commando
   if (bBuf[1] == 1 && bBuf[3] == 'T')
   {
      mwNDEF_Pos += bBuf[4] + 5;
      iLen = bBuf[2] - bBuf[4] -1;                    // Gesamtlänge [1] - Länge der Länderkennung [4] - Byte der Länderkennungslänge
      if (iLen >= iBufLen)
         iLen = iBufLen;                              // Länge entweder durch Buffergröße oder durch NDEF-Stringlänge begrenzen

      Read_Continuous(mwNDEF_Pos, pbBuffer, iLen );   // Nach den Binärdaten erwarten wir das Text-Commando
   }
   return iLen;
}

void RF430_InterpretCMD(uint8_t bHandy)
{
   LED_OFF(BIT7);
   BYTE bBuf[40];                                     // Mal 40 Zeichen für PC-Befehl vorsehen
   int iLen;
   if (bHandy)
      iLen = RF430_ReadText(-4, bBuf, sizeof(bBuf) ); // versuche den Text vor den Binärdaten zu interpretieren (Handy ohne App)
   else
      iLen = RF430_ReadText(0, bBuf, sizeof(bBuf) );  // Versuche den Text nach den Binärdaten zu interpretieren (Servicematte, Handy mit App)

   if (iLen>0)
   {
      switch (bBuf[0])
      {
      case 'a':                                       // Get Mac-Adresse
         LED_ON(BIT7);
         snprintf( (char*)bBuf, sizeof(bBuf), "Funk-ID: %.2hhX:%.2hhX:%.2hhX:%.2hhX",
                  gbRadio_ID[0], gbRadio_ID[1], gbRadio_ID[2], gbRadio_ID[3] );
         RF430_WriteSingleText( (char*) bBuf);
         break;
      case 'n':                                       // Get Device Name
         LED_ON(BIT7);
         RF430_WriteSingleText("EMETER-3 Phase");
         break;
      case 'm':
         mbOutFlag = true;
         break;
      case 't':                                       // Get Sensor-Type
         LED_ON(BIT7);
         RF430_WriteSingleText("40: 3-Phase EMETER with N-current");
         break;
      case 'v':                                       // Get Version (Software Version)
         LED_ON(BIT7);
         RF430_WriteSingleText("EMETER V1.0");
         break;
      default:
         RF430_WriteSingleText("CMD unknown");
         break;
      }
   }
}

void RF430_PowerUp(void)
{
//   kick_watchdog();
   NFC_POWER_ON;
   NFC_I2C_ON;
   __delay_cycles( MCLK_FREQ * 0.03);                 // 30ms Warten, bis Spannung stabil und Tag ready

   while((NGC_I2C_IN & NFC_I2C_BITS) != NFC_I2C_BITS) // Warte bis I²C-Port high ist
     _NOP();

   RF430_RESET_I2C;                                   // Bring das I²C in Grundzustand

   WORD wCounter = 100;
	while(!(Read_Register(STATUS_REG) & READY) && wCounter)     // Warte bis Tag Ready meldet
   {
      __delay_cycles((uint32_t) (MCLK_FREQ * 0.001)); // 1ms Warten bevor man es erneut probiert
      wCounter--;                                     // wait until READY bit has been set
//      kick_watchdog();
   }

   if (wCounter)
   {
      {
         /****************************************************************************/
         /* Errata Fix : Unresponsive RF - recommended firmware                      */
         /****************************************************************************/
         // Please implement this fix as given in this block.  It is important that
         // no line be removed or changed.
         unsigned int version;
         version = Read_Register(VERSION_REG);        // read the version register.  The fix changes based on what version of the
                                                      // RF430 is being used.  Version C and D have the issue.  Next versions are
                                                      // expected to have this issue corrected
                                                      // Ver C = 0x0101, Ver D = 0x0201
         if (version == 0x0101 || version == 0x0201)
         {	// the issue exists in these two versions
            Write_Register(0xFFE0, 0x004E);
            Write_Register(0xFFFE, 0x0080);
            if (version == 0x0101)
            {  // Ver C
               Write_Register(0x2a98, 0x0650);
            }
            else
            {	// Ver D
               Write_Register(0x2a6e, 0x0650);
            }
            Write_Register(0x2814, 0);
            Write_Register(0xFFE0, 0);
         }
         //Upon exit of this block, the control register is set to 0x0
      }

      // enable RF
//      RF430_WriteNdefHeader();
//      RF430_WriteText("Hallo World", NDEF_FLAG_MB);
//      RF430_WriteText("I'm here",    NDEF_FLAG_ME);
//      RF430_SetNdefLen();
//      Write_Register(CONTROL_REG, RF_ENABLE);

//      mbNFC_IsInitialised = true;
   }
}

void RF430_Init(void)
{
   // Ports initalisieren
   WDTCTL = WDTPW + WDTHOLD;

   NFC_POWER_OFF;
   NFC_POWER_DS;                                      // Drive-Strength des Power-Ausgangs setzen

   RF430_INT_DIR &= ~RF430_INT_BITS;
   RF430_IFG     &= ~RF430_INT_BITS;
   RF430_REN     &= ~RF430_INT_BITS;
   RF430_IES     |=  RF430_INT_BITS;                  // Interrupt bei high-low flanke

   RF430_I2C_INIT(100000);                            // I²C initalisieren (100kBaud)
}

void RF430_UpdateEmeterPhase(const struct phase_parms_s *pst_phase_parms, uint8_t bPhaseNumber)
{
   char buffer[40];

//   RF430_SetFirstNDEF_Rec_Pos();
//   Write_Register(CONTROL_REG, 0);
   RF430_WriteNdefHeader();
   snprintf( buffer, sizeof(buffer), "Phase: %i", bPhaseNumber);
   RF430_WriteText( buffer, NDEF_FLAG_NONE);
   snprintf( buffer, sizeof(buffer), "U rms: %f [V]", (float) pst_phase_parms->readings.V_rms/100);
   RF430_WriteText( buffer, NDEF_FLAG_NONE);
   snprintf( buffer, sizeof(buffer), "I rms: %f [A]", (float) pst_phase_parms->readings.I_rms/1000*IRMS_E_GAIN_FACTOR);
   RF430_WriteText( buffer, NDEF_FLAG_NONE);
   snprintf( buffer, sizeof(buffer), "Frequenz: %f [Hz]", (float) pst_phase_parms->readings.frequency/100);
   RF430_WriteText( buffer, NDEF_FLAG_NONE);
   snprintf( buffer, sizeof(buffer), "Wirkleistung: %f [W]", (float) pst_phase_parms->readings.active_power/100*IRMS_E_GAIN_FACTOR);
   RF430_WriteText( buffer, NDEF_FLAG_NONE);
   snprintf( buffer, sizeof(buffer), "Blindleistung: %f [W]", (float) pst_phase_parms->readings.reactive_power/100*IRMS_E_GAIN_FACTOR);
   RF430_WriteText( buffer, NDEF_FLAG_NONE);
   snprintf( buffer, sizeof(buffer), "Scheinleistung: %f [W]", (float) pst_phase_parms->readings.apparent_power/100*IRMS_E_GAIN_FACTOR);
   RF430_WriteText( buffer, NDEF_FLAG_NONE);
   snprintf( buffer, sizeof(buffer), "CosPh: %f", (float) pst_phase_parms->readings.power_factor/10000);
   RF430_WriteText( buffer, NDEF_FLAG_ME);
   RF430_SetNdefLen();
   Write_Register(INT_FLAG_REG, 0xFF);                // CLR all pending interrupts
   Write_Register(CONTROL_REG, RF_ENABLE);

//   __delay_cycles( MCLK_FREQ * 0.0015);         // zwischen 1 und 2 ms warten (siehe Datasheet Page 36)
//   int i=0;
//   while (i==0)
//      i=Read_Register( INT_FLAG_REG );
}

void RF430_UpdateEmeter(const struct phase_parms_s pst_phase_parms[3] )
{
   WDTCTL = WDTPW + WDTHOLD;

   static uint8_t iSkip=0;                            // Zähler für übersprungene Updates
   static uint8_t uiPhaseIndex = 0;

   if (isNFC_RF_FIELD_ON)
      LED_ON(BIT4);
   else
      LED_OFF(0x70);

   if (mbOutFlag == false)
   {
      iSkip = 30;                                     // 30 Sekunden die Anwort auf TAG-CMD im TAG belassen
      mbOutFlag = true;
      return;                                         // Ausgabe der Daten wurde abgeschaltet -> Return
   }

   if (iSkip==0)
   {                                                  // Auskommentiert da immer Busy wenn Feld da
      if (isNFC_RF_INT_FLG_SET || isNFC_RF_FIELD_ON)
      {
         LED_ON(BIT5);
         RF430_PowerUp();
         //Write_Register(CONTROL_REG, 0);
         RF430_UpdateEmeterPhase(&pst_phase_parms[uiPhaseIndex], uiPhaseIndex+1);
         uiPhaseIndex++;
         if (uiPhaseIndex == 3) uiPhaseIndex=0;
         LED_OFF(BIT5);
         mbNFC_IsInitialised = true;
         iSkip = 5;
      }
   }
   else
   {
      iSkip--;
      if (iSkip==1)
      {
         Write_Register(CONTROL_REG, 0);
         if (mbNFC_IsInitialised)
         {
            mbNFC_IsInitialised = false;
            LED_ON(BIT4);
            NFC_POWER_OFF;
            RF430_IFG &= ~RF430_INT_BITS;
         }
      }
      else
         LED_TOGGLE(0x80);
   }
}

void RF430_Task(void)
{
   if (mbNFC_IsInitialised)
   {
      BYTE bData = Read_Register(sizeof(mcbNDEF_Header_Data)-1 );
      if (bData != mbRecCounter)
      {
         LED_ON(BIT6);                                // Jemand hat in den Speicher geschrieben
         if ( bData == 0x81)
         {
            mbOutFlag = false;
            RF430_InterpretCMD( false );              // Servicematte / Handy-App hat ein Kommando gesendet
         } else if ( bData < 0x80)
         {
            mbOutFlag = false;
            RF430_InterpretCMD( true );               // Handy ohne App hat ein Kommando gesendet
         }
      }
      else
         LED_OFF(BIT6);
   }
   else
      LED_OFF(BIT6);
}