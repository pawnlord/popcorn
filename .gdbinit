add-symbol-file bin/kernel
target remote localhost:4444
break kmain
continue
