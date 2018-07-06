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
 * uart_driver.c
 *
 * Rework of the 16C550 compliant UART0 driver according to
 * AM335x and AM1808 datasheets
 *
 *  Created on: 19.04.2018
 *      Author: Dovydas Girdvainis
 */

#ifndef _PRU_UART_DRIVER_H_
#define _PRU_UART_DRIVER_H_

#include <stdint.h>

void setupUART(void);
uint8_t readData(void);
void writeData(volatile uint8_t* msg, int bufferSize);
void changeUartBaudrate(uint32_t baudrate);
void enableLoopback(int8_t flag);
void startUART(int8_t flag);

#endif
