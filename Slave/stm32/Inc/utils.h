/*
 * utils.h
 *
 *  Created on: Jul 21, 2023
 *      Author: furkankirlangic
 */

#ifndef INC_UTILS_H_
#define INC_UTILS_H_

#include "stdint.h"
#include "i2c.h"

uint32_t CRC_Calculate(uint8_t *buffer, uint32_t len);
void recoverI2C(I2C_HandleTypeDef* hi2c, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

#endif /* INC_UTILS_H_ */
