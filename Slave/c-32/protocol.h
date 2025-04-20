/*
 * protocol.h
 *
 *  Created on: Feb 24, 2023
 *      Author: beratComputer
 */

#ifndef INC_PROTOCOL_H_BAK_
#define INC_PROTOCOL_H_BAK_

#include "align.h"
#include "string.h"
#include "luts.h"
#include "circular_buffer.h"

// Header, ID, Device Family, PackageSize, Command, Status, [...DATA...], CRC

#define PROTOCOL_TIMEOUT 10

#define HEADER            			(0x55)
#define DEVICE_FAMILY				(0xBA)
#define CONSTANT_REG_SIZE 			(10) // Header, ID, Device Family, PackageSize, Command, Status, CRC
#define MAX_VAR_COUNT              	(56) /*Number of variables in the protocol structure*/
#define MAX_PACKAGE_SIZE  			(1024) /*FIXME : calculate correct number */
#define DATA(X) 					(CONSTANT_REG_SIZE + X - 4)

#define __POS_OF(Y, X) ((Y + X) & (BUFFER_LENGTH - 1))

#define BATCH_ID           			(0xFF)

#define RX_AVAIL_FLAG	  (1 << 0)
#define TX_CPLT_FLAG     (1 << 1)

typedef enum Command {
	ACK = 0x80,
	SYNC = 0x40,

	PING = 0x00,
	WRITE = 0x01,
	READ = 0x02,
	EEPROM_WRITE = 0x03,
	ERROR_CLEAR = 0x04,
	REBOOT = 0x05,
	HARD_RESET = 0x06,
	BL_JUMP = 0x07,
	TUNE = 0x08,

	WRITE_ACK = ACK | WRITE,
	WRITE_SYNC = SYNC | WRITE, // multiple write so cannot get ack.
	EEPROM_WRITE_ACK = ACK | EEPROM_WRITE, /* Unimplemented */
	EEPROM_WRITE_SYNC = SYNC | EEPROM_WRITE,
} tCommand;

typedef enum __attribute__ ((__packed__)) ProtocolStatus {
	ProtocolFalse, ProtocolTrue, ProtocolIrrelevant, ProtocolInvalid
} tProtocolStatus;

typedef struct {
	volatile tCircularBuffer *cb;
	uint8_t txBuffer[MAX_PACKAGE_SIZE];
	uint8_t rxBuffer[MAX_PACKAGE_SIZE];
	uint8_t customPackageIndex[MAX_VAR_COUNT]; //Do not include constant registers
	uint32_t **ptrLUT;
	uint8_t *sizeLUT;
	uint32_t CRCValue;
	_u8 header;
	_u8 deviceID;
	_u8 deviceFamily;
	_u8 packageSize;
	_u8 command;
	_u8 errorCount;
	_u8 customPackageIndexCount;
	volatile tProtocolStatus transmitAvailable;
	volatile tProtocolStatus receiveAvailable;
} __attribute__ ((aligned(4))) tCommunication;

tProtocolStatus commInit(volatile tCommunication *pComm, uint32_t **ptrLUT,
		uint8_t *sizeLUT);
tProtocolStatus processReceived(volatile tCommunication *pComm);
tProtocolStatus generateTransmitPackage(volatile tCommunication *pComm);
tProtocolStatus generateCustomTransmitPackage(volatile tCommunication *pComm,
		uint8_t *pData, uint8_t size);

#endif /* INC_PROTOCOL_H_BAK_ */

