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

//
//--------------------------------------------------------------------------

/* This is a demonstration version of the configuration data file for the
   MSP430 e-meter reference software. For most users, this file will need
   to be edited to match the hardware being used. */

/* N.B.
   This e-meter software uses a sampling rate of 3276.8/s. For a
   mains supply at exactly 50Hz, each sample is 5.49degrees of
   phase from the last. */


#define TWENTYFOUR_BIT      1

// Define this macro so can send active power readings to the IHD430
//#define IHD430_SUPPORT

/*! This switch, in combination with the calibrator switch, enables calibration
    with the meter cooperating with an external reference, through a UART port. */
#define SERIAL_CALIBRATION_SUPPORT
#define SERIAL_CALIBRATION_PASSWORD_1               0x1234
#define SERIAL_CALIBRATION_PASSWORD_2               0x5678
#define SERIAL_CALIBRATION_PASSWORD_3               0x9ABC
#define SERIAL_CALIBRATION_PASSWORD_4               0xDEF0

/*! This switch enables the sending of the current readings, through a UART port,
    for use in cooperative calibration with other meters. */
#define SERIAL_CALIBRATION_REF_SUPPORT

/*! This switch enables mains frequency measurement. This may be used as a
    meter feature. It may be a requirement, if non-linear CT sensors are used. */
#define MAINS_FREQUENCY_SUPPORT                     1

/*! The nominal mains frequency is used to prime the mains frequency measurement,
    and make its initial value settle quickly. It is not currently used after
    reset. */
#define MAINS_NOMINAL_FREQUENCY                     50

/*! This sets the number of pulses per kilo-watt hour the meter will produce at
    its total energy pulse LED. It does not affect the energy accumulation process. */
#define TOTAL_ENERGY_PULSES_PER_KW_HOUR             6400

/*! This sets the number of pulses per kilo-watt hour the meter will produce at
    each phase's energy pulse LED. It does not affect the energy accumulation
    process. */
#define PHASE_ENERGY_PULSES_PER_KW_HOUR             6400

/*! This is called after the meter has been initialised, and before interrupts are
    enabled for the first time. This is just before the meter enters power
    down mode waiting for the first power good indication. It may be used to implement
    customised initialisation of the meter's special features. */
#define custom_initialisation()                     /**/

/*! This is called just before the meter enters the power fail state. Since the power is
    failing when this is called, the custom routine should not take an extremely long time to
    execute. The maximum safe time, before energy is drawn from the battery, will depend on the
    hardware design, and how quickly the reservoir capacitor may deplete. There should be
    plenty of time to do things like saving essential usage information to EEPROM. */
#define custom_power_fail_handler()                 /**/

/*! This is called when the meter wakes from the power fail state. It is called after the
    main meter functions have all been restarted. */
#define custom_power_restore_handler()              /**/


/*! This is called to start some customer specific energy pulse operation. For example,
    turning on some special indicator device. */
#define custom_energy_pulse_start()                 (P5OUT &= ~BIT3)

/*! This is called to end some customer specific energy pulse operation. For example,
    turning off some special indicator device. */
#define custom_energy_pulse_end()                   (P5OUT |= BIT3)

/*! The duration of the LED on time for an energy pulse. This is measured in
    ADC samples . The maximum allowed is 255, giving a
    pulse of about 78ms. 163 gives a 50ms pulse. */
//#define ENERGY_PULSE_DURATION                       163
//#define ENERGY_PULSE_DURATION                       80
#define ENERGY_PULSE_DURATION                       20

/*! Tiny power levels should not record at all, as they may just be rounding errors,
    noise, or the consumption of the meter itself. This value is the cutoff level,
    in 0.01W increments. */
#define RESIDUAL_POWER_CUTOFF                       250



/*! This switch enables monitoring of the neutral lead for anti-fraud purposes. */
//#define NEUTRAL_MONITOR_SUPPORT                     1




/*! This is a shift value for comparing currents or powers when looking for
    imbalance between live and neutral. 3 give 12.5%. 4 give 6.25%. These are the
    two commonest values to use. The relaxed version is used at low power levels,
    where the values are less accurate, and a tight imbalance check might give false
    results. */
#define PERMITTED_IMBALANCE_FRACTION                4

/*! This is a relaxed version of the permitted imbalance fraction, for use at low
    powers/currents, where the power (and hence imbalance) measurement may be less
    precise. The tighter imbalance measurement may give false results under high
    EMI conditions. */
#define RELAXED_IMBALANCE_FRACTION                  2

/*! This definition specifies the port bit change required to turn on the
    normal operation indicator LED, when one is used. If this is defined,
    clr_normal_indicator() should also be defined. The normal operation LED will be
    lit when the mains voltage is normal. In limp mode, or power down mode,
    it is switched off. */
#define set_normal_indicator()                      /**/

/*! This definition specifies the port bit change required to turn off the
    normal operation indicator LED, when one is used.  If this is defined,
    set_normal_indicator() should also be defined. */
#define clr_normal_indicator()                      /**/

/*! This definition specifies the port bit change required to turn on the
    earthed condition indicator LED, when one is used. If this is defined,
    clr_earthed_indicator() should also be defined. The earthed LED will be lit when
    there is a substantial imbalance in power (normal operating mode) or
    current (limp mode) between the live and neutral leads. If the neutral
    has the higher reading, this will also be used instead of the live reading. */
#define set_earthed_indicator()                     /**/

/*! This definition specifies the port bit change required to turn off the
    earthed condition indicator LED, when one is used. If this is defined,
    set_earthed_indicator() should also be defined. */
#define clr_earthed_indicator()                     /**/

#if defined(__MSP430__)
/*! This is called to turn on a total energy pulse indicator (e.g. LED or LCD segment) */
#define total_active_energy_pulse_start()           (P5OUT &= ~BIT3)

/*! This is called to turn off a total energy pulse indicator (e.g. LED or LCD segment) */
#define total_active_energy_pulse_end()             (P5OUT |= BIT3)

/*! This is called to turn on a total reactive energy pulse indicator (e.g. LED or LCD segment) */
#define total_reactive_energy_pulse_start()         (P4OUT &= ~BIT3)

/*! This is called to turn off a total reactive energy pulse indicator (e.g. LED or LCD segment) */
#define total_reactive_energy_pulse_end()           (P4OUT |= BIT3)
#endif

/*! This is the number of successive measurement blocks which must agree the
    unbalanced status has changed before we accept it. */
#define PHASE_UNBALANCED_PERSISTENCE_CHECK          5

/*! This enables monitoring of the balance between the current in the live and neutral
    leads, for tamper detection. */
#define POWER_BALANCE_DETECTION_SUPPORT

/*! This is the minimum current level (limp mode) and minimum power level (normal
    mode) at which we will make checks for the earthed condition. Very small
    readings lack the accuracy and resolution needed to make meaningfulF comparisons
    between live and neutral. */
#define PHASE_UNBALANCED_THRESHOLD_CURRENT          500
#define PHASE_UNBALANCED_THRESHOLD_POWER            2000

/*! This selects the operation from current only, when only one lead is
    functioning, and the meter is powered by a parasitic CT supply attached to
    the leads. This is for anti-fraud purposes. Energy is accumulated at the
    highest possible rate, assuming unity power factor, and the nominal voltage */
#define LIMP_MODE_VOLTAGE_THRESHOLD                 50
#define NORMAL_MODE_VOLTAGE_THRESHOLD               80

/*! If limp mode is supported, this sets the threshold current, in mA, below which we
    no not operate. */
#define LIMP_MODE_MINIMUM_CURRENT                   800

/*! This selects the nominal voltage used for power calculations in limp mode */
#define MAINS_NOMINAL_VOLTAGE                       230

/*! This selects support for measuring the RMS voltage. */
#define VRMS_SUPPORT                                1

/*! This selects support for measuring the RMS current. */
#define IRMS_SUPPORT                                1

/*! This selects support for reactive power measurement. */
#define REACTIVE_POWER_SUPPORT                      1

/*! This selects support for reactive power measurement through quadrature processing.
    This is only effective when REACTIVE_POWER_SUPPORT is enabled. */
#define REACTIVE_POWER_BY_QUADRATURE_SUPPORT        1

/*! The selects support for apparent or VA power measurement. */
#define APPARENT_POWER_SUPPORT                      1

/*! This switch enables support for power factor measurement. This feature
    includes a lead/lag assessment. A frequency independant method, based
    on the ratio of scalar dot products, is used. */
#define POWER_FACTOR_SUPPORT

/*! This switch selects support for measuring the total active energy consumption. */
#define TOTAL_ACTIVE_ENERGY_SUPPORT                 1

/*! This switch selects support for measuring the active energy consumption on a phase
    by phase basis. This is only significant for poly-phase meters. */
#define PER_PHASE_ACTIVE_ENERGY_SUPPORT             1

#define phase_active_energy_pulse_start()          phase_1_active_energy_pulse_start()
#define phase_active_energy_pulse_end()            phase_1_active_energy_pulse_end()


#define phase_1_active_energy_pulse_start()         (P5OUT &= ~BIT0)
#define phase_1_active_energy_pulse_end()           (P5OUT |= BIT0)
#define phase_2_active_energy_pulse_start()         (P5OUT &= ~BIT1)
#define phase_2_active_energy_pulse_end()           (P5OUT |= BIT1)
#define phase_3_active_energy_pulse_start()         (P5OUT &= ~BIT2)
#define phase_3_active_energy_pulse_end()           (P5OUT |= BIT2)

#define LED4_on()         (P4OUT &= ~BIT7)
#define LED4_off()         (P4OUT |= BIT7)
#define LED5_on()         (P2OUT &= ~BIT7)
#define LED5_off()         (P2OUT |= BIT7)
#define LED6_on()         (P2OUT &= ~BIT4)
#define LED6_off()         (P2OUT |= BIT4)


/*! This switch selects support for measuring the total reactive energy. */
#define TOTAL_REACTIVE_ENERGY_SUPPORT

/*! This switch selects support for measuring the reactive energy on a phase by phase
    basis. This is only significant for poly-phase meters. */
#undef PER_PHASE_REACTIVE_ENERGY_SUPPORT

/*! This enables the generation of total energy pulses to an accuracy of 1/32768s, instead
    of 10/32768s, when building meters using the ADC12 ADC converter. This features uses channel
    2 of timer A, and the pulse LED/opto-coupler/etc. must be attached to the TA2 pin of the MCU.
    Because extra hardware resources are used, and most meters do not require this level of pulse
    position accuracy, this feature is made an option. */
#undef FINE_ENERGY_PULSE_TIMING_SUPPORT

/*! This switch enables use of the MSP430's internal temperature diode to
    measure the meter's temperature. */
#define TEMPERATURE_SUPPORT

/*! This switch selects support for the monitoring of a hall-effect magnetic sensor. This can be
    used to detect very high magnetic fields which might be used for tampering (i.e. fields strong
    enough to paralyse CTs, and other magnetic components in the meter's design). */

/*! When magnetic tampering is detected, the output of CTs is completely unreliable. The meter can only
    assume there is a very high load at unity power factor, and charge accordingly. This sets the current
    to be used in this calculation. It is specified in milliamps. */
#define MAGNETIC_INTERFERENCE_CURRENT               60000

/*! This selects real time clock support. This is implemented in software on
    the MSP430. */
#define RTC_SUPPORT

/*! Related definitions to get special action routines to be called at various
    intervals. User supplied callback functions must be provided. Note these
    callback routines are called from within the per second timer interrupt
    service routine. Don't do anything too complex within them. If a long
    activity is required, set a flag within a simple routine, and do the main
    work in the main non-interrupt loop. */
#undef PER_SECOND_ACTIVITY_SUPPORT
#undef PER_MINUTE_ACTIVITY_SUPPORT
#undef PER_HOUR_ACTIVITY_SUPPORT
#undef PER_DAY_ACTIVITY_SUPPORT
#undef PER_MONTH_ACTIVITY_SUPPORT
#undef PER_YEAR_ACTIVITY_SUPPORT

/*! Corrected RTC support enables temperature and basic error compensation for
    the MSP430's 32kHz crystal oscillator, so it makes for a higher quality RTC
    source, even using low accuracy (eg 20ppm) crystals. */
#define CORRECTED_RTC_SUPPORT

/*! This select support for a custom real time clock. The customer's routine
    will be called every second. */
#undef CUSTOM_RTC_SUPPORT
#if defined(__MSP430__)
#define custom_rtc()                                /**/
#define custom_rtc_set()                            /**/
#define custom_rtc_retrieve(x)                      /**/
#endif








//******************************************************************************
/*! The gain setting for the first current channel channel of the SD16,
    for devices using the SD16 sigma-delta ADC.
    This must be set to suit the shunt or CT in use. Typical values for a
    shunt are GAIN_16 (x16 gain) or GAIN_32 (x32 gain). Typical values for a
    CT are GAIN_1 (x1 gain) or GAIN_2 (x2 gain). */


      
   /* Gain  für I-Flex-Messsystem 
   Gain 1 für normale induktive Current-Transformer  */

   //#define CURRENT_PHASE_GAIN                          SD24GAIN_128
   //#define CURRENT_PHASE_GAIN                          SD24GAIN_32
   #define CURRENT_PHASE_GAIN                            EIGEN_CURRENT_PHASE_GAIN
//******************************************************************************







/*! The gain setting for the second current channel channel of the SD16,
    for devices using the SD16 sigma-delta ADC.
    This must be set to suit the shunt or CT in use. Typical values for a
    shunt are GAIN_16 (x16 gain) or GAIN_32 (x32 gain). Typical values for a
    CT are GAIN_1 (x1 gain) or GAIN_2 (x2 gain). */
#define CURRENT_NEUTRAL_GAIN                        SD24GAIN_1

/*! The gain setting for the voltage channel of the SD16, for devices using the
    SD16 sigma-delta ADC. This is usually GAIN_1 (i.e. x1 gain). */
#define VOLTAGE_GAIN                                SD24GAIN_1

/*! This switch select the use of current dependant phase correction, to allow
    the use of non-linear CTs. */
#define PHASE_CORRECTION_SUPPORT

/*! This switch enables dynmaic phase correction for non-linear CTs, or other sensors
    for which the phase changes with the current. */
#undef DYNAMIC_PHASE_CORRECTION_SUPPORT

/* This select support for a custom real time clock. The customer's routine
   will be called every second. */
#undef CUSTOM_RTC_SUPPORT

/* This switch enables power down to battery backup status on loss of line
   power. */
#undef POWER_DOWN_SUPPORT

/* These switches select a method of detecting when power is restored, if
   power down mode is supported. */
/* This method assumes the voltage waveform is being turned into simple
   digital pulses into an I/O pin. If this feature is used, POWER_UP_VOLTAGE_PULSE
   must define the way a voltage pulse is sensed. */
#undef POWER_UP_BY_VOLTAGE_PULSES
//#define power_up_voltage_pulse()                  (P1IN & BIT0)

/* This method assumes the pre-regulator power supply voltage is being
   monitored by Comparator A. This method is suitable for meters which
   support a live/neutral only mode, for tamper resistance. */

//#define POWER_UP_BY_SUPPLY_SENSING

/* This switch enables custom routines to be called as the meter enters and
   exits the power fail state. These might be used to save and restore things,
   using non-volatile memory. */
#undef CUSTOM_POWER_DOWN_SUPPORT

/* This switch enables support of an IR receiver and/or transmitter for
   programming and/or reading the meter. */

#define DLT645_SUPPORT

/*! This defines the speed of USART 0 or USCI 0 */

#define UART_PORT_1_SUPPORT                                 1
#define UART1_BAUD_RATE                                     2400

/*! This defines the speed of USART 1 or USCI 0 */
#define UART_PORT_0_SUPPORT                                 1
#define UART0_BAUD_RATE                                     9600
#define UART0_DLT645_SUPPORT                                1

#define INFRA_RED_38K_SUPPORT                               1
#define INFRA_RED_38K_FROM_TIMER_A                          1

/*! This definition specifies the port bit change required to enable the IR
    receiver, when one is used. */
#define enable_ir_receiver()                                /**/

/*! This definition specifies the port bit change required to disable the IR
    receiver, for power saving, when one is used. */
#define disable_ir_receiver()                               /**/




/*! This switch enables a rolling display of all the meter's measured
    parameters, for demonstration purposes. The parameters are displayed
    on a local attached LCD, which may be driven by an MSP430's internal
    LCD controller, or be part of a standard external module.
    Real meters will generally need to replace this with their own custom
    display routines. */
#define BASIC_LCD_SUPPORT


/*! This switch enables the DOG 102x6 user Display. */
#define DOG102X6_LCD_SUPPORT

/*! This switch enables the calling of a custom LCD display routine. This allows
    easy integration of custom code for specific meter designs. */
#undef CUSTOM_LCD_SUPPORT


/*! This is called to perform initialisation of a custom display */
#define custom_lcd_init()                                   /**/


/*! This is called to perform custom display processing when the meter goes to
    sleep. This is typically turning off the display, to conserve energy. */
#define custom_lcd_sleep_handler()                          (LCDsleep(), display_power_fail_message())

/*! This is called to perform custom display processing when the meter wakes up from
    the sleeping condition. */
#define custom_lcd_wakeup_handler()                         LCDawaken()

/*! This is called from within the main processing loop. This allows easy integration
    of custom code for specific meter designs. This routine is called often in normal
    operating mode. In limp mode it is called once each time the background activity
    informs the foreground activity there is a block of data to be processed. It will
    also be called when a key is pressed, or for each repeat if a key is in repeat
    mode. */
#define custom_mainloop_handler()                   /**/

/*! This is called in the main processing once every 2 seconds. */












/*! This switch enables support for some basic keypad handling, to work
    with the basic demonstration LCD display routines. It demonstrates an
    approach to keypad handling within an e-meter. */
#define BASIC_KEYPAD_SUPPORT

/*! These are used with the above switches to determine the port bits assigned
    to the keys. Only define the buttons which are actually used. */

#define sense_key_1_up()                            (!(P3IN & BIT1))

/*! These are used to select the features required of the keys. They are bits in
    the key_states variable. Only define the features being used. Usually for
    each key the "DOWN" option and either the "LONG_DOWN" or "REPEAT_DOWN" option
    should be specified. Do not select both the "LONG_DOWN" and "REPEAT_DOWN" for the
    same key. */
#define KEY_1_DOWN                                  0x01
#define KEY_1_REPEAT_DOWN                           0x02

/*! Normally the meter software only calculates the properly scaled values
    for voltage, current, etc. as these values are needed. This define
    enables additional global parameters, which are regularly updated with
    all the metrics gathered by the meter. This is generally less efficient,
    as it means calculating things more often than necessary. However, some
    may find this easier to use, so it is offered as a choice for the meter
    designer. */
#define PRECALCULATED_PARAMETER_SUPPORT

/*! A PWM generator can be used to add dithering to the signals, to expand the
    effective resolution of the ADC. */
#undef PWM_DITHERING_SUPPORT

/*! This switch select the use of current dependant phase correction, to allow
    the use of non-linear CTs. */
#define DYNAMIC_FIR_SUPPORT

/*! This switch selects where a backup battery's condition is monitored. */
#undef BATTERY_MONITOR_SUPPORT

/*! Target specific definitions for controlling the indicator LEDs */
#if defined(__MSP430__)
/*! This definition specifies the port bit change required to turn on the
    normal operation indicator LED, when one is used. If this is defined,
    clr_normal_indicator() should also be defined. The normal operation LED will be
    lit when the mains voltage is normal. In limp mode, or power down mode,
    it is switched off. */
#define set_normal_indicator()                      /**/

/*! This definition specifies the port bit change required to turn off the
    normal operation indicator LED, when one is used.  If this is defined,
    set_normal_indicator() should also be defined. */
#define clr_normal_indicator()                      /**/

/*! This definition specifies the port bit change required to turn on the
    earthed condition indicator LED, when one is used. If this is defined,
    clr_earthed_indicator() should also be defined. The earthed LED will be lit when
    there is a substantial imbalance in power (normal operating mode) or
    current (limp mode) between the live and neutral leads. If the neutral
    has the higher reading, this will also be used instead of the live reading. */
#define set_earthed_indicator()                     /**/

/*! This definition specifies the port bit change required to turn off the
    earthed condition indicator LED, when one is used. If this is defined,
    set_earthed_indicator() should also be defined. */
#define clr_earthed_indicator()                     /**/

/*! This is the number of successive measurement blocks which must agree the
    unbalanced status has changed before we accept it. */
#define PHASE_UNBALANCED_PERSISTENCE_CHECK          5

/*! These is the minimum current level (limp mode) and minimum power level (normal
    mode) at which we will make checks for the earthed condition. Very small
    readings lack the accuracy and resolution needed to make meaningfulF comparisons
    between live and neutral. */
#define PHASE_UNBALANCED_THRESHOLD_CURRENT          500
#define PHASE_UNBALANCED_THRESHOLD_POWER            2000

/*! This selects the detection of the reversed power condition. */
#define PHASE_REVERSED_DETECTION_SUPPORT

/*! This selects that the reversed power condition is to be treated as tampering. */
#define PHASE_REVERSED_IS_TAMPERING

/*! This definition specifies the port bit change required to turn on the
    reverse condition indicator LED, when one is used. If this is defined,
    clr_reverse_current_indicator() should also be defined. The reversed LED will be lit
    in normal operating mode, if the raw power reading is negative in the live
    lead (without neutral lead monitoring) or in either of the leads (with
    neutral lead monitoring). The power value used is simply minus the actual
    reading. This only functions in the normal operating mode. In limp mode we
    have no simple way to tell forward current flow from reverse. In limp mode
    the LED is switched off. Note that if the meter is used in applications
    means we are actively feeding power to the public grid this functionality
    is inappropriate. In those cases the negative power would generally be used
    to accumulate a separate "generated energy" reading, to complement the usual
    "consumed energy" reading. */
#define set_reverse_current_indicator()             /**/

/*! This definition specifies the port bit change required to turn on the
    reverse condition indicator LED, when one is used. If this is defined,
    set_reverse_current_indicator() should also be defined. */
#define clr_reverse_current_indicator()             /**/

/*! This is the number of successive measurement blocks which must agree the
    reversed current status has changed before we accept it. */
#define PHASE_REVERSED_PERSISTENCE_CHECK            5

/*! This is the minimum power level at which we will make checks for the reverse
    condition. Very small readings are not a reliable indicator, due to noise. */
#define PHASE_REVERSED_THRESHOLD_POWER              2000

#endif

/*! Some customers expect a meter to count pulses from the time the mains power
    is restored, looosing any fractional count of an LED pulse. This looses the
    power company a fraction of a pulse of revenue, and means the meter is not
    doing the best it can. In the end, we just do what the customer wants. */
#define LOSE_FRACTIONAL_PULSE_AT_POWER_ON

/*! Target specific definitions for the analogue I/O pins */
#define VOLTAGE_INPUT_1                             INCH_2
#define VOLTAGE_INPUT_2                             INCH_5
#define VOLTAGE_INPUT_3                             INCH_8
#define LIVE_LOW_CURRENT_INPUT_1                    INCH_1
#define LIVE_LOW_CURRENT_INPUT_2                    INCH_4
#define LIVE_LOW_CURRENT_INPUT_3                    INCH_7
#define LIVE_HIGH_CURRENT_INPUT_1                   INCH_0
#define LIVE_HIGH_CURRENT_INPUT_2                   INCH_3
#define LIVE_HIGH_CURRENT_INPUT_3                   INCH_6
#define NEUTRAL_CURRENT_INPUT                       INCH_9
#define AGND_INPUT                                  INCH_9


#define DEFAULT_TEMPERATURE_OFFSET                  (1615*8)
#define DEFAULT_TEMPERATURE_SCALING                 (704*2)

#define DEFAULT_ROOM_TEMPERATURE                    250

#define DEFAULT_V_RMS_SCALE_FACTOR_A                12380//9912//14506
#define DEFAULT_V_RMS_SCALE_FACTOR_B                12333//9886
#define DEFAULT_V_RMS_SCALE_FACTOR_C                12344 // 14503
#define DEFAULT_V_RMS_LIMP_SCALE_FACTOR_A           21280
#define DEFAULT_V_RMS_LIMP_SCALE_FACTOR             21280
#define DEFAULT_I_RMS_SCALE_FACTOR_A                11267//7425//5578
#define DEFAULT_I_RMS_SCALE_FACTOR_B                11284 // 5335 11171//5581
#define DEFAULT_I_RMS_SCALE_FACTOR_C                11268//  11184//5564
#define DEFAULT_I_RMS_LIMP_SCALE_FACTOR             4360
#define DEFAULT_I_RMS_LIMP_SCALE_FACTOR_A           4360

#if 1
#define DEFAULT_P_SCALE_FACTOR_A_LOW                4358//9196//10114 //        /* Low current, high gain, range. Test at 2A on 30A max meter */
//#define DEFAULT_P_SCALE_FACTOR_A_HIGH               14087       /* High current, low gain, range. Test at 7A on 30A max meter */
#define DEFAULT_P_SCALE_FACTOR_B_LOW               4348 // 13802//10180
//#define DEFAULT_P_SCALE_FACTOR_B_HIGH               14217
#define DEFAULT_P_SCALE_FACTOR_C_LOW               4346// 13826//10089
//#define DEFAULT_P_SCALE_FACTOR_C_HIGH               6343//14121
#else
#define DEFAULT_P_SCALE_FACTOR_A_LOW                1039        /* Low current, high gain, range. Test at 2A on 30A max meter */
#define DEFAULT_P_SCALE_FACTOR_A_HIGH               15663       /* High current, low gain, range. Test at 7A on 30A max meter */
#define DEFAULT_P_SCALE_FACTOR_B_LOW                1039
#define DEFAULT_P_SCALE_FACTOR_B_HIGH               15663
#define DEFAULT_P_SCALE_FACTOR_C_LOW                1039
#define DEFAULT_P_SCALE_FACTOR_C_HIGH               15663
#endif



#define DEFAULT_I_RMS_SCALE_FACTOR_NEUTRAL          4905
#define DEFAULT_I_RMS_LIMP_SCALE_FACTOR_NEUTRAL     4360
#define DEFAULT_P_SCALE_FACTOR_NEUTRAL              6210

#define DEFAULT_FREQUENCY_PHASE_FACTOR              500
#define DEFAULT_FREQUENCY_GAIN_FACTOR               0

#define DEFAULT_BASE_PHASE_A_CORRECTION_LOW         38           /* Low current, high gain, range. Test at 2A on 30A max meter. */
#define DEFAULT_BASE_PHASE_A_CORRECTION_HIGH        0            /* High current, low gain, range. Test at 7A on 30A max meter. */
#define DEFAULT_BASE_PHASE_B_CORRECTION_LOW        40            /* Value is phase angle in 1/256th of a sample increments. */
#define DEFAULT_BASE_PHASE_B_CORRECTION_HIGH        -3
#define DEFAULT_BASE_PHASE_C_CORRECTION_LOW         40
#define DEFAULT_BASE_PHASE_C_CORRECTION_HIGH        2

#define DEFAULT_V_PRESCALE_FACTOR                   0

#define DEFAULT_NEUTRAL_BASE_PHASE_CORRECTION       73

#define DEFAULT_PHASE_CORRECTION1                   0
#define DEFAULT_GAIN_CORRECTION1                    0
#define DEFAULT_PHASE_CORRECTION2                   0
#define DEFAULT_GAIN_CORRECTION2                    0



#define custom_set_consumption(x,y)                 /**/
#define custom_magnetic_sensor_test()               /**/

/*! This is called every ADC interrupt, after the main DSP work has finished.
    It can be used for things like custom keypad operations. It is important
    this is a very short routine, as it is called from the main ADC interrupt. */
#define custom_adc_interrupt()                      /**/

/*! This is called at intervals, to allow integration of RF protocols for AMR. */
#define custom_rf_sniffer()                         /**/

/*! This is called at intervals, to allow integration of RF protocols for AMR. */
#define custom_rf_exchange()                        /**/

#define custom_rtc()                                /* not used */
#define custom_rtc_set()                            /* not used */
#define custom_rtc_retrieve(x)                      /* not used */
#define custom_energy_pulse()                       /* not used */
#define custom_keypad_handler()                     /* not used */
#define custom_mainloop_handler()                   /* not used */
#define custom_power_fail_handler()                 /* not used */
#define custom_power_restore_handler()              /* not used */

//#include "lierda_lcd.h"

///* LCD display parameters */
//#define icon_desciptor(cell,bit)                    ((bit << 5) | cell)
//
//#define LCD_CELLS                                   20
//#define LCD_POS_BASE                                -1
//#define LCD_POS_STEP                                1
////typedef uint8_t lcd_cell_t;
//
//
//#define FIRST_ROW_START                             UPPER_NUMBER_FIRST_DIGIT
//#define FIRST_ROW_CHARS                             UPPER_NUMBER_DIGITS
//
//#define SECOND_ROW_START                            LOWER_NUMBER_FIRST_DIGIT
//#define SECOND_ROW_CHARS                            LOWER_NUMBER_DIGITS
//
//#define ICON_PHASE_A                                LCD_BOTTOM_PHASE_A_ICON
//#define ICON_PHASE_B                                LCD_BOTTOM_PHASE_B_ICON
//#define ICON_PHASE_C                                LCD_BOTTOM_PHASE_C_ICON
//
//#define ICON_H                                      LCD_UPPER_HOUR
//
//#define ICON_BATTERY                                LCD_BOTTOM_LOW_BATTERY
//
//#define FIRST_POSITION                              UPPER_NUMBER_FIRST_DIGIT
//
//#define INFO_POSITION                               SECOND_ROW_START
//#define INFO_CHARS                                  SECOND_ROW_CHARS
//
////#define ICON_TIME                                   LCD_TOP_TIME
//#define HOUR_POSITION                               (UPPER_NUMBER_FIRST_DIGIT + 0)
//#define ICON_TIME_COLON_1                           icon_loc(UPPER_NUMBER_FIRST_DIGIT + 2, 1)   /* Seg G of intervening digit */
//#define MINUTE_POSITION                             (UPPER_NUMBER_FIRST_DIGIT + 3)
//#define ICON_TIME_COLON_2                           icon_loc(UPPER_NUMBER_FIRST_DIGIT + 5, 1)   /* Seg G of intervening digit */
//#define SECONDS_POSITION                            (UPPER_NUMBER_FIRST_DIGIT + 6)
//
//#define YEAR_POSITION                               (LOWER_NUMBER_FIRST_DIGIT + 0)
//#define ICON_DATE_COLON_1                           icon_loc(LOWER_NUMBER_FIRST_DIGIT + 2, 1)   /* Seg G of intervening digit */
//#define MONTH_POSITION                              (LOWER_NUMBER_FIRST_DIGIT + 3)
//#define ICON_DATE_COLON_2                           icon_loc(LOWER_NUMBER_FIRST_DIGIT + 5, 1)   /* Seg G of intervening digit */
//#define DAY_POSITION                                (LOWER_NUMBER_FIRST_DIGIT + 6)
//
//#define ICON_kW                                     LCD_UPPER_KW
////#define ICON_ACTIVE_POWER                           LCD_UPPER_ACTIVE_POWER
//#define FIRST_ACTIVE_POWER_POSITION                 UPPER_NUMBER_FIRST_DIGIT
//#define ACTIVE_POWER_DIGITS                         UPPER_NUMBER_DIGITS
//#define ICON_ACTIVE_POWER_DECIMAL_POINT             LCD_UPPER_DP_4
//#define ACTIVE_POWER_RESOLUTION                     4
//
//#define ICON_kvar                                   LCD_LOWER_KVAR
////#define ICON_REACTIVE_POWER                         LCD_LOWER_REACTIVE_POWER
//#define FIRST_REACTIVE_POWER_POSITION               LOWER_NUMBER_FIRST_DIGIT
//#define REACTIVE_POWER_DIGITS                       LOWER_NUMBER_DIGITS
//#define ICON_REACTIVE_POWER_DECIMAL_POINT           LCD_LOWER_DP_4
//#define REACTIVE_POWER_RESOLUTION                   4
//
//#define ICON_kVA                                    LCD_UPPER_KW
////#define ICON_APPARENT_POWER
//#define FIRST_APPARENT_POWER_POSITION               UPPER_NUMBER_FIRST_DIGIT
//#define APPARENT_POWER_DIGITS                       UPPER_NUMBER_DIGITS
//#define ICON_APPARENT_POWER_DECIMAL_POINT           LCD_UPPER_DP_4
//#define APPARENT_POWER_RESOLUTION                   4
//
////#define ICON_ACTIVE_ENERGY                          LCD_UPPER_ACTIVE_POWER
//#define ICON_H_FOR_kW                               LCD_UPPER_HOUR
//#define FIRST_ACTIVE_ENERGY_POSITION                UPPER_NUMBER_FIRST_DIGIT
//#define ACTIVE_ENERGY_DIGITS                        UPPER_NUMBER_DIGITS
//#define ICON_ACTIVE_ENERGY_DECIMAL_POINT            LCD_UPPER_DP_6
//#define ACTIVE_ENERGY_RESOLUTION                    2
//
////#define ICON_REACTIVE_ENERGY                        LCD_LOWER_REACTIVE_POWER
//#define ICON_H_FOR_kvar                             LCD_LOWER_HOUR
//#define FIRST_REACTIVE_ENERGY_POSITION              LOWER_NUMBER_FIRST_DIGIT
//#define REACTIVE_ENERGY_DIGITS                      LOWER_NUMBER_DIGITS
//#define ICON_REACTIVE_ENERGY_DECIMAL_POINT          LCD_LOWER_DP_6
//#define REACTIVE_ENERGY_RESOLUTION                  2
//
////#define ICON_POWER_FACTOR
////#define ICON_COS_PHI
//#define FIRST_POWER_FACTOR_POSITION                 LOWER_NUMBER_FIRST_DIGIT
//#define POWER_FACTOR_DIGITS                         LOWER_NUMBER_DIGITS
//#define ICON_POWER_FACTOR_DECIMAL_POINT             LCD_LOWER_DP_4
//#define POWER_FACTOR_RESOLUTION                     4
//
////#define ICON_VOLTAGE                                LCD_UPPER_VOLTAGE
//#define ICON_V                                      LCD_UPPER_VOLTS
//#define FIRST_VOLTAGE_POSITION                      UPPER_NUMBER_FIRST_DIGIT
//#define VOLTAGE_DIGITS                              UPPER_NUMBER_DIGITS
//#define ICON_VOLTAGE_DECIMAL_POINT                  LCD_UPPER_DP_6
//#define VOLTAGE_RESOLUTION                          2
//
////#define ICON_CURRENT                                LCD_LOWER_CURRENT
//#define ICON_A                                      LCD_LOWER_AMPS
//#define FIRST_CURRENT_POSITION                      LOWER_NUMBER_FIRST_DIGIT
//#define CURRENT_DIGITS                              LOWER_NUMBER_DIGITS
//#define ICON_CURRENT_DECIMAL_POINT                  LCD_LOWER_DP_5
//#define CURRENT_RESOLUTION                          3
//
////#define ICON_TEMPERATURE
////#define ICON_CELCIUS
//#define FIRST_TEMPERATURE_POSITION                  UPPER_NUMBER_FIRST_DIGIT
//#define TEMPERATURE_DIGITS                          UPPER_NUMBER_DIGITS
//#define TEMPERATURE_RESOLUTION                      1
//#define ICON_TEMPERATURE_DECIMAL_POINT              LCD_UPPER_DP_7
//
////#define ICON_FREQUENCY
//#define ICON_HERTZ                                  LCD_LOWER_HERTZ
//#define FIRST_FREQUENCY_POSITION                    LOWER_NUMBER_FIRST_DIGIT
//#define FREQUENCY_DIGITS                            LOWER_NUMBER_DIGITS
//#define FREQUENCY_RESOLUTION                        2
//#define ICON_FREQUENCY_DECIMAL_POINT                LCD_LOWER_DP_6
//
//#define DISPLAY_TYPE_POSITION                       12
//
//#define TEXT_MESSAGE_LENGTH                         6
//
///* LCD display sequence table */
//#define DISPLAY_STEP_SEQUENCE \
//    DISPLAY_ITEM_SELECT_PHASE_1, \
//    DISPLAY_ITEM_ACTIVE_POWER, \
//    DISPLAY_ITEM_REACTIVE_POWER, \
//    DISPLAY_ITEM_SELECT_PHASE_1, \
//    DISPLAY_ITEM_VOLTAGE, \
//    DISPLAY_ITEM_CURRENT, \
//    DISPLAY_ITEM_SELECT_PHASE_1, \
//    DISPLAY_ITEM_MAINS_FREQUENCY, \
//    DISPLAY_ITEM_SELECT_PHASE_1, \
//    DISPLAY_ITEM_ACTIVE_ENERGY, \
//    DISPLAY_ITEM_REACTIVE_ENERGY, \
//    DISPLAY_ITEM_SELECT_PHASE_2, \
//    DISPLAY_ITEM_ACTIVE_POWER, \
//    DISPLAY_ITEM_REACTIVE_POWER, \
//    DISPLAY_ITEM_SELECT_PHASE_2, \
//    DISPLAY_ITEM_VOLTAGE, \
//    DISPLAY_ITEM_CURRENT, \
//    DISPLAY_ITEM_SELECT_PHASE_2, \
//    DISPLAY_ITEM_MAINS_FREQUENCY, \
//    DISPLAY_ITEM_SELECT_PHASE_2, \
//    DISPLAY_ITEM_ACTIVE_ENERGY, \
//    DISPLAY_ITEM_REACTIVE_ENERGY, \
//    DISPLAY_ITEM_SELECT_PHASE_3, \
//    DISPLAY_ITEM_ACTIVE_POWER, \
//    DISPLAY_ITEM_REACTIVE_POWER, \
//    DISPLAY_ITEM_SELECT_PHASE_3, \
//    DISPLAY_ITEM_VOLTAGE, \
//    DISPLAY_ITEM_CURRENT, \
//    DISPLAY_ITEM_SELECT_PHASE_3, \
//    DISPLAY_ITEM_MAINS_FREQUENCY, \
//    DISPLAY_ITEM_SELECT_PHASE_3, \
//    DISPLAY_ITEM_ACTIVE_ENERGY, \
//    DISPLAY_ITEM_REACTIVE_ENERGY, \
//    DISPLAY_ITEM_SELECT_TOTAL, \
//    DISPLAY_ITEM_ACTIVE_POWER, \
//    DISPLAY_ITEM_REACTIVE_POWER, \
//    DISPLAY_ITEM_SELECT_TOTAL, \
//    DISPLAY_ITEM_TIME, \
//    DISPLAY_ITEM_DATE, \
//    DISPLAY_ITEM_SELECT_RESTART
//
//#define custom_lcd_clear_periphery() \
//    LCDicon(LCD_1_1, FALSE); \
//    LCDicon(LCD_1_2, FALSE); \
//    LCDicon(LCD_1_3, FALSE); \
//    LCDicon(LCD_1_5, FALSE); \
//    LCDicon(LCD_1_6, FALSE); \
//    LCDicon(LCD_1_7, FALSE); \
//    LCDicon(LCD_1_8, FALSE); \
//    LCDicon(LCD_TOP_FORWARDS, FALSE); \
//    LCDicon(LCD_TOP_REVERSE, FALSE); \
//    LCDicon(LCD_UPPER_VOLTAGE, FALSE); \
//    LCDicon(LCD_11_6, FALSE); \
//    LCDicon(LCD_TOP_LAST_MONTH, FALSE); \
//    LCDicon(LCD_TOP_11_8, FALSE); \
//    LCDicon(LCD_20_1, FALSE); \
//    LCDicon(LCD_20_2, FALSE); \
//    LCDicon(LCD_20_3, FALSE); \
//    LCDicon(LCD_20_4, FALSE); \
//    LCDicon(LCD_TOP_TIME, FALSE); \
//    LCDicon(LCD_20_7, FALSE); \
//    LCDicon(LCD_20_8, FALSE); \
//    LCDicon(LCD_BOTTOM_QUADRANT_1, FALSE); \
//    LCDicon(LCD_BOTTOM_QUADRANT_2, FALSE); \
//    LCDicon(LCD_BOTTOM_QUADRANT_3, FALSE); \
//    LCDicon(LCD_BOTTOM_QUADRANT_4, FALSE); \
//    LCDicon(ICON_BATTERY, FALSE);
//
//#define custom_lcd_clear_line_1_tags() \
//    LCDicon(LCD_UPPER_HOUR, FALSE); \
//    LCDicon(LCD_UPPER_DP_2, FALSE); \
//    LCDicon(LCD_UPPER_KW, FALSE); \
//    LCDicon(LCD_UPPER_DP_4, FALSE); \
//    LCDicon(LCD_UPPER_DP_5, FALSE); \
//    LCDicon(LCD_UPPER_DP_6, FALSE); \
//    LCDicon(LCD_UPPER_DP_7, FALSE); \
//    LCDicon(LCD_UPPER_VOLTS, FALSE); \
//    LCDicon(LCD_UPPER_ACTIVE_POWER, FALSE);
//
//#define custom_lcd_clear_line_2_tags() \
//    LCDicon(LCD_LOWER_HERTZ, FALSE); \
//    LCDicon(LCD_LOWER_HOUR, FALSE); \
//    LCDicon(LCD_LOWER_DP_2, FALSE); \
//    LCDicon(LCD_LOWER_KVAR, FALSE); \
//    LCDicon(LCD_LOWER_DP_4, FALSE); \
//    LCDicon(LCD_LOWER_DP_5, FALSE); \
//    LCDicon(LCD_LOWER_DP_6, FALSE); \
//    LCDicon(LCD_LOWER_DP_7, FALSE); \
//    LCDicon(LCD_LOWER_AMPS, FALSE); \
//    LCDicon(LCD_LOWER_REACTIVE_POWER, FALSE); \
//    LCDicon(LCD_LOWER_CURRENT, FALSE);

#define USE_WATCHDOG

/*
    P1.0 = Taster 1
    P1.1 = Taster 2
    P1.2 =  ACLK
    P1.3 = Taster 3
    P1.4 = MCLK
    P1.5 = SMCLK
    P1.6 = Taster 4
    P1.7 = frei out
 */
#define P1OUT_INIT  0x00
#define P1DIR_INIT  (BIT7|BIT5|BIT4|BIT2)
#define P1REN_INIT  0x00
#define P1DS_INIT   0x00
#define P1SEL0_INIT  (BIT5|BIT4|BIT2)
#define P1IES_INIT  0x00
#define P1IE_INIT   0x00

/*
    P2.0 = EEPI2C.NFC_INT0 - noch nicht Aktiviert eingang mit internem Pullup belegt
    P2.1 = EEPI2C.NFC_INT1 - noch nicht Aktiviert eingang mit internem Pullup belegt
    P2.2 = RF.DataReady    - noch nicht Aktiviert eingang mit internem Pullup belegt - DataRady-IRQ neu ab 20.10.2015
    P2.3 = Display Beleuchtung on
    P2.4 = frei TP4 Ausgang
    P2.5 = frei TP5 Ausgang
    P2.6 = frei TP6 Ausgang
    P2.7 = VMon CB2 Eingang Leitung ziehen auf VMon, wegen Layoutfehler
 */
#define P2OUT_INIT   (BIT3|BIT2|BIT1|BIT0)
#define P2DIR_INIT   (BIT6|BIT5|BIT4|BIT3)
#define P2REN_INIT   (BIT2|BIT1|BIT0)
#define P2DS_INIT    0x00
#define P2SEL0_INIT  0x00
#define P2IES_INIT   0        // war auf 0
#define P2IE_INIT    BIT2

/*
    P3.0 = RS232 RX   -> USCIA0 Eingang
    P3.1 = RS232 TX   -> USCIA0 Ausgang
    P3.2 = LCD.CS0    -> Port Augang H
    P3.3 = LCD.CLK    -> USCIB1
    P3.4 = frei TP8   -> AUSGANG
    P3.5 = LCD.MOSI   -> USCIB1 Ausgang
    P3.6 = frei TP9   -> Ausgang
    P3.7 = EEPI2C.NFC_VCC -> Ausgang Port NFC- VCC erstmal L
 */
#define P3OUT_INIT   (BIT6|BIT5|BIT4|BIT3|BIT2|BIT1)
#define P3DIR_INIT   (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1)
#define P3REN_INIT   0x00
#define P3DS_INIT    0x00
#define P3SEL_INIT   (BIT1|BIT0)

/*
    P4.0 = RF.CS    -> Port Radio CS Ausgang
    P4.1 = RF.UART_RX  -> USCIA1 Radio RXD Eingang
    P4.2 = RF.UART_TX  -> USCIA1 Radio TXD Ausgang
    P4.3 = RF.SCLK  -> USCIA1 RADIO CLK  Ausgang
    P4.4 = EEPI2C.SCL -> erstmal Eingang
    P4.5 = EEPI2C.SDA -> erstmal Eingang
    P4.6 = EEPI2c.NFC.SCL -> USCIB0
    P4.7 = EEPI2c.NFC.SDA -> USCIB0
 */
#define P4OUT_INIT   (BIT0)
#define P4DIR_INIT   (BIT3|BIT2|BIT0)
#define P4REN_INIT   (BIT1)
#define P4DS_INIT     0x00
#define P4SEL_INIT   (BIT7|BIT6|BIT3|BIT2|BIT1)

/*
    P5.0 = frei TP10 Ausgang H
    P5.1 = frei TP11 Ausgang H
    P5.2 = frei TP12 Ausgang H
    P5.3 = frei TP13 Ausgang H
    P5.4 = frei TP14 Ausgang H
    P5.5 = RF.OnOff Ausgang H
    P5.6 = RF.RESET Ausgang H
    P5.7 = frei TP  Ausgang H (Version 1 VMON, falsch belegt)
*/
#define P5OUT_INIT   (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0)
#define P5DIR_INIT   (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0)
#define P5REN_INIT   0x00
#define P5DS_INIT    0x00
#define P5SEL_INIT   0x00

/*
    P6.0 = frei TP15    -> Ausgang H
    P6.1 = LCD.CD       -> Ausgang H
    P6.2 = LCD.RST      -> Ausgang H
    P6.3 = frei TP16    -> Ausgang H
    P6.4 = /Chg         -> Eingang
    P6.5 = /PGood       -> Eingang
    P6.6 = /ExtCharge   -> Eingang
    P6.7 = /ChargeEn    -> Ausgang H (L charge start)
*/

#define P6OUT_INIT   (BIT7|BIT3|BIT2|BIT1|BIT0)
#define P6DIR_INIT   (BIT7|BIT3|BIT2|BIT1|BIT0)
#define P6REN_INIT   0x00
#define P6DS_INIT    0x00
#define P6SEL_INIT   0x00

/*
    P7.0 = LED1         -> Ausgang H
    P7.1 = LED2         -> Ausgang H
    P7.2 = LED3         -> Ausgang H
    P7.3 = LED4         -> Ausgang H
    P7.4 = LED5         -> Ausgang H
    P7.5 = LED6         -> Ausgang H
    P7.6 = LED7         -> Ausgang H
    P7.7 = LED8         -> Ausgang H
 */
#define P7OUT_INIT   (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0)
#define P7DIR_INIT   (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0)
#define P7REN_INIT   0x00
#define P7DS_INIT    0x00
#define P7SEL_INIT   0x00


/*
    P8.0 = frei TP17            -> Ausgang H
    P8.1 = frei TP18            -> Ausgang RTCCLK for calibrat rtc
    P8.2 = nicht vorhanden      -> Ausgang H
    P8.3 = nicht vorhanden      -> Ausgang H
    P8.4 = nicht vorhanden      -> Ausgang H
    P8.5 = nicht vorhanden      -> Ausgang H
    P8.6 = nicht vorhanden      -> Ausgang H
    P8.7 = nicht vorhanden      -> Ausgang H
 */
#define P8OUT_INIT   (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0)
#define P8DIR_INIT   (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0)
#define P8REN_INIT   0x00
#define P8DS_INIT    0x00
#define P8SEL_INIT   (BIT1)

#define I_HISTORY_STEPS                             2

#define MCLK_DEF 16   /* 16 MHz */

// defines für Power- Control
#define CHARGE_OFF() P6OUT|=BIT7
#define CHARGE_ON() P6OUT &=~BIT7

#define PGOOD()  !(P6IN&BIT5)
#define CHARGE() !(P6IN&BIT4)


// Defines für LCD Backlight on/off
#define BACKLIGHT_ON() P2OUT |= BIT3
#define BACKLIGHT_OFF() P2OUT &= ~BIT3

// Defines für RF430 (NFC-Tag)
#define LED_ON(x)   P7OUT &= ~(x)
#define LED_OFF(x)  P7OUT |= x

#define RF430_INT0_VALUE      BIT0
#define RF430_INT1_VALUE      BIT1

#define RF430_PORT            P2IN
#define RF430_IFG             P2IFG
#define RF430_IE              P2IE
#define RF430_IES             P2IES
#define RF430_REN             P2REN

#define RF430_RF_FIELD_DEDECTED  !(RF430_PORT & RF430_INT0_VALUE)



