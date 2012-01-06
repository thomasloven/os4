#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <multiboot.h>

#define PAGE_SIZE	0x1000
#define PAGE_MASK	0xFFFFF000
#define PAGE_FLAG_MASK	0xFFF

void k_memcopy(uint8_t *dst, uint8_t *src, uint32_t len);
void k_memset(uint8_t *dst, uint8_t val, uint32_t len);

uint32_t pmm_alloc_page();
void pmm_free_page(uint32_t p);
void init_pmm(mboot_info *mboot);

#endif
