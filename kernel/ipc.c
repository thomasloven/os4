#include <k_ipc.h>
#include <stdint.h>
#include <pmm.h>
#include <vmm.h>
#include <thread.h>
#include <process.h>

int32_t pull(uint8_t *dst, uint8_t *src, uint32_t proc, uint32_t length)
{
	process *src_proc = get_process(proc);
	if(!src_proc)
		return -1;
	vmm_exdir_set(src_proc->page_directory);
	
	uint32_t page, to_read, copied=0;
	uint32_t write_pos = (uint32_t)dst;
	uint32_t read_pos = (uint32_t)src;
	while(length)
	{
		page = vmm_expage_get(read_pos) & PAGE_MASK;
		if(!page)
			return -1;
		to_read = PAGE_SIZE - (read_pos % PAGE_SIZE);
		if(to_read > length) to_read = length;
		vmm_page_set(VMM_TEMP1, VMM_PAGE_VAL(page, PAGE_PRESENT));
		vmm_flush_tlb(VMM_TEMP1);
		k_memcopy((uint8_t*)write_pos, (uint8_t*)(VMM_TEMP1 + (read_pos % PAGE_SIZE)), to_read);
		vmm_page_set(VMM_TEMP1,0);
		vmm_flush_tlb(VMM_TEMP1);
		length = length - to_read;
		write_pos = write_pos + to_read;
		read_pos = read_pos + to_read;
		copied = copied + to_read;
	}
	vmm_exdir_set(0);
	return copied;
}

int32_t push(uint32_t proc, uint8_t *dst, uint8_t *src, uint32_t length)
{
	process *dst_proc = get_process(proc);
	if(!dst_proc)
		return -1;
	vmm_exdir_set(dst_proc->page_directory);
	
	uint32_t page, to_write, copied = 0;
	uint32_t write_pos = (uint32_t)dst;
	uint32_t read_pos = (uint32_t)src;
	while(length)
	{
		page = vmm_expage_get(write_pos) & PAGE_MASK;
		if(!page)
			return -1;
		to_write = PAGE_SIZE - (write_pos % PAGE_SIZE);
		if(to_write > length) to_write = length;
		vmm_page_set(VMM_TEMP1, VMM_PAGE_VAL(page, PAGE_PRESENT));
		vmm_flush_tlb(VMM_TEMP1);
		k_memcopy((uint8_t*)(VMM_TEMP1 + (write_pos % PAGE_SIZE)), (uint8_t *)read_pos, to_write);
		vmm_page_set(VMM_TEMP1, 0);
		vmm_flush_tlb(VMM_TEMP1);
		length = length - to_write;
		write_pos = write_pos + to_write;
		read_pos = read_pos + to_write;
		copied = copied + to_write;
	}
	vmm_exdir_set(0);
	return copied;
}
