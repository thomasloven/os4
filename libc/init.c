#include <stdint.h>
#include <syscalls.h>
#include <ipc.h>

extern void main(void);

void _init()
{
	signals_init();
	_syscall_sbrk(-0x10000000);
	main();
}