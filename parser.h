#ifndef __PARSER_H__
#define __PARSER_H__

#include "dependencies.h"

void yel_parse_statement(YelTokens* yel_tokens);
void yel_parse_expression(YelTokens* yel_tokens);

#endif //__PARSER_H__