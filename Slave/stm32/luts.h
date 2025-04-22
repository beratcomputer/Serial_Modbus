/*
 * luts.h
 *
 *  Created on: Apr 19, 2025
 *      Author: Lenovo
 */

#ifndef DEVICE_LUTS_H_
#define DEVICE_LUTS_H_

// enter number of parameters instead of 0 (zero)
#define PARAMETER_SIZE	(10 + 0)

typedef enum{
    // Default Parameters //
    iHeader,
    iDeviceID,
    iDeviceFamily,
    iPackageSize,
    iCommand,
    iStatus,
    iHardwareVersion,
    iSoftwareVersion,
    iBaudrate,
    iWritableStart = iBaudrate,
	// user begin for parameters

	// user end for parameters
    iCRCValue,
} tParametersIndex;

extern uint32_t *ptrLUT[];
extern uint8_t sizeLUT[];



#endif /* DEVICE_LUTS_H_ */
