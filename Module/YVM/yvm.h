#ifndef __YVM_H__
#define __YVM_H__

#include "../Dependencies/dependencies.h"
#include <math.h>

//OPCODEWORD MAX_STACK_SIZE = 65536;

void yel_init_data_seg();
void yel_free_data_seg();
void yel_run(OPCODEWORD*, YelByteCode*, size_t);
OPCODEWORD* yel_init_stack(size_t);
SIZE_REF yel_alloc_Flt_data(long double);
SIZE_REF yel_alloc_Int_data(signed long long);
SIZE_REF yel_alloc_Str_data(char*);
SIZE_REF yel_alloc_Bool_data(_Bool);
void print_disassembly_bytecode(YelByteCode);

// debug
void print_const_int_data();

#endif // __YVM_H__