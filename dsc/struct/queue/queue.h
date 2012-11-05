#ifndef QUEUE_INCLUDE
#define QUEUE_INCLUDE

#include "g_type.h"
#include "v_data.h"

#define QUEUE_T  queue_attr_t
typedef struct  QUEUE_T *QUEUE_T;

typedef enum queue_type_E
{
    UNKNOWN_STACK,
    QUEUE_SIGNAL_LINK_LIST,
    QUEUE_DOUBLE_LINK_LIST,
    QUEUE_CIRCLE_LINK_LIST
}queue_type_t;


typedef Status  (*queue_visit) (void *val);
typedef Status  (*InitQueue) (QUEUE_T * q);
typedef Status  (*DestroyQueue) (QUEUE_T *q);
typedef Status  (*ClearQueue) (QUEUE_T q);
typedef Status  (*QueueEmpty) (QUEUE_T q);
typedef Int32_t (*QueueLength)(QUEUE_T q);
typedef Status  (*GetHead)  (QUEUE_T q, v_type_t type, void **val, size_t size);
typedef Status  (*EnQueue)  (QUEUE_T q, v_type_t type, void *val, size_t size);
typedef Status  (*DeQueue)  (QUEUE_T q, v_type_t type, void *val, size_t size);
typedef Status  (*QueueTraverse) (QUEUE_T q, queue_visit visit);


#endif
