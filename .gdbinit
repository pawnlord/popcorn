add-symbol-file bin/kernel
target remote localhost:1234
break kmain
continue
