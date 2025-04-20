// intterupt functions should be entegrated in slave device.
// should be checked which USART is used and replaced with current one.
// should be checked DMA and its streams and replaced with current ones.

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

// STM32F4XX UART interrupt function
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */
	extern osThreadId_t CommTaskHandle;
	if (LL_USART_IsActiveFlag_IDLE(USART2) && LL_USART_IsEnabledIT_IDLE(USART2)) {
		osThreadFlagsSet(CommTaskHandle, RX_AVAIL_FLAG);
		Device.comm.cb->writePos = Device.comm.cb->size - LL_DMA_GetDataLength(DMA1, LL_DMA_STREAM_5);
		Device.comm.receiveAvailable = ProtocolTrue;
		LL_USART_ClearFlag_IDLE(USART2);
	}

	if (LL_USART_IsActiveFlag_ORE(USART2)) {
		Device.status |= 148;
		LL_USART_ClearFlag_ORE(USART2);
	}

  /* USER CODE END USART2_IRQn 0 */
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

// STM32F3XX UART interrupt function
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
	extern osThreadId_t CommTaskHandle;
	if (__HAL_UART_GET_IT(&huart1, UART_IT_IDLE) && __HAL_UART_GET_IT_SOURCE(&huart1, UART_IT_IDLE)) {
		Device.comm.cb->writePos = actp.comm.cb->size - LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_5);
		__HAL_UART_CLEAR_IDLEFLAG(&huart1);
		osThreadFlagsSet(CommTaskHandle, RX_AVAIL_FLAG);
	}
  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
