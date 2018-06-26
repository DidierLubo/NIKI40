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

pthread_mutex_t dissectorMutex = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t senderMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t inputQueueCondition = PTHREAD_COND_INITIALIZER;
pthread_cond_t packetQueueCondition = PTHREAD_COND_INITIALIZER;

void dissect(const PacketType packetType, const char *inputBuffer, const iPatternMatcher *matcher, pthread_mutex_t *mutex, pthread_cond_t *condition)
{
    int index[MAX_INDEX_SIZE];
    int indexSize = 0;

    switch (packetType)
    {
    case SD1:
        break;
    case SD2:
    {
        std::remove_const <iPatternMatcher*>::type matcher;
        indexSize = matcher->search("68", inputBuffer, index);
        if (indexSize > 0)
        {
            iPacket *dataTelegram = Dissector::dissect_SD2(inputBuffer, index, indexSize, mutex, condition);

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
    int reader = *((int*)arg);

    std::cout << "Read routine started!" << std::endl;

    if (reader >= 0)
    {
        int readerStatus = 0;
        do
        {
            char inputBuffer[MAX_BUFFER_SIZE];
            readerStatus = read(reader, inputBuffer, MAX_BUFFER_SIZE);
            inputQueue.push(inputBuffer);
            std::cout << "pushed raw data to a queue!" << std::endl;

            pthread_mutex_unlock(&dissectorMutex);
            pthread_cond_signal(&inputQueueCondition);
        } while (readerStatus > 0);
    }
    pthread_exit(NULL);
}

void *disectorStartRoutine(void *arg)
{
    iPatternMatcher *matcher = static_cast<iPatternMatcher *>(arg);

    std::cout << "Disector routine started!" << std::endl;

    while (1)
    {
        pthread_mutex_lock(&dissectorMutex);
        while(inputQueue.empty())
            pthread_cond_wait(&inputQueueCondition,&dissectorMutex);
        pthread_mutex_unlock(&dissectorMutex);

        char unprocessedData[MAX_BUFFER_SIZE];
        strcpy(unprocessedData,inputQueue.front());
        inputQueue.pop();

        dissect(PacketType::SD2, unprocessedData, matcher,&senderMutex,&packetQueueCondition);
        dissect(PacketType::SD3, unprocessedData, matcher,&senderMutex,&packetQueueCondition);
    }
    pthread_exit(NULL);
}

void *printerRoutine(void *arg)
{
    std::cout << "Printer routine started!" << std::endl;

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
    int readThreadReturnCode, dissectorThreadReturnCode, printThreadReturnCode;
    void *threadJoinReturnCode;  

    int deviceReader = open(DEV_NAME, O_RDWR);

    if (deviceReader >= 0)
    {
        write(deviceReader, "start", 6);
    }

    printThreadReturnCode = pthread_create(&printThread, NULL, &printerRoutine, NULL);
    if(printThreadReturnCode){
        std::cout << "Failed creating printer thread #" << printThread << ".Thread exited with return code: " << printThreadReturnCode << std::endl;
        return EXIT_FAILURE;
    }

    iPatternMatcher *matcher = new RabinKarpPatternMatcher();
    dissectorThreadReturnCode = pthread_create(&dissectorThread, NULL, &disectorStartRoutine, matcher);
    if(dissectorThreadReturnCode){
        std::cout << "Failed creating dissector thread #" << dissectorThread << ".Thread exited with return code: " << dissectorThreadReturnCode << std::endl;
        return EXIT_FAILURE;
    }
    
    readThreadReturnCode = pthread_create(&readThread, NULL, &readStartRoutine, &deviceReader);
    if(readThreadReturnCode){
        std::cout << "Failed creating read thread #" << readThread << ".Thread exited with return code: " << readThreadReturnCode << std::endl;
        return EXIT_FAILURE;
    }
    
    if(!pthread_join(readThread, &threadJoinReturnCode)) 
        std::cout << "Reader thread closing failed with error: " << threadJoinReturnCode << std::endl;
    if(!pthread_join(dissectorThread, &threadJoinReturnCode))
        std::cout << "Dissector thread closing failed with error: " << threadJoinReturnCode << std::endl;
    if(!pthread_join(printThread, &threadJoinReturnCode))
        std::cout << "Printer thread closing failed with error: " << threadJoinReturnCode << std::endl;

    return EXIT_SUCCESS;
}