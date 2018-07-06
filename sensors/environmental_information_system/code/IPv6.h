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

#ifndef __IPv6__
#define __IPv6__

//===========================================================================================
//    IPv6_ParseIPv4OrIPv6
//===========================================================================================
//    ppszText:         in:  Zeiger auf den IPv6-String der mit 0 Terminiert sein muss!!
//    abyAddr:          out: Zeiger auf IPv6-Daten
//    pnPort:           out: Zeiger auf IPv6-Port
//    pbIsIPv6:         out: bool true wenn IPv6-Adresse erkannt wurde
//===========================================================================================
//    Return:           0=ok sonst Fehler-Nummer
//===========================================================================================
uint16_t IPv6_ParseIPv4OrIPv6 ( const char** ppszText,
		  uint8_t *abyAddr, uint16_t* pnPort, uint16_t *pbIsIPv6);

//===========================================================================================
//    Parse ein ASCII-String mit einer IPv6-Adresse in die 16-BYTE-IPv6-Adresse
//===========================================================================================
//    pData:         Zeiger auf den IPv6-String der mit 0 Terminiert sein muss!!
//    pbIP:          Zeiger auf IPv6-Daten
//    pwPort:        Zeiger auf IPv6-Port
//===========================================================================================
//    Return:        0=ok  sonst Fehler-Nummer
//===========================================================================================
uint32_t IPv6_ParseString( char *pData, uint8_t *pbIP, uint16_t *pwPort );

//===========================================================================================
//    Wandelt eine IPv6 Adresse in einen lesbaren ASCII-String um
//===========================================================================================
//    pcBuffer:      Zeiger auf den Ausgabebuffer - Mindestlänge 50 Zeichen+Description-Länge
//    wLen:          Länge des Ausgabebuffers
//    pbDescription: Zeiger auf die Beschreibung der IPv6-Adresse - z.B. "Server-IP"
//    pbIPv6:        Zeiger auf die IPv6-Adresse
//    wPort:         Portnummer
//===========================================================================================
//    return:        Länge des Strings
//===========================================================================================
uint16_t IPv6_toString( char *pcBuffer, uint16_t wLen, char *pbDescription, uint8_t *pbIPv6, uint16_t wPort);

#endif