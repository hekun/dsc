#include <string.h>
#include <assert.h>
#include "g_type.h"
#include "sys_lib.h"
#include "err_num.h"
#include "link.h"

#ifdef _DEBUG_
static Status visitnode(void *val);
#endif
#define LINK_T link_attr_t

typedef struct sig_node_S
{
    v_data_t            *data;
    struct sig_node_S   *next;
}sig_node_t;



struct LINK_T
{
    sig_node_t    *head;
    sig_node_t    *tail;
    Uint32_t      len;
};
static Status   InitLink_Sig(LINK_T *link);
static void     ClearList_Sig(LINK_T sig_attr);
static Status   ListEmpty_Sig(LINK_T sig_attr);
static void     DestroyLink_Sig(LINK_T *link);
static Status   MakeNode_Sig(sig_node_t * * p, v_type_t type, void * val, size_t size);
static void     FreeNode_Sig(sig_node_t * *p);
static Status   InsertFirstData_Sig(LINK_T sig_attr, v_type_t type, void * val, size_t size);
static Status   LinkTraverse_Sig(LINK_T sig_attr, opt_visit visit);

/*
��������:
    ��������ڵ�
����˵��:
    p--�洢�½��Ľڵ㡣������ҪΪNULL.
    type--���ݵ�ʵ�����͡�
    val--ָ��ʵ���������͵�һ��ָ�롣
        ���ʵ��������C�����ڽ��ķ�ָ�����ͣ�val�洢���������͵ĵ�ַ��
        ���ʵ��������һ��ָ�룬val�洢ָ��ֵ��
        ���ʵ���������������ͣ����Ƚ����ݴ���һ���ṹ����,val�洢�ýṹ���ַ��
    size--ʵ���������ʹ�С��
����ֵ:
    OK--�ɹ�.
    !OK--ʧ��.
ע������:
    �սڵ�����: MakeNode_Sig(&node,V_UNKNOWN_TYPE, NULL, 0);
*/
static Status MakeNode_Sig(sig_node_t **p, v_type_t type, void * val, size_t size)
{
    assert(!*p);
    Status rc = OK;
    sig_node_t * node = NULL;
    rc = Malloc((void * *) &node, sizeof(sig_node_t));
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"Malloc failed.rc=%d.",rc);
        return rc;
    }
    rc = init_vdata(&node->data, type, val, size);
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"init_vdata failed. rc=%d.",rc);
        Free((void **)&node);
        return rc;
    }
    *p = node;
    return rc;
}

/*
��������:
    �ͷŽڵ�����
����˵��:

����ֵ:

ע������:
*/
static void FreeNode_Sig(sig_node_t **p)
{
    if(*p)
    {
        if((*p)->data)
        {
            destroy_vdata(&(*p)->data);
        }
        Free((void **)p);
    }
}

/*
����˵��:
    �����������Կռ䣬�����Կռ��޽ڵ�
����˵��:
    link--�洢�½��������Կռ䡣
����ֵ:
    OK--�����ɹ���!OK--����ʧ��
ע������:
    �ޡ�
*/
static Status InitLink_Sig(LINK_T * link)
{
    Status rc = OK;
    if(*link != NULL)
    {
        log_msg(LOG_FILE_LINE, "signal link attr exist.");
        return OK;
    }
    /*
    1.�����洢ѭ���������ԵĿռ䣬����ʼ����
    */
    rc = Malloc((void * *)link,sizeof(**link));
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"malloc failed,rc=%d.",rc);
        return rc;
    }
    LINK_T tmp_attr = *link;
    tmp_attr->head = NULL;
    tmp_attr->tail = NULL;
    tmp_attr->len = 0;
    return rc;    
}

/*
��������:
    �жϵ������Ƿ��ǿձ�
����˵��:
    sig_attr--�Ѵ��ڵĵ��������Խ�㡣
    pos--ָ�����ڶ�ȡ���ݵĽ���ַ��
    index--���ݴ洢�ռ�������
    elem--Ҫ����Ľ���ַ��
����ֵ:
    FALSE--�ǿյ�����
    TRUE--ֻ�������Խ��Ŀ�����
*/
static Status ListEmpty_Sig(LINK_T sig_attr)
{
    if((sig_attr->head == NULL) || (sig_attr->len == 0) || (sig_attr->tail == NULL))
    {
        return TRUE;
    }
    return FALSE;
}

/*
��������:
    ������������Ϊ�ձ����ͷ�ԭ������ռ�
����˵��:
    sig_attr--���������Կռ��ַ��
����ֵ:
    OK--��յ�����ɹ���
    !OK--��յ�����ʧ�ܡ�
*/
static void ClearList_Sig(LINK_T sig_attr)
{
    if(!ListEmpty_Sig(sig_attr))
    {
	    sig_node_t *cur_node = sig_attr->head;
	    sig_node_t *prior_node = cur_node;
	    while(cur_node)
	    {
	        cur_node = cur_node->next;
            destroy_vdata(&prior_node->data);
	        FreeNode_Sig(&prior_node);
	        prior_node = cur_node;
	    }
    }
	sig_attr->head = NULL;
	sig_attr->tail = NULL;
	sig_attr->len = 0;
}


/*
��������:������������Կռ估������������нڵ㡣
*/
static void DestroyLink_Sig(LINK_T *link)
{
    ClearList_Sig(*link);
    Free((void * *)link);
#ifdef _DEBUG
    log_msg(LOG_FILE_LINE, "FREE Link");
#endif
}

/*
��������:
    ���������һ��ͷ���ֵ.
����˵��:
    sig_attr--���������Կռ��ַ��
    val--ָ��ʵ�����ݵ���ʼ��ַ��
    type--ʵ����������.
    val_size--ʵ�����ݳ���
����ֵ:
    OK--����ͷ���ɹ���
    !OK--����ͷ���ʧ�ܡ�
*/

static Status InsertFirstData_Sig(LINK_T sig_attr, v_type_t type, void * val, size_t size)
{
    assert(sig_attr);

    Status rc = OK;
    sig_node_t* node = NULL;

    rc = MakeNode_Sig(&node, type, val, size);
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"MakeNode_Sig failed.rc=%d",rc);
        return rc;
    }
    /*
    1.������Կռ�Ϊ�գ���������Կռ�����г�Ա��
    */
    if(ListEmpty_Sig(sig_attr))
    {
        sig_attr->head = node;
        sig_attr->tail = node;
    }
    /*
    2.������Կռ�ǿգ���������Կռ�ͷ���ͽ�������
    */
    else
    {
        node->next = sig_attr->head;
        sig_attr->head = node;
    }
    sig_attr->len++;
    return rc;
}

/*
��������:
    ��ÿ��������visit()��������ʾ�������������ݡ�
����˵��:
    sig_attr--�Ѵ��ڵĵ��������Խ�㡣
    vist--ÿ����㶼ִ�еĺ���ָ�롣
����ֵ:
    ��
*/
static Status LinkTraverse_Sig(LINK_T sig_attr, opt_visit visit)
{
	assert(sig_attr);
	
	sig_node_t *tmp = sig_attr->head;
    void       *tmp_val = NULL;
	Uint32_t   slLength = 0;
	if(ListEmpty_Sig(sig_attr) == TRUE)
	{
		log_msg(LOG_NO_FILE_LINE, "Signal link empty.");
		return OK;
	}
	while(tmp)
	{
		tmp_val = get_vdata(tmp->data);
        visit(tmp_val);
        slLength++;
		tmp = tmp->next;
	}
	printf(".\n");
	printf("head =");
    tmp_val = get_vdata(sig_attr->head->data);
    visit(tmp_val);
    printf(",len = %d,",slLength);
	printf("attr->len=%d,",sig_attr->len);
    printf("tail =");
    tmp_val = get_vdata(sig_attr->tail->data);
    visit(tmp_val);
    printf(".\n");
    return OK;
}



Status RegisterLinkFuncs_Sig(link_funcs_t *funcs,opt_visit visit)
{
    assert(funcs);
    Status rc = OK;
    funcs->init_link = InitLink_Sig;
    funcs->destroy_link = DestroyLink_Sig;
    funcs->clear_link = ClearList_Sig;
    
    funcs->insert_first_data = InsertFirstData_Sig;
    funcs->link_empty = ListEmpty_Sig;
    if(visit == NULL)
    {
        funcs->opt_func.visit = NULL;
		#ifdef _DEBUG
        log_msg(LOG_NO_FILE_LINE, "opt_visitδ���壬LinkTraverse�����޷�ʹ�á�");
		#endif
		funcs->link_traverse = NULL;
    }
    else
    {
        funcs->opt_func.visit = visit;
        funcs->link_traverse = LinkTraverse_Sig;
    }
    return rc;
}


