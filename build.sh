nasm -f elf32 asm/kernel.s -o bin/kasm.o
nasm -f elf32 asm/task.s -o bin/_asm_task.o
build_file(){
    gcc -fno-stack-protector -m32 -g -c c/$1.c  -o bin/$1.o -Wno-builtin-declaration-mismatch
}
gcc -fno-stack-protector -m32 -g -c c/kernel.c  -o bin/kc.o -Wno-builtin-declaration-mismatch
build_file kmalloc
build_file display
build_file testing
build_file buddy
build_file kio
build_file ksh
build_file "exec"
build_file task
ld -m elf_i386 -T link.ld -o bin/kernel bin/kasm.o bin/_asm_task.o  bin/kc.o bin/kmalloc.o bin/display.o bin/testing.o bin/buddy.o bin/kio.o bin/task.o bin/ksh.o "bin/exec.o"
rm bin/kasm.o  bin/kc.o bin/kmalloc.o bin/display.o bin/testing.o bin/buddy.o bin/kio.o bin/ksh.o "bin/exec.o"
