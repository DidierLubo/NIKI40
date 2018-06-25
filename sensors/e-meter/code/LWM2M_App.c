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

/******************************************************************************
 @file  LWM2M_App.c

 Group: ES

 Target Device: MSP430

 Author: Daniel Stojakov
 ******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#include "LWM2M.h"
#include "LWM2M_App.h"
#include "HAL_Dogs102x6.h"
#include "emeter-structs.h"
#include "Timer_Modul.h"
#include "msp430f67791.h"
#include "LWM2M_Transport_CRC.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/*******************************************************************************
 * CONSTANTS
 */
#define LWM2M_DEFAULT_CLI_NAME          "E-Meter-LWM2M" // max 32-Stellen

#define LWM2M_DEFAULT_BS_SRV_PORT       5683   // Max 16 stellen
#define LWM2M_DEFAULT_SRV_PORT          5683   // Max 16 stellen

#define LWM2M_DEFAULT_MAC_ADR           { 0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x00 }
#define LWM2M_DEFAULT_PANID             0x1633
#define LWM2M_DEFAULT_OPMODE            0x01
#define LWM2M_DEFAULT_CHANNEL           0x00
   
#define LWM2M_TIMEOUT_50MS              10
#define LWM2M_TIMEOUT_5000MS            500

#define NMB_INSTANCES                   1

/*******************************************************************************
 * TYPEDEFS
 */


/*******************************************************************************
 * GLOBAL VARIABLES
 */
volatile uint16_t LWM2M_Task;

/*******************************************************************************
 * LOCAL VARIABLES
 */

//static uint8_t LWM2M_DEFAULT_SRV_IP[16] = {0xBB, 0xBB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0xA3, 0x16, 0xFF, 0xFE, 0xED, 0x9C, 0x19};
static uint8_t LWM2M_DEFAULT_BS_SRV_IP[16] = {0xBB, 0xBB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0xA3, 0x16, 0xFF, 0xFE, 0xED, 0x9C, 0x19};
static uint8_t LWM2M_DEFAULT_SRV_IP[16] = {0xBB, 0xBB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x86, 0xEB, 0x18, 0xFF, 0xFE, 0xED, 0x7E, 0x67};
//static uint8_t LWM2M_DEFAULT_SRV_IP[16] = {0xBB, 0xBB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0xA3, 0x16, 0xFF, 0xFE, 0xB5, 0xAD, 0xE9};
static ST_UART_t* uart = NULL;
static LWM2M_CLIENT_DEV_t clientDevice =
{
    0,      //status code
    0,      //error code
    0,      //lwm2m status code
    0,      //lwm2m error code
    {
        {0xDE, 0xAD, 0xBE, 0xAF, 0x00, 0x01}, //mac address
        0xCAFE, //PAN ID
        CFG_OPMODE1, //Opmpde
        26 //Channel
    },
    {
        LWM2M_DEFAULT_BS_SRV_IP,
        LWM2M_DEFAULT_BS_SRV_PORT,
        LWM2M_DEFAULT_SRV_IP,
        LWM2M_DEFAULT_SRV_PORT,
        LWM2M_DEFAULT_CLI_NAME
    }
};

char outString1[sizeof(LWM2M_DEFAULT_CLI_NAME) + 4] = "CN: E-Meter-LWM2M";
char outString2[12]  = "IP-Address:";
char outString3[sizeof(LWM2M_DEFAULT_SRV_IP) + 1]  = "BBBB000000000000";
//char outString4[sizeof(LWM2M_DEFAULT_SRV_IP) + 1]  = "86EB18FFFEED7E67";
char outString4[sizeof(LWM2M_DEFAULT_SRV_IP) + 1]  = "86EB18FFFEED7EE9";
      
/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static bool process_Client_Response(uint8_t bytesExpected, uint32_t timeOut);
static LWM2M_APP_ERROR config_ClientDevice(void);
static LWM2M_APP_ERROR config_LWM2M(void);
static LWM2M_APP_ERROR Init_LWM2M_Client(void);
static LWM2M_APP_ERROR SAPI_GAS_Frame_Interpreter(uint8_t* frame, uint8_t len);
static LWM2M_APP_ERROR SAPI_LWM2M_Frame_Interpreter(uint8_t* frame, uint8_t len);
static LWM2M_APP_ERROR LWM2M_ESIMA_WR_RQ_Task(void);
static void GAS_Handle_GEN_RET(uint8_t retStatus);
static void GAS_Handle_PING(void);
static void GAS_Handle_CFG_RSP(uint8_t* buffer, uint8_t len);
static void GAS_Handle_STATUS_RSP(uint8_t status);
static void GAS_Handle_ERROR_RSP(uint8_t error);
static void LWM2M_Handle_CFG_RSP(uint8_t *buffer, uint8_t len);
static void LWM2M_Handle_STATUS_RSP(uint8_t status);
static void LWM2M_Handle_ERROR_RSP(uint8_t error);
static void LWM2M_Handle_OBJ_RSP(uint8_t* buffer, uint8_t len);
static void LWM2M_Handle_RES_RSP(uint8_t* buffer, uint8_t len);
static void LWM2M_Handle_RES_READ_REQ(uint8_t* buffer, uint8_t len);
static void LWM2M_Handle_RES_WRITE_REQ(uint8_t* buffer, uint8_t len);
static void LWM2M_Handle_RES_WRITE_RSP(uint8_t status);



/**********************************************************************
 Function:

 static bool process_Client_Response(uint8_t bytesExpected, 
                                     uint16_t timeOut)
 ______________________________________________________________________
 Waits for expected number of bytes reaching the UART input buffer and 
 interpretes the frame sent by the LWM2M client before an user specific
 timeout occures
 ______________________________________________________________________
 arguments:

 uint8_t bytesExpected -> number of expected bytes to wait for
 uint16_t timeOut      -> response timeout value
 ______________________________________________________________________
 return:

 GENERAL_ERR_CODE error code
************************************************************************/
static bool process_Client_Response(uint8_t  bytesExpected, 
                                    uint32_t timeOut)
{
  static uint8_t rxBuffer[64];
  uint8_t rxByteCnt = 0;

  SetTimeoutValue(timeOut);
  
#ifdef UART_UCA3_DMA_ENABLE
  UCA3_UART_RX_DMA_Read_Data(rxBuffer, bytesExpected);
#else
  
  do
  {
    if(GetTimeoutValue() == 0)
    {
      return false;
    }

    rxByteCnt = UART_Get_RXD_Anzahl(uart);
  }while(rxByteCnt < bytesExpected);

  UART_Get_ByteArray(uart, rxBuffer, rxByteCnt);
#endif

  SAPI_GAS_Frame_Interpreter(rxBuffer, rxByteCnt);  
    
  return true;
}



/**********************************************************************
 Function:

 static LWM2M_APP_ERROR config_ClientDevice(void)
 ______________________________________________________________________
 Initializes the LWM2M clients general functions
 ______________________________________________________________________
 arguments:

 None
 ______________________________________________________________________
 return:

 GENERAL_ERR_CODE error code
************************************************************************/
static LWM2M_APP_ERROR config_ClientDevice(void)
{
  LWM2M_APP_ERROR errCode = LWM2M_APP_ERROR_OK;

  UART_Clear_E_Buffer(uart);
  
/******************* Start step 1 *******************/
  SAPI_GAS_send_PING();
  
  if(!process_Client_Response(7, LWM2M_TIMEOUT_50MS) || //wait for RET (OK)
     clientDevice.clientStatus != GAS_ERROR_OK)
  {
    Dogs102x6_clearRow(3);
    Dogs102x6_stringDraw(2, 0, "Status: ST1 Fail", DOGS102x6_DRAW_NORMAL);
    return LWM2M_APP_ERROR_INIT;//Init failed
  }
  
/******************* Start step 2 *******************/
  SAPI_GAS_send_DEV_STOP();

  if(!process_Client_Response(7, LWM2M_TIMEOUT_50MS) || //wait for RET (OK)
     clientDevice.clientStatus != GAS_ERROR_OK)
  {
    Dogs102x6_clearRow(3);
    Dogs102x6_stringDraw(2, 0, "Status: ST2 Fail", DOGS102x6_DRAW_NORMAL);
    return LWM2M_APP_ERROR_INIT;//Init failed
  }

  if(!process_Client_Response(7, LWM2M_TIMEOUT_50MS) || //wait for STATUS_RET (STOPPED)
     clientDevice.clientStatus != GAS_STATUS_STOPPED)
  {
    Dogs102x6_clearRow(3);
    Dogs102x6_stringDraw(2, 0, "Status: ST3 Fail", DOGS102x6_DRAW_NORMAL);
    return LWM2M_APP_ERROR_INIT;//Init failed
  }
  
  if(!process_Client_Response(8, LWM2M_TIMEOUT_50MS) || //wait for STATUS_RET (LWM2M STOPPED)
     clientDevice.lwm2mStatus != LWM2M_STAT_STOPPED)
  {
    Dogs102x6_clearRow(3);
    Dogs102x6_stringDraw(2, 0, "Status: ST4 Fail", DOGS102x6_DRAW_NORMAL);
    return LWM2M_APP_ERROR_INIT;//Init failed
  }
  
#ifdef LWM2M_FULL_INIT
/******************* Start step 3 *******************/
  SER_API_CFG_SET_t config;

//========== set client mac address ==========
  config.cfgID      = CFG_ID_MACADDR;
  config.cfgData    = (uint8_t*)&clientDevice.clientBaseConfig.macAddr;
  config.cfgDataLen = sizeof(clientDevice.clientBaseConfig.macAddr);

  SAPI_GAS_send_CFG_SET(&config);

  if(!process_Client_Response(7, LWM2M_TIMEOUT_50MS) || //wait for RET (OK)
     clientDevice.clientStatus != GAS_ERROR_OK)
  {
    Dogs102x6_clearRow(3);
    Dogs102x6_stringDraw(2, 0, "Status: ST5 Fail", DOGS102x6_DRAW_NORMAL);
    return LWM2M_APP_ERROR_INIT;//Init failed
  }

//========== set client PAN-ID ==========
  config.cfgID      = CFG_ID_PANID;
  config.cfgData    = (uint8_t*)&clientDevice.clientBaseConfig.panID;
  config.cfgDataLen = sizeof(clientDevice.clientBaseConfig.panID);

  SAPI_GAS_send_CFG_SET(&config);

  if(!process_Client_Response(7, LWM2M_TIMEOUT_50MS) || //wait for RET (OK)
     clientDevice.clientStatus != GAS_ERROR_OK)
  {
    Dogs102x6_clearRow(3);
    Dogs102x6_stringDraw(2, 0, "Status: ST6 Fail", DOGS102x6_DRAW_NORMAL);
    return LWM2M_APP_ERROR_INIT;//Init failed
  }
  
//========== set client opmode ==========
  config.cfgID      = CFG_ID_OPMODE;
  config.cfgData    = (uint8_t*)&clientDevice.clientBaseConfig.opMode;
  config.cfgDataLen = sizeof(clientDevice.clientBaseConfig.opMode);

  SAPI_GAS_send_CFG_SET(&config);

  if(!process_Client_Response(7, LWM2M_TIMEOUT_50MS) || //wait for RET (OK)
     clientDevice.clientStatus != GAS_ERROR_OK)
  {
    Dogs102x6_clearRow(3);
    Dogs102x6_stringDraw(2, 0, "Status: ST7 Fail", DOGS102x6_DRAW_NORMAL);
    return LWM2M_APP_ERROR_INIT;//Init failed
  }

//========== set client channel ==========
  config.cfgID      = CFG_ID_CHANNEL;
  config.cfgData    = (uint8_t*)&clientDevice.clientBaseConfig.channel;
  config.cfgDataLen = sizeof(clientDevice.clientBaseConfig.channel);

  SAPI_GAS_send_CFG_SET(&config);

  if(!process_Client_Response(7, LWM2M_TIMEOUT_50MS) || //wait for RET (OK)
     clientDevice.clientStatus != GAS_ERROR_OK)
  {
    Dogs102x6_clearRow(3);
    Dogs102x6_stringDraw(2, 0, "Status: ST8 Fail", DOGS102x6_DRAW_NORMAL);
    return LWM2M_APP_ERROR_INIT;//Init failed
  }
#endif
    
  return errCode;
}



/**********************************************************************
 Function:

 static LWM2M_APP_ERROR config_LWM2M(void)
 ______________________________________________________________________
 Initializes the LWM2M clients LWM2M functions
 ______________________________________________________________________
 arguments:

 None
 ______________________________________________________________________
 return:

 GENERAL_ERR_CODE error code
************************************************************************/
static LWM2M_APP_ERROR config_LWM2M(void)
{
  LWM2M_APP_ERROR errCode = LWM2M_APP_ERROR_OK;

/******************* Start step 1 *******************/
  SER_API_CFG_SET_t config;

//========== set LWM2M Client Name ==========
  config.cfgID      = CFG_ID_CLI_NAME;
  config.cfgData    = (uint8_t*)&clientDevice.clientLWM2MConfig.cli_name;
  config.cfgDataLen = 13;

  SAPI_LWM2M_send_CFG_SET(&config);

  if(!process_Client_Response(8, LWM2M_TIMEOUT_50MS) || //wait for RET (OK)
     clientDevice.clientStatus != GAS_ERROR_OK)
  {
    Dogs102x6_clearRow(3);
    Dogs102x6_stringDraw(2, 0, "Status: ST9 Fail", DOGS102x6_DRAW_NORMAL);
    return LWM2M_APP_ERROR_INIT;//Init failed
  }

//========== set LWM2M Server IP ==========
//  config.cfgID      = CFG_ID_SRV_IP;
//  config.cfgData    = clientDevice.clientLWM2MConfig.srv_ip;
//  config.cfgDataLen = sizeof(LWM2M_DEFAULT_SRV_IP);
//
//  SAPI_LWM2M_send_CFG_SET(&config);
//
//  if(!process_Client_Response(8, LWM2M_TIMEOUT_50MS) || //wait for RET (OK)
//     clientDevice.clientStatus != GAS_ERROR_OK)
//  {
//    Dogs102x6_clearRow(3);
//    Dogs102x6_stringDraw(2, 0, "Status: ST10 Fail", DOGS102x6_DRAW_NORMAL);
//    return LWM2M_APP_ERROR_INIT;//Init failed
//  }

#ifdef LWM2M_FULL_INIT
//========== set LWM2M Server Port ==========
  config.cfgID      = CFG_ID_SRV_PORT;
  config.cfgData    = (uint8_t*)&clientDevice.clientLWM2MConfig.srv_port;
  config.cfgDataLen = sizeof(uint16_t);

  SAPI_GAS_send_CFG_SET(&config);

  if(!process_Client_Response(7) || //wait for RET (OK)
     clientDevice.clientStatus != GAS_ERROR_OK)
  {
    Dogs102x6_clearRow(3);
    Dogs102x6_stringDraw(2, 0, "Status: ST11 Fail", DOGS102x6_DRAW_NORMAL);
    return LWM2M_APP_ERROR_INIT;//Init failed
  }
#endif
  
/******************* Start step 3 *******************/
  
  SAPI_GAS_send_DEV_START();

  if(!process_Client_Response(7, LWM2M_TIMEOUT_50MS) || //wait for RET (OK)
     clientDevice.clientStatus != GAS_ERROR_OK)
  {
    Dogs102x6_clearRow(3);
    Dogs102x6_stringDraw(2, 0, "Status:  ST12 Fail", DOGS102x6_DRAW_NORMAL);
    return LWM2M_APP_ERROR_INIT;//Init failed
  }
  
////skip the 37 'BEGIN' bytes
//  process_Client_Response(37, LWM2M_TIMEOUT_50MS);
    
  if(!process_Client_Response(7, LWM2M_TIMEOUT_50MS) ||  //wait for STATUS_RET (STARTED)
     clientDevice.clientStatus != GAS_STATUS_STARTED)
  {
    Dogs102x6_clearRow(3);
    Dogs102x6_stringDraw(2, 0, "Status: ST13 Fail", DOGS102x6_DRAW_NORMAL);
    return LWM2M_APP_ERROR_INIT;//Init failed
  }

  if(!process_Client_Response(8, LWM2M_TIMEOUT_50MS) ||  //wait for LWM2M STATUS_RET (STARTED)
     clientDevice.lwm2mStatus != LWM2M_STAT_STARTED)
  {
    Dogs102x6_clearRow(3);
    Dogs102x6_stringDraw(2, 0, "Status: ST14 Fail", DOGS102x6_DRAW_NORMAL);
    return LWM2M_APP_ERROR_INIT;//Init failed
  }
 
  if(!process_Client_Response(7, 6000) ||  //wait for STATUS_RET (REGISTERED) 10min
     clientDevice.clientStatus != GAS_STATUS_NETWORK)
  {
    Dogs102x6_clearRow(3);
    Dogs102x6_stringDraw(2, 0, "Status: ST16 Fail", DOGS102x6_DRAW_NORMAL);
    return LWM2M_APP_ERROR_INIT;//Init failed
  }
  
//  if(!process_Client_Response(7, 60000) ||  //wait for STATUS_RET (REGISTERED) 10min
//     clientDevice.lwm2mStatus != LWM2M_STAT_REGISTERED)
//  {
//    Dogs102x6_clearRow(3);
//    Dogs102x6_stringDraw(2, 0, "Status: ST16 Fail", DOGS102x6_DRAW_NORMAL);
//    return LWM2M_APP_ERROR_INIT;//Init failed
//  }
  
  clientDevice.lwm2mStatus = LWM2M_STAT_REGISTERED;
  
  return errCode;
}



/**********************************************************************
 Function:

 static LWM2M_APP_CMD_INTPR_ERROR Init_LWM2M_Client(void)
 ______________________________________________________________________
 Initializition function of the LWM2M Client device
 ______________________________________________________________________
 arguments:

 None
 ______________________________________________________________________
 return:

 GENERAL_ERR_CODE error code
************************************************************************/
static LWM2M_APP_ERROR Init_LWM2M_Client(void)
{
  LWM2M_APP_ERROR errCode = LWM2M_APP_ERROR_OK;

  if(config_ClientDevice() != LWM2M_APP_ERROR_OK)
  {
    return LWM2M_APP_ERROR_INIT;
  }

  if(config_LWM2M() != LWM2M_APP_ERROR_OK)
  {
    return LWM2M_APP_ERROR_INIT;
  }

  return errCode;
}



/**********************************************************************
 Function:

 static LWM2M_APP_CMD_INTPR_ERROR SAPI_GAS_Frame_Interpreter(uint8_t* frame,
                                                             uint8_t len)
 ______________________________________________________________________
 Interpretes an incoming Serial GAS frame
 ______________________________________________________________________
 arguments:

 uint8_t* frame -> received frame buffer
 uint8_t len    -> received frame buffer length
 ______________________________________________________________________
 return:

 GENERAL_ERR_CODE error code
************************************************************************/
static LWM2M_APP_ERROR SAPI_GAS_Frame_Interpreter(uint8_t* frame,
                                                  uint8_t  len)
{
  LWM2M_APP_ERROR errCode = LWM2M_APP_ERROR_SFD;
  
  if(*frame++ == SERAPI_SFD)//SAPI-> SFD
  {
    uint16_t SAPI_length = 0;

    SAPI_length |= (uint16_t)(*frame++ << 8); //SAPI-> LEN[0]
    SAPI_length |= (uint16_t)*frame++;        //SAPI-> LEN[1]

    *(frame+SAPI_length) = ~*(frame+SAPI_length);
    *(frame+SAPI_length+1) = ~*(frame+SAPI_length+1);
    
    if(~crc_calc(0, frame, SAPI_length + 2) != 0)
    {
      return LWM2M_APP_ERROR_CRC;
    }
	
    switch(*frame++)//SAPI-> CMD
    {
      case SERAPI_GAS_FCN_GEN_RET:
      {
        GAS_Handle_GEN_RET(*frame);
      }
      break;

      case SERAPI_GAS_FCN_PING:
      {
        GAS_Handle_PING();
      }
      break;

      case SERAPI_GAS_FCN_CFG_RSP:
      {
        GAS_Handle_CFG_RSP(frame, (SAPI_length-1));
      }
      break;

      case SERAPI_GAS_FCN_STAT_RET:
      {
        GAS_Handle_STATUS_RSP(*frame);
      }
      break;

      case SERAPI_GAS_FCN_ERROR_RET:
      {
        GAS_Handle_ERROR_RSP(*frame);
      }
      break;

      case SERAPI_GAS_FCN_NXT_LAYER_LWM2M:
      {
        SAPI_LWM2M_Frame_Interpreter(frame, (SAPI_length-1));
      }
      break;

      default:
      {
        errCode = LWM2M_APP_ERROR_UNDEF_CMD;
      }
      break;
    };
  }

  return errCode;
}



/**********************************************************************
 Function:

 static LWM2M_APP_CMD_INTPR_ERROR SAPI_LWM2M_Frame_Interpreter(uint8_t* frame,
                                                               uint8_t len)
 ______________________________________________________________________
 Interpretes an incoming Serial LWM2M frame
 ______________________________________________________________________
 arguments:

 uint8_t* frame -> received frame buffer
 uint8_t len    -> received frame buffer length
 ______________________________________________________________________
 return:

 GENERAL_ERR_CODE error code
************************************************************************/
static LWM2M_APP_ERROR SAPI_LWM2M_Frame_Interpreter(uint8_t* frame,
                                                    uint8_t  len)
{
  LWM2M_APP_ERROR errCode = LWM2M_APP_ERROR_SFD;

  switch(*frame++)
  {
    case SERAPI_LWM2M_FCN_GEN_RET:
    {
      GAS_Handle_GEN_RET(*frame);
    }
    break;
    case SERAPI_LWM2M_FCN_CFG_RET:
    {
      LWM2M_Handle_CFG_RSP(frame, len-1);
    }
    break;

    case SERAPI_LWM2M_FCN_STATUS_RET:
    {
      LWM2M_Handle_STATUS_RSP(*frame);
    }
    break;

    case SERAPI_LWM2M_FCN_ERROR_RET:
    {
      LWM2M_Handle_ERROR_RSP(*frame);
    }
    break;

    case SERAPI_LWM2M_FCN_OBJ_RET:
    {
      LWM2M_Handle_OBJ_RSP(frame, len-1);
    }
    break;

    case SERAPI_LWM2M_FCN_RES_RET:
    {
      LWM2M_Handle_RES_RSP(frame, len-1);
    }
    break;

    case SERAPI_LWM2M_FCN_RES_RD_REQ:
    {
      LWM2M_Handle_RES_READ_REQ(frame, len-1);
    }
    break;

    case SERAPI_LWM2M_FCN_RES_WR_REQ:
    {
      LWM2M_Handle_RES_WRITE_REQ(frame, len-1);
    }
    break;

    case SERAPI_LWM2M_FCN_RES_WR_RSP:
    {
      LWM2M_Handle_RES_WRITE_RSP(*frame);
    }
    break;

    case SERAPI_LWM2M_FCN_INST_RD_REQ:
    {
      //future implementation
    }
    break;

    case SERAPI_LWM2M_FCN_INST_WR_REQ:
    {
      //future implementation
    }
    break;

    case SERAPI_LWM2M_FCN_INST_WR_RSP:
    {
      //future implementation
    }
    break;

    default:
    {
      errCode = LWM2M_APP_ERROR_UNDEF_CMD;
    }
    break;
  };

  return errCode;
}



/**********************************************************************
 Function:

 static void GAS_Handle_GEN_RET(uint8_t retStatus)
 ______________________________________________________________________
 Handles a "GAS General Return" frame from the LWM2M client
 ______________________________________________________________________
 arguments:

 uint8_t retStatus -> return status code
 ______________________________________________________________________
 return:

 None
************************************************************************/
static void GAS_Handle_GEN_RET(uint8_t retStatus)
{
  clientDevice.clientStatus = retStatus;
}



/**********************************************************************
 Function:

 static void GAS_Handle_PING(void)
 ______________________________________________________________________
 Handles a "GAS Ping" frame from the LWM2M client
 ______________________________________________________________________
 arguments:

 None
 ______________________________________________________________________
 return:

 None
************************************************************************/
static void GAS_Handle_PING(void)
{
  SAPI_GAS_send_RET(GAS_ERROR_OK);
}



/**********************************************************************
 Function:

 static void GAS_Handle_CFG_RSP(uint8_t *buffer,
                                uint8_t len)
 ______________________________________________________________________
 Handles a "GAS Configuration Response" frame from the LWM2M client
 ______________________________________________________________________
 arguments:

 uint8_t *buffer -> received frame buffer
 uint8_t len     -> received frame buffer length
 ______________________________________________________________________
 return:

 None
************************************************************************/
static void GAS_Handle_CFG_RSP(uint8_t* buffer,
                               uint8_t  len)
{
  switch(*buffer++)//SAPI-> CFG_ID
  {
    case CFG_ID_MACADDR:
    {
      memcpy(clientDevice.clientBaseConfig.macAddr,
             buffer,
             len);
    }
    break;

    case CFG_ID_PANID:
    {
      clientDevice.clientBaseConfig.panID |= (uint16_t)(*buffer++ << 8);
      clientDevice.clientBaseConfig.panID |= (uint16_t)(*buffer   << 0);
    }
    break;

    case CFG_ID_OPMODE:
    {
      clientDevice.clientBaseConfig.opMode = (SER_API_GAS_CFG_OPMODE)*buffer;
    }
    break;

    case CFG_ID_CHANNEL:
    {
      clientDevice.clientBaseConfig.channel = *buffer;
    }
    break;

    default:
    {
      //do nothing
    }
    break;
  }
}



/**********************************************************************
 Function:

 static void GAS_Handle_STATUS_RSP(uint8_t status)
 ______________________________________________________________________
 Handles a "GAS Status Response" frame from the LWM2M client
 ______________________________________________________________________
 arguments:

 uint8_t status -> status code
 ______________________________________________________________________
 return:

 None
************************************************************************/
static void GAS_Handle_STATUS_RSP(uint8_t status)
{
  clientDevice.clientStatus = status;
}



/**********************************************************************
 Function:

 static void GAS_Handle_ERROR_RSP(uint8_t error)
 ______________________________________________________________________
 Handles a "GAS Error Response" frame from the LWM2M client
 ______________________________________________________________________
 arguments:

 uint8_t error -> error code
 ______________________________________________________________________
 return:

 None
************************************************************************/
static void GAS_Handle_ERROR_RSP(uint8_t error)
{
  clientDevice.clientError = error;
}



/**********************************************************************
 Function:

 static void LWM2M_Handle_CFG_RSP(uint8_t *buffer,
                                  uint8_t len)
 ______________________________________________________________________
 Handles a "LWM2M Configuration Response" frame from the LWM2M client
 ______________________________________________________________________
 arguments:

 uint8_t *buffer -> received frame buffer
 uint8_t len     -> received frame buffer length
 ______________________________________________________________________
 return:

 None
************************************************************************/
static void LWM2M_Handle_CFG_RSP(uint8_t* buffer,
                                 uint8_t  len)
{
  switch(*buffer++)//SAPI-> CFG_ID
  {
    case CFG_ID_BS_SRV_IP:
    {
      memcpy(clientDevice.clientLWM2MConfig.bs_srv_ip, buffer, len);
    }
    break;

    case CFG_ID_BS_SRV_PORT:
    {
      clientDevice.clientLWM2MConfig.bs_srv_port |= (uint16_t)(*buffer++ << 8);
      clientDevice.clientLWM2MConfig.bs_srv_port |= (uint16_t)(*buffer << 0);
    }
    break;

    case CFG_ID_SRV_IP:
    {
      memcpy(clientDevice.clientLWM2MConfig.srv_ip, buffer, len);
    }
    break;

    case CFG_ID_SRV_PORT:
    {
      clientDevice.clientLWM2MConfig.srv_port |= (uint16_t)(*buffer++ << 8);
      clientDevice.clientLWM2MConfig.srv_port |= (uint16_t)(*buffer << 0);
    }
    break;

    case CFG_ID_CLI_NAME:
    {
      memcpy(clientDevice.clientLWM2MConfig.cli_name, buffer, len);
    }
    break;

    default:
    {
      //do nothing
    }
    break;
  }
}



/**********************************************************************
 Function:

 static void LWM2M_Handle_STATUS_RSP(uint8_t status)
 ______________________________________________________________________
 Handles a "LWM2M Status Response" frame from the LWM2M client
 ______________________________________________________________________
 arguments:

 uint8_t status -> status code
 ______________________________________________________________________
 return:

 None
************************************************************************/
static void LWM2M_Handle_STATUS_RSP(uint8_t status)
{
  clientDevice.lwm2mStatus = status;
}



/**********************************************************************
 Function:

 static void LWM2M_Handle_ERROR_RSP(uint8_t error)
 ______________________________________________________________________
 Handles a "LWM2M Error Response" frame from the LWM2M client
 ______________________________________________________________________
 arguments:

 uint8_t error -> error code
 ______________________________________________________________________
 return:

 None
************************************************************************/
static void LWM2M_Handle_ERROR_RSP(uint8_t error)
{
  clientDevice.clientError = error;
}



/**********************************************************************
 Function:

 static void LWM2M_Handle_OBJ_RSP(uint8_t *buffer,
                                  uint8_t len)
 ______________________________________________________________________
 Handles a "LWM2M Object Response" frame from the LWM2M client
 ______________________________________________________________________
 arguments:

 uint8_t *buffer -> recevied frame buffer
 uint8_t len     -> recevied frame buffer length
 ______________________________________________________________________
 return:

 None
************************************************************************/
static void LWM2M_Handle_OBJ_RSP(uint8_t* buffer,
                                 uint8_t  len)
{
  clientDevice.clientStatus = *buffer+(len-1);
}



/**********************************************************************
 Function:

 static void LWM2M_Handle_RES_RSP(uint8_t *buffer,
                                  uint8_t len)
 ______________________________________________________________________
 Handles a "LWM2M Resource Response" frame from the LWM2M client
 ______________________________________________________________________
 arguments:

 uint8_t *buffer -> recevied frame buffer
 uint8_t len     -> recevied frame buffer length
 ______________________________________________________________________
 return:

 None
************************************************************************/
static void LWM2M_Handle_RES_RSP(uint8_t* buffer,
                                 uint8_t len)
{
  //future implementation
}



/**********************************************************************
 Function:

 static void LWM2M_Handle_RES_READ_REQ(uint8_t *buffer,
                                       uint8_t len)
 ______________________________________________________________________
 Handles a "LWM2M Resource Read Request" frame from the LWM2M client
 ______________________________________________________________________
 arguments:

 uint8_t *buffer -> recevied frame buffer
 uint8_t len     -> recevied frame buffer length
 ______________________________________________________________________
 return:

 None
************************************************************************/
static void LWM2M_Handle_RES_READ_REQ(uint8_t* buffer, 
                                      uint8_t  len)
{
  uint16_t objID = 0;
  uint8_t instID = 0;
  uint16_t resID = 0;
  float measValue =0.0f;
  SER_API_RW_REQ_RSP_PL_t payload;

  objID = (uint16_t)(*buffer++ << 8);
  objID |= (uint16_t)(*buffer++ << 0);
  instID = *buffer++;
  resID = (uint16_t)(*buffer++ << 8);
  resID |= (uint16_t)(*buffer++ << 0);

  LWM2M_Get_ResourceValue(objID, instID,
                          resID,(uint8_t*)&measValue);

  payload.objID   = objID;
  payload.instID  = instID;
  payload.resID   = resID;
  payload.data    = (uint8_t*)&measValue;
  payload.dataLen = sizeof(float);

  SAPI_LWM2M_send_RES_READ_RSP(&payload);
}



/**********************************************************************
 Function:

 static void LWM2M_Handle_RES_WRITE_REQ(uint8_t *buffer,
                                        uint8_t len)
 ______________________________________________________________________
 Handles a "LWM2M Resource Write Request" frame from the LWM2M client
 ______________________________________________________________________
 arguments:

 uint8_t *buffer -> recevied frame buffer
 uint8_t len     -> recevied frame buffer length
 ______________________________________________________________________
 return:

 None
************************************************************************/
static void LWM2M_Handle_RES_WRITE_REQ(uint8_t* buffer, 
                                       uint8_t len)
{
  uint16_t objID = 0;
  uint8_t instID = 0;
  uint16_t resID = 0;

  objID  = (uint16_t)(*buffer++ << 8);
  objID |= (uint16_t)(*buffer++ << 0);
  instID = *buffer++;
  resID  = (uint16_t)(*buffer++ << 8);
  resID |= (uint16_t)(*buffer++ << 0);

  LWM2M_Set_ResourceValue(objID, instID, resID, buffer);

  SAPI_LWM2M_send_RES_WRITE_RSP(LWM2M_ERROR_OK);
}



/**********************************************************************
 Function:

 static void LWM2M_Handle_RES_WRITE_RSP(uint8_t status)
 ______________________________________________________________________
 Handles a "LWM2M Resource Write Response" frame from the LWM2M client
 ______________________________________________________________________
 arguments:

 uint8_t status -> status code
 ______________________________________________________________________
 return:

 None
************************************************************************/
static void LWM2M_Handle_RES_WRITE_RSP(uint8_t status)
{
    SAPI_GAS_send_RET(status);
}



/**********************************************************************
 Function:

 static void LWM2M_ESIMA_WR_RQ_Task(void)
 ______________________________________________________________________
 Handles a pending write request task after new measurements were taken
 ______________________________________________________________________
 arguments:

 None
 ______________________________________________________________________
 return:

 None
************************************************************************/
static LWM2M_APP_ERROR LWM2M_ESIMA_WR_RQ_Task(void)
{
  uint8_t i;
  SER_API_RW_REQ_RSP_PL_t payload;
  uint32_t data2Send = 0;
  LWM2M_APP_ERROR errCode = LWM2M_APP_ERROR_OK;

//============= Object Power instance 1-3 =============
//  payload.objID = LWM2M_POWER_MEAS_OBJ;
//  payload.resID = LWM2M_POWMEAS_INST_ACT_POWER;
//
//  for(i=0; i<NMB_INSTANCES; i++)
//  {
//    payload.instID = i;
//
//    LWM2M_Get_ResourceValue(payload.objID,
//                            payload.instID,
//                            payload.resID,
//                            (uint8_t*)&data2Send);
//
//    payload.data = (uint8_t*)&data2Send;
//    payload.dataLen = sizeof(float);
//
//    SAPI_LWM2M_send_RES_WRITE_REQ(&payload);
//
//    if(!process_Client_Response(7, LWM2M_TIMEOUT_50MS) ||
//       clientDevice.clientStatus != GAS_ERROR_OK)
//    {
//      Dogs102x6_clearRow(2);
//      Dogs102x6_stringDraw(2, 0, "Status: WRQ Fail", DOGS102x6_DRAW_NORMAL);
//      return LWM2M_APP_ERROR_WR_REQ_FAILED;//WRQ failed
//    }
//  }

//============ Object Voltage instance 1-3 ============
  payload.objID = LWM2M_VOLTAGE_MEAS_OBJ;
  payload.resID = LWM2M_GENSENS_SENS_VALUE;

  for(i=0; i<NMB_INSTANCES; i++)
  {
    payload.instID = i;

    LWM2M_Get_ResourceValue(payload.objID,
                            payload.instID,
                            payload.resID,
                            (uint8_t*)&data2Send);

    payload.data = (uint8_t*)&data2Send;
    payload.dataLen = sizeof(float);

    SAPI_LWM2M_send_RES_WRITE_REQ(&payload);

    if(!process_Client_Response(8, LWM2M_TIMEOUT_50MS) ||
       clientDevice.clientStatus != GAS_ERROR_OK)
    {
      Dogs102x6_clearRow(2);
      Dogs102x6_stringDraw(2, 0, "Status: WRQ Fail", DOGS102x6_DRAW_NORMAL);
      return LWM2M_APP_ERROR_WR_REQ_FAILED;//WRQ failed
    }
  }

//=========== Object Current instance 1-3 ============
  payload.objID = LWM2M_CURRENT_MEAS_OBJ;
  payload.resID = LWM2M_GENSENS_SENS_VALUE;

  for(i=0; i<NMB_INSTANCES; i++)
  {
    payload.instID = i;

    LWM2M_Get_ResourceValue(payload.objID,
                            payload.instID,
                            payload.resID,
                            (uint8_t*)&data2Send);

    payload.data = (uint8_t*)&data2Send;
    payload.dataLen = sizeof(float);

    SAPI_LWM2M_send_RES_WRITE_REQ(&payload);

    if(!process_Client_Response(8, LWM2M_TIMEOUT_50MS) ||
       clientDevice.clientStatus != GAS_ERROR_OK)
    {
      Dogs102x6_clearRow(2);
      Dogs102x6_stringDraw(2, 0, "Status: WRQ Fail", DOGS102x6_DRAW_NORMAL);
      return LWM2M_APP_ERROR_WR_REQ_FAILED;//WRQ failed
    }
  }

  Dogs102x6_clearRow(2);
  Dogs102x6_stringDraw(2, 0, "Status: OK", DOGS102x6_DRAW_NORMAL);

  return errCode;
}



/**********************************************************************
 Function:

 void LWM2M_App_Init(ST_UART_t* uartObj)
 ______________________________________________________________________
 Initialization of the LWM2M application
 ______________________________________________________________________
 arguments:

 ST_UART_t* uartObj -> pointer to previously initialised UART object
 ______________________________________________________________________
 return:

 None
************************************************************************/
LWM2M_APP_ERROR LWM2M_App_Init(ST_UART_t* uartObj)
{
  LWM2M_APP_ERROR errCode = LWM2M_APP_ERROR_OK;
  
  if(uartObj != NULL)
  {
    uart = uartObj;
    
    SAPI_Init(uartObj);
    LWM2M_Init();
    errCode = Init_LWM2M_Client();
    
    if(errCode == LWM2M_APP_ERROR_OK)
    {
      Dogs102x6_clearRow(2);
      Dogs102x6_stringDraw(2, 0, "Status: OK", DOGS102x6_DRAW_NORMAL);
      Dogs102x6_stringDraw(4, 0, outString1, DOGS102x6_DRAW_NORMAL);
//      Dogs102x6_stringDraw(5, 0, outString2, DOGS102x6_DRAW_NORMAL);
//      Dogs102x6_stringDraw(6, 0, outString3, DOGS102x6_DRAW_NORMAL);
//      Dogs102x6_stringDraw(7, 0, outString4, DOGS102x6_DRAW_NORMAL);
    }
  }
  
  LWM2M_Task &= ~LWM2M_TASK_UART_RX;
	
  return errCode;
}



/**********************************************************************
 Function:

 void LWM2M_App_Task(void)
 ______________________________________________________________________
 Handles all LWM2M <-> UART tasks
 ______________________________________________________________________
 arguments:

 None
 ______________________________________________________________________
 return:

 None
************************************************************************/
LWM2M_APP_ERROR LWM2M_App_Task(void)
{
  LWM2M_APP_ERROR errCode = LWM2M_APP_ERROR_INIT;
  
  if(LWM2M_Task == LWM2M_TASK_NO_TASK) //UART tx event
  {
    if(clientDevice.lwm2mStatus == LWM2M_STAT_REGISTERED)
    {
      errCode = LWM2M_APP_ERROR_OK;
    }
  }
  
  if(LWM2M_Task & LWM2M_TASK_UART_TX) //UART tx event
  {
    LWM2M_Task &= ~LWM2M_TASK_UART_TX;
  }

  if(LWM2M_Task & LWM2M_TASK_UART_RX) //CLIENT request/response event
  {
    uint8_t rxByteCnt = 0;
    uint16_t payloadLen = 0;
    uint8_t rxBuf[32] = {0};
    
    LWM2M_Task &= ~LWM2M_TASK_UART_RX;
        
    UART_Get_ByteArray(uart, rxBuf, 3);
    payloadLen = rxBuf[1] << 8 | rxBuf[2];

    SetTimeoutValue(10); //100ms
    
    do
    {
      if(GetTimeoutValue() == 0)
      {
        break;
      }

      rxByteCnt = UART_Get_RXD_Anzahl(uart);
    }while(rxByteCnt < payloadLen + 2);    
    
    if(rxByteCnt == payloadLen + 2)
    {
      UART_Get_ByteArray(uart, rxBuf+3, rxByteCnt);
      SAPI_GAS_Frame_Interpreter(rxBuf, rxByteCnt);
    }
    else
    {
      Dogs102x6_clearRow(2);
      Dogs102x6_stringDraw(2, 0, "Status: Frame Err", DOGS102x6_DRAW_NORMAL);   
    }
  }
  
  if(LWM2M_Task & LWM2M_TASK_WR_REQ)
  {
    LWM2M_Task &= ~LWM2M_TASK_WR_REQ;
      
    LWM2M_ESIMA_WR_RQ_Task();
    
    errCode = LWM2M_APP_ERROR_OK;
  }
  
  return errCode;
}

