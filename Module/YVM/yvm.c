#include "yvm.h"

void print_disassembly_opcode(OPCODEWORD w) {
    static int next = 0;
    // 1 = name
    // 2 = address
    // 3 = const
    // 0 = default

   if (next == 1) {next = 0;
        printf("%p (name address)\n", w);return;
    }
    else if (next == 2) {next = 0;
        printf("%p  (to %llu)\n", w, w);return;
    }
    else if (next == 3) {next = 0;
        printf("%p (const address)\n", w);return;
    }

    switch (w)
    {
    case OP_HALT:       puts("HALT");break;
    case LOAD_VALUE:    printf("LOAD_VALUE\t");next=1;break;
    case LOAD_CONST:    printf("LOAD_CONST\t");next=3;break;
    case POP_VALUE:     puts("POP_VALUE");break;
    case DUP_VALUE:     puts("DUP_VALUE");break;
    case UNARY_POS:     puts("UNARY_POS");break;
    case UNARY_NEG:     puts("UNARY_NEG");break;
    case UNARY_INC:     puts("UNARY_INC");break;
    case UNARY_DEC:     puts("UNARY_DEC");break;
    case BYNARY_OP:     printf("BYNARY_OP\t");break;
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
    case OP_JUMP_TO:    printf("JUMP_TO\t\t");next=2;break;
    case OP_JUMP_ZERO:  printf("JUMP_ZERO\t");next=2;break;
    case OP_CALL:       printf("CALL\t\t");next=1;break;
    case OP_RET:        puts("RET");break;
    case OP_BRK:        puts("BRK");break;
    case OP_STORE:      printf("STORE\t");next=1;break;
    default: printf("%p\n", w);break;
    }
}
