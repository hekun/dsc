#ifndef QUEUE_LINK_INCLUDE
#define QUEUE_LINK_INCLUDE


#include "queue.h"

void RegisterQueueFuncs_Link(queue_funcs_t * q_funcs, queue_type_t type, queue_visit visit);
void LogoutQueueFuncs_Link(queue_funcs_t * q_funcs);


#endif
