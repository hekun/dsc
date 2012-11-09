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
typedef Status  (*InitQueue) (QUEUE_T * q);//����һ����������
typedef Status  (*DestroyQueue) (QUEUE_T *q);//�����ѽ�����������
typedef Status  (*ClearQueue) (QUEUE_T q);//����������Ԫ��ȫ�����
typedef Status  (*QueueEmpty) (QUEUE_T q);//�ж��������Ƿ�Ϊ�ա�
typedef Int32_t (*QueueLength)(QUEUE_T q);//��ȡ������Ԫ������
typedef Status  (*GetHead)  (QUEUE_T q, v_type_t type, void **val, size_t size);//��ȡ������ͷԪ����ֵ
typedef Status  (*EnQueue)  (QUEUE_T q, v_type_t type, void *val, size_t size);//����������׷��βԪ��
typedef Status  (*DeQueue)  (QUEUE_T q, v_type_t type, void *val, size_t size);//ɾ��������ͷԪ��
typedef Status  (*QueueTraverse) (QUEUE_T q, queue_visit visit);//�������е�ÿ��Ԫ��ִ��visit������

typedef struct queue_opt_funcs_S
{
    queue_visit visit;
}queue_opt_funcs_t;

typedef struct Queue_funcs_S
{
    InitQueue       init_queue;
    DestroyQueue    destroy_queue;
    ClearQueue      clear_queue;
    QueueEmpty      queue_empty;
    QueueLength     queue_length;
    GetHead         get_head;
    EnQueue         en_queue;
    DeQueue         de_queue;
    queue_visit     queue_visit;
}Queue_funcs_t;

#undef QUEUE_T
#endif

