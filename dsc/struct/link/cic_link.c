#include <string.h>
#include <assert.h>
#include "g_type.h"
#include "sys_lib.h"
#include "err_num.h"
#include "link.h"

#define LINK_T link_attr_t


typedef struct cic_node_S
{
    v_data_t            *data;
    struct cic_node_S   *next;
}cic_node_t;

struct LINK_T
{
    cic_node_t    *head;
    cic_node_t    *tail;
    Uint32_t      len;
};
static Status   InitLink_Cic(LINK_T *link);
static void     ClearList_Cic(LINK_T cic_attr);
static Status   LinkEmpty_Cic(LINK_T cic_attr);
static void     DestroyLink_Cic(LINK_T *link);
static Status   MakeNode_Cic(cic_node_t * * p, v_type_t type, void * val, size_t size);
static void     FreeNode_Cic(cic_node_t * *p);
static Status   InsertFirstData_Cic(LINK_T cic_attr, v_type_t type, void * val, size_t size);
static Status   LinkTraverse_Cic(LINK_T cic_attr, opt_visit visit);
static void     DelFirstData_Cic(LINK_T cic_attr, v_type_t type, void * val, size_t size);
static void     GetFirstData_Cic(LINK_T cic_attr, v_type_t type, void **val, size_t size);
static void     GetLinkLength_Cic(LINK_T cic_attr, Int32_t *len);
static Status   AppendData_Cic(LINK_T cic_attr, v_type_t type, void *val, size_t size);

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
    �սڵ�����: MakeNode_Cic(&node,V_UNKNOWN_TYPE, NULL, 0);
*/
static Status MakeNode_Cic(cic_node_t **p, v_type_t type, void * val, size_t size)
{
    assert(!*p);
    Status rc = OK;
    cic_node_t * node = NULL;
    rc = Malloc((void * *) &node, sizeof(cic_node_t));
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
    p--�ͷŽڵ�Ķ���ָ��
����ֵ:

ע������:
*/
static void FreeNode_Cic(cic_node_t **p)
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
static Status InitLink_Cic(LINK_T * link)
{
    Status rc = OK;
    if(*link != NULL)
    {
        log_msg(LOG_FILE_LINE, "signal link attr exist.");
        return OK;
    }
    /*
    1.�����洢�������ԵĿռ䣬����ʼ����
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
#ifdef _DEBUG
    log_msg(LOG_NO_FILE_LINE, "Malloc Link");
#endif
    return rc;    
}

/*
��������:
    �ж������Ƿ��ǿձ�
����˵��:
    cic_attr--�Ѵ��ڵ��������Խ�㡣
����ֵ:
    FALSE--�ǿ�����
    TRUE--ֻ�������Խ��Ŀ�����
*/
static Status LinkEmpty_Cic(LINK_T cic_attr)
{
    if((cic_attr->head == NULL) || (cic_attr->len == 0) || (cic_attr->tail == NULL))
    {
        return TRUE;
    }
    return FALSE;
}

/*
��������:
    ����������Ϊ�ձ����ͷ�ԭ������ռ�
����˵��:
    cic_attr--�������Կռ��ַ��
����ֵ:
    OK--�������ɹ���
    !OK--�������ʧ�ܡ�
*/
static void ClearList_Cic(LINK_T cic_attr)
{
    if(!LinkEmpty_Cic(cic_attr))
    {
	    cic_node_t *cur_node = cic_attr->head;
	    cic_node_t *prior_node = cur_node;
	    while((cur_node->next != cic_attr->head)  && (cic_attr->len--))
	    {
	        cur_node = cur_node->next;
            destroy_vdata(&prior_node->data);
	        FreeNode_Cic(&prior_node);
	        prior_node = cur_node;
	    }
        destroy_vdata(&cur_node->data);
        FreeNode_Cic(&cur_node);
        cic_attr->len--;
    }
	cic_attr->head = NULL;
	cic_attr->tail = NULL;
	cic_attr->len =0;
}

/*
��������:������������Կռ估������������нڵ㡣
*/
static void DestroyLink_Cic(LINK_T *link)
{
    ClearList_Cic(*link);
    Free((void * *)link);
#ifdef _DEBUG
    log_msg(LOG_NO_FILE_LINE, "FREE Link");
#endif
}

/*
��������:
    ���������һ��ͷ���ֵ.
����˵��:
    cic_attr--�������Կռ��ַ��
    val--ָ��ʵ�����ݵ���ʼ��ַ��
    type--ʵ����������.
    val_size--ʵ�����ݳ���
����ֵ:
    OK--����ͷ���ɹ���
    !OK--����ͷ���ʧ�ܡ�
*/

static Status InsertFirstData_Cic(LINK_T cic_attr, v_type_t type, void * val, size_t size)
{
    assert(cic_attr);

    Status rc = OK;
    cic_node_t* node = NULL;

    rc = MakeNode_Cic(&node, type, val, size);
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"MakeNode_Cic failed.rc=%d",rc);
        return rc;
    }
    /*
    1.������Կռ�Ϊ�գ���������Կռ�����г�Ա��
    */
    if(LinkEmpty_Cic(cic_attr))
    {
        cic_attr->head = node;
        cic_attr->tail = node;
        node->next = node;	/*����β���ָ��ָ��ͷ��㡣*/
    }
    /*
    2.������Կռ�ǿգ���������Կռ�ͷ���ͽ�������
    */
    else
    {
        node->next = cic_attr->head;
        cic_attr->head = node;
        cic_attr->tail->next = node;/*����β���ָ��ָ��ͷ��㡣*/
    }
    cic_attr->len++;
    return rc;
}

/*
��������:
    �������жϿ�ͷ�ڵ㣬��ȡͷ�ڵ��е�����ָ�롣
����˵��:
    cic_attr--�������Կռ䡣
    type--�洢�ڵ��������͡�
    val--�洢���ݴ洢���׵�ַ��
    size--�洢���ݿռ��С��
����ֵ:
    ��
ע������:
    type,size���ڼ��valָ��Ĵ洢�ռ��Ƿ���Դ洢�ڵ�ʵ�����ݡ�
    valָ��Ļ�����������malloc����ġ�ֻҪ�ܴ洢���ݼ��ɡ�
    ��test_link.c�ļ���funcs.del_first_data�������á�
����:
    He kun
�������:
    2012-11-02
*/
static void DelFirstData_Cic(LINK_T cic_attr,v_type_t type, void *val, size_t size)
{
    assert(!LinkEmpty_Cic(cic_attr) && val
        && type == cic_attr->head->data->type
        && size == cic_attr->head->data->val_size);
    cic_node_t *node = cic_attr->head;
    Memcpy(val, node->data->val, size, size);
    if(cic_attr->len == 1)
    {
        cic_attr->head = cic_attr->tail = NULL;
    }
    else
    {
        cic_attr->head = cic_attr->head->next;
        cic_attr->tail->next = cic_attr->head;
    }
    FreeNode_Cic(&node);
    cic_attr->len--;
}
/*
��������:
    ��ȡ�����е�ͷ�ڵ����ݡ�
����˵��:
    cic_attr--�������Կռ��׵�ַ��
    type--����ڵ����͡�
    val--�洢ʵ�������׵�ַ��
    size--���ݴ洢�ռ��С��
����ֵ:
    �ޡ�
����:
    He kun
����:
    2012-11-05
*/
static void GetFirstData_Cic(LINK_T cic_attr, v_type_t type, void **val, size_t size)
{
    assert(cic_attr && !LinkEmpty_Cic(cic_attr));
    if(cic_attr->head->data->type == type && 
       cic_attr->head->data->val_size == size)
    {
        *val = cic_attr->head->data->val;
    }
}
/*
��������:
    ��ȡ����Ԫ�ظ�����
����˵��:
    cic_attr--�������Կռ��׵�ַ��
    len--�����ȡ�
����ֵ:
    ��
����:
    ����
����:
    2012-11-07
*/
static void GetLinkLength_Cic(LINK_T cic_attr, Int32_t *len)
{
    assert(cic_attr);
    *len = cic_attr->len;
}

/*
��������:
    ������׷�ӵ������β��
����˵��:
    cic_attr--�������Կռ䡣
    type--�洢�ڵ��������͡�
    val--�洢���ݵĴ洢���׵�ַ��
    size--�洢���ݿռ��С��    
����ֵ:
    
����:
    He kun
����:
    2012-11-12
*/
static Status AppendData_Cic(LINK_T cic_attr, v_type_t type, void *val, size_t size)
{
    assert(cic_attr && val);
    Status rc = OK;
    cic_node_t *node = NULL;
    rc = MakeNode_Cic(&node, type, val, size);
    if(rc != OK)
    {
        err_ret(LOG_NO_FILE_LINE,"AppendData_Cic: MakeNode_Cic failed.rc=%d.",rc);
        return rc;
    }
    if(LinkEmpty_Cic(cic_attr))
    {
        cic_attr->head = cic_attr->tail = node;
    }
    else
    {
        cic_attr->tail->next = node;
        node->next = cic_attr->head;
        cic_attr->tail = node;
    }
    cic_attr->len++;
    return rc;
}



/*
��������:
    ��ÿ��������visit()��������ʾ�����������ݡ�
����˵��:
    cic_attr--�Ѵ��ڵ��������Խ�㡣
    vist--ÿ����㶼ִ�еĺ���ָ�롣
����ֵ:
    ��
*/
static Status LinkTraverse_Cic(LINK_T cic_attr, opt_visit visit)
{
	assert(cic_attr);
	
	cic_node_t *tmp = cic_attr->head;
    void       *tmp_val = NULL;
	Uint32_t   slLength = 0;
	if(LinkEmpty_Cic(cic_attr) == TRUE)
	{
		log_msg(LOG_NO_FILE_LINE, "Signal link empty.");
		return OK;
	}
	while(tmp->next != cic_attr->head)
	{
		tmp_val = get_vdata(tmp->data);
        visit(tmp_val);
        slLength++;
		tmp = tmp->next;
        printf("->");
	}
    tmp_val = get_vdata(tmp->data);
    visit(tmp_val);
    slLength++;
	printf(".\n");
	printf("head = ");
    tmp_val = get_vdata(cic_attr->head->data);
    visit(tmp_val);
    printf(",len = %d,",slLength);
	printf("attr->len=%d,",cic_attr->len);
    printf("tail = ");
    tmp_val = get_vdata(cic_attr->tail->data);
    visit(tmp_val);
    printf(", tail next = ");
    tmp_val = get_vdata(cic_attr->tail->next->data);
    visit(tmp_val);
    printf(".\n");
    return OK;
}

Status RegisterLinkFuncs_Cic(link_funcs_t *funcs,opt_visit visit)
{
    assert(funcs);
    Status rc = OK;
    funcs->init_link = InitLink_Cic;
    funcs->destroy_link = DestroyLink_Cic;
    funcs->clear_link = ClearList_Cic;
    funcs->get_first_data = GetFirstData_Cic;
    funcs->insert_first_data = InsertFirstData_Cic;
    funcs->del_first_data = DelFirstData_Cic;
    funcs->get_link_length = GetLinkLength_Cic;
    funcs->append_data = AppendData_Cic;
    funcs->link_empty = LinkEmpty_Cic;
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
        funcs->link_traverse = LinkTraverse_Cic;
    }
    return rc;
}

