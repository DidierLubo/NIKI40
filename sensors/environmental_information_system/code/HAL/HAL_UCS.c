/*
;*************************************************************************************************************************
;								                                                                                                 *
;     UCS - Modul                                                                                                        *
;     Erstellt am 12.02.2013                                                                                             *
;	                                                                                                                      *
;*************************************************************************************************************************
;     API zum UCS . Modul des MSP430
;*************************************************************************************************************************
*/

#include "Hardware.h"
#include "HAL_PMM.h"

/*
 *  This macro is for use by other macros to form a fully valid C statement.
 */
#define st(x)      do { x } while (__LINE__ == -1)

/* Select source for FLLREF  e.g. SELECT_FLLREF(SELREF__XT1CLK) */
#define SELECT_FLLREF(source) st(UCSCTL3 = (UCSCTL3 & ~(SELREF_7)) | (source);)
/* Select source for ACLK    e.g. SELECT_ACLK(SELA__XT1CLK) */
#define SELECT_ACLK(source)   st(UCSCTL4 = (UCSCTL4 & ~(SELA_7))   | (source);)
/* Select source for MCLK    e.g. SELECT_MCLK(SELM__XT2CLK) */
#define SELECT_MCLK(source)   st(UCSCTL4 = (UCSCTL4 & ~(SELM_7))   | (source);)
/* Select source for SMCLK   e.g. SELECT_SMCLK(SELS__XT2CLK) */
#define SELECT_SMCLK(source)  st(UCSCTL4 = (UCSCTL4 & ~(SELS_7))   | (source);)
/* Select source for MCLK and SMCLK e.g. SELECT_MCLK_SMCLK(SELM__DCOCLK + SELS__DCOCLK) */
#define SELECT_MCLK_SMCLK(sources) st(UCSCTL4 = (UCSCTL4 & ~(SELM_7 + SELS_7)) | (sources);)

/* set ACLK/x */
#define ACLK_DIV(x)         st(UCSCTL5 = (UCSCTL5 & ~(DIVA_7)) | (DIVA__##x);)
/* set MCLK/x */
#define MCLK_DIV(x)         st(UCSCTL5 = (UCSCTL5 & ~(DIVM_7)) | (DIVM__##x);)
/* set SMCLK/x */
#define SMCLK_DIV(x)        st(UCSCTL5 = (UCSCTL5 & ~(DIVS_7)) | (DIVS__##x);)
/* Select divider for FLLREF  e.g. SELECT_FLLREFDIV(2) */
#define SELECT_FLLREFDIV(x) st(UCSCTL3 = (UCSCTL3 & ~(FLLREFDIV_7))|(FLLREFDIV__##x);)

//------------------------------------------------------------------------------------------------------------------------
// Setzt den DCO auf die angegebene Frequenz
// Achtung: Ref-Clock muss richtig gewählt werden und den dazugeörigen Multiplikator
// Beispiel: REFCLOCK = 32768Hz (Quarz-Clock) * 244 = 7,995392MHz
//------------------------------------------------------------------------------------------------------------------------
void Init_FLL(uint16_t fsystem, uint16_t ratio)
{
   uint16_t d, dco_div_bits;
   uint16_t mode = 0;

   // Save actual state of FLL loop control, then disable it. This is needed to
   // prevent the FLL from acting as we are making fundamental modifications to
   // the clock setup.

   __bis_SR_register(SCG0);

   d = ratio;
   dco_div_bits = FLLD__2;                            // Have at least a divider of 2

   if (fsystem > 16000) {
      d >>= 1 ;
      mode = 1;
   }
   else {
      fsystem <<= 1;                                  // fsystem = fsystem * 2
   }

   while (d > 512) {
    dco_div_bits = dco_div_bits + FLLD0;              // Set next higher div level
    d >>= 1;
   }

   UCSCTL0 = DCO1;                                    // Set DCO to lowest Tap

   UCSCTL2 &= ~(0x03FF);                              // Reset FN bits
   UCSCTL2 = dco_div_bits | (d - 1);

   if (fsystem <= 630)                                //           fsystem < 0.63MHz
      UCSCTL1 = DCORSEL_0;
   else if (fsystem <  1250)                          // 0.63MHz < fsystem < 1.25MHz
      UCSCTL1 = DCORSEL_1;
   else if (fsystem <  2500)                          // 1.25MHz < fsystem <  2.5MHz
      UCSCTL1 = DCORSEL_2;
   else if (fsystem <  5000)                          // 2.5MHz  < fsystem <    5MHz
      UCSCTL1 = DCORSEL_3;
   else if (fsystem <  10000)                         // 5MHz    < fsystem <   10MHz
      UCSCTL1 = DCORSEL_4;
   else if (fsystem <  20000)                         // 10MHz   < fsystem <   20MHz
      UCSCTL1 = DCORSEL_5;
   else if (fsystem <  40000)                         // 20MHz   < fsystem <   40MHz
      UCSCTL1 = DCORSEL_6;
   else
      UCSCTL1 = DCORSEL_7;

   while (SFRIFG1 & OFIFG)
   {                                                  // Check OFIFG fault flag
      UCSCTL7 &= ~(DCOFFG | XT1LFOFFG | XT2OFFG);     // Clear OSC flaut Flags
      SFRIFG1 &= ~OFIFG;                              // Clear OFIFG fault flag
   }

   if (mode == 1) {                              		// fsystem > 16000
      SELECT_MCLK_SMCLK(SELM__DCOCLK + SELS__DCOCLK); // Select DCOCLK
   }
   else {
      SELECT_MCLK_SMCLK(SELM__DCOCLKDIV + SELS__DCOCLKDIV); // Select DCODIVCLK
   }

}

//------------------------------------------------------------------------------------------------------------------------
// CLK-System des MSP430 nach Systemstart initialisieren
//------------------------------------------------------------------------------------------------------------------------
void UcsUse_XTAL2(WORD wXT2DRIVE)
{
   SetVCore(0x03);

   __bis_SR_register(SCG0);                           // Disable the FLL control loop

   while(BAKCTL & LOCKBAK)                            // Unlock XT1 pins for operation
      BAKCTL &= ~(LOCKBAK);

   SELECT_ACLK(SELA__VLOCLK);
   UCSCTL8 &= ~ACLKREQEN;

   UCSCTL6 = XT1DRIVE_3 | XCAP_2 | XT2DRIVE_3;        // um einen schnellen Start der Quarze zu ermöglichen Drive auf Maximum stellen
                                                      // Loop until XT1, XT2
   do                                                 // *** Oszillatorkontrolle nach TI- Beispiel
   {
      UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
                                                      // Clear XT2,XT1,DCO fault flags
      SFRIFG1 &= ~OFIFG;                              // Clear fault flags
   }  while (SFRIFG1&OFIFG);                          // Test oscillator fault flag

//   while (UCSCTL7&XT2OFFG);                           // @@ Test: Testweise da 32768-Quarzosc. nicht funktioniert

   UCSCTL6 = wXT2DRIVE | XCAP_2;                      // XT1 an (32768Hz) mit XCAP2; XT2 = 24MHZ nun mit richtigem Drive

   UCSCTL5 = DIVM__1 | DIVS__2 | DIVA__1;               // MCLK = XT2CLK = 19,2MHz, SMCLK = XT2CLK / 2 = 9.6MHz, ACLK = 32768Hz
   UCSCTL4 = SELA__XT1CLK | SELM__XT2CLK | SELS__XT2CLK;

   UCSCTL7 = 0;
   SFRIFG1 &= ~OFIFG;                                 // Clear fault flags
   UCSCTL8 &= ~SMCLKREQEN;
}

//------------------------------------------------------------------------------------------------------------------------
// CLK-System des MSP430 nach Systemstart initialisieren
//------------------------------------------------------------------------------------------------------------------------
void UcsUse_DCO(void)
{
   SELECT_FLLREF(SELREF__XT1CLK);                     // 32768kHz Quarz als Referenz für DCO verwenden
   SELECT_ACLK(SELA__XT1CLK);

   Init_FLL(9600, 293);                               // DCO ~ 9,6MHz

   UCSCTL5 = DIVM__1 | DIVS__1 | DIVA__1;             // MCLK = DCO, SMCLK = DCO = 12MHz , ACLK = 32768

   UCSCTL6 |= XT2OFF ;                                // Nur High-Speed Osszillator aus

   __bic_SR_register(SCG0);                           // Enable the FLL control loop

   do                                                 // *** Oszillatorkontrolle nach TI- Beispiel
   {
      UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
                                                      // Clear XT2,XT1,DCO fault flags
      SFRIFG1 &= ~OFIFG;                              // Clear fault flags
   } while (SFRIFG1&OFIFG);                           // Test oscillator fault flag

   SetVCore(0x01);                                    // Bei 12MHz benötigt man min. VCORE = 01 (Siehe Datenblatt Seite 52)
   UCSCTL8 &= ~SMCLKREQEN;
}

//------------------------------------------------------------------------------------------------------------------------
// CLK-System des MSP430 nach Systemstart initialisieren
//------------------------------------------------------------------------------------------------------------------------
void Ucs_Start_XT1(void)
{
   if (UCSCTL6 & XT1OFF)
   {                                                  // Wenn Quarz-CLK aus dann jetzt einschalten
      while(BAKCTL & LOCKBAK)                         // Unlock XT1 pins for operation
         BAKCTL &= ~(LOCKBAK);

      UCSCTL6 |= XT1DRIVE_3;                          // Drive auf Max stellen damit Quarz schnell anschwingt
      UCSCTL6 &= ~XT1OFF;                             // Quarz-Osz. einschalten

      do                                              // *** Oszillatorkontrolle nach TI- Beispiel
      {
         UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
                                                      // Clear XT2,XT1,DCO fault flags
         SFRIFG1 &= ~OFIFG;                           // Clear fault flags
      }  while (SFRIFG1&OFIFG);                       // Test oscillator fault flag

      UCSCTL6 &= ~XT1DRIVE_3;                         // Drive auf min. stellen damit wenig Strom verbraucht wird
   }                                                  // reicht für einen 32768Hz Quarz
}