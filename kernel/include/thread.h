#ifndef THREAD_H
#define THREAD_H

typedef struct thread_struct thread;
typedef struct process_struct process;

#include <stdint.h>
#include <queue.h>
#include <semaphores.h>
//#include <process.h>

#define TH_STACK_SIZE 0x10000
#define TH_STACK_MARGIN 0x1000

typedef struct registers_struct
{
	uint32_t ds;
	uint32_t edi, esi, ebp, esp;
	uint32_t ebx, edx, ecx, eax;
	uint32_t int_no, err_code;
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags, useresp, ss;
} registers;

typedef struct thread_struct
{
	registers r;
	uint32_t tid;
	uint32_t ptid;
	uint32_t flags;
	thread *next;
	uint32_t stack;
	process *proc;
	thread_queue *queue;
} thread;

#define THREAD_NOINT 0x1

thread *new_thread();
void free_thread(thread *th);
uint8_t bind_thread(thread *th, process *p);
thread *switch_thread(thread *old, thread *new);
thread *signal_thread(thread *sender, uint32_t pid, uint32_t signum);

#endif
