#include "vm_main.h"
#include "yvm.h"

void yvm_main(OPCODEWORD* stack, YelByteCode* bytecode, size_t stack_size) {
    register unsigned long long ip = 0;             // instruntion pointer
    register unsigned long long sp = 0;             // stack pointer

    register OPCODEWORD a;
    register OPCODEWORD b;

    OPCODEWORD* instructions = bytecode->opcode;

    clock_t begin = clock();

    while (1) {
        if (sp == stack_size) {
            puts("RuntimeError: stack overflow");
            goto _emergency_stop;
        }

        switch (instructions[ip]) {
        case OP_HALT:
            goto _debug_info; // _end;

        case LOAD_VALUE:
            stack[sp] = TO_YEL_VAR(instructions[ip+1]).ref;

            if (stack[sp] == NULL) {
                printf("RuntimeError: undeclared name '%s'\n", TO_YEL_VAR(instructions[ip+1]).name);
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

        case DUP_VALUE:     // fix it
            stack[sp] = stack[sp-1];
            ++sp;
            ++ip;
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
                stack[sp-1] = yel_set_unused_int_memory(
                    *(signed long long*)((YelConstant*)b)->ref /
                    *(signed long long*)((YelConstant*)a)->ref,
                stack, sp);
                break;
            case BYNARY_MUL:
                stack[sp-1] = yel_set_unused_int_memory(
                    *(signed long long*)((YelConstant*)b)->ref *
                    *(signed long long*)((YelConstant*)a)->ref,
                stack, sp);
                break;
            case BYNARY_MOD:
                stack[sp-1] = yel_set_unused_int_memory(
                    *(signed long long*)((YelConstant*)b)->ref %
                    *(signed long long*)((YelConstant*)a)->ref,
                stack, sp);
                break;
            case BYNARY_ADD:
                stack[sp-1] = yel_set_unused_int_memory(
                    *(signed long long*)((YelConstant*)b)->ref +
                    *(signed long long*)((YelConstant*)a)->ref,
                stack, sp);
                break;
            case BYNARY_SUB:
                stack[sp-1] = yel_set_unused_int_memory(
                    *(signed long long*)((YelConstant*)b)->ref -
                    *(signed long long*)((YelConstant*)a)->ref,
                stack, sp);
                
                break;
            case BYNARY_RSH:
                stack[sp-1] = yel_set_unused_int_memory(
                    *(signed long long*)((YelConstant*)b)->ref >>
                    *(signed long long*)((YelConstant*)a)->ref,
                stack, sp);
                break;
            case BYNARY_LSH:
                stack[sp-1] = yel_set_unused_int_memory(
                    *(signed long long*)((YelConstant*)b)->ref <<
                    *(signed long long*)((YelConstant*)a)->ref,
                stack, sp);
                break;
            case BYNARY_MORE:
                stack[sp-1] = yel_set_unused_int_memory(
                    *(signed long long*)((YelConstant*)b)->ref >
                    *(signed long long*)((YelConstant*)a)->ref,
                stack, sp);
                break;
            case BYNARY_LESS:
                stack[sp-1] = yel_set_unused_int_memory(
                    *(signed long long*)((YelConstant*)b)->ref <
                    *(signed long long*)((YelConstant*)a)->ref,
                stack, sp);
                break;
            case BYNARY_MORE_EQ:
                stack[sp-1] = yel_set_unused_int_memory(
                    *(signed long long*)((YelConstant*)b)->ref >=
                    *(signed long long*)((YelConstant*)a)->ref,
                stack, sp);
                break;
            case BYNARY_LESS_EQ:
                stack[sp-1] = yel_set_unused_int_memory(
                    *(signed long long*)((YelConstant*)b)->ref <=
                    *(signed long long*)((YelConstant*)a)->ref,
                stack, sp);
                break;
            case BYNARY_EQ:
                stack[sp-1] = yel_set_unused_int_memory(
                    *(signed long long*)((YelConstant*)b)->ref ==
                    *(signed long long*)((YelConstant*)a)->ref,
                stack, sp);
                break;
            case BYNARY_NOT_EQ:
                stack[sp-1] = yel_set_unused_int_memory(
                    *(signed long long*)((YelConstant*)b)->ref !=
                    *(signed long long*)((YelConstant*)a)->ref,
                stack, sp);
                break;
            case BYNARY_AND:
                stack[sp-1] = yel_set_unused_int_memory(
                    *(signed long long*)((YelConstant*)b)->ref &
                    *(signed long long*)((YelConstant*)a)->ref,
                stack, sp);
                break;
            case BYNARY_OR:
                stack[sp-1] = yel_set_unused_int_memory(
                    *(signed long long*)((YelConstant*)b)->ref |
                    *(signed long long*)((YelConstant*)a)->ref,
                stack, sp);
                break;
            case BYNARY_LOGICAL_AND:
                stack[sp-1] = yel_set_unused_int_memory(
                    *(signed long long*)((YelConstant*)b)->ref &&
                    *(signed long long*)((YelConstant*)a)->ref,
                stack, sp);
                break;
            case BYNARY_LOGICAL_OR:
                stack[sp-1] = yel_set_unused_int_memory(
                    *(signed long long*)((YelConstant*)b)->ref ||
                    *(signed long long*)((YelConstant*)a)->ref,
                stack, sp);
                break;
            }
        
            ++ip;

            break;
        case UNARY_NEG:
            stack[sp-1] = yel_set_unused_int_memory(
                -*(signed long long*)((YelConstant*)stack[sp-1])->ref,
                stack, sp);
            ++ip;
            break;
        
        case UNARY_POS:
            stack[sp-1] = yel_set_unused_int_memory(
                +*(signed long long*)((YelConstant*)stack[sp-1])->ref,
                stack, sp);
            ++ip;
            break;

        case UNARY_NOT:
            stack[sp-1] = yel_set_unused_int_memory(
                ~*(signed long long*)((YelConstant*)stack[sp-1])->ref,
                stack, sp);
            ++ip;
            break;

        case UNARY_LOGICAL_NOT:
            stack[sp-1] = yel_set_unused_int_memory(
                !*(signed long long*)((YelConstant*)stack[sp-1])->ref,
                stack, sp);
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
                else if ((*(YelConstant*)stack[sp-1]).type == FLT_TYPE) {
                    (*(YelVariable*)instructions[ip+1]).ref = __builtin_malloc(sizeof(YelConstant));
                    (*(YelConstant*)(*(YelVariable*)instructions[ip+1]).ref).ref = __builtin_malloc(YEL_SIZE_FLT);

                    (*(YelConstant*)(*(YelVariable*)instructions[ip+1]).ref).type = FLT_TYPE;
                }

                // another types will be here
            }

            if ((*(YelConstant*)stack[sp-1]).type == INT_TYPE)
                (*(long long*)(*(YelConstant*)(*(YelVariable*)instructions[ip+1]).ref).ref) = (*(long long*)(*(YelConstant*)stack[--sp]).ref);
            else if ((*(YelConstant*)stack[sp-1]).type == FLT_TYPE) 
                (*(long double*)(*(YelConstant*)(*(YelVariable*)instructions[ip+1]).ref).ref) = (*(long double*)(*(YelConstant*)stack[--sp]).ref);

            ip += 2;

            break;
        }
    }

_end: return;

_debug_info:
    clock_t end = clock(); 

    printf("Debug Info:\n--> sp = %llu\n--> ip = %llu\n", sp, ip);
    if (sp != 0) {
        if (((YelConstant*)stack[sp-1])->type == INT_TYPE)
            printf("--> top = %lld (%p)\n", *(signed long long*)((YelConstant*)stack[sp-1])->ref, ((YelConstant*)stack[sp-1])->ref);
        else if (((YelConstant*)stack[sp-1])->type == FLT_TYPE)
            printf("--> top = %Lf (%p)\n", *(long double*)((YelConstant*)stack[sp-1])->ref, ((YelConstant*)stack[sp-1])->ref);
    }
    else printf("--> stack is empty\n");

    printf("--> exec time = %.6f sec\n", (double)(end - begin) / CLOCKS_PER_SEC);

    if (sp == 0) return;

    puts("Stack:");
    for (unsigned long long i = 0; i < sp; i++)
        printf("[%p]: %lld\n", stack[i], TO_LL(TO_YEL_CONST(stack[i]).ref));

    return;

_emergency_stop:

}
