[global spin]
spin:
	mov edx, [esp + 4]
	mov eax, 0 
	mov ecx, 1
.retry:
	xor eax, eax
	lock cmpxchg [edx], cl
	jnz .retry
ret