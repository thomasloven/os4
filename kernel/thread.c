#include <thread.h>
#include <stdint.h>
#include <process.h>
#include <vmm.h>
#include <scheduler.h>
#include <queue.h>
#include <int.h>
#include <k_debug.h>

uint32_t num_threads = 1;

thread *new_thread(thread *orig)
{
	thread *th = (thread *)kcalloc(sizeof(thread));
	if(orig != 0)
	{
		k_memcopy((uint8_t *)th, (uint8_t *)orig, sizeof(thread));
	}
	th->tid = num_threads++;
	return th;
}

void free_thread(thread *th)
{
	uint8_t i;
	for(i = 0; i < PROC_NUM_THREADS; i++)
	{
		if(th->proc->threads[i] == th)
		{
			break;
		}
	}
	th->proc->threads[i] = 0;
	queue_remove(th->queue, th);
	kfree(th);
}

uint8_t bind_thread(thread *th, process *p)
{
	uint8_t i;
	for(i = 0; i < PROC_NUM_THREADS; i++)
		if(!p->threads[i])
			break;
	
	if(i == PROC_NUM_THREADS)
		return 0;
	
	p->threads[i] = th;
	th->proc = p;
	
	th->stack = (KERNEL_OFFSET -i*(TH_STACK_SIZE) - TH_STACK_MARGIN);
	return 1;
}

thread *switch_thread(thread *old, thread *new)
{
	if(!new)
	{
		while(1)
			asm volatile ("sti; hlt;");
	}
	
	if(old == 0 || old->proc != new->proc)
	{
		switch_process(new->proc);
	}
	
	new->r.eflags |= EFLAGS_CPL3  | EFLAGS_IF;
	
	global_tss.esp0 = (uint32_t)&new->tid;
	
	return new;
}

thread *signal_thread(thread *sender, uint32_t pid, uint32_t signum)
{
	thread *th = new_thread(0);
	if(!get_process(pid))
		return th;
	bind_thread(th, get_process(pid));
	th->flags = THREAD_NOINT;
	th->r.ss = SEG_USER_DATA | 0x3;
	th->r.ds = SEG_USER_DATA | 0x3;
	th->r.cs = SEG_USER_CODE | 0x3;

	th->r.esp = (uint32_t)&th->r.int_no;
	th->r.useresp = th->stack;
	th->r.ebp = th->r.useresp;
	
	vmm_exdir_set(th->proc->page_directory);
	vmm_expage_set(th->r.useresp-1, VMM_PAGE_VAL(pmm_alloc_page(), PAGE_PRESENT | PAGE_WRITE | PAGE_USER));
	vmm_page_set(VMM_TEMP1, VMM_PAGE_VAL(vmm_expage_get(th->r.useresp-1)&PAGE_MASK, PAGE_PRESENT | PAGE_WRITE));
	uint32_t *stack = (uint32_t *)(VMM_TEMP1 + PAGE_SIZE);
	*stack--;
	*stack-- = signum;
	*stack-- = sender->proc->pid;
	th->r.useresp = th->r.useresp - 3*sizeof(uint32_t);
	vmm_exdir_set(0);
	
	th->r.eip = th->proc->sig_handler[signum];
	
	th->r.ecx = signum;
	if(sender)
		th->r.ebx = sender->proc->pid;

	scheduler_insert(th);
	return th;
}

