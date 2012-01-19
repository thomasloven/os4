#include <k_syscalls.h>
#include <thread.h>
#include <int.h>
#include <process.h>
#include <scheduler.h>
#include <k_debug.h>
#include <elf.h>
#include <vmm.h>
#include <pmm.h>
#include <k_messaging.h>
#include <ipc.h>

void init_syscalls()
{
	install_int_handler(128, &syscall_handler);
	install_int_handler(255, &syscall_handler);
}

thread *syscall_handler(thread *th)
{
	switch(th->r.eax)
	{
		case SYSCALL_FORK:
			return syscall_fork(th);
		case SYSCALL_EXIT:
			return syscall_exit(th);
		case SYSCALL_DONE:
			return syscall_done(th);
		case SYSCALL_GETPID:
			return syscall_getpid(th);
		case SYSCALL_GETPPID:
			return syscall_getppid(th);
		case SYSCALL_SIGNAL:
			return syscall_signal(th);
		case SYSCALL_YIELD:
			return syscall_unsuported(th);
		case SYSCALL_EXEC:
			return syscall_exec(th);
		case SYSCALL_WAITPID:
			return syscall_waitpid(th);
		case SYSCALL_WHEN:
			return syscall_when(th);
		case SYSCALL_PAUSE:
			return syscall_pause(th);
		case SYSCALL_WAKE:
			return syscall_wake(th);
			
		case SYSCALL_SEND:
			return syscall_send(th);
		case SYSCALL_RECV:
			return syscall_recieve(th);
		case SYSCALL_MSGD:
			return syscall_message_data(th);
		case SYSCALL_PULL:
			return syscall_pull(th);
		case SYSCALL_PUSH:
			return syscall_push(th);

		case SYSCALL_MAP:
			return syscall_unsuported(th);
		case SYSCALL_UMAP:
			return syscall_unsuported(th);
		case SYSCALL_SBRK:
			return syscall_sbrk(th);
			
		case SYSCALL_PRINTF:
			return syscall_printf(th);
	}
}

thread *syscall_unsuported(thread *th)
{
	k_printf("\nSyscall was called %x", th->r.eax);
	th->r.eax = SYSC_UNSUP;
	return th;
}

thread *syscall_fork(thread *th)
{
	th->r.ebx = 0;
	th->r.eax = SYSC_UNSUP;
	process *p = fork_process(th->proc, th);
	th->r.ebx = p->pid;
	return switch_thread(0, scheduler_next());
}

thread *syscall_printf(thread *th)
{
	setColor(th->proc->pid);
	k_printf((char *)th->r.ebx, th->r.ecx);
	setColor(0);
	th->r.eax = SYSC_OK;
	return th;
}

thread *syscall_exit(thread *th)
{
	kill_process(th->proc, th->r.ebx);
	return switch_thread(0, scheduler_next());
}

thread *syscall_done(thread *th)
{
	free_thread(th);
	return switch_thread(0, scheduler_next());
}

thread *syscall_getpid(thread *th)
{
	th->r.ebx = th->proc->pid;
	th->r.eax = SYSC_OK;
	return th;
}

thread *syscall_getppid(thread *th)
{
	th->r.ebx = th->proc->ppid;
	th->r.eax = SYSC_OK;
	return th;
}

thread *syscall_signal(thread *th)
{
	process *p = get_process(th->r.ebx);
	if(!p)
	{
		th->r.eax = SYSC_FAIL;
		return th;
	}
	if(p->sig_handler[th->r.ecx] == SIG_IGN)
	{
		th->r.eax = SYSC_OK;
		return th;
	} else if(p->sig_handler[th->r.ecx] == SIG_DFL)
	{
		kill_process(p, 1);
		if(th)
		{
			th->r.eax = SYSC_OK;
		} else {
			th = switch_thread(0, scheduler_next());
		}
	} else {
		thread *new = signal_thread(th,th->r.ebx,th->r.ecx);
		scheduler_insert(new);
		th = switch_thread(th, new);
	}

	return th;
}

thread *syscall_exec(thread *th)
{
	elf_header *image = (elf_header *)th->r.ebx;
	load_elf(image);
	th->r.eip = image->elf_entry;
	return th;
}

thread *syscall_waitpid(thread *th)
{
	if(wait_process(th->r.ebx, th))
	{
		th->r.eax = SYSC_FAIL;
		return th;
	} else {
		th = switch_thread(0,scheduler_next());
		return switch_thread(0,scheduler_next());
	}
}

thread *syscall_when(thread *th)
{
	th->proc->sig_handler[th->r.ecx] = th->r.ebx;
	th->r.eax = SYSC_OK;
	return th;
}

thread *syscall_pause(thread *th)
{
	if(th->r.ebx != 0)
	{
		uint32_t *sem = (uint32_t *)th->r.ebx;
		sem[0] = 0;
	}
	queue_move(&th->proc->paused, th->queue, th);
	th = switch_thread(th,scheduler_next());
	return th;
}

thread *syscall_wake(thread *th)
{
	scheduler_wake_all(&th->proc->paused, th->r.ecx);
	return th;
}

thread *syscall_sbrk(thread *th)
{

	int32_t size = (int32_t) th->r.ebx;
	if(size > 0)
	{
		while(th->proc->heap_end+size > th->proc->heap_max)
		{
			vmm_page_set(th->proc->heap_max + HEAP_START, VMM_PAGE_VAL(pmm_alloc_page(), PAGE_PRESENT | PAGE_WRITE | PAGE_USER));
			k_memset((uint8_t *)(th->proc->heap_max + HEAP_START),0,PAGE_SIZE);
			th->proc->heap_max += PAGE_SIZE;
		}
		th->proc->heap_end += size;
	} else {
		uint32_t target_size = (uint32_t)((int32_t)th->proc->heap_end + size);
		if((int32_t)target_size < 0 ) target_size = 0;
		while((int32_t)(th->proc->heap_max-PAGE_SIZE) >= (int32_t)target_size)
		{
			pmm_free_page(vmm_page_get(th->proc->heap_max + HEAP_START) & PAGE_MASK);
			vmm_page_set(th->proc->heap_max + HEAP_START,0);
			th->proc->heap_max -= PAGE_SIZE;
		}
		th->proc->heap_end = target_size;
	}
	return th;
}

thread *syscall_send(thread *th)
{
	message_send(th->r.ebx, th->proc->pid, th->r.ecx, (uint8_t *)th->r.edx);
	return th;
}

thread *syscall_recieve(thread *th)
{
	if(th->r.ebx == 0)
	{
		message_discard(th);
	} else {
		th->r.ecx = message_recieve(th, (uint8_t *)th->r.ebx);
	}
	th->r.eax = SYSC_OK;
	return th;
}

thread *syscall_message_data(thread *th)
{
	th->r.eax = SYSC_OK;
	th->r.ebx = message_sender(th);
	th->r.ecx = message_length(th);
	return th;
}

thread *syscall_pull(thread *th)
{
	// ebx = dst buffer
	// ecx = src data
	// edx = low: src pid, high: length
	th->r.eax = SYSC_OK;
	th->r.ecx = pull(th->r.ebx, th->r.ecx, (th->r.edx & 0xFFFF), (th->r.edx >>0x10));
	return th;
}

thread *syscall_push(thread *th)
{
	// ebx = dst buffer
	// ecx = src data
	// edx = low: src pid, high: length
	th->r.eax = SYSC_OK;
	th->r.ecx = push((th->r.edx & 0xFFFF), th->r.ebx, th->r.ecx, (th->r.edx >>0x10));
	return th;
}