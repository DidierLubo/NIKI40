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


#include "resource_table_1.h"
#include "arm_pru_message.h"
#include "pru_uart_driver.h"
#include <pru_cfg.h>
#include <pru_intc.h>
#include <pru_virtqueue.h>
#include <pru_rpmsg.h>
#include <pru_uart.h>
#include <rsc_types.h>

#include <string.h>

volatile register uint32_t __R31;

#define MAX_CAHR_DEV_LEN            256

#define TO_ARM_HOST                 18
#define FROM_ARM_HOST               19

#define VIRTIO_CONFIG_S_DRIVER_OK   4

#define HOST_INT                    ((uint32_t) 1 << 31)

#define CHAN_NAME                   "rpmsg-pru"
#define CHAN_DESC                   "Channel 31"
#define CHAN_PORT                   31

uint8_t payload[RPMSG_BUF_SIZE];
static uart_state_e UART_STATE = UART_STOPPED;
struct pru_rpmsg_transport transport;
uint16_t src, dst;

uint32_t receivedKickEventFrom_ARM(){
    return (__R31 & HOST_INT);
}

void clearEventStatus(){
    CT_INTC.SICR_bit.STS_CLR_IDX = FROM_ARM_HOST;
}

void allowOCP_AccessToPRU(){
    CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;
}

void setup_rpmsg(){
    volatile uint8_t *status;

    allowOCP_AccessToPRU();
    clearEventStatus();

    //Get the status of the rpmsg Linux driver
    status = &resourceTable.rpmsg_vdev.status;
    //Wait for Linux driver to be ready for RPMsg communication
    while (!(*status & VIRTIO_CONFIG_S_DRIVER_OK));
    //Set up communication between ARM Host and PRU
    pru_rpmsg_init(&transport, &resourceTable.rpmsg_vring0, &resourceTable.rpmsg_vring1, TO_ARM_HOST, FROM_ARM_HOST);
    //Create the RPMsg channel between the PRU and ARM user space using the transport structure.
    while (pru_rpmsg_channel(RPMSG_NS_CREATE, &transport, CHAN_NAME, CHAN_DESC, CHAN_PORT) != PRU_RPMSG_SUCCESS);
}
/*
 * return 1 if successful, 0 if not
 */
uint8_t handlePoll(){
    uint16_t len;
    uint8_t returnCode = 0;

    if(receivedKickEventFrom_ARM()){
        clearEventStatus();
        if(pru_rpmsg_receive(&transport, &src, &dst, payload, &len) == PRU_RPMSG_SUCCESS){
            UART_STATE = handle_command((char*)payload,(int)len);
            returnCode = 1;
        }
    }
    return returnCode;
}

void handleDataRead(uint8_t *rxBuffer){
    int idx;

    for(idx = 0; idx < MAX_CAHR_DEV_LEN; idx++){
        rxBuffer[idx] = readData();
    }

    pru_rpmsg_send(&transport, dst, src, rxBuffer, MAX_CAHR_DEV_LEN);
}

void main(){

    uint8_t rxBuffer[MAX_CAHR_DEV_LEN];

    setup_rpmsg();
    setupUART();
    startUART(1);

    // Wait for initial population of source and destination address spaces
    while(!handlePoll());

    while(UART_STATE != UART_POWER_OFF){

        handlePoll();
        //__delay_cycles(100000); // 0.05 s delay
        handleDataRead(rxBuffer);
    }

    startUART(0);
    __halt();
}
