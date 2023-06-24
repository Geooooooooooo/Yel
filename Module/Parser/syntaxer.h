#ifndef __SYNTAXER_H__
#define __SYNTAXER_H__

#include "../Dependencies/dependencies.h"

_Bool yel_check_expr_grammar();
_Bool yel_check_expr(YelTokens* yel_tokens, int stmt);

_Bool yek_check_stmt(YelTokens* yel_tokens);
void yel_gen_opcode(YelTokens* yel_tokens, OPCODES* codes);

#endif //__SYNTAXER_H__