[global flush_idt]

flush_idt:
	mov eax, [esp+4]
	lidt [eax]
	ret
	
[global flush_tss]

flush_tss:
	mov eax, [esp+4]
	ltr ax
	ret
	
%MACRO INTNOERR 1
	[global isr%1]
	isr%1:
		cli
		push 0
		push %1
		jmp int_stub
%endmacro

%MACRO INTERR 1
	[global isr%1]
	isr%1:
		cli
		push %1
		jmp int_stub
%endmacro

INTNOERR 0
INTNOERR 1
INTNOERR 2
INTNOERR 3
INTNOERR 4
INTNOERR 5
INTNOERR 6
INTNOERR 7
INTERR 8
INTNOERR 9
INTERR 10
INTERR 11
INTERR 12
INTERR 13
INTERR 14
INTNOERR 15
INTNOERR 16
INTNOERR 17
INTNOERR 18
INTNOERR 19
INTNOERR 20
INTNOERR 21
INTNOERR 22
INTNOERR 23
INTNOERR 24
INTNOERR 25
INTNOERR 26
INTNOERR 27
INTNOERR 28
INTNOERR 29
INTNOERR 30
INTNOERR 31

INTNOERR 32
INTNOERR 33
INTNOERR 34
INTNOERR 35
INTNOERR 36
INTNOERR 37
INTNOERR 38
INTNOERR 39
INTNOERR 40
INTNOERR 41
INTNOERR 42
INTNOERR 43
INTNOERR 44
INTNOERR 45
INTNOERR 46
INTNOERR 47
INTNOERR 128
INTNOERR 255

[extern idt_handler]

int_stub:
	pusha
	xor ecx, ecx
	mov cx, ds
	push ecx
	
	mov cx, 0x10
	mov ds, cx
	mov es, cx
	mov fs, cx
	mov gs, cx
	mov ss, cx
	
	mov eax, esp
	[extern KernelStack]
	mov esp, KernelStack + 0x1000
	push eax
	call idt_handler
	mov esp, eax
	
[global int_return]
int_return:
	pop eax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	popa
	add esp, 8
	
	iret