#include "yvm.h"

void print_disassembly_opcode(OPCODEWORD w, size_t count) {
    static int next = 0;
    // 1 = name
    // 2 = address
    // 3 = const
    // 0 = default

    if (next == 1) {next = 0;
        printf("%s (%p)\n", (char*)w, w);return;
    }
    else if (next == 2) {next = 0;
        printf("%p (to %llu)\n", w, w);return;
    }
    else if (next == 3) {next = 0;
        printf("%p (const address)\n", w);return;
    }

    switch (w)
    {
    case OP_HALT:       printf("%llu:\tHALT\n", count);break;
    case LOAD_VALUE:    printf("%llu:\tLOAD_VALUE\t", count);next=1;break;
    case LOAD_CONST:    printf("%llu:\tLOAD_CONST\t", count);next=3;break;
    case POP_VALUE:     printf("%llu:\tPOP_VALUE\n", count);break;
    case DUP_VALUE:     printf("%llu:\tDUP_VALUE\n", count);break;
    case UNARY_POS:     printf("%llu:\tUNARY_POS\n", count);break;
    case UNARY_NOT:     printf("%llu:\tUNARY_NOT\n", count);break;
    case UNARY_NEG:     printf("%llu:\tUNARY_NEG\n", count);break;
    case UNARY_INC:     printf("%llu:\tUNARY_INC\n", count);break;
    case UNARY_DEC:     printf("%llu:\tUNARY_DEC\n", count);break;
    case BYNARY_OP:     printf("%llu:\tBYNARY_OP\t", count);break;
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
    case OP_JUMP_TO:    printf("%llu:\tJUMP_TO\t\t", count);next=2;break;
    case OP_JUMP_ZERO:  printf("%llu:\tJUMP_ZERO\t", count);next=2;break;
    case OP_CALL:       printf("%llu:\tCALL\t\t", count);next=1;break;
    case OP_RET:        printf("%llu:\tRET\n", count);break;
    case OP_BRK:        printf("%llu:\tBRK\n", count);break;
    case OP_STORE:      printf("%llu:\tSTORE\t\t", count);next=1;break;
    default:            printf("%p\n", w);break;
    }
}
