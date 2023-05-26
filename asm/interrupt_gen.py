

error_handler = "error_handler:\n"
for i in range(32):
    error_handler += "e"+str(i)+":\n"
    error_handler += "push dword " + str(i) + "\n"
    error_handler += "jmp e\n"
error_handler += "e:\n"
error_handler += "\tpop edi\n"
error_handler += "\tpushad\n"
error_handler += "\tcall error_handler_main\n"

with open("asm/interrupt.s.in") as infile:
    with open("asm/interrupt.s", "w") as outfile:
        outfile.write(infile.read().format(error_handler))
