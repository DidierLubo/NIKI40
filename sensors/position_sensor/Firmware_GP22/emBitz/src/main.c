/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include "hw_config.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "gp22.h"
#include "utils.h"
#include <stdbool.h>


/* USER CODE BEGIN Includes */
#define MESSMODE_GP22     0
#define MESSMODE_LVDT     1
#define FALSE             0
#define TRUE              1
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim;

HAL_StatusTypeDef hal_status;

uint8_t sys_status; // Daouda 04.10.2017
uint8_t  ret=0;

uint8_t  gp22_channel;
uint32_t  anzahl_mw;

// Anzahl der Werte für Mittelwert LVDT
uint8_t averageCountLVDT 	= 32;
bool bComProcessingActive = FALSE;
//uint16_t data_size;
//int64_t  pcap01_results[50][12];
//uint32_t pcap04_results[50][8];
//volatile uint32_t cyc1;
//volatile uint32_t cyc2;
//volatile uint32_t cyc3;
//uint32_t elapsed_us;
//uint32_t messzeit_mcu_pcap;
//uint32_t messzeit_mcu_vcom;
//
//uint64_t val_u64;
//int64_t  val_64;
//int32_t  val_32;
//
//int16_t  acc_x, acc_y, acc_z;
//int16_t  gyro_x, gyro_y, gyro_z;
//double   acc_x_f, acc_y_f, acc_z_f;
//double   gyro_x_f, gyro_y_f, gyro_z_f;
//double   temperature;
//
//double   ms5611_p;
//double   ms5611_t;
//
//float    sht2x_temperature, sht2x_humidity;
//int16_t  sht2x_temp, sht2x_humi;
//uint32_t adc1, adc2;
//uint8_t  adc_status1=0;
//uint8_t  adc_status2=0;
//double   adc_val=0;
//
//uint32_t raw_data1[4000];
//uint32_t raw_data2[4000];
//uint32_t raw_data3[4000];
//uint32_t raw_data4[4000];

uint8_t messung_laeuft=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

void SystemClock_Config(void);
void Error_Handler(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

uint8_t  calc_std_dev;
uint8_t  gp22_channel;
//uint16_t anzahl_mw;
uint8_t  temp_mess;
uint8_t  result_mask; // Result Register auslesen: Bit0: RES0, Bit1: RES1, Bit2: RES2, Bit3: RES3
uint32_t res_array[1000];
uint32_t result[4];
float    std_dev;
uint64_t res_sum;
uint64_t sum_squares;
float    gp22_temp;
uint16_t gp22_status;
uint32_t gp22_clock_calibration_value;
float    pt1000_temp;

uint32_t reg0temper;     // 25.05.2018 Daouda
uint32_t reg1temper;
uint32_t reg2temper;     // 29.05.2018 Kible
uint32_t reg3temper;
uint32_t reg0lvdt;       // 29.05.2018 Kible
uint32_t reg1lvdt;
uint32_t reg2lvdt;       // 29.05.2018 Kible
uint32_t reg3lvdt;

float rtdv;

uint8_t  messung_laeuft;
uint8_t  mess_mode;
volatile uint32_t cc;
volatile uint32_t cc_prev;
uint32_t elapsed_us;
uint8_t  erste_messung;
uint8_t  anzahl_datenbloecke;

/*****************************************************************************/
/*                                                                           */
/* gp22_init                                                               */
/*                                                                           */
/* Anfangskonfiguration setzen                                               */
/*                                                                           */
/*****************************************************************************/
uint8_t gp22_initialize()  //  Daouda 04.10.2017 code von Klaus
{
	uint8_t ret=0;

	gp22_hw_setup();

    GP22_CS_HIGH();
    __HAL_SPI_ENABLE(&hGP22_SPI);

	gp22_reset();
	ret = gp22_test_communication();
	if (ret>0)
	{
	  sys_status=1;
	}
	else
	{
      ret = gp22_power_on_reset();
      delay_us(500);
	  if (ret>0)
	  {
		sys_status=2;
	  }
	  else
	  {
		ret =  gp22_write_configuration(gp22_config_buffer,GP22_CONFIG_BUFFER_SIZE); // Konfiguration Registers
		  if (ret>0)
		  {
			sys_status=3;
		  }
	  }
	}
	return 0;

}

// 29.5.2018 V.Kible

uint8_t gp22_read_raw(void)
{
    uint8_t ret = 0;
    reg0lvdt = 0;
    reg1lvdt = 0;
    reg2lvdt = 0;
    reg3lvdt = 0;

    ret = gp22_read_result_register(GP22_RESULT_REG_0, &reg0lvdt);
	if (ret>0)
		return ret;

	ret = gp22_read_result_register(GP22_RESULT_REG_1, &reg1lvdt);
	if (ret>0)
		return ret;

	ret = gp22_read_result_register(GP22_RESULT_REG_2, &reg2lvdt);
	if (ret>0)
		return ret;

	ret = gp22_read_result_register(GP22_RESULT_REG_3, &reg3lvdt);
	if (ret>0)
		return ret;

	return 0;
}

 // ** 02.11.2017 Daouda

// Anfangs und nach Fehler:
uint8_t TDC_ResetAndConfig(void)
{
	// Reset
	uint8_t ret = gp22_reset();
	if (ret != 0) return ret;

	// Write Config
	ret = gp22_write_configuration(gp22_config_buffer, GP22_CONFIG_BUFFER_SIZE);
	if (ret != 0) return ret;

	// Clock Cal
	return gp22_calibrate_clock();
}

uint8_t GetLVDTaverageVal(int32_t* averageOfReg) // Rückgabe: Fehler (0 = kein)
{
	uint8_t ret = 0;
	uint8_t countVal = 0;
	uint32_t newResult = 0;
	uint32_t sumResults = 0;
	uint32_t val_u32;

    averageCountLVDT = anzahl_mw;

    if (averageCountLVDT == 0) averageCountLVDT = 1;
	do
	{
		ret = gp22_init_tdc(); // Wartet auf INT-Pin
		if ( ret == 0 )
		{
			ret = gp22_read_result_register(gp22_result_reg_nr, &val_u32); // HIER DANN HALT 4 RESULT REGISTER ABFRAGEN
			if (ret == 0)
            {
              sumResults += val_u32;
              countVal++;
            }
            else
              sprintf((char *)vcom_answer, "Timeout ");



		}
	} while ( (countVal < averageCountLVDT) && (ret == 0) );

    if ( countVal != 0 )	*averageOfReg = (int32_t)(sumResults/countVal);
    else				    *averageOfReg = 0;

	return ret; // Pfad für Error
}


int32_t CMD_lvdt_R(void)		// LVDT-Rohwert Lesen
{
	int32_t registerAvg = 0;
	// char timeString[20];
	// int32_t startTime = iTimerVal;

	if (GetLVDTaverageVal(&registerAvg) == 0)
	{
		//LED_RED_OFF();
		// sprintf((char*)timeString, "Zeit=%d us\r", (int)(iTimerVal-startTime));
		// VCOM_send_string(timeString);
		return registerAvg;
	}
	else
	{
		//LED_RED_ON();
		VCOM_send_string((char*)"Fehler beim Lesen!\r");
		return 0;
	}
}


/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
static void timer2_init()
{
    TIM_ClockConfigTypeDef sClockSourceConfig;

    htim.Instance = TIM2;
    htim.Init.Prescaler = 84;
    htim.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim.Init.Period = 4294967295;
    htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    if (HAL_TIM_Base_Init(&htim) != HAL_OK)
    {
        Error_Handler();
    }

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    HAL_TIM_Base_Start(&htim);
}

void process_vcom_command(void)
{
  //uint8_t  val_u8;
  uint16_t val_u16;
  uint32_t val_u32;
  //int32_t  val_s32;
  //int16_t  val_s16;
  uint8_t  send;
  int      i;

  uint8_t val_u8;

	uint16_t temp, temp2;
	uint8_t match;
//  uint32_t ticks1,ticks2;
	uint32_t val32;
	uint16_t val16;
//  int32_t val_i32;
	uint32_t ret;
	char s_temp[20];


  send = 0;
  ret = 0;

  // Dummy-Schleife
  while (1)
  {
    if (strcmp((const char *)vcom_command, "ver") == 0)
    {
      sprintf((char *)vcom_answer, "Firmware=GP22_V%d.%d.%d\r", VER_MAJ, VER_MIN, VER_BUILD);
      send = 1;
      break;
    }

    if (strcmp((const char *)vcom_command, "board") == 0)
    {
      sprintf((char *)vcom_answer, "%s\r",MCU_BOARD);
      send = 1;
      break;
    }

    if (strcmp(vcom_command, "gp22_reset") == 0)
	{
		ret = gp22_power_on_reset();
		if (ret == 0)
			sprintf(vcom_answer, "OK\r");
		else
			sprintf(vcom_answer, "GP22 reset failed: %d\r", ret);
		send = 1;
		break;
	}
    // Anzahl Messwerte pro Messregister lesen
    if (strcmp((const char *)vcom_command, "mw_pro_reg") == 0)
    {
      sprintf((char *)vcom_answer, "mw_pro_reg=%d\r", anzahl_mw);
      send = 1;
      break;
    }

    /********************/
     if (strcmp((const char *)vcom_command, " ") == 0)
    {
      ret = gp22_reset();

      if (ret == 0)
      {
        sprintf((char *)vcom_answer, "OK\r");
      }
      else
      {
        sprintf((char *)vcom_answer, "ERR%d\r", ret);
      }

      send = 1;
      break;
    }

    /*************************/
    if (strcmp((const char *)vcom_command, "gp22_initialize") == 0)
    {
      // gp22_hw_setup();
      // ret = TDC_ResetAndConfig();
      gp22_initialize();

      if (sys_status == 0)
      {
        sprintf((char *)vcom_answer, "OK\r");
      }
      else
      {
        sprintf((char *)vcom_answer, "ERR%d\r", sys_status);
      }

      send = 1;
      break;
    }

    // Anzahl Messwerte pro Messregister setzen
    if (strncmp((const char *)vcom_command, "mw_pro_reg=", 11) == 0)
    {
      anzahl_mw = (uint32_t)get_int_param((char *)vcom_command, 11);
      sprintf((char *)vcom_answer, "OK\r");

      send = 1;
      break;
    }

    if (strcmp((const char *)vcom_command, "gp22_test") == 0)
    {
      ret = gp22_test_communication();

      if (ret == 0)
      {
        sprintf((char *)vcom_answer, "OK\r");
      }
      else
      {
        sprintf((char *)vcom_answer, "ERR%d\r", ret);
      }

      send = 1;
      break;
    }

    if (strcmp((const char *)vcom_command, "gp22_start_temp") == 0)
    {
      ret = gp22_start_temp();
      if (ret == 0)
        sprintf((char *)vcom_answer, "OK\r");
      else
        sprintf((char *)vcom_answer, "GP22 start temp failed: %d\r", ret);

      send = 1;
      break;
    }

    if (strcmp((const char *)vcom_command, "gp22_start_tof") == 0)
    {
      ret = gp22_start_tof();
      if (ret == 0)
        sprintf((char *)vcom_answer, "OK\r");
      else
        sprintf((char *)vcom_answer, "GP22 start TOF failed: %d\r", ret);

      send = 1;
      break;
    }

    if (strcmp((const char *)vcom_command, "gp22_init_tdc") == 0)
    {
      ret = gp22_init_tdc();

      gp22_start_tof_restart();  //30.10.2017 Daouda

      if (ret == 0)
        sprintf((char *)vcom_answer, "OK\r");
      else
        sprintf((char *)vcom_answer, "GP22 init TDC failed: %d\r", ret);

      send = 1;
      break;
    }

    if (strcmp((const char *)vcom_command, "gp22_calibrate_clock") == 0)
    {
      ret = gp22_calibrate_clock();
      if (ret == 0)
        sprintf((char *)vcom_answer, "OK\r");
      else
        sprintf((char *)vcom_answer, "GP22 calibrate clock failed: %d\r", ret);

      send = 1;
      break;
    }

    if (strcmp((const char *)vcom_command, "gp22_calibrate_tdc") == 0)
    {
      ret = gp22_calibrate_tdc();
      if (ret == 0)
        sprintf((char *)vcom_answer, "OK\r");
      else
        sprintf((char *)vcom_answer, "GP22 calibrate tdc failed: %d\r", ret);

      send = 1;
      break;
    }

    if (strcmp((const char *)vcom_command, "gp22_status") == 0)
    {
      ret = gp22_read_status_register(&val_u16);
      if (ret == 0)
        sprintf((char *)vcom_answer, "GP22-Status=0x%04X\r", val_u16);
      else
        sprintf((char *)vcom_answer, "ERR%d\r", ret);

      send = 1;
      break;
    }

    if (strcmp((const char *)vcom_command, "gp22_int_status") == 0)
    {
      sprintf((char *)vcom_answer, "INTN: %d\r", GP22_INT_IN());

      send = 1;
      break;
    }

    // Konfiguration aus dem GP22 lesen
    if (strcmp((const char *)vcom_command, "gp22_read_config") == 0)
    {
      i = 0;
      ret = 0;
      for (i = 0; i < GP22_CONFIG_BUFFER_SIZE - 1; i++)
      {
        sprintf((char *)vcom_answer, "%08lX ", gp22_config_buffer[i]);
        VCOM_send_string((char *)vcom_answer);
        VCOM_clear_answer();
        //****************** 19.10.17
        delay_ms(10);
      }
      sprintf((char *)vcom_answer, "%08lX\r", gp22_config_buffer[i]);
      VCOM_send_string((char *)vcom_answer);
      VCOM_clear_answer();

      send = 0;
      break;
    }

    // TestCom: 18.10.2017 Daouda
    if(strcmp((const char *)vcom_command, "test_com") == 0)
    {
      uint8_t j;
      char dest[12];

      ret = gp22_read_result_register(0, &val_u32);
      if (ret == 0)
          sprintf((char *)vcom_answer, "%lu", val_u32);
      else
          sprintf((char *)vcom_answer, "Timeout ");

      for (j = 1; j < 4; j++)
      {
        ret = gp22_read_result_register(j, &val_u32);
        if (ret == 0)
        {
          sprintf((char *)dest, " %lu", val_u32);
          strcat((char *)vcom_answer, (char *)dest);
        }
        else
          sprintf((char *)vcom_answer, "Timeout ");
      }
      strcat((char *)vcom_answer, (char *)"\r");
      VCOM_send_string((char *)vcom_answer);
      VCOM_clear_answer();
      break;
    }


    // Konfiguration in den GP22 schreiben
    if (strcmp((const char *)vcom_command, "gp22_write_config") == 0)
    {
      ret = gp22_write_configuration(gp22_config_buffer, GP22_CONFIG_BUFFER_SIZE);
      if (ret == 0)
      {
        sprintf((char *)vcom_answer, "OK\r");
      }
      else
      {
        sprintf((char *)vcom_answer, "ERR%d\r", ret);
      }

      send = 1;
      break;
    }

    // Aktuelle Konfigurationsregisternummer setzen
    if (strncmp((const char *)vcom_command, "gp22_config_reg_nr=", 19) == 0)
    {
      gp22_config_reg_nr = get_int_param((char *)vcom_command, 19);
      sprintf((char *)vcom_answer, "OK\r");

      send = 1;
      break;
    }

    // Aktuelle Konfigurationsregisternummer lesen
    if (strcmp((const char *)vcom_command, "gp22_config_reg_nr") == 0)
    {
      sprintf((char *)vcom_answer, "gp22_config_reg_nr=%d\r", gp22_config_reg_nr);

      send = 1;
      break;
    }

    // Wert des aktuellen Konfigurationsregisters setzen
    if (strncmp((const char *)vcom_command, "gp22_config_reg_data=", 21) == 0)
    {
      val_u32 = get_hex_param((char *)vcom_command, 21);
      gp22_config_buffer[gp22_config_reg_nr] = val_u32;
      sprintf((char *)vcom_answer, "OK\r");

      send = 1;
      break;
    }

    // Wert des aktuellen Konfigurationsregisters auslesen
    if (strcmp((const char *)vcom_command, "gp22_config_reg_data") == 0)
    {
      sprintf((char *)vcom_answer, "gp22_register%d=%08lX\r", gp22_config_reg_nr, gp22_config_buffer[gp22_config_reg_nr]);

      send = 1;
      break;
    }

    if (strncmp((const char *)vcom_command, "gp22_result_reg_nr=", 19) == 0)
    {
      gp22_result_reg_nr = get_int_param((char *)vcom_command, 19);
      sprintf((char *)vcom_answer, "OK\r");

      send = 1;
      break;
    }

    if (strcmp((const char *)vcom_command, "gp22_result_reg_nr") == 0)
    {
      sprintf((char *)vcom_answer, "gp22_result_reg_nr=%d\r", gp22_result_reg_nr);

      send = 1;
      break;
    }

    if (strcmp((const char *)vcom_command, "gp22_result_reg_data") == 0)
    {
      ret = 0;
      for (i = 0; i < anzahl_mw - 1; i++)
      {
        ret = gp22_read_result_register(gp22_result_reg_nr, &val_u32);
        if (ret == 0)
          sprintf((char *)vcom_answer, "Reg=%08lX ", val_u32);
        else
          sprintf((char *)vcom_answer, "Timeout ");
        VCOM_send_string((char *)vcom_answer);
        VCOM_clear_answer();
	    delay_us(500);
      }
      ret = gp22_read_result_register(gp22_result_reg_nr, &val_u32);
      if (ret == 0)
        sprintf((char *)vcom_answer, "Reg=%08lX\r", val_u32);
      else
        sprintf((char *)vcom_answer, "Timeout\r");
      VCOM_send_string((char *)vcom_answer);
      VCOM_clear_answer();

      send = 0;
      break;
    }

    if (strcmp((const char *)vcom_command, "gp22_results") == 0)
    {
     // int j = 0;
      ret = 0;
      uint8_t j;
      char dest[12];

        // ******* 24.10.17 V.Kible
      gp22_init_tdc();      //Warte auf INT_pin

      ret = gp22_read_result_register(0, &val_u32);  // result-register0 lesen
      if (ret == 0)
        sprintf((char *)vcom_answer, "%lu", val_u32);
      else
        sprintf((char *)vcom_answer, "Timeout ");

        // Anwortstring zusammenbauen
      for (j = 1; j < 4; j++)
      {
        ret = gp22_read_result_register(j, &val_u32);
        if (ret == 0)
        {
          sprintf((char *)dest, " %lu", val_u32);
          strcat((char *)vcom_answer, (char *)dest);
        }
        else
          sprintf((char *)vcom_answer, "Timeout ");
       }

      strcat((char *)vcom_answer, (char *)"\r");
      VCOM_send_string((char *)vcom_answer);        // String werden an PC gesendet
      VCOM_clear_answer();
      break;
    }

    if (strcmp(vcom_command, "gp22_mess") == 0)
	{
      int i = 0;
      int err=0;
      ret = 0;
      messung_laeuft=1;
      cc_prev = cc;
      cc = read_cycle_counter();
      elapsed_us = (cc-cc_prev)/72;

    // Messung starten
      gp22_init_tdc();
    // Ergebnis(se) auslesen
      for (i = 0; i < 4; i++)
      {
        if (result_mask & (1<<i))
        {
          ret = gp22_read_result_register(i, &val32);
          if (ret==0)
            result[i] = val32;
            err += ret;
        }
      }
		// Statusregister auslesen
      ret = gp22_read_status_register(&val16);
      if (ret == 0)
        gp22_status = val16;
		err += ret;

		// Antwortstring zusammenbauen
      if (err==0)
      {
        for (i=0; i<4; i++)
        {
          if (result_mask & (1<<i))
          {
            sprintf(vcom_answer, "%s%lu ", vcom_answer,result[i]);
          }
        }
       sprintf(vcom_answer, "%s%u\r", vcom_answer,gp22_status);
       }
       else
       {
        sprintf(vcom_answer, "Timeout ");
       }
       send = 1;
       break;
	 }

	if (strcmp((const char *)vcom_command, "gp22_read_pt1000") == 0)
	{
	    uint32_t reg0v, reg1v;
	    float    rtdv;

		ret = gp22_read_pt1000(&pt1000_temp, &rtdv, &reg0temper, &reg1temper, &reg2temper, &reg3temper);//, &rtdv, &reg0v, &reg1v); // 25.05.2018 Daouda
		if (ret == 0)
		  sprintf((char *)vcom_answer, "Temprg0-3: %X %X %X %X\r", reg0temper, reg1temper, reg2temper, reg3temper); // 25.05.2018 Daouda
		 // sprintf((char *)vcom_answer, "pt1000=%f°C\r", pt1000_temp);
		else
		  sprintf((char *)vcom_answer, "Err:%d\r", ret);

		send = 1;
		break;
	}

    if (strcmp((const char *)vcom_command,"gp22_pt1000_ON")==0)
    {
      temp_mess=1;
      sprintf(vcom_answer, "OK\r");
      send=1;
      break;
    }

    if (strcmp((const char *)vcom_command,"gp22_pt1000_OFF")==0)
    {
      temp_mess=0;
      sprintf(vcom_answer, "OK\r");
      send=1;
      break;
    }

    if (strcmp((const char *)vcom_command,"gp22_start")==0)
    {
      messung_laeuft=1;
      sprintf(vcom_answer, "OK\r");
      send=0;
      break;
    }

    if (strcmp((const char *)vcom_command,"gp22_stop")==0)
    {
      messung_laeuft=0;
      sprintf(vcom_answer, "OK\r");
      send=0;
      break;
    }

     // Messmode lesen
	if (strcmp(vcom_command, "mess_mode") == 0)
	{
		sprintf(vcom_answer, "mess_mode=%d\r", mess_mode);
		send = 1;
		break;
	}

	// Messmode setzen
	if (strncmp(vcom_command, "mess_mode=", 10) == 0)
	{
		mess_mode = get_int_param(vcom_command, 10);
		sprintf(vcom_answer, "OK\r");
		send = 1;
		break;
	}

    if ( strlen((const char*)vcom_command) > 0 )
    {
      sprintf((char *)vcom_answer, "unbekannter Befehl: %s\r", vcom_command);
      send = 1;
      break;
    }

  }

//  if (match == 0)
//  {
//    sprintf((char *)vcom_answer, "unbekannter Befehl: %s\r", vcom_command);
//    send = 1;
//  }
  VCOM_clear_command();

  if (send == 1)
  {
    VCOM_send_string((char *)vcom_answer);
    VCOM_clear_answer();
  }

  return;
}



//void messung()
//{
////  HAL_TIM_Base_Stop(&htim);
//    __HAL_TIM_SetCounter(&htim,0);
////  HAL_TIM_Base_Start(&htim);
//    switch(sensor_chip)
//    {
//    case SENSORCHIP_BMA280:
//    {
//        ret = bma280_acc_xyzt(&acc_x_f,&acc_y_f,&acc_z_f,&temperature);
//        //HAL_TIM_Base_Stop(&htim);
//        count_us = __HAL_TIM_GetCounter(&htim);
//        if (ret==0)
//            sprintf((char *)vcom_answer,"%ld %f %f %f %5.2f\r",count_us,acc_x_f,acc_y_f,acc_z_f,temperature);
//        else
//            sprintf((char *)vcom_answer,"Fehler: %d\r",ret);
//
//    }
//    break;
//
//    case SENSORCHIP_ADXL355:
//    {
//        ret = adxl355_acc_xyz(&acc_x_f,&acc_y_f,&acc_z_f);
//        //HAL_TIM_Base_Stop(&htim);
//        count_us = __HAL_TIM_GetCounter(&htim);
//        if (ret==0)
//            sprintf((char *)vcom_answer,"%ld %f %f %f\r",count_us,acc_x_f,acc_y_f,acc_z_f);
//        else
//            sprintf((char *)vcom_answer,"Fehler: %d\r",ret);
//
//    }
//    break;
//    }
//    VCOM_send_string((char *)vcom_answer);
//    VCOM_clear_answer();
//}
/* USER CODE END 0 */

int main(void)
{
    /* USER CODE BEGIN 1 */
    anzahl_mw = 1;
    /* USER CODE END 1 */
    char dest[12];
    int ret = 0;
	int err = 0;
	int j=0;
	int i = 0;
	uint32_t val32 = 0;
	uint16_t val16 = 0;
	uint32_t blockzaehler=0;
	sys_status = 0;
	calc_std_dev = 1;
	anzahl_mw = 1;
	temp_mess = 0;
	gp22_result_reg_nr = 0;

	for (i=0; i<4; i++)
    {
  	  result[i]=0;
    }

	result_mask = 1; // RES0 aktiv
    /* MCU Configuration----------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();
    init_cycle_counter();
    timer2_init();   // Mikrosekunden-Stoppuhr

    /* Initialize all configured peripherals */
    i2c_select_init();
    SPI_SELECT();
    HAL_Delay(500);

    MX_USB_DEVICE_Init();
    /* USER CODE BEGIN 2 */
    VCOM_init();

    gp22_initialize();  // Daouda 04.10.2017, code von Klaus
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    HAL_Delay(10);

    while (1)
    {
        /* USER CODE END WHILE */
        if (vcom_data_available == 1) // Daten von serieller Schnittstelle empfangen ?
        {
          if (vcom_transfer_mode == VCOM_RX_COMMAND_MODE)
             process_vcom_command();
             vcom_data_available = 0;
        }

        if (messung_laeuft==1)
        {
          if (erste_messung==1)
           {
             cc_prev = read_cycle_counter();
             cc = cc_prev;
             erste_messung=0;
             blockzaehler=0;
           }
          else
           {
             cc_prev = cc;
             cc = read_cycle_counter();
             blockzaehler++;
           }

           elapsed_us = (cc-cc_prev)/72;

          if (mess_mode==MESSMODE_GP22)      // mess_mode=0
           {
            // Messung starten
             gp22_init_tdc();
            // Ergebnis(se) auslesen
             for (i = 0; i < 4; i++)
              {
                if (result_mask & (1<<i))
                 {
                   ret = gp22_read_result_register(i, &val32);
                   if (ret==0)
                   result[i] = val32;
                   err += ret;
                 }
              }

            // Statusregister auslesen
              ret = gp22_read_status_register(&val16);
              if (ret == 0)
                gp22_status = val16;
                err += ret;

//            // Antwortstring zusammenbauen
              if (err==0)
               {
                 sprintf(vcom_answer, "%lu ",elapsed_us);
                 for (i=0; i<4; i++)
                  {
                    if (result_mask & (1<<i))
                     {
                       sprintf(vcom_answer, "%s%lu ", vcom_answer,result[i]);
                     }
                  }
                }
              else
               {
                 sprintf(vcom_answer, "Timeout ");
               }
              VCOM_send_string(vcom_answer);
              delay_ms(200);
            }

           if (mess_mode==MESSMODE_LVDT)         // mess_mode=1
            {
              if (anzahl_mw<=0)
                anzahl_mw=1;
              if (anzahl_mw>1000)
                anzahl_mw=1000;

              res_sum=0;
              for (i=0; i<anzahl_mw; i++)
              {
                // Messung starten
                   gp22_init_tdc();
                // Ergebnis auslesen
                   ret = gp22_read_result_register(0, &val32);
                   res_array[i]=val32;
                   res_sum += val32;
                   err += ret;
              }

                ret = gp22_read_raw();

               if (err==0)
               {
                 result[0] = (uint32_t)(res_sum/anzahl_mw);
                 if (calc_std_dev>0)
                 {
                // Standardabweichung berechnen
                   sum_squares=0;
                   for (i=0; i<anzahl_mw; i++)
                   {
                     sum_squares += (res_array[i]-result[0])*(res_array[i]-result[0]);
                   }
                  std_dev = sqrt((float)sum_squares/anzahl_mw);
                 }
                }

                err += ret;

              // Statusregister auslesen
                ret = gp22_read_status_register(&val16);
                if (ret == 0)
                  gp22_status = val16;
                err += ret;

                if (temp_mess)
                    //ret = gp22_read_pt1000(&gp22_temp);
                    //ret = gp22_read_pt1000(&gp22_temp, &rtdv, &reg0v, &reg1v);
                    ret=gp22_read_pt1000(&pt1000_temp, &rtdv, &reg0temper, &reg1temper, &reg2temper, &reg3temper);  // 25.05.2018 Daouda
                err += ret;

              // Antwortstring zusammenbauen
                if (err==0)
                {
                  sprintf(vcom_answer, "%lu ",elapsed_us);
 //               sprintf(vcom_answer, "%lu ",blockzaehler);
                  sprintf(vcom_answer, "%s%lu %lu %lu %lu %lu ", vcom_answer,result[0],reg0lvdt,reg1lvdt,reg2lvdt,reg3lvdt);
                  if (temp_mess)
                    sprintf(vcom_answer, "%s%.2f %lu %lu %lu %lu ", vcom_answer,gp22_temp,reg0temper,reg1temper,reg2temper,reg3temper);
                  if (calc_std_dev)
                    sprintf(vcom_answer, "%s%u ", vcom_answer,(uint32_t)std_dev);
                 sprintf(vcom_answer, "%s%u\r", vcom_answer,gp22_status);
                }
                else
                {
                  sprintf(vcom_answer, "Timeout\r");
                }
                VCOM_send_string(vcom_answer);
                delay_ms(200);
             }
             //****** 27.10.2017 Daouda
             if(mess_mode==2)            // mess_mode=2
             {
               // Messung starten
               gp22_init_tdc();
               // Reg0 lesen
               ret = gp22_read_result_register(0, &val32);
               if (ret == 0)
                 sprintf((char *)vcom_answer, "%lu", val32);
               else
                 sprintf((char *)vcom_answer, "Timeout ");
               // Statusregister auslesen
               ret = gp22_read_status_register((uint16_t*)&val32);
               if (ret == 0)
                 gp22_status = val32;
                 err += ret;
              // Anwortstring zusammenbauen
               for (j = 1; j < 4; j++)
                {
                  ret = gp22_read_result_register(j, &val32);
                  if (ret == 0)
                   {
                     sprintf((char *)dest, " %lu", val32);
                     strcat((char *)vcom_answer, (char *)dest);
                   }
                  else
                    sprintf((char *)vcom_answer, "Timeout ");
                }

              strcat((char *)vcom_answer, (char *)"\r");
              VCOM_send_string((char *)vcom_answer);
              VCOM_clear_answer();

              delay_ms(200);
             }

             if(mess_mode==3)            // mess_mode=3
             {
                // Messung starten
                //gp22_init_tdc();
                sprintf((char *)vcom_answer, "%lu", CMD_lvdt_R());
                VCOM_send_string((char *)vcom_answer);        // String werden an PC gesendet
                VCOM_clear_answer();

                delay_ms(100);
             }

        }
        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;

    __HAL_RCC_PWR_CLK_ENABLE();

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler */
    /* User can add his own implementation to report the HAL error return state */
    while(1)
    {
    }
    /* USER CODE END Error_Handler */
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
      ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */

}

#endif

/**
  * @}
  */

/**
  * @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
