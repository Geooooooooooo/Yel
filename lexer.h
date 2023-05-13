#include "dependencies.h"

void daf_get_next_token(Source* source, DafTokenType* t_token_type, char* token_value);
DafTokens daf_parse_tokens(Source* source);
void daf_free_tokens(DafTokens* daf_tokens);
