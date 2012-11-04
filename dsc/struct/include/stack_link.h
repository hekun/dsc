#ifndef STACK_LINK_INCLUDE
#define STACK_LINK_INCLUDE

#include "stack.h"

void RegisterStackFuncs_Link(Stack_funcs_t * stk_funcs, stack_type_t type, opt_visit visit);
void LogoutStackFuncs_Link(Stack_funcs_t * stk_funcs);



#endif
