#include "buffer.h"

#include <string.h>
#include <assert.h>
#include "memory.h"

buffer_t* buffer_wrap(uint8_t* payload, size_t size)
{
	buffer_t* buffer = safe_alloc(sizeof(buffer_t));
	uint8_t* copy = safe_calloc(size, sizeof(uint8_t));
	memcpy(copy, payload, size);
	buffer->size = buffer->position = size;
	buffer->payload = copy;
	return buffer;
}

buffer_t* new_buffer(size_t init_size)
{
	buffer_t* buffer = safe_alloc(sizeof(buffer_t));
	buffer->size = init_size;
	buffer->position = 0;
	buffer->payload = safe_calloc(init_size, sizeof(uint8_t));
	return buffer;
}

void free_buffer(buffer_t* buffer)
{
	assert(buffer->payload != NULL);
	assert(buffer);
	free(buffer->payload);
	free(buffer);
}

size_t buffer_available(buffer_t* buffer)
{
	return buffer->size - buffer->position;
}

static inline void check_capacity(buffer_t* buffer, size_t size)
{
	if (buffer->size - buffer->position < size)
	{
		buffer->size += size;
		buffer->payload = safe_realloc(buffer->payload, buffer->size);
	}
}

void buffer_add_byte(buffer_t* buffer, uint8_t b)
{
	check_capacity(buffer, sizeof(uint8_t));
	buffer->payload[buffer->position++] = b;
}

void buffer_add_short(buffer_t* buffer, uint16_t s)
{
	check_capacity(buffer, sizeof(uint16_t));
	buffer->payload[buffer->position++] = (s >> 8) & 0xFF;
	buffer->payload[buffer->position++] = s & 0xFF;
}

void buffer_add_int(buffer_t* buffer, uint32_t i)
{
	check_capacity(buffer, sizeof(uint32_t));
	buffer->payload[buffer->position++] = (i >> 24) & 0xFF;
	buffer->payload[buffer->position++] = (i >> 16) & 0xFF;
	buffer->payload[buffer->position++] = (i >> 8) & 0xFF;
	buffer->payload[buffer->position++] = i & 0xFF;
}

void buffer_add_long(buffer_t* buffer, uint64_t l)
{
	check_capacity(buffer, sizeof(uint64_t));
	buffer->payload[buffer->position++] = (uint8_t) (l >> 56);
	buffer->payload[buffer->position++] = (uint8_t) (l >> 48);
	buffer->payload[buffer->position++] = (uint8_t) (l >> 40);
	buffer->payload[buffer->position++] = (uint8_t) (l >> 32);
	buffer->payload[buffer->position++] = (uint8_t) (l >> 24);
	buffer->payload[buffer->position++] = (uint8_t) (l >> 16);
	buffer->payload[buffer->position++] = (uint8_t) (l >> 8);
	buffer->payload[buffer->position++] = (uint8_t) l;
}

void buffer_add_str(buffer_t* buffer, char* str)
{
	check_capacity(buffer, strlen(str));
	memcpy(buffer->payload + buffer->position, str, strlen(str));
}

void buffer_add_bits(buffer_t* buffer, uint32_t value, uint32_t amount)
{
	// TODO: complete
}

static buffer_res has_available(buffer_t* buffer, size_t amount)
{
	if (buffer->size - buffer->position >= amount)
		return buffer_ok;
	else
		return buffer_overflow;
}

buffer_res buffer_get_byte(buffer_t* buffer, uint8_t* b)
{
	buffer_res res = has_available(buffer, sizeof(uint8_t));
	if (res == buffer_ok)
	{
		*b = buffer->payload[buffer->position++];
	}
	return res;
}

buffer_res buffer_get_short(buffer_t* buffer, uint16_t* s)
{
	buffer_res res = has_available(buffer, sizeof(uint16_t));
	if (res == buffer_ok)
	{
		*s = buffer->payload[buffer->position++] << 8;
		*s |= buffer->payload[buffer->position++];
	}
	return res;
}

buffer_res buffer_get_int(buffer_t* buffer, uint32_t* i)
{
	buffer_res res = has_available(buffer, sizeof(uint32_t));
	if (res == buffer_ok)
	{
		*i = buffer->payload[buffer->position++] << 24;
		*i |= buffer->payload[buffer->position++] << 16;
		*i |= buffer->payload[buffer->position++] << 8;
		*i |= buffer->payload[buffer->position++];
	}
	return res;
}

buffer_res buffer_get_long(buffer_t* buffer, uint64_t* l)
{
	buffer_res res = has_available(buffer, sizeof(uint64_t));
	if (res == buffer_ok)
	{
		*l = (uint64_t) buffer->payload[buffer->position++] << 56;
		*l |= (uint64_t) buffer->payload[buffer->position++] << 48;
		*l |= (uint64_t) buffer->payload[buffer->position++] << 40;
		*l |= (uint64_t) buffer->payload[buffer->position++] << 32;
		*l |= buffer->payload[buffer->position++] << 24;
		*l |= buffer->payload[buffer->position++] << 16;
		*l |= buffer->payload[buffer->position++] << 8;
		*l |= buffer->payload[buffer->position++];
	}
	return res;
}

buffer_res buffer_get_str(buffer_t* buffer, char* str, size_t len)
{
	buffer_res res = has_available(buffer, sizeof(uint8_t) * len);
	if (res == buffer_ok)
	{
		memcpy(str, buffer->payload + buffer->position, len);
		buffer->position += len;
		str[len] = '\0';
	}
	return res;
}

buffer_res buffer_get_bits(buffer_t* buffer, uint32_t* i, uint32_t amount)
{
	// TODO: complete
	return buffer_overflow;
}