#include <stdint.h>
#include "include/vnode.h"

vnode_t *vnodes[VNODE_NUM];

void init_vnodes()
{
	memset(vnodes,0,sizeof(vnode_t*)*VNODE_NUM);
}

vnode_t *get_free_vnode()
{
	uint32_t i;
	for(i=0; i<VNODE_NUM && vnodes[i]; i++);
	vnodes[i] = calloc(sizeof(vnode_t));
	return vnodes[i];
}

vnode_t *find_vnode(uint32_t driver_num, uint32_t inode_num)
{
	uint32_t i;
	for(i=0; i<VNODE_NUM; i++)
	{
		if(vnodes[i]->driver_num == driver_num)
			if(vnodes[i]->inode_num == inode_num)
				return vnodes[i];
	}
	return 0;
}

void dup_vnode(vnode_t *vnode)
{
	vnode->use_counter = vnode->use_counter + 1;
}

void put_vnode(vnode_t *vnode)
{
	vnode->use_counter = vnode->use_counter - 1;
}

vnode_t *get_vnode(uint32_t driver_num, uint32_t inode_num)
{
	
}
