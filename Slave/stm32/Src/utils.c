/*
 * utils.c
 *
 *  Created on: Jul 21, 2023
 *      Author: furkankirlangic
 */

#include "utils.h"

uint32_t CRC_Calculate(uint8_t *buffer, uint32_t len) {
	uint32_t i, j;
	uint32_t crc, msb;

	crc = 0xFFFFFFFF;	// Crc Initial
	for (i = 0; i < len; i++) {
		crc ^= (((uint32_t) buffer[i]) << 24);
		for (j = 0; j < 8; j++) {
			msb = crc >> 31;
			crc <<= 1;
			crc ^= (0 - msb) & 0x04C11DB7;	// Crc Poly
		}
	}
	return crc;
}


void recoverI2C(I2C_HandleTypeDef* hi2c, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {

	/* Deinit I2C peripheral */
	HAL_I2C_DeInit(hi2c);

	/* Init SCL pin as GPIO */
	GPIO_InitTypeDef SCL_InitStruct = { 0 };
	SCL_InitStruct.Pin = GPIO_Pin;
	SCL_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	SCL_InitStruct.Pull = GPIO_NOPULL;
	SCL_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOx, &SCL_InitStruct);
	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, 1); /* Set SCL line to HIGH */

	for (size_t i = 0; i < 10; i++) {
		HAL_GPIO_WritePin(GPIOx, GPIO_Pin, 1);
		for (int i = 0; i < 50; i++) {
			__NOP();
		}
		HAL_GPIO_WritePin(GPIOx, GPIO_Pin, 0);
		for (int i = 0; i < 50; i++) {
			__NOP();
		}
	}

	HAL_GPIO_DeInit(GPIOx, GPIO_Pin); /* SCL GPIO de-init for I2C re-init*/
	HAL_I2C_Init(hi2c);
}


