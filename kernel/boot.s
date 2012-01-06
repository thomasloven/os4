
MBOOT_PAGE_ALIGNED_FLAG	equ	1<<0
MBOOT_MEMORY_INFO_FLAG	equ	1<<1
MBOOT_HEADER_MAGIC	equ	0x1BADB002

MBOOT_HEADER_FLAGS	equ	MBOOT_PAGE_ALIGNED_FLAG | MBOOT_MEMORY_INFO_FLAG
MBOOT_HEADER_CHECKSUM	equ	-(MBOOT_HEADER_FLAGS + MBOOT_HEADER_MAGIC)

KERNEL_OFFSET	equ	0xC0000000
BOOT_STACK_SIZE	equ	0x1FFF

[bits 32]

section .bss


[global KernelStack]
align 0x8
BootStack:
	resb BOOT_STACK_SIZE
KernelStack:

section .data
[global BootPageDirectory]
align 0x1000
BootPageDirectory:
	dd (BootPageTable - KERNEL_OFFSET) + 0x3
	times ((KERNEL_OFFSET >> 22) - 1) dd 0x0
	dd (BootPageTable - KERNEL_OFFSET) + 0x3
	times (1022 - (KERNEL_OFFSET >> 22)) dd 0x0
	dd (BootPageDirectory - KERNEL_OFFSET) + 0x3
	
BootPageTable:
	%assign i 0
	%rep 1024
		dd (i << 12) | 0x3
		%assign i i+1
	%endrep
	
[global gdt]
gdt_ptr:
gdt:
	dw 0x002F
	dd gdt
	dw 0x0000
	dd 0x0000FFFF, 0x00CF9A00
	dd 0x0000FFFF, 0x00CF9200
	dd 0x0000FFFF, 0x00CFFA00
	dd 0x0000FFFF, 0x00CFF200
	dd 0x00000000, 0x0000E900
	
section .text
align 4
MultiBootHeader:
	dd MBOOT_HEADER_MAGIC
	dd MBOOT_HEADER_FLAGS
	dd MBOOT_HEADER_CHECKSUM
	
[global start]

start:
	cli
	
	mov ecx, BootPageDirectory - KERNEL_OFFSET
	mov cr3, ecx
	mov ecx, cr0
	or ecx, 0x80000000
	mov cr0, ecx
	lea ecx, [.higherHalf]
	jmp ecx

.higherHalf:
	mov ecx, gdt_ptr
	lgdt [ecx]

	mov ecx, 0x10
	mov ds, cx
	mov es, cx
	mov fs, cx
	mov gs, cx
	mov ss, cx
	jmp 0x8:.gdtLoaded
	
.gdtLoaded:
	mov edx, BootPageDirectory
	xor ecx, ecx
	mov [edx], ecx
	invlpg[0]
	
	mov esp, KernelStack
	mov ebp, KernelStack
	
	add ebx, KERNEL_OFFSET
	push ebx

[extern kinit]
	call kinit
	mov esp, eax
[extern int_return]
	jmp int_return
	
section .end
[global kernelEnd]
kernelEnd: