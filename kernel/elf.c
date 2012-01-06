#include <elf.h>
#include <stdint.h>
#include <vmm.h>
#include <k_debug.h>
void load_elf_segment(elf_header *image, elf_phead *phead)
{

	uint8_t *img = (uint8_t *)image;
	uint32_t first_page = ((phead->p_vaddr)&PAGE_MASK);
	uint32_t num_pages = ((phead->p_memsz + PAGE_SIZE)&PAGE_MASK)/PAGE_SIZE;
	uint32_t i;
	for(i = 0; i < num_pages; i++)
	{
		vmm_page_set(first_page + i*PAGE_SIZE, VMM_PAGE_VAL(pmm_alloc_page(), PAGE_PRESENT | PAGE_WRITE | PAGE_USER));
	}
	k_memcopy((uint8_t *)phead->p_vaddr, (uint8_t *)&img[phead->p_offset], phead->p_filesz);
	k_memset((uint8_t *)(phead->p_vaddr + phead->p_filesz), 0, phead->p_memsz - phead->p_filesz);
}

void load_elf(elf_header *image)
{
	elf_phead *program_head = (elf_phead *)((uint32_t)image + image->elf_phoff);
	uint32_t i;
	for(i = 0; i < image->elf_phnum; i++)
	{
		load_elf_segment(image, &program_head[i]);
	}
}
