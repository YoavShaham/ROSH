global usermode
usermode:
	mov ax, 0x20 | 3 ; user data segment
	mov ds, ax
	mov es, ax 
	mov fs, ax 
	mov gs, ax
 
	mov eax, esp
	push 0x20 | 3 ; user data segment
	push eax ; current esp
	pushf
	push 0x18 | 3 ; user code segment
	push 0x7530000 ; jump to main usermode function
	iret