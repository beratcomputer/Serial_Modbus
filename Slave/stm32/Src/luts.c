/*
 * luts.c
 *
 *  Created on: March 23, 2025
 *      Author: beratComputer
 */

#include "device.h"
#include "stdint.h"
#include "luts.h"

#define ADDR(X) ((uint32_t*)&(X))


uint32_t *ptrLUT[] = {
    ADDR(Device.comm.header),
    ADDR(Device.comm.deviceID),
    ADDR(Device.comm.deviceFamily),
    ADDR(Device.comm.packageSize),
    ADDR(Device.comm.command),
	ADDR(Device.status),
	ADDR(Device.hwVersion),
	ADDR(Device.swVersion),
	ADDR(Device.baudrate),
	// user begin for parameters

	// user end for parameters
    ADDR(Device.comm.CRCValue),
};


uint8_t sizeLUT[] = {
    sizeof(Device.comm.header),
    sizeof(Device.comm.deviceID),
    sizeof(Device.comm.deviceFamily),
    sizeof(Device.comm.packageSize),
    sizeof(Device.comm.command),
	sizeof(Device.status),
	sizeof(Device.hwVersion),
	sizeof(Device.swVersion),
	sizeof(Device.baudrate),
	// user begin for parameters

	// user end for parameters
    sizeof(Device.comm.CRCValue),
};
