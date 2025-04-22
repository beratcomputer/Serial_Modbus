/*
 * CircularBuffer.c
 *
 *  Created on: 14 Ara 2019
 *      Author: Umut
 */

 #include "circular_buffer.h"

 #include "string.h"

 //#warning "Buffer size must be a power of two"

 volatile uint8_t uart_rx_buffer[COMM_BUFFER_LENGTH] = { 0xFF };

 tCircularBuffer cb = {
         .Buffer = (uint8_t*) uart_rx_buffer,
         .readPos = 0,
         .writePos = 0,
         .size = COMM_BUFFER_LENGTH };


 inline uint32_t getRealPos(tCircularBuffer* cb, uint32_t pos) {
   return ((pos) & (cb->size - 1));
 }

 tBufferStatus readCircularBuffer(tCircularBuffer* cb, uint8_t* data) {
   if (bufferLength(cb) == 0) {
     return BufferEmpty;
   }
   *data = cb->Buffer[cb->readPos];
   cb->readPos = ((cb->readPos + 1) & (cb->size - 1));
   return BufferTrue;
 }

 tBufferStatus writeCircularBuffer(tCircularBuffer* cb, uint8_t data) {
   if (bufferLength(cb) == (cb->size - 1)) {
     return BufferFull;
   }
   cb->Buffer[cb->writePos] = data;
   cb->writePos = (cb->writePos + 1) & (cb->size - 1);  // must be atomic
   return BufferTrue;
 }

 uint32_t bufferLength(tCircularBuffer* cb) {
   return ((cb->writePos - cb->readPos) & (cb->size - 1));
 }

 tCircularBuffer initBuffer(uint32_t size) {
   tCircularBuffer cb;
   cb.size = size;
   cb.Buffer = (uint8_t*)malloc(sizeof(uint8_t) * cb.size);
   return cb;
 }

 tBufferStatus peekCircularBuffer(tCircularBuffer* cb, uint8_t* data,
                                  uint32_t offset) {
   if (bufferLength(cb) <= offset) {
     return BufferEmpty;
   }
   *data = cb->Buffer[getRealPos(cb, cb->readPos + offset)];
   return BufferTrue;
 }

 tBufferStatus readCircularBufferBlock(tCircularBuffer* cb, uint8_t* data,
                                       uint32_t len) {
   if (bufferLength(cb) < len) {
     return BufferEmpty;
   }
   if ((cb->readPos + len) < cb->size) {
     memcpy(data, cb->Buffer + cb->readPos, len);
   } else {
     memcpy(data, cb->Buffer + cb->readPos, cb->size - cb->readPos);
     memcpy(data + cb->size - cb->readPos, cb->Buffer,
            len - (cb->size - cb->readPos));
   }
   cb->readPos = ((cb->readPos + len) & (cb->size - 1));
   return BufferTrue;
 }

 tBufferStatus writeCircularBufferBlock(tCircularBuffer* cb, uint8_t* data,
                                        uint32_t len) {
   if (cb->size - bufferLength(cb) < len) {
     return BufferFull;
   }
   if ((cb->writePos + len) < cb->size) {
     memcpy(cb->Buffer + cb->writePos, data, len);
   } else {
     memcpy(cb->Buffer + cb->writePos, data, cb->size - cb->writePos);
     memcpy(cb->Buffer, data + cb->size - cb->writePos,
            len - (cb->size - cb->readPos));
   }
   cb->writePos = ((cb->writePos + len) & (cb->size - 1));
   return BufferTrue;
 }

 void destroyBuffer(tCircularBuffer* cb) { free(cb->Buffer); }

