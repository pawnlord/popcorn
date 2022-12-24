# notes

memory layout  
```
==header (multiboot spec)==
==1MB for stack, counting downwards==
*stack_space
*heap_space
==1MB for heap, counting upwards==
--- other bss
IDT
active_page_dir
kernel_page_dir
video buffer
buddy bitmap
...
--- end of identity mapped pages

```
