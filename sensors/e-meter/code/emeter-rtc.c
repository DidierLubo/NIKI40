
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
//#include <io.h>
#include <emeter-toolkit.h>
#include "emeter-structs.h"
#include "emeter-rtc.h"


#if !defined(NULL)
  #define NULL    (void *) 0
#endif

#if defined(CORRECTED_RTC_SUPPORT)
  int32_t rtc_correction;
#endif

/* We need a small seconds counter, so we can do things like a display update every 2 seconds. */
uint8_t seconds;

rtc_t gstrRTC_Time; // Global time structur
















//#if (defined(RTC_SUPPORT)  ||  defined(CUSTOM_RTC_SUPPORT))  &&  defined(CORRECTED_RTC_SUPPORT)
//void correct_rtc(void)
//{
//    int32_t temp;
//
//    /* Correct the RTC to allow for basic error in the crystal, and
//       temperature dependant changes. This is called every two seconds,
//       so it must accumulate two seconds worth of error at the current
//       temperature. */
//    if (nv_parms.seg_a.s.temperature_offset)
//    {
//        temp = temperature - nv_parms.seg_a.s.temperature_offset;
//        temp *= nv_parms.seg_a.s.temperature_scaling;
//        temp >>= 16;
//
//        /* The temperature is now in degrees C. */
//        /* Subtract the centre point of the crystal curve. */
//        temp -= 25;
//        /* Do the parabolic curve calculation, to find the current ppm of
//           error due to temperature, and then the scaled RTC correction
//           value for 2 seconds at this temperature. */
//        temp = temp*temp*(2589L*4295L >> 5);
//        temp >>= 11;
//        temp = -temp;
//    }
//    else
//    {
//        temp = 0;
//    }
//    /* Allow for the basic manufacturing tolerance error of the crystal, found
//       at calibration time. */
//    temp += nv_parms.seg_a.s.crystal_base_correction;
//    if (rtc_correction >= 0)
//    {
//        rtc_correction += temp;
//        if (rtc_correction < 0)
//        {
//            rtc_correction -= 0x80000000;
//            /* We need to add an extra second to the RTC */
//    #if defined(CUSTOM_RTC_SUPPORT)
//            custom_rtc();
//    #else
//            //rtc_bumper();
//    #endif
//        }
//    }
//    else
//    {
//        rtc_correction += temp;
//        if (rtc_correction >= 0)
//        {
//            rtc_correction += 0x80000000;
//            /* We need to drop a second from the RTC */
//            meter_status |= SKIP_A_SECOND;
//        }
//    }
//}
//#endif
//// *********************************************************************

/*******************************************************************************
* RTC_CE Init 
*******************************************************************************/
void RTC_CE_Init(void)
{
  RTCCTL0_H = RTCKEY_H;                                   // Unlock RTC   
  if (RTCCTL0 & RTCOFIFG)                                 // init if RTCOFIFG is set, means oszillator starts new
  {

    RTCCTL13 = RTCHOLD+RTCMODE+RTCTEV_0;                   // Init RTC
    RTCCTL0_L &= ~RTCOFIFG_L;                             // Clear Flag
    gstrRTC_Time.second       =  00;
    gstrRTC_Time.minute       =  00;
    gstrRTC_Time.hour         =  00;
    gstrRTC_Time.dow          =  00;
    gstrRTC_Time.day          =  00;
    gstrRTC_Time.month        =  01;
    gstrRTC_Time.year         =  2015;   
    
    RTCOCAL = nv_parms.seg_a.s.crystal_base_correction;
    RTCTCMP = nv_parms.seg_a.s.crystal_base_correction;
    
    RTCCTL13 &= ~RTCHOLD;                                       // Enable RTC
  }
  else
  {
    RTCCTL13 = RTCMODE+RTCTEV_0|RTCCALF_3;                      // Init RTC
    RTCOCAL = nv_parms.seg_a.s.crystal_base_correction;
    RTCTCMP = nv_parms.seg_a.s.crystal_tco_correction;
  }
  RTCCTL0_H = 0;                                                // LOCK RTC
// *****************************************************************************
}

/*******************************************************************************
* RTC_CE get Time return pointer to global struct
*******************************************************************************/
__monitor rtc_t *RTC_CE_Get_Time()
{
  return &gstrRTC_Time;
}


/*******************************************************************************
* RTC_CE set Time return                                                             *
*******************************************************************************/
void RTC_CE_Set_Time(rtc_t *lptrNewTime)
{  
  memcpy((uint8_t*)&gstrRTC_Time,(uint8_t*)lptrNewTime, sizeof (rtc_t));        // Copy time to global structure
  RTCCTL0_H = RTCKEY_H;                                                         // Unlock RTC   
  memcpy((uint8_t*)&RTCTIM0,(uint8_t*)&gstrRTC_Time, sizeof (rtc_t)-2);         // Copy time to RTC modul without year
  RTCYEAR = lptrNewTime->year;                                                  // Set year only word access possible
  RTCCTL0_H = 0x00;                                                             // Lock RTC   
}






/*******************************************************************************
* RTC_CE ISR                                                                   *
*******************************************************************************/

#pragma vector=RTC_VECTOR
__interrupt void RTC_C_ISR(void)
{
   RTCPS1CTL &= ~RT1PSIFG;                                                      //  clear the flash   
   kick_watchdog();
   
// In interrupt we read the clock synchron it is not necessary 
//    do{  
//    _NOP();
//    }while(!(RTCCTL0_L&RTCRDYIFG));                                             // Wait for rtc data changing
     if (RTCCTL0_L & RTCRDYIFG)                                                 // Copy tim value into the global struct
     {
       memcpy((uint8_t*)&gstrRTC_Time,(uint8_t*)&RTCTIM0, sizeof (rtc_t)-2);    // Copy RTC values to globel structure without year
       gstrRTC_Time.year = RTCYEAR;                                             // copy year in global structure
     };

        
    
   
   
   if (++seconds & 1)                                                          /* 2 second ticker */
   meter_status |= TICKER;                                                  /* Kick every 2 seconds */
}
//#endif


