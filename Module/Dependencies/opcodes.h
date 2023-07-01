#define OP_HALT             0LL
#define LOAD_VALUE          1LL
#define LOAD_CONST          2LL
#define POP_VALUE           3LL
#define DUP_VALUE           4LL

#define UNARY_POS           5LL
#define UNARY_NEG           6LL
#define UNARY_NOT           7LL
#define UNARY_INC           8LL
#define UNARY_DEC           9LL
#define UNARY_LOGICAL_NOT   10LL

#define BYNARY_OP           11LL
#define BYNARY_POW          12LL
#define BYNARY_DIV          13LL
#define BYNARY_MUL          14LL
#define BYNARY_MOD          15LL
#define BYNARY_ADD          16LL
#define BYNARY_SUB          17LL
#define BYNARY_RSH          18LL
#define BYNARY_LSH          19LL
#define BYNARY_MORE         20LL
#define BYNARY_LESS         21LL
#define BYNARY_MORE_EQ      22LL
#define BYNARY_LESS_EQ      23LL
#define BYNARY_EQ           24LL
#define BYNARY_NOT_EQ       25LL
#define BYNARY_AND          26LL
#define BYNARY_OR           27LL
#define BYNARY_LOGICAL_AND  28LL
#define BYNARY_LOGICAL_OR   29LL

#define OP_JUMP_TO          30LL
#define OP_POP_JUMP_ZERO    31LL

#define OP_CALL             32LL
#define OP_RET              33LL
#define OP_BRK              -1LL
#define OP_CNT              -2LL
#define OP_STORE            34LL
#define OP_NOP              35LL
#define MAKE_FUNC           36LL
