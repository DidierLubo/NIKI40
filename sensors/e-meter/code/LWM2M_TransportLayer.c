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
/* Hahn-Schickard:	LWM2M
 *******************************************************************************
 * FileName:            LWM2M_Transport_CRC.c
 * Dependencies:        See INCLUDES section below
 * Processor:           MSP430
 *
 * Author               Daniel Stojakov
 ******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "LWM2M_Transport_CRC.h"
#include "LWM2M_TransportLayer.h"
#include "LWM2M.h"
#include "LWM2M_App.h"
#include "UART_USCI.h"

/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */
static ST_UART_t* uart = NULL;

/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static LWM2M_APP_ERROR SAPI_UART_TX(SER_API_BASE_FRAME_t* frame);


/**********************************************************************
 Function:

 LWM2M_APP_ERROR SAPI_UART_TX(SER_API_BASE_FRAME_t* frame)
 ______________________________________________________________________
 Builds and sends a LWM2M Serial frame via UART 
 ______________________________________________________________________
 arguments:

 None
 ______________________________________________________________________
 return:

 LWM2M_APP_ERROR error code
************************************************************************/
static LWM2M_APP_ERROR SAPI_UART_TX(SER_API_BASE_FRAME_t* frame)
{
  uint8_t txBuffer[32] = {0};
  uint8_t i, index = 0;

  txBuffer[index++] = frame->header.sfd;
  txBuffer[index++] = frame->header.len >> 8;
  txBuffer[index++] = frame->header.len >> 0;
  txBuffer[index++] = frame->header.cmd;

  for(i=0; i<(frame->header.len >> 0)-1; i++)
  {
    txBuffer[index++] = *(frame->payload)++;
  }

  txBuffer[index++] = frame->footer >> 8;
  txBuffer[index++] = frame->footer >> 0;

  UART_Put_ByteArray(uart, index, txBuffer);

  return LWM2M_APP_ERROR_OK;
}



/**********************************************************************
 Function:

 LWM2M_APP_ERROR SAPI_Init(ST_UART_t* uartObj)
 ______________________________________________________________________
 Initialization of the Serial API
 ______________________________________________________________________
 arguments:

 None
 ______________________________________________________________________
 return:

 LWM2M_APP_ERROR error code
************************************************************************/
void SAPI_Init(ST_UART_t* uartObj)
{
  if(uartObj != NULL)
  {
    uart = uartObj;
  }
}



//===============================================================
// GAS functions
//===============================================================
/**********************************************************************
 Function:

 LWM2M_APP_ERROR SAPI_GAS_send_RET(SER_API_GAS_STATUS stat)
 ______________________________________________________________________
 Function to build a "GAS Return" frame
 ______________________________________________________________________
 arguments:

 None
 ______________________________________________________________________
 return:

 LWM2M_APP_ERROR error code
************************************************************************/
void SAPI_GAS_send_RET(SER_API_GAS_ERR_RET_CODES stat)
{
  SER_API_BASE_FRAME_t frame;
  uint16_t crcValue = 0;
  uint8_t payloadBuf[2] = {SERAPI_GAS_FCN_GEN_RET,
                           (uint8_t)stat};

  crcValue = crc_calc(0, payloadBuf, sizeof(payloadBuf));

  frame.header.sfd = SERAPI_SFD;
  frame.header.len = sizeof(payloadBuf);
  frame.header.cmd = payloadBuf[0];

  frame.payload = &payloadBuf[1];

  frame.footer = crcValue;

  SAPI_UART_TX(&frame);
}



/**********************************************************************
 Function:

 LWM2M_APP_ERROR SAPI_GAS_send_PING(void)
 ______________________________________________________________________
 Function to build a "GAS Ping" frame
 ______________________________________________________________________
 arguments:

 None
 ______________________________________________________________________
 return:

 LWM2M_APP_ERROR error code
************************************************************************/
void SAPI_GAS_send_PING(void)
{
  SER_API_BASE_FRAME_t frame;
  uint16_t crcValue = 0;
  uint8_t payloadBuf[1] = {SERAPI_GAS_FCN_PING};

  crcValue = crc_calc(0, payloadBuf, sizeof(payloadBuf));

  frame.header.sfd = SERAPI_SFD;
  frame.header.len = sizeof(payloadBuf);
  frame.header.cmd = payloadBuf[0];

  frame.payload = NULL;

  frame.footer = crcValue;

  SAPI_UART_TX(&frame);
}



/**********************************************************************
 Function:

 LWM2M_APP_ERROR SAPI_GAS_send_CFG_SET(SER_API_CFG_SET_t* config)
 ______________________________________________________________________
 Function to build a "GAS Configuration Set" frame
 ______________________________________________________________________
 arguments:

 None
 ______________________________________________________________________
 return:

 LWM2M_APP_ERROR error code
************************************************************************/
void SAPI_GAS_send_CFG_SET(SER_API_CFG_SET_t* config)
{
  SER_API_BASE_FRAME_t frame;
  uint16_t crcValue = 0;
  uint8_t index = 0;

  uint8_t *payloadBuf = malloc((config->cfgDataLen + 2));

  payloadBuf[index++] = SERAPI_GAS_FCN_CFG_SET;
  payloadBuf[index++] = config->cfgID;
  memcpy(payloadBuf+index, config->cfgData, config->cfgDataLen);

  crcValue = crc_calc(0, payloadBuf, (config->cfgDataLen + index));

  frame.header.sfd = SERAPI_SFD;
  frame.header.len = (config->cfgDataLen + index);
  frame.header.cmd = payloadBuf[0];

  frame.payload = &payloadBuf[1];

  frame.footer = crcValue;

  SAPI_UART_TX(&frame);

  free(payloadBuf);
}



/**********************************************************************
 Function:

 LWM2M_APP_ERROR SAPI_GAS_send_CFG_GET(uint8_t configID)
 ______________________________________________________________________
 Function to build a "GAS Configuration Get" frame
 ______________________________________________________________________
 arguments:

 None
 ______________________________________________________________________
 return:

 LWM2M_APP_ERROR error code
************************************************************************/
void SAPI_GAS_send_CFG_GET(SER_API_GAS_SET_GET_CFG_ID configID)
{
  SER_API_BASE_FRAME_t frame;
  uint16_t crcValue = 0;
  uint8_t payloadBuf[2] = {SERAPI_GAS_FCN_CFG_GET,
                           configID};

  crcValue = crc_calc(0, payloadBuf, sizeof(payloadBuf));

  frame.header.sfd = SERAPI_SFD;
  frame.header.len = sizeof(payloadBuf);
  frame.header.cmd = payloadBuf[0];

  frame.payload = &payloadBuf[1];

  frame.footer = crcValue;

  SAPI_UART_TX(&frame);
}



/**********************************************************************
 Function:

 LWM2M_APP_ERROR SAPI_GAS_send_DEV_START(void)
 ______________________________________________________________________
 Function to build a "GAS Device Start" frame
 ______________________________________________________________________
 arguments:

 None
 ______________________________________________________________________
 return:

 LWM2M_APP_ERROR error code
************************************************************************/
void SAPI_GAS_send_DEV_START(void)
{
  SER_API_BASE_FRAME_t frame;
  uint16_t crcValue = 0;
  uint8_t payloadBuf[1] = {SERAPI_GAS_FCN_START};

  crcValue = crc_calc(0, payloadBuf, sizeof(payloadBuf));

  frame.header.sfd = SERAPI_SFD;
  frame.header.len = sizeof(payloadBuf);
  frame.header.cmd = payloadBuf[0];

  frame.payload = NULL;

  frame.footer = crcValue;

  SAPI_UART_TX(&frame);
}



/**********************************************************************
 Function:

 LWM2M_APP_ERROR SAPI_GAS_send_DEV_STOP(void)
 ______________________________________________________________________
 Function to build a "GAS Send Stop" frame
 ______________________________________________________________________
 arguments:

 None
 ______________________________________________________________________
 return:

 LWM2M_APP_ERROR error code
************************************************************************/
void SAPI_GAS_send_DEV_STOP(void)
{
  SER_API_BASE_FRAME_t frame;
  uint16_t crcValue = 0;
  uint8_t payloadBuf[1] = {SERAPI_GAS_FCN_STOP};

  crcValue = crc_calc(0, payloadBuf, sizeof(payloadBuf));

  frame.header.sfd = SERAPI_SFD;
  frame.header.len = sizeof(payloadBuf);
  frame.header.cmd = payloadBuf[0];

  frame.payload = NULL;

  frame.footer = crcValue;

  SAPI_UART_TX(&frame);
}



/**********************************************************************
 Function:

 LWM2M_APP_ERROR SAPI_GAS_send_STATUS_GET(void)
 ______________________________________________________________________
 Function to build a "GAS Status Get" frame
 ______________________________________________________________________
 arguments:

 None
 ______________________________________________________________________
 return:

 LWM2M_APP_ERROR error code
************************************************************************/
void SAPI_GAS_send_STATUS_GET(void)
{
  SER_API_BASE_FRAME_t frame;
  uint16_t crcValue = 0;
  uint8_t payloadBuf[1] = {SERAPI_GAS_FCN_STAT_GET};

  crcValue = crc_calc(0, payloadBuf, sizeof(payloadBuf));

  frame.header.sfd = SERAPI_SFD;
  frame.header.len = sizeof(payloadBuf);
  frame.header.cmd = payloadBuf[0];

  frame.payload = NULL;

  frame.footer = crcValue;

  SAPI_UART_TX(&frame);
}



/**********************************************************************
 Function:

 LWM2M_APP_ERROR SAPI_GAS_send_ERROR_GET(void)
 ______________________________________________________________________
 Function to build a "GAS Error Get" frame
 ______________________________________________________________________
 arguments:

 None
 ______________________________________________________________________
 return:

 LWM2M_APP_ERROR error code
************************************************************************/
void SAPI_GAS_send_ERROR_GET(void)
{
  SER_API_BASE_FRAME_t frame;
  uint16_t crcValue = 0;
  uint8_t payloadBuf[1] = {SERAPI_GAS_FCN_ERROR_GET};

  crcValue = crc_calc(0, payloadBuf, sizeof(payloadBuf));

  frame.header.sfd = SERAPI_SFD;
  frame.header.len = sizeof(payloadBuf);
  frame.header.cmd = payloadBuf[0];

  frame.payload = NULL;

  frame.footer = crcValue;

  SAPI_UART_TX(&frame);
}



//===============================================================
// LWM2M functions
//===============================================================
/**********************************************************************
 Function:

 LWM2M_APP_ERROR SAPI_LWM2M_send_CFG_SET(SER_API_CFG_SET_t* config)
 ______________________________________________________________________
 Function to build a "LWM2M Configuration Set" frame
 ______________________________________________________________________
 arguments:

 None
 ______________________________________________________________________
 return:

 LWM2M_APP_ERROR error code
************************************************************************/
void SAPI_LWM2M_send_CFG_SET(SER_API_CFG_SET_t* config)
{
  SER_API_BASE_FRAME_t frame;
  uint16_t crcValue = 0;
  uint8_t index = 0;

  uint8_t *payloadBuf = malloc((config->cfgDataLen + 3));

  payloadBuf[index++] = SERAPI_GAS_FCN_NXT_LAYER_LWM2M;
  payloadBuf[index++] = SERAPI_LWM2M_FCN_CFG_SET;
  payloadBuf[index++] = config->cfgID;
  memcpy(payloadBuf+index, config->cfgData, config->cfgDataLen);

  crcValue = crc_calc(0, payloadBuf, (config->cfgDataLen + index));

  frame.header.sfd = SERAPI_SFD;
  frame.header.len = (config->cfgDataLen + index);
  frame.header.cmd = payloadBuf[0];

  frame.payload = &payloadBuf[1];

  frame.footer = crcValue;

  SAPI_UART_TX(&frame);

  free(payloadBuf);
}



/**********************************************************************
 Function:

 LWM2M_APP_ERROR SAPI_LWM2M_send_CFG_GET(uint8_t configID)
 ______________________________________________________________________
 Function to build a "LWM2M Configuration Get" frame
 ______________________________________________________________________
 arguments:

 None
 ______________________________________________________________________
 return:

 LWM2M_APP_ERROR error code
************************************************************************/
void SAPI_LWM2M_send_CFG_GET(uint8_t configID)
{
  SER_API_BASE_FRAME_t frame;
  uint16_t crcValue = 0;
  uint8_t payloadBuf[3] = {SERAPI_GAS_FCN_NXT_LAYER_LWM2M,
                           SERAPI_LWM2M_FCN_CFG_GET,
                           configID};

  crcValue = crc_calc(0, payloadBuf, sizeof(payloadBuf));

  frame.header.sfd = SERAPI_SFD;
  frame.header.len = sizeof(payloadBuf);
  frame.header.cmd = SERAPI_GAS_FCN_NXT_LAYER_LWM2M;

  frame.payload = &payloadBuf[1];

  frame.footer = crcValue;

  SAPI_UART_TX(&frame);
}



/**********************************************************************
 Function:

 LWM2M_APP_ERROR SAPI_LWM2M_send_BS(void)
 ______________________________________________________________________
 Function to build a "LWM2M Initiate a Bootstrap Procedure" frame
 ______________________________________________________________________
 arguments:

 None
 ______________________________________________________________________
 return:

 LWM2M_APP_ERROR error code
************************************************************************/
void SAPI_LWM2M_send_BS(void)
{
  SER_API_BASE_FRAME_t frame;
  uint16_t crcValue = 0;
  uint8_t payloadBuf[2] = {SERAPI_GAS_FCN_NXT_LAYER_LWM2M,
                           SERAPI_LWM2M_FCN_BS};

  crcValue = crc_calc(0, payloadBuf, sizeof(payloadBuf));

  frame.header.sfd = SERAPI_SFD;
  frame.header.len = sizeof(payloadBuf);
  frame.header.cmd = SERAPI_GAS_FCN_NXT_LAYER_LWM2M;

  frame.payload = &payloadBuf[1];

  frame.footer = crcValue;

  SAPI_UART_TX(&frame);
}



/**********************************************************************
 Function:

 LWM2M_APP_ERROR SAPI_LWM2M_send_START(void)
 ______________________________________________________________________
 Function to build a "LWM2M Start" frame
 ______________________________________________________________________
 arguments:

 None
 ______________________________________________________________________
 return:

 LWM2M_APP_ERROR error code
************************************************************************/
void SAPI_LWM2M_send_START(void)
{
  SER_API_BASE_FRAME_t frame;
  uint16_t crcValue = 0;
  uint8_t payloadBuf[2] = {SERAPI_GAS_FCN_NXT_LAYER_LWM2M,
                           SERAPI_LWM2M_FCN_START};

  crcValue = crc_calc(0, payloadBuf, sizeof(payloadBuf));

  frame.header.sfd = SERAPI_SFD;
  frame.header.len = sizeof(payloadBuf);
  frame.header.cmd = SERAPI_GAS_FCN_NXT_LAYER_LWM2M;

  frame.payload = &payloadBuf[1];

  frame.footer = crcValue;

  SAPI_UART_TX(&frame);
}



/**********************************************************************
 Function:

 LWM2M_APP_ERROR SAPI_LWM2M_send_STATUS_GET(void)
 ______________________________________________________________________
 Function to build a "LWM2M Status Get" frame
 ______________________________________________________________________
 arguments:

 None
 ______________________________________________________________________
 return:

 LWM2M_APP_ERROR error code
************************************************************************/
void SAPI_LWM2M_send_STATUS_GET(void)
{
  SER_API_BASE_FRAME_t frame;
  uint16_t crcValue = 0;
  uint8_t payloadBuf[2] = {SERAPI_GAS_FCN_NXT_LAYER_LWM2M,
                           SERAPI_LWM2M_FCN_STATUS_GET};

  crcValue = crc_calc(0, payloadBuf, sizeof(payloadBuf));

  frame.header.sfd = SERAPI_SFD;
  frame.header.len = sizeof(payloadBuf);
  frame.header.cmd = SERAPI_GAS_FCN_NXT_LAYER_LWM2M;

  frame.payload = &payloadBuf[1];

  frame.footer = crcValue;

  SAPI_UART_TX(&frame);
}



/**********************************************************************
 Function:

 LWM2M_APP_ERROR SAPI_LWM2M_send_ERROR_GET(void)
 ______________________________________________________________________
 Function to build a "LWM2M Error Get" frame
 ______________________________________________________________________
 arguments:

 None
 ______________________________________________________________________
 return:

 LWM2M_APP_ERROR error code
************************************************************************/
void SAPI_LWM2M_send_ERROR_GET(void)
{
  SER_API_BASE_FRAME_t frame;
  uint16_t crcValue = 0;
  uint8_t payloadBuf[2] = {SERAPI_GAS_FCN_NXT_LAYER_LWM2M,
                           SERAPI_LWM2M_FCN_ERROR_GET};

  crcValue = crc_calc(0, payloadBuf, sizeof(payloadBuf));

  frame.header.sfd = SERAPI_SFD;
  frame.header.len = sizeof(payloadBuf);
  frame.header.cmd = SERAPI_GAS_FCN_NXT_LAYER_LWM2M;

  frame.payload = &payloadBuf[1];

  frame.footer = crcValue;

  SAPI_UART_TX(&frame);
}



/**********************************************************************
 Function:

 LWM2M_APP_ERROR SAPI_LWM2M_send_OBJ_CREATE(uint16_t objID, 
                                            uint8_t instID)
 ______________________________________________________________________
 Function to build a "LWM2M Object Create" frame
 ______________________________________________________________________
 arguments:

 uint16_t objID -> Object ID
 uint8_t instID -> Instance ID
 ______________________________________________________________________
 return:

 LWM2M_APP_ERROR error code
************************************************************************/
void SAPI_LWM2M_send_OBJ_CREATE(uint16_t objID, 
                                uint8_t  instID)
{
  SER_API_BASE_FRAME_t frame;
  uint16_t crcValue = 0;
  uint8_t payloadBuf[5] = {SERAPI_GAS_FCN_NXT_LAYER_LWM2M,
                           SERAPI_LWM2M_FCN_OBJ_CREATE,
                           (objID >> 8),
                           (objID >> 0),
                           instID};

  crcValue = crc_calc(0, payloadBuf, sizeof(payloadBuf));

  frame.header.sfd = SERAPI_SFD;
  frame.header.len = sizeof(payloadBuf);
  frame.header.cmd = SERAPI_GAS_FCN_NXT_LAYER_LWM2M;

  frame.payload = &payloadBuf[1];

  frame.footer = crcValue;

  SAPI_UART_TX(&frame);
}



/**********************************************************************
 Function:

 LWM2M_APP_ERROR SAPI_LWM2M_send_OBJ_CREATE_XML(void)
 ______________________________________________________________________
 Function to build a "LWM2M Object Create" frame
 ______________________________________________________________________
 arguments:

 None
 ______________________________________________________________________
 return:

 LWM2M_APP_ERROR error code
************************************************************************/
void SAPI_LWM2M_send_OBJ_CREATE_XML(uint8_t instID, LWM2M_XML_OBJ_t* xmlObj)
{
  SER_API_BASE_FRAME_t frame;
  uint16_t crcValue = 0;
  uint8_t index = 0;

  uint8_t *payloadBuf = malloc((xmlObj->xmlDataLen + 3));

  payloadBuf[index++] = SERAPI_GAS_FCN_NXT_LAYER_LWM2M;
  payloadBuf[index++] = SERAPI_LWM2M_FCN_OBJ_CREATE_XML;
  payloadBuf[index++] = instID;
  memcpy(payloadBuf+index, xmlObj->xmlData, xmlObj->xmlDataLen);

  crcValue = crc_calc(0, payloadBuf, (xmlObj->xmlDataLen + index));

  frame.header.sfd = SERAPI_SFD;
  frame.header.len = (xmlObj->xmlDataLen + index);
  frame.header.cmd = payloadBuf[0];

  frame.payload = &payloadBuf[1];

  frame.footer = crcValue;

  SAPI_UART_TX(&frame);

  free(payloadBuf);
}



/**********************************************************************
 Function:

 LWM2M_APP_ERROR SAPI_LWM2M_send_OBJ_DEL(uint16_t objID)
 ______________________________________________________________________
 Function to build a "LWM2M Object Delete" frame
 ______________________________________________________________________
 arguments:

 uint16_t objID -> Object ID
 ______________________________________________________________________
 return:

 LWM2M_APP_ERROR error code
************************************************************************/
void SAPI_LWM2M_send_OBJ_DEL(uint16_t objID)
{
  SER_API_BASE_FRAME_t frame;
  uint16_t crcValue = 0;
  uint8_t payloadBuf[4] = {SERAPI_GAS_FCN_NXT_LAYER_LWM2M,
                           SERAPI_LWM2M_FCN_OBJ_DEL,
                           (objID >> 8),
                           (objID >> 0)};

  crcValue = crc_calc(0, payloadBuf, sizeof(payloadBuf));

  frame.header.sfd = SERAPI_SFD;
  frame.header.len = sizeof(payloadBuf);
  frame.header.cmd = SERAPI_GAS_FCN_NXT_LAYER_LWM2M;

  frame.payload = &payloadBuf[1];

  frame.footer = crcValue;

  SAPI_UART_TX(&frame);
}



/**********************************************************************
 Function:

 LWM2M_APP_ERROR SAPI_LWM2M_send_RES_CREATE(void)
 ______________________________________________________________________
 Function to build a "LWM2M Resource Create" frame
 ______________________________________________________________________
 arguments:

 None
 ______________________________________________________________________
 return:

 LWM2M_APP_ERROR error code
************************************************************************/
void SAPI_LWM2M_send_RES_CREATE(void)
{
  //TBD
}



/**********************************************************************
 Function:

 LWM2M_APP_ERROR SAPI_LWM2M_send_RES_DEL(void)
 ______________________________________________________________________
 Function to build a "LWM2M Resource Delete" frame
 ______________________________________________________________________
 arguments:

 None
 ______________________________________________________________________
 return:

 LWM2M_APP_ERROR error code
************************************************************************/
void SAPI_LWM2M_send_RES_DEL(void)
{
  //TBD
}



/**********************************************************************
 Function:

 LWM2M_APP_ERROR SAPI_LWM2M_send_RES_READ_RSP(SER_API_RW_REQ_RSP_PL_t* payload)
 ______________________________________________________________________
 Function to build a "LWM2M Resource Read Response" frame
 ______________________________________________________________________
 arguments:

 SER_API_RW_REQ_RSP_PL_t* payload -> payload containing: Object ID
                                                         Resource ID
                                                         Instance ID
                                                         Instance Data
 ______________________________________________________________________
 return:

 LWM2M_APP_ERROR error code
************************************************************************/
void SAPI_LWM2M_send_RES_READ_RSP(SER_API_RW_REQ_RSP_PL_t* payload)
{
  SER_API_BASE_FRAME_t frame;
  uint16_t crcValue = 0;
  uint8_t index = 0;

  uint8_t *payloadBuf = malloc((payload->dataLen + 7));

  payloadBuf[index++] = SERAPI_GAS_FCN_NXT_LAYER_LWM2M;
  payloadBuf[index++] = SERAPI_LWM2M_FCN_RES_RD_RSP;
  payloadBuf[index++] = (payload->objID >> 8);
  payloadBuf[index++] = (payload->objID >> 0);
  payloadBuf[index++] = payload->instID;
  payloadBuf[index++] = (payload->resID >> 8);
  payloadBuf[index++] = (payload->resID >> 0);
  memcpy(payloadBuf+index, payload->data, payload->dataLen);

  crcValue = crc_calc(0, payloadBuf, (payload->dataLen + index));

  frame.header.sfd = SERAPI_SFD;
  frame.header.len = (payload->dataLen + index);
  frame.header.cmd = payloadBuf[0];

  frame.payload = &payloadBuf[1];

  frame.footer = crcValue;

  SAPI_UART_TX(&frame);

  free(payloadBuf);
}



/**********************************************************************
 Function:

 LWM2M_APP_ERROR SAPI_LWM2M_send_RES_WRITE_REQ(SER_API_RW_REQ_RSP_PL_t* payload)
 ______________________________________________________________________
 Function to build a "LWM2M Resource Write Request" frame
 ______________________________________________________________________
 arguments:

 SER_API_RW_REQ_RSP_PL_t* payload -> payload containing: Object ID
                                                         Resource ID
                                                         Instance ID
                                                         Instance Data
 ______________________________________________________________________
 return:

 LWM2M_APP_ERROR error code
************************************************************************/
void SAPI_LWM2M_send_RES_WRITE_REQ(SER_API_RW_REQ_RSP_PL_t* payload)
{
  SER_API_BASE_FRAME_t frame;
  uint16_t crcValue = 0;
  uint8_t index = 0;

  uint8_t *payloadBuf = malloc((payload->dataLen + 7));

  payloadBuf[index++] = SERAPI_GAS_FCN_NXT_LAYER_LWM2M;
  payloadBuf[index++] = SERAPI_LWM2M_FCN_RES_WR_REQ;
  payloadBuf[index++] = (payload->objID >> 8);
  payloadBuf[index++] = (payload->objID >> 0);
  payloadBuf[index++] = payload->instID;
  payloadBuf[index++] = (payload->resID >> 8);
  payloadBuf[index++] = (payload->resID >> 0);
  memcpy(payloadBuf+index, payload->data, payload->dataLen);

  crcValue = crc_calc(0, payloadBuf, (payload->dataLen + index));

  frame.header.sfd = SERAPI_SFD;
  frame.header.len = (payload->dataLen + index);
  frame.header.cmd = payloadBuf[0];

  frame.payload = &payloadBuf[1];

  frame.footer = crcValue;

  SAPI_UART_TX(&frame);

  free(payloadBuf);
}



/**********************************************************************
 Function:

 LWM2M_APP_ERROR SAPI_LWM2M_send_RES_WRITE_RSP(uint8_t status)
 ______________________________________________________________________
 Function to build a "LWM2M Resource Write Response" frame
 ______________________________________________________________________
 arguments:

 uint8_t status -> Status code
 ______________________________________________________________________
 return:

 LWM2M_APP_ERROR error code
************************************************************************/
void SAPI_LWM2M_send_RES_WRITE_RSP(uint8_t status)
{
  SER_API_BASE_FRAME_t frame;
  uint16_t crcValue = 0;
  uint8_t payloadBuf[3] = {SERAPI_GAS_FCN_NXT_LAYER_LWM2M,
                           SERAPI_LWM2M_FCN_RES_WR_RSP,
                           status};

  crcValue = crc_calc(0, payloadBuf, sizeof(payloadBuf));

  frame.header.sfd = SERAPI_SFD;
  frame.header.len = sizeof(payloadBuf);
  frame.header.cmd = SERAPI_GAS_FCN_NXT_LAYER_LWM2M;

  frame.payload = &payloadBuf[1];

  frame.footer = crcValue;

  SAPI_UART_TX(&frame);
}



/**********************************************************************
 Function:

 LWM2M_APP_ERROR SAPI_LWM2M_send_INST_READ_RSP(void)
 ______________________________________________________________________
 Function to build a "LWM2M Instance Read Response" frame
 ______________________________________________________________________
 arguments:

 None
 ______________________________________________________________________
 return:

 LWM2M_APP_ERROR error code
************************************************************************/
void SAPI_LWM2M_send_INST_READ_RSP(void)
{
  //TBD
}



/**********************************************************************
 Function:

 LWM2M_APP_ERROR SAPI_LWM2M_send_INST_WRITE_REQ(void)
 ______________________________________________________________________
 Function to build a "LWM2M Instance Write Request" frame
 ______________________________________________________________________
 arguments:

 None
 ______________________________________________________________________
 return:

 LWM2M_APP_ERROR error code
************************************************************************/
void SAPI_LWM2M_send_INST_WRITE_REQ(void)
{
  //TBD
}



/**********************************************************************
 Function:

 LWM2M_APP_ERROR SAPI_LWM2M_send_INST_WRITE_RSP(void)
 ______________________________________________________________________
 Function to build a "LWM2M Instance Write Response" frame
 ______________________________________________________________________
 arguments:

 None
 ______________________________________________________________________
 return:

 LWM2M_APP_ERROR error code
************************************************************************/
void SAPI_LWM2M_send_INST_WRITE_RSP(void)
{
  //TBD
}

