#include <syscalls.h>
#include <stdint.h>
#include <semaphores.h>
#include <debug.h>

uint32_t _syscall_fork()
{
	uint32_t ret;
	asm volatile("int $0xFF" : "=a" (errno), "=b" (ret) : "a" (SYSCALL_FORK) );
	return ret;
}

void _syscall_exit(uint32_t status)
{
	asm volatile("int $0xFF" : : "b" (status), "a" (SYSCALL_EXIT) );
}

void _syscall_done()
{
	asm volatile("int $0xFF" : : "a" (SYSCALL_DONE) );
}

uint32_t _syscall_getpid()
{
	uint32_t ret;
	asm volatile("int $0xFF" : "=a" (errno), "=b" (ret) : "a" (SYSCALL_GETPID) );
	return ret;
}

uint32_t _syscall_getppid()
{
	uint32_t ret;
	asm volatile("int $0xFF" : "=a" (errno), "=b" (ret) : "a" (SYSCALL_GETPPID) );
	return ret;
}

void _syscall_printf(char *str, uint32_t arg)
{
	asm volatile("int $0xFF" : "=a" (errno) :"c" (arg), "b" (str), "a" (SYSCALL_PRINTF) );
}

uint32_t _syscall_waitpid(uint32_t pid)
{
	asm volatile("int $0xFF": "=a" (errno) : "b" (pid), "a" (SYSCALL_WAITPID));
	if(errno >= 0)
		return 0;
	else
		return -1;
}

void _syscall_when(uint32_t address, uint32_t signum)
{
	asm volatile("int $0xFF": "=a" (errno) : "c" (signum), "b" (address), "a" (SYSCALL_WHEN));
}

void _syscall_signal(uint32_t pid, uint32_t signum)
{
	asm volatile("int $0xFF": "=a" (errno) :"c" (signum), "b" (pid), "a" (SYSCALL_SIGNAL));
}

void _syscall_pause(semaphore *sem)
{
	asm volatile("int $0xFF": "=a" (errno) : "a" (SYSCALL_PAUSE), "b" (sem) );
}

void _syscall_wake(uint32_t status)
{
	asm volatile("int $0xFF": "=a" (errno) : "a" (SYSCALL_WAKE), "c" (status) );
}

void _syscall_exec(uint32_t *image)
{
	asm volatile("int $0xFF": "=a" (errno) : "b" (image), "a" (SYSCALL_EXEC));
}

void _syscall_sbrk(int32_t size)
{
	asm volatile("int $0xFF" : "=a" (errno) : "b" (size), "a" (SYSCALL_SBRK));
}

void _syscall_send(uint32_t pid, uint32_t length, uint8_t *data)
{
	asm volatile("int $0xFF" : "=a" (errno) : "d" (data), "c" (length), "b" (pid), "a" (SYSCALL_SEND));
}

int32_t _syscall_message_data(uint32_t *pid)
{
	uint32_t ret, retpid;
	asm volatile("int $0xFF" : "=c" (ret), "=b" (retpid), "=a" (errno) : "a" (SYSCALL_MSGD));
	if(pid)
	{
		*pid = retpid;
	}
	return ret;
}

int32_t _syscall_recieve(uint8_t *buffer)
{
	uint32_t ret;
	asm volatile("int $0xFF" : "=c" (ret), "=a" (errno) : "b" (buffer), "a" (SYSCALL_RECV));
	return ret;
}
