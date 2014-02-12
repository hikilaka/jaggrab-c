#include "memory.h"

#include <stdlib.h>

void* safe_alloc(size_t size)
{
	if (size == 0)
		return NULL;
	void* block = malloc(size);

	// need to call error handlers if block == NULL
	return block;
}

void* safe_calloc(size_t amount, size_t size)
{
	if (amount == 0 || size == 0)
		return NULL;
	void* block = calloc(amount, size);

	// need to call error handlers if block == NULL
	return block;
}

void* safe_realloc(void* ptr, size_t size)
{
	if (size == 0)
		return NULL;
	void* newptr = realloc(ptr, size);

	if (newptr != NULL)
		ptr = newptr;
	// need to call error handlers if block == NULL
	return newptr;
}