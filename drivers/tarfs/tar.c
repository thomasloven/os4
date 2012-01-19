#include <stdint.h>
#include <strings.h>
#include "include/tar.h"
#include "include/fs.h"


size_t tar_size(uint8_t *size)
{
	size_t ret = 0;
	int i;
	for(i = 0; i < 12 && size[i] >= '0'; i++)
	{
		ret = ret*8;
		ret = ret+((uint32_t)size[i] - (uint32_t)'0');
	}
	return ret;
}

uint32_t *tar_find(tar_head *head, char *name)
{
	while(head->name[0] != '\0')
	{
		if(strcmp(head->name, name) == 0)
		{
			return (uint32_t *)((uint32_t)head + sizeof(tar_head));
		}
		uint32_t size = tar_size(head->size);
		head = (tar_head *)((uint32_t)head + sizeof(tar_head) + size + (sizeof(tar_head) - (size%512)));
	}
	return 0;
}

uint32_t *tar_num(tar_head *head, uint32_t offset)
{
	while(offset)
	{ 
		if(head->name[0] != '\0')
			return 0;
		uint32_t size = tar_size(head->size);
		head = (tar_head *)((uint32_t)head + sizeof(tar_head) + size + (sizeof(tar_head) - (size%512)));
	}
	return (uint32_t*)((uint32_t)head + sizeof(tar_head));
}

tar_head *get_head(uint32_t *pos)
{
	return (tar_head *)((uint32_t)pos - sizeof(tar_head));
}