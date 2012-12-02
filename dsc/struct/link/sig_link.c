#include <string.h>
#include <assert.h>
#include "g_type.h"
#include "sys_lib.h"
#include "err_num.h"
#include "link.h"


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
static Status   LinkEmpty_Sig(LINK_T sig_attr);
static void     DestroyLink_Sig(LINK_T *link);
static Status   MakeNode_Val_Sig(sig_node_t * * p, v_type_t type, void * val, size_t size);
static Status   MakeNode_Vdata_Sig(sig_node_t **p, v_data_t *vdata);
static void     FreeNode_Sig(sig_node_t * *p);
static Status   InsertFirstVal_Sig(LINK_T sig_attr, v_type_t type, void * val, size_t size);
static Status   InsertFirstVdata_Sig(LINK_T sig_attr, v_data_t *vdata);
static Status   LinkTraverse_Sig(LINK_T sig_attr, opt_visit visit);
static void     DelFirstVal_Sig(LINK_T sig_attr, v_type_t type, void * val, size_t size);
static Status   DelFirstVdata_Sig(LINK_T sig_attr, v_data_t **vdata);
static void     GetFirstVal_Sig(LINK_T sig_attr, v_type_t type, void **val, size_t size);
static void GetFirstVdata_Sig(LINK_T sig_attr, v_data_t **vdata);
static Status   AppendVal_Sig(LINK_T sig_attr, v_type_t type, void *val, size_t size);


static void     GetLinkLength_Sig(LINK_T sig_attr, Int32_t *len);
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
    �սڵ�����: MakeNode_Val_Sig(&node,V_UNKNOWN_TYPE, NULL, 0);
*/
static Status MakeNode_Val_Sig(sig_node_t **p, v_type_t type, void * val, size_t size)
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
    Ϊ�ѽ����ĳ����������ʹ����½ڵ㡣
����˵��:
    p--�洢�½��ɹ�������ڵ㡣
    vdata--�����������͡�
����ֵ:
    OK--�����ɹ�!
    !OK--����ʧ��!
����:
    He kun
����:
    2012-11-29
*/

static Status MakeNode_Vdata_Sig(sig_node_t **p, v_data_t *vdata)
{
    assert(!*p && vdata);
    Status rc = OK;
    sig_node_t * node = NULL;
    rc = Malloc((void * *) &node, sizeof(sig_node_t));
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"Malloc failed.rc=%d.",rc);
        return rc;
    }
    node->data = vdata;
    *p = node;
    return rc;

}


/*
��������:
    �ͷŽڵ�����
����˵��:
    p--Ҫ�ͷŵĽڵ�Ķ���ָ�롣
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
    sig_attr--�Ѵ��ڵ��������Խ�㡣
����ֵ:
    FALSE--�ǿ�����
    TRUE--ֻ�������Խ��Ŀ�����
*/
static Status LinkEmpty_Sig(LINK_T sig_attr)
{
    if((sig_attr->head == NULL) || (sig_attr->len == 0) || (sig_attr->tail == NULL))
    {
        return TRUE;
    }
    return FALSE;
}

/*
��������:
    ����������Ϊ�ձ����ͷ�ԭ������ռ�
����˵��:
    sig_attr--�������Կռ��ַ��
����ֵ:
    OK--�������ɹ���
    !OK--�������ʧ�ܡ�
*/
static void ClearList_Sig(LINK_T sig_attr)
{
    if(!LinkEmpty_Sig(sig_attr))
    {
	    sig_node_t *cur_node = sig_attr->head;
	    sig_node_t *prior_node = cur_node;
	    while(cur_node)
	    {
	        cur_node = cur_node->next;
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
    log_msg(LOG_NO_FILE_LINE, "FREE Link");
#endif
}

/*
��������:
    ���������һ��ͷ���ֵ.
����˵��:
    sig_attr--�������Կռ��ַ��
    val--ָ��ʵ�����ݵ���ʼ��ַ��
    type--ʵ����������.
    val_size--ʵ�����ݳ���
����ֵ:
    OK--����ͷ���ɹ���
    !OK--����ͷ���ʧ�ܡ�
����:
    He kun
����:
    2012-11-29
*/

static Status InsertFirstVal_Sig(LINK_T sig_attr, v_type_t type, void * val, size_t size)
{
    assert(sig_attr);

    Status rc = OK;
    sig_node_t* node = NULL;

    rc = MakeNode_Val_Sig(&node, type, val, size);
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"MakeNode_Val_Sig failed.rc=%d",rc);
        return rc;
    }
    /*
    1.������Կռ�Ϊ�գ���������Կռ�����г�Ա��
    */
    if(LinkEmpty_Sig(sig_attr))
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
    �������������Ͳ��뵽�����С�
����˵��:
    sig_attr--�������Կռ��ַ��
    vdata--�����������Ϳռ��׵�ַ��
����ֵ:
    OK--����ͷ���ɹ���
    !OK--����ͷ���ʧ�ܡ�
����:
    He kun
����:
    2012-11-29
ע������:
    vdataָ���Ѵ洢��ʵ�����ݵĴ洢�ռ�
*/
static Status InsertFirstVdata_Sig(LINK_T sig_attr, v_data_t *vdata)
{
    assert(sig_attr);

    Status rc = OK;
    sig_node_t* node = NULL;
    rc = MakeNode_Vdata_Sig(&node,vdata);
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"MakeNode_Vdata_Sig failed.rc=%d",rc);
        return rc;
    }    
    /*
    1.������Կռ�Ϊ�գ���������Կռ�����г�Ա��
    */
    if(LinkEmpty_Sig(sig_attr))
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
    ��ȡ�����е�ͷ�ڵ����ݡ�
����˵��:
    sig_attr--�������Կռ��׵�ַ��
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
static void GetFirstVal_Sig(LINK_T sig_attr, v_type_t type, void **val, size_t size)
{
    assert(sig_attr && !LinkEmpty_Sig(sig_attr));
    if(sig_attr->head->data->type == type && 
       sig_attr->head->data->val_size == size)
    {
        *val = sig_attr->head->data->val;
    }
}
/*
��������:
    ��ȡ�����нڵ�洢�ĳ����������͵��׵�ַ��
����˵��:
    sig_attr--�������Կռ��׵�ַ��
    vdata--�洢�ĳ����������͵��׵�ַ��
����ֵ:
    ��
����:
    He kun
����:
    2012-12-02
*/
static void GetFirstVdata_Sig(LINK_T sig_attr, v_data_t **vdata)
{
    assert(sig_attr && !LinkEmpty_Sig(sig_attr));
    *vdata = sig_attr->head->data;
}

/*
��������:
    ��ÿ��������visit()��������ʾ�����������ݡ�
����˵��:
    sig_attr--�Ѵ��ڵ��������Խ�㡣
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
	if(LinkEmpty_Sig(sig_attr) == TRUE)
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
        printf("->");
	}
	printf("NULL.\n");
	printf("head = ");
    tmp_val = get_vdata(sig_attr->head->data);
    visit(tmp_val);
    printf(",len = %d,",slLength);
	printf("attr->len=%d,",sig_attr->len);
    printf("tail = ");
    tmp_val = get_vdata(sig_attr->tail->data);
    visit(tmp_val);
    printf(".\n");
    return OK;
}
/*
��������:
    �������жϿ�ͷ�ڵ㣬��ȡͷ�ڵ��е�����ָ�롣
����˵��:
    sig_attr--�������Կռ䡣
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
static void DelFirstVal_Sig(LINK_T sig_attr,v_type_t type, void *val, size_t size)
{
    assert(!LinkEmpty_Sig(sig_attr) && val
        && type == sig_attr->head->data->type
        && size == sig_attr->head->data->val_size);
    sig_node_t *node = sig_attr->head;
    Memcpy(val, node->data->val, size, size);
    if(sig_attr->len == 1)
    {
        sig_attr->head = sig_attr->tail = NULL;
    }
    else
    {
        sig_attr->head = sig_attr->head->next;
    }
    FreeNode_Sig(&node);
    sig_attr->len--;
}

/*
��������:
    ɾ������ͷ�ڵ㣬���ڵ���ֵ�洢���½���vdataָ��Ĵ洢�ռ��С�
����˵��:
    sig_attr--�������Կռ䡣
    vdata--ָ�����½��Ĵ洢�������ݿռ䣬�ÿռ�洢ͷ�ڵ�ʵ�����ݡ�
����ֵ:
    OK--�ɹ�:
    !OK--ʧ�ܡ�
����:
    He kun
����:
    2012-12-02
*/
static Status DelFirstVdata_Sig(LINK_T sig_attr, v_data_t **vdata)
{
    assert(!LinkEmpty_Sig(sig_attr) && !*vdata);
    Status rc = OK;
    sig_node_t * node = sig_attr->head;
    rc = init_vdata(vdata, node->data->type, node->data->val, node->data->val_size);
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"DelFirstVdata_Sig failed.rc=%d.",rc);
        return rc;
    }
    if(sig_attr->len == 1)
    {
        sig_attr->head = sig_attr->tail = NULL;
    }
    else
    {
        sig_attr->head = sig_attr->head->next;
    }
    FreeNode_Sig(&node);
    sig_attr->len--;
    return OK;
}

/*
��������:
    �������жϿ�ͷ�ڵ㣬��ȡͷ�ڵ��еĳ������ݡ�
����˵��:
    sig_attr--�������Կռ䡣
    vdata--��ʼֵΪNULL;
����ֵ:
    ��
����:
    He kun
����:
    2012-11-29
*/
static void DelFirstVdata_Sig(LINK_T sig_attr,v_data_t **vdata)
{
    assert(!LinkEmpty_Sig(sig_attr) && vdata);
    Status rc = OK;
    sig_node_t *node = sig_attr->head;
    rc = init_vdata(vdata, node->data->type, node->data->val, node->data->val_size);
    if(rc != OK)
    {
        return ;
    }
    if(sig_attr->len == 1)
    {
        sig_attr->head = sig_attr->tail = NULL;
    }
    else
    {
        sig_attr->head = sig_attr->head->next;
    }
    FreeNode_Sig(&node);
    sig_attr->len--;    
}


/*
��������:
    ��ȡ����Ԫ�ظ�����
����˵��:
    sig_attr--�������Կռ��׵�ַ��
    len--�����ȡ�
����ֵ:
    ��
����:
    ����
����:
    2012-11-07
*/
static void GetLinkLength_Sig(LINK_T sig_attr, Int32_t *len)
{
    assert(sig_attr);
    *len = sig_attr->len;
}

/*
��������:
    ������׷�ӵ������β��
����˵��:
    sig_attr--�������Կռ䡣
    type--�洢�ڵ��������͡�
    val--�洢���ݵĴ洢���׵�ַ��
    size--�洢���ݿռ��С��    
����ֵ:
    
����:
    He kun
����:
    2012-11-12
*/
static Status AppendVal_Sig(LINK_T sig_attr, v_type_t type, void *val, size_t size)
{
    assert(sig_attr && val);
    Status rc = OK;
    sig_node_t *node = NULL;
    rc = MakeNode_Val_Sig(&node, type, val, size);
    if(rc != OK)
    {
        err_ret(LOG_NO_FILE_LINE,"AppendVal_Sig: MakeNode_Val_Sig failed.rc=%d.",rc);
        return rc;
    }
    if(LinkEmpty_Sig(sig_attr))
    {
        sig_attr->head = sig_attr->tail = node;
    }
    else
    {
        sig_attr->tail->next = node;
        sig_attr->tail = node;
    }
    sig_attr->len++;
    return rc;
}

/*
��������:
    ��vdataָ��ĳ����������͵Ľṹ��׷�ӵ������β��
����˵��:
    sig_attr--�������Կռ䡣
    vdata--Ҫ׷�ӵĳ����������͡�
����ֵ:
    OK--�ɹ�
    !OK--ʧ�ܡ� 
����:
    He kun
����:
    2012-12-02
*/
static Status AppendVdata_Sig(LINK_T sig_attr, v_data_t *vdata)
{
    assert(sig_attr && vdata);
    Status rc = OK;
    sig_node_t *node = NULL;
    rc = MakeNode_Vdata_Sig(&node, vdata);
    if(rc != OK)
    {
        err_ret(LOG_NO_FILE_LINE,"AppendVal_Sig: MakeNode_Val_Sig failed.rc=%d.",rc);
        return rc;
    }
    if(LinkEmpty_Sig(sig_attr))
    {
        sig_attr->head = sig_attr->tail = node;
    }
    else
    {
        sig_attr->tail->next = node;
        sig_attr->tail = node;
    }
    sig_attr->len++;
    return rc;
}



Status RegisterLinkFuncs_Sig(link_funcs_t *funcs,opt_visit visit)
{
    assert(funcs);
    Status rc = OK;
    funcs->init_link = InitLink_Sig;
    funcs->destroy_link = DestroyLink_Sig;
    funcs->clear_link = ClearList_Sig;
    
    funcs->insert_first_data = InsertFirstVal_Sig;
    funcs->del_first_data = DelFirstVal_Sig;
    funcs->get_first_data = GetFirstVal_Sig;
    funcs->get_link_length = GetLinkLength_Sig;
    funcs->append_data  = AppendVal_Sig;
    funcs->link_empty = LinkEmpty_Sig;
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


