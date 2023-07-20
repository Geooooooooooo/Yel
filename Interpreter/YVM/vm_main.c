#include "vm_main.h"
#include "yvm.h"

void yvm_main(OPCODEWORD* stack, YelByteCode* bytecode, size_t stack_size) {
    register unsigned long long ip = 0;             // instruntion pointer
    register unsigned long long sp = 0;             // stack pointer

    register OPCODEWORD a;
    register OPCODEWORD b;

    clock_t begin = clock();

    while (1) {
        if (sp == stack_size) {
            puts("RuntimeError: stack overflow");
            goto _emergency_stop;
        }

        switch (bytecode->opcode[ip]) {
        case OP_HALT:
            goto _debug_info; // _end;

        case LOAD_VALUE:
            stack[sp] = TO_YEL_VAR(bytecode->opcode[ip+1]).ref;

            if (stack[sp] == NULL) {
                printf("RuntimeError: undeclared name '%s'\n", TO_YEL_VAR(bytecode->opcode[ip+1]).name);
                goto _emergency_stop;
            }

            ++sp;
            ip += 2;

            break;

        case LOAD_CONST: 
            stack[sp] = bytecode->opcode[ip+1];
            ++sp;
            ip += 2;
            break;

        case DUP_VALUE:     // fix it
            stack[sp] = stack[sp-1];
            ++sp;
            ++ip;
            break;

        case BYNARY_OP:
            --sp;
            
            a = stack[sp];
            b = stack[sp-1];

            switch (bytecode->opcode[++ip])
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
                if (TO_YEL_CONST(b).type == INT_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_float_memory(
                            (long double)TO_LL(TO_YEL_CONST(b).ref) / (long double)TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_float_memory(
                            (long double)TO_LL(TO_YEL_CONST(b).ref) / TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_float_memory(
                            (long double)TO_LL(TO_YEL_CONST(b).ref) / (long double)TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else if (TO_YEL_CONST(b).type == FLT_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_float_memory(
                            TO_LD(TO_YEL_CONST(b).ref) / (long double)TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_float_memory(
                            TO_LD(TO_YEL_CONST(b).ref) / TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_float_memory(
                            TO_LD(TO_YEL_CONST(b).ref) / (long double)TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else if (TO_YEL_CONST(b).type == BOOL_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_float_memory(
                            (long double)TO_B(TO_YEL_CONST(b).ref) / (long double)TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_float_memory(
                            (long double)TO_B(TO_YEL_CONST(b).ref) / TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_float_memory(
                            (long double)TO_B(TO_YEL_CONST(b).ref) / (long double)TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }

                break;
            case BYNARY_MUL:
                if (TO_YEL_CONST(b).type == INT_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_int_memory(
                            TO_LL(TO_YEL_CONST(b).ref) * TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_float_memory(
                            TO_LL(TO_YEL_CONST(b).ref) * TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_int_memory(
                            TO_LL(TO_YEL_CONST(b).ref) * TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else if (TO_YEL_CONST(b).type == FLT_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_float_memory(
                            TO_LD(TO_YEL_CONST(b).ref) * TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_float_memory(
                            TO_LD(TO_YEL_CONST(b).ref) * TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_float_memory(
                            TO_LD(TO_YEL_CONST(b).ref) * TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else if (TO_YEL_CONST(b).type == BOOL_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_int_memory(
                            TO_B(TO_YEL_CONST(b).ref) * TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_float_memory(
                            TO_B(TO_YEL_CONST(b).ref) * TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) * TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }

                break;
            case BYNARY_MOD:
                if (TO_YEL_CONST(b).type == INT_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) % TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if(TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) % TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else if (TO_YEL_CONST(b).type == BOOL_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) % TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if(TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) % TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else {
                    puts("Error");
                }
                break;
            case BYNARY_ADD:
                if (TO_YEL_CONST(b).type == INT_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_int_memory(
                            TO_LL(TO_YEL_CONST(b).ref) + TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_float_memory(
                            TO_LL(TO_YEL_CONST(b).ref) + TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_int_memory(
                            TO_LL(TO_YEL_CONST(b).ref) + TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else if (TO_YEL_CONST(b).type == FLT_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_float_memory(
                            TO_LD(TO_YEL_CONST(b).ref) + (long double)TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_float_memory(
                            TO_LD(TO_YEL_CONST(b).ref) + TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_float_memory(
                            TO_LD(TO_YEL_CONST(b).ref) + (long double)TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else if (TO_YEL_CONST(b).type == BOOL_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_int_memory(
                            TO_B(TO_YEL_CONST(b).ref) + TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_float_memory(
                            (long double)TO_B(TO_YEL_CONST(b).ref) + TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_int_memory(
                            TO_B(TO_YEL_CONST(b).ref) + TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                break;
            case BYNARY_SUB:
                if (TO_YEL_CONST(b).type == INT_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_int_memory(
                            TO_LL(TO_YEL_CONST(b).ref) - TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_float_memory(
                            (long double)TO_LL(TO_YEL_CONST(b).ref) - TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_int_memory(
                            TO_LL(TO_YEL_CONST(b).ref) - TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else if (TO_YEL_CONST(b).type == FLT_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_float_memory(
                            TO_LD(TO_YEL_CONST(b).ref) - (long double)TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_float_memory(
                            TO_LD(TO_YEL_CONST(b).ref) - TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_float_memory(
                            TO_LD(TO_YEL_CONST(b).ref) - (long double)TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else if (TO_YEL_CONST(b).type == BOOL_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_int_memory(
                            TO_B(TO_YEL_CONST(b).ref) - TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_float_memory(
                            (long double)TO_B(TO_YEL_CONST(b).ref) - TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_int_memory(
                            TO_B(TO_YEL_CONST(b).ref) - TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                
                break;
            case BYNARY_RSH:
                if (TO_YEL_CONST(b).type == INT_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) >> TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if(TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) >> TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else if (TO_YEL_CONST(b).type == BOOL_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) >> TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if(TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) >> TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else {
                    puts("Error");
                }
                break;
            case BYNARY_LSH:
                if (TO_YEL_CONST(b).type == INT_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) << TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if(TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) << TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else if (TO_YEL_CONST(b).type == BOOL_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) << TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if(TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) << TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else {
                    puts("Error");
                }
                break;
            case BYNARY_MORE: 
                if (TO_YEL_CONST(b).type == INT_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) > TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) > TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) > TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else if (TO_YEL_CONST(b).type == FLT_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LD(TO_YEL_CONST(b).ref) > TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LD(TO_YEL_CONST(b).ref) > TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LD(TO_YEL_CONST(b).ref) > TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else if (TO_YEL_CONST(b).type == BOOL_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) > TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) > TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) > TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                break;
            case BYNARY_LESS:
                if (TO_YEL_CONST(b).type == INT_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) < TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) < TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) < TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else if (TO_YEL_CONST(b).type == FLT_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LD(TO_YEL_CONST(b).ref) < TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LD(TO_YEL_CONST(b).ref) < TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LD(TO_YEL_CONST(b).ref) < TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else if (TO_YEL_CONST(b).type == BOOL_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) < TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) < TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) < TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                break;
            case BYNARY_MORE_EQ:
                if (TO_YEL_CONST(b).type == INT_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) >= TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) >= TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) >= TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else if (TO_YEL_CONST(b).type == FLT_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LD(TO_YEL_CONST(b).ref) >= TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LD(TO_YEL_CONST(b).ref) >= TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LD(TO_YEL_CONST(b).ref) >= TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else if (TO_YEL_CONST(b).type == BOOL_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) >= TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) >= TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) >= TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                break;
            case BYNARY_LESS_EQ:
                if (TO_YEL_CONST(b).type == INT_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) <= TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) <= TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) <= TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else if (TO_YEL_CONST(b).type == FLT_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LD(TO_YEL_CONST(b).ref) <= TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LD(TO_YEL_CONST(b).ref) <= TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LD(TO_YEL_CONST(b).ref) <= TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else if (TO_YEL_CONST(b).type == BOOL_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) <= TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) <= TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) <= TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                break;
            case BYNARY_EQ:
                if (TO_YEL_CONST(b).type == INT_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) == TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) == TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) == TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else if (TO_YEL_CONST(b).type == FLT_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LD(TO_YEL_CONST(b).ref) == TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LD(TO_YEL_CONST(b).ref) == TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LD(TO_YEL_CONST(b).ref) == TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else if (TO_YEL_CONST(b).type == BOOL_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) == TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) == TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) == TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }

                break;
            case BYNARY_NOT_EQ:
                if (TO_YEL_CONST(b).type == INT_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) != TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) != TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) != TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else if (TO_YEL_CONST(b).type == FLT_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LD(TO_YEL_CONST(b).ref) != TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LD(TO_YEL_CONST(b).ref) != TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LD(TO_YEL_CONST(b).ref) != TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else if (TO_YEL_CONST(b).type == BOOL_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) != TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) != TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) != TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                break;
            case BYNARY_AND:
                if (TO_YEL_CONST(b).type == INT_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) & TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if(TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) & TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else if (TO_YEL_CONST(b).type == BOOL_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) & TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if(TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) & TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else {
                    puts("Error");
                }
                break;
            case BYNARY_OR:
                if (TO_YEL_CONST(b).type == INT_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) | TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if(TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) | TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else if (TO_YEL_CONST(b).type == BOOL_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) | TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if(TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) | TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else {
                    puts("Error");
                }
                break;
            case BYNARY_LOGICAL_AND:
                if (TO_YEL_CONST(b).type == INT_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) && TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) && TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) && TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else if (TO_YEL_CONST(b).type == FLT_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LD(TO_YEL_CONST(b).ref) && TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LD(TO_YEL_CONST(b).ref) && TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LD(TO_YEL_CONST(b).ref) && TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else if (TO_YEL_CONST(b).type == BOOL_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) && TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) && TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) && TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                break;
            case BYNARY_LOGICAL_OR:
                if (TO_YEL_CONST(b).type == INT_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) || TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) || TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LL(TO_YEL_CONST(b).ref) || TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else if (TO_YEL_CONST(b).type == FLT_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LD(TO_YEL_CONST(b).ref) || TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LD(TO_YEL_CONST(b).ref) || TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_LD(TO_YEL_CONST(b).ref) || TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                else if (TO_YEL_CONST(b).type == BOOL_TYPE) {
                    if (TO_YEL_CONST(a).type == INT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) || TO_LL(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == FLT_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) || TO_LD(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                    else if (TO_YEL_CONST(a).type == BOOL_TYPE) {
                        stack[sp-1] = yel_set_unused_bool_memory(
                            TO_B(TO_YEL_CONST(b).ref) || TO_B(TO_YEL_CONST(a).ref), stack, sp
                        );
                    }
                }
                break;
            }
        
            ++ip;

            break;

        case OP_CALL:
            int call_args = (int)bytecode->opcode[ip+1];

            if (TO_YEL_CONST(stack[sp-1]).type != FUNC_TYPE) {
                printf("RuntimeError: '");

                if (TO_YEL_CONST(stack[sp-1]).type == INT_TYPE)
                    printf("Int");
                else if (TO_YEL_CONST(stack[sp-1]).type == FLT_TYPE)
                    printf("Flt");
                else if (TO_YEL_CONST(stack[sp-1]).type == BOOL_TYPE)
                    printf("Bool");
                else if (TO_YEL_CONST(stack[sp-1]).type == STR_TYPE)
                    printf("Str");

                printf("' type can not be call, must be 'func'\n");
                goto _emergency_stop;
            } 

            YelFunction func = TO_FUNC(TO_YEL_CONST(stack[--sp]).ref);

            if (call_args > func.argc) {
                printf("RuntimeError: too many arguments passed\n");
                goto _emergency_stop;
            }
            else if (call_args < func.argc) {
                printf("RuntimeError: too few arguments passed\n");
                goto _emergency_stop;
            }

            for (int l = func.argc-1; l >= 0; l--) {
                YelConstant arg = TO_YEL_CONST(stack[--sp]);

                if (TO_YEL_VAR(func.args[l]).ref == NULL) {
                    if (arg.type == INT_TYPE) {
                        TO_YEL_VAR(func.args[l]).ref = yel_alloc_Int_data(TO_LL(arg.ref));
                    }
                    else if (arg.type == FLT_TYPE) {
                        TO_YEL_VAR(func.args[l]).ref = yel_alloc_Flt_data(TO_LD(arg.ref));
                    }
                    else if (arg.type == BOOL_TYPE) {
                        TO_YEL_VAR(func.args[l]).ref = yel_alloc_Bool_data(TO_B(arg.ref));
                    }
                    else if (arg.type == FUNC_TYPE) {
                        TO_YEL_VAR(func.args[l]).ref = yel_alloc_func((YelFunction*)(arg.ref));
                    }
                }

                if (arg.type == INT_TYPE) {
                    TO_LL(TO_YEL_CONST(TO_YEL_VAR(func.args[l]).ref).ref) = TO_LL(arg.ref);
                }
                else if (arg.type == FLT_TYPE) {
                    TO_LD(TO_YEL_CONST(TO_YEL_VAR(func.args[l]).ref).ref) = TO_LD(arg.ref);
                }
                else if (arg.type == BOOL_TYPE) {
                    TO_B(TO_YEL_CONST(TO_YEL_VAR(func.args[l]).ref).ref) = TO_B(arg.ref);
                }
                else if (arg.type == FUNC_TYPE) {
                    TO_FUNC(TO_YEL_CONST(TO_YEL_VAR(func.args[l]).ref).ref) = TO_FUNC(arg.ref);
                }
            }

            stack[sp] = ip+2;
            stack[++sp] = NULL;
            ++sp;
            ip = func.start;

            break;

        case OP_RET: 
            a = stack[--sp];
            while (stack[sp] != NULL) --sp;

            ip = stack[--sp];
            stack[sp] = a;
            ++sp;

            break;

        case UNARY_NEG:
            if (TO_YEL_CONST(stack[sp-1]).type == INT_TYPE) {
                stack[sp-1] = yel_set_unused_int_memory(
                    -TO_LL(TO_YEL_CONST(stack[sp-1]).ref), stack, sp
                );
            }
            else if (TO_YEL_CONST(stack[sp-1]).type == FLT_TYPE) {
                stack[sp-1] = yel_set_unused_float_memory(
                    -TO_LD(TO_YEL_CONST(stack[sp-1]).ref), stack, sp
                );
            }
            else if (TO_YEL_CONST(stack[sp-1]).type == BOOL_TYPE) {
                stack[sp-1] = yel_set_unused_int_memory(
                    -TO_B(TO_YEL_CONST(stack[sp-1]).ref), stack, sp
                );
            }

            ++ip;
            break;
        
        case UNARY_POS:
            if (TO_YEL_CONST(stack[sp-1]).type == INT_TYPE) {
                stack[sp-1] = yel_set_unused_int_memory(
                    +TO_LL(TO_YEL_CONST(stack[sp-1]).ref), stack, sp
                );
            }
            else if (TO_YEL_CONST(stack[sp-1]).type == FLT_TYPE) {
                stack[sp-1] = yel_set_unused_float_memory(
                    +TO_LD(TO_YEL_CONST(stack[sp-1]).ref), stack, sp
                );
            }
            else if (TO_YEL_CONST(stack[sp-1]).type == BOOL_TYPE) {
                stack[sp-1] = yel_set_unused_int_memory(
                    +TO_B(TO_YEL_CONST(stack[sp-1]).ref), stack, sp
                );
            }

            ++ip;
            break;

        case UNARY_NOT:
            if (TO_YEL_CONST(stack[sp-1]).type == INT_TYPE) {
                stack[sp-1] = yel_set_unused_int_memory(
                    ~TO_LL(TO_YEL_CONST(stack[sp-1]).ref), stack, sp
                );
            }
            else if (TO_YEL_CONST(stack[sp-1]).type == BOOL_TYPE) {
                stack[sp-1] = yel_set_unused_int_memory(
                    ~TO_B(TO_YEL_CONST(stack[sp-1]).ref), stack, sp
                );
            }
            else {
                puts("Error");
            }
            ++ip;
            break;

        case UNARY_LOGICAL_NOT:
            if (TO_YEL_CONST(stack[sp-1]).type == BOOL_TYPE) {
                stack[sp-1] = yel_set_unused_bool_memory(
                    !TO_B(TO_YEL_CONST(stack[sp-1]).ref), stack, sp
                );
            }
            else if (TO_YEL_CONST(stack[sp-1]).type == INT_TYPE) {
                stack[sp-1] = yel_set_unused_bool_memory(
                    !TO_LL(TO_YEL_CONST(stack[sp-1]).ref), stack, sp
                );
            }
            else if (TO_YEL_CONST(stack[sp-1]).type == FLT_TYPE) {
                stack[sp-1] = yel_set_unused_bool_memory(
                    !TO_LD(TO_YEL_CONST(stack[sp-1]).ref), stack, sp
                );
            }
            ++ip;

            break;
        case OP_JUMP_TO: 
            ip = bytecode->opcode[ip+1];

            break;
        case OP_POP_JUMP_ZERO:
            if (TO_YEL_CONST(stack[sp-1]).type == BOOL_TYPE) {
                if (TO_B(TO_YEL_CONST(stack[--sp]).ref)) 
                    ip += 2;
                else
                    ip = bytecode->opcode[ip+1];
            }
            else if (TO_YEL_CONST(stack[sp-1]).type == INT_TYPE) {
                if (TO_LL(TO_YEL_CONST(stack[--sp]).ref))
                    ip += 2;
                else
                    ip = bytecode->opcode[ip+1];
            }
            else if (TO_YEL_CONST(stack[sp-1]).type == FLT_TYPE) {
                if (TO_LD(TO_YEL_CONST(stack[--sp]).ref))
                    ip += 2;
                else
                    ip = bytecode->opcode[ip+1];
            }

            break;

        case OP_STORE:
            if (TO_YEL_VAR(bytecode->opcode[ip+1]).ref == NULL) {
                if (TO_YEL_CONST(stack[sp-1]).type == INT_TYPE) {
                    TO_YEL_VAR(bytecode->opcode[ip+1]).ref = __builtin_malloc(sizeof(YelConstant));
                    TO_YEL_CONST(TO_YEL_VAR(bytecode->opcode[ip+1]).ref).ref = __builtin_malloc(YEL_SIZE_INT);
                    TO_YEL_CONST(TO_YEL_VAR(bytecode->opcode[ip+1]).ref).type = INT_TYPE;
                }
                else if (TO_YEL_CONST(stack[sp-1]).type == FLT_TYPE) {
                    TO_YEL_VAR(bytecode->opcode[ip+1]).ref = __builtin_malloc(sizeof(YelConstant));
                    TO_YEL_CONST(TO_YEL_VAR(bytecode->opcode[ip+1]).ref).ref = __builtin_malloc(YEL_SIZE_FLT);
                    TO_YEL_CONST(TO_YEL_VAR(bytecode->opcode[ip+1]).ref).type = FLT_TYPE;
                }
                else if (TO_YEL_CONST(stack[sp-1]).type == BOOL_TYPE) {
                    TO_YEL_VAR(bytecode->opcode[ip+1]).ref = __builtin_malloc(sizeof(YelConstant));
                    TO_YEL_CONST(TO_YEL_VAR(bytecode->opcode[ip+1]).ref).ref = __builtin_malloc(sizeof(_Bool));
                    TO_YEL_CONST(TO_YEL_VAR(bytecode->opcode[ip+1]).ref).type = BOOL_TYPE;
                }
                else if (TO_YEL_CONST(stack[sp-1]).type == FUNC_TYPE) {
                    TO_YEL_VAR(bytecode->opcode[ip+1]).ref = __builtin_malloc(sizeof(YelConstant));
                    TO_YEL_CONST(TO_YEL_VAR(bytecode->opcode[ip+1]).ref).ref = __builtin_malloc(sizeof(YelFunction));
                    TO_YEL_CONST(TO_YEL_VAR(bytecode->opcode[ip+1]).ref).type = FUNC_TYPE;
                }

                // another types will be here
            }

            // store int
            if (TO_YEL_CONST(TO_YEL_VAR(bytecode->opcode[ip+1]).ref).type == INT_TYPE) {
                if (TO_YEL_CONST(stack[sp-1]).type == INT_TYPE)
                    TO_LL(TO_YEL_CONST(TO_YEL_VAR(bytecode->opcode[ip+1]).ref).ref) = TO_LL(TO_YEL_CONST(stack[--sp]).ref);

                else if (TO_YEL_CONST(stack[sp-1]).type == FLT_TYPE)
                    TO_LL(TO_YEL_CONST(TO_YEL_VAR(bytecode->opcode[ip+1]).ref).ref) = (long long)TO_LD(TO_YEL_CONST(stack[--sp]).ref);

                else if (TO_YEL_CONST(stack[sp-1]).type == BOOL_TYPE)
                    TO_LL(TO_YEL_CONST(TO_YEL_VAR(bytecode->opcode[ip+1]).ref).ref) = (long long)TO_B(TO_YEL_CONST(stack[--sp]).ref);
            }

            // store float
            else if (TO_YEL_CONST(TO_YEL_VAR(bytecode->opcode[ip+1]).ref).type == FLT_TYPE) {
                if (TO_YEL_CONST(stack[sp-1]).type == FLT_TYPE)
                    TO_LD(TO_YEL_CONST(TO_YEL_VAR(bytecode->opcode[ip+1]).ref).ref) = TO_LD(TO_YEL_CONST(stack[--sp]).ref);

                else if (TO_YEL_CONST(stack[sp-1]).type == INT_TYPE)
                    TO_LD(TO_YEL_CONST(TO_YEL_VAR(bytecode->opcode[ip+1]).ref).ref) = (long double)TO_LL(TO_YEL_CONST(stack[--sp]).ref);

                else if (TO_YEL_CONST(stack[sp-1]).type == BOOL_TYPE)
                    TO_LD(TO_YEL_CONST(TO_YEL_VAR(bytecode->opcode[ip+1]).ref).ref) = (long double)TO_B(TO_YEL_CONST(stack[--sp]).ref);
            }

            // store bool
            else if (TO_YEL_CONST(TO_YEL_VAR(bytecode->opcode[ip+1]).ref).type == BOOL_TYPE) {
                if (TO_YEL_CONST(stack[sp-1]).type == BOOL_TYPE)
                    TO_B(TO_YEL_CONST(TO_YEL_VAR(bytecode->opcode[ip+1]).ref).ref) = TO_B(TO_YEL_CONST(stack[--sp]).ref);

                else if (TO_YEL_CONST(stack[sp-1]).type == INT_TYPE)
                    TO_B(TO_YEL_CONST(TO_YEL_VAR(bytecode->opcode[ip+1]).ref).ref) = (_Bool)TO_LL(TO_YEL_CONST(stack[--sp]).ref);

                else if (TO_YEL_CONST(stack[sp-1]).type == FLT_TYPE)
                    TO_B(TO_YEL_CONST(TO_YEL_VAR(bytecode->opcode[ip+1]).ref).ref) = (_Bool)TO_LD(TO_YEL_CONST(stack[--sp]).ref);
            }

            // store func
            else if (TO_YEL_CONST(TO_YEL_VAR(bytecode->opcode[ip+1]).ref).type == FUNC_TYPE) {
                if (TO_YEL_CONST(stack[sp-1]).type == FUNC_TYPE) {
                    TO_FUNC(TO_YEL_CONST(TO_YEL_VAR(bytecode->opcode[ip+1]).ref).ref).returt_type = TO_FUNC(TO_YEL_CONST(stack[sp-1]).ref).returt_type;
                    TO_FUNC(TO_YEL_CONST(TO_YEL_VAR(bytecode->opcode[ip+1]).ref).ref).argc = TO_FUNC(TO_YEL_CONST(stack[sp-1]).ref).argc;
                    TO_FUNC(TO_YEL_CONST(TO_YEL_VAR(bytecode->opcode[ip+1]).ref).ref).args = TO_FUNC(TO_YEL_CONST(stack[sp-1]).ref).args;
                    TO_FUNC(TO_YEL_CONST(TO_YEL_VAR(bytecode->opcode[ip+1]).ref).ref).start = TO_FUNC(TO_YEL_CONST(stack[--sp]).ref).start;
                }
            }

            ip += 2;

            break;
        }
    }

_end: return;

_debug_info:
    clock_t end = clock(); 

    printf("Debug Info:\n--> sp = %llu\n--> ip = %llu\n", sp, ip);
    if (sp != 0) {
        if (TO_YEL_CONST(stack[sp-1]).type == INT_TYPE)
            printf("--> top = %lld ", TO_LL(TO_YEL_CONST(stack[sp-1]).ref));
        else if (((YelConstant*)stack[sp-1])->type == FLT_TYPE)
            printf("--> top = %Lf ", TO_LD(TO_YEL_CONST(stack[sp-1]).ref));
        else if (((YelConstant*)stack[sp-1])->type == BOOL_TYPE)
            printf("--> top = %d ", TO_B(TO_YEL_CONST(stack[sp-1]).ref));

        printf("(%p)\n", TO_YEL_CONST(stack[sp-1]).ref);
    }
    else printf("--> stack is empty\n");

    printf("--> exec time = %.6f sec\n", (double)(end - begin) / CLOCKS_PER_SEC);

    if (sp == 0) return;

    puts("Stack:");
    for (unsigned long long i = 0; i < sp; i++) {
        printf("[%p]: ", stack[i]);

        if (((YelConstant*)stack[i])->type == INT_TYPE)
            printf("%lld (Int)\n", TO_LL(TO_YEL_CONST(stack[i]).ref));
        else if (((YelConstant*)stack[i])->type == FLT_TYPE)
            printf("%Lf (Flt)\n", TO_LD(TO_YEL_CONST(stack[i]).ref));
        else if (((YelConstant*)stack[i])->type == BOOL_TYPE)
            printf("%d (Bool)\n", TO_B(TO_YEL_CONST(stack[i]).ref));
        else if (((YelConstant*)stack[i])->type == FUNC_TYPE)
            printf("%p (func)\n", TO_YEL_CONST(stack[i]).ref);
    }

    return;

_emergency_stop:

}
