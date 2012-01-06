[bits 32]
section .text

[global _entry]
[extern _init]

_entry:
	jmp _init
	jmp $
	
