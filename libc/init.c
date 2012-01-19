#include <stdint.h>
#include <syscalls.h>
#include <ipc.h>

extern uint32_t main(void);

void _init()
{
	signals_init();
	_syscall_sbrk(-0x10000000);
	_syscall_exit(main());
}