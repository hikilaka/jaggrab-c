#ifndef MEMORY_H
#define MEMORY_H

#pragma once

#include <stdlib.h>

/**
 * Allocates a block of memory with a specified
 * size. If the allocation fails, the registered
 * error handlers are called.
 * If size is 0 NULL is returned.
 * The underlying allocation function is malloc.
 */
void* safe_alloc(size_t size);

/**
 * Allocates a block of memory with a specified
 * amount and size. The resulting memory block is
 * effectively the size of amount * size. If the allocation
 * fails, the registered handlers are called.
 * If amount or size equals 0
 * NULL is returned. The underlying allocation
 * function is calloc, which initializes the
 * block with all 0s.
 */
void* safe_calloc(size_t amount, size_t size);

/**
 * Resizes an allocated block of contiguous memory.
 * if ptr is NULL this function acts like safe_malloc,
 * likewise, if size equals 0 NULL is returned.
 */
void* safe_realloc(void* ptr, size_t size);

#endif