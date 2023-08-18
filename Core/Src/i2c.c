/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "i2c.h"

/* USER CODE BEGIN 0 */
#define MCP4716_I2C_ADDRESS 0xC4U //I2C_Slave_ADDRESS 0x62 << 1
#define MCP4716_VRL 3.3
#define COMMAND_VOLATILE_DAC_REGISTER (0x00U)>>4
#define COMMAND_VOLATILE_WRITE 0x40U
#define COMMAND_WRITE_ALL_MEMORY 0x60U
#define COMMAND_VOLATILE_CONFIGURATION 0x80U
#define GENERAL_CALL_RESET 0x06U
#define GENERAL_CALL_WAKRUP	0x09U
/* USER CODE END 0 */

I2C_HandleTypeDef hi2c1;

/* I2C1 init function */
void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspInit 0 */

  /* USER CODE END I2C1_MspInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C1 clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();
  /* USER CODE BEGIN I2C1_MspInit 1 */

  /* USER CODE END I2C1_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{

  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspDeInit 0 */

  /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();

    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);

  /* USER CODE BEGIN I2C1_MspDeInit 1 */

  /* USER CODE END I2C1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void MCP4716_Start(void)
{
	 MCP4716_Configuration(GENERAL_CALL_RESET); // General gall reset 0x06U
	 HAL_Delay(8);
	 MCP4716_Configuration(GENERAL_CALL_WAKRUP);// General gall wake up 0x09U
	 HAL_Delay(10);
	 set_voltage(3.3);

}

void MCP4716_Configuration(uint8_t config)
{
	uint8_t cmd1 = config;
	if (HAL_I2C_Master_Transmit(&hi2c1, MCP4716_I2C_ADDRESS, &cmd1, 1, HAL_MAX_DELAY) != HAL_OK)
	{
		Error_Handler(); // Implement your error handling function.
	}
}

void MCP4716_WriteData(uint16_t value_data)
{
    // Break the 10-bit value into two bytes: MSB and LSB
    uint8_t msb = (value_data >> 2) & 0xFF;
    uint8_t lsb = (value_data & 0x03) << 6;  // The last two bits of the DAC code should be 0.
    uint8_t cmd = COMMAND_VOLATILE_WRITE;
    uint8_t data[3] = {cmd, msb, lsb};
    if (HAL_I2C_Master_Transmit(&hi2c1, MCP4716_I2C_ADDRESS, data, 3, HAL_MAX_DELAY) != HAL_OK)
    {
        Error_Handler(); // Implement your error handling function.
    }
}

void set_voltage(float value_volt)
{

	uint16_t DAC_code = (value_volt/MCP4716_VRL) * 1024;
	MCP4716_WriteData(DAC_code);
}
/* USER CODE END 1 */
