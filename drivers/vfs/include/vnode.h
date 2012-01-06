#pragma once

#include <stdint.h>

typedef struct vnode_struct
{
	uint32_t driver_num;
	uint32_t inode_num;
	uint32_t flags;
	uint32_t file_size;
	uint32_t use_counter;
} vnode_t;


#define VNODE_NUM 512

void init_vnodes();
vnode_t *get_free_vnode();
vnode_t *find_vnode(uint32_t driver_num, uint32_t inode_num);
void dup_vnode(vnode_t *vnode);
void put_vnode(vnode_t *vnode);
vnode_t *get_vnode(uint32_t driver_num, uint32_t inode_num);
