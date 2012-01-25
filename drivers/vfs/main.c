#include <syscalls.h>
#include <stdint.h>
#include <debug.h>
#include <ipc.h>
#include <vfs.h>
#include <heap.h>

#include "include/operations.h"
#include "include/vnode.h"
#include "include/fproc.h"


uint32_t main()
{

	init_vnodes();
	init_fproc();
	uint32_t ppid = _syscall_getppid();
	uint32_t pid = _syscall_getpid();
	new_fproc(ppid,0);
	new_fproc(pid,0);

	_syscall_printf("\n\nHello from virtual file system! %x",pid);
	kill(ppid,SIGSYNC);

	while(1)
	{
		uint32_t sender = waitsig(SIGMSG,0);
		vfsm *message = (vfsm *)malloc(msglen(sender));
		recieve(sender,(uint8_t *)message);

		switch(message->m.msg_type)
		{
			case VFSM_FORK: ;
				vfs_fork(message, sender);
				break;
				
			case VFSM_CHRD: ;
				vfs_chrd(message, sender);
				break;
				
			case VFSM_CHWD: ;
				vfs_chwd(message, sender);
				break;
				
			case VFSM_OPEN: ;
				vfs_open(message, sender);
				break;
				
			case VFSM_READDIR: ;
				vfs_readdir(message, sender);
				break;
				
			case VFSM_READ: ;
				vfs_read(message, sender);
				break;
		}
		
		free(message);
	}
	return 0;
}