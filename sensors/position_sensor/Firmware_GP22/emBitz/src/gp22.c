/*****************************************************************************/
/*                                                                           */
/* gp22.c                                                                    */
/*                                                                           */
/* Routinen zur Ansteuerung des GP22 mit STM32-HAL und SPI                   */
/*                                                                           */
/* (2017) Universitaet Stuttgart / IFM (Dipl.-Ing.(FH) Klaus Skibowski)      */
/*                                                                           */
/*****************************************************************************/
#include "hw_config.h"
#include "gp22.h"
#include "utils.h"
#include <math.h>

static HAL_StatusTypeDef hal_status=0;

uint32_t gp22_config_buffer[GP22_CONFIG_BUFFER_SIZE] =
{
  0x03C5EA01,  //0x03C5E801,
  0x21420000,
  0x20000000,
  0x18000000,
  0x10000000,
  0x40000000,
  0x00006000
    //** 26.10.17 Daouda
//    0xF3076800, //Basic TDC-GP22 configuration example for a single hit application
//    0x21420000, //of the device in measure mode 2. It executes a delay time
//    0x28000000,  //0x28000000//measurement in the range between 500ns to 4 ms, triggered by a start
//    0x10000000, //event on start input and a stop event on digital stop 1 input
//    0x10000000,
//    0x40000000,
//    0x40004000
};

/* PT1000 --------------------------------------------------------------------*/
//R0, A, B are parameters as specified in EN60 751
float     R0 = 1000;                  // R0 is the RTD resistance at 0 °C
float     Coeff_A  = 3.9083/1000;     // A = 3,9083 x 10-3 °C-1
float     Coeff_B  = -5.775/10000000; // B = -5,775 x 10-7 °C-1
float     R1 = 0, R2 = 0;             // R1, R2 is resistance measured in Ohm
float     corr_fact = 1.0000;         // Corr.-factor for temperature resistance ratio
float     gain_factor = 0.99555;      // interpolierter Gain Factor bei 3,3 V
float     ma = -5.775 / 10000;        // Coeff_B * R0
float     mb = 3.9083;                // Coeff_A * R0
float     mc;


uint8_t gp22_send_opcode8(uint8_t opcode)
{
  GP22_CS_LOW();
  hal_status = HAL_SPI_Transmit(&hGP22_SPI, (uint8_t*) &opcode, 1, GP22_SPI_TIMEOUT);
  GP22_CS_HIGH();

  return hal_status;
}

uint8_t gp22_read_result_register(uint8_t reg_addr, uint32_t *reg_data)
{
	uint8_t buf[5];
	uint8_t tmp[1];

  //tmp[0] = (uint8_t)(0xB0 + (reg_addr & 0x03)); //(reg_addr & 0x03) DAOUDA/KIBLE 2.10.2017
  tmp[0] = (uint8_t)(0xB0 + (reg_addr & 0x07)); //  test here

  GP22_CS_LOW();
  hal_status = HAL_SPI_TransmitReceive(&hGP22_SPI, tmp, buf, 5, GP22_SPI_TIMEOUT);
  GP22_CS_HIGH();
  *reg_data = (((uint32_t)buf[4])<<0) + (((uint32_t)buf[3])<<8) + (((uint32_t)buf[2])<<16) + (((uint32_t)buf[1])<<24); // reihenfolge getauscht v.kible 9.11.2017

  return hal_status;
}

uint8_t gp22_read_status_register(uint16_t *reg_data)
{
	uint8_t buf[3];
	uint8_t tmp[1];

  tmp[0] = (uint8_t)(0xB4);

  GP22_CS_LOW();
  hal_status = HAL_SPI_TransmitReceive(&hGP22_SPI, tmp, buf, 3, GP22_SPI_TIMEOUT);
  GP22_CS_HIGH();
  *reg_data = (buf[2]<<0) + (buf[1]<<8);

  return hal_status;
}

uint8_t gp22_write_config_register(uint8_t reg_addr, uint32_t reg_data)
{
  uint8_t tmp[5];

  tmp[0] = (uint8_t)(0x80 + (reg_addr & 0x07));
  tmp[1] = (uint8_t)(reg_data>>24);
  tmp[2] = (uint8_t)( (reg_data>>16) & 0xFF );
  tmp[3] = (uint8_t)( (reg_data>>8) & 0xFF );
  tmp[4] = (uint8_t)(  reg_data & 0xFF );

  GP22_CS_LOW();
  hal_status = HAL_SPI_Transmit(&hGP22_SPI, tmp, 5, GP22_SPI_TIMEOUT);
  GP22_CS_HIGH();

  return hal_status;
}

uint8_t gp22_write_configuration(uint32_t *configuration, uint8_t size) // Daouda 04.10.2017
{
	uint8_t i = 0;
	uint8_t ret = 0;

	ret = gp22_power_on_reset();
	if (ret>0)
		return ret;
	delay_us(500);
	i = 0;
	while ((i < GP22_CONFIG_BUFFER_SIZE) && (ret == 0))
	{
		ret = gp22_write_config_register(i, configuration[i]);
		i++;
		delay_us(100);
	}

	i = (uint8_t)((configuration[0] >> 20) & 0x03);
    gp22_div_clkhs = (1 << i);

	return ret;
}

uint8_t gp22_start_tof()
{
	return gp22_send_opcode8(GP22_OPCODE_START_TOF);
}

uint8_t gp22_start_temp()
{
	return gp22_send_opcode8(GP22_OPCODE_START_TEMP);
}

uint8_t gp22_start_cal_res()
{
	return gp22_send_opcode8(GP22_OPCODE_START_CAL_RES);
}

uint8_t gp22_start_cal_tdc()
{
	return gp22_send_opcode8(GP22_OPCODE_START_CAL_TDC);
}

uint8_t gp22_start_tof_restart()
{
	uint8_t ret=0;

	ret = gp22_send_opcode8(GP22_OPCODE_START_TOF_RESTART);
	if (ret>0)
		return ret;
	ret = gp22_wait_for_interrupt(10000000);

	return ret;
}

uint8_t gp22_start_temp_restart()
{
	return gp22_send_opcode8(GP22_OPCODE_START_TEMP_RESTART);
}

uint8_t gp22_power_on_reset()
{
	return gp22_send_opcode8(GP22_OPCODE_POWER_ON_RESET);
}

uint8_t gp22_init_tdc()
{
	uint8_t ret=0;
	ret = gp22_send_opcode8(GP22_OPCODE_INIT_TDC);

	if (ret>0)
		return ret;
	ret = gp22_wait_for_interrupt(10000000);

	return ret;
}

uint8_t gp22_write_config_to_eeprom()
{
	return gp22_send_opcode8(GP22_OPCODE_CONFIG_TO_EEPROM);
}

uint8_t gp22_transfer_eeprom_to_config()
{
	return gp22_send_opcode8(GP22_OPCODE_EEPROM_TO_CONFIG);
}

uint8_t gp22_compare_config_eeprom()
{
	return gp22_send_opcode8(GP22_OPCODE_COMPARE_CONFIG_EEPROM);
}



//
/*****************************************************************************/
/*                                                                           */
/* gp22_test_communication                                                   */
/*                                                                           */
/* Kommunikationstest                                                        */
/*                                                                           */
/* Rueckgabewert: Fehlernummer der Uebertragung,     0: keine Fehler         */
/*                                                   n: Fehlernummer         */
/*                                                                           */
/*****************************************************************************/
uint8_t gp22_test_communication()
{
  uint8_t  byte1=0xC5;
  uint8_t  byte2=0;
  uint32_t temp=0;

  if (gp22_write_config_register(0x01, ( (uint32_t)byte1 )<<24 )!=0)
	  return 1;
  delay_ms(10);
  if (gp22_read_result_register(0x05, &temp)!=0)
	  return 2;
  byte2 = (uint8_t)(temp>>24);
  if (byte1!=byte2)
	  return 3;
  delay_ms(10);

  return 0;
}

uint8_t gp22_reset()
{
	uint8_t ret = 0;

	GP22_RESET_LOW();
	delay_us(500);
	GP22_RESET_HIGH();

	delay_ms(100);
	ret = gp22_power_on_reset();
	delay_us(500);

	return ret;
}

uint8_t gp22_calibrate_clock()
{
	uint8_t ret = 0;

	ret = gp22_start_cal_res();
	if (ret>0)
		return ret;
	ret = gp22_wait_for_interrupt(10000000);

	return ret;
}

uint8_t gp22_calibrate_tdc()
{
	uint8_t ret = 0;

	ret = gp22_start_cal_tdc();
	if (ret>0)
		return ret;
	ret = gp22_wait_for_interrupt(10000000);

	return ret;
}

uint8_t gp22_read_pt1000(float* val, float* rtdVal, uint32_t* reg0temp, uint32_t* reg1temp, uint32_t* reg2temp, uint32_t* reg3temp) //, float* rtdVal, uint32_t* reg0val, uint32_t* reg1val)
{
	uint8_t  ret=0;
	uint32_t res0=0;
	uint32_t res1=0;
	uint32_t res2=0;
	uint32_t res3=0;

	float    RTD = 0;
	float    temp=0;

	ret = gp22_send_opcode8(GP22_OPCODE_START_TEMP);

	if (ret>0)
		return ret;
	ret = gp22_wait_for_interrupt(10000000);
	if (ret>0)
		return ret;

	ret = gp22_read_result_register(GP22_RESULT_REG_0, &res0);
	if (ret>0)
		return ret;

	ret = gp22_read_result_register(GP22_RESULT_REG_1, &res1);
	if (ret>0)
		return ret;

	ret = gp22_read_result_register(GP22_RESULT_REG_2, &res2);
	if (ret>0)
		return ret;

	ret = gp22_read_result_register(GP22_RESULT_REG_3, &res3);
	if (ret>0)
		return ret;

    *reg0temp = res0;
    *reg1temp = res1;
    *reg2temp = res2;
    *reg3temp = res3;

    RTD = gain_factor*R0*(float)res1/(float)res0;
    //*rtdVal = RTD;
    mc = R0 - RTD;
//  temp = (-R0*Coeff_A + sqrt(R0*R0*Coeff_A*Coeff_A - 4*R0*Coeff_B*(R0-RTD)))/(2*R0*Coeff_B);
    temp = ( -mb + sqrt( mb*mb - 4.0*ma*mc ) )/( 2*ma );
    *val = temp;

    return 0;
}




