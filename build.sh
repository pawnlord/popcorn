nasm -f elf32 asm/kernel.s -o bin/kasm.o
gcc -fno-stack-protector -m32 -c c/kernel.c  -o bin/kc.o
gcc -fno-stack-protector -m32 -c c/kmalloc.c -o bin/kmalloc.o
gcc -fno-stack-protector -m32 -c c/display.c -o bin/display.o
gcc -fno-stack-protector -m32 -c c/testing.c -o bin/testing.o
gcc -fno-stack-protector -m32 -c c/buddy.c   -o bin/buddy.o
gcc -fno-stack-protector -m32 -c c/kio.c     -o bin/kio.o
ld -m elf_i386 -T link.ld -o bin/kernel bin/kasm.o bin/kc.o bin/kmalloc.o bin/display.o bin/testing.o bin/buddy.o bin/kio.o
rm bin/kasm.o  bin/kc.o bin/kmalloc.o bin/display.o bin/testing.o bin/buddy.o bin/kio.o
