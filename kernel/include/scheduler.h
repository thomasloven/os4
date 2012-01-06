#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <thread.h>

void init_scheduler();
void scheduler_insert(thread *th);
void scheduler_remove(thread *th);
thread *scheduler_next();
void scheduler_wake(thread_queue *q);
void scheduler_wake_all(thread_queue *q, uint32_t status);
void schedule_list();

#endif
