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
/******************************************************************************************
 * uart_driver.c
 *
 * Rework of the 16C550 compliant UART0 driver according to
 * AM335x and AM1808 datasheets
 *
 *  Created on: 19.04.2018
 *      Author: Dovydas Girdvainis
 */

#include "pru_uart_driver.h"
#include <pru_cfg.h>
#include <pru_uart.h>

//-----------------------------------------------------------
/*              Internal function prototypes               */
void configureUartBaudrate(uint32_t baudrate);
void configureFIFO(void);
void configureLCR(void);
void configureIER(void);
uint8_t isUART_TX_Empty(void);
uint8_t isUART_RX_Empty(void);
//-----------------------------------------------------------

void setupUART(void){

    configureUartBaudrate(9600);
    configureIER();
    configureFIFO();
    configureLCR();
}

/*
 * Configure the baudrate values
 * BAUDRATE     |   Error % |
 * 9600         |   0.00    |
 * 19200        |   0.00    |
 * 45450        |   0.01    |
 * 93750        |   0.00    |
 * 187500       |   0.00    |
 * 500000       |   0.00    |
 * 1500000      |   0.00    |
 * 3000000      |   0.00    |
 * 6000000      |   0.00    |
 * 12000000     |   0.00    |
 *
 * default - 9600
 *
 * Calculated for 192 MHz
 */
void configureUartBaudrate(uint32_t baudrate){

    switch(baudrate){
        case 19200 : {
            CT_UART.DLH = 0x02;
            CT_UART.DLL = 0x71;
        } break;
        case 45450 : {
            CT_UART.DLH = 0x01;
            CT_UART.DLL =  0x08;
        } break;
        case 93750 : {
            CT_UART.DLL =  0x80;
        } break;
        case 187500 : {
            CT_UART.DLL =  0x40;
        } break;
        case 500000 : {
            CT_UART.DLL =  0x18;
        } break;
        case 1500000 : {
            CT_UART.DLL =  0x08;
        } break;
        case 3000000 : {
            CT_UART.DLL =  0x04;
        } break;
        case 6000000 : {
            CT_UART.DLL =  0x02;
        } break;
        case 12000000 : {
            CT_UART.DLL =  0x01;
        } break;
        default : {
            CT_UART.DLH = 0x04;
            CT_UART.DLL = 0xE2;
        }
    }

    CT_UART.MDR = 0x0;
}

void changeUartBaudrate(uint32_t baudrate){
    startUART(0);
    configureUartBaudrate(baudrate);
    startUART(1);
}

void configureFIFO(void) {
    /*            DMAMODE1   TXCLR      RXCLR      FIFOEN  */
    CT_UART.FCR = (0x1<<3) | (0x1<<2) | (0x1<<1) | (0x1);
}
/*
 * No brake control, stick parity is disabled, even parity selected,
 * parity bit is generated, 1 stop bit is generated,
 * word length is 8 bits
 */
void configureLCR(void){
    /*            EPS        PEN        WLS */
    CT_UART.LCR = (0x1<<4) | (0x1<<3) | (0x3);
}
/*
 * Receiver line status interrupt enabled,
 * Receiver data available interrupt enabled
 */
void configureIER(void){
    /*            ELSI       ETBEI      ERBI */
    CT_UART.IER = (0x1<<2) | (0x1<<1) | (0x1);
}
/*
 * Enable/disable the loopback mode
 * 0 - disabled
 * 1 - enabled
 *
 * default - disabled
 */
void enableLoopback(int8_t flag){
    switch(flag){
        case 1 : {
            CT_UART.MCR |= (0x1<<4);
        } break;
        default : {
            CT_UART.MCR &= (0xFFFFFFEF);
        }
    }
}

uint8_t readData(void){
    //Wait until we received some data
    //while(isUART_RX_Empty());
    while ((CT_UART.LSR & 0x1) == 0x0);
    return CT_UART.RBR;
}

/*
 * writes a message to the UART TX buffer
 */
void writeData(volatile uint8_t* msg, int bufferSize){
    uint8_t i;

    for(i=0; i<bufferSize; i++){
        //Wait until data transmission has been finished
        //while(!isUART_TX_Empty());
        while (!((CT_UART.FCR & 0x2) == 0x2));
        CT_UART.THR=msg[i];
    }
}

/*
 * Start/Stop the UART
 * 0 - stop the UART
 * 1 - start the UART
 *
 * default - stopped
 */
void startUART(int8_t flag){

    switch(flag){
        case 1 : {
            /*                    UTRST       URRST       FREE  */
            CT_UART.PWREMU_MGMT = (0x1<<14) | (0x1<<13) | (0x1);
        } break;
        default : {
            CT_UART.PWREMU_MGMT &= 0xFFFF9FFE;
        }
    }

}

uint8_t isUART_TX_Empty(void) {
    return (((CT_UART.FCR & 0x2)==0x2)? 1:0);
}

uint8_t isUART_RX_Empty(void) {
    return (((CT_UART.LSR & 0x1)==0x0)? 1:0);
}
