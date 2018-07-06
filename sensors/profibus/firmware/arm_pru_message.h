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
/*******************************************************************************************
 * arm_pru_message.h
 *
 *  Created on: 21.10.2016
 *      Author: Fesseha Mamo
 */

#ifndef FIRMWARE_ARM_PRU_MESSAGE_H_
#define FIRMWARE_ARM_PRU_MESSAGE_H_

typedef enum UART_STATE_E {
	UART_STOPPED = 0,
	UART_RUNNING,
	UART_POWER_OFF
} uart_state_e;

uart_state_e handle_command(char* payload, int len);

#endif /* FIRMWARE_ARM_PRU_MESSAGE_H_ */
