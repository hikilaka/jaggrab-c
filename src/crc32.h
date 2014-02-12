#ifndef CRC_CRC32_H
#define CRC_CRC32_H

#pragma once

#include <stdint.h>
#include <stddef.h>

uint32_t crc32(uint32_t crc, const void *buf, size_t size);

#endif