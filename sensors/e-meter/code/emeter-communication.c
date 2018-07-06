/*
        Copyright 2016-2018 NIKI 4.0 project team
        NIKI 4.0 was financed by the Baden-Württemberg Stiftung gGmbH (www.bwstiftung.de).
        Project partners are FZI Forschungszentrum Informatik am Karlsruher
        Institut für Technologie (www.fzi.de), Hahn-Schickard-Gesellschaft
        für angewandte Forschung e.V. (www.hahn-schickard.de) and
        Hochschule Offenburg (www.hs-offenburg.de).
        This file was developed by Hahn-Schickard.
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
//  File: emeter-communications.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: emeter-communication.c,v 1.18 2009/04/27 09:05:25 a0754793 Exp $
//
/*! \file emeter-structs.h */
//
//--------------------------------------------------------------------------
//
#include <stdint.h>
#include <io.h>
#include <emeter-toolkit.h>
#include "emeter-structs.h"
#if defined(IEC62056_21_SUPPORT)
  #include "iec62056-21.h"
#endif

#if !defined(NULL)
#define NULL    (void *) 0
#endif

//enum
//{
//    MEASURES_ACTIVE_POWER                       = 0x01,
//    MEASURES_TRIGONOMETRIC_REACTIVE_POWER       = 0x02,
//    MEASURES_APPARENT_POWER                     = 0x04,
//    MEASURES_VRMS                               = 0x08,
//    MEASURES_IRMS                               = 0x10,
//    MEASURES_POWER_FACTOR                       = 0x20,
//    MEASURES_MAINS_FREQUENCY                    = 0x40,
//    MEASURES_QUADRATURE_REACTIVE_POWER          = 0x80
//};
//
//enum host_commands_e
//{
//    HOST_CMD_GET_METER_CONFIGURATION            = 0x56,
//    HOST_CMD_SET_METER_CONSUMPTION              = 0x57,
//    HOST_CMD_SET_RTC                            = 0x58,
//    HOST_CMD_GET_RTC                            = 0x59,
//    HOST_CMD_SET_PASSWORD                       = 0x60,
//    HOST_CMD_GET_READINGS_PHASE_1               = 0x61,
//    HOST_CMD_GET_READINGS_PHASE_2               = 0x62,
//    HOST_CMD_GET_READINGS_PHASE_3               = 0x63,
//    HOST_CMD_GET_READINGS_NEUTRAL               = 0x64,
//    HOST_CMD_ERASE_FLASH_SEGMENT                = 0x70,
//    HOST_CMD_SET_FLASH_POINTER                  = 0x71,
//    HOST_CMD_FLASH_DOWNLOAD                     = 0x72,
//    HOST_CMD_FLASH_UPLOAD                       = 0x73,
//    HOST_CMD_ZAP_MEMORY_AREA                    = 0x74,
//    HOST_CMD_SUMCHECK_MEMORY                    = 0x75,
//    HOST_CMD_GET_RAW_ACTIVE_POWER_PHASE_1       = 0x91,
//    HOST_CMD_GET_RAW_ACTIVE_POWER_PHASE_2       = 0x92,
//    HOST_CMD_GET_RAW_ACTIVE_POWER_PHASE_3       = 0x93,
//    HOST_CMD_GET_RAW_REACTIVE_POWER_PHASE_1     = 0x95,
//    HOST_CMD_GET_RAW_REACTIVE_POWER_PHASE_2     = 0x96,
//    HOST_CMD_GET_RAW_REACTIVE_POWER_PHASE_3     = 0x97,
//    HOST_CMD_GET_RAW_ACTIVE_POWER_NEUTRAL       = 0x99,
//    HOST_CMD_GET_RAW_REACTIVE_POWER_NEUTRAL     = 0x9D,
//    HOST_CMD_CHECK_RTC_ERROR                    = 0xA0,
//    HOST_CMD_RTC_CORRECTION                     = 0xA1,
//    HOST_CMD_MULTIRATE_SET_PARAMETERS           = 0xC0,
//    HOST_CMD_MULTIRATE_GET_PARAMETERS           = 0xC1,
//    HOST_CMD_MULTIRATE_CLEAR_USAGE              = 0xC2,
//    HOST_CMD_MULTIRATE_GET_USAGE                = 0xC3
//};

#if defined(__MSP430__)  &&  (defined(IEC1107_SUPPORT)  ||  defined(SERIAL_CALIBRATION_SUPPORT))

#if defined(UART_PORT_1_SUPPORT)
serial_msg_buf_t tx_msg[2];
serial_msg_buf_t rx_msg[2];
#elif defined(UART_PORT_0_SUPPORT)
serial_msg_buf_t tx_msg[1];
serial_msg_buf_t rx_msg[1];
#endif

#endif

int is_calibration_enabled(void)
{
    return TRUE;
}

  #if defined(__MSP430__)  &&  (defined(IEC62056_21_SUPPORT)  ||  defined(IEC1107_SUPPORT)  ||  defined(SERIAL_CALIBRATION_SUPPORT))
/* Interrupt routines to send serial messages. */

/*
    #if defined(UART_PORT_0_SUPPORT)
        #if defined(__MSP430_HAS_EUSCI_A0__)


ISR(USCI_A0, serial_interrupt0)
{
    uint8_t ch;
    switch(__even_in_range(UCA0IV,USCI_UART_UCTXCPTIFG))
    {
    case USCI_NONE:
    break;
    case USCI_UART_UCRXIFG:
    ch = UCA0RXBUF;
//    UCA0TXBUF=ch + 1;

#if defined(UART0_DLT645_SUPPORT)
    dlt645_rx_byte(0, ch);
#endif
    break;
    case USCI_UART_UCTXIFG:
    UCA0TXBUF = tx_msg[0].buf.uint8[tx_msg[0].ptr++];
    if (tx_msg[0].ptr >= tx_msg[0].len)
    {
        // Stop transmission
        UCA0IE &= ~UCTXIE;
        tx_msg[0].ptr = 0;
        tx_msg[0].len = 0;
    }
    break;
    case USCI_UART_UCSTTIFG:
    break;
    case USCI_UART_UCTXCPTIFG:
    break;
    }

}
        #else
            #error Device does not have a UART port 0
        #endif
    #endif

    #if defined(UART_PORT_1_SUPPORT)

        #if defined(__MSP430_HAS_EUSCI_A1__)
ISR(USCI_A1, serial_interrupt1)
{
    uint8_t ch;
    switch(__even_in_range(UCA1IV,USCI_UART_UCTXCPTIFG))
    {
    case USCI_UART_UCRXIFG:
    ch = UCA1RXBUF;
    break;
    case USCI_UART_UCTXIFG:
    UCA1TXBUF = tx_msg[1].buf.uint8[tx_msg[1].ptr++];
    if (tx_msg[1].ptr >= tx_msg[1].len)
    {
        // Stop transmission
        UCA1IE &= ~UCTXIE;
        tx_msg[1].ptr = 0;
        tx_msg[1].len = 0;
    }
    break;
    case USCI_UART_UCSTTIFG:
    break;
    case USCI_UART_UCTXCPTIFG:
    break;
    }

}
        #else
            #error Device does not have a UART port 1
        #endif
    #endif

  void send_message(int port, int len)
  {
      tx_msg[port].ptr = 0;
      tx_msg[port].len = len;
      switch (port)
      {
          #if defined(UART_PORT_0_SUPPORT)
            case 0:
              #if defined(__MSP430_HAS_UART0__)
                U0IE |= UTXIE0;
              #else
                  UCA0IE |= UCTXIE;
              #endif
            break;
          #endif
          #if defined(UART_PORT_1_SUPPORT)
            case 1:
              #if defined(__MSP430_HAS_UART1__)
                U1IE |= UTXIE1;
              #else
               UCA1IE |= UCTXIE;
              #endif
            break;
          #endif
      }
  }
*/
  #endif
