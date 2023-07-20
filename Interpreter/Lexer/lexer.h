#include "../Dependencies/dependencies.h"

#ifndef __LEXER_H__
#define __LEXER_H__

void yel_get_next_token(YelSource*, YelTokenType*, char*);
YelTokens yel_parse_tokens(YelSource*);
void  yel_free_tokens(YelTokens*);

#endif //__LEXER_H__
