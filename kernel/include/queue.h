#ifndef QUEUE_H
#define QUEUE_H

typedef struct thread_struct thread;
typedef struct thread_queue_struct thread_queue;

#include <thread.h>
#include <semaphores.h>

typedef struct thread_queue_struct
{
	thread *first;
	thread *last;
	semaphore sem;
} thread_queue;


void queue_init(thread_queue *q);
void queue_list(thread_queue *q);
void queue_insert(thread_queue *q, thread *th);
void queue_remove(thread_queue *q, thread *th);
thread *queue_next(thread_queue *q);
void queue_move(thread_queue *dst, thread_queue *src, thread *th);
void queue_move_all(thread_queue *dst, thread_queue *src);

#endif
