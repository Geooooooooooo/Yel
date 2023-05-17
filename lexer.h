#include "dependencies.h"

#ifndef __LEXER_H__
#define __LEXER_H__

void yel_get_next_token(Source* source, YelTokenType* t_token_type, char* token_value);
YelTokens yel_parse_tokens(Source* source);
void  yel_free_tokens(YelTokens* daf_tokens);

#endif //__LEXER_H__