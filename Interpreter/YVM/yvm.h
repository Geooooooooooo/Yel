#ifndef __YVM_H__
#define __YVM_H__

#include "../Dependencies/dependencies.h"
#include <math.h>

//OPCODEWORD MAX_STACK_SIZE = 65536;

void        init_yvm_env();
void        yel_init_data_seg();
void        yel_free_data_seg();
OPCODEWORD* yel_init_stack(size_t);
SIZE_REF    yel_alloc_Flt_data(long double);
SIZE_REF    yel_alloc_Int_data(signed long long);
SIZE_REF    yel_alloc_Str_data(char*);
SIZE_REF    yel_alloc_Bool_data(_Bool);
SIZE_REF    yel_alloc_func(YelFunction*);

// @return ref to YelVariable struct
SIZE_REF yel_alloc_variable(char*, SIZE_REF);

// @return unused int memory or alloc new memory
SIZE_REF yel_set_unused_int_memory(signed long long, OPCODEWORD*, unsigned long long);
SIZE_REF yel_set_unused_float_memory(long double, OPCODEWORD*, unsigned long long);
SIZE_REF yel_set_unused_bool_memory(_Bool, OPCODEWORD*, unsigned long long);

void print_disassembly_bytecode(YelByteCode);

// debug
void print_const_int_data();

#endif // __YVM_H__