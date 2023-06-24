#include "yvm.h"

typedef struct _StackItem {
    WORD type;
    void* value;
} StackItem;

void YelRun(void const** memory, size_t stack_size) {
    StackItem* stack = (StackItem*)malloc(stack_size*sizeof(StackItem));
    if (stack == NULL) {
        exit(-1);
    }

    register WORD ip = 0;   // instruction pointer
    register WORD sp = 0;   // stack pointer

    register WORD opcode = *(WORD*)memory[ip];
    ++ip;

    while (1) {
        switch (opcode)
        {
        case YVM_PUSH_CONST:
            stack[sp].type = *(WORD*)memory[++ip];
            stack[sp].value = memory[++ip];
            ++sp;
            ++ip;
            break;
        
        default:
            break;
        }
    }
}