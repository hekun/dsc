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
typedef Status  (*InitQueue) (QUEUE_T *Q, queue_type_t type, opt_visit visit);//����һ����������
typedef void    (*DestroyQueue) (QUEUE_T *Q);//�����ѽ�����������
typedef void    (*ClearQueue) (QUEUE_T Q);//����������Ԫ��ȫ�����
typedef Status  (*QueueEmpty) (QUEUE_T Q);//�ж��������Ƿ�Ϊ�ա�
typedef void    (*QueueLength)(QUEUE_T Q, Int32_t *Length);//��ȡ������Ԫ������
typedef Status  (*GetQueueHead)  (QUEUE_T Q, v_type_t type, void **val, size_t size);//��ȡ������ͷԪ����ֵ
typedef Status  (*EnQueue)  (QUEUE_T Q, v_type_t type, void *val, size_t size);//����������׷��βԪ��
typedef void    (*DeQueue)  (QUEUE_T Q, v_type_t type, void *val, size_t size);//ɾ��������ͷԪ��
typedef Status  (*QueueTraverse) (QUEUE_T Q, queue_visit visit);//�������е�ÿ��Ԫ��ִ��visit������

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
    GetQueueHead         get_head;
    EnQueue         en_queue;
    DeQueue         de_queue;
    QueueTraverse  queue_traverse;
    queue_opt_funcs_t opt_func;//��ѡ�������ú����Ƿ����ã���Ӱ�����������Ƿ���Ч��
}queue_funcs_t;

#undef QUEUE_T
#endif
