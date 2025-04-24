/*
 * comm_config.h
 *
 *  Created on: Apr 24, 2025
 *      Author: beratComputer
 */

#ifndef INC_COMM_CONFIG_H_
#define INC_COMM_CONFIG_H_

#include "main.h"


/////////////////////////// DMA ///////////////////////////

#define COMM_DMA_RX				DMA2
#define COMM_DMA_RX_STREAM		LL_DMA_STREAM_2
#define COMM_DMA_TX				DMA2
#define COMM_DMA_TX_STREAM		LL_DMA_STREAM_7

#ifndef COMM_DMA_RX
	#error "COMM_DMA_RX is not defined!"
#endif

#ifndef COMM_DMA_RX_STREAM
	#error "COMM_DMA_RX_STREAM is not defined!"
#endif

#ifndef COMM_DMA_TX
	#error "COMM_DMA_TX is not defined!"
#endif

#ifndef COMM_DMA_TX_STREAM
	#error "COMM_DMA_TX_STREAM is not defined!"
#endif


/////////////////////////// RS485 ///////////////////////////
#define RS485 // if RS485 is used leave here as defined.

#ifdef RS485

#ifndef RX_EN_GPIO_Port
	#error "RX_EN_GPIO_Port is not defined!"
#endif

#ifndef RX_EN_Pin
	#error "RX_EN_GPIO_Port is not defined!"
#endif

#ifndef TX_EN_GPIO_Port
	#error "RX_EN_GPIO_Port is not defined!"
#endif

#ifndef TX_EN_Pin
	#error "RX_EN_GPIO_Port is not defined!"
#endif

#endif






#endif /* INC_COMM_CONFIG_H_ */
