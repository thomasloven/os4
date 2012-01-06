#include <heap.h>
#include <ctype.h>
#include <stdint.h>
#include <memory.h>
#include <semaphores.h>
#include <syscalls.h>

semaphore heap_sem = 0;
volatile uint32_t heap_max = HEAP_START;
volatile chunk_t *heap_first = 0;

void expand_heap(uint32_t start, uint32_t length)
{

	_syscall_sbrk(length);
	heap_max += length;
}

void contract_heap(chunk_t *c)
{
	if(c->next)
		return;
	if(c == heap_first)
	{
		heap_first = 0;
	}
	else
	{
		c->prev->next = 0;
	}
	
	_syscall_sbrk((uint32_t)c - heap_max);
	heap_max -= (heap_max - (uint32_t)c);
}

void split_chunk(chunk_t *c, uint32_t length)
{
	if((c->size - length) > sizeof(chunk_t))
	{
		chunk_t *new = (chunk_t *)((uint32_t)c + length);
		new->prev = c;
		new->next = c->next;
		new->size = c->size - length;
		new->allocated = FALSE;
		
		c->next = new;
		c->size = length;
	}
}

void glue_chunk(chunk_t *c)
{
	if(c->next)
	{
		if(!c->next->allocated)
		{
			c->size = c->size + c->next->size;
			c->next = c->next->next;
			if(c->next) c->next->prev = c;
		}
	}
	
	if(c->prev)
	{
		if(!c->prev->allocated)
		{
			c->prev->size = c->prev->size + c->size;
			c->prev->next = c->next;
			if(c->next) c->next->prev = c->prev;
			c = c->prev;
		}
	}
	
	if(!c->next)
		contract_heap(c);
}

void free(void *a)
{
	spin(&heap_sem);
	if((uint32_t)a < HEAP_START || (uint32_t)a > HEAP_END)
		return;
		
	chunk_t *c = (chunk_t *)((uint32_t)a - sizeof(chunk_t));
	c->allocated = FALSE;
	glue_chunk(c);
	heap_sem = 0;
}

void *malloc(uint32_t size)
{
	spin(&heap_sem);
	size += sizeof(chunk_t);
	chunk_t *current = heap_first;
	chunk_t *prev = 0;
	while(current)
	{
		if(!current->allocated && current->size >= size)
		{
			
			split_chunk(current, size);
			current->allocated = TRUE;
			heap_sem = 0;
			return (void *)((uint32_t)current + sizeof(chunk_t));
		}
		prev = current;
		current = current->next;
	}
	
	if(prev)
	{
		current = (chunk_t *)((uint32_t)prev + prev->size);
	} else {
		heap_first = current = (chunk_t *)HEAP_START;
	}
	
	expand_heap((uint32_t)current, size);
	memset((uint8_t *)current, 0, sizeof(chunk_t));
	current->prev = prev;
	current->next = 0;
	current->allocated = TRUE;
	current->size = size;
	if(prev) prev->next = current;
	
	heap_sem = 0;
	return (void *)((uint32_t)current + sizeof(chunk_t));
}

void *calloc(uint32_t size)
{
	void *ret = malloc(size);
	memset((uint8_t *)ret, 0, size);
	return ret;
}

void *valloc(uint32_t size)
{
	uint32_t pos = (uint32_t)malloc(size + PAGE_SIZE);
	chunk_t *old = (chunk_t *)(pos - sizeof(chunk_t));
	
	split_chunk(old, PAGE_SIZE - (pos % PAGE_SIZE));
	chunk_t *new = old->next;
	
	split_chunk(new, size + sizeof(chunk_t));
	new->allocated = TRUE;
	
	free((void *)pos);
	
	return (void *)((uint32_t)new + sizeof(chunk_t));
}
