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
//  File: emeter-fir-table.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: emeter-fir-table.c,v 1.11 2009/01/19 10:42:36 a0754793 Exp $
//
/*! \file emeter-structs.h */
//
//--------------------------------------------------------------------------
//
// This file was generated automatically by generate-fir-table
//

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


#include <stdint.h>
#include <io.h>
#include <emeter-toolkit.h>
#include "emeter-structs.h"
#if (defined(__HAS_SD_ADC__)  ||  !defined(__MSP430__))  &&  defined(REACTIVE_POWER_BY_QUADRATURE_SUPPORT)
/* The FIR table is needed to create accurate 90 degree phase shifts */
// FIR coefficients to correct specific phase shift angles at 50Hz
// and sampling 4096.000000 times per second.
const int16_t fir_coeffs[][2] =
{
    {  32767,   8198 }, // 2.1973 deg
    {  31761,   8326 }, // 2.1629 deg
    {  30785,   8454 }, // 2.1286 deg
    {  29838,   8582 }, // 2.0943 deg
    {  28918,   8710 }, // 2.0599 deg
    {  28025,   8838 }, // 2.0256 deg
    {  27158,   8965 }, // 1.9913 deg
    {  26315,   9093 }, // 1.9569 deg
    {  25496,   9221 }, // 1.9226 deg
    {  24698,   9349 }, // 1.8883 deg
    {  23923,   9477 }, // 1.8539 deg
    {  23168,   9605 }, // 1.8196 deg
    {  22433,   9733 }, // 1.7853 deg
    {  21716,   9861 }, // 1.7509 deg
    {  21019,   9988 }, // 1.7166 deg
    {  20338,  10116 }, // 1.6823 deg
    {  19675,  10244 }, // 1.6479 deg
    {  19028,  10372 }, // 1.6136 deg
    {  18397,  10500 }, // 1.5793 deg
    {  17781,  10628 }, // 1.5450 deg
    {  17180,  10756 }, // 1.5106 deg
    {  16593,  10884 }, // 1.4763 deg
    {  16019,  11011 }, // 1.4420 deg
    {  15459,  11139 }, // 1.4076 deg
    {  14911,  11267 }, // 1.3733 deg
    {  14376,  11395 }, // 1.3390 deg
    {  13852,  11523 }, // 1.3046 deg
    {  13340,  11651 }, // 1.2703 deg
    {  12839,  11779 }, // 1.2360 deg
    {  12348,  11907 }, // 1.2016 deg
    {  11869,  12035 }, // 1.1673 deg
    {  11399,  12162 }, // 1.1330 deg
    {  10939,  12290 }, // 1.0986 deg
    {  10488,  12418 }, // 1.0643 deg
    {  10047,  12546 }, // 1.0300 deg
    {   9614,  12674 }, // 0.9956 deg
    {   9190,  12802 }, // 0.9613 deg
    {   8775,  12930 }, // 0.9270 deg
    {   8367,  13058 }, // 0.8926 deg
    {   7968,  13185 }, // 0.8583 deg
    {   7576,  13313 }, // 0.8240 deg
    {   7191,  13441 }, // 0.7896 deg
    {   6814,  13569 }, // 0.7553 deg
    {   6444,  13697 }, // 0.7210 deg
    {   6080,  13825 }, // 0.6866 deg
    {   5724,  13953 }, // 0.6523 deg
    {   5373,  14081 }, // 0.6180 deg
    {   5029,  14209 }, // 0.5836 deg
    {   4691,  14337 }, // 0.5493 deg
    {   4360,  14465 }, // 0.5150 deg
    {   4033,  14593 }, // 0.4807 deg
    {   3713,  14720 }, // 0.4463 deg
    {   3398,  14848 }, // 0.4120 deg
    {   3088,  14976 }, // 0.3777 deg
    {   2784,  15104 }, // 0.3433 deg
    {   2485,  15232 }, // 0.3090 deg
    {   2190,  15360 }, // 0.2747 deg
    {   1901,  15488 }, // 0.2403 deg
    {   1616,  15616 }, // 0.2060 deg
    {   1336,  15744 }, // 0.1717 deg
    {   1060,  15872 }, // 0.1373 deg
    {    789,  16000 }, // 0.1030 deg
    {    522,  16128 }, // 0.0687 deg
    {    259,  16256 }, // 0.0343 deg
    {     -0,  16384 }, // -0.0000 deg
    {   -255,  16512 }, // -0.0343 deg
    {   -506,  16640 }, // -0.0687 deg
    {   -753,  16768 }, // -0.1030 deg
    {   -996,  16896 }, // -0.1373 deg
    {  -1236,  17024 }, // -0.1717 deg
    {  -1472,  17152 }, // -0.2060 deg
    {  -1705,  17280 }, // -0.2403 deg
    {  -1934,  17408 }, // -0.2747 deg
    {  -2160,  17536 }, // -0.3090 deg
    {  -2383,  17664 }, // -0.3433 deg
    {  -2602,  17792 }, // -0.3777 deg
    {  -2819,  17920 }, // -0.4120 deg
    {  -3032,  18049 }, // -0.4463 deg
    {  -3242,  18177 }, // -0.4807 deg
    {  -3450,  18305 }, // -0.5150 deg
    {  -3654,  18433 }, // -0.5493 deg
    {  -3856,  18561 }, // -0.5836 deg
    {  -4055,  18689 }, // -0.6180 deg
    {  -4252,  18817 }, // -0.6523 deg
    {  -4445,  18945 }, // -0.6866 deg
    {  -4636,  19074 }, // -0.7210 deg
    {  -4825,  19202 }, // -0.7553 deg
    {  -5011,  19330 }, // -0.7896 deg
    {  -5195,  19458 }, // -0.8240 deg
    {  -5376,  19586 }, // -0.8583 deg
    {  -5555,  19714 }, // -0.8926 deg
    {  -5732,  19843 }, // -0.9270 deg
    {  -5906,  19971 }, // -0.9613 deg
    {  -6079,  20099 }, // -0.9956 deg
    {  -6249,  20227 }, // -1.0300 deg
    {  -6417,  20356 }, // -1.0643 deg
    {  -6583,  20484 }, // -1.0986 deg
    {  -6747,  20612 }, // -1.1330 deg
    {  -6908,  20740 }, // -1.1673 deg
    {  -7068,  20869 }, // -1.2016 deg
    {  -7226,  20997 }, // -1.2360 deg
    {  -7382,  21125 }, // -1.2703 deg
    {  -7536,  21254 }, // -1.3046 deg
    {  -7689,  21382 }, // -1.3390 deg
    {  -7839,  21510 }, // -1.3733 deg
    {  -7988,  21639 }, // -1.4076 deg
    {  -8135,  21767 }, // -1.4420 deg
    {  -8281,  21895 }, // -1.4763 deg
    {  -8424,  22024 }, // -1.5106 deg
    {  -8566,  22152 }, // -1.5450 deg
    {  -8707,  22281 }, // -1.5793 deg
    {  -8846,  22409 }, // -1.6136 deg
    {  -8983,  22537 }, // -1.6479 deg
    {  -9119,  22666 }, // -1.6823 deg
    {  -9253,  22794 }, // -1.7166 deg
    {  -9386,  22923 }, // -1.7509 deg
    {  -9517,  23051 }, // -1.7853 deg
    {  -9647,  23180 }, // -1.8196 deg
    {  -9775,  23308 }, // -1.8539 deg
    {  -9903,  23437 }, // -1.8883 deg
    { -10028,  23565 }, // -1.9226 deg
    { -10153,  23694 }, // -1.9569 deg
    { -10276,  23823 }, // -1.9913 deg
    { -10397,  23951 }, // -2.0256 deg
    { -10518,  24080 }, // -2.0599 deg
    { -10637,  24208 }, // -2.0943 deg
    { -10755,  24337 }, // -2.1286 deg
    { -10872,  24466 }, // -2.1629 deg
};
#endif
