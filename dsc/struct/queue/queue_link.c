#include <assert.h>
#include <strings.h>
#include "err_num.h"
#include "sys_lib.h"
#include "link.h"
#include "queue.h"

#define QUEUE_T queue_attr_t;
typedef struct queue_depdf_S
{
    InitLink        init_link;
    DestroyLink     destroy_link;
    InsertFirstData insert_first_data;
    AppendData      append_data;
    ClearLink       clear_link;
    LinkEmpty       link_empty;
    GetFirstData    get_first_data;
    GetLinkLength   get_link_length;
    LinkTraverse    link_traverse;
}queue_depdf_t;

struct QUEUE_T
{
    link_attr_t attr;
    queue_depdf_t depdf;
};
static Status RegisterDepdFuncs_Queue(queue_depdf_t *s_depdf,queue_type_t type, opt_visit visit);
static Status InitQueue_Link(QUEUE_T *Q, queue_type_t type, opt_visit visit);
static void DestroyQueue_Link(QUEUE_T *Q);
static Status EnQueue_Link(QUEUE_T Q, v_type_t type, void *data, size_t size);
static void DeQueue_Link(QUEUE_T Q, v_type_t type, void *data, size_t size);
static Status GetQueueHead_Link(QUEUE_T Q, v_type_t type, void **val, size_t size);
static Status QueueEmpty_Link(QUEUE_T Q);
static void ClearQueue_Link(QUEUE_T Q);
static void QueueLength_Link(QUEUE_T Q, Int32_t *Length);
static Status QueueTraverse_Link (QUEUE_T Q, queue_visit visit);


/*
��������:
    ע�������в���������Ҫ���õ�����������ֺ�����
����˵��:
    depdf--ջ��������������������������ռ��׵�ַ��
    type--ָ����������֯���͡�
    visit--��ʾջֵ������
����ֵ:
    OK--ע��ɹ���
    !OK--��ȡ������ʧ�ܡ�
����:
    He kun
��������:
    2012-11-04
*/
static Status RegisterDepdFuncs_Queue(queue_depdf_t *s_depdf,queue_type_t type, opt_visit visit)
{
    Status rc = OK;
    link_funcs_t l_func;
    link_opt_funcs_t optf;
    optf.visit = visit;
    switch(type)
    {
        case QUEUE_CIRCLE_LINK_LIST:
        case QUEUE_DOUBLE_LINK_LIST:
        case QUEUE_SIGNAL_LINK_LIST:
            rc = RegisterLinkFuncs(&l_func, type, &optf);
            break;
        default:break;
    }
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"RegisterLinkFuncs failed,rc=%d",rc);
        return rc;
    }
    if(s_depdf != NULL)
    {
        s_depdf->init_link = l_func.init_link;
        s_depdf->destroy_link = l_func.destroy_link;
        s_depdf->insert_first_data = l_func.insert_first_data;
        s_depdf->append_data = l_func.append_data;
        s_depdf->clear_link = l_func.clear_link;
        s_depdf->link_empty = l_func.link_empty;
        s_depdf->get_first_data = l_func.get_first_data;
        s_depdf->get_link_length = l_func.get_link_length;
        s_depdf->link_traverse = l_func.link_traverse;
    }
    return rc;
}

/*
����˵��:
    ��ʼ��������(����������ʽ�洢���ݵĶ���).
����˵��:
    Q--�洢��ʼ��������������Կռ��ַ��
    type--ָ������������
    visit--������Ԫ���������ָ��
����ֵ:
    OK--��ʼ���ɹ���
    !OK--��ʼ��ʧ�ܡ�
ע������:
    *Q--��Ϊ�������ʱ��ҪΪNULL;
    visit--����Ϊ�ա�
����:
    He kun
��������:
    2012-11-04
*/
static Status InitQueue_Link(QUEUE_T *Q, queue_type_t type, opt_visit visit)
{
    assert(!*Q);
    Status rc = OK;
    rc = Malloc((void * *) Q, sizeof(**Q));
	if(rc != OK)
	{
		err_ret(LOG_FILE_LINE,"InitStack_Link:Malloc failed.");
		return rc;
	}
    STACK_T tmp_q = *Q;
    rc = RegisterDepdFuncs_Queue(&tmp_q->depdf, type, visit);
    if(rc != OK)
    {
		Free((void * *) Q);
        err_ret(LOG_FILE_LINE,"InitStack_Link:RegisterDepdFuncs_Stack failed.rc=%d",rc);
        return rc;
    }
    rc = tmp_q->depdf.init_link(&tmp_q->attr);
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"InitStack_Link:init_link failed.rc=%d",rc);
        Free((void * *) Q);
        return rc;
    }
    return rc;
}
/*
��������:
    �������������м����������Կռ䡣
����˵��:
    Q--�Ѵ��ڵ����������Կռ䡣
����ֵ:
    ��
����:
    He kun
����:
    2012-11-13
*/
static void DestroyQueue_Link(QUEUE_T *Q)
{
    if(*Q)
    {
        if((*Q)->attr)
        {
            (*Q)->depdf.destroy_link(&(*Q)->attr);
        }
        Free((void * *) Q);
    }
}
/*
��������:
    ����������ѹ������
����˵��:
    Q--�Ѵ��ڵ����������Կռ䡣
    type--���ݵ�ʵ�����͡�
    val--ָ��ʵ���������͵�һ��ָ�롣
        ���ʵ��������C�����ڽ��ķ�ָ�����ͣ�val�洢���������͵ĵ�ַ��
        ���ʵ��������һ��ָ�룬val�洢ָ��ֵ��
        ���ʵ���������������ͣ����Ƚ����ݴ���һ���ṹ����,val�洢�ýṹ���ַ��
    val_size--ʵ���������ʹ�С��
����ֵ:
    OK--�ɹ���
    !OK--ʧ��;
����:
    He kun
����:
    2012-11-13
*/

static Status EnQueue_Link(QUEUE_T Q, v_type_t type, void *data, size_t size)
{
    assert(Q && data);
    Status rc = OK;
    rc = Q->depdf.append_data(Q->attr, type, data, size);
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"EnQueue_Link: append_data failed,rc=%d.",rc);
    }
    return rc;
}
/*
��������:
    ɾ������ͷԪ��
����˵��:
    Q--�Ѵ��ڵ����������Կռ䡣
    type--���ݵ�ʵ�����͡�
    val--ָ��ʵ���������͵�һ��ָ�롣
        ���ʵ��������C�����ڽ��ķ�ָ�����ͣ�val�洢���������͵ĵ�ַ��
        ���ʵ��������һ��ָ�룬val�洢ָ��ֵ��
        ���ʵ���������������ͣ����Ƚ����ݴ���һ���ṹ����,val�洢�ýṹ���ַ��
    val_size--ʵ���������ʹ�С��
����ֵ:
    OK--�ɹ�.
    !OK--ʧ�ܡ�
����:
    He kun
����:
    2012-11-13

*/
static void DeQueue_Link(QUEUE_T Q, v_type_t type, void *data, size_t size)
{
    assert(Q && data);

    Q->depdf.del_first_data(Q->attr,type,data,size);
}

/*
��������:
    ��ȡ������ͷ�ڵ����ݡ�
����˵��:
    Q--�Ѵ��ڵ����������Կռ䡣
    type--
����ֵ:

����:
    He kun
����:
    2012-11-13
*/
static Status GetQueueHead_Link(QUEUE_T Q, v_type_t type, void **val, size_t size)
{
    assert(Q && Q->attr);
    if(StackEmpty_Link(Q))
    {
        err_ret(LOG_NO_FILE_LINE,"empty Queue.");
        return ERR_EMPTY_LIST;
    }
    Q->depdf.get_first_data(Q->attr, type, val, size);
    return OK;
}
/*
��������:
    ����������Ƿ�Ϊ�ա�
����˵��:
    Q--ָ���ѽ��������������Կռ䡣
����ֵ:
    TRUE--�������С�
    FALSE--�ǿ������С�
����:
    He kun
����:
    2012-11-13
*/
static Status QueueEmpty_Link(QUEUE_T Q)
{
    assert(Q && Q->attr);
    return Q->depdf.link_empty(Q->attr);
}

/*
��������:
    ��������е��������ݡ�
����˵��:
    Q--�ѽ��������������Կռ��ָ�롣
����ֵ:
    ��
����:
    He kun
����:
    2012-11-05
*/
static void ClearQueue_Link(QUEUE_T Q)
{
    assert(Q);
    if(QueueEmpty_Link(Q) != TRUE)
    {
        Q->depdf.clear_link(Q->attr);
    }
}

/*
��������:
    ��ȡ������Ԫ��������
����˵��:
    Q--���������Կռ��׵�ַ��
    Length--�洢������Ԫ��������
����ֵ:
    ��
����:
    ����
����:
    2012-11-07
*/

static void QueueLength_Link(QUEUE_T Q, Int32_t *Length)
{
    assert(Q);
    Q->depdf.get_link_length(Q->attr, Length);
}

/*
��������:
    ����������������ݡ�
����˵��:
    Q--�Ѵ��ڵ����������Կռ䡣
    visit--�������������ֵ���ն˺���ָ�롣
����ֵ:
    OK--�ɹ�.
    !OK--ʧ�ܡ�
ע������:
    ��
����:
    He kun
����:
    2012-11-05
*/
static Status QueueTraverse_Link (QUEUE_T Q, queue_visit visit)
{
    assert(Q && Q->attr);
    Status rc = OK;
    rc = Q->depdf.link_traverse(Q->attr, visit);
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"StackTraverse:link_traverse failed.rc=%d",rc);
    }
    return rc;
}


/*
��������:
    ע�������нӿں���
����˵��:
    q_funcs--�Ѵ��ڵĽӿں����洢�ռ��׵�ַ��
    visit--��������ÿ��Ԫ�ؽ��в����ĺ���ָ�롣
����ֵ:
    OK--�ɹ�.
    !OK--ʧ�ܡ�
ע������:
    ��
����:
    He kun
����:
    2012-11-14
*/


void RegisterQueueFuncs_Link(queue_funcs_t * q_funcs, queue_type_t type, queue_visit visit)
{
    assert(type != UNKNOWN_QUEUE && q_funcs);
    q_funcs->destroy_queue = DestroyQueue_Link;
    q_funcs->init_queue = InitQueue_Link;
    q_funcs->clear_queue = ClearQueue_Link;
    q_funcs->queue_empty = QueueEmpty_Link;
    q_funcs->queue_length = QueueLength_Link;
    q_funcs->get_head = GetQueueHead_Link;
    q_funcs->en_queue = EnQueue_Link;
    q_funcs->de_queue = DeQueue_Link;
    if(visit == NULL)
    {
        q_funcs->opt_func.visit = NULL;
#ifdef _DEBUG
        log_msg(LOG_NO_FILE_LINE, "opt_visitδ���壬QueueTraverse�����޷�ʹ�á�");
#endif
        q_funcs->queue_traverse = NULL;
    }
    else
    {
        q_funcs->opt_func.visit = visit;
        q_funcs->queue_traverse = QueueTraverse_Link;
    }
}


void LogoutQueueFuncs_Link(queue_funcs_t * q_funcs)
{
    if(q_funcs == NULL)
    {
        log_msg(LOG_NO_FILE_LINE,"The Queue funcs is NULL.");
    }
    else
    {
        bzero(q_funcs, sizeof(*q_funcs));
    }
}



