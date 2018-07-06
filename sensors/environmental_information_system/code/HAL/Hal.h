/*****< HAL.h >****************************************************************/
/*      Copyright 2010 - 2012 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  HAL - Hardware Abstraction functions for Stellaris LM3S9B96 Board         */
/*                                                                            */
/*  Author:  Tim Thomas                                                       */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   07/05/11  Tim Thomas     Initial creation.                               */
/******************************************************************************/
#ifndef __HAL_H__
#define __HAL_H__
#include <msp430.h>

   /* The following define the valid Peripheral values that may be      */
   /* passed into HAL_EnableSMCLK() and HAL_DisableSMCLK().             */
#define HAL_PERIPHERAL_DEBUG_UART                        0x01
#define HAL_PERIPHERAL_BLUETOOTH_UART                    0x02

   /* The following function is used to return the configured system    */
   /* clock speed in MHz.                                               */
unsigned long HAL_GetSystemSpeed(void);

   /* This function is called to get the system Tick Count.             */
unsigned long HAL_GetTickCount(void);


   /* The following function is called to enable the SMCLK Peripheral   */
   /* on the MSP430.                                                    */
   /* * NOTE * This function should be called with interrupts disabled. */
void HAL_EnableSMCLK(unsigned char Peripheral);

   /* The following function is called to disable the SMCLK Peripheral  */
   /* on the MSP430.                                                    */
   /* * NOTE * This function should be called with interrupts disabled. */
void HAL_DisableSMCLK(unsigned char Peripheral);

   /* The following function is called to enter LPM3 mode on the MSP    */
   /* with the OS Timer Tick Disabled.                                  */
void HAL_LowPowerMode(unsigned char DisableLED);

void ConfigureTimer(void);

//void HAL_Delay_ms( unsigned long ulTime_ms, unsigned short uiLPM_Bits );
#endif

