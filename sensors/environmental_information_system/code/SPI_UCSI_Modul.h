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
#define SPI_UCSI_B0_USED
#define SPI_UCSI_B1_USED

// Folgende define's bestimmen, die Bit-Rate (Geschwindigkeit) der SPI-Schnittstelle
#define SPI_UCSI_A0_BR
#define SPI_UCSI_A1_BR
#define SPI_UCSI_B0_BR
#define SPI_UCSI_B1_BR

// Folgende define's bestimmen, die Taktquelle (ACLK, SMCLK)
#define SPI_UCSI_A0_CLK_SRC
#define SPI_UCSI_A1_CLK_SRC
#define SPI_UCSI_B0_CLK_SRC
#define SPI_UCSI_B1_CLK_SRC

// Folgende define's bestimmen, den Initwert für das CTL0-Register
// Dadurch besteht völlige Kontrolle über das Taktverhalten ect.
#define SPI_UCSI_A0_CTL0_INIT
#define SPI_UCSI_A1_CTL0_INIT
#define SPI_UCSI_B0_CTL0_INIT
#define SPI_UCSI_B1_CTL0_INIT

// Beispiel für LIS3LV02DL-Sensor:
#define SPI_UCSI_B1_USED             // SPI-Modul informieren, dass SPI-B1-Schnitstelle verwendet werden soll
#define SPI_UCSI_B1_BR              0x3
#define SPI_UCSI_B1_CLK_SRC       SMCLK
#define SPI_UCSI_B1_CTL0_INIT   (UCCKPL | UCMSB | UCMST | UCMODE_0 | UCSYNC)

*/

//===========================================================================================
// Führt die Initialisierung der SPI-Schnistelle(n) durch
//===========================================================================================
void SPI_UCSI_Init(void);

//===========================================================================================
// Schreibt und liest ein Zeichen von der SPI-Schittstelle A0
//===========================================================================================
BYTE SPI_UCSI_A0_Put_Get_Byte(BYTE bData);

//===========================================================================================
// Schreibt und liest ein Zeichen von der SPI-Schittstelle A1
//===========================================================================================
BYTE SPI_UCSI_A1_Put_Get_Byte(BYTE bData);

//===========================================================================================
// Schreibt und liest ein Zeichen von der SPI-Schittstelle A2
//===========================================================================================
BYTE SPI_UCSI_A2_Put_Get_Byte(BYTE bData);

//===========================================================================================
// Schreibt und liest ein Zeichen von der SPI-Schittstelle B0
//===========================================================================================
BYTE SPI_UCSI_B0_Put_Get_Byte(BYTE bData);

//===========================================================================================
// Schreibt und liest ein Zeichen von der SPI-Schittstelle B1
//===========================================================================================
BYTE SPI_UCSI_B1_Put_Get_Byte(BYTE bData);

//===========================================================================================
// Schreibt und liest ein Zeichen von der SPI-Schittstelle B2
//===========================================================================================
BYTE SPI_UCSI_B2_Put_Get_Byte(BYTE bData);
