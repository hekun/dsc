#include <assert.h>
#include "stack.h"
#include "stack_link.h"
void RegisterStackFuncs(Stack_funcs_t * stk_funcs, stack_type_t type, stack_visit visit)
{
    assert(stk_funcs);
    RegisterStackFuncs_Link(stk_funcs, type, visit);
}

void LogoutStackFuncs(Stack_funcs_t *funcs, stack_type_t type)
{
    LogoutStackFuncs_Link(funcs);
}

