#ifndef __PARSER_H__
#define __PARSER_H__

#include "../Dependencies/dependencies.h"

void yel_parse_statement(YelTokens*, YelByteCode*);
void yel_parse_expression(YelTokens*, YelByteCode*);

void yel_parse_stmt();
void yel_parse_name_decl_stmt();
void yel_parse_assign_stmt();
void yel_parse_return_stmt();
void yel_parse_if_stmt();
void yel_parse_if_elsef_stmt();
void yel_parse_curly_bracket_stmt();

void yel_parse_decl();
void yel_parse_func_decl();

void yel_parse_expr();
void yel_parse_bin_op_expr();
void yel_parse_unary_op_expr();
void yel_parse_enumeration_expr();
void yel_parse_func_decl_expr();
void yel_parse_brackets_expr();
void yel_parse_func_call_expr();

#endif //__PARSER_H__
