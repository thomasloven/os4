#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>
#include <semaphores.h>

#define SYSCALL_FORK	0x100
#define SYSCALL_EXIT	0x101
#define SYSCALL_DONE	0x102
#define SYSCALL_GETPID	0x103
#define SYSCALL_GETPPID	0x104
#define SYSCALL_SIGNAL	0x105
#define SYSCALL_YIELD	0x106
#define SYSCALL_EXEC	0x107
#define SYSCALL_PAUSE	0x108
#define SYSCALL_WAKE	0x109

#define SYSCALL_WHEN	0x10C
#define SYSCALL_WAITPID	0x10D
#define SYSCALL_SEND	0x110
#define SYSCALL_RECV	0x111
#define SYSCALL_MSGD	0x112
#define SYSCALL_PULL	0x113
#define SYSCALL_PUSH	0x114

#define SYSCALL_MAP		0x500
#define SYSCALL_UMAP	0x501
#define SYSCALL_SBRK	0x502

#define SYSCALL_PRINTF	0xF00


#define SYSC_UNSUP	-2
#define SYSC_FAIL	-1
#define SYSC_OK	0

uint32_t _syscall_fork();
void _syscall_printf(char *str, uint32_t arg);
void _syscall_exit(uint32_t status);
void _syscall_done();
uint32_t _syscall_getpid();
uint32_t _syscall_getppid();
uint32_t _syscall_waitpid(uint32_t pid);
void _syscall_when(uint32_t address, uint32_t signum);
void _syscall_signal(uint32_t pid, uint32_t signum);
void _syscall_pause(semaphore *sem);
void _syscall_wake(uint32_t status);
void _syscall_exec(uint32_t *image);
void _syscall_send(uint32_t pid, uint32_t length, uint8_t *data);
int32_t _syscall_message_data(uint32_t *pid);
int32_t _syscall_recieve(uint8_t *buffer);
int32_t _syscall_pull(uint8_t *src, uint8_t *dst, uint16_t pid, uint16_t length);
int32_t _syscall_push(uint16_t pid, uint8_t *src, uint8_t *dst, uint16_t length);

uint32_t errno;

#endif
