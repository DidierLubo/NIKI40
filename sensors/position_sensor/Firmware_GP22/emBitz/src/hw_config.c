#include "hw_config.h"

static HAL_StatusTypeDef hal_status;

// Init I2C_SELECT-Pin
void i2c_select_init()
{
    GPIO_InitTypeDef GPIO_InitStruct;

    I2C_SELECT_CLK_ENABLE();

    GPIO_InitStruct.Pin = I2C_SELECT_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;

    HAL_GPIO_Init(I2C_SELECT_PORT, &GPIO_InitStruct);
    SPI_SELECT();
}

// ***** GP22 - Hardware Setup
uint8_t gp22_hw_setup()
{
  GPIO_InitTypeDef GPIO_InitStruct;
  HAL_SPI_StateTypeDef spi_status;


  // Init INT-Pin
  GPIO_InitStruct.Pin = GP22_INT_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(GP22_INT_PORT, &GPIO_InitStruct);
  GP22_INT_CLK_ENABLE();

  // Init CS-Pin
  GPIO_InitStruct.Pin = GP22_CS_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(GP22_CS_PORT, &GPIO_InitStruct);
  GP22_CS_CLK_ENABLE();
  GP22_CS_HIGH();

  // Init RESET-Pin
  GPIO_InitStruct.Pin = GP22_RESET_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(GP22_RESET_PORT, &GPIO_InitStruct);
  GP22_RESET_CLK_ENABLE();
  GP22_RESET_LOW();

  // Init SPI
  hGP22_SPI.Instance = GP22_SPI;
  hGP22_SPI.Init.BaudRatePrescaler = GP22_SPI_PRESCALER;
  hGP22_SPI.Init.Direction = SPI_DIRECTION_2LINES;
  hGP22_SPI.Init.CLKPhase = SPI_PHASE_2EDGE;
  hGP22_SPI.Init.CLKPolarity = SPI_POLARITY_LOW;
  hGP22_SPI.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
  hGP22_SPI.Init.CRCPolynomial = 7;
  hGP22_SPI.Init.DataSize = SPI_DATASIZE_8BIT;
  hGP22_SPI.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hGP22_SPI.Init.NSS = SPI_NSS_SOFT;
  hGP22_SPI.Init.TIMode = SPI_TIMODE_DISABLED;
  hGP22_SPI.Init.Mode = SPI_MODE_MASTER;

  spi_status = HAL_SPI_GetState(&hGP22_SPI);
  if (spi_status != HAL_SPI_STATE_RESET)
  {
    hal_status = HAL_SPI_DeInit(&hGP22_SPI);
  }
  hal_status = HAL_SPI_Init(&hGP22_SPI);

  return hal_status;
}

uint8_t gp22_wait_for_interrupt(uint32_t timeout)
{
//  uint32_t ticks_start=0;
//
//  ticks_start = HAL_GetTick();
//  while ((GP22_INT_IN()>0) && ((HAL_GetTick()-ticks_start) < timeout_ms)){};
//  if ((HAL_GetTick()-ticks_start) > timeout_ms)
//    return 1; // Timeout
//  else
//    return 0;

  while((GP22_INT_IN()>0) && ((--timeout)>0));

  return (timeout==0);
}


uint8_t gp22_spi_reconfig(SPI_HandleTypeDef *hspi)
{
  HAL_SPI_DeInit(hspi);

  hspi->Instance = GP22_SPI;
  hspi->Init.BaudRatePrescaler = GP22_SPI_PRESCALER;
  hspi->Init.Direction = SPI_DIRECTION_2LINES;
  hspi->Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi->Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
  hspi->Init.CRCPolynomial = 7;
  hspi->Init.DataSize = SPI_DATASIZE_8BIT;
  hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi->Init.NSS = SPI_NSS_SOFT;
  hspi->Init.TIMode = SPI_TIMODE_DISABLED;
  hspi->Init.Mode = SPI_MODE_MASTER;

  return HAL_SPI_Init(hspi);
}

