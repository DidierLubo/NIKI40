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
//  File: emeter-main.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: emeter-main.c,v 1.10 2009/04/27 06:21:22 a0754793 Exp $
//
/*! \file emeter-structs.h */
//
//--------------------------------------------------------------------------
//
//  MSP430 foreground (non-interrupt) routines for e-meters
//
//  This software is appropriate for single phase and three phase e-meters
//  using a voltage sensor plus a CT or shunt resistor current sensors, or
//  a combination of a CT plus a shunt.
//
//    Foreground process includes:
//    -Using timer tick to wait
//    -Calculating the power per channel
//    -Determine if current channel needs scaling.
//    -Determine if needs to be in low power modes.
//    -Compensate reference from temperature sensor
//
//--------------------------------------------------------------------------
// ESIMA-E-Meter Bernd Ehrbrecht, Jürgen Merz, Daniel Stojakov(LWM2M + UART)
// Stand 30.04.2017
// TI- E-Meter Software an neues Hardware Design angepasst
// LCD Display auf DOG102 umgestellt
// NFC- Modul implementiert
// ESIMA- Radio-Modul implementiert, nach ESIMA- Record-Definitionen
//---------------------------------------------------------------------------
#include "Hardware.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//#if defined(__GNUC__)
//#include <signal.h>
//#endif
#include <math.h>
//#include <io.h>
#include <stdio.h>
#include <stdbool.h>
//#include <emeter-toolkit.h>
#define __MAIN_PROGRAM__

#include "emeter-structs.h"
#if defined(MESH_NET_SUPPORT)
#include "mesh_structure.h"
#endif

#include "HAL_Dogs102x6.h"
#include "RF430.h"
#include "RadioEsimaLWM2M.h"
#include "LWM2M_App.h"
#include "Timer_Modul.h"
#include "LWM2M.h"

//#include "emeter-rtc.h"

static void Radio_Reset_On(void);
static void Radio_Reset_Off(void);

#if defined(TOTAL_ACTIVE_ENERGY_SUPPORT)
typedef union
{
    unsigned char uint8[4];
    unsigned int uint16[2];
    uint32_t uint32;
} power_array;
uint8_t total_active_energy_pulse_remaining_time;
int32_t total_active_power;
power_array    total_active_power_array;
int32_t total_active_power_counter;
    #if TOTAL_ENERGY_PULSES_PER_KW_HOUR < 1000
int16_t extra_total_active_power_counter;
    #endif
uint32_t total_consumed_active_energy;
#endif

#if defined(TOTAL_REACTIVE_ENERGY_SUPPORT)
uint8_t total_reactive_energy_pulse_remaining_time;
int32_t total_reactive_power;
int32_t total_reactive_power_counter;
    #if TOTAL_ENERGY_PULSES_PER_KW_HOUR < 1000
int16_t extra_total_reactive_power_counter;
    #endif
uint32_t total_consumed_reactive_energy;
#endif

#if 0 //CUSTOM_LCD_SUPPORT

#endif

#if defined(TEMPERATURE_SUPPORT)
uint16_t temperature;
#endif

float fVolatgeRMS[3];
float fCurrentRMS[4];
float fFrequency [3];
float fActivePower[3];
float fReactivePower[3];
float fApparentPower[3];
float fPowerFactor[3];
char LCD_String[20];

/* Meter status flag bits. */
uint16_t meter_status;

/* Current operating mode - normal, limp, power down, etc. */
int8_t operating_mode;

volatile ST_UART_t* uartObj;

/* Persistence check counters for anti-tamper measures. */
#if defined(PHASE_REVERSED_DETECTION_SUPPORT)
int8_t current_reversed;
#endif
#if defined(POWER_BALANCE_DETECTION_SUPPORT)
int8_t current_unbalanced;
#endif
#if defined(MAGNETIC_INTERFERENCE_SUPPORT)
int8_t magnetic_interference_persistence;
#endif

#if defined(IHD430_SUPPORT)
unsigned char RF_Tx[17]={0xFE,0x0C,0x29,0x00,0x09,0x00,0x00,0x00,0x00,0x00,0x05,0x01};
#endif

/* The main per-phase working parameter structure */
struct phase_parms_s chan[NUM_PHASES];
    #if defined(NEUTRAL_MONITOR_SUPPORT)
struct neutral_parms_s neutral;
    #endif



/* The main per-phase non-volatile parameter structure */
// Geändert von nv_parms zu nv_parms_default
__no_init struct info_mem_s nv_parms @ "INFO";
static const struct info_mem_s nv_parms_default =
{
    {
    {
        0,
#if defined(SELF_TEST_SUPPORT)
        0xFFFF,
        0xFFFF,
#endif
        {
            {
                {
#if defined(IRMS_SUPPORT)
                    {DEFAULT_I_RMS_SCALE_FACTOR_A},
                    0,
#endif

                    DEFAULT_P_SCALE_FACTOR_A_LOW,
                    DEFAULT_I_DC_ESTIMATE << 16,
#if defined(PHASE_CORRECTION_SUPPORT)
                    DEFAULT_BASE_PHASE_A_CORRECTION_LOW,
#endif
                },
#if defined(VRMS_SUPPORT)
                DEFAULT_V_RMS_SCALE_FACTOR_A,
#endif
                DEFAULT_V_DC_ESTIMATE << 16,
            },
            {
                {
    #if defined(IRMS_SUPPORT)
                    {DEFAULT_I_RMS_SCALE_FACTOR_B},
                    0,
    #endif
                    DEFAULT_P_SCALE_FACTOR_B_LOW,
                    DEFAULT_I_DC_ESTIMATE << 16,
    #if defined(PHASE_CORRECTION_SUPPORT)
                    DEFAULT_BASE_PHASE_B_CORRECTION_LOW,
    #endif
                },
    #if defined(VRMS_SUPPORT)
                DEFAULT_V_RMS_SCALE_FACTOR_B,
    #endif
                DEFAULT_V_DC_ESTIMATE << 16,
            },
            {
                {
    #if defined(IRMS_SUPPORT)
                    {DEFAULT_I_RMS_SCALE_FACTOR_C},
                    0,
    #endif
                    DEFAULT_P_SCALE_FACTOR_C_LOW,
                    DEFAULT_I_DC_ESTIMATE << 16,
    #if defined(PHASE_CORRECTION_SUPPORT)
                    DEFAULT_BASE_PHASE_C_CORRECTION_LOW,
    #endif
                },
    #if defined(VRMS_SUPPORT)
                DEFAULT_V_RMS_SCALE_FACTOR_C,
    #endif
                DEFAULT_V_DC_ESTIMATE << 16,
            }
        },
#if defined(NEUTRAL_MONITOR_SUPPORT)
        {
    #if defined(IRMS_SUPPORT)
            DEFAULT_I_RMS_SCALE_FACTOR_NEUTRAL,
            0,
    #endif
            DEFAULT_P_SCALE_FACTOR_NEUTRAL,
            DEFAULT_I_DC_ESTIMATE << 16,
    #if defined(PHASE_CORRECTION_SUPPORT)
            DEFAULT_NEUTRAL_BASE_PHASE_CORRECTION,
    #endif
        },
#endif
#if defined(TEMPERATURE_SUPPORT)
        25,
        DEFAULT_TEMPERATURE_OFFSET,
        DEFAULT_TEMPERATURE_SCALING,
#endif



#if defined(CORRECTED_RTC_SUPPORT)
        0x0000,                  // crystal_base_Correction
        0x4000,                 // crystal_tco_correction
#endif
        {
            0,
            0,
            0,
            0,
            0,
            0
        },
        "",
        "",
        "",
//        { 0x48, 0x00, 0x00, 0x10 }                    // Funk-ID
    }
    }
};

__no_init const uint8_t gbRadio_ID[4] @ "INFOB";                // Diesen Wert ins INFO-Flash segment B mit Adresse 0x1900 speichern

#if !defined(__IAR_SYSTEMS_ICC__)
static __inline__ long labs(long __x);
static __inline__ long labs(long __x)
{
    return (__x < 0) ? -__x : __x;
}
#endif

#if defined(SELF_TEST_SUPPORT)
int record_meter_failure(int type)
{
    /* The error type should be a value between 0 and 15, specifying the unrecoverable error
       type to be recorded in the failures word in flash. */
    /* Don't worry about the time taken to write to flash - we are recording a serious
       error condition! */
    flash_write_int16((int *) &(nv_parms.seg_a.s.meter_failures), nv_parms.seg_a.s.meter_failures & ~(1 << type));
    flash_secure();
    return TRUE;
}

int record_meter_warning(int type)
{
    /* The warning type should be a value between 0 and 15, specifying the warning type to be
       recorded in the recoverable failures word in flash. */
    /* Don't worry about the time taken to write to flash - we are recording a serious
       problem! */
    flash_write_int16((int *) &(nv_parms.seg_a.s.meter_warnings), nv_parms.seg_a.s.meter_warnings & ~(1 << type));
    flash_secure();
    return TRUE;
}
#endif

#if defined(BATTERY_MONITOR_SUPPORT)
void test_battery(void)
{
    P3DIR |= (BIT1);
    P3OUT &= ~(BIT1);
    battery_countdown = 1000;
}
#endif

#if defined(IO_EXPANDER_SUPPORT)
/* This routine supports the use of a device like the 74HC595 to expand the number of
   output bits available on the lower pin count MSP430s. */
void set_io_expander(int what, int which)
{
    static uint8_t io_state = 0;
    int i;
    int j;

    if (what < 0)
        io_state &= ~which;
    else if (what > 0)
        io_state |= which;
    else
        io_state = which;
    /* Pump the data into the shift register */
    for (i = 8, j = io_state;  i > 0;  i--)
    {
        P1OUT &= ~BIT4;
        if ((j & 0x80))
            P1OUT |= BIT7;
        else
            P1OUT &= ~BIT7;
        P1OUT |= BIT4;
        j <<= 1;
    }
    /* Clock the data into the output register */
    P1OUT &= ~BIT6;
    P1OUT |= BIT6;
}
#endif


enum display_LCD
{
   DISPLAY_PHASE1                          = 0,
   DISPLAY_PHASE2                          = 1,
   DISPLAY_PHASE3                          = 2,
   DISPLAY_END                             = 3

};


//UART Verbessern
void main(void)
{
    int ch;
    static struct phase_parms_s *phase;
    static struct phase_nv_parms_s const *phase_nv;
    LWM2M_APP_ERROR errCode = LWM2M_APP_ERROR_INIT;
    
    WDTCTL = WDTPW | WDTHOLD;
      
    _NOP();
    __delay_cycles( 100000 ); //100ms

    //    if (nv_parms.seg_a.s.meter_uncalibrated)           // Würde ausreichen, wenn die PC-Software dieses Feld nicht wieder auf 0xFFFF setzen würde
    if (nv_parms.seg_a.s.chan[0].V_rms_scale_factor == -1 && // Also wenn alle Skalenfaktoren = -1 = 0xFFFF dann ist Info-Memory leer und muss mit default-Werten belegt werden
        nv_parms.seg_a.s.chan[1].V_rms_scale_factor == -1 &&
        nv_parms.seg_a.s.chan[2].V_rms_scale_factor == -1 )
    {
       flash_clr( (int*) &nv_parms );                         // Sicherheitshalber Info-Memory löschen - eigentlich nicht notwendig da bereits leer sein sollte
       flash_memcpy( (char*) &nv_parms, (char*) &nv_parms_default, sizeof(nv_parms_default));
    }

    system_setup();
    
    #if !defined(ESP_SUPPORT)  &&  defined(PHASE_CORRECTION_SUPPORT)  &&  !defined(DYNAMIC_PHASE_CORRECTION_SUPPORT)
        for (ch = 0;  ch < NUM_PHASES;  ch++)
        {
            phase = &chan[ch];
            phase_nv = &nv_parms.seg_a.s.chan[ch];
            set_sd16_phase_correction(&phase->metrology.current.in_phase_correction[0], ch, phase_nv->current.phase_correction[0]);
        }
    #endif

    #if defined DOG102X6_LCD_SUPPORT
        Dogs102x6_init();
        Dogs102x6_clearScreen();
        Dogs102x6_stringDraw(0, 19, "ESIMA LWM2M", DOGS102x6_DRAW_NORMAL);
        Dogs102x6_stringDraw(2, 0, "Status: Init...", DOGS102x6_DRAW_NORMAL);
    #endif

    //Put LWM2M Client into reset state
    Radio_Reset_On();
    
    //NFC-Initialisierung
    //RF430_Init();

    //Initialize Timer A0
    InitTimerA0();

    //Initialize UART-Interface UCA3
    uartObj = UART_Init(3, BR57600, false);
    
    //Release LWM2M Client from reset state
    Radio_Reset_Off();

    __delay_cycles( 15000000 ); //1500ms
    
    //Init LWM2M application
    errCode = LWM2M_App_Init(uartObj);
   
    // *** Main loop
    for(;;)
    {
        kick_watchdog();

        //RF430_Task();

        phase = chan;// Set local phase pointer = measuring channel pointer
        phase_nv = nv_parms.seg_a.s.chan;

        errCode = LWM2M_App_Task();
        
        if ((phase->status & NEW_LOG))
        {
          phase->status &= ~NEW_LOG;

          // *** Check for power status
          if (PGOOD() || isNFC_RF_FIELD_ON ) // Wenn Stromversorgung an oder NFC-Feld dedektiert
          {
            BACKLIGHT_ON();
          }
          else
          {
            BACKLIGHT_OFF();
          }
          if (PGOOD())
          {
             CHARGE_ON(); // Switch charging on
          }
          else
          {
             CHARGE_OFF(); // Switch charging on
          }

          if (CHARGE())
          {
            LED_ON(BIT0);          // LED red on
          }
          else
          {
            LED_OFF(BIT0);         // LED red on
          }


        // *****************************************************************************
          for (ch = 0;  ch < NUM_PHASES;  ch++)
          {
              /* Unless we are in normal operating mode, we should wait to be
                 woken by a significant event from the interrupt routines. */
              #if defined(POWER_DOWN_SUPPORT)
                if (operating_mode == OPERATING_MODE_POWERFAIL)
                    switch_to_powerfail_mode();
              #endif

                phase->readings.I_rms = current(phase, phase_nv, ch);
                phase->readings.V_rms = voltage(phase, phase_nv);
                phase->readings.frequency = frequency(phase, phase_nv);
                phase->readings.active_power   = active_power(phase, phase_nv);
                phase->readings.reactive_power = reactive_power(phase, phase_nv);
                phase->readings.apparent_power = apparent_power(phase, phase_nv);

                phase->readings.power_factor = power_factor(phase, phase_nv);

                fVolatgeRMS[ch] = (float) phase->readings.V_rms /100;
                fCurrentRMS[ch] = (float) phase->readings.I_rms/1000*IRMS_E_GAIN_FACTOR;

                fFrequency [ch] = (float) phase->readings.frequency/100;
                fActivePower[ch] = (float) phase->readings.active_power/100*IRMS_E_GAIN_FACTOR;
                fReactivePower[ch] = (float) phase->readings.reactive_power/100*IRMS_E_GAIN_FACTOR;
                fApparentPower[ch] =(float) phase->readings.apparent_power/100*IRMS_E_GAIN_FACTOR;
                fPowerFactor[ch] = (float) phase->readings.power_factor/10000;

                phase++;
                phase_nv++;
            }

            // NFC-Update
            // RF430_UpdateEmeter(chan);

          if(errCode == LWM2M_APP_ERROR_OK)
          {
            // LWM2M Object Updates
             Radio_UpdateEmeter_LWM2M(chan);
          }
        }// END OF CHANNEL FOR LOOP

         if ((meter_status & TICKER))
         {
            meter_status &= ~TICKER;
         }
         
         custom_keypad_handler();
         custom_mainloop_handler();

        } //End of the infinite FOR loop
} // END OF MAIN



#if defined(PRECALCULATED_PARAMETER_SUPPORT)
  int32_t current_consumed_active_energy(int ph)
  {
    #if defined(TOTAL_ACTIVE_ENERGY_SUPPORT)
      if (ph == FAKE_PHASE_TOTAL)
        return total_consumed_active_energy;
    #endif
    #if defined(PER_PHASE_ACTIVE_ENERGY_SUPPORT)
      return chan[ph].consumed_active_energy;
    #else
      return 0;
    #endif
  }
  int32_t current_active_power(int ph)
  {
      if (ph == FAKE_PHASE_TOTAL)
        return total_active_power;
      return chan[ph].readings.active_power;
  }
  #if defined(REACTIVE_POWER_SUPPORT)
    int32_t current_consumed_reactive_energy(int ph)
    {
        #if defined(TOTAL_REACTIVE_ENERGY_SUPPORT)
            if (ph == FAKE_PHASE_TOTAL)
                return total_consumed_reactive_energy;
        #endif
        #if defined(PER_PHASE_REACTIVE_ENERGY_SUPPORT)
            return chan[ph].consumed_reactive_energy;
        #else
            return 0;
        #endif
    }
    int32_t current_reactive_power(int ph)
    {
        if (ph == FAKE_PHASE_TOTAL)
            return total_reactive_power;
        return chan[ph].readings.reactive_power;

    }
  #endif
  #if defined(APPARENT_POWER_SUPPORT)
    int32_t current_apparent_power(int ph)
    {
       return chan[ph].readings.apparent_power;
    }
  #endif

  #if defined(POWER_FACTOR_SUPPORT)
    int32_t current_power_factor(int ph)
    {
      return chan[ph].readings.power_factor;
    }
  #endif

  #if defined(VRMS_SUPPORT)
    int32_t current_rms_voltage(int ph)
    {
      int32_t x;
      if (chan[ph].readings.V_rms == 0xFFFF)
          x = -1;
      else
          x = chan[ph].readings.V_rms;
      return x;
    }
  #endif

  #if defined(IRMS_SUPPORT)
    int32_t current_rms_current(int ph)
    {
      int32_t x;
      #if defined(NEUTRAL_MONITOR_SUPPORT)
        if (ph == 3)
          x = neutral.readings.I_rms;
        else
      #endif
      x = chan[ph].readings.I_rms;
      if (x == 0xFFFF)
        x = -1;
      return x;
    }
  #endif

  #if defined(MAINS_FREQUENCY_SUPPORT)
  int32_t current_mains_frequency(int ph)
  {
      return chan[ph].readings.frequency;
  }
  #endif
#else
  int32_t current_consumed_active_energy(int ph)
  {
      return chan[ph].consumed_active_energy;
  }

  int32_t current_active_power(int ph)
  {
      if (ph == FAKE_PHASE_TOTAL)
          return total_active_power;
      return active_power(&chan[ph], &nv_parms.seg_a.s.chan[ch]);
  }
  #if defined(REACTIVE_POWER_SUPPORT)
    int32_t current_consumed_reactive_energy(int ph)
    {
        return chan[ph].consumed_active_energy;
    }

    int32_t current_reactive_power(int ph)
    {
        if (ph == FAKE_PHASE_TOTAL)
            return total_reactive_power;
        return reactive_power(&chan[ph], &nv_parms.seg_a.s.chan[ch]);
    }
  #endif
  #if defined(APPARENT_POWER_SUPPORT)
    int32_t current_apparent_power(int ph)
    {
        return apparent_power(&chan[ph], &nv_parms.seg_a.s.chan[ch]);
    }
  #endif
  #if defined(POWER_FACTOR_SUPPORT)
    int32_t current_power_factor(int ph)
    {
        return power_factor(&chan[ph], &nv_parms.seg_a.s.chan[ch]);
    }
  #endif
  #if defined(VRMS_SUPPORT)
    int32_t current_rms_voltage(int ph)
    {
        return voltage(&chan[ph], &nv_parms.seg_a.s.chan[ch]);
    }
  #endif

  #if defined(IRMS_SUPPORT)
    int32_t current_rms_current(int ph)
    {
        return current(&chan[ph], &nv_parms.seg_a.s.chan[ch]);
    }
  #endif

  #if defined(MAINS_FREQUENCY_SUPPORT)
    int32_t current_mains_frequency(int ph)
    {
        return frequency(&chan[ph], &nv_parms.seg_a.s.chan[ch]);
    }
  #endif
#endif



static void Radio_Reset_On(void)
{
  _RadioReset_L();
}



static void Radio_Reset_Off(void)
{
  _RadioReset_H();
}