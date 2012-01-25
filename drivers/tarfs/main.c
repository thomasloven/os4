#include <syscalls.h>
#include <stdint.h>
#include <debug.h>
#include <ipc.h>
#include <vfs.h>
#include <heap.h>
#include "include/inodes.h"
#include "include/fs.h"
#include "include/tar.h"
// TARFS

tar_head *drivers;

void ls()
{
	tar_head *head = drivers;
	while(head->name[0] != '\0')
	{
		_syscall_printf("\n",0);
		uint32_t size = tar_size(head->size);
		_syscall_printf(head->name,0);
		_syscall_printf(" %d b",size);
		
		inode_t *new = new_inode(head);
		new->flags = VFS_FILE;
		new->size = size;
		add_node(root_node, new, head->name);
		
		head = (tar_head *)((uint32_t)head + sizeof(tar_head) + size + (sizeof(tar_head) - (size%512)));
	}
}

void main()
{
	init_inodes();
	
	uint32_t ppid = _syscall_getppid();
	kill(ppid,SIGSYNC);
	uint32_t pid = _syscall_getpid();
	
	_syscall_printf("\n\nHello from ram file system! %x",pid);
	
	waitsig(SIGMSG, ppid);
	uint16_t len = msglen(ppid);
	drivers = (tar_head *)malloc(len);
	recieve(ppid,(uint8_t *)drivers);
	_syscall_printf(drivers->name,0);
	
	ls();
	while(1)
	{
		uint32_t sender = waitsig(SIGMSG,0);
		vfsm *message = (vfsm *)malloc(msglen(sender));
		recieve(sender,(uint8_t *)message);
		_syscall_printf("\n Got message %x", message->m.msg_type);
		
		switch(message->m.msg_type)
		{
			case VFSM_GETNODE: ;
				tarfs_getnode(message, sender, drivers);
				break;
			case VFSM_READDIR: ;
				tarfs_readdir(message, sender);
				break;
			case VFSM_LOOKUP: ;
				tarfs_lookup(message, sender);
				break;
			case VFSM_READ_INT: ;
				tarfs_read(message, sender);
				break;
		}
		
		free(message);
	}
	_syscall_printf("\nlsed",0);
	
	for(;;);
	_syscall_exit(0);
}