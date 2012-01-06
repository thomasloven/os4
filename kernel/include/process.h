#ifndef PROCESS_H
#define PROCESS_H

typedef struct thread_struct thread;
typedef struct thread_queue_struct thread_queue;
typedef struct message_queue_struct message_queue;

#include <stdint.h>
#include <queue.h>
#include <k_messaging.h>
#include <heap.h>
#include <ipc.h>

#define PROC_NUM_THREADS 256
#define PROC_NUM 256

#define PROC_HEAP_MAX_SIZE (HEAP_END - HEAP_START)

typedef struct process_struct
{
	uint32_t pnum;
	uint32_t pid;
	uint32_t ppid;
	thread *threads[PROC_NUM_THREADS];
	uint32_t page_directory;
	uint32_t sig_handler[NUM_SIGNALS];
	thread_queue waiting;
	thread_queue paused;
	message_queue messages;
	uint32_t heap_max;
	uint32_t heap_end;
} process;

process *new_process();
process *fork_process(process *parent, thread *th);
void free_process(process *p);
void kill_process(process *p, uint32_t status);
void switch_process(process *p);
uint32_t wait_process(uint32_t pid, thread *th);
process *get_process(uint32_t pid);
process *init_processes(uint32_t init_addr);


#endif
