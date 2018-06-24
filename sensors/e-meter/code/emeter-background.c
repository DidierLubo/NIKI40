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
//--------------------------------------------------------------------------
//
//  Software for MSP430 based e-meters.
//
//  THIS PROGRAM IS PROVIDED "AS IS". TI MAKES NO WARRANTIES OR
//  REPRESENTATIONS, EITHER EXPRESS, IMPLIED OR STATUTORY,
//  INCLUDING ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
//  FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
//  COMPLETENESS OF RESPONSES, RESULTS AND LACK OF NEGLIGENCE.
//  TI DISCLAIMS ANY WARRANTY OF TITLE, QUIET ENJOYMENT, QUIET
//  POSSESSION, AND NON-INFRINGEMENT OF ANY THIRD PARTY
//  INTELLECTUAL PROPERTY RIGHTS WITH REGARD TO THE PROGRAM OR
//  YOUR USE OF THE PROGRAM.
//
//  IN NO EVENT SHALL TI BE LIABLE FOR ANY SPECIAL, INCIDENTAL,
//  CONSEQUENTIAL OR INDIRECT DAMAGES, HOWEVER CAUSED, ON ANY
//  THEORY OF LIABILITY AND WHETHER OR NOT TI HAS BEEN ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES, ARISING IN ANY WAY OUT
//  OF THIS AGREEMENT, THE PROGRAM, OR YOUR USE OF THE PROGRAM.
//  EXCLUDED DAMAGES INCLUDE, BUT ARE NOT LIMITED TO, COST OF
//  REMOVAL OR REINSTALLATION, COMPUTER TIME, LABOR COSTS, LOSS
//  OF GOODWILL, LOSS OF PROFITS, LOSS OF SAVINGS, OR LOSS OF
//  USE OR INTERRUPTION OF BUSINESS. IN NO EVENT WILL TI'S
//  AGGREGATE LIABILITY UNDER THIS AGREEMENT OR ARISING OUT OF
//  YOUR USE OF THE PROGRAM EXCEED FIVE HUNDRED DOLLARS
//  (U.S.$500).
//
//  Unless otherwise stated, the Program written and copyrighted
//  by Texas Instruments is distributed as "freeware".  You may,
//  only under TI's copyright in the Program, use and modify the
//  Program without any charge or restriction.  You may
//  distribute to third parties, provided that you transfer a
//  copy of this license to the third party and the third party
//  agrees to these terms by its first use of the Program. You
//  must reproduce the copyright notice and any other legend of
//  ownership on each copy or partial copy, of the Program.
//
//  You acknowledge and agree that the Program contains
//  copyrighted material, trade secrets and other TI proprietary
//  information and is protected by copyright laws,
//  international copyright treaties, and trade secret laws, as
//  well as other intellectual property laws.  To protect TI's
//  rights in the Program, you agree not to decompile, reverse
//  engineer, disassemble or otherwise translate any object code
//  versions of the Program to a human-readable form.  You agree
//  that in no event will you alter, remove or destroy any
//  copyright notice included in the Program.  TI reserves all
//  rights not specifically granted under this license. Except
//  as specifically provided herein, nothing in this agreement
//  shall be construed as conferring by implication, estoppel,
//  or otherwise, upon you, any license or other right under any
//  TI patents, copyrights or trade secrets.
//
//  You may not use the Program in non-TI devices.
//
//  File: emeter-background.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: emeter-background.c,v 1.50 2009/04/23 06:44:30 a0754793 Exp $
//
/*! \file emeter-structs.h */
//
//--------------------------------------------------------------------------
//
//  MSP430 background (interrupt) routines for e-meters
//
//  This software is appropriate for single phase and three phase e-meters
//  using a voltage sensor plus a CT or shunt resistor current sensors, or
//  a combination of a CT plus a shunt.
//
//    The background process deals with the input samples.
//    These are first stored into buffers.
//    The buffered samples are processed as follows:
//    -Voltage and current signals are converted to DC-less AC signals
//    -The current signal is phase compensated
//    -Voltage and current are signed multiplied to give power.
//    -Power samples are accumulated. The accumulated power samples are averaged (in foreground.c)
//     after a number of voltage cycles has been detected.
//
#include <stdint.h>
#include <stdlib.h>
#include <io.h>
#include <emeter-toolkit.h>
#include "emeter-structs.h"
#if !defined(NULL)
#define NULL    (void *) 0
#endif

int16_t samples_per_second;
uint16_t adc_interrupts;


#if defined(PWM_DITHERING_SUPPORT)
  uint8_t pwm_stutter;
#endif
#if defined(TEMPERATURE_SUPPORT)
  int32_t rolling_random;
  int32_t rolling_0s = 0;
  int32_t rolling_1s = 0;
#endif
static int32_t sample_count = 0;
#if defined(x__MSP430__)
    #if defined(BASIC_KEYPAD_SUPPORT)  ||  defined(CUSTOM_KEYPAD_SUPPORT)
        #if defined(sense_key_1_up)
          static uint8_t debounce_key_1;
          static int16_t key_timer_1;
        #endif
        #if defined(sense_key_2_up)
          static uint8_t debounce_key_2;
          static int16_t key_timer_2;
        #endif
        #if defined(sense_key_3_up)
          static uint8_t debounce_key_3;
          static int16_t key_timer_3;
        #endif
        #if defined(sense_key_4_up)
          static uint8_t debounce_key_4;
          static int16_t key_timer_4;
        #endif
        uint8_t key_states;
    #endif
#endif
uint16_t battery_countdown;
//#if defined(POWER_DOWN_SUPPORT)  &&  defined(POWER_UP_BY_SUPPLY_SENSING)
//  int8_t power_down_debounce;
//#endif
#if  defined(POWER_DOWN_SUPPORT)  &&  defined(POWER_UP_BY_VOLTAGE_PULSES)
  uint8_t pd_pin_debounce;
#endif

#if defined(FINE_ENERGY_PULSE_TIMING_SUPPORT)
  uint8_t fine_pulse_operation;
#endif
#if defined(MAGNETIC_INTERFERENCE_SUPPORT)
  uint16_t magnetic_sensor_count;
  uint16_t magnetic_sensor_count_logged;
#endif

/* This keypad debounce code provides for 1 to 4 keys, with debounce + long
   press detect, of debounce + auto-repeat on long press selectable for each
   key. Definitions in emeter.h control this. A long press means >2s.
   Auto-repeat means holding the key >1s starts repeats at 3 per second. */
#if defined(x__MSP430__)  &&  (defined(BASIC_KEYPAD_SUPPORT)  ||  defined(CUSTOM_KEYPAD_SUPPORT))
static __inline__ int keypad_debounce(void)
{
    int kick_foreground;
    
    kick_foreground = FALSE;
    #if defined(sense_key_1_up)
    switch (debounce(&debounce_key_1, sense_key_1_up()))
    {
    case DEBOUNCE_JUST_RELEASED:
        key_timer_1 = 0;
        break;
    case DEBOUNCE_JUST_HIT:
        #if defined(KEY_1_LONG_DOWN)
        /* Start a 2s timer to detect mode change request */
        key_timer_1 = samples_per_second << 1;
        #elif defined(KEY_1_REPEAT_DOWN)
        /* Start an initial 1s timeout for repeats */
        key_timer_1 = samples_per_second;
        #endif
        key_states |= KEY_1_DOWN;
        kick_foreground = TRUE;
        break;
    case DEBOUNCE_HIT:
        if (key_timer_1  &&  --key_timer_1 == 0)
        {
        #if defined(KEY_1_LONG_DOWN)
            key_states |= KEY_1_LONG_DOWN;
        #elif defined(KEY_1_REPEAT_DOWN)
            key_timer_1 = 1092;
            key_states |= KEY_1_REPEAT_DOWN;
        #endif
            kick_foreground = TRUE;
        }
        break;
    }
    #endif
    #if defined(sense_key_2_up)
    switch (debounce(&debounce_key_2, sense_key_2_up()))
    {
    case DEBOUNCE_JUST_RELEASED:
        key_timer_2 = 0;
        break;
    case DEBOUNCE_JUST_HIT:
        #if defined(KEY_2_LONG_DOWN)
        /* Start a 2s timer to detect mode change request */
        key_timer_2 = samples_per_second << 1;
        #elif defined(KEY_2_REPEAT_DOWN)
        /* Start an initial 1s timeout for repeats */
        key_timer_2 = samples_per_second;
        #endif
        key_states |= KEY_2_DOWN;
        kick_foreground = TRUE;
        break;
    case DEBOUNCE_HIT:
        if (key_timer_2  &&  --key_timer_2 == 0)
        {
        #if defined(KEY_2_LONG_DOWN)
            key_states |= KEY_2_LONG_DOWN;
        #elif defined(KEY_2_REPEAT_DOWN)
            /* Start a 1/3s timeout for repeats */
            key_timer_2 = 1092;
            key_states |= KEY_2_REPEAT_DOWN;
        #endif
            kick_foreground = TRUE;
        }
        break;
    }
    #endif
    #if defined(sense_key_3_up)
    switch (debounce(&debounce_key_3, sense_key_3_up()))
    {
    case DEBOUNCE_JUST_RELEASED:
        key_timer_3 = 0;
        break;
    case DEBOUNCE_JUST_HIT:
        #if defined(KEY_3_LONG_DOWN)
        /* Start a 2s timer to detect mode change request */
        key_timer_3 = samples_per_second << 1;
        #elif defined(KEY_3_REPEAT_DOWN)
        /* Start an initial 1s timeout for repeats */
        key_timer_3 = samples_per_second;
        #endif
        key_states |= KEY_3_DOWN;
        kick_foreground = TRUE;
        break;
    case DEBOUNCE_HIT:
        if (key_timer_3  &&  --key_timer_3 == 0)
        {
        #if defined(KEY_3_LONG_DOWN)
            key_states |= KEY_3_LONG_DOWN;
        #elif defined(KEY_3_REPEAT_DOWN)
            /* Start a 1/3s timeout for repeats */
            key_timer_3 = 1092;
            key_states |= KEY_3_REPEAT_DOWN;
        #endif
            kick_foreground = TRUE;
        }
        break;
    }
    #endif
    #if defined(sense_key_4_up)
    switch (debounce(&debounce_key_4, sense_key_4_up()))
    {
    case DEBOUNCE_JUST_RELEASED:
        key_timer_4 = 0;
        break;
    case DEBOUNCE_JUST_HIT:
        #if defined(KEY_4_LONG_DOWN)
        /* Start a 2s timer to detect mode change request */
        key_timer_4 = samples_per_second << 1;
        #elif defined(KEY_4_REPEAT_DOWN)
        /* Start an initial 1s timeout for repeats */
        key_timer_4 = samples_per_second;
        #endif
        key_states |= KEY_4_DOWN;
        kick_foreground = TRUE;
        break;
    case DEBOUNCE_HIT:
        if (key_timer_4  &&  --key_timer_4 == 0)
        {
        #if defined(KEY_4_LONG_DOWN)
            key_states |= KEY_3_LONG_DOWN;
        #elif defined(KEY_4_REPEAT_DOWN)
            /* Start a 1/3s timeout for repeats */
            key_timer_4 = 1092;
            key_states |= KEY_4_REPEAT_DOWN;
        #endif
            kick_foreground = TRUE;
        }
        break;
    }
    #endif
    return  kick_foreground;
}
#endif 

static void __inline__ log_parameters(struct phase_parms_s *phase)
{
    #define i 0
    /* Take a snapshot of various values for logging purposes; tell the
       foreground to deal with them; and clear the working values ready
       for the next analysis period. */
    if (phase->metrology.V_endstops <= 0)
        phase->status |= V_OVERRANGE;
    else
        phase->status &= ~V_OVERRANGE;
    phase->metrology.V_endstops = ENDSTOP_HITS_FOR_OVERLOAD;
    #if defined(VRMS_SUPPORT)
        transfer48(phase->metrology.dot_prod_logged.V_sq, phase->metrology.dot_prod.V_sq);
    #endif
    if (phase->metrology.current.I_endstops <= 0) 
        phase->status |= I_OVERRANGE;
    else
        phase->status &= ~I_OVERRANGE;
    phase->metrology.current.I_endstops = ENDSTOP_HITS_FOR_OVERLOAD;
    phase->metrology.current.dot_prod_logged.sample_count = phase->metrology.current.dot_prod.sample_count;
    phase->metrology.current.dot_prod.sample_count = 0;
    #if defined(IRMS_SUPPORT)
            transfer_ac_current(phase->metrology.current.dot_prod_logged.I_sq, phase->metrology.current.dot_prod.I_sq);
    #endif
    transfer_ac_power(phase->metrology.current.dot_prod_logged.P_active, phase->metrology.current.dot_prod.P_active);
    #if defined(REACTIVE_POWER_BY_QUADRATURE_SUPPORT)
      transfer_ac_power(phase->metrology.current.dot_prod_logged.P_reactive, phase->metrology.current.dot_prod.P_reactive);
    #endif
    phase->metrology.dot_prod_logged.sample_count = phase->metrology.dot_prod.sample_count;
    phase->metrology.dot_prod.sample_count = 0;
    /* Tell the foreground there are things to process. */
    phase->status |= NEW_LOG;
      #undef i
}
#if defined(NEUTRAL_MONITOR_SUPPORT)  &&  defined(IRMS_SUPPORT)
/* This routine logs neutral lead information for poly-phase meters. It is
   not used for single phase meters with neutral monitoring. */
  static void __inline__ log_neutral_parameters(void)
  {
    #define i 0
    if (neutral.metrology.I_endstops <= 0)
        neutral.status |= I_OVERRANGE;
    else
        neutral.status &= ~I_OVERRANGE;
    neutral.metrology.I_endstops = ENDSTOP_HITS_FOR_OVERLOAD;
    neutral.metrology.sample_count_logged = neutral.metrology.sample_count;
    #if defined(IRMS_SUPPORT)  ||  defined(POWER_FACTOR_SUPPORT)
        transfer_ac_current(neutral.metrology.I_sq_accum_logged, neutral.metrology.I_sq_accum);
    #endif
    neutral.metrology.sample_count = 0;
    /* Tell the foreground there are things to process. */
    neutral.status |= NEW_LOG;
    #undef i
}
#endif

/*---------------------------------------------------------------------------
  This is the main interrupt routine where the main signal processing is done
  ---------------------------------------------------------------------------*/
ISR(SD24B, adc_interrupt)
{
    #if defined(PWM_DITHERING_SUPPORT)
        extern uint16_t rndnum;
    #endif
    int16_t V_sample;
    int16_t V_corrected;
    int16_t V_quad_corrected;
    current_sample_t corrected;
    current_sample_t I_live_sample;
   // current_sample_t I_neutral_sample;
    #define i 0
    #define use_stage 0
    struct phase_parms_s *phase;
    int j;
    static int16_t adc_v_buffer[3];
    static current_sample_t adc_i_buffer[4]; 
    //int adc_ptr;
    #if defined(FINE_ENERGY_PULSE_TIMING_SUPPORT)
        int32_t xxx;
    #endif
    int k;
    #if defined(MAINS_FREQUENCY_SUPPORT)
        int x;
        int y;
        int z;
    #endif
    sample_count++;
    if (!(TST_SD16IFG_VOLTAGE_1))
    {
        /* We do not have a complete set of samples yet, but we may need to pick
           up some current values at this time */
        if ((TST_SD16IFG_CURRENT_1))
        {
            adc_i_buffer[0] = SD16MEM_CURRENT_1;
            CLR_SD16IFG_CURRENT_1;
        }
        if ((TST_SD16IFG_CURRENT_2))
        {
            adc_i_buffer[1] = SD16MEM_CURRENT_2;
            CLR_SD16IFG_CURRENT_2;
        }
        if ((TST_SD16IFG_CURRENT_3))
        {
            adc_i_buffer[2] = SD16MEM_CURRENT_3;
            CLR_SD16IFG_CURRENT_3;
        }
        if ((TST_SD16IFG_NEUTRAL))
        {
           adc_i_buffer[3] = SD16MEM_NEUTRAL;
            CLR_SD16IFG_NEUTRAL;
        }
        return;
    }

    #if defined(FINE_ENERGY_PULSE_TIMING_SUPPORT)
        /* Trigger the energy pulse, timed by timer A */
        /* This needs to be at the start of the interrupt service routine so its timing is well defined in
           relation to the counting of timer A */
        if (fine_pulse_operation)
        {
            /* This early in the interrupt routine timer A's counter should still be at zero. If we request
               a target compare match count of zero while the count is already zero, the output will change
               immediately. Counts of 0 to 9 should, therefore, hit somewhere through the current ADC interrupt
               period. A match of zero will not be as well timed as the others, as we will have missed the exact
               moment when it should begin, but it is good enough. */
            TACCTL2 = fine_pulse_operation;
            fine_pulse_operation = 0;
        }
    #endif

    /* Voltage is available */
    adc_v_buffer[0] = SD16MEM_VOLTAGE_1;
    CLR_SD16IFG_VOLTAGE_1;
    adc_v_buffer[1] = SD16MEM_VOLTAGE_2;
     CLR_SD16IFG_VOLTAGE_2;
    adc_v_buffer[2] = SD16MEM_VOLTAGE_3;
     CLR_SD16IFG_VOLTAGE_3;
    /* Pick up any current samples which may have occurred a little before the
       voltage sample, but not those which may have occurred just after the
       voltage sample. */
    if (chan[0].metrology.current.in_phase_correction[0].sd16_preloaded_offset < 128  &&  (TST_SD16IFG_CURRENT_1))
    {
        adc_i_buffer[0] = SD16MEM_CURRENT_1;
        CLR_SD16IFG_CURRENT_1;
    }
    if (chan[1].metrology.current.in_phase_correction[0].sd16_preloaded_offset < 128  &&  (TST_SD16IFG_CURRENT_2))
    {
        adc_i_buffer[1] = SD16MEM_CURRENT_2;
        CLR_SD16IFG_CURRENT_2;
    }
    if (chan[2].metrology.current.in_phase_correction[0].sd16_preloaded_offset < 128  &&  (TST_SD16IFG_CURRENT_3))
    {
        adc_i_buffer[2] = SD16MEM_CURRENT_3;
        CLR_SD16IFG_CURRENT_3;
    }
    if (TST_SD16IFG_NEUTRAL)
    {
        adc_i_buffer[3] = SD16MEM_NEUTRAL;
       CLR_SD16IFG_NEUTRAL;
    }
    /* We have a complete set of samples. Process them. */
    //adc_ptr = -1;
    kick_watchdog();
    phase = chan;
    for (j = 0;  j < NUM_PHASES;  j++)
    {
      //adc_ptr++;  
      V_sample = adc_v_buffer[j];
     
        if ((V_sample >= V_ADC_MAX  ||  V_sample <= V_ADC_MIN)  &&  phase->metrology.V_endstops)
            phase->metrology.V_endstops--;
        V_sample = dc_filter_voltage(phase->metrology.V_dc_estimate, V_sample);
        #if defined(VRMS_SUPPORT)  ||  defined(POWER_FACTOR_SUPPORT)       
          sqac_voltage(phase->metrology.dot_prod.V_sq, V_sample);
        #endif
        #if defined(REACTIVE_POWER_BY_QUADRATURE_SUPPORT)  ||  defined(PHASE_CORRECTION_SUPPORT)
                /* We need to save the history of the voltage signal if we are performing phase correction, and/or
                   measuring the quadrature shifted power (to obtain an accurate measure of one form of the reactive power). */
            phase->metrology.V_history[(int) phase->metrology.V_history_index] = V_sample;
        #endif
        I_live_sample = dc_filter_current(phase->metrology.current.I_dc_estimate[0], phase->metrology.current.I_history[0][0]);
          //adc_ptr++;
        corrected = adc_i_buffer[j];
        if ((corrected >= I_ADC_MAX  ||  corrected <= I_ADC_MIN)  &&  phase->metrology.current.I_endstops)
            phase->metrology.current.I_endstops--;
        for (k = 0;  k < I_HISTORY_STEPS - 1;  k++)
            phase->metrology.current.I_history[0][k] = phase->metrology.current.I_history[0][k + 1];
        phase->metrology.current.I_history[0][I_HISTORY_STEPS - 1] = corrected;
        #if defined(IRMS_SUPPORT)  ||  defined(POWER_FACTOR_SUPPORT)
              sqac_current(phase->metrology.current.dot_prod.I_sq, I_live_sample);
        #endif
        if (operating_mode == OPERATING_MODE_NORMAL)
        {
            /* Perform phase shift compensation, to allow for the time
               between ADC samplings, internal phase shifts in CTs, etc.
               This uses a 1 tap FIR (basically an interpolator/extrapolator) */
            #if defined(PHASE_CORRECTION_SUPPORT)
              V_corrected = phase->metrology.V_history[(phase->metrology.V_history_index - phase->metrology.current.in_phase_correction[use_stage].step) & V_HISTORY_MASK];
            #else
              corrected = V_sample;
            #endif
            mac_power(phase->metrology.current.dot_prod.P_active, V_corrected, I_live_sample);
              
            #if defined(REACTIVE_POWER_BY_QUADRATURE_SUPPORT)
               V_quad_corrected = (Q1_15_mul(phase->metrology.V_history[(phase->metrology.V_history_index - phase->metrology.current.quadrature_correction[use_stage].step - 1) & V_HISTORY_MASK], phase->metrology.current.quadrature_correction[use_stage].fir_beta) >> 1)
                                  + (phase->metrology.V_history[(phase->metrology.V_history_index - phase->metrology.current.quadrature_correction[use_stage].step) & V_HISTORY_MASK] >> 1);
               mac_power(phase->metrology.current.dot_prod.P_reactive, V_quad_corrected, I_live_sample);
            #endif
        }
        ++phase->metrology.current.dot_prod.sample_count;
        #if defined(REACTIVE_POWER_BY_QUADRATURE_SUPPORT)  ||  defined(PHASE_CORRECTION_SUPPORT)
                phase->metrology.V_history_index = (phase->metrology.V_history_index + 1) & V_HISTORY_MASK;
        #endif
        ++phase->metrology.dot_prod.sample_count;

        #if defined(PER_PHASE_ACTIVE_ENERGY_SUPPORT)
        /* We now play the last measurement interval's power level, evaluated
           in the foreground, through this measurement interval. In this way
           we can evenly pace the pulsing of the LED. The only error produced
           by this is the ambiguity in the number of samples per measurement.
           This should not exceed 1 or 2 in over 4000. */
          
          #if defined(INHIBIT_NEGATIVE_PHASE_POWER_ACCUMULATION)
                  if (phase->readings.active_power > 0  &&  (phase->active_power_counter += phase->readings.active_power) >= PHASE_ENERGY_PULSE_THRESHOLD)
          #else
                  if ((phase->active_power_counter += phase->readings.active_power) >= PHASE_ENERGY_PULSE_THRESHOLD)
          #endif
          {
              phase->active_power_counter -= PHASE_ENERGY_PULSE_THRESHOLD;
              ++phase->consumed_active_energy;
              /* Ideally we want to log the energy each kWh unit, but doing
                 it with a mask here is good enough and faster. */
              if ((phase->consumed_active_energy & 0x3FF) == 0)
                 phase->status |= ENERGY_LOGABLE;
              /* Pulse the indicator. Long pulses may not be reliable, as at full
                   power we may be pulsing several times per second. People may
                   check the meter's calibration with an instrument that counts
                   the pulsing rate, so it is important the pulses are clear,
                   distinct, and exactly at the rate of one per
                   1/ENERGY_PULSES_PER_KW_HOUR kW/h. */
              switch (j)
              {
                  case 0:
                     phase_1_active_energy_pulse_start();
                  break;
                  case 1:
                      phase_2_active_energy_pulse_start();
                  break;
                  case 2:
                      phase_3_active_energy_pulse_start();
                  break;
               }
               phase->active_energy_pulse_remaining_time = ENERGY_PULSE_DURATION;
            }
            if (phase->active_energy_pulse_remaining_time  &&  --phase->active_energy_pulse_remaining_time == 0)
            {
                switch (j)
                {
                    case 0:
                        phase_1_active_energy_pulse_end();
                    break;
                    case 1:
                        phase_2_active_energy_pulse_end();
                    break;
                    case 2:
                        phase_3_active_energy_pulse_end();
                    break;
                }
            }
         #endif
        /* Do the power cycle start detection */
        /* There is no hysteresis used here, but since the signal is
           changing rapidly at the zero crossings, and is always of
           large amplitude, miscounting cycles due to general noise
           should not occur. Spikes are another matter. A large spike
           could cause the power cycles to be miscounted, but does not
           matter very much. The cycle counting is not critical to power
           or energy measurement. */
        #if defined(MAINS_FREQUENCY_SUPPORT)
            phase->metrology.cycle_sample_count += 256;
        #endif
        if (abs(V_sample - phase->metrology.last_V_sample) <= phase->metrology.since_last*MAX_PER_SAMPLE_VOLTAGE_SLEW)
        {
            /* This doesn't look like a spike - do mains cycle detection, and
               estimate the precise mains period */
            if (V_sample < 0)
            {
                /* Log the sign of the signal */
                phase->status &= ~V_POS;
            }
            else
            {
                if (!(phase->status & V_POS))
                {
                  #if defined(MAINS_FREQUENCY_SUPPORT)
                    /* Apply limits to the sample count, to avoid spikes or dying power lines disturbing the
                       frequency reading too much */
                    /* The mains should be <40Hz or >70Hz to fail this test! */
                    if (256*SAMPLES_PER_10_SECONDS/700 <= phase->metrology.cycle_sample_count  &&  phase->metrology.cycle_sample_count <= 256*SAMPLES_PER_10_SECONDS/400)
                    {
                        /* A mains frequency measurement procedure based on interpolating zero crossings,
                           to get a fast update rate for step changes in the mains frequency */
                        /* Interpolate the zero crossing by successive approx. */
                        z = V_sample - phase->metrology.last_V_sample;
                        x = 0;
                        y = 0;
                        for (k = 0;  k < 8;  k++)
                        {
                            y <<= 1;
                            z >>= 1;
                            x += z;
                            if (x > V_sample)
                                x -= z;
                            else
                                y |= 1;
                        }
                        /* Now we need to allow for skipped samples, due to spike detection */
                        z = y;
                        while (phase->metrology.since_last > 1)
                        {
                            z += y;
                            phase->metrology.since_last--;
                        }
                        /* z is now the fraction of a sample interval between the zero
                           crossing and the current sample, in units of 1/256 of a sample */
                        /* A lightly damped filter should now be enough to remove noise and get a
                           stable value for the frequency */
                        phase->metrology.mains_period += ((int32_t) (phase->metrology.cycle_sample_count - z) << 12) - (phase->metrology.mains_period >> 4);
                        /* Start the next cycle with the residual fraction of a sample */
                        phase->metrology.cycle_sample_count = z;
                    }
                    else
                    {
                        phase->metrology.cycle_sample_count = 0;
                    }
                  #endif
                  #if defined(POWER_FACTOR_SUPPORT)
                    /* Determine whether the current leads or lags, in a noise tolerant manner.
                       Testing 50 cycles means we will respond in about one second to a genuine
                       swap between lead and lag. Since that is also about the length of our
                       measurement blocks, this seems a sensible response time. */
                    if (I_live_sample < V_sample)
                    {
                        if (phase->metrology.current.leading > -50)
                            phase->metrology.current.leading--;
                    }
                    else
                    {
                        if (phase->metrology.current.leading < 50)
                            phase->metrology.current.leading++;
                    }
                  #endif
                  /* See if a sufficiently long measurement interval has been
                       recorded, and catch the start of the next cycle. We do not
                       really care how many cycles there are, as long as the block
                       is a reasonable length. Setting a minimum of 1 second is
                       better than counting cycles, as it is not affected by noise
                       spikes. Synchronising to a whole number of cycles reduces
                       block to block jitter, though it doesn't affect the long
                       term accuracy of the measurements. */
                  if (phase->metrology.dot_prod.sample_count >= samples_per_second)
                  {
                      log_parameters(phase);
                      _BIC_SR_IRQ(LPM0_bits);
                      /* When run as a host program there is no RTC, but we still need to
                        kick the foreground somehow. */
                  }
                }
                /* Log the sign of the signal */
                phase->status |= V_POS;
            }
            phase->metrology.since_last = 0;
            phase->metrology.last_V_sample = V_sample;
        }
        phase->metrology.since_last++;
        if (phase->metrology.dot_prod.sample_count >= samples_per_second + 200)
        {
            /* We don't seem to be detecting the end of a mains cycle, so force
               the end of processing block condition. */
            log_parameters(phase);
            _BIC_SR_IRQ(LPM0_bits);
        }
        #if defined(MAINS_FREQUENCY_SUPPORT)
            phase->metrology.current.cycle_sample_count += 256;
        #endif
        if (I_live_sample < 0)
        {
            /* Log the sign of the signal */
            phase->status &= ~I_POS;
        }
        else
        {
            if (!(phase->status & I_POS))
            {
                /* A negative to positive transition has occurred. Trust it
                   blindly as a genuine zero crossing/start of cycle, even
                   though it might really be due to a noise spike. */
                #if defined(MAINS_FREQUENCY_SUPPORT)
                                if (SAMPLES_PER_10_SECONDS/700 <= phase->metrology.current.cycle_sample_count  &&  phase->metrology.current.cycle_sample_count <= SAMPLES_PER_10_SECONDS/400)
                                    phase->metrology.current.mains_period += ((int32_t) phase->metrology.current.cycle_sample_count << 16) - (phase->metrology.current.mains_period >> 8);
                                phase->metrology.current.cycle_sample_count = 0;
                #endif
            }
            /* Log the sign of the signal */
            phase->status |= I_POS;
        }
        phase++;
    }

    #if  defined(NEUTRAL_MONITOR_SUPPORT)  &&  defined(IRMS_SUPPORT)
        /* For multi-phase meters, neutral monitoring is limited to measuring the
           RMS current. */
        corrected = adc_i_buffer[3];
        if ((corrected >= I_ADC_MAX  ||  corrected <= I_ADC_MIN)  &&  neutral.metrology.I_endstops)
            neutral.metrology.I_endstops--;
        I_live_sample = corrected; 
        I_live_sample = dc_filter_current(neutral.metrology.I_dc_estimate[0], I_live_sample);
        sqac_current(neutral.metrology.I_sq_accum, I_live_sample);
        if (++neutral.metrology.sample_count >= samples_per_second)
            log_neutral_parameters();
    #endif
  
    #if defined(TOTAL_ACTIVE_ENERGY_SUPPORT)
        /* We now play the last measurement interval's power level, evaluated
           in the foreground, through this measurement interval. In this way
           we can evenly pace the pulsing of the LED. The only error produced
           by this is the ambiguity in the number of samples per measurement.
           This should not exceed 1 or 2 in over 4000. */
          
        #if defined(INHIBIT_NEGATIVE_TOTAL_POWER_ACCUMULATION)
            if (total_active_power > 0  &&  (total_active_power_counter += total_active_power) >= TOTAL_ENERGY_PULSE_THRESHOLD)
        #else
            if ((total_active_power_counter += total_active_power) >= TOTAL_ENERGY_PULSE_THRESHOLD)
        #endif
            {
                total_active_power_counter -= TOTAL_ENERGY_PULSE_THRESHOLD;
        #if TOTAL_ENERGY_PULSES_PER_KW_HOUR < 1000
                if (++extra_total_active_power_counter >= 16)
                {
                    extra_total_active_power_counter = 0;
        #endif
                    ++total_consumed_active_energy;
                    /* Ideally we want to log the energy each kWh unit, but doing
                       it with a mask here is good enough and faster. */
                    if ((total_consumed_active_energy & 0x3FF) == 0)
                        phase->status |= ENERGY_LOGABLE;
                    /* Pulse the LED. Long pulses may not be reliable, as at full
                       power we may be pulsing many times per second. People may
                       check the meter's calibration with an instrument that counts
                       the pulsing rate, so it is important the pulses are clear,
                       distinct, and exactly at the rate of one per
                       1/ENERGY_PULSES_PER_KW_HOUR kW/h. */
        #if defined(total_active_energy_pulse_start)
            #if defined(FINE_ENERGY_PULSE_TIMING_SUPPORT)
                    /* TODO: This will not work if extra_total_active_power_counter is in use */
                    /* Work out the fraction of an ADC interrupt, in 1/10ths of an ADC interrupt period,
                       at which the real transition occurs */
                    /* Break up the loop a bit, for efficiency */
                    xxx = total_active_power_counter << 1;
                    j = 10;
                    if (xxx >= total_active_power)
                    {
                        xxx -= total_active_power;
                        j = 5;
                    }
                    xxx += (xxx << 2);
                    do
                        j--;
                    while ((xxx -= total_active_power) > 0)
                        ;
                    /* j is now our fraction of an ADC interrupt. If we use this right now to control timer A
                       its effect would be indeterminate. We need timer A to be updated at the very start of an
                       ADC interrupt service, to ensure the fraction of an ADC interrupt is programmed into the
                       timer while its count is at a well defined value - zero. */
                    fine_pulse_operation = OUTMOD_5 | SCS;
                    TACCR2 = j;
            #else
                    total_active_energy_pulse_start();
            #endif
                    total_active_energy_pulse_remaining_time = ENERGY_PULSE_DURATION;
        #endif
        #if TOTAL_ENERGY_PULSES_PER_KW_HOUR < 1000
                }
        #endif
            }
        #if defined(total_active_energy_pulse_start)
            if (total_active_energy_pulse_remaining_time  &&  --total_active_energy_pulse_remaining_time == 0)
            {
            #if defined(FINE_ENERGY_PULSE_TIMING_SUPPORT)
                /* Turn off the indicator at the next CCR2 match. */
                fine_pulse_operation = OUTMOD_1 | SCS;
                /* Leave TACCR2 alone, and we should get the same offset as last time, resulting in even length pulses */
            #else
                total_active_energy_pulse_end();
            #endif
            }
        #endif
    
        #if defined(INHIBIT_NEGATIVE_TOTAL_POWER_ACCUMULATION)
            if (total_reactive_power > 0  &&  (total_reactive_power_counter += total_reactive_power) >= TOTAL_ENERGY_PULSE_THRESHOLD)
        #else
            if ((total_reactive_power_counter += total_reactive_power) >= TOTAL_ENERGY_PULSE_THRESHOLD)
        #endif
            {
                total_reactive_power_counter -= TOTAL_ENERGY_PULSE_THRESHOLD;
        #if TOTAL_ENERGY_PULSES_PER_KW_HOUR < 1000
                if (++extra_total_reactive_power_counter >= 16)
                {
                    extra_total_reactive_power_counter = 0;
        #endif
                    ++total_consumed_reactive_energy;
                    /* Ideally we want to log the energy each kWh unit, but doing
                       it with a mask here is good enough and faster. */
                    if ((total_consumed_reactive_energy & 0x3FF) == 0)
                        phase->status |= ENERGY_LOGABLE;
                    /* Pulse the LED. Long pulses may not be reliable, as at full
                       power we may be pulsing many times per second. People may
                       check the meter's calibration with an instrument that counts
                       the pulsing rate, so it is important the pulses are clear,
                       distinct, and exactly at the rate of one per
                       1/ENERGY_PULSES_PER_KW_HOUR kW/h. */
        #if defined(total_reactive_energy_pulse_start)
            #if defined(FINE_ENERGY_PULSE_TIMING_SUPPORT)
                    /* TODO: This will not work if extra_total_active_power_counter is in use */
                    /* Work out the fraction of an ADC interrupt, in 1/10ths of an ADC interrupt period,
                       at which the real transition occurs */
                    /* Break up the loop a bit, for efficiency */
                    xxx = total_reactive_power_counter << 1;
                    j = 10;
                    if (xxx >= total_reactive_power)
                    {
                        xxx -= total_reactive_power;
                        j = 5;
                    }
                    xxx += (xxx << 2);
                    do
                        j--;
                    while ((xxx -= total_reactive_power) > 0);
                    /* j is now our fraction of an ADC interrupt. If we use this right now to control timer A
                       its effect would be indeterminate. We need timer A to be updated at the very start of an
                       ADC interrupt service, to ensure the fraction of an ADC interrupt is programmed into the
                       timer while its count is at a well defined value - zero. */
                    fine_pulse_operation = OUTMOD_5 | SCS;
                    TACCR2 = j;
            #else
                    total_reactive_energy_pulse_start();
            #endif
                    total_reactive_energy_pulse_remaining_time = ENERGY_PULSE_DURATION;
        #endif
        #if TOTAL_ENERGY_PULSES_PER_KW_HOUR < 1000
            }
        #endif
            }
        #if defined(total_reactive_energy_pulse_end)
            if (total_reactive_energy_pulse_remaining_time  &&  --total_reactive_energy_pulse_remaining_time == 0)
            {
            #if defined(FINE_ENERGY_PULSE_TIMING_SUPPORT)
                /* Turn off the LED at the next TACCR2 match. */
                fine_pulse_operation = OUTMOD_1 | SCS;
                /* Leave TACCR2 alone, and we should get the same offset as last time, resulting in even length pulses */
            #else
                total_reactive_energy_pulse_end();
            #endif
            }
        #endif
    #endif

    custom_adc_interrupt();

    #if defined(x__MSP430__)
//      #if defined(POWER_DOWN_SUPPORT)  &&  defined(POWER_UP_BY_SUPPLY_SENSING)
//      /* Select the lower threshold to watch for the power supply dying. */ 
//        CACTL1 = CAREF_1 | CAON;
//      #endif
      #if defined(BASIC_KEYPAD_SUPPORT)  ||  defined(CUSTOM_KEYPAD_SUPPORT)
      if (keypad_debounce())
          _BIC_SR_IRQ(LPM0_bits);
      #endif

      #if defined(PWM_DITHERING_SUPPORT)
      if (operating_mode == OPERATING_MODE_NORMAL)
      {
          /* Only try to dither when in full operating mode.
             The dithering increases current consumtion a little! */ 
          /* Calculate new PWM duty cycle for the dithering */
          if ((meter_status & PWM_RAMPING_DOWN))
          {
              if (!(TBCCR1 == PWM_MID_POINT  &&  pwm_stutter--))
              {
                  if (--TBCCR1 <= PWM_LOWER_LIMIT)
                  {
                      meter_status &= ~PWM_RAMPING_DOWN;
                      /* Set a small randomised stutter for the cycle
                         of the triangular dithering waveform to avoid the
                         possibility of high correlation with the mains
                         waveform. */
                      pwm_stutter = ((rndnum >> 8) & 0x7);
                  }
              }
          }
          else
          {
              if (++TBCCR1 >= PWM_UPPER_LIMIT)
                  meter_status |= PWM_RAMPING_DOWN;
          }
      }
      #endif
      #if defined(POWER_DOWN_SUPPORT)  &&  defined(POWER_UP_BY_SUPPLY_SENSING)
        #if defined(__MSP430_HAS_COMPA__)
          if ((CACTL2 & CAOUT))
        #else
    /* Use an I/O pin to sense the power falling */
          POWER_GOOD_THRESHOLD_LOW;
          if (!POWER_GOOD_SENSE)
        #endif
          {
        /* The comparator output can oscillate a little around the
           switching point, so we need to do some debouncing. */
              if (power_down_debounce < POWER_FAIL_DEBOUNCE + 1)
              {
                  if (++power_down_debounce == POWER_FAIL_DEBOUNCE)
                  {
                      power_down_debounce = 0;
                      /* The power is falling. We need to get to a low power
                         consumption state now! The battery will be supplying the
                         meter soon. */
                      operating_mode = OPERATING_MODE_POWERFAIL;
                      _BIC_SR_IRQ(LPM0_bits);
                  }
              }
          }
          else
          {
              power_down_debounce = 0;
          }
          #if defined(__MSP430_HAS_COMPA__)
            CACTL1 &= ~(CAON);
          #endif
      #endif
    #endif
    if ((TST_SD16IFG_CURRENT_1))
    {
        adc_i_buffer[0] = SD16MEM_CURRENT_1;
        CLR_SD16IFG_CURRENT_1;
    }
    if ((TST_SD16IFG_CURRENT_2))
    {
        adc_i_buffer[1] = SD16MEM_CURRENT_2;
        CLR_SD16IFG_CURRENT_2;
    }
    if ((TST_SD16IFG_CURRENT_3))
    {
        adc_i_buffer[2] = SD16MEM_CURRENT_3;
        CLR_SD16IFG_CURRENT_3;
    }
     if ((TST_SD16IFG_NEUTRAL))
    {
        adc_i_buffer[3] = SD16MEM_NEUTRAL;
        CLR_SD16IFG_NEUTRAL;
    }
    adc_interrupts++;
}


