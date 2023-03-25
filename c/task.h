#define KERNEL_DS 0
#define KERNEL_CS 0
#define USER_DS 0
#define USER_CS 0

#define MAX_TASKS 255

typedef struct ProtectedTSS {
    unsigned short link;
    unsigned short res1; // cannot be touched
    uint32_t ESP0;
    unsigned short SS0;
    unsigned short res2; // cannot be touched
    uint32_t ESP1;
    unsigned short SS1;
    unsigned short res3; // cannot be touched
    uint32_t ESP2;
    unsigned short SS2;
    unsigned short res4; // cannot be touched
    uint32_t ESP3;
    uint32_t CR3;
    uint32_t EIP;
    uint32_t EFLAGS;
    uint32_t EAX;
    uint32_t ECX;
    uint32_t EDX;
    uint32_t EBX;
    uint32_t ESP;
    uint32_t EBP;
    uint32_t ESI;
    uint32_t EDI;
    unsigned short ES;
    unsigned short res5; // cannot be touched
    unsigned short CS;
    unsigned short res6; // cannot be touched
    unsigned short SS;
    unsigned short res7; // cannot be touched
    unsigned short DS;
    unsigned short res8; // cannot be touched
    unsigned short FS;
    unsigned short res9; // cannot be touched
    unsigned short GS;
    unsigned short res10; // cannot be touched
    unsigned short LDTR;
    unsigned short res11; // cannot be touched
    unsigned short res12; // cannot be touched
    unsigned short IOPB;    
    uint32_t SSP;
} ProtectedTSS;

typedef enum TaskState{
  TASK_STOPPED,
  TASK_RUNNING
} TaskState;

typedef struct Task {
  ProtectedTSS tss;
  int num;
  int pid;
  int kernel_stack_page;
  int task_stack_page;
  TaskState state;
} Task;

void setup_tss(ProtectedTSS *tss);
