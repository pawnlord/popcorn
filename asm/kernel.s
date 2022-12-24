bits 32section .texttext_start:		;multiboot spec        align 4        dd 0x1BADB002            ;magic        dd 0x00                  ;flags        dd - (0x1BADB002 + 0x00) ;checksum. m+f+c should be zeroglobal startglobal load_idtglobal read_portglobal write_portglobal keyboard_handlerglobal get_heap_spaceglobal get_stack_spaceglobal get_stack_ptrglobal get_kernel_startglobal load_page_directoryglobal enable_paging	extern kmain	        ;kmain is defined in the c fileextern keyboard_handler_mainget_heap_space:	mov eax, heap_space	retget_stack_space:	mov eax, stack_space	retget_stack_ptr:	mov eax, esp	retget_kernel_start:	mov eax, text_start	ret	load_idt:	mov edx, [esp + 4]	lidt [edx]	sti	retread_port:	mov edx, [esp+4]	in al, dx	retwrite_port:	mov edx, [esp+4]	mov al, [esp+4+4]	out dx, al	retkeyboard_handler:	call keyboard_handler_main	iretdload_page_directory:	push ebp	mov ebp, esp	mov eax, [esp+8]	mov cr3, eax	mov esp, ebp	pop ebp	retenable_paging:	push ebp	mov ebp, esp	mov eax, cr0	or eax, 0x80000000	mov cr0, eax	mov esp, ebp	pop ebp	retstart:	cli 			;block interrupts	mov esp, stack_space	;set stack pointer	call kmain	hlt		 	;halt the CPUsection .bss	resb 1048576		;1MB for stackstack_space:heap_space:resb 1048576