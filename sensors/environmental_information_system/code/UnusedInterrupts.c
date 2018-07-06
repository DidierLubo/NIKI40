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
//************************************************************************************************************************
//*   Modul dient dazu, dass für die ungenutzten Interrupts ein Vector vorhanden ist und so das Programm nicht           *
//*   abstürzt. Dazu wird der verurachende Interrupt abgeschlatet, so dass dieser nicht wieder zuschlagen sollte.        *
//************************************************************************************************************************
#include "Hardware.h"

void IllegalIRQ(void)
{
   return;
}

#pragma vector=SYSNMI_VECTOR
__interrupt void SYSNMI_IRQ(void)
{
   SYSSNIV = 0;
   IllegalIRQ();
}

#pragma vector=UNMI_VECTOR
__interrupt void UNMI_IRQ(void)
{
   SYSUNIV = 0;
   IllegalIRQ();
}

#pragma vector=RTC_VECTOR
__interrupt void RTC_IRQ(void)
{
   RTCCTL01 = RTCHOLD;
   IllegalIRQ();
}

#pragma vector=PORT1_VECTOR
__interrupt void PORT1_IRQ(void)
{
   P1IE = 0;
}

#pragma vector=PORT2_VECTOR
__interrupt void PORT2_IRQ(void)
{
   P2IE = 0;
   IllegalIRQ();
}

//#pragma vector=TIMER0_A0_VECTOR
//__interrupt void TIMER0_A0_IRQ (void)
//{
//   TA0CCR0 &= ~CCIE;
//   IllegalIRQ();
//}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_IRQ (void)
{
   TA0CTL  &= ~TAIE;
   TA0CCR1 &= ~CCIE;
   TA0CCR2 &= ~CCIE;
   IllegalIRQ();
}

//#pragma vector=TIMER1_A0_VECTOR
//__interrupt void TIMER1_A0_IRQ (void)
//{
//   TA1CCR0 &= ~CCIE;
//    IllegalIRQ();
//}

#pragma vector=TIMER1_A1_VECTOR
__interrupt void TIMER1_A1_IRQ (void)
{
   TA1CTL  &= ~TAIE;
   TA1CCR1 &= ~CCIE;
   TA1CCR2 &= ~CCIE;
   IllegalIRQ();
}

#pragma vector=TIMER2_A0_VECTOR
__interrupt void TIMER2_A0_IRQ (void)
{
   TA2CCR0 &= ~CCIE;
   IllegalIRQ();
}

#pragma vector=TIMER2_A1_VECTOR
__interrupt void TIMER2_A1_IRQ (void)
{
   TA2CTL  &= ~TAIE;
   TA2CCR1 &= ~CCIE;
   TA2CCR2 &= ~CCIE;
   IllegalIRQ();
}

#pragma vector=COMP_B_VECTOR
__interrupt void COMP_B_IRQ (void)
{
   CBINT = 0;
   IllegalIRQ();
}

#pragma vector=WDT_VECTOR
__interrupt void WDT_IRQ (void)
{
   SFRIE1 = 0;
   IllegalIRQ();
}

/*
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_IRQ (void)
{
   UCA0IE = 0;
   IllegalIRQ();
}
*/
#pragma vector=USCI_B1_VECTOR
__interrupt void USCI_B1_IRQ (void)
{
   UCB1IE = 0;
   IllegalIRQ();
}

//#pragma vector=DMA_VECTOR
//__interrupt void DMA_IRQ (void)
//{
//   DMA0CTL &= ~DMAIE;
//   DMA1CTL &= ~DMAIE;
//   DMA2CTL &= ~DMAIE;
//   IllegalIRQ();
//}
