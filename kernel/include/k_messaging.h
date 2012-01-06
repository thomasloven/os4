#ifndef KMESSAGING_H
#define KMESSAGING_H

typedef struct message_struct message;

#include <stdint.h>
#include <thread.h>

typedef struct message_struct
{
	uint32_t dst;
	uint32_t src;
	uint32_t srcnum;
	uint32_t length;
	message *next;
	uint8_t *data;
} message;

typedef struct message_queue_struct
{
	message *first;
	message *last;
} message_queue;

int32_t message_send(uint32_t dst, uint32_t src, uint32_t length, uint8_t *data);
int32_t message_recieve(thread *th, uint8_t *buffer);
int32_t message_sender(thread *th);
int32_t message_length(thread *th);
void message_discard(thread *th);

#endif
