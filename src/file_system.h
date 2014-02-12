#ifndef FS_FILE_SYSTEM_H
#define FS_FILE_SYSTEM_H

#pragma once

#include <stdint.h>
#include <stddef.h>
#include "buffer.h"

int file_sys_init(char* root_dir);

void file_sys_dealloc(void);

size_t file_sys_file_count(uint8_t type);

buffer_t* file_sys_get_crc_tbl(void);

buffer_t* file_sys_get_file(uint8_t type, int16_t file);

#endif