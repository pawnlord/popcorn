nasm -f elf32 asm/kernel.s -o bin/kasm.o
gcc -fno-stack-protector -m32 -c c/kernel.c -o bin/kc.o
gcc -fno-stack-protector -m32 -c c/kmalloc.c -o bin/kmalloc.o
gcc -fno-stack-protector -m32 -c c/display.c -o bin/display.o
ld -m elf_i386 -T link.ld -o bin/kernel bin/kasm.o bin/kc.o bin/kmalloc.o bin/display.o
rm bin/kasm.o  bin/kc.o bin/kmalloc.o bin/display.o
