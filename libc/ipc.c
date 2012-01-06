#include <ipc.h>
#include <syscalls.h>
#include <debug.h>
#include <semaphores.h>

volatile signal_queue sig_queue;
semaphore sig_queue_sem = 0;

extern void _on_event(void);

sig_handler signal_handler[NUM_SIGNALS];

sig_handler default_handler[NUM_SIGNALS];

void _event(uint32_t sender, uint32_t signum)
{
	_syscall_printf("Got signal %x ", signum);
	_syscall_printf("From %x ", sender);
	_syscall_done();
}

void signal(uint32_t signum, uint32_t *handler, uint32_t *old)
{
	_syscall_when((uint32_t)handler, signum);
	if(old)
		*old = (uint32_t)signal_handler[signum];
	signal_handler[signum] = (sig_handler)handler;
}

void kill(uint32_t pid, uint32_t signum)
{
	_syscall_signal(pid, signum);
}

void signals_init()
{
	sig_queue.first = 0;
	sig_queue.last = 0;
	memset((uint8_t *)signal_handler, 0, sizeof(sig_handler)*NUM_SIGNALS);
	memset((uint8_t *)default_handler, 0, sizeof(sig_handler)*NUM_SIGNALS);
	int i;
	for(i = 0x10; i < NUM_SIGNALS; i++)
		signal(i,(uint32_t *)signal_buffer,0);
	signal(0xAB,(uint32_t *)_event,0);
}

void signal_buffer(uint32_t sender, uint32_t signum)
{
	spin(&sig_queue_sem);
	signal_t *new = (signal_t *)malloc(sizeof(signal_t));
	new->signum = signum;
	new->sender = sender;
	if(sig_queue.first == 0)
		sig_queue.first = sig_queue.last = new;
	else
		sig_queue.last->next = new;
	new->next = 0;
	sig_queue_sem = 0;
	_syscall_wake(0);
	_syscall_done();
}

int32_t sigpending(uint32_t signum, uint32_t sender)
{

	
	volatile signal_t *s = sig_queue.first;
	int32_t retval = -1;

	if(!s)
	{
		return retval;
	}

	if(s->signum == signum)
	{
		if(sender && s->sender != sender)
		{
		} else {
			sig_queue.first = s->next;
			retval = s->sender;
			free(s);
			return retval;
		}
	}
	while(s->next)
	{
		if(s->next->signum == signum && s->sender == sender)
		{
			if(sender && s->next->sender != sender)
				continue;
			retval = s->next->sender;
			signal_t *r = s->next;
			s->next = s->next->next;
			free(r);
			return retval;
		}
		s = s->next;
	}
	return retval;
}

int32_t waitsig(uint32_t signum, uint32_t pid)
{
	spin(&sig_queue_sem);
	int32_t pending = sigpending(signum, pid);
	while(pending < 0 )
	{
		_syscall_pause(&sig_queue_sem);
		spin(&sig_queue_sem);
		pending = sigpending(signum, pid);
		sig_queue_sem = 0;
		return pending;
	}
	sig_queue_sem = 0;
}

void send(uint32_t pid, uint32_t length, uint8_t *message)
{
	_syscall_send(pid,length,message);
	kill(pid,SIGMSG);
	waitsig(SIGMAC,pid);
}

uint32_t recieve(uint32_t pid, uint8_t *buffer)
{
	uint32_t p = 0;
	uint32_t length = 0;
	if(pid)
	{
	while(p != pid)
	{
		length = _syscall_message_data(&p);
		if(length == -1)
			return -1;
		if(p != pid)
			_syscall_recieve(0);
	}
	} else {
		length = _syscall_message_data(0);
	}
	_syscall_recieve(buffer);
	kill(pid,SIGMAC);
	return length;
}

int32_t msglen(uint32_t pid)
{
	uint32_t p = 0;
	int32_t length = 0;
	if(pid)
	{
	while (p != pid && length > -1)
	{
		length =  _syscall_message_data(&p);
		if(p != pid)
			_syscall_recieve(0);
	}
	} else {
		length = _syscall_message_data(0);
	}
	return length;
}

int32_t msgsrc()
{
	uint32_t p = -1;
	_syscall_message_data(&p);
	return p;
}

void *waitmsg(uint32_t pid, uint32_t *sender)
{
	uint32_t sndr = waitsig(SIGMSG,pid);
	void *message = (void *)malloc(msglen(sndr));
	recieve(sndr,(uint8_t *)message);
	if(sender)
		sender[0] = sndr;
	return message;
}

