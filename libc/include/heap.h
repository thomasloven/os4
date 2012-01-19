#ifndef HEAP_H
#define HEAP_H

typedef volatile struct chunk_header chunk;

#include <stdint.h>

#define HEAP_START	0xA0000000
#define HEAP_END	0xB0000000
#define PAGE_SIZE	0x1000

typedef volatile struct chunk_header
{
	chunk *prev, *next;
	uint32_t allocated :1;
	uint32_t size :31;
}__attribute__((packed)) chunk_t;

void expand_heap(uint32_t start, uint32_t length);
void contract_heap(chunk_t *c);
void split_chunk(chunk_t *c, uint32_t length);
void glue_chunk(chunk_t *c);
void free(void *a);
void *malloc(uint32_t size);
void *calloc(uint32_t size);
void *valloc(uint32_t size);

#define MALLOCP(NAME, TYPE) TYPE *NAME = (TYPE *)malloc(sizeof(TYPE))
#define CALLOCP(NAME, TYPE) TYPE *NAME = (TYPE *)calloc(sizeof(TYPE))

#endif
