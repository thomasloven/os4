#ifndef KERNEL_SYSCALLS_H
#define KERNEL_SYSCALLS_H

#include <syscalls.h>
typedef struct thread_struct thread;

void init_syscalls();
thread *syscall_fork(thread *th);
thread *syscall_handler(thread *th);
thread *syscall_unsuported(thread *th);
thread *syscall_printf(thread *th);
thread *syscall_exit(thread *th);
thread *syscall_done(thread *th);
thread *syscall_getpid(thread *th);
thread *syscall_getppid(thread *th);
thread *syscall_signal(thread *th);
thread *syscall_exec(thread *th);
thread *syscall_waitpid(thread *th);
thread *syscall_when(thread *th);
thread *syscall_pause(thread *th);
thread *syscall_wake(thread *th);
thread *syscall_sbrk(thread *th);
thread *syscall_send(thread *th);
thread *syscall_recieve(thread *th);
thread *syscall_message_data(thread *th);
#endif
