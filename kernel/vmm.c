#include <stdint.h>
#include <vmm.h>
#include <pmm.h>
#include <thread.h>
#include <process.h>
#include <k_debug.h>

uint32_t *page_directory = (uint32_t *)VMM_PAGE_DIR;
uint32_t *page_tables = (uint32_t *)VMM_PAGE_TABLES;

uint32_t *expage_directory = (uint32_t *)VMM_EXPAGE_DIR;
uint32_t *expage_tables = (uint32_t *)VMM_EXPAGE_TABLES;

extern uint32_t *BootPageDirectory;

void vmm_flush_tlb(uint32_t page)
{
	page &= PAGE_MASK;
	asm volatile ("invlpg (%0)" : : "a" (page));
}

void vmm_clear_page(uint32_t physical)
{
	vmm_page_set(VMM_TEMP1, VMM_PAGE_VAL(physical & PAGE_MASK, PAGE_PRESENT | PAGE_WRITE));
	vmm_flush_tlb(VMM_TEMP1);
	k_memset((uint8_t *)VMM_TEMP1, 0, PAGE_SIZE);
	vmm_page_set(VMM_TEMP1, 0);
	vmm_flush_tlb(VMM_TEMP1);
}

void vmm_copy_page(uint32_t dst, uint32_t src)
{
	vmm_page_set(VMM_TEMP1, VMM_PAGE_VAL(dst & PAGE_MASK, PAGE_PRESENT | PAGE_WRITE));
	vmm_flush_tlb(VMM_TEMP1);
	vmm_page_set(VMM_TEMP2, VMM_PAGE_VAL(src & PAGE_MASK, PAGE_PRESENT));
	vmm_flush_tlb(VMM_TEMP2);
	k_memcopy((uint8_t *)VMM_TEMP1, (uint8_t *) VMM_TEMP2, PAGE_SIZE);
	vmm_page_set(VMM_TEMP1, 0);
	vmm_flush_tlb(VMM_TEMP1);
	vmm_page_set(VMM_TEMP2, 0);
	vmm_flush_tlb(VMM_TEMP2);
}


uint32_t vmm_page_get(uint32_t page)
{
	page &= PAGE_MASK;
	if(page_directory[VMM_DIR_IDX(page)])
		if(page_tables[VMM_TABLE_IDX(page)])
			return page_tables[VMM_TABLE_IDX(page)];
	return 0;
}

void vmm_page_touch(uint32_t page, uint32_t flags)
{
	page &= PAGE_MASK;
	flags &= PAGE_FLAG_MASK;
	if(page_directory[VMM_DIR_IDX(page)] == 0)
	{
		page_directory[VMM_DIR_IDX(page)] = (uint32_t)pmm_alloc_page() | flags;
		vmm_clear_page(page_directory[VMM_DIR_IDX(page)]& PAGE_MASK);
		vmm_flush_tlb((uint32_t)&page_tables[VMM_TABLE_IDX(page)] & PAGE_MASK);
		k_memset((uint8_t *)((uint32_t)&page_tables[VMM_TABLE_IDX(page)] & PAGE_MASK), 0, PAGE_SIZE);
	}
}

void vmm_page_set(uint32_t page, uint32_t value)
{
	page &= PAGE_MASK;
	vmm_page_touch(page, value & PAGE_FLAG_MASK);
	page_tables[VMM_TABLE_IDX(page)] = value;
	vmm_flush_tlb(page);
}

void vmm_exdir_set(uint32_t dir)
{
	if(dir)
		page_directory[VMM_DIR_IDX(VMM_EXPAGE_DIR)] = dir | PAGE_PRESENT | PAGE_WRITE;
	else
		page_directory[VMM_DIR_IDX(VMM_EXPAGE_DIR)] = 0;
	flush_pagedir();
}

uint32_t vmm_expage_get(uint32_t page)
{
	page &= PAGE_MASK;
	if(expage_directory[VMM_DIR_IDX(page)])
		if(expage_tables[VMM_TABLE_IDX(page)])
			return expage_tables[VMM_TABLE_IDX(page)];
	return 0;
}

void vmm_expage_touch(uint32_t page, uint32_t flags)
{
	page &= PAGE_MASK;
	flags &= PAGE_FLAG_MASK;
	
	if(!expage_directory[VMM_DIR_IDX(page)])
	{
		expage_directory[VMM_DIR_IDX(page)] = (uint32_t)pmm_alloc_page() | flags;
		vmm_clear_page(expage_directory[VMM_DIR_IDX(page)& PAGE_MASK]);
		vmm_flush_tlb((uint32_t)&expage_tables[VMM_TABLE_IDX(page)] & PAGE_MASK);
		k_memset((uint8_t *)((uint32_t)&expage_tables[VMM_TABLE_IDX(page)] & PAGE_MASK), 0, PAGE_SIZE);
	}
}

void vmm_expage_set(uint32_t page, uint32_t value)
{
	page &= PAGE_MASK;
	vmm_page_touch(page, value & PAGE_FLAG_MASK);
	expage_tables[VMM_TABLE_IDX(page)] = value;
}

uint32_t vmm_create_pd()
{
	uint32_t pd = pmm_alloc_page();
	vmm_page_set(VMM_TEMP1, VMM_PAGE_VAL(pd & PAGE_MASK, PAGE_PRESENT | PAGE_WRITE));
	vmm_flush_tlb(VMM_TEMP1);
	k_memset((uint8_t *)VMM_TEMP1, 0, PAGE_SIZE);
	uint32_t *dir = (uint32_t *)VMM_TEMP1;
	dir[1023] = VMM_PAGE_VAL(pd, PAGE_PRESENT | PAGE_WRITE);
	vmm_page_set(VMM_TEMP1, 0);
	vmm_flush_tlb(VMM_TEMP1);
	return pd;
}

void vmm_clone_table(uint32_t table)
{
	uint32_t page;
	for(page = 0; page < VMM_PAGES_PER_TABLE; page++)
	{
		uint32_t entry = table * VMM_PAGES_PER_TABLE + page;
		if(page_tables[entry])
		{
			if(entry < VMM_TABLE_IDX(KERNEL_MEM_MAX))
			{
				expage_tables[entry] = VMM_PAGE_VAL(pmm_alloc_page(), page_tables[entry] & PAGE_FLAG_MASK);
				vmm_copy_page(expage_tables[entry] & PAGE_MASK, page_tables[entry] & PAGE_MASK);
			}
		}
	}
}

uint32_t vmm_clone_directory()
{	
	uint32_t new_dir = vmm_create_pd();
	vmm_exdir_set(new_dir);
	uint32_t table;
	for(table = 0; table < VMM_DIR_IDX(KERNEL_OFFSET); table++)
	{
		if(page_directory[table])
		{
			expage_directory[table] = VMM_PAGE_VAL(pmm_alloc_page(), page_directory[table] & PAGE_FLAG_MASK);
			vmm_clear_page(expage_directory[table] & PAGE_MASK);
			vmm_clone_table(table);
		}
	}
	for(table = VMM_DIR_IDX(KERNEL_OFFSET); table < 1022; table++)
	{
		expage_directory[table] = page_directory[table];
	}
	expage_directory[VMM_DIR_IDX(VMM_EXPAGE_DIR)] = 0;
	vmm_exdir_set(0);
	return new_dir;
}

void init_vmm()
{
	install_int_handler(14, &pf_handler);
}

thread *pf_handler(thread *th)
{
	uint32_t fault_address;
	asm volatile ("mov %%cr2, %0" : "=r" (fault_address));
	
	if(fault_address <= th->stack && fault_address >(th->stack - TH_STACK_SIZE))
	{
		vmm_page_set(fault_address & PAGE_MASK, VMM_PAGE_VAL(pmm_alloc_page(), PAGE_PRESENT | PAGE_WRITE | PAGE_USER));
		return th;
	}
	k_printf("\n\nPAGE FAULT! %x %x", th->proc->pid, th->tid);
	k_printf("\n Address: %x", fault_address);
	k_printf("\n From: %x", th->r.eip);
	k_printf("\n Code: %x:%b",th->r.err_code, th->r.err_code);
	if(th->r.err_code &0x1)k_printf(" Not present ");
	if(th->r.err_code &0x2)k_printf(" Write ");
	if(th->r.err_code &0x4)k_printf(" User ");
	for(;;);
	return th;
}