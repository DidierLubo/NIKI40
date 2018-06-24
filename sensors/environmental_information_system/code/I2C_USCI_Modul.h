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
//    Erstellt am       20.08.2012                                                                                       *
//    Erstellt durch:   Bernd Ehrbrecht                                                                                  *
//                                                                                                                       *
//************************************************************************************************************************

#ifndef __I2C_USCI_MODUL__
#define __I2C_USCI_MODUL__

#define I2C_DEFAULT_BAUDRATE     100000

#ifdef I2C_USE_USCI_B0

//------------------------------------------------------------------------------------------------------------------------
//    I2C_Init:
//------------------------------------------------------------------------------------------------------------------------
//    Initialisiert die UCSI-Schnittstelle und verwendet die angegebene Bausrate (100000 / 400000)
//------------------------------------------------------------------------------------------------------------------------
//    BaudRate:      (100000 / 400000)
//------------------------------------------------------------------------------------------------------------------------
void I2C_B0_Init(long BaudRate);

//------------------------------------------------------------------------------------------------------------------------
//    I2C_ReadRegister:
//------------------------------------------------------------------------------------------------------------------------
//    Liest ein Register von der angegebenen I²C-Slave-Adresse und der angegeben Register-Adresse
//------------------------------------------------------------------------------------------------------------------------
//    I2C_Slave_ADR:       I²C-Slave-Adresse (Device-Adresse) ohne RD/WR-Bit
//    I2C_Register:        Register-Adresse
//    ByteCount:           Anzahl der zu lesenden Bytes
//    bBuffer:             Pointer wo die Daten hin sollen
//------------------------------------------------------------------------------------------------------------------------
//    Return int:          0 Alles ok sonst Fehlernummer
//------------------------------------------------------------------------------------------------------------------------
//__monitor int I2C_B0_ReadRegister( char I2C_Slave_ADR, char I2C_Register, void* bBuffer, char ByteCounter);
int I2C_B0_ReadRegister( char I2C_Slave_ADR, char I2C_Register, void* bBuffer, char ByteCounter);

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
//__monitor int I2C_B0_WriteRegister( char I2C_Slave_ADR, char I2C_Register, void* bBuffer, char ByteCounter);
int I2C_B0_WriteRegister( char I2C_Slave_ADR, char I2C_Register, void* bBuffer, char ByteCounter);

//------------------------------------------------------------------------------------------------------------------------
//    I2C_ReadRegister:
//------------------------------------------------------------------------------------------------------------------------
//    Liest ein Register von der angegebenen I²C-Slave-Adresse und der angegeben Register-Adresse
//------------------------------------------------------------------------------------------------------------------------
//    I2C_Slave_ADR:       I²C-Slave-Adresse (Device-Adresse) ohne RD/WR-Bit
//    I2C_Register:        Register-Adresse
//    ByteCount:           Anzahl der zu lesenden Bytes
//    bBuffer:             Pointer wo die Daten hin sollen
//------------------------------------------------------------------------------------------------------------------------
//    Return int:          0 Alles ok sonst Fehlernummer
//------------------------------------------------------------------------------------------------------------------------
//__monitor int I2C_B0_ReadRegister_Adr16( char I2C_Slave_ADR, WORD I2C_Register, void* bBuffer, char ByteCounter);
int I2C_B0_ReadRegister_Adr16( char I2C_Slave_ADR, WORD I2C_Register, void* bBuffer, char ByteCounter);

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
//__monitor int I2C_B0_WriteRegister_Adr16( char I2C_Slave_ADR, WORD I2C_Register, void* bBuffer, char ByteCounter);
int I2C_B0_ReadRegister_Adr16( char I2C_Slave_ADR, WORD I2C_Register, void* bBuffer, char ByteCounter);
#endif

#ifdef I2C_USE_USCI_B1

//------------------------------------------------------------------------------------------------------------------------
//    I2C_Init:
//------------------------------------------------------------------------------------------------------------------------
//    Initialisiert die UCSI-Schnittstelle und verwendet die angegebene Bausrate (100000 / 400000)
//------------------------------------------------------------------------------------------------------------------------
//    BaudRate:      (100000 / 400000)
//------------------------------------------------------------------------------------------------------------------------
void I2C_B1_Init(long BaudRate);

//------------------------------------------------------------------------------------------------------------------------
//    I2C_ReadRegister:
//------------------------------------------------------------------------------------------------------------------------
//    Liest ein Register von der angegebenen I²C-Slave-Adresse und der angegeben Register-Adresse
//------------------------------------------------------------------------------------------------------------------------
//    I2C_Slave_ADR:       I²C-Slave-Adresse (Device-Adresse) ohne RD/WR-Bit
//    I2C_Register:        Register-Adresse
//    ByteCount:           Anzahl der zu lesenden Bytes
//    bBuffer:             Pointer wo die Daten hin sollen
//------------------------------------------------------------------------------------------------------------------------
//    Return int:          0 Alles ok sonst Fehlernummer
//------------------------------------------------------------------------------------------------------------------------
//__monitor int I2C_B1_ReadRegister( char I2C_Slave_ADR, char I2C_Register, void* bBuffer, char ByteCounter);
int I2C_B1_ReadRegister( char I2C_Slave_ADR, char I2C_Register, void* bBuffer, char ByteCounter);

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
//__monitor int I2C_B1_WriteRegister( char I2C_Slave_ADR, char I2C_Register, void* bBuffer, char ByteCounter);
int I2C_B1_WriteRegister( char I2C_Slave_ADR, char I2C_Register, void* bBuffer, char ByteCounter);

//------------------------------------------------------------------------------------------------------------------------
//    I2C_ReadRegister:
//------------------------------------------------------------------------------------------------------------------------
//    Liest ein Register von der angegebenen I²C-Slave-Adresse und der angegeben Register-Adresse
//------------------------------------------------------------------------------------------------------------------------
//    I2C_Slave_ADR:       I²C-Slave-Adresse (Device-Adresse) ohne RD/WR-Bit
//    I2C_Register:        Register-Adresse
//    ByteCount:           Anzahl der zu lesenden Bytes
//    bBuffer:             Pointer wo die Daten hin sollen
//------------------------------------------------------------------------------------------------------------------------
//    Return int:          0 Alles ok sonst Fehlernummer
//------------------------------------------------------------------------------------------------------------------------
//__monitor int I2C_B1_ReadRegister_Adr16( char I2C_Slave_ADR, WORD I2C_Register, void* bBuffer, char ByteCounter);
int I2C_B1_ReadRegister_Adr16( char I2C_Slave_ADR, WORD I2C_Register, void* bBuffer, char ByteCounter);

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
//__monitor int I2C_B1_WriteRegister_Adr16( char I2C_Slave_ADR, WORD I2C_Register, void* bBuffer, char ByteCounter);
int I2C_B1_WriteRegister_Adr16( char I2C_Slave_ADR, WORD I2C_Register, void* bBuffer, char ByteCounter);
#endif

#ifdef I2C_USE_USCI_B2
//------------------------------------------------------------------------------------------------------------------------
//    I2C_Init:
//------------------------------------------------------------------------------------------------------------------------
//    Initialisiert die UCSI-Schnittstelle und verwendet die angegebene Bausrate (100000 / 400000)
//------------------------------------------------------------------------------------------------------------------------
//    BaudRate:      (100000 / 400000)
//------------------------------------------------------------------------------------------------------------------------
void I2C_B2_Init(long BaudRate);

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
//__monitor int I2C_B2_ReadBytes( char I2C_Slave_ADR, void* pvDaten, char ByteCounter);
int I2C_B2_ReadBytes( char I2C_Slave_ADR, void* pvDaten, char ByteCounter);

//------------------------------------------------------------------------------------------------------------------------
//    I2C_ReadRegister:
//------------------------------------------------------------------------------------------------------------------------
//    Liest ein Register von der angegebenen I²C-Slave-Adresse und der angegeben Register-Adresse
//------------------------------------------------------------------------------------------------------------------------
//    I2C_Slave_ADR:       I²C-Slave-Adresse (Device-Adresse) ohne RD/WR-Bit
//    I2C_Register:        Register-Adresse
//    ByteCount:           Anzahl der zu lesenden Bytes
//    bBuffer:             Pointer wo die Daten hin sollen
//------------------------------------------------------------------------------------------------------------------------
//    Return int:          0 Alles ok sonst Fehlernummer
//------------------------------------------------------------------------------------------------------------------------
//__monitor int I2C_B2_ReadRegister( char I2C_Slave_ADR, char I2C_Register, void* bBuffer, char ByteCounter);
int I2C_B2_ReadRegister( char I2C_Slave_ADR, char I2C_Register, void* bBuffer, char ByteCounter);

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
//__monitor int I2C_B2_WriteRegister( char I2C_Slave_ADR, char I2C_Register, void* bBuffer, char ByteCounter);
int I2C_B2_WriteRegister( char I2C_Slave_ADR, char I2C_Register, void* bBuffer, char ByteCounter);

//------------------------------------------------------------------------------------------------------------------------
//    I2C_ReadRegister:
//------------------------------------------------------------------------------------------------------------------------
//    Liest ein Register von der angegebenen I²C-Slave-Adresse und der angegeben Register-Adresse
//------------------------------------------------------------------------------------------------------------------------
//    I2C_Slave_ADR:       I²C-Slave-Adresse (Device-Adresse) ohne RD/WR-Bit
//    I2C_Register:        Register-Adresse
//    ByteCount:           Anzahl der zu lesenden Bytes
//    bBuffer:             Pointer wo die Daten hin sollen
//------------------------------------------------------------------------------------------------------------------------
//    Return int:          0 Alles ok sonst Fehlernummer
//------------------------------------------------------------------------------------------------------------------------
//__monitor int I2C_B2_ReadRegister_Adr16( char I2C_Slave_ADR, WORD I2C_Register, void* bBuffer, char ByteCounter);
int I2C_B2_ReadRegister_Adr16( char I2C_Slave_ADR, WORD I2C_Register, void* bBuffer, char ByteCounter);

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
//__monitor int I2C_B2_WriteRegister_Adr16( char I2C_Slave_ADR, WORD I2C_Register, void* bBuffer, char ByteCounter);
int I2C_B2_WriteRegister_Adr16( char I2C_Slave_ADR, WORD I2C_Register, void* bBuffer, char ByteCounter);
#endif

#endif