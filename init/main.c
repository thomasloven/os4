#include <syscalls.h>
#include <stdint.h>
#include <strings.h>
#include <debug.h>
#include <ipc.h>
#include <vfs.h>
#include <heap.h>
// init

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

uint32_t start_daemon(uint32_t *image)
{
	uint32_t child = _syscall_fork();
	if(child)
	{
		waitsig(SIGSYNC, child);
		return child;
	} else {
		_syscall_exec(image);
		_syscall_exit(0);
	}
}

void main()
{

	uint32_t pid = _syscall_getpid();
	_syscall_printf("\nHello from process %x!", pid);

	uint32_t length = _syscall_message_data(0);
	tar_head *drivers = (tar_head *)malloc(length);
	_syscall_recieve((uint8_t *)drivers);

	uint32_t vfs_num = start_daemon(tar_find(drivers,"vfs.elf"));
	uint32_t ramfs_num = start_daemon(tar_find(drivers,"tarfs.elf"));
	
	send(ramfs_num, length, (uint8_t *)drivers);
	
	
	_syscall_printf("\nHello! %x \n",vfs_num);
	
	
	
	CALLOCP(node, inode_t);
	node->driver_num = ramfs_num;
	node->inode_num = 1;

	chdir(node);	
	chroot(node);


	
	dirent_t *dent = readdir(node, 3);
	
	_syscall_printf(dent->name,0);
	
	uint32_t file = open("/myfile.txt");
	_syscall_printf("Opened file %x", file);
	file = open("/myfile.txt");
	_syscall_printf("Opened file %x", file);
	file = open("vfs.elf");
	_syscall_printf("Opened file %x", file);
	
	for(;;);
	_syscall_exit(0);
	_syscall_printf("Not printed! %x",0);

	for(;;);
}