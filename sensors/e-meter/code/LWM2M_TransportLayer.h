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
 @file  LWM2M_TransportLayer.h

 Group: ES

 Target Device: MSP430

 Author: Daniel Stojakov
 ******************************************************************************/

#ifndef LWM2M_TRANSPORTLAYER_H
#define LWM2M_TRANSPORTLAYER_H

/*******************************************************************************
 * INCLUDES
 */
#include "UART_USCI.h"
#include <stdint.h>

/*******************************************************************************
 * CONSTANTS
 */
//Start of frame delimeter
#define SERAPI_SFD                      0xA5

//===============================================================
// GAS = Genaral API Set
//===============================================================
// Functions of the general API Set
#define SERAPI_GAS_FCN_GEN_RET          0x00
#define SERAPI_GAS_FCN_PING             0x10
#define SERAPI_GAS_FCN_CFG_SET          0x20
#define SERAPI_GAS_FCN_CFG_GET          0x21
#define SERAPI_GAS_FCN_CFG_RSP          0x22
#define SERAPI_GAS_FCN_STOP             0x30
#define SERAPI_GAS_FCN_START            0x31
#define SERAPI_GAS_FCN_STAT_GET         0x40
#define SERAPI_GAS_FCN_STAT_RET         0x41
#define SERAPI_GAS_FCN_ERROR_GET        0x50
#define SERAPI_GAS_FCN_ERROR_RET        0x51
#define SERAPI_GAS_FCN_NXT_LAYER_UDP    0xE0
#define SERAPI_GAS_FCN_NXT_LAYER_LWM2M  0xE1

//===============================================================
// LWM2M API Set
//===============================================================

// Functions of the LWM2M API Set
#define SERAPI_LWM2M_FCN_GEN_RET        0x00
#define SERAPI_LWM2M_FCN_CFG_SET        0x20
#define SERAPI_LWM2M_FCN_CFG_GET        0x21
#define SERAPI_LWM2M_FCN_CFG_RET        0x22
#define SERAPI_LWM2M_FCN_STOP           0x30
#define SERAPI_LWM2M_FCN_START          0x31
#define SERAPI_LWM2M_FCN_BS             0x33
#define SERAPI_LWM2M_FCN_STATUS_GET     0x40
#define SERAPI_LWM2M_FCN_STATUS_RET     0x41
#define SERAPI_LWM2M_FCN_ERROR_GET      0x50
#define SERAPI_LWM2M_FCN_ERROR_RET      0x51
#define SERAPI_LWM2M_FCN_OBJ_CREATE     0x60
#define SERAPI_LWM2M_FCN_OBJ_CREATE_XML 0x61
#define SERAPI_LWM2M_FCN_OBJ_RET        0x62
#define SERAPI_LWM2M_FCN_OBJ_DEL        0x63
#define SERAPI_LWM2M_FCN_RES_CREATE     0x70
#define SERAPI_LWM2M_FCN_RES_RET        0x72
#define SERAPI_LWM2M_FCN_RES_DEL        0x73
#define SERAPI_LWM2M_FCN_RES_RD_REQ     0x80
#define SERAPI_LWM2M_FCN_RES_RD_RSP     0x81
#define SERAPI_LWM2M_FCN_RES_WR_REQ     0x82
#define SERAPI_LWM2M_FCN_RES_WR_RSP     0x83
#define SERAPI_LWM2M_FCN_INST_RD_REQ    0x90
#define SERAPI_LWM2M_FCN_INST_RD_RSP    0x91
#define SERAPI_LWM2M_FCN_INST_WR_REQ    0x92
#define SERAPI_LWM2M_FCN_INST_WR_RSP    0x93

/*******************************************************************************
 * TYPEDEFS
 */
//===============================================================
// LWM2M API status / error definitions
//===============================================================
typedef enum
{
  LWM2M_ERROR_OK                    =  (0x00),
  LWM2M_ERROR_ERROR                 =  (0x00),
  LWM2M_ERROR_CMD                   =  (0x00),
  LWM2M_ERROR_PARAM                 =  (0x00),
  LWM2M_STAT_STOPPED                =  (0x30),
  LWM2M_STAT_STARTED                =  (0x31),
  LWM2M_STAT_REGISTERED             =  (0x32),
  LWM2M_STAT_BOOT                   =  (0x33)
}SER_API_LWM2M_ERR_RET_CODES;

//===============================================================
// GAS status / error definitions
//===============================================================
// GAS error/status codes
typedef enum
{
  GAS_ERROR_OK                      =  (0x00),
  GAS_ERROR_ERROR                   =  (0x01),
  GAS_ERROR_CMD                     =  (0x02),
  GAS_ERROR_PARAM                   =  (0x03),
  GAS_STATUS_STOPPED                =  (0x30),
  GAS_STATUS_STARTED                =  (0x31),
  GAS_STATUS_NETWORK                =  (0x32),
  GAS_STATUS_ERROR                  =  (0x3E),
  GAS_STATUS_UDEF                   =  (0x3F)
}SER_API_GAS_ERR_RET_CODES;

// GAS get error status
typedef enum
{
  CLIENT_ERROR_NO                   =  (0x00),
  CLIENT_ERROR_UNKNOWN              =  (0x01),
  CLIENT_ERROR_FATAL                =  (0xFF)
}SER_API_CLIENT_ERR_CODE;

//===============================================================
// Serial API GAS LWM2M client configuration structures / enums
//===============================================================
typedef enum
{
  CFG_ID_MACADDR                   =  (0x00),
  CFG_ID_PANID                     =  (0x01),
  CFG_ID_OPMODE                    =  (0x02),
  CFG_ID_CHANNEL                   =  (0x03)
}SER_API_GAS_SET_GET_CFG_ID;


typedef enum
{
  CFG_OPMODE0                      =  (0x00),
  CFG_OPMODE1                      =  (0x01),
  CFG_OPMODE2                      =  (0x02),
  CFG_OPMODE3                      =  (0x03)
}SER_API_GAS_CFG_OPMODE;

typedef struct
{
  char                    macAddr[8];
  uint16_t                panID;
  SER_API_GAS_CFG_OPMODE  opMode;
  uint8_t                 channel;
}LWM2M_CLIENT_BASE_CONFIG;

//===============================================================
// Serial API LWM2M client LWM2M configuration structures / enums
//===============================================================
typedef enum
{
  CFG_ID_BS_SRV_IP                 =  (0x00),
  CFG_ID_BS_SRV_PORT               =  (0x01),
  CFG_ID_SRV_IP                    =  (0x02),
  CFG_ID_SRV_PORT                  =  (0x03),
  CFG_ID_CLI_NAME                  =  (0x04)
}SER_API_LWM2M_SET_GET_CFG_ID;

typedef struct
{
  uint8_t*                bs_srv_ip;
  uint16_t                bs_srv_port;
  uint8_t*                srv_ip;
  uint16_t                srv_port;
  char                    cli_name[32];
}LWM2M_CLIENT_LWM2M_CONFIG;

//===============================================================
// Serial API Read/Write request/response payload structure
//===============================================================
typedef struct
{
  uint16_t objID;
  uint8_t  instID;
  uint16_t resID;
  uint8_t* data;
  uint8_t  dataLen;
}SER_API_RW_REQ_RSP_PL_t;

//===============================================================
// Serial API GAS/LWM2M configuration structure
//===============================================================
typedef struct
{
  uint8_t  cfgID;
  uint8_t* cfgData;
  uint8_t  cfgDataLen;
}SER_API_CFG_SET_t;

//===============================================================
// Serial API LWM2M XML-Object structure
//===============================================================
typedef struct
{
  uint8_t* xmlData;
  uint8_t  xmlDataLen;
}
LWM2M_XML_OBJ_t;

//===============================================================
// Serial API frame definition
//===============================================================
typedef struct
{
  uint8_t  sfd;
  uint16_t len;
  uint8_t  cmd;
}SER_API_HEADER_t;

typedef struct
{
  SER_API_HEADER_t  header;
  uint8_t*          payload;
  uint16_t          footer;
}SER_API_BASE_FRAME_t;

/*******************************************************************************
 * FUNCTIONS
 */
void SAPI_Init(ST_UART_t* uartObj);

//===============================================================
// Serial GAS functions
//===============================================================
void SAPI_GAS_send_RET(SER_API_GAS_ERR_RET_CODES stat);
void SAPI_GAS_send_PING(void);
void SAPI_GAS_send_CFG_SET(SER_API_CFG_SET_t* config);
void SAPI_GAS_send_CFG_GET(SER_API_GAS_SET_GET_CFG_ID configID);
void SAPI_GAS_send_DEV_START(void);
void SAPI_GAS_send_DEV_STOP(void);
void SAPI_GAS_send_STATUS_GET(void);
void SAPI_GAS_send_ERROR_GET(void);

//===============================================================
// Serial LWM2M functions
//===============================================================
void SAPI_LWM2M_send_CFG_SET(SER_API_CFG_SET_t* config);
void SAPI_LWM2M_send_CFG_GET(uint8_t configID);
void SAPI_LWM2M_send_BS(void);
void SAPI_LWM2M_send_START(void);
void SAPI_LWM2M_send_STATUS_GET(void);
void SAPI_LWM2M_send_ERROR_GET(void);
void SAPI_LWM2M_send_OBJ_CREATE(uint16_t objID, uint8_t instID);
void SAPI_LWM2M_send_OBJ_CREATE_XML(uint8_t instID, LWM2M_XML_OBJ_t* xmlObj);
void SAPI_LWM2M_send_OBJ_DEL(uint16_t objID);
void SAPI_LWM2M_send_RES_CREATE(void);
void SAPI_LWM2M_send_RES_DEL(void);
void SAPI_LWM2M_send_RES_READ_RSP(SER_API_RW_REQ_RSP_PL_t* payload);
void SAPI_LWM2M_send_RES_WRITE_REQ(SER_API_RW_REQ_RSP_PL_t* payload);
void SAPI_LWM2M_send_RES_WRITE_RSP(uint8_t status);
void SAPI_LWM2M_send_INST_READ_RSP(void);
void SAPI_LWM2M_send_INST_WRITE_REQ(void);
void SAPI_LWM2M_send_INST_WRITE_RSP(void);

#endif