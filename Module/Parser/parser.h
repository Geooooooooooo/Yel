#ifndef __PARSER_H__
#define __PARSER_H__

#include "../Dependencies/dependencies.h"

void yel_parse_statement(YelTokens*, YelByteCode*);
void yel_parse_expression(YelTokens*, YelByteCode*);

#endif //__PARSER_H__