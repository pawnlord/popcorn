    bits 32
    section .text

    global keyboard_handler
    global error_handler
    global out_handler


    extern keyboard_handler_main
    extern error_handler_main
    extern handle_stdout

out_handler:
	pushad
	call handle_stdout
    popad
	iretd

keyboard_handler:
	pushad
    call keyboard_handler_main
	popad
	iretd

error_handler:
e0:
push dword 0
jmp e
e1:
push dword 1
jmp e
e2:
push dword 2
jmp e
e3:
push dword 3
jmp e
e4:
push dword 4
jmp e
e5:
push dword 5
jmp e
e6:
push dword 6
jmp e
e7:
push dword 7
jmp e
e8:
push dword 8
jmp e
e9:
push dword 9
jmp e
e10:
push dword 10
jmp e
e11:
push dword 11
jmp e
e12:
push dword 12
jmp e
e13:
push dword 13
jmp e
e14:
push dword 14
jmp e
e15:
push dword 15
jmp e
e16:
push dword 16
jmp e
e17:
push dword 17
jmp e
e18:
push dword 18
jmp e
e19:
push dword 19
jmp e
e20:
push dword 20
jmp e
e21:
push dword 21
jmp e
e22:
push dword 22
jmp e
e23:
push dword 23
jmp e
e24:
push dword 24
jmp e
e25:
push dword 25
jmp e
e26:
push dword 26
jmp e
e27:
push dword 27
jmp e
e28:
push dword 28
jmp e
e29:
push dword 29
jmp e
e30:
push dword 30
jmp e
e31:
push dword 31
jmp e
e:
	pop edi
	pushad
	push edi
	call error_handler_main

