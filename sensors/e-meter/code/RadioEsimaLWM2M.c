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
 @file  RadioEsimaLWM2M.h

 Group: ES

 Target Device: MSP430

 Author: Daniel Stojakov
 ******************************************************************************/
/*******************************************************************************
 * INCLUDES
 */
 #include "msp430.h"
#include "Hardware.h"
#include "emeter-structs.h"
#include "RadioEsimaLWM2M.h"
#include "LWM2M.h"
#include "LWM2M_App.h"
#include "Timer_Modul.h"

#include <string.h>

/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */
static struct phase_readings_s mstReadings[4];

/*******************************************************************************
 * FUNCTIONS
 */



#pragma vector=PORT2_VECTOR
__interrupt void Radio_ISR(void)
{
   RADIO_DATA_READY_IFG &= ~RADIO_DATA_READY_BIT;
}



//==============================================================================================================================================
//    Diese Funktion wird einmal pro Sekunde aufgerufen, wenn neue Messdaten vorliegen. Der Aufruf erfolgt in der Main-loop
//==============================================================================================================================================
void Radio_UpdateEmeter_LWM2M(const struct phase_parms_s pst_phase_parms[4] )
{
   static float   iU = 10000.0f;                          // Spannung in V
   static float   fI = 0;                                 // Strom in A
   float   fE = 0;                                        // Energie in Ws
   //int8_t  cCosPhi = 0;                                 // 100 = cos 0° = 1

   memcpy( &mstReadings[0], &pst_phase_parms[0].readings, sizeof(struct phase_readings_s));
   memcpy( &mstReadings[1], &pst_phase_parms[1].readings, sizeof(struct phase_readings_s));
   memcpy( &mstReadings[2], &pst_phase_parms[2].readings, sizeof(struct phase_readings_s));

#if defined(NEUTRAL_MONITOR_SUPPORT)
   memcpy( &mstReadings[3], &pst_phase_parms[3].readings, sizeof(struct phase_readings_s));
#endif

//====================== Phase 1 ======================
//   iU      = (int16_t)((mstReadings[0].V_rms + 50)  / 100);           // +50 wegen Rundung / 100 weil V_rms in 10mV / 1/100V vorliegt
   iU--;     // +50 wegen Rundung / 100 weil V_rms in 10mV / 1/100V vorliegt
//   fI      = (float)    mstReadings[0].I_rms        / 1000.0f*IRMS_E_GAIN_FACTOR;        // Umwandlung mA in A
   fI++;
   fE      = (float)    mstReadings[0].active_power / 100.0f*IRMS_E_GAIN_FACTOR;         // Umwandlung 10mWs in Ws
   //cCosPhi =  mstReadings[0].power_factor           / 100;            // 0 .. 10000 in 0 .. 100 umwandeln

   LWM2M_Set_ResourceValue(LWM2M_POWER_MEAS_OBJ, 0,
                           LWM2M_POWMEAS_INST_ACT_POWER,
                           (uint8_t*)&fE);

   LWM2M_Set_ResourceValue(LWM2M_VOLTAGE_MEAS_OBJ, 0,
                           LWM2M_GENSENS_SENS_VALUE,
                           (uint8_t*)&iU);

   LWM2M_Set_ResourceValue(LWM2M_CURRENT_MEAS_OBJ, 0,
                           LWM2M_GENSENS_SENS_VALUE,
                           (uint8_t*)&fI);

//====================== Phase 2 ======================
//   iU      = (int16_t)((mstReadings[1].V_rms + 50)  / 100);           // +50 wegen Rundung / 100 weil V_rms in 10mV / 1/100V vorliegt
//   fI      = (float)    mstReadings[1].I_rms        / 1000.0f*IRMS_E_GAIN_FACTOR;        // Umwandlung mA in A
   fE      = (float)    mstReadings[1].active_power / 100.0f*IRMS_E_GAIN_FACTOR;         // Umwandlung 10mWs in Ws
   //cCosPhi =  mstReadings[1].power_factor           / 100;            // 0 .. 10000 in 0 .. 100 umwandeln

   LWM2M_Set_ResourceValue(LWM2M_POWER_MEAS_OBJ, 1,
                           LWM2M_POWMEAS_INST_ACT_POWER,
                           (uint8_t*)&fE);

   LWM2M_Set_ResourceValue(LWM2M_VOLTAGE_MEAS_OBJ, 1,
                           LWM2M_GENSENS_SENS_VALUE,
                           (uint8_t*)&iU);

   LWM2M_Set_ResourceValue(LWM2M_CURRENT_MEAS_OBJ, 1,
                           LWM2M_GENSENS_SENS_VALUE,
                           (uint8_t*)&fI);

//====================== Phase 3 ======================
//   iU      = (int16_t)((mstReadings[2].V_rms + 50)  / 100);           // +50 wegen Rundung / 100 weil V_rms in 10mV / 1/100V vorliegt
//   fI      = (float)    mstReadings[2].I_rms        / 1000.0f*IRMS_E_GAIN_FACTOR;        // Umwandlung mA in A
   fE      = (float)    mstReadings[2].active_power / 100.0f*IRMS_E_GAIN_FACTOR;         // Umwandlung 10mWs in Ws
   //cCosPhi =  mstReadings[2].power_factor           / 100;            // 0 .. 10000 in 0 .. 100 umwandeln

   LWM2M_Set_ResourceValue(LWM2M_POWER_MEAS_OBJ, 2,
                           LWM2M_POWMEAS_INST_ACT_POWER,
                           (uint8_t*)&fE);

   LWM2M_Set_ResourceValue(LWM2M_VOLTAGE_MEAS_OBJ, 2,
                           LWM2M_GENSENS_SENS_VALUE,
                           (uint8_t*)&iU);

   LWM2M_Set_ResourceValue(LWM2M_CURRENT_MEAS_OBJ, 2,
                           LWM2M_GENSENS_SENS_VALUE,
                           (uint8_t*)&fI);

//====================== Phase Neutral ======================
   /*
   mstUART[3].fI          = (float) mstReadings[3].I_rms         / 1000.0f*IRMS_E_GAIN_FACTOR;        // Umwandlung mA in A
   mstUART[3].cCosPhi     = mstReadings[3].active_power          / 100*IRMS_E_GAIN_FACTOR;            // 0 .. 10000 in 0 .. 100 umwandeln


   // double ist notwendig, da sehr kleine Werte (Ws) mit sehr großen Werten (kWh) addiert werden
   mstEnergy.dE_L1 += (double) pst_phase_parms[0].readings.active_power / 3600000.0*IRMS_E_GAIN_FACTOR;      // Ws ind kWh umrechnen
   mstEnergy.dE_L2 += (double) pst_phase_parms[1].readings.active_power / 3600000.0*IRMS_E_GAIN_FACTOR;      // Ws ind kWh umrechnen
   mstEnergy.dE_L3 += (double) pst_phase_parms[2].readings.active_power / 3600000.0*IRMS_E_GAIN_FACTOR;      // Ws ind kWh umrechnen

   mstUART_E.fE_L1 = (float) mstEnergy.dE_L1;                // aus double wird float beim senden
   mstUART_E.fE_L2 = (float) mstEnergy.dE_L2;                // aus double wird float beim senden
   mstUART_E.fE_L3 = (float) mstEnergy.dE_L3;                // aus double wird float beim senden
   */
//=====================================================

   LWM2M_Task |= LWM2M_TASK_WR_REQ;
}
