#include <syscalls.h>
#include <stdint.h>
#include <debug.h>
#include <ipc.h>
#include <vfs.h>

// RAMFS

typedef struct tar_fhead_st
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

void ls()
{
	inode_t *ramfs_root = (inode_t *)calloc(sizeof(inode_t));
	ramfs_root->driver_num = 0x2;
	ramfs_root->inode_number = 1;
	ramfs_root->use_counter = 1;
	uint32_t i = 0;
	dirent_t *d = readdir(ramfs_root,i);
	while(d->inode_number)
	{
		_syscall_printf("\n",0);
		_syscall_printf(d->name,0);
		i++;
		d = readdir(ramfs_root,i);
	}
}

void main()
{
	uint32_t ppid = _syscall_getppid();
	kill(ppid,SIGSYNC);
	uint32_t pid = _syscall_getpid();
	
	_syscall_printf("\n\nHello from ram file system! %x",pid);
	
	waitsig(SIGMSG, ppid);
	tar_head *drivers = (tar_head *)malloc(msglen(ppid));
	recieve(ppid,(uint8_t *)drivers);
	_syscall_printf(drivers->name,0);
	
	ls();

	
	for(;;);
	_syscall_exit(0);
}