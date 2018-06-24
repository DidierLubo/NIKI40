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
/* Hahn-Schickard:	BME280 von Bosch - Druck / Feuchte / Temp-Sensor
 *********************************************************************
 * FileName:         BME280.h
 * Dependencies:     See INCLUDES section below
 * Processor:        MSP430
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Bernd Ehrbrecht		25.08.2015
 ********************************************************************/

#include "Hardware.h"
#include "BME280.h"

#include "I2C_USCI_Modul.h"
#include "Task_Manager.h"
#include "Radio_Task.h"

//===============================================================================
//    Reg-Adr
//===============================================================================
#define BME280_ADR_ID               0xD0
#define BME280_ADR_RESET            0xE0
#define BME280_ADR_CTRL_HUM         0xF2
#define BME280_ADR_STATUS           0xF3
#define BME280_ADR_CTRL_MEAS        0xF4
#define BME280_ADR_CONFIG           0xF5

#define BME280_ADR_PRESS_MSB        0xF7
#define BME280_ADR_PRESS_LSB        0xF8
#define BME280_ADR_PRESS_XLSB       0xF9

#define BME280_ADR_TEMP_MSB         0xFA
#define BME280_ADR_TEMP_LSB         0xFB
#define BME280_ADR_TEMP_XLSB        0xFC

#define BME280_ADR_HUM_MSB          0xFD
#define BME280_ADR_HUM_LSB          0xFE

#define BME280_ADR_DIG_T1           0x88
#define BME280_ADR_DIG_T2           0x8A
#define BME280_ADR_DIG_T3           0x8C

#define BME280_ADR_DIG_P1           0x8E
#define BME280_ADR_DIG_P2           0x90
#define BME280_ADR_DIG_P3           0x92
#define BME280_ADR_DIG_P4           0x94
#define BME280_ADR_DIG_P5           0x96
#define BME280_ADR_DIG_P6           0x98
#define BME280_ADR_DIG_P7           0x9A
#define BME280_ADR_DIG_P8           0x9C
#define BME280_ADR_DIG_P9           0x9E

#define BME280_ADR_DIG_H1           0xA1
#define BME280_ADR_DIG_H2           0xE1
#define BME280_ADR_DIG_H3           0xE3
#define BME280_ADR_DIG_H4           0xE4
#define BME280_ADR_DIG_H5           0xE5
#define BME280_ADR_DIG_H6           0xE7

//===============================================================================
//    Reg/Bit-Values
//===============================================================================
#define BME280_VAL_ID                        0x60

#define BME280_VAL_RESET                     0xB6

// Values für BME280_ADR_CTRL_HUM
#define BME280_VAL_CH_SKIPP                  0x00
#define BME280_VAL_CH_OVERSAMPLING1          0x01
#define BME280_VAL_CH_OVERSAMPLING2          0x02
#define BME280_VAL_CH_OVERSAMPLING4          0x03
#define BME280_VAL_CH_OVERSAMPLING8          0x04
#define BME280_VAL_CH_OVERSAMPLING16         0x05

// Values für BME280_ADR_STATUS
#define BME280_BIT_STATUS_MEASURING          BIT3
#define BME280_BIT_STATUS_IM_UPDATE          BIT0

// Values für BME280_ADR_CTRL_MEAS
#define BME280_VAL_CM_TEMP_SKIPP             0x00
#define BME280_VAL_CM_TEMP_OVERSAMPLING1     0x20
#define BME280_VAL_CM_TEMP_OVERSAMPLING2     0x40
#define BME280_VAL_CM_TEMP_OVERSAMPLING4     0x60
#define BME280_VAL_CM_TEMP_OVERSAMPLING8     0x80
#define BME280_VAL_CM_TEMP_OVERSAMPLING16    0xA0

#define BME280_VAL_CM_PRESS_SKIPP            0x00
#define BME280_VAL_CM_PRESS_OVERSAMPLING1    0x04
#define BME280_VAL_CM_PRESS_OVERSAMPLING2    0x08
#define BME280_VAL_CM_PRESS_OVERSAMPLING4    0x0C
#define BME280_VAL_CM_PRESS_OVERSAMPLING8    0x10
#define BME280_VAL_CM_PRESS_OVERSAMPLING16   0x14

#define BME280_VAL_CM_SLEEP                  0x00
#define BME280_VAL_CM_FORCED_MODE            0x01
#define BME280_VAL_CM_NORMAL_MODE            0x03

// Values für BME280_ADR_CONFIG
#define BME280_VAL_CON_IA_0_5ms              0x00
#define BME280_VAL_CON_IA_62_5ms             0x20
#define BME280_VAL_CON_IA_125ms              0x40
#define BME280_VAL_CON_IA_250ms              0x60
#define BME280_VAL_CON_IA_500ms              0x80
#define BME280_VAL_CON_IA_1000ms             0xA0
#define BME280_VAL_CON_IA_10ms               0xC0
#define BME280_VAL_CON_IA_20ms               0xE0

#define BME280_VAL_CON_FC_OFF                0x00
#define BME280_VAL_CON_FC_2                  0x04
#define BME280_VAL_CON_FC_4                  0x08
#define BME280_VAL_CON_FC_8                  0x0C
#define BME280_VAL_CON_FC_16                 0x10

#define BME280_VAL_CON_SPI3_EN               0x01

//===============================================================================
// Sensor-abhängige Werte ausgelesen vom NV-Speicher des Sensors beim Init
//===============================================================================
#pragma pack(2)
typedef struct
{
   uint16_t  dig_T1;
   int16_t   dig_T2;
   int16_t   dig_T3;

   uint16_t  dig_P1;
   int16_t   dig_P2;
   int16_t   dig_P3;
   int16_t   dig_P4;
   int16_t   dig_P5;
   int16_t   dig_P6;
   int16_t   dig_P7;
   int16_t   dig_P8;
   int16_t   dig_P9;
}  ST_COM_T_P;

typedef struct
{
   uint8_t   dig_H1;
   int16_t   dig_H2;
   uint8_t   dig_H3;
   int16_t   dig_H4;
   int16_t   dig_H5;
   int8_t    dig_H6;
}  ST_COM_H;

typedef union
{
   uint8_t    b[4];
   uint16_t   w[2];
   uint32_t   dw;
   int32_t   l;
}  UN_BME;

typedef struct
{
   int32_t   adcT;
   int32_t   adcP;
   int32_t   adcH;
}  ST_BME_ADC;



//===============================================================================
//    Modul-Variable
//===============================================================================
ST_COM_T_P     mstCOM_T_P;
ST_COM_H       mstCOM_H;

//EN_BME280_STATUS en_Status = BME280_IDEL;

unsigned long  mulStartTime;
unsigned long  mulExpectedMessTime;
ST_BME280_MESS_DATA mstMessData;

//===============================================================================
// Interne Prototypen
//===============================================================================
uint32_t bme280_compensate_H_int32(int32_t adc_H);
uint32_t BME280_compensate_P_int32(int32_t adc_P);
int32_t BME280_compensate_T_int32(int32_t adc_T);
uint8_t  BME280_ReadMessdaten(void);
uint8_t  isBME280_MessungAktive(void);


//===============================================================================
// Globale Tabelle mit den Task-Funktionen
//===============================================================================
ST_FUNCTION_TABLE gstBME280_Func_Table[] =
{
   { BME280_IDEL                    , 1024   },       // TASK_IDEL:  nichts tun                       -> weiter mit: BME280_MESSUNG_START  hier die Messperiode eintragen
   { BME280_MESSUNG_START           , 200    },       // TASK_STEP1: Startet die Sensormessung        -> weiter mit: BME280_MESSUNG_READ_SENSOR
   { BME280_MESSUNG_READ_SENSOR     , 0      },       // TASK_STEP2: hier die Sensordaten einlesen    -> weiter mit: Send_BME280_Daten
   { Task_Send_BME280_Daten         , 0      }        // TASK_STEP3: Messdaten ausgeben               -> weiter mit: BME280_IDEL
};

//===============================================================================
//    Externe Funktionen
//===============================================================================

//===============================================================================
//    Initalisiert die Internen Variablen und den Sensor
//===============================================================================
uint8_t BME280_Init(uint32_t uiTime_ms )
{
   BME280_SetMessinterval( uiTime_ms );

   uint8_t bBuf[2];
   BME280_READ_REG(BME280_I2C_ADR, BME280_ADR_ID, bBuf, 1);

   if (bBuf[0] != BME280_VAL_ID)
      return 1;

   bBuf[0] = BME280_VAL_RESET;
   BME280_WRITE_REG(BME280_I2C_ADR, BME280_ADR_RESET, bBuf, 1);

   // Warte bis NVM-Data in die Register copiert sind
   for (int i=0; i<30 ; i++)
   {
      BME280_READ_REG(BME280_I2C_ADR, BME280_ADR_STATUS, &bBuf, 1);
      if (bBuf[0] & BME280_BIT_STATUS_IM_UPDATE)
         HAL_Delay_ms(1, LPM3_bits);
      else
         break;
   }

   BME280_READ_REG(BME280_I2C_ADR, BME280_ADR_DIG_T1, &mstCOM_T_P, sizeof(mstCOM_T_P));

   BME280_READ_REG(BME280_I2C_ADR, BME280_ADR_DIG_H1, &mstCOM_H.dig_H1, sizeof(mstCOM_H.dig_H1));
   BME280_READ_REG(BME280_I2C_ADR, BME280_ADR_DIG_H2, &mstCOM_H.dig_H2, 3);                           // dig_H2 + dig_H3 einlesen

   BME280_READ_REG(BME280_I2C_ADR, BME280_ADR_DIG_H5, &mstCOM_H.dig_H5, 3);                           // dig_H5 + dig_H6 einlesen
   mstCOM_H.dig_H5 >>= 4;

   BME280_READ_REG(BME280_I2C_ADR, BME280_ADR_DIG_H4, bBuf, 2);
   mstCOM_H.dig_H4   = bBuf[0];
   mstCOM_H.dig_H4 <<= 4;
   mstCOM_H.dig_H4  |= (bBuf[1] & 0x0F);

   bBuf[0] = BME280_VAL_RESET;
   BME280_WRITE_REG(BME280_I2C_ADR, BME280_ADR_RESET, bBuf, 1);

   bBuf[0] = BME280_VAL_CTRL_HUM;
   BME280_WRITE_REG(BME280_I2C_ADR, BME280_ADR_CTRL_HUM, bBuf, 1);

   bBuf[0] = BME280_VAL_CTRL_MEAS;
   BME280_WRITE_REG(BME280_I2C_ADR, BME280_ADR_CTRL_MEAS, bBuf, 1);

   bBuf[0] = BME280_VAL_CONFIG;
   BME280_WRITE_REG(BME280_I2C_ADR, BME280_ADR_CONFIG, bBuf, 1);

   // Extpected Messtime = 2 + 3 * Oversampling T + 3 * Oversampling P + 3 * Oversampling H
   mulExpectedMessTime = 4;
   if (BME280_VAL_CTRL_MEAS & 0xE0)
      mulExpectedMessTime += 3 * (1 << ((BME280_VAL_CTRL_MEAS & 0xE0) >> 5) -1);

   if (BME280_VAL_CTRL_MEAS & 0x1C)
      mulExpectedMessTime += 3 * (1 << ((BME280_VAL_CTRL_MEAS & 0x1C) >> 2) -1);

   if (BME280_VAL_CTRL_HUM & 0x07)
      mulExpectedMessTime += 3 * (1 << ((BME280_VAL_CTRL_HUM & 0x07)) -1);

   return 0;
}

//===============================================================================
//    BME280_GetLastMessdata: gibt den Zeiger auf die Messdaten zurück und
//    setzt den Status auf IDEL
//===============================================================================
ST_BME280_MESS_DATA *BME280_GetLastMessdata(void)
{
   return &mstMessData;
}

void BME280_SetMessinterval( uint32_t uiTime_ms )
{
   if (uiTime_ms<300) uiTime_ms = 300;

   gstBME280_Func_Table[0].ulTime = uiTime_ms - 200;
   Task_InitTaskExecutionTime( gstBME280_Func_Table, 0 );    // Step 0: IDEL-State
}

//===============================================================================
//    TASK_IDEL:     Warte-State bis Messung durchgeführt werden soll
//===============================================================================
EN_TASK_STATUS   BME280_IDEL( void )
{
   return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP1);
}

//===============================================================================
//    TASK_STEP1:    hier die Messung einleiten (Sensoren aktivieren)
//===============================================================================
EN_TASK_STATUS   BME280_MESSUNG_START( void )
{
   uint8_t bBuf[1];

   bBuf[0] = BME280_VAL_CTRL_MEAS | BME280_VAL_CM_FORCED_MODE;
   BME280_WRITE_REG(BME280_I2C_ADR, BME280_ADR_CTRL_MEAS, bBuf, 1);
   return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP2);
}

//===============================================================================
//    TASK_STEP2:    hier die Sensordaten einlesen
//===============================================================================
EN_TASK_STATUS   BME280_MESSUNG_READ_SENSOR( void )
{
   uint8_t bBuf[8];
   mstMessData.bStatus = BME280_READ_REG(BME280_I2C_ADR, BME280_ADR_PRESS_MSB, &bBuf, 8);

   if (mstMessData.bStatus)
   {
      Radio_SetStatusBit( RADIO_SENSOR_STATUS_BME280_ERR );
      return (EN_TASK_STATUS) (TASK_LPM3 | TASK_IDEL);
   }
   else
      Radio_ResetStatusBit( RADIO_SENSOR_STATUS_BME280_ERR );

   ST_BME_ADC stADC;
   UN_BME un;

   un.b[0] = bBuf[2];
   un.b[1] = bBuf[1];
   un.b[2] = bBuf[0];
   un.b[3] = 0;
   stADC.adcP = un.l >>4;

   un.b[0] = bBuf[5];
   un.b[1] = bBuf[4];
   un.b[2] = bBuf[3];
   un.b[3] = 0;
   stADC.adcT = un.l >>4;

   un.b[0] = bBuf[7];
   un.b[1] = bBuf[6];
   un.w[1] = 0;
   stADC.adcH = un.l;

   mstMessData.i32Temp      = BME280_compensate_T_int32( stADC.adcT );
   mstMessData.u32Pressure  = BME280_compensate_P_int32( stADC.adcP );
   mstMessData.u32Humity    = bme280_compensate_H_int32( stADC.adcH );

   return (EN_TASK_STATUS) (TASK_LPM3 | TASK_STEP3);
}



//===============================================================================
//    ab hier die Modulinternen funktionen
//===============================================================================


//===============================================================================
//    Prüft ob Messung noch andauert
//===============================================================================
uint8_t isBME280_MessungAktive(void)
{
   uint8_t bBuf[1];
   BME280_READ_REG(BME280_I2C_ADR, BME280_ADR_STATUS, &bBuf, 1);
   return (bBuf[0] & BME280_BIT_STATUS_MEASURING );
}

//===============================================================================
// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
//===============================================================================
int32_t t_fine;
int32_t BME280_compensate_T_int32(int32_t adc_T)
{
   int32_t var1, var2, T;
   T = (adc_T>>3) - ((int32_t)mstCOM_T_P.dig_T1<<1);
   var1 = (T * ((int32_t)mstCOM_T_P.dig_T2)) >> 11;
   T >>= 1;
   var2 = (((T * T ) >> 12) * ((int32_t)mstCOM_T_P.dig_T3)) >> 14;
   t_fine = var1 + var2;
   T = (t_fine * 5 + 128) >> 8;
   return T;
}

//===============================================================================
// Returns pressure in Pa as unsigned 32 bit integer. Output value of “96386” equals 96386 Pa = 963.86 hPa
//===============================================================================
uint32_t BME280_compensate_P_int32(int32_t adc_P)
{
   int32_t var1, var2;
   uint32_t p;
   var1 = (((int32_t)t_fine)>>1) - (int32_t)64000;
   var2 = (((var1>>2) * (var1>>2)) >> 11 ) * ((int32_t)mstCOM_T_P.dig_P6);
   var2 = var2 + ((var1*((int32_t)mstCOM_T_P.dig_P5))<<1);
   var2 = (var2>>2)+(((int32_t)mstCOM_T_P.dig_P4)<<16);
   var1 = (((mstCOM_T_P.dig_P3 * (((var1>>2) * (var1>>2)) >> 13 )) >> 3) + ((((int32_t)mstCOM_T_P.dig_P2) * var1)>>1))>>18;
   var1 =((((32768+var1))*((int32_t)mstCOM_T_P.dig_P1))>>15);
   if (var1 == 0)
   {
      return 0; // avoid exception caused by division by zero
   }
   p = (((uint32_t)(((int32_t)1048576)-adc_P)-(var2>>12)))*3125;
   if (p < 0x80000000)
   {
      p = (p << 1) / ((uint32_t)var1);
   }
   else
   {
      p = (p / (uint32_t)var1) * 2;
   }
   var1 = (((int32_t)mstCOM_T_P.dig_P9) * ((int32_t)(((p>>3) * (p>>3))>>13)))>>12;
   var2 = (((int32_t)(p>>2)) * ((int32_t)mstCOM_T_P.dig_P8))>>13;
   p = (uint32_t)((int32_t)p + ((var1 + var2 + mstCOM_T_P.dig_P7) >> 4));
   return p;
}

//===============================================================================
// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
// Output value of “47445” represents 47445/1024 = 46.333 %RH
//===============================================================================
uint32_t bme280_compensate_H_int32(int32_t adc_H)
{
   int32_t v_x1_u32r;

   v_x1_u32r = (t_fine - ((int32_t)76800));
   v_x1_u32r = (((((adc_H << 14) - (((int32_t)mstCOM_H.dig_H4) << 20) - (((int32_t)mstCOM_H.dig_H5) * v_x1_u32r)) +
      ((int32_t)16384)) >> 15) * (((((((v_x1_u32r * ((int32_t)mstCOM_H.dig_H6)) >> 10) * (((v_x1_u32r *
      ((int32_t)mstCOM_H.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) *
      ((int32_t)mstCOM_H.dig_H2) + 8192) >> 14));
   v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)mstCOM_H.dig_H1)) >> 4));
   v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
   v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
   return (uint32_t)(v_x1_u32r>>12);
}