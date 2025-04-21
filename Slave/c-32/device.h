/*
 * device.h
 *
 *  Created on: Apr 19, 2025
 *      Author: Lenovo
 */

#ifndef DEVICE_DEVICE_H_
#define DEVICE_DEVICE_H_

#include "align.h"
#include "protocol.h"

 typedef struct{
	 uint8_t status;
	 uint32_t hwVersion;
	 uint32_t swVersion;
	 uint32_t baudrate;
     tCommunication comm;
     uint32_t **ptrLUT;
     uint8_t *sizeLUT;
     // user start



     // user end
}tDevice;

extern tDevice Device;

#endif /* DEVICE_DEVICE_H_ */
