#ifndef KHEAP_H
#define KHEAP_H

#include <stdint.h>
#include <heap.h>

void expand_heap(uint32_t start, uint32_t length);
void contract_heap(chunk_t *c);
void split_chunk(chunk_t *c, uint32_t length);
void glue_chunk(chunk_t *c);
void kfree(void *a);
void *kmalloc(uint32_t size);
void *kcalloc(uint32_t size);
void *kvalloc(uint32_t size);

#endif
