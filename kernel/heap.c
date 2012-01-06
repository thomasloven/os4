#include <k_heap.h>
#include <vmm.h>
#include <pmm.h>
#include <ctype.h>
#include <stdint.h>

uint32_t heap_max = KERNEL_HEAP_START;
chunk_t *heap_first = 0;

void expand_heap(uint32_t start, uint32_t length)
{
	while(start+length > heap_max)
	{
		vmm_page_set(heap_max, VMM_PAGE_VAL(pmm_alloc_page(), PAGE_PRESENT | PAGE_WRITE));
		k_memset((uint8_t *)heap_max, 0, PAGE_SIZE);
		heap_max += PAGE_SIZE;
	}
}

void contract_heap(chunk_t *c)
{
	if(c->next)
		return;
	if(c == heap_first)
		heap_first = 0;
	else
		c->prev->next = 0;
		
	while(heap_max - PAGE_SIZE >= (uint32_t)c)
	{
		pmm_free_page(vmm_page_get(heap_max) & PAGE_MASK);
		vmm_page_set(heap_max, 0);
		heap_max -= PAGE_SIZE;
	}
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

void kfree(void *a)
{
	if((uint32_t)a < KERNEL_HEAP_START || (uint32_t)a > KERNEL_HEAP_END)
		return;
		
	chunk_t *c = (chunk_t *)((uint32_t)a - sizeof(chunk_t));
	c->allocated = FALSE;
	glue_chunk(c);
}

void *kmalloc(uint32_t size)
{
	size += sizeof(chunk_t);
	chunk_t *current = heap_first;
	chunk_t *prev = 0;
	
	while(current)
	{
		if(!current->allocated && current->size >= size)
		{
			split_chunk(current, size);
			current->allocated = TRUE;
			return (void *)((uint32_t)current + sizeof(chunk_t));
		}
		prev = current;
		current = current->next;
	}
	
	if(prev)
	{
		current = (chunk_t *)((uint32_t)prev + prev->size);
	} else {
		heap_first = current = (chunk_t *)KERNEL_HEAP_START;
	}
	
	expand_heap((uint32_t)current, size);
	k_memset((uint8_t *)current, 0, sizeof(chunk_t));
	current->prev = prev;
	current->next = 0;
	current->allocated = TRUE;
	current->size = size;
	if(prev) prev->next = current;
	
	return (void *)((uint32_t)current + sizeof(chunk_t));
}

void *kcalloc(uint32_t size)
{
	void *ret = kmalloc(size);
	k_memset((uint8_t *)ret, 0, size);
	return ret;
}

void *kvalloc(uint32_t size)
{
	uint32_t pos = (uint32_t)kmalloc(size + PAGE_SIZE);
	chunk_t *old = (chunk_t *)(pos - sizeof(chunk_t));
	
	split_chunk(old, PAGE_SIZE - (pos % PAGE_SIZE));
	chunk_t *new = old->next;
	
	split_chunk(new, size + sizeof(chunk_t));
	new->allocated = TRUE;
	
	kfree((void *)pos);
	
	return (void *)((uint32_t)new + sizeof(chunk_t));
}
