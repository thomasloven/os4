#include <stdint.h>
#include <process.h>
#include <thread.h>
#include <int.h>
#include <scheduler.h>
#include <k_debug.h>

process *processes[PROC_NUM];
uint32_t num_process = 1;

process *new_process()
{
	uint8_t i;
	for(i = 0; i < PROC_NUM; i++)
		if(!processes[i])
			break;
	if(i == PROC_NUM)
		return 0;
		
	processes[i] = (process *)kcalloc(sizeof(process));
	
	processes[i]->pid = i;
	processes[i]->pnum = num_process++;
	
	return processes[i];
}

process *fork_process(process *parent, thread *th)
{
	process *child = new_process();
	if(!child)
	{
		return 0;
	}
	uint32_t pid = child->pid;
	uint32_t pnum = child->pnum;
	k_memcopy((uint8_t *)child, (uint8_t *)parent, sizeof(process));
	child->pid = pid;
	child->pnum = pnum;
	child->ppid = parent->pid;
	
	k_memset((uint8_t *)child->threads, 0, sizeof(thread *)*PROC_NUM_THREADS);
	
	thread *new_th = new_thread(th);
	
	if(!bind_thread(new_th,child))
	{
		kfree(child);
		kfree(new_th);
		return 0;
	}
	
	scheduler_insert(new_th);
	child->page_directory = vmm_clone_directory();
	
	return child;
}

void free_process(process *p)
{
	processes[p->pid] = 0;
	kfree(p);
}

void kill_process(process *p, uint32_t status)
{
	uint32_t i;
	for(i = 0; i < 256; i++)
	{
		if(p->threads[i])
		{
			free_thread(p->threads[i]);
		}
	}
	scheduler_wake_all(&p->waiting, status);
	free_process(p);
}

void switch_process(process *p)
{
	set_pagedir(p->page_directory);
}

uint32_t wait_process(uint32_t pid, thread *th)
{
	if(!processes[pid])
		return -1;

	queue_move(&processes[pid]->waiting, th->queue, th);

	return 0;
}

process *get_process(uint32_t pid)
{
	return processes[pid];
}

process *init_processes(uint32_t init_addr)
{
	process *idle = new_process();
	idle->page_directory = get_pagedir();
	
	process *init = fork_process(idle, 0);
	switch_process(init);
	
	init->threads[0]->r.ss = SEG_USER_DATA | 0x3;
	init->threads[0]->r.ds = SEG_USER_DATA | 0x3;
	init->threads[0]->r.cs = SEG_USER_CODE | 0x3;
	
	init->threads[0]->r.esp = (uint32_t)&init->threads[0]->r.int_no;
	init->threads[0]->r.useresp = init->threads[0]->stack;
	init->threads[0]->r.ebp = init->threads[0]->r.useresp;

	asm volatile ("pushf; \
		pop %%eax" : "=a" (init->threads[0]->r.eflags));
	init->threads[0]->r.eip = (uint32_t)init_addr;
	
	return init;
}
