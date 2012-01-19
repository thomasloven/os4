#include <stdint.h>
#include "include/vnode.h"
#include <vfs.h>
#include <ipc.h>

vnode_t *vnodes[VNODE_NUM];

void init_vnodes()
{
	memset(vnodes,0,sizeof(vnode_t*)*VNODE_NUM);
}

vnode_t *get_free_vnode()
{
	uint32_t i;
	for(i=0; i<VNODE_NUM && vnodes[i]; i++);
	vnodes[i] = (vnode_t *)calloc(sizeof(vnode_t));
	return vnodes[i];
}

vnode_t *find_vnode(uint32_t driver_num, uint32_t inode_num)
{
	uint32_t i;
	for(i=0; i<VNODE_NUM; i++)
	{
		if(!vnodes[i])
			continue;
		if(vnodes[i]->driver_num == driver_num)
		{
			if(vnodes[i]->inode_num == inode_num)
			{
				return vnodes[i];
			}
		}
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

void drop_vnode(vnode_t *vnode)
{
	put_vnode(vnode);
	if(vnode->use_counter == 0)
		free(vnode);
}

vnode_t *get_vnode(uint32_t driver_num, uint32_t inode_num)
{
	vnode_t *vnode = find_vnode(driver_num, inode_num);
	if(!vnode)
	{
		vnode = get_free_vnode();
		vfs_msg_getnode *getnode_msg = (vfs_msg_getnode *)calloc(sizeof(vfs_msg_getnode));
		getnode_msg->msg_type = VFSM_GETNODE;
		getnode_msg->node.driver_num = driver_num;
		getnode_msg->node.inode_num = inode_num;
		send2(driver_num, getnode_msg);
		free(getnode_msg);
		getnode_msg = waitmsg(driver_num,0);
		if(getnode_msg->node.driver_num)
		{
			memcopy((uint8_t *)vnode, (uint8_t *)&getnode_msg->node, sizeof(vnode_t));
		} else {
			_syscall_printf("\nGetnode failed!",0);
		}
		free(getnode_msg);
	}
	return vnode;
}
