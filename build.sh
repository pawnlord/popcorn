nasm -f elf32 asm/kernel.asm -o bin/kasm.o
gcc -fno-stack-protector -m32 -c c/kernel.c -o bin/kc.o
ld -m elf_i386 -T link.ld -o bin/kernel bin/kasm.o bin/kc.o
rm bin/kasm.o bin/kc.o
