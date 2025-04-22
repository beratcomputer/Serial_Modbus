// This is FreeRTOS communication Task example. 
// Communication entegration can be done like this in slave device. 

void StartCommTask(void *argument)
{
  /* USER CODE BEGIN StartCommTask */
  /* Infinite loop */

	// init for communications.
	if (commInit(&Device.comm, ptrLUT, sizeLUT) != ProtocolTrue) {
			//driver.Telemetry.status |= ST_COMM_ERR;
		}

	/* DMA config for UART1 RX */
	LL_DMA_SetPeriphAddress(DMA2, LL_DMA_STREAM_2, LL_USART_DMA_GetRegAddr(USART1));
	LL_DMA_SetMemoryAddress(DMA2, LL_DMA_STREAM_2, (uint32_t) Device.comm.cb->Buffer);
	LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_2, COMM_BUFFER_LENGTH);

	/* DMA config for UART1 TX */
	LL_DMA_ConfigAddresses(DMA2, LL_DMA_STREAM_7, (uint32_t) Device.comm.txBuffer, LL_USART_DMA_GetRegAddr(USART1), LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
	LL_DMA_EnableIT_TC(DMA2, LL_DMA_STREAM_7);

	/*RS485 Transceiver enabled as receive only.*/
	HAL_GPIO_WritePin(RX_EN_GPIO_Port, RX_EN_Pin, 0);
	HAL_GPIO_WritePin(TX_EN_GPIO_Port, TX_EN_Pin, 0);

	/* Enable DMA stream for USART RX and set to receive state */
	/* Activate transfer complete IT for RS485 transmit pin control */
	LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_2);
	LL_USART_EnableIT_TC(USART1);
	LL_USART_EnableDMAReq_RX(USART1);
	LL_USART_EnableDMAReq_TX(USART1);

  for(;;)
  {
	  osThreadFlagsWait(RX_AVAIL_FLAG, osFlagsWaitAny, osWaitForever);
	if (processReceived(&Device.comm) == ProtocolTrue) {
		switch (Device.comm.command) {
			case PING:
			case READ:
				Device.comm.transmitAvailable = ProtocolTrue;
				break;

			case EEPROM_WRITE:
				//osThreadFlagsSet(EEPROMTaskHandle, TF_EEPROM_SAVE);
				//osThreadSuspend(CommTaskHandle);
				break;

			case REBOOT:
				HAL_NVIC_SystemReset();
				while (1)
					;
				break;

			case TUNE:
				//osEventFlagsSet(TunerEFHandle, EF_TUNE_EN);
				break;

			case HARD_RESET:
				//osThreadFlagsSet(EEPROMTaskHandle, TF_EEPROM_RESET);
				break;

			case BL_JUMP:
				//HAL_GPIO_WritePin(USART1_TX_EN_GPIO_Port, USART1_TX_EN_Pin, 1);
				//Bootloader();
				break;

			case ERROR_CLEAR:
			case WRITE:
			case EEPROM_WRITE_ACK:
			default:
				Device.comm.transmitAvailable = ProtocolFalse;
				break;
	}


	if (Device.comm.cb->writePos > Device.comm.cb->readPos){
		  // restart the task.
		  osThreadFlagsSet(CommTaskHandle, RX_AVAIL_FLAG);
	}

	}
    osDelay(1);
  }
  /* USER CODE END StartCommTask */
}