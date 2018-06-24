/*
        Copyright 2016-2018 NIKI 4.0 project team
        NIKI 4.0 was financed by the Baden-Württemberg Stiftung gGmbH (www.bwstiftung.de).
        Project partners are FZI Forschungszentrum Informatik am Karlsruher
        Institut für Technologie (www.fzi.de), Hahn-Schickard-Gesellschaft
        für angewandte Forschung e.V. (www.hahn-schickard.de) and
        Hochschule Offenburg (www.hs-offenburg.de).
        This file was developed by FZI Forschungszentrum Informatik.
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
/******************************************************************************
 @file  LWM2M_App.h

 Group: ES

 Target Device: MSP430

 Author: Daniel Stojakov
 ******************************************************************************/

#ifndef LWM2M_APP_H
#define LWM2M_APP_H

/*******************************************************************************
 * INCLUDES
 */
#include "UART_USCI.h"
#include "LWM2M_TransportLayer.h"
#include <stdint.h>

/*******************************************************************************
 * CONSTANTS
 */
#define LWM2M_TASK_NO_TASK             0x0000
#define LWM2M_TASK_UART_RX             0x0001
#define LWM2M_TASK_UART_TX             0x0002
#define LWM2M_TASK_WR_REQ              0x0004

/*******************************************************************************
 * GLOBAL VARIABLES
 */
extern volatile uint16_t LWM2M_Task;

/*******************************************************************************
 * TYPEDEFS
 */
typedef enum
{
  LWM2M_APP_ERROR_OK                    = 0,
  LWM2M_APP_ERROR_SFD                   = -1,
  LWM2M_APP_ERROR_CRC                   = -2,
  LWM2M_APP_ERROR_UNDEF_CMD             = -3,
  LWM2M_APP_ERROR_INIT                  = -4,
  LWM2M_APP_ERROR_SAPI_TO               = -5,
  LWM2M_APP_ERROR_WR_REQ_FAILED         = -6
}LWM2M_APP_ERROR;

typedef struct
{
  uint8_t                    clientStatus;
  uint8_t                    clientError;
  uint8_t                    lwm2mStatus;
  uint8_t                    lwm2mError;
  LWM2M_CLIENT_BASE_CONFIG   clientBaseConfig;
  LWM2M_CLIENT_LWM2M_CONFIG  clientLWM2MConfig;
}LWM2M_CLIENT_DEV_t;

/*******************************************************************************
 * FUNCTIONS
 */
LWM2M_APP_ERROR LWM2M_App_Init(ST_UART_t* uartObj);
LWM2M_APP_ERROR LWM2M_App_Task(void);

#endif