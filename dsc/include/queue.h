#ifndef QUEUE_INCLUDE
#define QUEUE_INCLUDE

#include "g_type.h"
#include "v_data.h"

#define QUEUE_T  queue_attr_t
typedef struct  QUEUE_T *QUEUE_T;

typedef enum queue_type_E
{
    UNKNOWN_QUEUE,
    QUEUE_SIGNAL_LINK_LIST,
    QUEUE_DOUBLE_LINK_LIST,
    QUEUE_CIRCLE_LINK_LIST
}queue_type_t;


typedef Status  (*queue_visit) (void *val);
typedef Status  (*InitQueue) (QUEUE_T *Q, queue_type_t type, queue_visit visit);//构造一个空链队列
typedef void    (*DestroyQueue) (QUEUE_T *Q);//销毁已建立的链队列
typedef void    (*ClearQueue) (QUEUE_T Q);//将链队列中元素全部清空
typedef Status  (*QueueEmpty) (QUEUE_T Q);//判断链队列是否为空。
typedef void    (*QueueLength)(QUEUE_T Q, Int32_t *Length);//获取链队列元素总数

typedef Status  (*GetQueueHead)  (QUEUE_T Q, v_type_t type, void **val, size_t size);//获取链队列头元素数值
typedef Status  (*GetQueueHead_Vdata) (QUEUE_T Q, v_data_t **vdata);

typedef Status  (*EnQueue)  (QUEUE_T Q, v_type_t type, void *val, size_t size);//在链队列中追加尾元素
typedef Status  (*EnQueue_Vdata) (QUEUE_T Q, v_data_t *vdata);

typedef void    (*DeQueue)  (QUEUE_T Q, v_type_t type, void *val, size_t size);//删除链队列头元素
typedef Status  (*DeQueue_Vdata)(QUEUE_T Q, v_data_t **vdata);

typedef Status  (*QueueTraverse) (QUEUE_T Q, queue_visit visit);//对链队列的每个元素执行visit函数。

typedef struct queue_opt_funcs_S
{
    queue_visit visit;
}queue_opt_funcs_t;

typedef struct queue_funcs_S
{
    InitQueue       init_queue;
    DestroyQueue    destroy_queue;
    ClearQueue      clear_queue;
    QueueEmpty      queue_empty;
    QueueLength     queue_length;
    GetQueueHead    get_head;
    GetQueueHead_Vdata get_queue_head_vdata;
    EnQueue         en_queue;
    EnQueue_Vdata   en_queue_vdata;
    DeQueue         de_queue;
    DeQueue_Vdata   de_queue_vdata;
    QueueTraverse   queue_traverse;
    queue_opt_funcs_t opt_func;//可选函数，该函数是否设置，会影响其他函数是否有效。
}queue_funcs_t;

void RegisterQueueFuncs(queue_funcs_t * q_funcs, queue_type_t type, queue_visit visit);
void LogoutqueueFuncs(queue_funcs_t * funcs, queue_type_t type);


#undef QUEUE_T
#endif

