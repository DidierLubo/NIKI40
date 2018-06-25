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
 * SD3_Packet.cpp
 *
 *  Created on: 15.06.2018
 *      Author: Dovydas Girdvainis
 */

#include "SD3_Packet.h"
#include <string.h>

SD3_Packet::SD3_Packet(const char destinationAddress[], const char sourceAddress[], const char functionCode[], const char payload[], const char frameCheckSequence[]){
    strcpy(this->destinationAddress, destinationAddress); 
    strcpy(this->sourceAddress,sourceAddress); 
    strcpy(this->functionCode,functionCode); 
    strcpy(this->payload,payload);
}

PacketType SD3_Packet::getPacketType(){
    return SD3; 
}
std::string SD3_Packet::getPacketAsString(){
    return nullptr;
}
