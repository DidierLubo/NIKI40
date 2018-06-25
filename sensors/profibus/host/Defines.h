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
 * Defines.h
 *
 * Contains the global project definitions.
 *
 *  Created on: 15.06.2018
 *      Author: Dovydas Girdvainis
 */

#ifndef __DEFINES_H__
#define __DEFINES_H__

#include <pthread.h>

#define DEV_NAME            "/dev/rpmsg_pru31"
#define NUMBER_OF_THREADS  3
#define SD2_LENGHT_CHECK   6 
#define MAX_SD2_PDU_SIZE   246
#define MAX_SD3_PDU_SIZE   8     
#define BASE_FIELD_SIZE    2
#define MAX_BUFFER_SIZE    MAX_SD2_PDU_SIZE
#define PATTERN_SIZE	   2
#define MAX_INDEX_SIZE     (MAX_BUFFER_SIZE * PATTERN_SIZE) 

pthread_mutex_t dissectorMutex; 
pthread_mutex_t senderMutex;
pthread_cond_t inputQueueCondition = PTHREAD_COND_INITIALIZER;
pthread_cond_t packetQueueCondition = PTHREAD_COND_INITIALIZER;

#endif //__DEFINES_H__
