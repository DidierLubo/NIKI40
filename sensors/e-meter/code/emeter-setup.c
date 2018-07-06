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
//  File: emeter-setup.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: emeter-setup.c,v 1.44 2009/04/27 09:46:32 a0754793 Exp $
//
/*! \file emeter-structs.h */
//
//--------------------------------------------------------------------------
//
//  MSP430 setup routines for e-meters.
//
//  This software is appropriate for single phase and three phase e-meters
//  using a voltage sensor plus CT or shunt resistor current sensors, or
//  a combination of a CT plus a shunt.
//
#include <Hardware.h>
#include <stdint.h>
#include <string.h>
#include <io.h>
#include <emeter-toolkit.h>
#include "emeter-structs.h"
#include <HAL_Dogs102x6.h>

   static __inline__ void init_analog_front_end_normal(void)
      {

        int i;
        REFCTL0 = REFMSTR + REFVSEL_1 + REFON;         /* Enabale Reference = 2.0V */
        SD24BCTL1 &= ~SD24GRP0SC;

//        #if MCLK_DEF == 16
//    SD24BCTL0 = SD24SSEL__SMCLK  /* Clock is SMCLK */
//            | SD24PDIV_2    /* Divide by 8 => ADC clock: 2MHz */
//            | SD24DIV0
//            | SD24REFS;     /* Use internal reference */
//        #endif


    #if MCLK_DEF == 16
    SD24BCTL0 = SD24SSEL__SMCLK  /* Clock is SMCLK */
            | SD24PDIV_3    /* Divide by 8 => ADC clock: 2MHz */
            | SD24DIV0
            | SD24REFS;     /* Use internal reference */
        #endif



          SD16INCTL_CURRENT_1 = CURRENT_PHASE_GAIN;        /* Set gain for channel 1 */
          SD16CCTL_CURRENT_1 = SD24DF_1  | SD24SCS_4;
          SD16BOSR_CURRENT_1 = 256 - 1;
//           SD16BOSR_CURRENT_1 = 512 - 1;
          SD16PRE_CURRENT_1 = 0;

          SD16INCTL_CURRENT_2 =  CURRENT_PHASE_GAIN;        /* Set gain for channel 2 */
          SD16CCTL_CURRENT_2 = SD24DF_1  | SD24SCS_4;       /* Set oversampling ratio to 256 (default) */
          SD16BOSR_CURRENT_2 = 256 - 1;
//            SD16BOSR_CURRENT_2 = 512 - 1;
          SD16PRE_CURRENT_2 = 0;

          //
          SD16INCTL_CURRENT_3 =  CURRENT_PHASE_GAIN;        /* Set gain for channel 3 */
          SD16CCTL_CURRENT_3 = SD24DF_1  | SD24SCS_4;//| SD16IE;       /* Set oversampling ratio to 256 (default) */
          SD16BOSR_CURRENT_3 = 256 - 1;
//            SD16BOSR_CURRENT_3 = 512 - 1;
          SD16PRE_CURRENT_3 = 0;

          SD16INCTL_NEUTRAL =  CURRENT_NEUTRAL_GAIN;        /* Set gain for channel neutral */
          SD16CCTL_NEUTRAL = SD24DF_1  | SD24SCS_4;         /* Set oversampling ratio to 256 (default) */
         SD16BOSR_NEUTRAL = 256 - 1;
          SD16PRE_NEUTRAL = 0;

//            SD16BOSR_NEUTRAL = 512 - 1;


          /* Configure analog front-end channel 2 - Voltage */
          SD16INCTL_VOLTAGE_1 =  VOLTAGE_GAIN;
          SD16CCTL_VOLTAGE_1 = SD24DF_1 | SD24ALGN | SD24SCS_4;
          SD16BOSR_VOLTAGE_1 = 256 - 1;
//          SD16BOSR_VOLTAGE_1 = 512 - 1;
          SD16PRE_VOLTAGE_1 = DEFAULT_V_PRESCALE_FACTOR;

//             SD16PRE_VOLTAGE_1 = DEFAULT_V_PRESCALE_FACTOR_A;

          SD16INCTL_VOLTAGE_2 = VOLTAGE_GAIN;
          SD16CCTL_VOLTAGE_2 = SD24DF_1 | SD24ALGN | SD24SCS_4;
          SD16BOSR_VOLTAGE_2 = 256 - 1;
//          SD16PRE_VOLTAGE_2 = DEFAULT_V_PRESCALE_FACTOR_B;
//          SD16BOSR_VOLTAGE_2 = 512 - 1;
          SD16PRE_VOLTAGE_2 = DEFAULT_V_PRESCALE_FACTOR;

          SD16INCTL_VOLTAGE_3 =  VOLTAGE_GAIN;
          SD16CCTL_VOLTAGE_3 = SD24DF_1 | SD24ALGN | SD24SCS_4;
          SD16BOSR_VOLTAGE_3 = 256 - 1;
//          SD16BOSR_VOLTAGE_3 = 512 - 1;
          SD16PRE_VOLTAGE_3 = DEFAULT_V_PRESCALE_FACTOR;
//          SD16PRE_VOLTAGE_3 = DEFAULT_V_PRESCALE_FACTOR_C;



                    for (i = 0;  i < NUM_PHASES;  i++)
              chan[i].metrology.current.in_phase_correction[0].sd16_preloaded_offset = 0;

           SD24BIE=0x7F;
           SD24BCTL1 |= SD24GRP0SC;

      }
      static __inline__ void disable_analog_front_end(void)
      {
          int i;

          SD16INCTL_VOLTAGE_1 = 0;
          SD16CCTL_VOLTAGE_1 = 0;
          SD16PRE_VOLTAGE_1 = 0;

          SD16INCTL_VOLTAGE_2 = 0;
          SD16CCTL_VOLTAGE_2 = 0;
          SD16PRE_VOLTAGE_2 = 0;

          SD16INCTL_VOLTAGE_3 = 0;
          SD16CCTL_VOLTAGE_3 = 0;
          SD16PRE_VOLTAGE_3 = 0;

          SD16INCTL_CURRENT_1 = 0;
          SD16CCTL_CURRENT_1 = 0;
          SD16PRE_CURRENT_1 = 0;

          SD16INCTL_CURRENT_2 = 0;
          SD16CCTL_CURRENT_2 = 0;
          SD16PRE_CURRENT_2 = 0;

          SD16INCTL_CURRENT_3 = 0;
          SD16CCTL_CURRENT_3 = 0;
          SD16PRE_CURRENT_3 = 0;

          SD16INCTL_NEUTRAL = 0;
          SD16CCTL_NEUTRAL = 0;
          SD16PRE_NEUTRAL = 0;
          SD24BCTL0 = 0;
          #if defined(SD16CONF0_FUDGE)
            SD16CONF0 = SD16CONF0_FUDGE;
          #endif
          #if defined(SD16CONF1_FUDGE)
            SD16CONF1 = SD16CONF1_FUDGE;
          #endif
          for (i = 0;  i < NUM_PHASES;  i++)
              chan[i].metrology.current.in_phase_correction[0].sd16_preloaded_offset = 0;
      }


void system_setup(void)
{
    struct phase_parms_s *phase;
    struct phase_nv_parms_s const *phase_nv;
    int j;

    WDTCTL = (WDTCTL & 0xFF) | WDTPW | WDTHOLD;
          #if defined(__MSP430_HAS_UCS__)
#include "hal_UCS.h"
#include "hal_PMM.h"
    #if defined (__MSP430_HAS_AUX_SUPPLY__)

         AUX3CHCTL = AUXCHKEY | AUXCHC_1 | AUXCHV_1 | AUXCHEN;  // Enable Charger for AUX3 to enable RTC

    #endif
#if MCLK_DEF > 8
    SetVCore(3);      // Set VCore to level 0
#endif
    SetVCore(3);
    LFXT_Start(XT1DRIVE_3);
    Init_FLL_Settle(MCLK_DEF*8388608/8/1000, MCLK_DEF*32768*32/32768);
    /* There seems no benefit in waiting for the FLL to settle at this point. */
    #endif

    #if defined (__MSP430_HAS_AUX_SUPPLY__)
        PMMCTL0_H = PMMPW_H;
        SVSMHCTL|=SVSMHRRL_4;

        AUXCTL0 = AUXKEY;
        AUXCTL1 |= AUX2MD;
        AUXCTL2 |=AUX0LVL_6+AUX1LVL_5;
        PMMCTL0_H = 0;
    #endif

    #if defined(__MSP430_HAS_RTC_C__) || defined(__MSP430_HAS_RTC_CE__)
#ifdef LOCKBAK
   while(BAKCTL & LOCKBAK){                // unlock Backup Subsystem
      BAKCTL &= ~LOCKBAK;
    }
#endif


    /* Basic timer setup */
    /* Set ticker to 32768/(256*256) */
   RTCPS0CTL = RT0IP_7;                    //  / 256
   RTCPS1CTL = RT1IP_6 + RT1PSIE;          //  /128 / 256 = / 32768 -> sec interval

    /* We want a real watchdog function, but it doesn't have to be fast. */
    /* Use the longest timer - 1s */
        #if defined(USE_WATCHDOG)
    kick_watchdog();    /* Set the watchdog timer to exactly 1s */
        #else
    WDTCTL = (WDTCTL & 0xFF) | WDTPW | WDTHOLD;
        #endif
    #else
    #if defined(__MSP430_HAS_BT__)  ||  defined(__MSP430_HAS_BT_RTC__)
    /* Basic timer setup */
    /* Set ticker to 32768/(256*256) */
        #if defined(__MSP430_HAS_BT__)
    BTCTL = BT_fLCD_DIV64 | BT_fCLK2_DIV128 | BT_fCLK2_ACLK_DIV256;
        #else
    BTCTL = BT_fCLK2_DIV128 | BT_fCLK2_ACLK_DIV256;
        #endif
    /* Enable the 1 second counter interrupt */
    IE2 |= BTIE;

    /* We want a real watchdog function, but it doesn't have to be fast. */
    /* Use the longest timer - 1s */
        #if defined(USE_WATCHDOG)
    kick_watchdog();    /* Set the watchdog timer to exactly 1s */
        #else
    WDTCTL = (WDTCTL & 0xFF) | WDTPW | WDTHOLD;
        #endif
    #else
    IE1 |= WDTIE;               /* Enable the WDT interrupt */
    #endif
    #endif



    //* Initalize all Ports
    PMAPKEYID = PMAPKEY;
    // UART Abgleichschnittselle
    P3MAP0 =    PM_UCA0RXD;     //UCA0 UART RXD
    P3MAP1 =    PM_UCA0TXD;     //UCA0 UART TXD
    // SPI LCD- Displag DOGS102x6
    P3MAP3 =    PM_UCB1CLK;     //UCB1 CLK
    P3MAP5  =   PM_UCB1SIMO;    //UCB1 AIMO
    // UART Radio LWM2M Client
    P4MAP1 = PM_UCA3RXD;       //UCA3 UART RXD
    P4MAP2 = PM_UCA3TXD;       //UCA3 UART TXD
    // I2C NFC
    P4MAP6 = PM_UCB0SCL;        // UCB0 SCL
    P4MAP7 = PM_UCB0SDA;        // UCBO SDA

    PMAPKEYID = 0x00;


    P1OUT = P1OUT_INIT;
    P1DIR = P1DIR_INIT;
    P1REN = P1REN_INIT;
    P1DS = P1DS_INIT;
    P1SEL0 = P1SEL0_INIT;
    P1IES = P1IES_INIT;
    P1IE = P1IE_INIT;

    P2OUT = P2OUT_INIT;
    P2DIR = P2DIR_INIT;
    P2REN = P2REN_INIT;
    P2DS = P2DS_INIT;
    P2SEL0 = P2SEL0_INIT;
    P2IES = P2IES_INIT;
    P2IE = P2IE_INIT;

    P3OUT = P3OUT_INIT;
    P3DIR = P3DIR_INIT;
    P3REN = P3REN_INIT;
    P3SEL0 = P3SEL_INIT;
    P3DS  = P3DS_INIT;


    P4OUT  = P4OUT_INIT;
    P4DIR  = P4DIR_INIT;
    P4SEL0 = P4SEL_INIT;
    P4DS   = P4DS_INIT;

    P5OUT = P5OUT_INIT;
    P5DIR = P5DIR_INIT;
    P5SEL0 = P5SEL_INIT;
    P5DS   = P5DS_INIT;


    P6OUT = P6OUT_INIT;
    P6DIR = P6DIR_INIT;
    P6SEL0 = P6SEL_INIT;
    P6DS   = P6DS_INIT;

    P7OUT = P7OUT_INIT;
    P7DIR = P7DIR_INIT;
    P7SEL0 = P7SEL_INIT;
    P7DS   = P7DS_INIT;

    P8OUT = P8OUT_INIT;
    P8DIR = P8DIR_INIT;
    P8SEL0 = P8SEL_INIT;
    P8DS   = P8DS_INIT;

//**********************


    #if defined(IO_EXPANDER_SUPPORT)
      set_io_expander(0, 0);
    #endif

//*****************************************************************************




 // *** Comperator B on CB2 Port 2.7
    CBCTL0 = CBIPEN|CBIPSEL_2;
    CBCTL1 = CBON|CBPWRMD_2|CBF;
    CBCTL2 = CBREFACC|CBREFL_0|CBREF1_17|CBRS_1|CBREF0_18|CBRSEL;               // On Voltage 3,56V; Off Voltage 3,375V
    CBCTL3 = CBPD2;

    samples_per_second = SAMPLES_PER_10_SECONDS/10;
    disable_analog_front_end();





//#if defined(POWER_UP_BY_SUPPLY_SENSING)
//        /* Set up comparator A to monitor a drooping voltage within the
//           e-meter's main power supply. This is an early warning of power
//           fail, so we can get to low power mode before we start burning the
//           battery. */
//        CACTL1 = CAREF_1;
//        CACTL2 = P2CA1 | CAF;
//        P1SEL |= BIT7;
//        CAPD |= BIT7;
//    #endif





    #if defined(IEC1107_SUPPORT)  ||  defined(IEC62056_21_SUPPORT)  ||  defined(SERIAL_CALIBRATION_SUPPORT)  ||  defined(SERIAL_CALIBRATION_REF_SUPPORT)
        #if defined(__MSP430_HAS_EUSCI_A0__)  &&  defined(UART0_BAUD_RATE)
    /* Configure USCI0 UART */

    UCA0CTL0 = 0;                       /* 8-bit character */
            #if UART0_BAUD_RATE == 9600
    UCA0CTL1 |= UCSSEL__ACLK;                     /* UCLK = ACLK */
    UCA0BR1 = 0x0;
    UCA0BR0 = 0x3;
#ifdef _ZEBU_
    UCA0MCTLW_H = 0x25;
#else
    UCA0MCTLW_H = 0x92;
#endif
            #elif UART0_BAUD_RATE == 4800
    UCA0CTL1 |= UCSSEL__ACLK;                     /* UCLK = ACLK */
            #elif UART0_BAUD_RATE == 2400
    UCA0CTL1 |= UCSSEL__ACLK;                     /* UCLK = ACLK */
    UCA0BR1 = 0x0;
    UCA0BR0 = 13;
#ifdef _ZEBU_
    UCA0MCTLW_H = 0x25;
#else
    UCA0MCTLW_H = 0xB6;
#endif
            #else
    UCA0CTL1 |= UCSSEL__ACLK;                     /* UCLK = ACLK */
            #endif

    UCA0CTL1 &= ~UCSWRST;
    UCA0IE |= UCRXIE;
        #endif



#endif









  #if defined(IHD430_SUPPORT)
    UCA3CTL1 |= UCSWRST;
    UCA3CTL1 |= UCSSEL_2;                     // SMCLK
    UCA3BRW = 9;                           // 16 MHz 115200
    //UCA3BR1 = 0x00;                         //16 8MHz 115200
    UCA3MCTLW = 0xB511;
    P4SEL0 |= BIT1+BIT2;
    UCA3CTL1 &= ~UCSWRST;
  #endif

    meter_status &= ~(STATUS_REVERSED | STATUS_EARTHED | STATUS_PHASE_VOLTAGE_OK);
    clr_normal_indicator();
    clr_earthed_indicator();
    clr_reverse_current_indicator();
    #if defined(total_active_energy_pulse_end)
        total_active_energy_pulse_end();
    #endif
    #if defined(total_reactive_energy_pulse_end)
        total_reactive_energy_pulse_end();
    #endif
    #if defined(PER_PHASE_ACTIVE_ENERGY_SUPPORT)
            phase_1_active_energy_pulse_end();
            phase_2_active_energy_pulse_end();
            phase_3_active_energy_pulse_end();
    #endif

    phase = chan;
    phase_nv = nv_parms.seg_a.s.chan;
    for (j = 0;  j < NUM_PHASES;  j++)
    {

      #if defined(NEUTRAL_MONITOR_SUPPORT)

        //neutral.metrology.I_dc_estimate[0] = nv_parms.seg_a.s.neutral.initial_dc_estimate;
        neutral.metrology.I_endstops = ENDSTOP_HITS_FOR_OVERLOAD;
    #endif
        dc_filter_current_init(phase->metrology.current.I_dc_estimate[0], phase_nv->current.initial_dc_estimate);
        dc_filter_voltage_init(phase->metrology.V_dc_estimate, phase_nv->initial_v_dc_estimate);
      /* Prime the DC estimates for quick settling */
        //phase->metrology.current.I_dc_estimate[0] = phase_nv->current.initial_dc_estimate;
        phase->metrology.current.I_endstops = ENDSTOP_HITS_FOR_OVERLOAD;
        //phase->metrology.V_dc_estimate = phase_nv->initial_v_dc_estimate;
        phase->metrology.V_endstops = ENDSTOP_HITS_FOR_OVERLOAD;
        #if defined(MAINS_FREQUENCY_SUPPORT)
           phase->metrology.mains_period = ((SAMPLES_PER_10_SECONDS*6554)/MAINS_NOMINAL_FREQUENCY) << 8;
        #endif
        phase++;
        phase_nv++;
    }

    #if defined(NEUTRAL_MONITOR_SUPPORT)

        //neutral.metrology.I_dc_estimate[0] = nv_parms.seg_a.s.neutral.initial_dc_estimate;
        neutral.metrology.I_endstops = ENDSTOP_HITS_FOR_OVERLOAD;
    #endif

    #if defined(TEMPERATURE_SUPPORT)
      temperature = 0;
    #endif


     RTC_CE_Init();                                                             // Initialisation of real time clock




    custom_initialisation();
    _EINT();
    #if defined(POWER_DOWN_SUPPORT)
    /* Now go to lower power mode, until we know we should do otherwise */
        switch_to_powerfail_mode();
    #else
       #if defined(__MSP430_HAS_SVS__)
          /* Before we go to high speed we need to make sure the supply voltage is
             adequate. If there is an SVS we can use that. There should be no wait
             at this point, since we should only have been woken up if the supply
             is healthy. However, it seems better to be cautious. */
          SVSCTL |= (SVSON | 0x60);
          /* Wait for adequate voltage to run at full speed */
          while ((SVSCTL & SVSOP))
              /* dummy loop */;
          /* The voltage should now be OK to run the CPU at full speed. Now it should
             be OK to use the SVS as a reset source. */
          SVSCTL |= PORON;
        #endif
        #if defined(__MSP430_HAS_FLLPLUS__)  ||  defined(__MSP430_HAS_FLLPLUS_SMALL__)
           /* Speed up the clock to 8.388608MHz */
          SCFI0 = SCFI0_HIGH;
          SCFQCTL = SCFQCTL_HIGH;
          /* There seems no benefit in waiting for the FLL to settle at this point. */
        #endif
        kick_watchdog();
        switch_to_normal_mode();
    #endif
}

void switch_to_normal_mode(void)
{
    /* Switch to full speed, full power mode */
    meter_status |= STATUS_PHASE_VOLTAGE_OK;
    set_normal_indicator();
    #if defined(__MSP430_HAS_TA3__)
        /* Disable the TIMER_A0 interrupt */
        TACTL = 0;
        TACCTL0 = 0;
    #endif
    _DINT();
    init_analog_front_end_normal();
    samples_per_second = SAMPLES_PER_10_SECONDS/10;
    _EINT();
    operating_mode = OPERATING_MODE_NORMAL;
}

#if defined(CORRECTED_RTC_SUPPORT)  &&  defined(__MSP430_HAS_TA3__)
int32_t assess_rtc_speed(void)
{
    int32_t period;
    uint16_t this_capture;
    uint16_t last_capture;
    uint16_t step;
    int32_t counter;
    int limit;

    /* The fast clock should be an exact multiple of the crystal clock, once the FLL has
        settled. If we capture many cycles of an accurate external 32768Hz clock, using
        timer A (or B), we can measure the speed difference between the MSP430's crystal
        and the external clock in a reasonable time. */
    /* The SM clock should be running at 244*32768Hz at this time. */
    _DINT();
    /* Change timer A to running fast, and sampling the external 32768Hz reference. */
    P2SEL |= BIT0;
    TACCR0 = 0xFFFF;
    TACCTL0 = CAP | CCIS_0 | CM_1;
    TACCTL2 = CAP | CCIS_0 | CM_1 | SCS;
    TACTL = TACLR | MC_2 | TASSEL_2;    /* start TIMER_A up mode, SMCLK as input clock */
    period = 0;
    last_capture = TACCR2;
    limit = -1;
    TACCTL2 &= ~CCIFG;
    for (counter = 0;  counter < 32768*5 + 1;  counter++)
    {
        limit = 1000;
        while (!(TACCTL2 & CCIFG))
        {
            if (--limit <= 0)
                break;
        }
        if (limit <= 0)
            break;
        TACCTL2 &= ~CCIFG;
        this_capture = TACCR2;
        step = this_capture - last_capture;
        last_capture = this_capture;
        /* Skip the first sample, as it will be meaningless */
        if (counter)
        {
    #if 0
            if (step < (244 - 5)  ||  step > (244 + 5))
            {
                limit = -2;
                break;
            }
    #endif
            period += step;
        }
        kick_watchdog();
    }
    if (limit <= 0)
        period = limit;
    TACTL = TACLR | MC_1 | TASSEL_1;
    TACCTL0 = CCIE;
    P2SEL &= ~BIT0;
    _EINT();
    return  period;
}
#endif

int align_hardware_with_calibration_data(void)
{
    int ch;
    static struct phase_parms_s *phase;
    static struct phase_nv_parms_s const *phase_nv;
    disable_analog_front_end();
    init_analog_front_end_normal();
    for (ch = 0;  ch < NUM_PHASES;  ch++)
    {
        phase = &chan[ch];
        phase_nv = &nv_parms.seg_a.s.chan[ch];
        set_sd16_phase_correction(&phase->metrology.current.in_phase_correction[0], ch, phase_nv->current.phase_correction[0]);

    }
    return 0;
}
