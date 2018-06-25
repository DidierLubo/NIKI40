/*
 * arm_pru_message.c
 *
 *  Created on: 21.10.2016
 *      Author: Fesseha Mamo
 *  Reworked on: 20.04.2018
 *      Author: Dovydas Girdvainis
 */

#include <string.h>
#include <stdlib.h>
#include "pru_uart_driver.h"
#include "arm_pru_message.h"


#define START_LEN                      5u
#define STOP_LEN                       4u
#define BAUD_LEN                       4u
#define POWER_OFF_LEN                  3u
#define BAUD_VAL_LEN                   5u
#define MIN_BAUD_LEN                   4u
#define MAX_BAUD_LEN                   9u


uart_state_e handle_command(char* payload, int len) {
	uart_state_e uart_new_state = UART_STOPPED;

	if(!strncmp(payload, "start", START_LEN)) {
	    startUART(1);
		uart_new_state = UART_RUNNING;
	}
	else if(!strncmp(payload, "stop", STOP_LEN)) {
	    startUART(0);
		uart_new_state = UART_STOPPED;
	}
	else if(!strncmp(payload, "baud", BAUD_LEN)) {
		char speed[MAX_BAUD_LEN];
		uint32_t baudrate = 0;
		if(payload[BAUD_LEN] == '=' && len - BAUD_VAL_LEN >= MIN_BAUD_LEN) {
			strncpy(speed, &payload[BAUD_VAL_LEN], len - BAUD_VAL_LEN);
			baudrate = atoi(speed);
			changeUartBaudrate(baudrate);
			uart_new_state = UART_RUNNING;
		}
	}
	else if(!strncmp(payload, "off", POWER_OFF_LEN)) {
		uart_new_state = UART_POWER_OFF;
	}

	return uart_new_state;
}
