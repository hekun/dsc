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
static Status GetQueueHead_Link(QUEUE_T Q, v_type_t type, void **val, size_t size);


/*
��������:
    ע�������в���������Ҫ���õ�����������ֺ�����
����˵��:
    depdf--ջ��������������������������ռ��׵�ַ��
    type--ָ����ջ��֯���͡�
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
static Status InitStack_Link(QUEUE_T *Q, queue_type_t type, opt_visit visit)
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

