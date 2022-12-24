# notes

### memory layout  
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

### malloc strategy
```
block_start; first block we have allocated

Issue 1: how to deal with freed blocks
without any freed blocks, we have a layout that looks like this
block1
->block2
--- data in block 1
block2
->block3
.
.
.
block_n
->block_n+1
--- data in block n
EOM

when we add a block, we must know that there is no space after block_n
we could also store a bit that keeps track of this
problem comes, if we delete block_n-1 we get

block_n-2
->block_n-1
--data for block_n-2
block_n-1
->block_n
--- UNALLOCATED
block_n
->block_n+1
--- data in block_n
EOM

The question is what we should point to
because if we point to block_n-1, then we would need to assume that we have at least the information for the next block allocated



```
