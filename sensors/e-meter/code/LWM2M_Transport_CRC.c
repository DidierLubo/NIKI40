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
 * FileName:            LWM2M_TransportCRC.c
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

#include "LWM2M_Transport_CRC.h"

/*******************************************************************************
 * CONSTANTS
 */
#define POLYNOMAL                     0x3D65U

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * LOCAL FUNCTIONS
 */



/*******************************************************************************
 * FUNCTIONS
 */
 
   
   
/**********************************************************************
 Function: 

 uint16_t crc_calc(uint16_t i_init, uint8_t *pc_data, uint16_t i_len)
 ______________________________________________________________________
 Calculates the crc in 16bit format
 ______________________________________________________________________
 arguments:

 uint16_t i_init  -> 
 uint8_t *pc_data ->  
 uint16_t i_len   -> 
 ______________________________________________________________________ 
 return:
 
 uint16_t i_crc  ->
************************************************************************/   
uint16_t crc_calc(uint16_t i_init, uint8_t *pc_data, uint16_t i_len)
{
  /* Temporary variables */
  uint16_t i, j, c;
  
  /* Stores the current calculated crc value */
  uint16_t i_crc;
  i_crc = i_init;

  for(i = 0U;i < i_len;i++)
  {
    c = pc_data[i];
    c <<= 8U;
    i_crc ^= c;

    for(j = 0U;j < 8U;j++)
    {
      if(i_crc & 0x8000U)
        i_crc = (i_crc << 1U) ^ POLYNOMAL;
      else
         i_crc <<= 1U;
    }
    
    i_crc &= 0xFFFFU;
  }
  
  i_crc = ~i_crc;
  
  return i_crc;
}
