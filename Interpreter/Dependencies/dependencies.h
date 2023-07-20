#ifndef __DEPENDECIES_H__
#define __DEPENDECIES_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "opcodes.h"
#include "structures.h"

#define print_cur() printf("\ncur = %s\n", _CurVal)
#define print_tokens() puts("tokens = {");\
    for (size_t i = 0; i < token_array.length; i++) {\
        printf("   [%d, '%s', %lu:%lu]\n", token_array.type[i], token_array.value[i], token_array.line[i], token_array.start_symbol[i]);\
    } puts("}")

#define print_info() printf("_CurVal = %s, _NextVal = %s\n_SimpleExpr == %d\n _Unary == %d\n\n", _CurVal, _NextVal, _SimpleExpr, _Unary);getchar()

#define _CurType    yel_tokens->type[yel_tokens->pointer]
#define _NextType   yel_tokens->type[yel_tokens->pointer+1]
#define _CurVal     yel_tokens->value[yel_tokens->pointer]
#define _NextVal    yel_tokens->value[yel_tokens->pointer+1]

#define RET_CODE_ERROR  0x1
#define RET_CODE_OK     0x0

#define INT_TYPE            1
#define FLT_TYPE            2
#define BOOL_TYPE           3
#define STR_TYPE            4
#define FUNC_TYPE           5

#define YEL_SIZE_INT        sizeof(long long)
#define YEL_SIZE_FLT        sizeof(long double)

#define TO_YEL_VAR(var)     (*(YelVariable*)(var))
#define TO_YEL_CONST(var)   (*(YelConstant*)(var))
#define TO_LL(var)          (*(signed long long*)(var))
#define TO_LD(var)          (*(long double*)(var))
#define TO_B(var)           (*(_Bool*)(var))
#define TO_FUNC(var)        (*(YelFunction*)(var))

#endif // __DEPENDECIES_H_
