#include <k_messaging.h>
#include <stdint.h>
#include <process.h>
#include <vmm.h>
#include <k_heap.h>
#include <thread.h>
#include <debug.h>
#include <pmm.h>

int32_t message_send(uint32_t dst, uint32_t src, uint32_t length, uint8_t *data)
{
	process *p = get_process(dst);
	
	if(!p)
		return -1;
		
	message *msg = (message *)kcalloc(sizeof(message));
	msg->dst = dst;
	msg->src = src;
	process *srcp = get_process(src);
	msg->srcnum = srcp->pnum;
	msg->length = length;
	msg->data = data;
	
	if(p->messages.first == 0)
	{	
		p->messages.first = p->messages.last = msg;
	}
	else
	{
		p->messages.last->next = msg;
		p->messages.last = msg;
	}
	return 0;
}

int32_t message_recieve(thread *th, uint8_t *buffer)
{
	// Copy message from other page directory
	message *msg = th->proc->messages.first;
	if(msg == 0)
		return -1;
	uint32_t position = (uint32_t)msg->data;
	process *src = get_process(msg->src);
	if(!src)
		return -1;
	if(src->pnum != msg->srcnum)
		return -1;
	vmm_exdir_set(src->page_directory);
	
	uint32_t length = msg->length;
	uint32_t page, left;
	uint32_t copied = 0;
	while(length)
	{
		page = vmm_expage_get(position) & PAGE_MASK;
		left = PAGE_SIZE - (position % PAGE_SIZE);
		if(left > length) left = length;
		vmm_page_set(VMM_TEMP1, VMM_PAGE_VAL(page, PAGE_PRESENT));
		vmm_flush_tlb(VMM_TEMP1);
		k_memcopy((uint8_t *)((uint32_t)buffer + copied), (uint8_t *)(VMM_TEMP1 + (position % PAGE_SIZE)), left);
		vmm_page_set(VMM_TEMP1,0);
		vmm_flush_tlb(VMM_TEMP1);
		length = length - left;
		position += left;
		copied += left;
	}
	vmm_exdir_set(0);
	message_discard(th);
	return copied;
}

int32_t message_sender(thread *th)
{
	message *msg = th->proc->messages.first;
	if(!msg)
		return -1;
		
	return msg->src;
}

int32_t message_length(thread *th)
{
	message *msg = th->proc->messages.first;
	if(!msg)
		return -1;
		
	return msg->length;
}

void message_discard(thread *th)
{
	message *msg = th->proc->messages.first;
	if(msg)
	{
		if(th->proc->messages.last == msg)
			th->proc->messages.last = 0;
		th->proc->messages.first = msg->next;
		kfree(msg);
	}
}
