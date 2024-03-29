section .text
text_start:	
	;multiboot spec
        align 4
        dd 0x1BADB002            ;magic
        dd 0x00                  ;flags
        dd - (0x1BADB002 + 0x00) ;checksum. m+f+c should be zero

global start
global load_idt
global load_gdt
global load_tss
global read_port
global write_port
global get_heap_space
global get_stack_space
global get_stack_ptr
global get_esp
global get_kernel_start
global load_page_directory
global enable_paging
global display_int
global disable_ints
global enable_ints
	
extern kmain	        ;kmain is defined in the c file
extern print_int

get_heap_space:
	mov eax, heap_space
	ret
get_stack_space:
	mov eax, stack_space
	ret
get_stack_ptr:
	mov eax,ebp
	add eax,4
	ret
get_esp:
	mov eax,esp
	add eax,4
	ret
	
get_kernel_start:
	mov eax, text_start
	ret
	
load_idt:
	mov edx, [esp + 4]
	lidt [edx]
	sti
	ret

load_gdt:
	mov edx, [esp + 4]
	lgdt [edx]
	ret

load_tss:
	mov ax, [esp + 4]
    ltr ax
	ret

read_port:
	mov edx, [esp+4]
	in al, dx
	ret

write_port:
	mov edx, [esp+4]
	mov al, [esp+4+4]
	out dx, al
	ret



load_page_directory:
	push ebp
	mov ebp, esp
	mov eax, [esp+8]
	mov cr3, eax
	mov esp, ebp
	pop ebp
	ret
display_int:
	int 80h
	ret
	
enable_paging:
	push ebp
	mov ebp, esp
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax
	mov esp, ebp
	pop ebp
	ret

disable_ints:
	cli
	ret
enable_ints:
	sti
	ret
start:
	cli 			;block interrupts
	mov esp, stack_space	;set stack pointer
	call kmain
	hlt		 	;halt the CPU

	
section .bss	
resb 1048576		;1MB for stack
stack_space:
heap_space:
resb 1048576
