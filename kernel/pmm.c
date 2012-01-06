#include <stdint.h>
#include <pmm.h>
#include <vmm.h>
#include <ctype.h>
#include <multiboot.h>
#include <k_debug.h>

uint8_t pmm_running;
uint32_t pmm_pos;

uint32_t *pmm_page_stack_top = (uint32_t *)PMM_PAGE_STACK;
uint32_t *pmm_page_stack_max = (uint32_t *)PMM_PAGE_STACK;

void k_memcopy(uint8_t *dst, uint8_t *src, uint32_t len)
{
	for(;len;len--) *dst++ = *src++;
}

void k_memset(uint8_t *dst, uint8_t val, uint32_t len)
{
	for(;len;len--) *dst++ = val;
}

uint32_t pmm_alloc_page()
{
	if(!pmm_running)
	{
		pmm_pos += PAGE_SIZE;
		return (uint32_t)(pmm_pos - PAGE_SIZE);
	} else {
		if((uint32_t)pmm_page_stack_top == PMM_PAGE_STACK)
		{
			// OUT OF MEMORY
			k_printf("OUT OF MEMORY!");
			for(;;);
		}
		pmm_page_stack_top  = (uint32_t *)((uint32_t)pmm_page_stack_top - sizeof(uint32_t *));
		if((uint32_t)pmm_page_stack_top <= ((uint32_t)pmm_page_stack_max-PAGE_SIZE))
		{
			uint32_t ret = vmm_page_get((uint32_t)pmm_page_stack_max - PAGE_SIZE) & PAGE_MASK;
			vmm_page_set(((uint32_t)pmm_page_stack_max - PAGE_SIZE) & PAGE_MASK, 0);
			pmm_page_stack_max -= PAGE_SIZE;
			return (uint32_t)ret;
		}
		return (uint32_t)*pmm_page_stack_top;
	}
}

void pmm_free_page(uint32_t p)
{
	p &= PAGE_MASK;
	
	if(p <= pmm_pos)
		return;
		
	if((uint32_t)pmm_page_stack_top >= (uint32_t)pmm_page_stack_max)
	{
		vmm_page_set((uint32_t)pmm_page_stack_max, VMM_PAGE_VAL(p, PAGE_PRESENT|PAGE_WRITE));
		pmm_page_stack_max = (uint32_t *)((uint32_t)pmm_page_stack_max + PAGE_SIZE);
	} else {
		*(pmm_page_stack_top) = p;
		pmm_page_stack_top = (uint32_t *)((uint32_t)pmm_page_stack_top + sizeof(uint32_t));
	}
	pmm_running = TRUE;
}

void init_pmm(mboot_info *mboot)
{
	
	pmm_pos = (uint32_t)((uint32_t)&_end+PAGE_SIZE)&PAGE_MASK;
	ASSERT_LOWER(pmm_pos);
	pmm_running = FALSE;
	
	ASSERT_HIGHER(mboot->mmap_addr);
	uint32_t i = mboot->mmap_addr;
	while(i < mboot->mmap_addr + mboot->mmap_length)
	{

		mmap_entry *me = (mmap_entry *)i;
		if(me->type == MBOOT_MEM_FLAG_FREE)
		{
			uint32_t j;
			for(j = me->base_addr_lower; j <= (me->base_addr_lower + me->length_lower); j += PAGE_SIZE)
			{
				pmm_free_page(j);
			}
		}
		i += me->size + sizeof(uint32_t);
	}

}
