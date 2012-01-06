#ifndef VMM_H
#define VMM_H

#include <stdint.h>
#include <pmm.h>
#include <thread.h>

#define KERNEL_OFFSET 0xC0000000
#define KERNEL_HEAP_START 0xD0000000
#define KERNEL_HEAP_END 0xF0000000
#define PMM_PAGE_STACK 0xFE000000
#define KERNEL_MEM_MAX 0xFF000000
#define VMM_TEMP1 0xFF7FE000
#define VMM_TEMP2 0xFF7FF000
#define VMM_EXPAGE_TABLES 0xFF800000
#define VMM_EXPAGE_DIR 0xFFBFF000
#define VMM_PAGE_TABLES 0xFFC00000
#define VMM_PAGE_DIR 0xFFFFF000

#define PAGE_PRESENT 0x1
#define PAGE_WRITE 0x2
#define PAGE_USER 0x4

extern void _end();

#define VMM_PAGE_VAL(PAGE, FLAGS) (((uint32_t)PAGE & PAGE_MASK) | (FLAGS & PAGE_FLAG_MASK))
#define VMM_TABLE_IDX(PAGE) (PAGE >> 12)
#define VMM_DIR_IDX(PAGE) (PAGE >> 22)
#define VMM_PAGES_PER_TABLE 1024
#define ASSERT_HIGHER(VAR) (VAR=(((uint32_t)(VAR) > KERNEL_OFFSET)?(VAR):((VAR) + KERNEL_OFFSET)))
#define ASSERT_LOWER(VAR) (VAR=(((uint32_t)(VAR) < KERNEL_OFFSET)?(VAR):((VAR) - KERNEL_OFFSET)))

extern void flush_pagedir(void);
extern void set_pagedir(uint32_t pd);
extern uint32_t get_pagedir(void);
extern void flush_tlb(uint32_t addr);

void vmm_flush_tlb(uint32_t page);
void vmm_clear_page(uint32_t physical);
void vmm_copy_page(uint32_t dst, uint32_t src);

uint32_t vmm_page_get(uint32_t page);
void vmm_page_touch(uint32_t page, uint32_t flags);
void vmm_page_set(uint32_t page, uint32_t value);

void vmm_exdir_set(uint32_t dir);
uint32_t vmm_expage_get(uint32_t page);
void vmm_expage_touch(uint32_t page, uint32_t flags);
void vmm_expage_set(uint32_t page, uint32_t value);

uint32_t vmm_create_pd();
uint32_t vmm_clone_directory();

void init_vmm();
thread *pf_handler(thread *th);

#endif
