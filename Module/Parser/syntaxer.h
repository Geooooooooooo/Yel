#ifndef __SYNTAXER_H__
#define __SYNTAXER_H__

#include "../Dependencies/dependencies.h"

void yel_gen_parse_tree(YelTokens* yel_tokens);
YelEntities yel_define_next_entity(YelTokens* yel_tokens);

#endif //__SYNTAXER_H__
