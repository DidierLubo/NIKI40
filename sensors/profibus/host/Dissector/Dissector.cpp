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
 * Dissector.cpp
 *
 *  Created on: 15.06.2018
 *      Author: Dovydas Girdvainis
 */

#include "Dissector.h"
#include "Defines.h"
#include <string.h>

void Dissector::dissect_SD2(const char inputBuffer[], const int index[], int indexSize, std::queue<iPacket *> &outputQueue)
{
    for (int i = 0; i < indexSize; i++)
    {
        if (((index[i + 1] - index[i]) == SD2_LENGHT_CHECK) && (index[i] != indexSize - 1))
        {
            char packetLenghtChar[BASE_FIELD_SIZE+1] = {0};
            char repeatedPacketLenghtChar[BASE_FIELD_SIZE+1] = {0};
            char endDilliminator[BASE_FIELD_SIZE+1] = {0};
            char destinationAdress[BASE_FIELD_SIZE+1] = {0};
            char sourceAdress[BASE_FIELD_SIZE+1] = {0};
            char functionCode[BASE_FIELD_SIZE+1] = {0};
            char payload[MAX_SD2_PDU_SIZE+1] = {0};
            char frameCheckSequence[BASE_FIELD_SIZE+1] = {0};
            unsigned int packetLenght = 0;
            unsigned int currentOffset = index[i] + BASE_FIELD_SIZE;

            strncpy(packetLenghtChar, inputBuffer + currentOffset, 2);
            currentOffset+=BASE_FIELD_SIZE;
            strncpy(repeatedPacketLenghtChar, inputBuffer + currentOffset, 2);
            currentOffset+=BASE_FIELD_SIZE*2;

            if (strcmp(packetLenghtChar, repeatedPacketLenghtChar)==0)
            {
                packetLenghtChar[BASE_FIELD_SIZE] = '\0';
                packetLenght = strtoul(packetLenghtChar, NULL, 16) * 2;
                if (index[i] + currentOffset + packetLenght <= MAX_BUFFER_SIZE - 1)
                {
                    strncpy(endDilliminator, inputBuffer + currentOffset + packetLenght + BASE_FIELD_SIZE, 2);
                    if (strcmp(endDilliminator, "16") == 0)
                    {
                        strncpy(destinationAdress, inputBuffer + currentOffset, 2);
                        currentOffset+=BASE_FIELD_SIZE;
                        strncpy(sourceAdress, inputBuffer + currentOffset, 2);
                        currentOffset+=BASE_FIELD_SIZE;
                        strncpy(functionCode, inputBuffer + currentOffset, 2);
                        currentOffset+=BASE_FIELD_SIZE;
                        strncpy(payload, inputBuffer + currentOffset, (packetLenght-BASE_FIELD_SIZE*3));
                        currentOffset+=packetLenght-BASE_FIELD_SIZE*3;
                        payload[packetLenght+1]='\0';
                        strncpy(frameCheckSequence, inputBuffer + currentOffset, 2);
                        if (isFCS_Correct(destinationAdress, sourceAdress, functionCode, payload, frameCheckSequence,(packetLenght-BASE_FIELD_SIZE*3)/2)){
                            iPacket *packet = new SD2_Packet(destinationAdress, sourceAdress, functionCode, payload);
                            outputQueue.push(packet);
                        }
                    }
                }
            }
        }
    }
}

void Dissector::dissect_SD3(const char inputBuffer[], const int index[], int indexSize, std::queue<iPacket*> &outputQueue)
{

}

bool Dissector::isFCS_Correct(const char destinationAddress[], const char sourceAddress[], const char functionCode[], const char payload[], const char frameCheckSequence[], int payloadLenght)
{    
    int fcs = (strtol(destinationAddress,NULL,16) + strtol(sourceAddress,NULL,16) + strtol(functionCode,NULL,16) + sumPsdu(payload,payloadLenght) ) & 0xFF;
    return ( fcs == strtol(frameCheckSequence,NULL,16) ? true : false );
}

int Dissector::sumPsdu(const char payload[], int payloadLenght){
    int psdu_sum = 0; 
    char byteBuffer[3] = {0};

    for(int i=0; i<payloadLenght; i++){
        strncpy(byteBuffer,payload + (2*i),2);
        psdu_sum += strtol(byteBuffer,NULL,16);
    }  

    return psdu_sum;
}