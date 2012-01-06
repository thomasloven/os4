[bits 32]
section .text
[global flush_pagedir]
[global set_pagedir]
[global get_pagedir]
[global flush_tlb]

flush_pagedir:
	mov eax, cr3
	mov cr3, eax
	ret
	
set_pagedir:
	mov eax, [esp+4]
	mov cr3, eax
	ret
	
get_pagedir:
	mov eax, cr3
	ret
