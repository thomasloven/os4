#include <k_messaging.h>
#include <stdint.h>
#include <process.h>
#include <vmm.h>
#include <k_heap.h>
#include <thread.h>
#include <debug.h>
#include <pmm.h>
#include <ipc.h>

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
	uint32_t copied = pull(buffer, (uint8_t *)msg->data, msg->src, msg->length);
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
