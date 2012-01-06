#include <scheduler.h>
#include <queue.h>
#include <k_debug.h>
#include <process.h>

thread_queue main_queue;


void init_scheduler()
{
	queue_init(&main_queue);
}

void scheduler_insert(thread *th)
{
	queue_insert(&main_queue, th);
}

void scheduler_remove(thread *th)
{
	queue_remove(&main_queue, th);
}

thread *scheduler_next()
{
	thread *ret = queue_next(&main_queue);
	return ret;
}

void scheduler_wake(thread_queue *q)
{
	queue_move(&main_queue, q, q->first);
}

void scheduler_wake_all(thread_queue *q, uint32_t status)
{
	thread *t = q->first;
	while(t)
	{
		t->r.eax = status;
		t = t->next;
	}
	queue_move_all(&main_queue, q);
}

void schedule_list()
{
	queue_list(&main_queue);
}
