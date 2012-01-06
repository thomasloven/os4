#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#define MBOOT_FLAG_MEM		0x001	// Fields mem_lower and mem_upper are valid
#define MBOOT_FLAG_DEVICE	0x002	// boot_device field is valid
#define MBOOT_FLAG_CMD		0x004	// cmdline field is valid
#define MBOOT_FLAG_MODS		0x008	// mods_count and mods_addr are valid
#define MBOOT_FLAG_AOUT		0x010	// Kernel is in aout format
#define MBOOT_FLAG_ELF		0x020	// Kernel is in elf format
#define MBOOT_FLAG_MMAP		0x040	// mmap_length and mmap_addr are valid
#define MBOOT_FLAG_DRIVES	0x080	// drives_length and drives_addr are valid
#define MBOOT_FLAG_CONFIG	0x100	// config_table is valid
#define MBOOT_FLAG_NAME		0x200	// boot_loader_name is valid
#define MBOOT_FLAG_APM		0x400	// apm_table is valid
#define MBOOT_FLAG_VBE		0x800	// vbe_* fileds are valid

#define MBOOT_MEM_FLAG_FREE	0x1

typedef struct mboot_info_struct
{
	unsigned int flags;			// Valid fields flag
	unsigned int mem_lower;			// Ammound of available RAM
	unsigned int mem_upper;
	unsigned int boot_device;		// Device used to load kernel
	unsigned int cmdline;			// Address of command line passed to kernel
	unsigned int mods_count;		// Number of loaded boot modules
	unsigned int mods_addr;			// Address of boot module table
	unsigned int symbol_num;		// Number of elf symbol table entries
	unsigned int symbol_size;		// Symbol table size
	unsigned int symbol_addr;		// Symbol table address
	unsigned int symbol_shndx;		// String table address
	unsigned int mmap_length;		// Length of memory map
	unsigned int mmap_addr;			// Address of memory map
	unsigned int drives_length;		// Size of drive table
	unsigned int drives_addr;		// Address of drive table
	unsigned int config_table;		// Address of ROM configuration table
	unsigned int boot_loader_name;		// Address of string with the boot loader name
	unsigned int apm_table;			// Address of APM table
	unsigned int vbe_control_info;		// Graphics mode info
	unsigned int vbe_mode_info;
	unsigned int vbe_mode;
	unsigned int vbe_interface_seg;
	unsigned int vbe_interface_off;
	unsigned int vbe_inerfae_len;
}__attribute__((packed)) mboot_info;

typedef struct mboot_mods_struct
{
	unsigned int mod_start;
	unsigned int mod_end;
	unsigned char string[4];
	unsigned int reserved;
}__attribute__((packed)) mboot_mod;

typedef struct mboot_mmap_entry
{
	unsigned int size;
	unsigned int base_addr_lower;
	unsigned int base_addr_upper;
	unsigned int length_lower;
	unsigned int length_upper;
	unsigned int type;
}__attribute__((packed)) mmap_entry;

#endif
