
typedef int size_t;

typedef struct block_info{
  size_t sz;
  struct block_info *next;
  char free;
  
} block_info_t;

void mem_init(void);

void *kmalloc(size_t size);

void kfree(void *ptr);

