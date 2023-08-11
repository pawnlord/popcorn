#/etc/bin/bash
./build.sh
qemu-system-i386 -S -gdb tcp::4444 -kernel bin/kernel > /dev/null  &
gdb
