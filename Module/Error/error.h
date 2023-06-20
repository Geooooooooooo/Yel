#ifndef __ERROR_H__
#define __ERROR_H__

#include "../Dependencies/dependencies.h"

void yel_print_error(const char* err_name, const char* err_desc, Source* src, size_t line, size_t symbol);

#endif //__ERROR_H__