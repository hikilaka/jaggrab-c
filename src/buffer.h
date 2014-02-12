#ifndef BUFFER_H
#define BUFFER_H

#pragma once

#include <stddef.h>
#include <stdint.h>

typedef enum
{
	buffer_ok = 0,
	buffer_overflow = -1
} buffer_res;

typedef struct
{
	size_t size;
	size_t position;
	uint8_t* payload;
} buffer_t;

/**
 * Wraps a payload in an allocated buffer structure.
 * This is typically used for reading data from a
 * buffer. The payload of the buffer is copied memory
 * from the argument payload.
 */
buffer_t* buffer_wrap(uint8_t* payload, size_t size);

/**
 * Creates an empty payload buffer of the supplied size.
 * This construct is typically used when data is needing
 * to be written to a buffer. The size of the buffer will
 * grow as it exceeds the initial size.
 */
buffer_t* new_buffer(size_t init_size);

/**
 * Deallocates a buffer struct, along with its payload
 */
void free_buffer(buffer_t* buffer);

/**
 * Returns the amount of available bytes that can
 * be read
 */
size_t buffer_available(buffer_t* buffer);

/**
 * Adds an unsigned 8 bit integer to a buffer
 */
void buffer_add_byte(buffer_t* buffer, uint8_t b);

/**
 * Adds an unsigned 16 bit integer to a buffer
 */
void buffer_add_short(buffer_t* buffer, uint16_t s);

/**
 * Adds an unsigned 32 bit integer to a buffer
 */
void buffer_add_int(buffer_t* buffer, uint32_t i);

/**
 * Adds an unsigned 64 bit integer to a buffer
 */
void buffer_add_long(buffer_t* buffer, uint64_t l);

/**
 * Adds a string to a buffer.
 * NOTE: The string MUST be null terminated,
 * the null terminator isn't added to the buffer.
 */
void buffer_add_str(buffer_t* buffer, char* str);

/**
 * Writes 0-32 (inclusive) bits to a buffer.
 */
void buffer_add_bits(buffer_t* buffer, uint32_t value, uint32_t amount);

/**
 * Reads a single byte into b, returns buffer_ok on success,
 * and buffer_overflow if an overflow occurs.
 */
buffer_res buffer_get_byte(buffer_t* buffer, uint8_t* b);

/**
 * Reads a 16 bit inteter to s, returns buffer_ok on success,
 * and buffer_overflow if an overflow occurs.
 */
buffer_res buffer_get_short(buffer_t* buffer, uint16_t* s);

/**
 * Reads a 32 bit inteter to i, returns buffer_ok on success,
 * and buffer_overflow if an overflow occurs.
 */
buffer_res buffer_get_int(buffer_t* buffer, uint32_t* i);

/**
 * Reads a 64 bit inteter to l, returns buffer_ok on success,
 * and buffer_overflow if an overflow occurs.
 */
buffer_res buffer_get_long(buffer_t* buffer, uint64_t* l);

/**
 * Reads a string from a buffer (with a length of len), the resulting
 * string is automatically null terminated. Returns buffer_ok on success
 * or buffer_overflow if an overflow occurs.
 */
buffer_res buffer_get_str(buffer_t* buffer, char* str, size_t len);

/**
 * Reads 0-32 bits of data from a buffer into i.
 */
buffer_res buffer_get_bits(buffer_t* buffer, uint32_t* i, uint32_t amount);

#endif