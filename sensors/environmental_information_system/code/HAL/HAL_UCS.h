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

void Init_FLL(WORD fsystem, WORD ratio);
void UcsUse_XTAL2(WORD wXT2DRIVE);
void UcsUse_DCO(void);
void Ucs_Start_XT1(void);
