#ifndef __YVM_H__
#define __YVM_H__

#include "../Dependencies/dependencies.h"

OPCODEWORD MAX_STACK_SIZE = 65536;

void yel_run();
void yel_init_vm(OPCODEWORD stack_size);

#endif // __YVM_H__