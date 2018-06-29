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
#include "LimitiedQueue.h"
#include "Defines.h"
#include "RawDataStruct.h"
#include <unistd.h>
#include <fcntl.h>
#include <queue>
#include <string.h>
#include <iostream>
#include <pthread.h>

static LimitiedQueue<RawDataStruct> inputQueue;
static std::queue<iPacket *> packetQueue;
static bool readThreadAsleep = false;

pthread_mutex_t dequeueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t printerMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t queueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t searcherMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t dissectorMutex = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t senderMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t inputQueueCondition = PTHREAD_COND_INITIALIZER;
pthread_cond_t packetQueueCondition = PTHREAD_COND_INITIALIZER;
pthread_cond_t readThreadCondition = PTHREAD_COND_INITIALIZER;

void dissect(const PacketType packetType, const char *inputBuffer, iPatternMatcher *matcher)
{
    int index[MAX_INDEX_SIZE];
    int indexSize = 0;

    switch (packetType)
    {
    case SD1:
        break;
    case SD2:
    {
        //TODO: set iPatrnMatcher as const arg
        //std::remove_const <iPatternMatcher*>::type matcher;
        pthread_mutex_lock(&searcherMutex);
        indexSize = matcher->search("68", inputBuffer, index);
        pthread_mutex_unlock(&searcherMutex);
        std::cout << "dissect() indexSize is: " << indexSize << std::endl;
        if (indexSize > 1)
        {
            Dissector::dissect_SD2(inputBuffer, index, indexSize, packetQueue);

            std::cout << "dissect() packet dissected!" << std::endl;

            if (packetQueue.size() > 0){
                std::cout << "dissect() packet pushed to the queue!" << std::endl;
                pthread_mutex_unlock(&senderMutex);
                pthread_cond_signal(&packetQueueCondition);
            }
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

            if(inputQueue.isLimit()){
                readThreadAsleep=true;
                pthread_mutex_lock(&queueMutex);
                while(readThreadAsleep)
                    pthread_cond_wait(&readThreadCondition,&queueMutex);
                pthread_mutex_unlock(&queueMutex);
            } else {
                readerStatus = read(reader, inputBuffer, MAX_BUFFER_SIZE);
                RawDataStruct rawData;
                strcpy(rawData.rawDataArray,inputBuffer);
                inputQueue.push(rawData);
                pthread_mutex_unlock(&dissectorMutex);
                pthread_cond_signal(&inputQueueCondition);
            }
                        
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
        pthread_mutex_lock(&dequeueMutex);
        strcpy(unprocessedData,inputQueue.front().rawDataArray);
        inputQueue.pop();
        pthread_mutex_unlock(&dequeueMutex);

        if(inputQueue.size() < inputQueue.getMaxQueueLimit()/2 ){
            pthread_mutex_unlock(&queueMutex); 
            pthread_cond_signal(&readThreadCondition);
            readThreadAsleep=false;
        }

        dissect(PacketType::SD2, unprocessedData, matcher);
        dissect(PacketType::SD3, unprocessedData, matcher);
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

        pthread_mutex_lock(&printerMutex);
        iPacket *packet = packetQueue.front();
        packetQueue.pop();

        std::cout << "---------- Packet received ----------" << std::endl << packet->getPacketAsString();
        pthread_mutex_unlock(&printerMutex);
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
    
    if(pthread_join(readThread, &threadJoinReturnCode)) 
        std::cout << "Reader thread closing failed with error: " << threadJoinReturnCode << std::endl;
    if(pthread_join(dissectorThread, &threadJoinReturnCode))
        std::cout << "Dissector thread closing failed with error: " << threadJoinReturnCode << std::endl;
    if(pthread_join(printThread, &threadJoinReturnCode))
        std::cout << "Printer thread closing failed with error: " << threadJoinReturnCode << std::endl;

    return EXIT_SUCCESS;
}