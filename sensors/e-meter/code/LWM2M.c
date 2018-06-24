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
 * FileName:            LWM2M.c
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

#include "LWM2M.h"

/*******************************************************************************
 * CONSTANTS
 */
#define LWM2M_EMETER_OBJ_NMB                                    3
#define LWM2M_EMETER_INST_NMB                                   3
#define LWM2M_EMETER_RES_NMB                                    3

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */
static LWM2M_OBJ_T Emeter_LWM2M_Objects[LWM2M_EMETER_OBJ_NMB];

//Measurements for Phase 1
static struct
{
  float   absPow;
  float   minPow;
  float   maxPow;
  float   absI;
  float   minI;
  float   maxI;
  float   absU;
  float   minU;
  float   maxU;
}phase1;

//Measurements for Phase 2
static struct
{
  float   absPow;
  float   minPow;
  float   maxPow;
  float   absI;
  float   minI;
  float   maxI;
  float   absU;
  float   minU;
  float   maxU;
}phase2;

//Measurements for Phase 3
static struct
{
  float   absPow;
  float   minPow;
  float   maxPow;
  float   absI;
  float   minI;
  float   maxI;
  float   absU;
  float   minU;
  float   maxU;
}phase3;



/**********************************************************************
 Function:

 void LWM2M_Init(void)
 ______________________________________________________________________
  Initializes the IPSO defined objects to be used by the LWM2M Client.
  This Objects "Power Measurement, Voltage Measurement, Current Measurement"
  are representing the E-Meter sensor capabilities.
 ______________________________________________________________________
 arguments:

 None
 ______________________________________________________________________
 return:

 None
************************************************************************/
void LWM2M_Init(void)
{
  /* ===== Init power measurement object ===== */
    LWM2M_OBJ_T powObj;

    powObj.objectID = LWM2M_POWER_MEAS_OBJ; //IPSO object ID "Power Measurement"

    //Instance 0 for Ph1 power measurements
    powObj.Instances[0].instID = 0; //IPSO instance ID "0"
    powObj.Instances[0].resource[0].resID = LWM2M_POWMEAS_INST_ACT_POWER; //IPSO resource ID "5800"
    powObj.Instances[0].resource[0].resValue = (uint8_t *)&phase1.absPow;
    powObj.Instances[0].resource[1].resID = LWM2M_POWMEAS_MIN_MEAS_ACT_POWER; //IPSO resource ID "5801"
    powObj.Instances[0].resource[1].resValue = (uint8_t *)&phase1.minPow;
    powObj.Instances[0].resource[2].resID = LWM2M_POWMEAS_MAX_MEAS_ACT_POWER; //IPSO resource ID "5802"
    powObj.Instances[0].resource[2].resValue = (uint8_t *)&phase1.maxPow;

    //Instance 1 for Ph2 power measurements
    powObj.Instances[1].instID = 1; //IPSO instance ID "1"
    powObj.Instances[1].resource[0].resID = LWM2M_POWMEAS_INST_ACT_POWER; //IPSO resource ID "5800"
    powObj.Instances[1].resource[0].resValue = (uint8_t *)&phase2.absPow;
    powObj.Instances[1].resource[1].resID = LWM2M_POWMEAS_MIN_MEAS_ACT_POWER; //IPSO resource ID "5801"
    powObj.Instances[1].resource[1].resValue = (uint8_t *)&phase2.minPow;
    powObj.Instances[1].resource[2].resID = LWM2M_POWMEAS_MAX_MEAS_ACT_POWER; //IPSO resource ID "5802"
    powObj.Instances[1].resource[2].resValue = (uint8_t *)&phase2.maxPow;

    //Instance 2 for Ph3 power measurements
    powObj.Instances[2].instID = 2; //IPSO instance ID "2"
    powObj.Instances[2].resource[0].resID = LWM2M_POWMEAS_INST_ACT_POWER; //IPSO resource ID "5800"
    powObj.Instances[2].resource[0].resValue = (uint8_t *)&phase3.absPow;
    powObj.Instances[2].resource[1].resID = LWM2M_POWMEAS_MIN_MEAS_ACT_POWER; //IPSO resource ID "5801"
    powObj.Instances[2].resource[1].resValue = (uint8_t *)&phase3.minPow;
    powObj.Instances[2].resource[2].resID = LWM2M_POWMEAS_MAX_MEAS_ACT_POWER; //IPSO resource ID "5802"
    powObj.Instances[2].resource[2].resValue = (uint8_t *)&phase3.maxPow;

  /* ===== Init voltage measurement object ===== */
    LWM2M_OBJ_T volObj;

    volObj.objectID = LWM2M_VOLTAGE_MEAS_OBJ; //IPSO object ID "Voltage Measurement"

    //Instance 0 for Ph1 current measurements
    volObj.Instances[0].instID = 0; //IPSO instance ID "0"
    volObj.Instances[0].resource[0].resID = LWM2M_GENSENS_SENS_VALUE; //IPSO resource ID "5700"
    volObj.Instances[0].resource[0].resValue = (uint8_t *)&phase1.absU;
    volObj.Instances[0].resource[2].resID = LWM2M_GENSENS_MIN_MEAS_VALUE; //IPSO resource ID "5601"
    volObj.Instances[0].resource[2].resValue = (uint8_t *)&phase1.minU;
    volObj.Instances[0].resource[3].resID = LWM2M_GENSENS_MAX_MEAS_VALUE; //IPSO resource ID "5602"
    volObj.Instances[0].resource[3].resValue = (uint8_t *)&phase1.maxU;

    //Instance 1 for Ph2 voltage measurements
    volObj.Instances[1].instID = 1; //IPSO instance ID "1"
    volObj.Instances[1].resource[0].resID = LWM2M_GENSENS_SENS_VALUE; //IPSO resource ID "5700"
    volObj.Instances[1].resource[0].resValue = (uint8_t *)&phase2.absU;
    volObj.Instances[1].resource[1].resID = LWM2M_GENSENS_MIN_MEAS_VALUE; //IPSO resource ID "5601"
    volObj.Instances[1].resource[1].resValue = (uint8_t *)&phase2.minU;
    volObj.Instances[1].resource[2].resID = LWM2M_GENSENS_MAX_MEAS_VALUE; //IPSO resource ID "5602"
    volObj.Instances[1].resource[2].resValue = (uint8_t *)&phase2.maxU;

    //Instance 2 for Ph3 voltage measurements
    volObj.Instances[2].instID = 2; //IPSO instance ID "2"
    volObj.Instances[2].resource[0].resID = LWM2M_GENSENS_SENS_VALUE; //IPSO resource ID "5700"
    volObj.Instances[2].resource[0].resValue = (uint8_t *)&phase3.absU;
    volObj.Instances[2].resource[1].resID = LWM2M_GENSENS_MIN_MEAS_VALUE; //IPSO resource ID "5601"
    volObj.Instances[2].resource[1].resValue = (uint8_t *)&phase3.minU;
    volObj.Instances[2].resource[2].resID = LWM2M_GENSENS_MAX_MEAS_VALUE; //IPSO resource ID "5602"
    volObj.Instances[2].resource[2].resValue = (uint8_t *)&phase3.maxU;

  /* ===== Init voltage measurement object ===== */
    LWM2M_OBJ_T curObj;

    curObj.objectID = LWM2M_CURRENT_MEAS_OBJ; //IPSO object ID "Current Measurement"

    //Instance 0 for Ph1 current measurements
    curObj.Instances[0].instID = 0; //IPSO instance ID "0"
    curObj.Instances[0].resource[0].resID = LWM2M_GENSENS_SENS_VALUE; //IPSO resource ID "5700"
    curObj.Instances[0].resource[0].resValue = (uint8_t *)&phase1.absI;
    curObj.Instances[0].resource[2].resID = LWM2M_GENSENS_MIN_MEAS_VALUE; //IPSO resource ID "5601"
    curObj.Instances[0].resource[2].resValue = (uint8_t *)&phase1.minI;
    curObj.Instances[0].resource[3].resID = LWM2M_GENSENS_MAX_MEAS_VALUE; //IPSO resource ID "5602"
    curObj.Instances[0].resource[3].resValue = (uint8_t *)&phase1.maxI;

    //Instance 1 for Ph2 current measurements
    curObj.Instances[1].instID = 1; //IPSO instance ID "1"
    curObj.Instances[1].resource[0].resID = LWM2M_GENSENS_SENS_VALUE; //IPSO resource ID "5700"
    curObj.Instances[1].resource[0].resValue = (uint8_t *)&phase2.absI;
    curObj.Instances[1].resource[1].resID = LWM2M_GENSENS_MIN_MEAS_VALUE; //IPSO resource ID "5601"
    curObj.Instances[1].resource[1].resValue = (uint8_t *)&phase2.minI;
    curObj.Instances[1].resource[2].resID = LWM2M_GENSENS_MAX_MEAS_VALUE; //IPSO resource ID "5602"
    curObj.Instances[1].resource[2].resValue = (uint8_t *)&phase2.maxI;

    //Instance 2 for Ph3 current measurements
    curObj.Instances[2].instID = 2; //IPSO instance ID "2"
    curObj.Instances[2].resource[0].resID = LWM2M_GENSENS_SENS_VALUE; //IPSO resource ID "5700"
    curObj.Instances[2].resource[0].resValue = (uint8_t *)&phase3.absI;
    curObj.Instances[2].resource[1].resID = LWM2M_GENSENS_MIN_MEAS_VALUE; //IPSO resource ID "5601"
    curObj.Instances[2].resource[1].resValue = (uint8_t *)&phase3.minI;
    curObj.Instances[2].resource[2].resID = LWM2M_GENSENS_MAX_MEAS_VALUE; //IPSO resource ID "5602"
    curObj.Instances[2].resource[2].resValue = (uint8_t *)&phase3.maxI;

  Emeter_LWM2M_Objects[0] = powObj;
  Emeter_LWM2M_Objects[1] = volObj;
  Emeter_LWM2M_Objects[2] = curObj;
}



/**********************************************************************
 Function:

 LWM2M_OBJ_ERROR LWM2M_Set_ResourceValue(uint16_t objID,
                                         uint8_t instID,
                                         uint16_t resID,
                                         uint8_t *value)
 ______________________________________________________________________
This function sets the value of an IPSO: Object -> Instance -> Resource
 ______________________________________________________________________
 arguments:

 uint16_t objID -> objects identification number (see IPSO Smart Object declaration)
 uint8_t instID -> instance idendtification number (user assigned)
 uint16_t resID -> resource identification number (see IPSO Smart Object declaration)
 uint8_t *value -> pointer to value to write into resource
 ______________________________________________________________________
 return:

 Error code
************************************************************************/
LWM2M_OBJ_ERROR LWM2M_Set_ResourceValue(uint16_t objID,
                                        uint8_t instID,
                                        uint16_t resID,
                                        void* value)
{
  uint8_t ind0;
  LWM2M_OBJ_ERROR errCode = ERROR_OBJ_NOT_FOUND;

  //suche object ID loop
  for(ind0 = 0; ind0 < LWM2M_EMETER_OBJ_NMB; ind0++) //for loop 0
  {
    //Ist die gewuenschte object ID vorhanden?
    if(objID == Emeter_LWM2M_Objects[ind0].objectID)
    {
      uint8_t ind1;

      errCode = ERROR_INST_NOT_FOUND;

      //suche instance ID loop
      for(ind1 = 0; ind1 < LWM2M_EMETER_INST_NMB; ind1++)//for loop 1
      {
        //Ist die gewuenschte instance ID vorhanden?
        if(instID == Emeter_LWM2M_Objects[ind0].Instances[ind1].instID)
        {
          uint8_t ind3;

          errCode = ERROR_RES_NOT_FOUND;

          //suche resource ID loop
          for(ind3 = 0; ind3 < LWM2M_EMETER_INST_NMB; ind3++)//for loop 2
          {
            //Ist die gewuenschte resource ID vorhanden?
            if(resID == Emeter_LWM2M_Objects[ind0].Instances[ind1].resource[ind3].resID)
            {
              memcpy(Emeter_LWM2M_Objects[ind0].Instances[ind1].resource[ind3].resValue,
                     (uint8_t *)value, sizeof(float));
              
              errCode = ERROR_OBJ_OK;

              goto RETURN;
            }
          }//================================ end for loop 2
        }
      }//==================================== end for loop 1
    }
  }//======================================== end for loop 0

  RETURN : return errCode;
}




/**********************************************************************
 Function:

 LWM2M_OBJ_ERROR LWM2M_Get_ResourceValue(uint16_t objID,
                                         uint8_t instID,
                                         uint16_t resID,
                                         uint8_t *value)
 ______________________________________________________________________
This function gets the value of an IPSO: Object -> Instance -> Resource
 ______________________________________________________________________
 arguments:

 uint16_t objID -> objects identification number (see IPSO Smart Object declaration)
 uint8_t instID -> instance idendtification number (user assigned)
 uint16_t resID -> resource identification number (see IPSO Smart Object declaration)
 uint8_t *value -> pointer to a buffer to put the resource value in
 ______________________________________________________________________
 return:

 Error code
************************************************************************/
LWM2M_OBJ_ERROR LWM2M_Get_ResourceValue(uint16_t objID,
                                        uint8_t instID,
                                        uint16_t resID,
                                        uint8_t* value)
{
  uint8_t ind0;
  LWM2M_OBJ_ERROR errCode = ERROR_OBJ_NOT_FOUND;

  //suche object ID loop
  for(ind0 = 0; ind0 < LWM2M_EMETER_OBJ_NMB; ind0++) //for loop 0
  {
    //Ist die gewuenschte object ID vorhanden?
    if(objID == Emeter_LWM2M_Objects[ind0].objectID)
    {
      uint8_t ind1;

      errCode = ERROR_INST_NOT_FOUND;

      //suche instance ID loop
      for(ind1 = 0; ind1 < LWM2M_EMETER_INST_NMB; ind1++)//for loop 1
      {
        //Ist die gewuenschte instance ID vorhanden?
        if(instID == Emeter_LWM2M_Objects[ind0].Instances[ind1].instID)
        {
          uint8_t ind3;

          errCode = ERROR_RES_NOT_FOUND;

          //suche resource ID loop
          for(ind3 = 0; ind3 < LWM2M_EMETER_INST_NMB; ind3++)//for loop 2
          {
            //Ist die gewuenschte resource ID vorhanden?
            if(resID == Emeter_LWM2M_Objects[ind0].Instances[ind1].resource[ind3].resID)
            {
              uint8_t index;
              
              for(index=0; index<4; index++)
              {
                value[3 - index] = Emeter_LWM2M_Objects[ind0].Instances[ind1].resource[ind3].resValue[index];
              }
              errCode = ERROR_OBJ_OK;

              goto RETURN;
            }
          }//================================ end for loop 2
        }
      }//==================================== end for loop 1
    }
  }//======================================== end for loop 0

  RETURN : return errCode;
}
