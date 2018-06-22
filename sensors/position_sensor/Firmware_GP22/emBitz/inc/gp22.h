/*****************************************************************************/
/*                                                                           */
/* gp22.h                                                                    */
/*                                                                           */
/* Routinen zur Ansteuerung des GP22 mit STM32 und SPI                       */
/*                                                                           */
/* (2013) Universitaet Stuttgart / IZFM (Dipl.-Ing.(FH) Klaus Skibowski)     */
/*                                                                           */
/*****************************************************************************/

#ifndef GP22_H_
#define GP22_H_

#define GP22_CONFIG_BUFFER_SIZE			    7

// Registeradressen
#define GP22_CONFIG_REG_0	          				0x00
#define GP22_CONFIG_REG_1	          				0x01
#define GP22_CONFIG_REG_2					          0x02
#define GP22_CONFIG_REG_3	          				0x03
#define GP22_CONFIG_REG_4	          				0x04
#define GP22_CONFIG_REG_5	          				0x05
#define GP22_CONFIG_REG_6	          				0x06

#define GP22_RESULT_REG_0     	            0x00
#define GP22_RESULT_REG_1	    	            0x01
#define GP22_RESULT_REG_2		                0x02
#define GP22_RESULT_REG_3		                0x03
#define GP22_STATUS_REGISTER		            0x04

#define GP22_OPCODE_START_TOF               0x01
#define GP22_OPCODE_START_TEMP              0x02
#define GP22_OPCODE_START_CAL_RES           0x03
#define GP22_OPCODE_START_CAL_TDC           0x04
#define GP22_OPCODE_START_TOF_RESTART       0x05
#define GP22_OPCODE_START_TEMP_RESTART      0x06
#define GP22_OPCODE_POWER_ON_RESET          0x50
#define GP22_OPCODE_INIT_TDC                0x70
#define GP22_OPCODE_CONFIG_TO_EEPROM        0xC0
#define GP22_OPCODE_COMPARE_CONFIG_EEPROM   0xC6
#define GP22_OPCODE_EEPROM_TO_CONFIG        0xF0

extern uint32_t gp22_config_buffer[GP22_CONFIG_BUFFER_SIZE];
uint8_t  gp22_config_reg_nr;
uint8_t  gp22_result_reg_nr;
uint8_t  gp22_div_clkhs;
float    pt1000_temp;

void    gp22_setup();
uint8_t gp22_reset();

uint8_t gp22_start_tof();
uint8_t gp22_start_tof_restart();
uint8_t gp22_start_temp();
uint8_t gp22_init_tdc();
uint8_t gp22_power_on_reset();
uint8_t gp22_write_config_to_eeprom();
uint8_t gp22_transfer_eeprom_to_config();
uint8_t gp22_compare_config_eeprom();
uint8_t gp22_calibrate_tdc();
uint8_t gp22_calibrate_clock();

uint8_t gp22_test_communication();
uint8_t gp22_write_config_register(uint8_t reg_addr, uint32_t reg_data);
//uint8_t gp22_write_configuration(uint32_t *configuration);
uint8_t gp22_read_result_register(uint8_t reg_addr, uint32_t *reg_data);
uint8_t gp22_read_status_register(uint16_t *reg_data);
uint8_t gp22_read_pt1000(float* val, float* rtdVal, uint32_t* reg0temp, uint32_t* reg1temp, uint32_t* reg2temp, uint32_t* reg3temp);//, float* rtdVal, uint32_t* reg0val, uint32_t* reg1val);
uint8_t gp22_write_configuration(uint32_t *configuration, uint8_t size); // Daouda 04.10.2017

#endif
