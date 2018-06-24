/*****< HARDWARE.c >**********************************************************/
/*      Copyright 2010 - 2012 Stonestreet One.                               */
/*      All Rights Reserved.                                                 */
/*                                                                           */
/*  HARDWARE - Hardware API for MSP430 Experimentor Board                    */
/*                                                                           */
/*  Author:  Tim Cook                                                        */
/*                                                                           */
/*** MODIFICATION HISTORY ****************************************************/
/*                                                                           */
/*   mm/dd/yy  F. Lastname    Description of Modification                    */
/*   --------  -----------    -----------------------------------------------*/
/*   07/07/10  Tim Cook       Initial creation.                              */
/*****************************************************************************/
#include <msp430.h>
#include <string.h>
#include "HAL.h"                                      /* MSP430 Hardware Abstraction API.         */

#include "Hardware.h"

#define BTPS_MSP430_DEFAULT_BAUD           115200L  /* Default UART Baud Rate*/
                                                    /* used in baud rate     */
                                                    /* given to this module  */
                                                    /* is invalid.           */

   /* The following are some defines that we will define to be 0 if they*/
   /* are not define in the device header.                              */

#ifndef XT1LFOFFG

   #define XT1LFOFFG   0

#endif

#ifndef XT1HFOFFG

   #define XT1HFOFFG   0

#endif

#ifndef XT2OFFG

   #define XT2OFFG     0

#endif

   /* Auxilary clock frequency                                          */
#define ACLK_FREQUENCY_HZ  ((unsigned int)32768)

   /* Macro to do a floating point divide.                              */
#define FLOAT_DIVIDE(x,y)  (((float)x)/((float)y))

   /* Macro to stop the OS Scheduler.                                   */
#define STOP_SCHEDULER()   (TA1CTL &= (~(MC_3)))

   /* Instruction to start the Scheduler Tick ISR.                      */
#define START_SCHEDULER()  (TA1CTL |= MC_1)


   /* Internal Variables to this Module (Remember that all variables    */
   /* declared static are initialized to 0 automatically by the         */
   /* compiler as part of standard C/C++).                              */

                              /* The following variable is used to hold */
                              /* a system tick count for the Bluetopia  */
                              /* No-OS stack.                           */
//static volatile unsigned long MSP430Ticks;

                              /* The following function is provided to  */
                              /* keep track of the number of peripherals*/
                              /* that have requested that the SMCLK stay*/
                              /* active. When this decrements to ZERO,  */
                              /* the clock will be turned off.          */
static volatile unsigned char ClockRequestedPeripherals;

   /* Local Function Prototypes.                                        */

//void ConfigureTimer(void);
//
///* This function is called to configure the System Timer, i.e TA1.   */
///* This timer is used for all system time scheduling.                */
//void ConfigureTimer(void)
//{
//   /* Ensure the timer is stopped.                                      */
//   TA1CTL = 0;
//
//   /* Run the timer off of the ACLK.                                    */
//   TA1CTL = TASSEL_1 | ID_0;
//
//   /* Clear everything to start with.                                   */
//   TA1CTL |= TACLR;
//
//   /* Set the compare match value according to the tick rate we want.   */
//   TA1CCR0 = ( ACLK_FREQUENCY_HZ / MSP430_TICK_RATE_HZ ) + 1;
//
//   /* Enable the interrupts.                                            */
//   TA1CCTL0 = CCIE;
//
//   /* Start up clean.                                                   */
//   TA1CTL |= TACLR;
//
//   /* Up mode.                                                          */
//   TA1CTL |= TASSEL_1 | MC_1 | ID_0;
//}
//
//
//   /* The following function is used to return the configured system    */
//   /* clock speed in MHz.                                               */
//unsigned long HAL_GetSystemSpeed(void)
//{
//   return SMCLK_FREQ;
//}
//
//   /* This function is called to get the system Tick Count.             */
//unsigned long HAL_GetTickCount(void)
//{
//   return(MSP430Ticks);
//}
//
//void HAL_Delay_ms( unsigned long ulTime_ms, unsigned short uiLPM_Bits )
//{
//   if (ulTime_ms<2) ulTime_ms = 2;                    // min. 1ms warten (genaue Zeit liegt zwischen 1ms und 2ms
//   unsigned long uiStartTime = MSP430Ticks;
//   while ( (MSP430Ticks - uiStartTime) < ulTime_ms )
//   {
//      //__bis_SR_register( uiLPM_Bits );
//      _NOP();
//   }
//}

//   /* The following function is called to enter LPM3 mode on the MSP    */
//   /* with the OS Timer Tick Disabled.                                  */
//void HAL_LowPowerMode(unsigned char DisableLED)
//{
//   /* Turn off Timer 1, which is used for the FreeRTOS and NoRTOS       */
//   /* timers. The timer runs off of the Auxilary Clock (ACLK) thus      */
//   /* without this the timer would continue to run, consuming power     */
//   /* and waking up the processor every 1 ms. Enter a critical section  */
//   /* to do so that we do get switched out by the OS in the middle of   */
//   /* stopping the OS Scheduler.                                        */
//   __disable_interrupt();
//
//   STOP_SCHEDULER();
//
//   /* Clear the count register.                                         */
//   TA1R = 0;
//
//   __enable_interrupt();
//
//   /* Turn off the LEDs if requested.                                   */
//   if(DisableLED)
//      LED_ALL_OFF;
//
//   /* Enter LPM3.                                                       */
//   LPM3;
//
//   /* Re-start the OS scheduler.                                        */
//   START_SCHEDULER();
//
//   /* Set the interrupt trigger bit to trigger an interrupt.            */
//   TA1CCTL0 |= 0x01;
//}
//
//   /* The following function is called to enable the SMCLK Peripheral   */
//   /* on the MSP430.                                                    */
//   /* * NOTE * This function should be called with interrupts disabled. */
//void HAL_EnableSMCLK(unsigned char Peripheral)
//{
//
//}
//
//   /* The following function is called to disable the SMCLK Peripheral  */
//   /* on the MSP430.                                                    */
//   /* * NOTE * This function should be called with interrupts disabled. */
//void HAL_DisableSMCLK(unsigned char Peripheral)
//{
//
//}
//
//   /* Timer A Get Tick Count Function for BTPSKRNL Timer A Interrupt.   */
//   /* Included for Non-OS builds                                        */
//#pragma vector=TIMER1_A0_VECTOR
//__interrupt void TIMER_INTERRUPT(void)
//{
//   ++MSP430Ticks;
//
//   /* Exit from LPM if necessary (this statement will have no effect if */
//   /* we are not currently in low power mode).                          */
//   LPM3_EXIT;
//}

