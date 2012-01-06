#include <queue.h>
#include <stdint.h>
#include <thread.h>
#include <k_debug.h>
#include <process.h>
#include <semaphores.h>

void queue_init(thread_queue *q)
{
	q->first = 0;
	q->last = 0;
	q->sem = 0;
}

void queue_list(thread_queue *q)
{
	k_printf("\nThread queue");
	thread *t = q->first;
	while(t)
	{
		k_printf("\n %x %x",t->tid,t->proc->pid);
		t = t->next;
	}
}

void queue_insert(thread_queue *q, thread *th)
{
	if(th->queue)
		queue_remove(th->queue, th);
	spin(&q->sem);
	th->queue = q;
	if(q->first)
	{
		q->last->next = th;
		q->last = th;
	} else {
		q->first = q->last = th;
	}
	th->next = 0;
	q->sem = 0;
}

void queue_remove(thread_queue *q, thread *th)
{
	spin(&q->sem);
	th->queue = 0;
	if(q->first == th)
	{
		q->first = q->first->next;
	} else {
		thread *i = q->first;
		while(i)
		{
			if(i->next == th)
			{
				if(i->next == q->last)
					q->last = i;
				i->next = i->next->next;
				q->sem = 0;
				return;
			}
			i = i->next;
		}
	}
	q->sem = 0;
}

thread *queue_next(thread_queue *q)
{
	thread *next = q->first;
	if(next)
	{
		q->first = next->next;
		queue_insert(q, next);
	}
	return next;
}

void queue_move(thread_queue *dst, thread_queue *src, thread *th)
{
	if(th)
	{
		queue_remove(src, th);
		queue_insert(dst, th);
	}
}

void queue_move_all(thread_queue *dst, thread_queue *src)
{
	thread *th;
	while(th = queue_next(src))
		queue_move(dst, src, th);
}
