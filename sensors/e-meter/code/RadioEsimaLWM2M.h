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
#ifndef RADIO_ESIMA_INCLUDED
#define RADIO_ESIMA_INCLUDED

/*******************************************************************************
 * INCLUDES
 */

/*******************************************************************************
 * CONSTANTS
 */
#define RADIO_RESET_BIT                 BIT6
#define RADIO_RESET_PORT                P5OUT

#define _RadioReset_L()                 RADIO_RESET_PORT &= ~RADIO_RESET_BIT
#define _RadioReset_H()                 RADIO_RESET_PORT |=  RADIO_RESET_BIT
/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * FUNCTIONS
 */
void Radio_UpdateEmeter_LWM2M(const struct phase_parms_s pst_phase_parms[4] );

#endif