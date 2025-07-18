/*
 * protocol.c
 *
 *  Created on: Feb 24, 2023
 *      Author: beratComputer
 */
#include "protocol.h"
#include "utils.h"

/*TODO: Read baudrate from UART registers.*/

static inline tProtocolStatus commandChecker(volatile tCommunication *pComm);
static inline tProtocolStatus patternFinder(volatile tCommunication *pComm);
static inline tProtocolStatus parseReceivedReg(volatile tCommunication *pComm);
static inline tProtocolStatus parseSyncReceived(volatile tCommunication *pComm);

tProtocolStatus commInit(volatile tCommunication *pComm, uint32_t **ptrLUT,
		uint8_t *sizeLUT, uint8_t parameter_list_length) {
	for (int i = 0; i < MAX_VAR_COUNT; i++) {
		if ((ptrLUT[i] == NULL) || (sizeLUT[i] == 0)) {
			return ProtocolFalse;
		}
	}
	pComm->header = HEADER;
	pComm->deviceFamily = DEVICE_FAMILY;
	pComm->ptrLUT = ptrLUT;
	pComm->sizeLUT = sizeLUT;
	pComm->parameter_size = parameter_list_length;
	pComm->cb = &cb;

	LL_DMA_DisableIT_TC(DMA2, LL_DMA_STREAM_7);

	/* USART 1 TX DMA Config */
	LL_DMA_ConfigAddresses(COMM_DMA_TX, COMM_DMA_TX_STREAM,
			(uint32_t) pComm->txBuffer, (uint32_t) &(USART->DR),
			LL_DMA_GetDataTransferDirection(COMM_DMA_TX, COMM_DMA_TX_STREAM));

	/* USART 1 RX DMA Config */
	LL_DMA_ConfigAddresses(COMM_DMA_RX, COMM_DMA_RX_STREAM,
			LL_USART_DMA_GetRegAddr(USART), (uint32_t) (pComm->cb->Buffer),
			LL_DMA_GetDataTransferDirection(COMM_DMA_RX, COMM_DMA_RX_STREAM));
	LL_DMA_SetDataLength(COMM_DMA_RX, COMM_DMA_RX_STREAM, pComm->cb->size);
	LL_DMA_EnableStream(COMM_DMA_RX, COMM_DMA_RX_STREAM);

	LL_USART_DisableIT_ERROR(USART);
	LL_USART_EnableIT_IDLE(USART);
	LL_USART_EnableIT_TC(USART);

	/* Enable DMA Requests on USART1 RX */
	LL_USART_EnableDMAReq_RX(USART);
	/* Enable DMA Requests on USART1 TX */
	LL_USART_EnableDMAReq_TX(USART);

#ifdef RS485
	/*RS485 Transceiver enabled as receive only.*/
	HAL_GPIO_WritePin(RX_EN_GPIO_Port, RX_EN_Pin, 0);
	HAL_GPIO_WritePin(TX_EN_GPIO_Port, TX_EN_Pin, 1);
#endif

	return ProtocolTrue;
}

tProtocolStatus comm_IDLE_InterruptRoutine(tCommunication *pComm) {
	pComm->cb->writePos = pComm->cb->size
			- LL_DMA_GetDataLength(COMM_DMA_RX, COMM_DMA_RX_STREAM);
	pComm->receiveAvailable = ProtocolTrue;
	return ProtocolTrue;
}
tProtocolStatus comm_TC_InterruptRoutine(tCommunication *pComm) {
	LL_DMA_ClearFlag_TC7(DMA2);
	LL_DMA_DisableStream(COMM_DMA_TX, COMM_DMA_TX_STREAM);
#ifdef RS485
	/*RS485 Transceiver enabled as receive only.*/
	HAL_GPIO_WritePin(RX_EN_GPIO_Port, RX_EN_Pin, 0);
	HAL_GPIO_WritePin(TX_EN_GPIO_Port, TX_EN_Pin, 1);
#endif
}

tProtocolStatus processReceived(volatile tCommunication *pComm) {
	if (patternFinder(pComm) == ProtocolTrue) {

		peekCircularBuffer((tCircularBuffer*) (pComm->cb),
				(uint8_t*) (pComm->ptrLUT[iPackageSize_protocol]),
				iPackageSize_protocol);
		if (*(pComm->ptrLUT[iPackageSize_protocol])
				> bufferLength((tCircularBuffer*) (pComm->cb))) {
			return (ProtocolInvalid);
		}
	} else {
		return (ProtocolInvalid);
	}

	/* Check package size and read whole block if size is valid */
	if ((pComm->packageSize >= CONSTANT_REG_SIZE)
			&& (pComm->packageSize < MAX_PACKAGE_SIZE)) {
		if (readCircularBufferBlock((tCircularBuffer*) (pComm->cb),
				(uint8_t*) pComm->rxBuffer, pComm->packageSize) != BufferTrue) {
			return ProtocolFalse;
		}
	} else {
		pComm->cb->readPos = pComm->cb->writePos; //TODO circular_buffer function for package size error.
		return ProtocolInvalid;
	}

	/* Check CRC and parse if valid */
	pComm->CRCValue = *((uint32_t*) (pComm->rxBuffer + pComm->packageSize
			- sizeof(pComm->CRCValue)));
	if (CRC_Calculate((uint8_t*) pComm->rxBuffer,
			pComm->packageSize - sizeof(pComm->CRCValue)) == pComm->CRCValue) {
		if (pComm->deviceID == BATCH_ID) {
			pComm->rxBuffer[iCommand_protocol] &= (uint8_t) ~ACK;
		}
		return (commandChecker(pComm));

	} else {
		return ProtocolFalse;
	}

	return ProtocolFalse;
}

tProtocolStatus processTransmit(volatile tCommunication *pComm) {
	tProtocolStatus status = ProtocolFalse;
	status = generateTransmitPackage(pComm);
	if (status == ProtocolTrue) {
#ifdef RS485	/*RS485 Transceiver enabled as transmit only.*/
		HAL_GPIO_WritePin(TX_EN_GPIO_Port, TX_EN_Pin, 1);
#endif
		LL_DMA_DisableStream(COMM_DMA_TX, COMM_DMA_TX_STREAM);
		LL_DMA_SetDataLength(COMM_DMA_TX, COMM_DMA_TX_STREAM,
				(uint32_t) pComm->txBuffer[iPackageSize_protocol]);
		LL_DMA_EnableStream(COMM_DMA_TX, COMM_DMA_TX_STREAM);
	} else {
		pComm->transmitAvailable = ProtocolFalse;
	}
}

tProtocolStatus generateTransmitPackage(volatile tCommunication *pComm) {
	uint8_t generatedSize = CONSTANT_REG_SIZE - 4;  //Substract CRC size

	//Pre-populate constants
	for (tProtocolParametersIndex i = iHeader_protocol; i <= iStatus_protocol;
			i++) {
		pComm->txBuffer[i] = *(uint8_t*) pComm->ptrLUT[i];
	}

	switch ((tCommand) (pComm->command)) {
	case PING:
		break;
	case ACK:
	case EEPROM_WRITE_ACK:
	case (WRITE_ACK):
		return ProtocolFalse;
		break;
	case READ:
		for (uint8_t i = 0; i < pComm->customPackageIndexCount; i++) {
			if (pComm->customPackageIndex[i] < pComm->parameter_size) {
				pComm->txBuffer[generatedSize++] = pComm->customPackageIndex[i];
				memcpy((uint8_t*) pComm->txBuffer + generatedSize,
						pComm->ptrLUT[pComm->customPackageIndex[i]],
						pComm->sizeLUT[pComm->customPackageIndex[i]]);
				generatedSize += pComm->sizeLUT[pComm->customPackageIndex[i]];
			}
		}
		break;

	default:
		return ProtocolFalse;
		break;
	}

	pComm->txBuffer[iPackageSize_protocol] = generatedSize + 4;  //Add CRC size

	if (pComm->txBuffer[iPackageSize_protocol] <= MAX_PACKAGE_SIZE) {
		//Add CRC to the end
		pComm->CRCValue = CRC_Calculate((uint8_t*) pComm->txBuffer,
				generatedSize);
		*(uint32_t*) (pComm->txBuffer + generatedSize) = pComm->CRCValue;
	} else {
		pComm->errorCount++;
		return ProtocolFalse;
	}

	return ProtocolTrue;
}

tProtocolStatus generateCustomTransmitPackage(volatile tCommunication *pComm,
		uint8_t *pData, uint8_t size) {

	uint8_t generatedSize = CONSTANT_REG_SIZE + size - 4;

	for (tProtocolParametersIndex i = iHeader_protocol; i <= iStatus_protocol;
			i++) {
		pComm->txBuffer[i] = *(uint8_t*) pComm->ptrLUT[i];
	}

	for (size_t i = 0; i < size; i++) {
		pComm->txBuffer[DATA(i)] = pData[i];
	}

	pComm->txBuffer[iPackageSize_protocol] = generatedSize + 4;

	if (pComm->txBuffer[iPackageSize_protocol] <= MAX_PACKAGE_SIZE) {
		pComm->CRCValue = CRC_Calculate((uint8_t*) pComm->txBuffer,
				generatedSize);
		*(uint32_t*) (pComm->txBuffer + generatedSize) = pComm->CRCValue;
	} else {
		pComm->errorCount++;
		return ProtocolFalse;
	}

	return ProtocolTrue;
}

static inline tProtocolStatus patternFinder(volatile tCommunication *pComm) {

	for (uint16_t i = 0; i < COMM_BUFFER_LENGTH; i++) {
		uint8_t tempArr[3];

		if (peekCircularBuffer((tCircularBuffer*) pComm->cb, &(tempArr[0]),
				iHeader_protocol) == BufferTrue) {
			if (tempArr[0] == pComm->header) {
				if (peekCircularBuffer((tCircularBuffer*) pComm->cb,
						&(tempArr[1]), iDeviceID_protocol) == BufferTrue) {
					if ((tempArr[1]
							== *(uint8_t*) (pComm->ptrLUT[iDeviceID_protocol]))
							|| (tempArr[1] == BATCH_ID)) {
						if (peekCircularBuffer((tCircularBuffer*) pComm->cb,
								&(tempArr[2]), iDeviceFamily_protocol)
								== BufferTrue) {
							if (tempArr[2] == pComm->deviceFamily) {
								return (ProtocolTrue);
							} else {
								readCircularBuffer((tCircularBuffer*) pComm->cb,
										&(tempArr[2]));
							}
						} else {
							break;
						}

					} else {
						readCircularBuffer((tCircularBuffer*) pComm->cb,
								&(tempArr[1]));
					}
				} else {
					break;
				}
			} else {
				readCircularBuffer((tCircularBuffer*) pComm->cb, &(tempArr[0]));
			}
		} else {
			break;
		}
	}
	return (ProtocolFalse);
}

static inline tProtocolStatus parseReceivedReg(volatile tCommunication *pComm) {

	for (uint8_t i = 0; i < pComm->packageSize - CONSTANT_REG_SIZE; i++) {
		//Check if received index is valid
		uint8_t varIndex = pComm->rxBuffer[DATA(i)];
		if ((varIndex >= iWritableStart_protocol)
				|| (varIndex == iDeviceID_protocol)) {
			memcpy(pComm->ptrLUT[varIndex],
					(uint8_t*) pComm->rxBuffer + DATA(i + 1),
					pComm->sizeLUT[varIndex]);
			i += pComm->sizeLUT[varIndex];
		} else {
			return ProtocolFalse;
		}
	}

	return ProtocolTrue;
}

static inline tProtocolStatus parseSyncReceived(volatile tCommunication *pComm) {

	uint8_t varIndex = pComm->rxBuffer[DATA(0)];
	for (uint8_t i = 1; i < pComm->packageSize - CONSTANT_REG_SIZE + 1; i++) {
		//Check if received index is valid
		uint8_t deviceIndex = pComm->rxBuffer[DATA(i)];
		if (deviceIndex == (uint8_t) *pComm->ptrLUT[iDeviceID_protocol]) {
			memcpy(pComm->ptrLUT[varIndex],
					(uint8_t*) pComm->rxBuffer + DATA(i + 1),
					pComm->sizeLUT[varIndex]);
			return ProtocolTrue;
		} else {
			i += pComm->sizeLUT[varIndex];
		}
	}

	return ProtocolFalse;
}

static inline tProtocolStatus parseReadIndex(volatile tCommunication *pComm) {
	uint8_t valid_index_count = 0;
	pComm->customPackageIndexCount = valid_index_count;
	for (uint8_t i = 0; i < pComm->packageSize - CONSTANT_REG_SIZE; i++) {
		//Check if received index is valid
		uint8_t varIndex = pComm->rxBuffer[DATA(i)];
		if (varIndex < pComm->parameter_size) {
			pComm->customPackageIndex[valid_index_count++] = varIndex;
		} else {
			return ProtocolFalse;
		}
	}
	pComm->customPackageIndexCount = valid_index_count;
	return ProtocolTrue;
}

static inline tProtocolStatus commandChecker(volatile tCommunication *pComm) {

	switch (pComm->rxBuffer[iCommand_protocol] & ~ACK) {

	case PING:
		if (pComm->rxBuffer[iDeviceID_protocol] != BATCH_ID) {
			pComm->command = PING;
			return (ProtocolTrue);
		} else {
			return (ProtocolFalse); /*Can not ping with batch ID;*/
		}
	case WRITE:
		pComm->command = WRITE;
		return parseReceivedReg(pComm);

	case WRITE_SYNC:
		pComm->command = WRITE_SYNC;
		return parseSyncReceived(pComm);

	case READ:
		pComm->command = READ;
		return parseReadIndex(pComm);

	case EEPROM_WRITE:
		pComm->command = EEPROM_WRITE;
		return ProtocolTrue;

	case HARD_RESET:
		pComm->command = HARD_RESET;
		return ProtocolTrue;

	case BL_JUMP:
		pComm->command = BL_JUMP;
		return ProtocolTrue;

	case REBOOT:
		pComm->command = REBOOT;
		return ProtocolTrue;

	case TUNE:
		pComm->command = TUNE;
		return ProtocolTrue;

	case ERROR_CLEAR:
		pComm->command = ERROR_CLEAR;
		return ProtocolTrue;

	default:
		return (ProtocolFalse);
	}
	return (ProtocolFalse);
}
