#ifndef __SYNTAXER_H__
#define __SYNTAXER_H__

#include "../Dependencies/dependencies.h"

_Bool yel_check_expr_grammar();
_Bool yel_check_expr(YelTokens* yel_tokens);
void yel_gen_parse_tree(YelTokens* yel_tokens);
YelEntities yel_define_next_entity(YelTokens* yel_tokens);

#endif //__SYNTAXER_H__