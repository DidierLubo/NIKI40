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
 * main.cpp
 *
 *  Created on: 15.06.2018
 *      Author: Dovydas Girdvainis
 */

#include "PacketTypes/iPacket.h"
#include "PatternMachers/RabinKarpPatternMatcher.h"
#include "Dissector/Dissector.h"
#include "Defines.h"
#include <unistd.h>
#include <fcntl.h>
#include <queue>
#include <string.h>
#include <iostream>

static std::queue<char *> inputQueue;
static std::queue<iPacket *> packetQueue;

void dissect(PacketType packetType, const char *inputBuffer, iPatternMatcher *matcher)
{
    int index[MAX_INDEX_SIZE];
    int indexSize = 0;

    switch (packetType)
    {
    case SD1:
        break;
    case SD2:
    {
        indexSize = matcher->search("68", inputBuffer, index);

        if (indexSize > 0)
        {
            iPacket *dataTelegram = Dissector::dissect_SD2(inputBuffer, index, indexSize);

            if (dataTelegram != nullptr)
                packetQueue.push(dataTelegram);
        }
    }
    break;
    case SD3:
    {
    }
    break;
    case SD4:
        break;
    case SD5:
        break;
    case SD6:
        break;
    }
}

void *readStartRoutine(void *arg)
{
    int reader = (intptr_t)arg;

    if (reader >= 0)
    {
        int readerStatus = 0;
        do
        {
            char inputBuffer[MAX_BUFFER_SIZE];
            readerStatus = read(reader, inputBuffer, MAX_BUFFER_SIZE);
            inputQueue.push(inputBuffer);
            pthread_mutex_unlock(&dissectorMutex);
            pthread_cond_signal(&inputQueueCondition);
        } while (readerStatus > 0);
    }
    pthread_exit(NULL);
}

void *disectorStartRoutine(void *arg)
{
    iPatternMatcher *matcher = static_cast<iPatternMatcher *>(arg);

    while (1)
    {
        pthread_mutex_lock(&dissectorMutex);
        while(inputQueue.empty()) 
            pthread_cond_wait(&inputQueueCondition,&dissectorMutex);
        pthread_mutex_unlock(&dissectorMutex);

        char unprocessedData[MAX_BUFFER_SIZE];
        strcpy(unprocessedData,inputQueue.front());
        inputQueue.pop();

        dissect(PacketType::SD2, unprocessedData, matcher);
        dissect(PacketType::SD3, unprocessedData, matcher);
    }
    pthread_exit(NULL);
}

void *printerRoutine(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&senderMutex);
        while(packetQueue.empty()) 
            pthread_cond_wait(&packetQueueCondition,&senderMutex);
        pthread_mutex_unlock(&senderMutex);

        iPacket *packet = packetQueue.front();
        packetQueue.pop();

        std::cout << "---------- Packet received ----------" << std::endl << packet->getPacketAsString();
    }
    pthread_exit(NULL);
}

int main()
{
    pthread_t readThread, dissectorThread, printThread;
    int deviceReader = open(DEV_NAME, O_RDWR);
    iPatternMatcher *matcher = new RabinKarpPatternMatcher();

    if (deviceReader >= 0)
    {
        write(deviceReader, "start", 6);
    }

    pthread_create(&readThread, NULL, &readStartRoutine, &deviceReader);
    pthread_create(&dissectorThread, NULL, &disectorStartRoutine, matcher);
    pthread_create(&printThread, NULL, &printerRoutine, NULL);

    pthread_join(readThread, NULL);
    pthread_join(dissectorThread, NULL);
    pthread_join(printThread, NULL);

    return 0;
}