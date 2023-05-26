#/etc/bin/bash
./build.sh
qemu-system-i386 -S -s -kernel bin/kernel > /dev/null  &
gdb
