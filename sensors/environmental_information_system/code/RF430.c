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

#include "Hardware.h"
#include "RF430.h"

#include "OLED.h"

#include "Globals.h"
#include "main.h"

#include "I2C_USCI_Modul.h"
#include "BQ27421-G1.h"
#include "Radio_Task.h"
#include "IPv6.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>

// \xC2 = Escape für Sonderzeichen von 0x80 ... 0xBF
// \xC3 = Escape für Sonderzeichen von 0x80 ... 0xBF

uint16_t mwNDEF_Pos;

uint8_t  mbRecCounter = 0;

// Defines für OutFlag
#define OUT_FLG_NEW_BME280_DAT   BIT0
#define OUT_FLG_NEW_Si1132_DAT   BIT1
#define OUT_FLG_NEW_EE893_DAT    BIT2
#define OUT_FLG_NEW_FLOW_DAT     BIT3
#define OUT_FLG_NEW_AKKU_DAT     BIT4
#define OUT_FLG_NEW_DATA_MASK    0x1F

#define OUT_FLG_FAST_MODE        BIT5
#define OUT_FLG_ALL_OUT          BIT6
#define OUT_FLG_MESS_OUT         BIT7

uint8_t  mbOutFlag = OUT_FLG_MESS_OUT | OUT_FLG_ALL_OUT;
//uint8_t  mbOutFlag = 0;

ST_FLOW_MESS_DATA    *mpstFlow       = 0;
ST_Si1132_MessData   *mpstSi1132     = 0;
ST_BME280_MESS_DATA  *mpstBME280     = 0;
ST_EE893_MESS_DATA   *mpstEE893      = 0;

uint8_t              mbRF430_Akku_Value;              // hier der Speicher für den aktuellen Akku-Ladezustand

static uint8_t mbRF430_Buffer[1000];

const uint8_t mcbNDEF_Header_Data[] =
{
   0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01,          // NDEF Tag Application Name
   0xE1, 0x03, 											   	// Capability Container ID
   0x00, 0x0F,	                                       // CCLEN
   0x20,		                                          // Mapping version 2.0
   0x00, 0xF9,	                                       // MLe (49 bytes); Maximum R-APDU data size
   0x00, 0xF6,                                        // MLc (52 bytes); Maximum C-APDU data size
//   0x00, 0x3B,	                                       // MLe ausgelesen aus CHIP; Maximum R-APDU data size
//   0x00, 0x34,                                        // MLc ausgelesen aus CHIP; Maximum C-APDU data size
   0x04, 		                                       // Tag, File Control TLV (4 = NDEF file)
   0x06, 		                                       // Length, File Control TLV (6 = 6 bytes of data for this tag)
   0xE1, 0x04,	                                       // File Identifier
   0x0B, 0xDF,                                        // Max NDEF size (3037 bytes of useable memory)
//   0x00, 0xFF,                                        // Max NDEF size (3037 bytes of useable memory)
//   0x0B, 0xD0,                                        // Max NDEF size (3024 bytes of useable memory)

//   0x03, 0x66,                                        // Max NDEF size 868+2?

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

const uint8_t mcbNDEF_SN_ID[] =                       // SN_ID = Sensor-Netzwerk-ID = MAC / IP-Adresse
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

const char mbCMD_unknown[] = "CMD unknown";
const char mbDone[]        = "Done";


EN_TASK_STATUS RF430_Task_WriteNdefDatenFastMode(void);

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
   // Wenn der erste Datensatz (0x95....) vorhanden ist
   uint8_t bBuf[] = { 0x00, 37  ,                     // NDEF Length: D1...'F' =  37bytes
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

//   wenn der erste Datensatz nicht 0x95 ist
//   uint8_t bBuf[] = { 0x00, 37 };                     // NDEF Length: D1...'F' =  37bytes
//   uint16_t wLen = mwNDEF_Pos - sizeof(mcbNDEF_Header_Data);
//   bBuf[0] = wLen >> 8;                               // Länge in NDEF-Daten eintragen
//   bBuf[1] = wLen &  0xFF;
//   Write_Continuous(sizeof(mcbNDEF_Header_Data) -sizeof(bBuf), &bBuf, sizeof(bBuf));
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
//   RF430_WriteText( pText, NDEF_FLAG_MB | NDEF_FLAG_ME);       // ToDo:  NDEF_FLAG_MB entfernen
   RF430_WriteText( pText, NDEF_FLAG_ME);
   RF430_SetNdefLen();
//   Write_Register(INT_FLAG_REG, 0xFF);                // CLR all pending interrupts
//   Write_Register(CONTROL_REG, RF_ENABLE);
}

int RF430_ReadText( int iNDEF_Pos, BYTE *pbBuffer, int iBufLen)
{
   BYTE bBuf[5];
   int iLen = 0;
   mwNDEF_Pos = sizeof(mcbNDEF_Header_Data) + iNDEF_Pos -4;    // Unser BIN-Datensatz mit 4 Byte sollte verwschwunden sein
   Read_Continuous(mwNDEF_Pos, bBuf, 5);              // Nach dem NDEF-Header erwarten wir die Text-Daten
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

void RF430_ReadMemory(void)
{
   Read_Continuous(0, mbRF430_Buffer, 200);           // Nach den Binärdaten erwarten wir das Text-Commando
}

//===========================================================================================
//    Liest einen String aus dem NFC-Record - falls er zu lang ist wird er gekürtzt
//===========================================================================================
//    pData:         Zeiger auf die NFC-Daten (z.B. set:xxxx) Zeiger auf xxxx 0-terminiert!!
//    pgSYS_Value:   Zeiger auf uin8_t (String) wo die Daten hingeschrieben werden sollen
//    iMaxStrLen:    Maximale String-Länge (ohne 0Byte) ( sizeof(Array)-1 )
//===========================================================================================
//    Return:        0=ok                                   - noch keine NFC-Antwort gegeben
//===========================================================================================
uint32_t getStringValue( char *pData, char *pbString, uint32_t iMaxStrLen )
{                                                     // der kleinere Wert gewinnt
   while(*pData && iMaxStrLen)                        // solange in NDEF-Daten kein 0-Byte und max Len nicht erreicht
   {
      *pbString++ = *pData++;                         // Daten ins Speicher kopieren
      iMaxStrLen--;
   }
   *pbString = 0;                                     // Nullbyte anfügen (Sring-Termination)
   return 0;
}


//------------------------------------------------------------------------------------------------------------------------
// RF430_Send_int32_ASCII_Data: gibt eine int32-Zahl mit Beschreibung und Einheit als ASCII-String aus
//------------------------------------------------------------------------------------------------------------------------
void RF430_Send_int32_ASCII_Data(BYTE *pbDiscription, int32_t value, BYTE *pbUnit)
{
   char bBuffer[80];
   int iLen = snprintf( bBuffer, sizeof bBuffer,  "%s: %li [%s]",
                       pbDiscription,
                       value,
                       pbUnit );
   RF430_WriteSingleText( bBuffer );
}

//------------------------------------------------------------------------------------------------------------------------
// RF430_Send_int32_ASCII_Data: gibt einen String mit Beschreibung aus
//------------------------------------------------------------------------------------------------------------------------
void RF430_SendStringValue( char *pcDiscription, char *pcString)
{
   char bBuffer[80];
   int iLen = snprintf( bBuffer, sizeof bBuffer,  "%s: %s",
                       pcDiscription,
                       pcString );
   RF430_WriteSingleText( bBuffer );
}

//------------------------------------------------------------------------------------------------------------------------
// RF430_Send_RadioMac: gibt die Mac-Adresse aus
//------------------------------------------------------------------------------------------------------------------------
//void RF430_Send_RadioMac(void)
//{
//   char bBuffer[80];
////   Radio_Get_FunkID( gSYS.bRadioID );                 // aktuelle Funk-ID aus Funk-Modul auslesen
//   int iLen = snprintf( bBuffer, sizeof bBuffer,  "Funk ID: %02hhX:%02hhX:%02hhX:%02hhX",
//                        gSYS.bRadioID[0], gSYS.bRadioID[1], gSYS.bRadioID[2], gSYS.bRadioID[3] );
//   RF430_WriteSingleText( bBuffer );
//}

//------------------------------------------------------------------------------------------------------------------------
// RF430_Send_Text: gibt einen Text mit Beschreibung aus. (z. B. Device-Name)
//------------------------------------------------------------------------------------------------------------------------
void RF430_Send_Text(BYTE *pbDiscription, BYTE *pValue )
{
   char bBuffer[80];
   int iLen = snprintf( bBuffer, sizeof bBuffer,  "%s: %s",
                        pbDiscription,  pValue);
   RF430_WriteSingleText( bBuffer );
}

//=================================================================================================================
//    Start der Augabefunktionen
//=================================================================================================================

void RF430_BME280_Out( uint8_t bNDEF_Flg )
{
   mbOutFlag &= ~OUT_FLG_NEW_BME280_DAT;
   char buffer[40];
   snprintf( buffer, sizeof(buffer), "Druck:      %lu [Pa]", mpstBME280->u32Pressure );
   RF430_WriteText( buffer, NDEF_FLAG_NONE);
   // \xC2 = Escape für Sonderzeichen von 0x80 ... 0xBF
   // \xC3 = Escape für Sonderzeichen von 0x80 ... 0xBF
   snprintf( buffer, sizeof(buffer), "Temperatur: %.2f [\xC2°C]", ((float) mpstBME280->i32Temp) * 0.01f );
   RF430_WriteText( buffer, NDEF_FLAG_NONE);
   snprintf( buffer, sizeof(buffer), "Feuchte:    %lu [%%]", mpstBME280->u32Humity >> 10 );
   RF430_WriteText( buffer, bNDEF_Flg);
}

void RF430_Si1132_Out( uint8_t bNDEF_Flg )
{
   mbOutFlag &= ~OUT_FLG_NEW_Si1132_DAT;
   char buffer[40];
   snprintf( buffer, sizeof(buffer), "Licht avg: %hu [lux]", mpstSi1132->wAvgVIS );
   RF430_WriteText( buffer, NDEF_FLAG_NONE);
   snprintf( buffer, sizeof(buffer), "Licht min: %hu [lux]", mpstSi1132->wMinVIS );
   RF430_WriteText( buffer, NDEF_FLAG_NONE);
   snprintf( buffer, sizeof(buffer), "Licht max: %hu [lux]", mpstSi1132->wMaxVIS );
   RF430_WriteText( buffer, NDEF_FLAG_NONE);
   snprintf( buffer, sizeof(buffer), "Infrarot avg: %hu []", mpstSi1132->iAvgIR );
   RF430_WriteText( buffer, NDEF_FLAG_NONE);
   snprintf( buffer, sizeof(buffer), "UV-Index: %hu []", mpstSi1132->wAvgUV_Index );
   RF430_WriteText( buffer, bNDEF_Flg);
}

void RF430_Flow_Out( uint8_t bNDEF_Flg )
{
   mbOutFlag &= ~OUT_FLG_NEW_FLOW_DAT;
   char buffer[40];
   snprintf( buffer, sizeof(buffer), "Flow X: %.2f [m/s]", (float) mpstFlow->iFlowX_avg * 0.01f);
   RF430_WriteText( buffer, NDEF_FLAG_NONE);
   snprintf( buffer, sizeof(buffer), "Flow Y: %.2f [m/s]", (float) mpstFlow->iFlowY_avg * 0.01f);
   RF430_WriteText( buffer, bNDEF_Flg);
}

void RF430_CO2_Out( uint8_t bNDEF_Flg )
{
   mbOutFlag &= ~OUT_FLG_NEW_EE893_DAT;
   char buffer[40];
   snprintf( buffer, sizeof(buffer), "CO2: %hu [ppm]", mpstEE893->wCO2_avg);
   RF430_WriteText( buffer, bNDEF_Flg);
}

void RF430_BatOut( uint8_t bNDEF_Flg )
{
   char buffer[40];
   mbOutFlag &= ~OUT_FLG_NEW_AKKU_DAT;
   snprintf( buffer, sizeof(buffer), "Funk ID: %02hhx:%02hhx:%02hhx:%02hhx", gSYS.bRadioID[0], gSYS.bRadioID[1], gSYS.bRadioID[2], gSYS.bRadioID[3] );
   RF430_WriteText( buffer, NDEF_FLAG_NONE);
   snprintf( buffer, sizeof(buffer), "Akku: %hhi [%%]", mbRF430_Akku_Value);
   RF430_WriteText( buffer, bNDEF_Flg);
}

void RF430_IPv6Out( char* pbDescription, uint8_t *pbIP, uint16_t wPort )
{
   char bBuffer[80];
   IPv6_toString( bBuffer, sizeof(bBuffer), pbDescription, pbIP, wPort);
   RF430_WriteSingleText( bBuffer);
}

//------------------------------------------------------------------------------------------------------------------------
// RF430_PollData: gibt die NFC-Daten in Kurzform aus.
// Wenn bFlg=true erfolgt Meldung "no Data" wenn keine neuen Sensor-Daten vorhanden
//------------------------------------------------------------------------------------------------------------------------
void RF430_PollData( uint8_t bFlg )
{
   if ( mbOutFlag & OUT_FLG_NEW_DATA_MASK )
   {
      RF430_WriteNdefHeader();
      if( mbOutFlag & OUT_FLG_NEW_BME280_DAT )
      {
         if (mbOutFlag & (~OUT_FLG_NEW_BME280_DAT & OUT_FLG_NEW_DATA_MASK) )
            RF430_BME280_Out( NDEF_FLAG_NONE );          // es folgend noch weitere Ausgaben
         else
            RF430_BME280_Out( NDEF_FLAG_ME );            // Das war die letzte Ausgabe in diesem Task-Schritt
      }
      if( mbOutFlag & OUT_FLG_NEW_Si1132_DAT )
      {
         if (mbOutFlag & (~OUT_FLG_NEW_Si1132_DAT & OUT_FLG_NEW_DATA_MASK) )
            RF430_Si1132_Out( NDEF_FLAG_NONE );
         else
            RF430_Si1132_Out( NDEF_FLAG_ME );
      }
      if( mbOutFlag & OUT_FLG_NEW_FLOW_DAT )
      {
         if (mbOutFlag & (~OUT_FLG_NEW_FLOW_DAT & OUT_FLG_NEW_DATA_MASK) )
            RF430_Flow_Out( NDEF_FLAG_NONE );
         else
            RF430_Flow_Out( NDEF_FLAG_ME );
      }
      if( mbOutFlag & OUT_FLG_NEW_EE893_DAT )
      {
         if (mbOutFlag & (~OUT_FLG_NEW_EE893_DAT & OUT_FLG_NEW_DATA_MASK) )
            RF430_CO2_Out( NDEF_FLAG_NONE );
         else
            RF430_CO2_Out( NDEF_FLAG_ME );
      }
      if( mbOutFlag & OUT_FLG_NEW_AKKU_DAT )
      {
         RF430_BatOut( NDEF_FLAG_ME );
      }
      RF430_SetNdefLen();
   }
   else if (bFlg)
      RF430_WriteSingleText("no Data");
}

//------------------------------------------------------------------------------------------------------------------------
//    kopiert einen String von pSrc nach pDest. Max werden iLen-1 kopiert.
//    Es wird sichergestellt dass ein 0-Byte am Ende des pDst-String ist
//    ist pDst länger als pSrc so wird der Rest von pDst mit 0-Bytes gefüllt
//------------------------------------------------------------------------------------------------------------------------
void RF430_Read_String( BYTE* pDst, uint16_t wDstLen, BYTE *pSrc, uint16_t wSrcLen)
{
   if (wDstLen)                                       // Sonderfall wDstLen=0 hier abfangen
   {
      wDstLen--;                                      // Platz für 0-Byte schon mal abziehen
      while(wDstLen && wSrcLen && *pSrc)
      {
         *pDst++ = *pSrc++;                           // SRC nach DST kopieren
         wDstLen--;
         wSrcLen--;
      }
      while(wDstLen)
      {
         *pDst++ = 0;                                 // Rest von DST mit 0 auffüllen
         wDstLen--;
      }
      *pDst = 0;                                      // Rest von DST mit 0 auffüllen
   }
}

//------------------------------------------------------------------------------------------------------------------------
// RF430_TryInterpretCMD: versuche die TAG-Daten zu interpretieren
//------------------------------------------------------------------------------------------------------------------------
int16_t RF430_TryInterpretCMD()
{
   char  bBuf[80];                                    // Mal 80 Zeichen für PC-Befehl vorsehen
   int iLen;

   Read_Continuous(0, bBuf, 9);                       // Checke ob NDEF-Header vorhanden
   for (int i=0; i<9; i++)
   {
      if(bBuf[i] != mcbNDEF_Header_Data[i])
         return 0;
   }
                                                      // wir erwarten, dass nach dem NDEF-Header ein Text-Rekord folgt
                                                      // bzw. der Bin-Record von uns verschwunden ist
   iLen = RF430_ReadText(0, (BYTE*) bBuf, sizeof(bBuf)-1 );
   if (iLen>0)
      mbOutFlag &= ~OUT_FLG_FAST_MODE ;               // bei Tablet oder Handy kein Fast-Mode
   else
   {
      BYTE bData = Read_Register(sizeof(mcbNDEF_Header_Data)-1 );
      if (bData>0x80)                                             // Die Binärdaten wurden von der Service-Matte verändert
         iLen = RF430_ReadText(4, (BYTE*) bBuf, sizeof(bBuf)-1 ); // Falls wir es mit der Service-Matte zu tun haben,
                                                                  // nochmal nach den Binärdaten nach dem CMD suchen
      if (iLen > 0)
      {
         mbOutFlag |= OUT_FLG_FAST_MODE ;             // Standard-Mäßig Ausgabe der Messwerte im Fast-Mode deaktivieren
         mbOutFlag &= ~OUT_FLG_MESS_OUT;
      }
   }

   if (iLen>0)
   {
      bBuf[iLen] = 0;                                 // Stringende (0-Byte) anhängen um mit string-funktionen arbeiten zu können
      Write_Register(CONTROL_REG, 0);                 // RF430 disablen um Daten ändern zu können

      switch (bBuf[0])
      {
      case 'f':                                       // egentlich unnötig geworden da sofort nach dem Setzen gespeichert wird
         StoreFlashData();
         RF430_WriteSingleText(mbDone);
         break;

//      case 'a':
//      case 'i':                                       // Get Mac-Adresse
//         RF430_Send_RadioMac();
//         break;

      case 'h':                                       // h: wie halt die Ausgabe der Messdaten an
         mbOutFlag &= ~OUT_FLG_MESS_OUT;
         RF430_WriteSingleText(mbDone);
         break;
      case 'm':                                       // m: wie Messung wieder aktivieren
         mbOutFlag |= OUT_FLG_MESS_OUT;
         RF430_WriteSingleText(mbDone);
         break;
      case 'n':
         RF430_WriteSingleText("EIS: Enviroment-Information-System");
         break;

      case 'p':
         RF430_PollData( true );                      // Daten Pollen - falls keine neuen Daten vorhanden no-Data ausgeben
         break;

      case 'P':
         RF430_PollData( false );                     // Daten Pollen - falls keine neuen Daten vorhanden nichts ausgeben
         break;

      case 's':
         switch (bBuf[1] )
         {
         case 'c':                                    // sc. CO2-Messintervall
            sscanf( &bBuf[3], "%lu", &gSYS.ulEE893_MessInterval);
            #ifdef USE_MSP430F6658
            EE893_SetMessInterval( gSYS.ulEE893_MessInterval );
            #endif
            break;
         case 'd':
            RF430_Read_String( gSYS.bDeviceName, sizeof(gSYS.bDeviceName), (BYTE*) &bBuf[3], iLen);
            break;
         case 'f':                                    // sf. flow
            switch (bBuf[2] )
            {
            case 'a':
               sscanf( &bBuf[3], "%hhu", &gSYS.bFlowMessAnzahl);
               Flow_SetMessAnzahl( gSYS.bFlowMessAnzahl );
               break;
            case 'i':
               sscanf( &bBuf[3], "%lu", &gSYS.ulFlowMessInterval);
               Flow_SetMessInterval( gSYS.ulFlowMessInterval );
               break;
            }
            break;
         case 'l':                                    // sl. light
            switch (bBuf[2] )
            {
            case 'a':
               sscanf( &bBuf[3], "%hhu", &gSYS.bLightMessAnzahl);
               Si1132_SetMessAnzahl( gSYS.bLightMessAnzahl );
               break;
            case 'i':
               sscanf( &bBuf[3], "%lu", &gSYS.ulLightMessInterval);
               Si1132_SetMessInterval( gSYS.ulLightMessInterval );
               break;
            }
            break;
         case 'p':                                    // sp. Druck
            sscanf( &bBuf[3], "%lu", &gSYS.ulPressureMessInterval);
            BME280_SetMessinterval( gSYS.ulPressureMessInterval );
            break;
         case 'r':
            switch (bBuf[2] )
            {
            case 'a':
               sscanf( &bBuf[3], "%u", &gSYS.stCC1350_SET.wPANID);
               Radio_ReInitFunkModul();
               break;
            case 'c':
               sscanf( &bBuf[3], "%hhu", &gSYS.stCC1350_SET.bCHANNEL);
               Radio_ReInitFunkModul();
               break;

            case 'i':
               IPv6_ParseString( &bBuf[3],            // Server-IP-Adresse und evtl Port für den CC1350 setzen
                             gSYS.stLWM2M_SET.bSRV_IP,
                             &gSYS.stLWM2M_SET.wSRV_PORT );
               Radio_ReInitFunkModul();
               break;
            case 'p':
               sscanf( &bBuf[3], "%u", &gSYS.stLWM2M_SET.wSRV_PORT);
               Radio_ReInitFunkModul();
               break;
            case 'n':
               getStringValue( &bBuf[4], gSYS.stLWM2M_SET.strCLI_NAME, sizeof(gSYS.stLWM2M_SET.strCLI_NAME)-1);
               Radio_ReInitFunkModul();
               break;
            }
         }
         StoreFlashData();
                                                      // es geht bewusst bei 'g' weiter um den aktuellen Parameterwert nach dem setzen auszugeben
      case 'g':
         switch (bBuf[1] )
         {
         case 'c':
            RF430_Send_int32_ASCII_Data("CO2-Mess-Intervall", gSYS.ulEE893_MessInterval, "ms");
            break;
         case 'd':
            RF430_Send_Text("Device-Name", gSYS.bDeviceName);
            break;
         case 'f':
            switch (bBuf[2] )
            {
            case 'a':
               RF430_Send_int32_ASCII_Data("Flow-Mess-Anzahl", gSYS.bFlowMessAnzahl , "none");
               break;
            case 'i':
               RF430_Send_int32_ASCII_Data("Flow-Mess-Intervall", gSYS.ulFlowMessInterval , "ms");
               break;
            default:
               RF430_WriteSingleText(mbCMD_unknown);
            }
            break;
         case 'l':
            switch (bBuf[2] )
            {
            case 'a':
               RF430_Send_int32_ASCII_Data("Light-Mess-Anzahl", gSYS.bLightMessAnzahl , "none");
               break;
            case 'i':
               RF430_Send_int32_ASCII_Data("Light-Mess-Intervall", gSYS.ulLightMessInterval , "ms");
               break;
            default:
               RF430_WriteSingleText(mbCMD_unknown);
            }
            break;
         case 'p':
            RF430_Send_int32_ASCII_Data("Pressure-Mess-Intervall", gSYS.ulPressureMessInterval , "ms");
            break;
         case 'r':
            switch (bBuf[2] )
            {
            case 'a':
               RF430_Send_int32_ASCII_Data("PANID", gSYS.stCC1350_SET.wPANID , "");
               break;
            case 'c':
               RF430_Send_int32_ASCII_Data("Radio-Channel", gSYS.stCC1350_SET.bCHANNEL , "");
               break;

            case 'i':
            case 'p':
               RF430_IPv6Out( "Server-IP", gSYS.stLWM2M_SET.bSRV_IP, gSYS.stLWM2M_SET.wSRV_PORT);
               break;
            case 'n':
               RF430_SendStringValue( "Client-Name", gSYS.stLWM2M_SET.strCLI_NAME );
               break;
            default:
               RF430_WriteSingleText(mbCMD_unknown);
               break;
            }
            break;
         case 's':
            RF430_Send_Text("Serien-Nr", gSYS.bSerienNr );
            break;
         default:
            RF430_WriteSingleText(mbCMD_unknown);
            break;
         }
         break;
      case 't':
         RF430_WriteSingleText("30: EIS-Temp/Feuchte/Luftdruck/2d Wind/Licht/CO2");
         break;
      case 'v':                                       // Get Version (Software Version)
         //LED_GE_ON;
         RF430_WriteSingleText((char*) gbVersion);
         break;
      default:
         RF430_WriteSingleText(mbCMD_unknown);
         break;
      }
   }
   return iLen;
}

//=================================================================================================================
//    Hier kommen die Funktionen zur Datenübergabe aus den anderen Modulen
//=================================================================================================================
void RF430_Send_BME280_Data( ST_BME280_MESS_DATA *pstM )
{
   mbOutFlag |= OUT_FLG_NEW_BME280_DAT;
   mpstBME280 = pstM;
}

void RF430_Send_Si1132_Data( ST_Si1132_MessData *pstM )
{
   mbOutFlag |= OUT_FLG_NEW_Si1132_DAT;
   mpstSi1132 = pstM;
}

void RF430_Send_Flow_Data  ( ST_FLOW_MESS_DATA  *pstM )
{
   mbOutFlag |= OUT_FLG_NEW_FLOW_DAT;
   mpstFlow = pstM;
}

void RF430_Send_EE893_Data (ST_EE893_MESS_DATA *pstM )
{
   mbOutFlag |= OUT_FLG_NEW_EE893_DAT;
   mpstEE893 = pstM;
}

void RF430_Send_Akku_Data( uint8_t bAkkuValue )
{
   mbOutFlag |= OUT_FLG_NEW_AKKU_DAT;
   mbRF430_Akku_Value = bAkkuValue;
}

//=================================================================================================================
//    Ende der Augabefunktionen
//=================================================================================================================

//=================================================================================================================
// Ab hier die neuen Task-Funktionen
//=================================================================================================================

//=================================================================================================================
//    Step0: Checke ob RF-Feld vorhanden
//=================================================================================================================
EN_TASK_STATUS RF430_Task_CheckRF_Field(void)
{
   if ((isNFC_RF_INT_FLG_SET || isNFC_RF_FIELD_ON) && isWP_OFF)
   {
      NFC_RF_INT_FLG_CLR;
      return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP1);
   }
   else
   {
      return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP0);
   }
}

//=================================================================================================================
//    Step1: Schalte die 3V-Versorgung vom RF430-Chip ein extra step da 30ms Wartezeit bis Step2
//=================================================================================================================
EN_TASK_STATUS RF430_Task_PowerOn(void)
{
   LED_GN_ON;
   RF430_POWER_ON;
   OLED_On( OLED_NFC_TIMEOUT );
   return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP2);
}

//=================================================================================================================
//    Step2: Prüfe ob I²C-Leitungen nun high geworden sind
//=================================================================================================================
uint8_t mbCounter;

EN_TASK_STATUS RF430_Task_isI2C_Ready(void)
{
   if((RF430_I2C_IN & RF430_I2C_BITS) != RF430_I2C_BITS)
      return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP2);     // Bleibe in diesem State
   else
   {
      RF430_RESET_I2C;                                      // Bring das I²C-Modul des MSP430 in Grundzustand
      mbCounter = 100;                                      // max 100 Versuche eine Tag-Ready-Meldung zu erhalten
      return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP3);     // Gehe in nächsten State
   }
}

//=================================================================================================================
//    Step3: Frage das RF430-Statusregister ab und warte auf Ready
//=================================================================================================================
EN_TASK_STATUS RF430_Task_isTAG_Ready(void)
{
   if (Read_Register(STATUS_REG) & READY)
      return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP4);     // Gehe in nächsten State
   else if (--mbCounter)
      return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP3);     // Bleibe in diesem State
   else
   {
      LED_GN_OFF;
      RF430_POWER_OFF;                                      // Tag ausschalten
      return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP0);     // gehe in Step0 und warte ob RF-Feld da ist
   }
}

//=================================================================================================================
//    Step4: Fix Unresponsive RF
//=================================================================================================================
EN_TASK_STATUS RF430_Task_Errata_Fix(void)
{
   /****************************************************************************/
   /* Errata Fix : Unresponsive RF - recommended firmware                      */
   /****************************************************************************/
   // Please implement this fix as given in this block.  It is important that
   // no line be removed or changed.
   unsigned int version;
   version = Read_Register(VERSION_REG);              // read the version register.  The fix changes based on what version of the
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

   return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP5);  // gehe in Step5 und gebe nun die Messdaten aus
}

//=================================================================================================================
//    Step5: Schreibe NDEF-Daten in RF430-Chip
//           entweder als Antwort auf die Interpretierten Daten RF430_TryInterpretCMD() oder
//           die normalen Messdaten
//=================================================================================================================
EN_TASK_STATUS RF430_Task_WriteNdefDaten(void)
{
   static uint16_t bOutFlg=0;

   Write_Register(CONTROL_REG, 0);

   if(mbOutFlag & OUT_FLG_MESS_OUT)                   // Wenn Messdaten-Ausgabe gewünscht
   {                                                  // hier alle Messdaten ausgeben
      RF430_WriteNdefHeader();
      if ( bOutFlg )                                  // Daten in zwei Phasen ausgeben da sonst NFC-reader im Tablet spinnt
      {                                               // größenbegrenzung
         RF430_Flow_Out( NDEF_FLAG_NONE );
         RF430_CO2_Out( NDEF_FLAG_NONE );
         RF430_BatOut( NDEF_FLAG_ME );
      }
      else
      {
         RF430_BME280_Out( NDEF_FLAG_NONE );
         RF430_Si1132_Out( NDEF_FLAG_ME );
      }
      RF430_SetNdefLen();

      bOutFlg ^= 1;                                   // Beim nächsten mal den anderen Datenblock ausgeben
   }
   else
   {                                                  // Damit nicht immer TI.COM/NFC aufgerufen wird
                                                      // (Das steht nämlich sonst im Speicher)
      RF430_WriteSingleText("Messdatenausgabe gestoppt - mit Tag-Writer ein 'm'-CMD-Eingeben");
   }

   Write_Register(INT_FLAG_REG, 0xFF);                // CLR all pending interrupts
   Write_Register(CONTROL_REG, RF_ENABLE);

   return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP6);
}

//=================================================================================================================
//    Step6: Prüfe ob unsere Daten verändert wurden
//=================================================================================================================
EN_TASK_STATUS RF430_Task_CheckDataChanged(void)
{
   static uint16_t wFastCounter = 0;

   Write_Register(CONTROL_REG, 0);                    // RF von RF430 abschalten

   int16_t iLen = RF430_TryInterpretCMD();            // Wurden NDEF-Daten ins Tag geschrieben (Tablet/Handy...)

   if (iLen>0)
   {
      if (mbOutFlag & OUT_FLG_FAST_MODE)
      {
         LED_GE_TOGGLE;
         wFastCounter = 1000;
         return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP6);
      }
      else
         return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP7);
   }
   else
   {
      if (wFastCounter)
      {
         LED_GN_TOGGLE;
         wFastCounter--;
         if ((mbOutFlag & (OUT_FLG_MESS_OUT | OUT_FLG_NEW_DATA_MASK)) > OUT_FLG_MESS_OUT)
            return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP10);
         else
            return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP6);
      }
      else
      {
         mbOutFlag |= OUT_FLG_MESS_OUT;
         return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP9);
      }
   }
}

//=================================================================================================================
//    Step7: Einfach nur NFC-Radio aus damit Handy "denkt" der Tag sei entfernt worden
//=================================================================================================================
EN_TASK_STATUS RF430_Task_WaitNFC_RadioOff(void)
{
   static uint16_t wSecCounter = 5*4;
                                                      // RF430 - RF sollte bereits aus sein also hier nichts tun
                                                      // nur LED blinken lassen
//   Write_Register(INT_FLAG_REG, 0xFF);                // CLR all pending interrupts
//   Write_Register(CONTROL_REG, 0);                    // RF430 - RF ausschalten

   if ((wSecCounter & 0x03) == 0)
      LED_GN_ON;
   else
      LED_GN_OFF;

   if (wSecCounter--)
      return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP7);
   else
   {
      Write_Register(INT_FLAG_REG, 0xFF);             // CLR all pending interrupts
      Write_Register(CONTROL_REG, RF_ENABLE);
      wSecCounter = 15*4;
      return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP8);
   }
}

//=================================================================================================================
//    Step8: RF430_Task_WaitNFC_RadioOn   jetzt das Radio wieder einschalten um Antwort oder Messdaten zu senden
//=================================================================================================================
EN_TASK_STATUS RF430_Task_WaitNFC_RadioOn(void)
{
                                                      // RF430 - RF sollte bereits an sein also hier nichts tun
                                                      // nur LED blinken lassen
   static uint16_t wSecCounter = 15*4;

   if ((wSecCounter & 0x03) == 0)
      LED_GN_OFF;
   else
      LED_GN_ON;

   if (wSecCounter--)
      return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP8);
   else
   {
      wSecCounter = 15*4;
      return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP9);
   }
}

//=================================================================================================================
//    Step9: Schalte den RF430-Chip aus
//=================================================================================================================
EN_TASK_STATUS RF430_Task_PowerOff(void)
{
   LED_GN_OFF;
   Write_Register(CONTROL_REG, 0);                    // RF-off (RF430-Chip)
   RF430_POWER_OFF;                                   // Power off
   return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP0);  // gehe in Step0
}

//=================================================================================================================
//    Step10: Datenausgabe im Fast-Mode
//=================================================================================================================
EN_TASK_STATUS RF430_Task_WriteNdefDatenFastMode(void)
{
   if ((mbOutFlag & (OUT_FLG_MESS_OUT | OUT_FLG_NEW_DATA_MASK)) > OUT_FLG_MESS_OUT)
   {
      Write_Register(CONTROL_REG, 0);

      RF430_PollData( false );                        // keine Meldung wenn keine Sensordaten vorhanden

      Write_Register(INT_FLAG_REG, 0xFF);             // CLR all pending interrupts
      Write_Register(CONTROL_REG, RF_ENABLE);
   }
   return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP6);
}

//=================================================================================================================
//    Hier die Funktion-Table der RF430-Task (Step-Nr. = Index in dieser Tabelle)
//=================================================================================================================
ST_FUNCTION_TABLE gstRF430_Func_Table[] =
{
   { RF430_Task_CheckRF_Field          ,    1024 },   // Step0:   prüfe 1mal pro Sekunden ob ein NFC-Reader in reichweite ist.

   { RF430_Task_PowerOn                ,      30 },   // Step1:   Schalte die RF430-Spannung ein und warte ca. 30ms
   { RF430_Task_isI2C_Ready            ,       2 },   // Step2:   Prüfe ob I²C-Leitungen Ready
   { RF430_Task_isTAG_Ready            ,       2 },   // Step3:   Prüfe ob Tag Ready
   { RF430_Task_Errata_Fix             ,       2 },   // Step4:   Prüfe ob Errata Fix notwendig falls ja füre den jetzt durch

   { RF430_Task_WriteNdefDaten         , 10*1024 },   // Step5:   Gebe 3 Sek die Messdaten oder den Standard-Text aus
   { RF430_Task_CheckDataChanged       ,      40 },   // Step6:   Prüfe ob NDEF-Daten verändert wurden -
                                                      //          falls ja Daten interpretieren und Antwort geben (Radio aus lassen)
                                                      //          weiter in Sep 7 oder 9
   { RF430_Task_WaitNFC_RadioOff       ,     256 },//15*1024 },   // Step7:   Schalte RF430-NFC-Radio aus und warte damit Tablet "denkt"
                                                      //          der Tag sei entfernt worden und um User Zeit zu geben den
                                                      //          Tag-Writer zu verlassen um Antwort anzusehen

   { RF430_Task_WaitNFC_RadioOn        ,     256 },//,15*1024 },   // Step8:   Schalte RF430-NFC-Radio ein

   { RF430_Task_PowerOff               , 10*1024 },   // Step9:   Schalte den RF430-Chip aus

   { RF430_Task_WriteNdefDatenFastMode ,     100 },   // Step10:  Datenausgabe im Fast-Mode

} ;

//=================================================================================================================
//    Hier die Initialisierung des Moduls
//=================================================================================================================
void RF430_Init(void)
{
   // Ports initalisieren
   RF430_POWER_OFF;

   RF430_INT_DIR &= ~RF430_INT_BITS;
   RF430_INT_OUT |=  RF430_INT_BITS;
   RF430_REN     |=  RF430_INT_BITS;
   RF430_IES     |=  RF430_INT_BITS;                  // Interrupt bei high-low flanke
   RF430_IFG     &= ~RF430_INT_BITS;

   RF430_I2C_INIT(100000);                            // I²C initalisieren (100kBaud)

}
