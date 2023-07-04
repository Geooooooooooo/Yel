#include "yvm.h"
#include <math.h>

void print_disassembly_bytecode(YelByteCode bytecode) {
    static int next = 0;

    puts("Disassembled code:\n");

    for (size_t i = 0; i < bytecode.len; i++) {
        if (next == 1) {
            next = 0;
            printf("%s (%p)\n", (char*)bytecode.opcode[i], bytecode.opcode[i]);
            continue;
        }
        else if (next == 2) {
            next = 0;
            printf("%p (to %llu)\n", bytecode.opcode[i], bytecode.opcode[i]);
            continue;
        }
        else if (next == 3) {
            next = 0;
            printf("%s\n", (char*)(*(YelVariable*)bytecode.opcode[i]).name);
            continue;
        }
        else if (next == 4) {
            next = 0;
            printf("%llu\n", bytecode.opcode[i]);
            continue;
        }
        else if (next == 5) {
            next = 0;
            printf("%p\n", bytecode.opcode[i]);
            continue;
        }

        switch (bytecode.opcode[i]) {
        case OP_HALT:       printf("%llu:\tHALT\n", i);break;
        case LOAD_VALUE:    printf("%llu:\tLOAD_VALUE\t", i);next=3;break;
        case LOAD_CONST:    printf("%llu:\tLOAD_CONST\t", i);next=5;break;
        case POP_VALUE:     printf("%llu:\tPOP_VALUE\n", i);break;
        case DUP_VALUE:     printf("%llu:\tDUP_VALUE\n", i);break;
        case UNARY_POS:     printf("%llu:\tUNARY_POS\n", i);break;
        case UNARY_NOT:     printf("%llu:\tUNARY_NOT\n", i);break;
        case UNARY_LOGICAL_NOT:printf("%llu:\tLOGICAL_NOT\n", i);break;
        case UNARY_NEG:     printf("%llu:\tUNARY_NEG\n", i);break;
        case UNARY_INC:     printf("%llu:\tUNARY_INC\n", i);break;
        case UNARY_DEC:     printf("%llu:\tUNARY_DEC\n", i);break;
        case BYNARY_OP:     printf("%llu:\tBYNARY_OP\t", i);break;
        case BYNARY_POW:    printf("%llu (**)\n", BYNARY_POW);break;
        case BYNARY_DIV:    printf("%llu (/)\n", BYNARY_DIV);break;
        case BYNARY_MUL:    printf("%llu (*)\n", BYNARY_MUL);break;
        case BYNARY_MOD:    printf("%llu (%%)\n", BYNARY_MOD);break;
        case BYNARY_ADD:    printf("%llu (+)\n", BYNARY_ADD);break;
        case BYNARY_SUB:    printf("%llu (-)\n", BYNARY_SUB);break;
        case BYNARY_RSH:    printf("%llu (>>)\n", BYNARY_RSH);break;
        case BYNARY_LSH:    printf("%llu (<<)\n", BYNARY_LSH);break;
        case BYNARY_MORE:   printf("%llu (>)\n", BYNARY_MORE);break;
        case BYNARY_MORE_EQ:printf("%llu (>=)\n", BYNARY_MORE_EQ);break;
        case BYNARY_LESS:   printf("%llu (<)\n", BYNARY_LESS);break;
        case BYNARY_LESS_EQ:printf("%llu (<=)\n", BYNARY_LESS_EQ);break;
        case BYNARY_EQ:     printf("%llu (==)\n", BYNARY_EQ);break;
        case BYNARY_NOT_EQ: printf("%llu (!=)\n", BYNARY_NOT_EQ);break;
        case BYNARY_AND:    printf("%llu (&)\n", BYNARY_AND);break;
        case BYNARY_OR:     printf("%llu (|)\n", BYNARY_OR);break;
        case BYNARY_LOGICAL_AND:printf("%llu (and)\n", BYNARY_LOGICAL_AND);break;
        case BYNARY_LOGICAL_OR:printf("%llu (or)\n", BYNARY_LOGICAL_OR);break;
        case OP_JUMP_TO:    printf("%llu:\tJUMP_TO\t\t", i);next=2;break;
        case OP_POP_JUMP_ZERO:printf("%llu:\tPOP_JUMP_ZERO\t", i);next=2;break;
        case OP_CALL:       printf("%llu:\tCALL\t\t", i);next=3;break;
        case OP_RET:        printf("%llu:\tRET\n", i);break;
        case OP_BRK:        printf("%llu:\tBRK\n", i);break;
        case OP_STORE:      printf("%llu:\tSTORE\t\t", i);next=3;break;
        case MAKE_FUNC:     printf("%llu:\tMAKE_FUNC\n", i);break;
        default:            printf("%p\n", bytecode.opcode[i]);break;
        }
    }
}

SIZE_REF* data_float_segment;
SIZE_REF* data_bool_segment;
SIZE_REF* data_int_segment;
SIZE_REF* data_str_segment;
SIZE_REF* variables_segment;

unsigned long long data_float_segment_len;
unsigned long long data_int_segment_len;
unsigned long long data_bool_segment_len;
unsigned long long data_str_segment_len;
unsigned long long variables_segment_len;

void yel_init_data_seg() {
    data_float_segment = (SIZE_REF*)__builtin_malloc(sizeof(SIZE_REF));
    data_float_segment_len = 0;

    data_int_segment = (SIZE_REF*)__builtin_malloc(sizeof(SIZE_REF));
    data_int_segment_len = 0;

    data_bool_segment = (SIZE_REF*)__builtin_malloc(sizeof(SIZE_REF));
    data_bool_segment_len = 0;

    data_str_segment = (SIZE_REF*)__builtin_malloc(sizeof(SIZE_REF));
    data_str_segment_len = 0;

    variables_segment = (SIZE_REF*)__builtin_malloc(sizeof(SIZE_REF));
    variables_segment_len = 0;
}

// fix
void yel_free_data_seg() {
    size_t i;

    for (i = 0; i < data_float_segment_len; i++)
        __builtin_free(data_float_segment[i]);
    __builtin_free(data_float_segment);

    for (i = 0; i < data_int_segment_len; i++)
        __builtin_free(data_int_segment[i]);
    __builtin_free(data_int_segment);

    for (i = 0; i < data_bool_segment_len; i++)
        __builtin_free(data_bool_segment[i]);
    __builtin_free(data_bool_segment);

    for (i = 0; i < data_str_segment_len; i++)
        __builtin_free(data_str_segment[i]);
    __builtin_free(data_str_segment);

    for (i = 0; i < variables_segment_len; i++)
        __builtin_free(variables_segment[i]);
    __builtin_free(variables_segment);
}

// returns ref to YelVariable struct
SIZE_REF yel_alloc_variable(char* name, SIZE_REF c_ref) {
    for (unsigned long long i = 0; i < variables_segment_len; i++) {
        if (__builtin_strcmp((*(YelVariable*)variables_segment[i]).name, name) == 0) {
            // (*(YelVariable*)variables_segment[i]).ref = c_ref;
            return (SIZE_REF)variables_segment[i];
        }
    }
    
    variables_segment = (SIZE_REF*)__builtin_realloc(variables_segment, (variables_segment_len+1) * sizeof(SIZE_REF));
    YelVariable* var = (YelVariable*)__builtin_malloc(sizeof(YelVariable));

    var->ref = c_ref;
    var->name = name;

    variables_segment[variables_segment_len] = (SIZE_REF)var;

    ++variables_segment_len;
    return (SIZE_REF)variables_segment[variables_segment_len-1];
}

SIZE_REF yel_alloc_Flt_data(long double _Val) {
    for (unsigned long long i = 0; i < data_float_segment_len; i++) {
        if (*(long double*)(*(YelConstant*)data_float_segment[i]).ref == (long double)_Val) {
            return (SIZE_REF)data_float_segment[i];
        }
    }
    
    data_float_segment = (SIZE_REF*)__builtin_realloc(data_float_segment, (data_float_segment_len+1) * sizeof(SIZE_REF));
    long double* tmp = (long double*)__builtin_malloc(sizeof(long double));
    *tmp = _Val;
    YelConstant* cnst = (YelConstant*)__builtin_malloc(sizeof(YelConstant));

    cnst->ref = tmp;
    cnst->type = FLT_TYPE;

    ++data_float_segment_len;
    return (SIZE_REF)data_float_segment[data_float_segment_len-1];
}

void print_const_int_data() {
    for (unsigned long long i=0; i<data_int_segment_len; i++)
        printf("%p:\t%lld\n", (*(YelConstant*)data_int_segment[i]).ref, *(signed long long*)((*(YelConstant*)data_int_segment[i]).ref)); 
}

SIZE_REF yel_alloc_Int_data(signed long long _Val) {
    for (unsigned long long i = 0; i < data_int_segment_len; i++) {
        if (*(signed long long*)(*(YelConstant*)data_int_segment[i]).ref == (signed long long)_Val) {
            return (SIZE_REF)data_int_segment[i];
        }
    }
    
    data_int_segment = (SIZE_REF*)__builtin_realloc(data_int_segment, (data_int_segment_len+1) * sizeof(SIZE_REF));

    signed long long* tmp = (signed long long*)__builtin_malloc(sizeof(signed long long));
    *tmp = _Val;

    YelConstant* cnst = (YelConstant*)__builtin_malloc(sizeof(YelConstant));
    cnst->ref = tmp;
    cnst->type = INT_TYPE;

    data_int_segment[data_int_segment_len] = (SIZE_REF)cnst;
    ++data_int_segment_len;
    
    return (SIZE_REF)data_int_segment[data_int_segment_len-1];
}

// fix it
SIZE_REF yel_alloc_Str_data(char* _Val) {
    data_str_segment = (SIZE_REF*)__builtin_realloc(data_str_segment, (data_str_segment_len+1) * sizeof(SIZE_REF));
    char* tmp = (char*)__builtin_malloc(sizeof(char*));
    *tmp = _Val;
    YelConstant* cnst = (YelConstant*)__builtin_malloc(sizeof(YelConstant));
    cnst->ref = tmp;
    cnst->type = STR_TYPE;
    ++data_str_segment_len;

    return (SIZE_REF)data_str_segment[data_str_segment_len-1];
}

SIZE_REF yel_alloc_Bool_data(_Bool _Val) {
    for (unsigned long long i = 0; i < data_bool_segment_len; i++) {
        if (*(_Bool*)(*(YelConstant*)data_bool_segment[i]).ref == _Val) {
            return (SIZE_REF)data_bool_segment[i];
        }
    }
    
    data_bool_segment = (SIZE_REF*)__builtin_realloc(data_bool_segment, (data_bool_segment_len+1) * sizeof(SIZE_REF));
    _Bool* tmp = (_Bool*)__builtin_malloc(sizeof(_Bool));
    *tmp = _Val;
    YelConstant* cnst = (YelConstant*)__builtin_malloc(sizeof(YelConstant));
    cnst->ref = tmp;
    cnst->type = BOOL_TYPE;
    ++data_bool_segment_len;
    return (SIZE_REF)data_bool_segment[data_bool_segment_len-1];
}

OPCODEWORD* yel_init_stack(size_t stack_size) {
    OPCODEWORD* stack = (OPCODEWORD*)__builtin_malloc((size_t)(stack_size * sizeof(OPCODEWORD)));
    if (stack == NULL) {
        puts("memory is correpted!");
        return NULL;
    }

    return stack;
}

void yel_run(OPCODEWORD* stack, YelByteCode* bytecode, size_t stack_size) {
    register unsigned long long ip = 0;             // instruntion pointer
    register unsigned long long sp = 0;             // stack pointer

    register OPCODEWORD a;
    register OPCODEWORD b;

    YelConstant tmp_const;
    long long tmp_int;

    OPCODEWORD* instructions = bytecode->opcode;

    clock_t begin = clock();

    while (1) {
        switch (instructions[ip]) {
        case OP_HALT:
            goto _debug_info; // _end;

        case LOAD_VALUE:
            stack[sp] = (*(YelVariable*)instructions[ip+1]).ref;

            if (stack[sp] == NULL) {
                printf("RuntimeError: undeclared name '%s'\n", (*(YelVariable*)instructions[ip+1]).name);
                goto _emergency_stop;
            }

            ++sp;
            ip += 2;

            break;

        case LOAD_CONST: 
            stack[sp] = instructions[ip+1];
            ++sp;
            ip += 2;
            break;

        case POP_VALUE:
            --sp;
            break;

        case DUP_VALUE:
            stack[sp] = stack[sp-1];
            ++sp;
            break;

        case BYNARY_OP:
            --sp;

            a = stack[sp];
            b = stack[sp-1];

            switch (instructions[++ip])
            {
            case BYNARY_POW:
                /* tmp_int = powl(
                    *(signed long long*)((YelConstant*)b)->ref, 
                    *(signed long long*)((YelConstant*)a)->ref);

                tmp_const.ref = &tmp_int;
                tmp_const.type = INT_TYPE;
                stack[sp-1] = &tmp_const; */
                break;
            case BYNARY_DIV:
                tmp_int = *(signed long long*)((YelConstant*)b)->ref /
                        *(signed long long*)((YelConstant*)a)->ref;

                tmp_const.ref = &tmp_int;
                tmp_const.type = INT_TYPE;
                stack[sp-1] = &tmp_const;
                break;
            case BYNARY_MUL:
                tmp_int = *(signed long long*)((YelConstant*)b)->ref *
                        *(signed long long*)((YelConstant*)a)->ref;

                tmp_const.ref = &tmp_int;
                tmp_const.type = INT_TYPE;
                stack[sp-1] = &tmp_const;
                break;
            case BYNARY_MOD:
                tmp_int = *(signed long long*)((YelConstant*)b)->ref %
                        *(signed long long*)((YelConstant*)a)->ref;

                tmp_const.ref = &tmp_int;
                tmp_const.type = INT_TYPE;
                stack[sp-1] = &tmp_const;
                break;
            case BYNARY_ADD:
                tmp_int = *(signed long long*)((YelConstant*)b)->ref +
                        *(signed long long*)((YelConstant*)a)->ref;

                tmp_const.ref = &tmp_int;
                tmp_const.type = INT_TYPE;
                stack[sp-1] = &tmp_const;
                break;
            case BYNARY_SUB:
                tmp_int = *(signed long long*)((YelConstant*)b)->ref -
                        *(signed long long*)((YelConstant*)a)->ref;

                tmp_const.ref = &tmp_int;
                tmp_const.type = INT_TYPE;
                stack[sp-1] = &tmp_const;
                break;
            case BYNARY_RSH:
                tmp_int = *(signed long long*)((YelConstant*)b)->ref >>
                        *(signed long long*)((YelConstant*)a)->ref;

                tmp_const.ref = &tmp_int;
                tmp_const.type = INT_TYPE;
                stack[sp-1] = &tmp_const;
                break;
            case BYNARY_LSH:
                tmp_int = *(signed long long*)((YelConstant*)b)->ref <<
                        *(signed long long*)((YelConstant*)a)->ref;

                tmp_const.ref = &tmp_int;
                tmp_const.type = INT_TYPE;
                stack[sp-1] = &tmp_const;
                break;
            case BYNARY_MORE:
                tmp_int = *(signed long long*)((YelConstant*)b)->ref >
                        *(signed long long*)((YelConstant*)a)->ref;

                tmp_const.ref = &tmp_int;
                tmp_const.type = INT_TYPE;
                stack[sp-1] = &tmp_const;
                break;
            case BYNARY_LESS:
                tmp_int = *(signed long long*)((YelConstant*)b)->ref <
                        *(signed long long*)((YelConstant*)a)->ref;

                tmp_const.ref = &tmp_int;
                tmp_const.type = INT_TYPE;
                stack[sp-1] = &tmp_const;
                break;
            case BYNARY_MORE_EQ:
                tmp_int = *(signed long long*)((YelConstant*)b)->ref >=
                        *(signed long long*)((YelConstant*)a)->ref;

                tmp_const.ref = &tmp_int;
                tmp_const.type = INT_TYPE;
                stack[sp-1] = &tmp_const;
                break;
            case BYNARY_LESS_EQ:
                tmp_int = *(signed long long*)((YelConstant*)b)->ref <=
                        *(signed long long*)((YelConstant*)a)->ref;

                tmp_const.ref = &tmp_int;
                tmp_const.type = INT_TYPE;
                stack[sp-1] = &tmp_const;
                break;
            case BYNARY_EQ:
                tmp_int = *(signed long long*)((YelConstant*)b)->ref ==
                        *(signed long long*)((YelConstant*)a)->ref;

                tmp_const.ref = &tmp_int;
                tmp_const.type = INT_TYPE;
                stack[sp-1] = &tmp_const;
                break;
            case BYNARY_NOT_EQ:
                tmp_int = *(signed long long*)((YelConstant*)b)->ref !=
                        *(signed long long*)((YelConstant*)a)->ref;

                tmp_const.ref = &tmp_int;
                tmp_const.type = INT_TYPE;
                stack[sp-1] = &tmp_const;
                break;
            case BYNARY_AND:
                tmp_int = *(signed long long*)((YelConstant*)b)->ref &
                        *(signed long long*)((YelConstant*)a)->ref;

                tmp_const.ref = &tmp_int;
                tmp_const.type = INT_TYPE;
                stack[sp-1] = &tmp_const;
                break;
            case BYNARY_OR:
                tmp_int = *(signed long long*)((YelConstant*)b)->ref |
                        *(signed long long*)((YelConstant*)a)->ref;

                tmp_const.ref = &tmp_int;
                tmp_const.type = INT_TYPE;
                stack[sp-1] = &tmp_const;
                break;
            case BYNARY_LOGICAL_AND:
                tmp_int = *(signed long long*)((YelConstant*)b)->ref &&
                        *(signed long long*)((YelConstant*)a)->ref;

                tmp_const.ref = &tmp_int;
                tmp_const.type = INT_TYPE;
                stack[sp-1] = &tmp_const;
                break;
            case BYNARY_LOGICAL_OR:
                tmp_int = *(signed long long*)((YelConstant*)b)->ref ||
                        *(signed long long*)((YelConstant*)a)->ref;

                tmp_const.ref = &tmp_int;
                tmp_const.type = INT_TYPE;
                stack[sp-1] = &tmp_const;
                break;
            }
        
            ++ip;

            break;
        case UNARY_NEG:
            tmp_int = -*(signed long long*)((YelConstant*)stack[sp-1])->ref;
            tmp_const.ref = &tmp_int;
            tmp_const.type = INT_TYPE;
            stack[sp-1] = &tmp_const;
            ++ip;
            break;
        
        case UNARY_POS:
            tmp_int = +*(signed long long*)((YelConstant*)stack[sp-1])->ref;
            tmp_const.ref = &tmp_int;
            tmp_const.type = INT_TYPE;
            stack[sp-1] = &tmp_const;
            ++ip;
            break;

        case UNARY_NOT:
            tmp_int = ~*(signed long long*)((YelConstant*)stack[sp-1])->ref;
            tmp_const.ref = &tmp_int;
            tmp_const.type = INT_TYPE;
            stack[sp-1] = &tmp_const;
            ++ip;
            break;

        case UNARY_LOGICAL_NOT:
            tmp_int = !*(signed long long*)((YelConstant*)stack[sp-1])->ref;
            tmp_const.ref = &tmp_int;
            tmp_const.type = INT_TYPE;
            stack[sp-1] = &tmp_const;
            ++ip;

            break;
        case OP_JUMP_TO: 
            ip = instructions[ip+1];

            break;
        case OP_POP_JUMP_ZERO:
            if (*(signed long long*)((YelConstant*)stack[--sp])->ref)
                ip += 2;
            else 
                ip = instructions[ip+1];

            break;

        case OP_STORE:
            if ((*(YelVariable*)instructions[ip+1]).ref == NULL) {
                if ((*(YelConstant*)stack[sp-1]).type == INT_TYPE) {
                    (*(YelVariable*)instructions[ip+1]).ref = __builtin_malloc(sizeof(YelConstant));
                    (*(YelConstant*)(*(YelVariable*)instructions[ip+1]).ref).ref = __builtin_malloc(YEL_SIZE_INT);

                    (*(YelConstant*)(*(YelVariable*)instructions[ip+1]).ref).type = INT_TYPE;
                }
            }

            (*(long long*)(*(YelConstant*)(*(YelVariable*)instructions[ip+1]).ref).ref) = (*(long long*)(*(YelConstant*)stack[--sp]).ref);

            //(*(YelVariable*)instructions[ip+1]).ref = yel_alloc_Int_data((*(long long*)(*(YelConstant*)stack[--sp]).ref));

            ip += 2;

            break;
        }
    }

_end:
    return;

_debug_info:
    clock_t end = clock(); 

    printf("VM Info:\n--> sp = %llu\n--> ip = %llu\n", sp, ip);
    if (sp != 0) printf("--> top = %lld (%p)\n", *(signed long long*)((YelConstant*)stack[sp-1])->ref, ((YelConstant*)stack[sp-1])->ref);
    else printf("--> stack is empty\n");

    printf("--> exec time = %.7f\n", (double)(end - begin) / CLOCKS_PER_SEC);
    return;

_emergency_stop:

}
