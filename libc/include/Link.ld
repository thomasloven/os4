ENTRY(_entry)
OUTPUT_FORMAT(elf32-i386)

SECTIONS {

	. = 0x10000;
	.text : { 
		code = .; _code = .; __code = .;
		*(.text)
		*(.eh_frame)
		. = ALIGN(4096);
	}
	
	.data : { 
		data = .; _data = .; __data = .;
		*(.data)
		*(.rodata)
		. = ALIGN(4096);
	}
	.bss : {
		
		*(.bss)
		. = ALIGN(4096);
	}
	.end : {
		end = .; _end = .; __end = .;
	}
}
