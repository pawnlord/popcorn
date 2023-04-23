bits 32

section .text

global switch_task
global jump_usermode
global test_user_function_fail
extern current_task
extern tss
extern println_int
global reload_segments
reload_segments:
	jmp 0x08:reload_cs
reload_cs:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

    ; credit: https://wiki.osdev.org/Brendan%27s_Multi-tasking_Tutorial
switch_task:
    ;Save previous task's state

    ;Notes:
    ;  For cdecl; EAX, ECX, and EDX are already saved by the caller and don't need to be saved again
    ;  EIP is already saved on the stack by the caller's "CALL" instruction
    ;  The task isn't able to change CR3 so it doesn't need to be saved
    ;  Segment registers are constants (while running kernel code) so they don't need to be saved

    push ebx
    push esi
    push edi
    push ebp

    mov edi,[current_task]    ;edi = address of the previous task's "thread control block"
    mov [edi+Task.ESP],esp         ;Save ESP for previous task's kernel stack in the thread's TCB

    ;Load next task's state

    mov esi,[esp+(4+1)*4]         ;esi = address of the next task's "thread control block" (parameter passed on stack)
    mov [current_task],esi    ;Current task's TCB is the next task TCB

    mov esp,[esi+Task.ESP]         ;Load ESP for next task's kernel stack from the thread's TCB
    mov eax,[esi+Task.page_dir]         ;eax = address of page directory for next task
    mov ebx,[esi+Task.ESP0]        ;ebx = address for the top of the next task's kernel stack
    mov edx, tss
    mov [edx+4],ebx            ;Adjust the ESP0 field in the TSS (used by CPU for for CPL=3 -> CPL=0 privilege level changes)
    mov ecx,cr3                   ;ecx = previous task's virtual address space

    cmp eax,ecx                   ;Does the virtual address space need to being changed?
    je .doneVAS                   ; no, virtual address space is the same, so don't reload it and cause TLB flushes
    mov cr3,eax                   ; yes, load the next task's virtual address space
.doneVAS:

    pop ebp
    pop edi
    pop esi
    pop ebx

    ret


    ; credit: https://wiki.osdev.org/Getting_to_Ring_3#iret_method
jump_usermode:
	mov ax, (4 * 8) | 3 ; ring 3 data with bottom 2 bits set for ring 3
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax ; SS is handled by iret

	                            ; set up the stack frame iret expects
	mov eax, esp
	push (4 * 8) | 3 ; data selector
	push eax ; c1urrent esp
	pushf ; eflags
	push (3 * 8) | 3 ; code selector (ring 3 code with bottom 2 bits set for ring 3)
    mov eax, [esp+4]
	push eax ; instruction address to return to
	iret

test_user_function_fail:
    cli
    ret

section .bss
    struc Task
        .num resd 1
        .pid resd 1
        .page_dir resd 1
        .ESP resd 1
        .ESP0 resd 1
        .state resd 1
    endstruc