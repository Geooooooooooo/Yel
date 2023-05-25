#ifndef __PARSER_H__
#define __PARSER_H__

#include "dependencies.h"

static int brakets = 0;
static int recursive_descent = 0;

void yel_parse_statement(YelTokens* yel_tokens);
void yel_parse_expression(YelTokens* yel_tokens);

#endif //__PARSER_H__