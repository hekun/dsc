#include <assert.h>
#include "queue.h"
#include "queue_link.h"
void RegisterQueueFuncs(queue_funcs_t * q_funcs, queue_type_t type, queue_visit visit)
{
    assert(q_funcs);
    RegisterQueueFuncs_Link(q_funcs, type, visit);
}

void LogoutqueueFuncs(queue_funcs_t *funcs, queue_type_t type)
{
    LogoutQueueFuncs_Link(funcs);
}
