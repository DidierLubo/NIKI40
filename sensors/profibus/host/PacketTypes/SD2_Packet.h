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
/*****************************************************************************************
 * SD2_Packet.h
 *
 *  Created on: 15.06.2018
 *      Author: Dovydas Girdvainis
 */

#ifndef __SD2_PACKET_H__
#define __SD2_PACKET_H__

#include "iPacket.h"
#include "Defines.h"
#include <string>

class SD2_Packet : public iPacket{
  private:
    std::string packetString;
    char destinationAddress[BASE_FIELD_SIZE+1];
    char sourceAddress[BASE_FIELD_SIZE+1];
    char functionCode[BASE_FIELD_SIZE+1];
    char payload[MAX_SD2_PDU_SIZE];

    void packToString();
    
  public:
    SD2_Packet(const char destinationAddress[], const char sourceAddress[], const char functionCode[], const char payload[]);
    PacketType getPacketType();
    std::string getPacketAsString();
};

#endif //__SD2_PACKET_H_