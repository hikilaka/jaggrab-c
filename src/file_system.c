#include "file_system.h"

#include <stdio.h>
#include <string.h>
#include "memory.h"
#include "crc32.h"

static FILE* data;
static FILE* indicies[256];
static buffer_t* crc_table;

int file_sys_init(char* root_dir)
{
	puts("loading file system..");
	int result = 0;
	
	char buff[128];
	int cx = snprintf(buff, 128, "%s", root_dir);

	snprintf(buff + cx, 128 - cx, "main_file_cache.dat");
	data = fopen(buff, "r");
	if (data == NULL)
	{
		snprintf(buff + cx, 128 - cx, "main_file_cache.dat2");
		data = fopen(buff, "r");
	}

	if (data != NULL)
		printf("\tloaded data file\n");

	int index_count = 0;
	for (int i = 0; i < 256; ++i)
	{
		if (indicies[i] == NULL)
		{
			snprintf(buff + cx, 128 - cx, "main_file_cache.idx%d", i);
			indicies[i] = fopen(buff, "r");
			if (indicies[i])
				index_count++;
		}
	}

	if (index_count > 0)
		printf("\tloaded %d index files\n", index_count);

	if (data == NULL)
		result |= 0x1;
	if (index_count == 0)
		result |= 0x2;
	return result;
}

void file_sys_dealloc()
{
	if (data != NULL)
	{
		fclose(data);
		data = NULL;
	}
	for (int i = 0; i < 256; ++i)
	{
		if (indicies[i] != NULL)
		{
			fclose(indicies[i]);
			indicies[i] = NULL;
		}
	}
	if (crc_table != NULL)
	{
		free_buffer(crc_table);
		crc_table = NULL;
	}
}

size_t file_sys_file_count(uint8_t type)
{
	size_t count = 0;
	if (indicies[type] != NULL)
	{
		fseek(indicies[type], 0L, SEEK_END);
		count = ftell(indicies[type]) / 6;
	}
	return count;
}

buffer_t* file_sys_get_crc_tbl()
{
	if (crc_table != NULL)
		return crc_table;

	size_t file_ct = file_sys_file_count(0);
	uint32_t crcs[file_ct];
	uint32_t crc;

	for (size_t i = 1; i < file_ct; ++i)
	{
		buffer_t* buf = file_sys_get_file(0, i);
		crc = 0;
		crc = crc32(crc, buf->payload, buf->size);
		crcs[i] = crc;
		free(buf);
	}
	
	uint32_t hash = 1234;
	crc_table = new_buffer((file_ct * 4) + 4);

	for (size_t i = 0; i < file_ct; ++i)
	{
		hash = (hash << 1) + crcs[i];
		buffer_add_int(crc_table, crcs[i]);
	}
	buffer_add_int(crc_table, hash);
	crc_table->position = 0;
	return crc_table;
}

typedef struct
{
	size_t file_sz;
	size_t block;
} fs_index_t;

static fs_index_t* get_index(uint8_t type, int16_t file)
{
	if (indicies[type] == NULL)
		return NULL;

	fseek(indicies[type], 0L, SEEK_END);
	size_t idx_sz = ftell(indicies[type]);
	long ptr = file * 6;

	if (ptr >= 0 && idx_sz >= ptr + 6)
	{
		uint8_t buff[6];
		fseek(indicies[type], ptr, SEEK_SET);
		fread(buff, 1, 6, indicies[type]);
		fs_index_t* idx = safe_calloc(1, sizeof(fs_index_t));
		idx->file_sz = (buff[0] << 16) | (buff[1] << 8) | buff[2];
		idx->block = (buff[3] << 16) | (buff[4] << 8) | buff[5];
		return idx;
	}
	else
	{
		return NULL;
	}
}

buffer_t* file_sys_get_file(uint8_t type, int16_t file)
{
	fs_index_t* idx = get_index(type, file);

	if (!idx || !data)
		return NULL;

	buffer_t* buff = new_buffer(idx->file_sz);
	long ptr = idx->block * 520;
	size_t blocks = idx->file_sz / 512;
	size_t read = 0;

	if ((idx->file_sz % 512) != 0)
		blocks++;

	for (size_t i = 0; i < blocks; ++i)
	{
		uint8_t head[8];
		fseek(data, ptr, SEEK_SET);
		fread(head, 1, 8, data);
		ptr += 8;

		uint16_t next_file = (head[0] << 8) | head[1];
		uint16_t cur_chunk = (head[2] << 8) | head[3];
		uint32_t next_block = (head[4] << 16) | (head[5] << 8) | head[6];
		uint8_t next_type = head[7];

		if (i != cur_chunk)
		{
			printf("chunk id mismatch! type -> %d, file -> %d\n", type, file);
			return NULL;
		}

		size_t chunk_sz = idx->file_sz - read;
		if (chunk_sz > 512)
			chunk_sz = 512;

		char chunk[chunk_sz];
		fseek(data, ptr, SEEK_SET);
		fread(chunk, 1, chunk_sz, data);

		for (size_t j = 0; j < chunk_sz; ++j)
			buffer_add_byte(buff, chunk[j]);

		read += chunk_sz;
		ptr = next_block * 520;

		if (idx->file_sz > read)
		{
			if (next_type != (type + 1))
			{
				printf("file sys type mismatch! type -> %d, file -> %d\n", type, file);
				return NULL;
			}
			if (next_file != file)
			{
				printf("file sys id mismatch! type -> %d, file -> %d\n", type, file);
				return NULL;
			}
		}
	}
	buff->position = 0;
	free(idx);
	return buff;
}