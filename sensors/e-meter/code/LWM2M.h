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
 @file  LWM2M.h

 Group: ES

 Target Device: MSP430

 Author: Daniel Stojakov
 ******************************************************************************/

#ifndef LWM2M_H
#define LWM2M_H

/*******************************************************************************
 * INCLUDES
 */
#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
 * CONSTANTS
 */
//===============================================================
//   LWM2M-Object-ID's
//===============================================================
#define LWM2M_POWER_MEAS_OBJ               3305
#define LWM2M_VOLTAGE_MEAS_OBJ             3316
#define LWM2M_CURRENT_MEAS_OBJ             3317

//===============================================================
//   LWM2M-Resource-IDs
//===============================================================
//Power Measurement
#define LWM2M_POWMEAS_INST_ACT_POWER       5800
#define LWM2M_POWMEAS_MIN_MEAS_ACT_POWER   5801
#define LWM2M_POWMEAS_MAX_MEAS_ACT_POWER   5802

//Generic Sensor
#define LWM2M_GENSENS_SENS_VALUE           5700
#define LWM2M_GENSENS_SENS_UNITS           5701
#define LWM2M_GENSENS_MIN_MEAS_VALUE       5601
#define LWM2M_GENSENS_MAX_MEAS_VALUE       5602

//===============================================================
// LWM2M data types and mask definitions
//===============================================================
#define LWM2M_TYPE_UINT8            0x11
#define LWM2M_TYPE_UINT16           0x22
#define LWM2M_TYPE_UINT32           0x43
#define LWM2M_TYPE_INT8             0x14
#define LWM2M_TYPE_INT16            0x25
#define LWM2M_TYPE_INT32            0x46
#define LWM2M_TYPE_FLOAT            0x47
#define LWM2M_TYPE_DOUBLE           0x88
#define LWM2M_TYPE_STRING           0x09
#define LWM2M_TYPE_UNKNOWN          0x0F
#define LWM2M_TYPE_MASK             0x0F
#define LWM2M_LEN_MASK              0xF0

#define LWM2M_ACCESS_MASK           0xF00
#define LWM2M_ACCESS_RD             0x100
#define LWM2M_ACCESS_WR             0x200
#define LWM2M_ACCESS_RD_WR          0x300
#define LWM2M_ACCESS_EX             0x400

/*******************************************************************************
 * TYPEDEFS
 */
//Error codes
typedef enum
{
  ERROR_OBJ_OK                =  (0),
  ERROR_OBJ_NOT_FOUND         =  (-1),
  ERROR_INST_NOT_FOUND        =  (-2),
  ERROR_RES_NOT_FOUND         =  (-3)
}LWM2M_OBJ_ERROR;

//IPSO resource struct
typedef struct
{
  uint16_t       resID;
  uint8_t        accesType;
  bool           mltInst;
  uint8_t        mandatory;
  uint8_t        dataType;
  uint8_t        units;
  uint8_t*       resValue;
}LWM2M_RES_T;

//IPSO instance struct
typedef struct
{
  uint8_t     instID;
  LWM2M_RES_T resource[5];
}LWM2M_INST_T;

//IPSO object struct
typedef struct
{
  uint16_t objectID;
  LWM2M_INST_T Instances[3];
}LWM2M_OBJ_T;

/*******************************************************************************
 * FUNCTIONS
 */
void LWM2M_Init(void);

LWM2M_OBJ_ERROR LWM2M_Set_ResourceValue(uint16_t objID,
                                        uint8_t instID,
                                        uint16_t resID,
                                        void* value);

LWM2M_OBJ_ERROR LWM2M_Get_ResourceValue(uint16_t objID,
                                        uint8_t instID,
                                        uint16_t resID,
                                        uint8_t* value);
#endif
