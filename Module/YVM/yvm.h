#ifndef __YVM_H__
#define __YVM_H__

#include "../Dependencies/dependencies.h"

//OPCODEWORD MAX_STACK_SIZE = 65536;

void yel_init_data_seg();
void yel_free_data_seg();
void yel_run(OPCODEWORD*, YelByteCode*, size_t);
YelByteCode* yel_init_stack(size_t);
SIZE_REF yel_alloc_Flt_data(__float128);
SIZE_REF yel_alloc_Int_data(__int128_t);
SIZE_REF yel_alloc_Str_data(char*);
SIZE_REF yel_alloc_Bool_data(_Bool);
void print_disassembly_opcode(OPCODEWORD, size_t);


#endif // __YVM_H__