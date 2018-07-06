#ifndef HW_CONFIG_H_INCLUDED
#define HW_CONFIG_H_INCLUDED

#include "stm32f4xx_hal.h"
#include <math.h>

#define VER_MAJ		  1		// Versionsnummer major
#define VER_MIN		  0		// Versionsnummer minor
#define VER_BUILD   0   // Build-Nummer

#define MCU_BOARD                           "Board=STM32H405_REV3"

#define M_PI                                3.14159265358979323846

#define SENSOR_INTERFACE_NOT_CONFIGURED     255
#define SENSOR_INTERFACE_SPI                0
#define SENSOR_INTERFACE_I2C                1
#define SENSOR_INTERFACE_RS232              2
#define SENSOR_INTERFACE_RS485              3
#define SENSOR_INTERFACE_CAN                4
#define SENSOR_INTERFACE_HP34401A_V         5
#define SENSOR_INTERFACE_HP34401A_I         6

// ***** USB-Disconnect
#define USB_DISCONNECT_PIN                  GPIO_PIN_11
#define USB_DISCONNECT_PORT                 GPIOC
#define USB_DISCONNECT_CLK_ENABLE()         __HAL_RCC_GPIOC_CLK_ENABLE()
#define USB_DISCONNECT_HIGH()               HAL_GPIO_WritePin(USB_DISCONNECT_PORT, USB_DISCONNECT_PIN, GPIO_PIN_SET)
#define USB_DISCONNECT_LOW()                HAL_GPIO_WritePin(USB_DISCONNECT_PORT, USB_DISCONNECT_PIN, GPIO_PIN_RESET)

// ***** Bus-Selektor
#define I2C_SELECT_PIN                      GPIO_PIN_13
#define I2C_SELECT_PORT                     GPIOC
#define I2C_SELECT_CLK_ENABLE()             __HAL_RCC_GPIOC_CLK_ENABLE()
#define I2C_SELECT()                        HAL_GPIO_WritePin(I2C_SELECT_PORT, I2C_SELECT_PIN, GPIO_PIN_SET)
#define SPI_SELECT()                        HAL_GPIO_WritePin(I2C_SELECT_PORT, I2C_SELECT_PIN, GPIO_PIN_RESET)

// ***** GP22-Konfiguration
#define GP22_SPI                            SPI1
#define GP22_SPI_TIMEOUT                    1000
#define GP22_SPI_PRESCALER                  SPI_BAUDRATEPRESCALER_32 // 2.625 MHz

#define GP22_INT_PIN                        GPIO_PIN_2
#define GP22_INT_PORT                       GPIOC
#define GP22_INT_CLK_ENABLE()               __HAL_RCC_GPIOC_CLK_ENABLE()
#define GP22_INT_IN()                       HAL_GPIO_ReadPin(GP22_INT_PORT, GP22_INT_PIN)

#define GP22_CS_PIN                         GPIO_PIN_4
#define GP22_CS_PORT                        GPIOA
#define GP22_CS_CLK_ENABLE()                __HAL_RCC_GPIOA_CLK_ENABLE()
#define GP22_CS_HIGH()                      HAL_GPIO_WritePin(GP22_CS_PORT, GP22_CS_PIN, GPIO_PIN_SET)
#define GP22_CS_LOW()                       HAL_GPIO_WritePin(GP22_CS_PORT, GP22_CS_PIN, GPIO_PIN_RESET)

#define GP22_RESET_PIN                      GPIO_PIN_3
#define GP22_RESET_PORT                     GPIOC
#define GP22_RESET_CLK_ENABLE()             __HAL_RCC_GPIOC_CLK_ENABLE()
#define GP22_RESET_HIGH()                   HAL_GPIO_WritePin(GP22_RESET_PORT, GP22_RESET_PIN, GPIO_PIN_SET)
#define GP22_RESET_LOW()                    HAL_GPIO_WritePin(GP22_RESET_PORT, GP22_RESET_PIN, GPIO_PIN_RESET)

SPI_HandleTypeDef  hGP22_SPI;

volatile uint32_t systick_timeout_ms;
volatile uint32_t systick_counter_ms;

uint8_t gp22_use_int;

void    i2c_select_init();
uint8_t gp22_hw_setup();
uint8_t gp22_wait_for_interrupt(uint32_t timeout);
uint8_t gp22_spi_reconfig(SPI_HandleTypeDef *hspi);


#endif /* HW_CONFIG_H_INCLUDED */
