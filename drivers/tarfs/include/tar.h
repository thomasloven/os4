#pragma once

#include <stdint.h>

typedef struct
{
	uint8_t name[100];
	uint8_t mode[8];
	uint8_t uid[8];
	uint8_t gid[8];
	uint8_t size[12];
	uint8_t mtime[12];
	uint8_t checksum[8];
	uint8_t linkflag[1];
	uint8_t linkname[100];
	uint8_t pad[255];
}__attribute__((packed)) tar_head;

size_t tar_size(uint8_t *size);
uint32_t *tar_find(tar_head *head, char *name);
uint32_t *tar_num(tar_head *head, uint32_t offset);
tar_head *get_head(uint32_t *pos);