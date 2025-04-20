/*
 * CircularBuffer.h
 *
 *  Created on: 14 Ara 2019
 *      Author: Umut Ediz
 */

#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_

#include "stdint.h"
#include "stdlib.h"

#define COMM_BUFFER_LENGTH     1024

typedef struct circularBuffer {
	uint8_t *Buffer;
	uint32_t readPos;
	uint32_t writePos;
	uint32_t size;
} tCircularBuffer;
extern tCircularBuffer cb;

typedef enum {
	BufferFalse, BufferTrue, BufferEmpty, BufferFull, BufferAvailable

} tBufferStatus;

tBufferStatus readCircularBuffer(tCircularBuffer *cb, uint8_t *data);
tBufferStatus writeCircularBuffer(tCircularBuffer *cb, uint8_t data);
uint32_t bufferLength(tCircularBuffer *cb);
tCircularBuffer initBuffer(uint32_t size);
void destroyBuffer(tCircularBuffer *cb);
uint32_t getRealPos(tCircularBuffer *cb, uint32_t pos);
tBufferStatus peekCircularBuffer(tCircularBuffer *cb, uint8_t *data,
		uint32_t offset);
tBufferStatus readCircularBufferBlock(tCircularBuffer *cb, uint8_t *data,
		uint32_t len);
tBufferStatus writeCircularBufferBlock(tCircularBuffer *cb, uint8_t *data,
		uint32_t len);

#endif /* CIRCULARBUFFER_H_ */
