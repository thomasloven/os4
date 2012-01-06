#include <stdint.h>
#include <k_debug.h>
#include <multiboot.h>
#include <pmm.h>
#include <vmm.h>
#include <scheduler.h>
#include <elf.h>
#include <k_syscalls.h>
#include <process.h>
#include <k_messaging.h>


thread *kinit(mboot_info *mboot)
{
	init_debug();
	k_printf("\nHello, world!");
	init_tss();
	init_idt();
	init_pmm(mboot);
	init_vmm();
	init_timer(500);
	init_syscalls();

	mboot_mod *modules = (mboot_mod *)(ASSERT_HIGHER(mboot->mods_addr));
	elf_header *init_image = (elf_header *)(ASSERT_HIGHER(modules[0].mod_start));
	

	process *init = init_processes(init_image->elf_entry);
	load_elf(init_image);
	uint32_t length = modules[1].mod_end - modules[1].mod_start;
	message_send(init->pid, init->pid, length, (uint8_t *)(ASSERT_HIGHER(modules[1].mod_start)));

	return switch_thread(0, scheduler_next());
}