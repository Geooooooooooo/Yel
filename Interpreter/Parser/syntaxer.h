#ifndef __SYNTAXER_H__
#define __SYNTAXER_H__

#include "../Dependencies/dependencies.h"

YelParsingEntities yel_define_next_entity(YelTokens*);

_Bool yel_check_expr_grammar();
_Bool yel_check_expr(YelTokens*, int);
_Bool yek_check_stmt(YelTokens*);
void yel_gen_opcode(YelTokens*, YelByteCode*);

#endif //__SYNTAXER_H__