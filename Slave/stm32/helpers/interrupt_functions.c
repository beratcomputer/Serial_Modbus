// intterupt functions should be entegrated in slave device.
// should be checked which USART is used and replaced with current one.
// should be checked DMA and its streams and replaced with current ones.

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#include "protocol.h"
#include "device.h"
#include "cmsis_os2.h"

// STM32F4XX UART interrupt function
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
	extern osThreadId_t CommTaskHandle;
	if(LL_USART_IsActiveFlag_IDLE(USART1) && LL_USART_IsEnabledIT_IDLE(USART1)){
		osThreadFlagsSet(CommTaskHandle, RX_AVAIL_FLAG);
		commInterruptRoutine((tCommunication*)&Device.comm, LL_DMA_GetDataLength(DMA2, LL_DMA_STREAM_2));
		LL_USART_ClearFlag_IDLE(USART1);
	}

	if (LL_USART_IsActiveFlag_ORE(USART1)){
		LL_USART_ClearFlag_ORE(USART1);
	}

	if (LL_USART_IsActiveFlag_TC(USART1)){
		LL_USART_ClearFlag_TC(USART1);
	}

  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

// STM32F3XX UART interrupt function
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
	extern osThreadId_t CommTaskHandle;
	if (__HAL_UART_GET_IT(&huart1, UART_IT_IDLE) && __HAL_UART_GET_IT_SOURCE(&huart1, UART_IT_IDLE)) {
		commInterruptRoutine((tCommunication*)&Device.comm, LL_DMA_GetDataLength(DMA2, LL_DMA_STREAM_2));
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
