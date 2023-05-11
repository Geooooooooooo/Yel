#include "dependencies.h"

void daf_get_next_token(Source* source, DafTokenType* t_token_type, char* token_value);
_Bool daf_is_number(char c);
_Bool daf_is_alpha(char c);
_Bool daf_is_operator(char c);
