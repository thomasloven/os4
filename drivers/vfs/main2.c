#include <syscalls.h>
#include <stdint.h>
#include <debug.h>
#include <ipc.h>
#include <vfs.h>

#include "include/fs.h"
#include "include/inodes.h"
#include "include/operations.h"

// VFS

void ls()
{
	uint32_t i = 0;
	dirent_t *d = fs_readdir(root_node,i);
	while(d->inode_number)
	{
		_syscall_printf("\n",0);
		_syscall_printf(d->name,0);
		i++;
		d = fs_readdir(root_node,i);
	}
}

void main2()
{
	
	init_inodes();

	_syscall_printf("\n\nHello from virtual file system!",0);
	
	ls();
	
	
	uint32_t ppid = _syscall_getppid();
	kill(ppid,SIGSYNC);

	while(1)
	{
		uint32_t sender = waitsig(SIGMSG,0);
		vfs_msg *message = (vfs_msg *)malloc(msglen(sender));
		recieve(sender,(uint8_t *)message);

		switch(message->msg_type)
		{
			case VMSG_MOUNT: ;
				_syscall_printf("\n Got a mount request",0);
				vfs_msg_mount *mount_msg = (vfs_msg_mount *)message;
				_syscall_printf(" from %x\n", mount_msg->root.driver_num);
				break;
				
			case VMSG_READDIR: ;
			
				vfs_msg_readdir *readdir_msg = (vfs_msg_readdir *)message;
				dirent_t *readdir_dirent = fs_readdir(&readdir_msg->node, readdir_msg->offset);

				vfs_msg_readdir_reply *readdir_reply = (vfs_msg_readdir_reply *)malloc(sizeof(vfs_msg_readdir_reply));
				readdir_reply->msg_type = VMSG_READDIR | VMSG_REPLY;
				memcopy(&readdir_reply->dirent, readdir_dirent, sizeof(dirent_t));

				send2(sender,readdir_reply);
				free(readdir_reply);
				
				break;
				
			case VMSG_FINDDIR: ;
				
				vfs_msg_finddir *finddir_msg = (vfs_msg_finddir *)message;
				inode_t *finddir_inode = fs_finddir(&finddir_msg->node, &finddir_msg->name);
				
				vfs_msg_finddir_reply *finddir_reply = (vfs_msg_finddir_reply *)malloc(sizeof(vfs_msg_finddir_reply));
				finddir_reply->msg_type = VMSG_READDIR | VMSG_REPLY;
				memcopy(&finddir_reply->node, finddir_inode, sizeof(inode_t));
				
				send2(sender, finddir_reply);
				free(finddir_reply);
				
				break;
		}
		
		free(message);
	}
	
	_syscall_exit(0);
}