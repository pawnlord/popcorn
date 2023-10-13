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

{}
