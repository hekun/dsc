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
static void     DelFirstVal_Sig(LINK_T sig_attr, v_type_t type, void **val, size_t size);
static Status   DelFirstVdata_Sig(LINK_T sig_attr, v_data_t **vdata);
static void     GetFirstVal_Sig(LINK_T sig_attr, v_type_t type, void **val, size_t size);
static void     GetFirstVdata_Sig(LINK_T sig_attr, v_data_t **vdata);
static Status   AppendVal_Sig(LINK_T sig_attr, v_type_t type, void *val, size_t size);
static Status   AppendVdata_Sig(LINK_T sig_attr, v_data_t *vdata);
static void     GetLinkLength_Sig(LINK_T sig_attr, Int32_t *len);
/*
功能描述:
    创建链表节点
参数说明:
    p--存储新建的节点。输入是要为NULL.
    type--数据的实际类型。
    val--指向实际数据类型的一级指针。
        如果实际数据是C语言内建的非指针类型，val存储该数据类型的地址。
        如果实际数据是一级指针，val存储指针值。
        如果实际数据是其他类型，则先将数据存入一个结构体中,val存储该结构体地址。
    size--实际数据类型大小。
返回值:
    OK--成功.
    !OK--失败.
注意事项:
    空节点设置: MakeNode_Val_Sig(&node,V_UNKNOWN_TYPE, NULL, 0);
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
功能描述:
    为已建立的抽象数据类型创建新节点。
参数说明:
    p--存储新建成功的链表节点。
    vdata--抽象数据类型。
返回值:
    OK--创建成功!
    !OK--创建失败!
作者:
    He kun
日期:
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
功能描述:
    释放节点数据
参数说明:
    p--要释放的节点的二级指针。
返回值:

注意事项:
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
功能说明:
    创建链表属性空间，该属性空间无节点
参数说明:
    link--存储新建链表属性空间。
返回值:
    OK--创建成功。!OK--创建失败
注意事项:
    无。
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
    1.创建存储链表属性的空间，并初始化。
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
功能描述:
    判断链表是否是空表。
参数说明:
    sig_attr--已存在的链表属性结点。
返回值:
    FALSE--非空链表
    TRUE--只包含属性结点的空链表。
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
功能描述:
    将链表重置为空表，并释放原链表结点空间
参数说明:
    sig_attr--链表属性空间地址。
返回值:
    OK--清空链表成功。
    !OK--清空链表失败。
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
功能描述:销毁链表的属性空间及链表包含的所有节点。
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
功能描述:
    向链表插入一个头结点值.
参数说明:
    sig_attr--链表属性空间地址。
    val--指向实际数据的起始地址。
    type--实际数据类型.
    val_size--实际数据长度
返回值:
    OK--插入头结点成功。
    !OK--插入头结点失败。
作者:
    He kun
日期:
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
    1.如果属性空间为空，则更新属性空间的所有成员。
    */
    if(LinkEmpty_Sig(sig_attr))
    {
        sig_attr->head = node;
        sig_attr->tail = node;
    }
    /*
    2.如果属性空间非空，则更新属性空间头结点和结点个数。
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
功能描述:
    将抽象数据类型插入到链表中。
参数说明:
    sig_attr--链表属性空间地址。
    vdata--抽象数据类型空间首地址。
返回值:
    OK--插入头结点成功。
    !OK--插入头结点失败。
作者:
    He kun
日期:
    2012-11-29
注意事项:
    vdata指向已存储了实际数据的存储空间
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
    1.如果属性空间为空，则更新属性空间的所有成员。
    */
    if(LinkEmpty_Sig(sig_attr))
    {
        sig_attr->head = node;
        sig_attr->tail = node;
    }
    /*
    2.如果属性空间非空，则更新属性空间头结点和结点个数。
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
功能描述:
    获取链表中的头节点数据。
参数说明:
    sig_attr--链表属性空间首地址。
    type--链表节点类型。
    val--存储实际数据首地址。
    size--数据存储空间大小。
返回值:
    无。
作者:
    He kun
日期:
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
功能描述:
    获取链表有节点存储的抽象数据类型的首地址。
参数说明:
    sig_attr--链表属性空间首地址。
    vdata--存储的抽象数据类型的首地址。
返回值:
    无
作者:
    He kun
日期:
    2012-12-02
*/
static void GetFirstVdata_Sig(LINK_T sig_attr, v_data_t **vdata)
{
    assert(sig_attr && !LinkEmpty_Sig(sig_attr));
    *vdata = sig_attr->head->data;
}

/*
功能描述:
    对每个结点调用visit()函数。显示整个链表内容。
参数说明:
    sig_attr--已存在的链表属性结点。
    vist--每个结点都执行的函数指针。
返回值:
    无
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
功能描述:
    从链表中断开头节点，获取头节点中的数据指针。
参数说明:
    sig_attr--链表属性空间。
    type--存储节点数据类型。
    val--存储数据存储区首地址。
    size--存储数据空间大小。
返回值:
    无
注意事项:
    type,size用于检测val指向的存储空间是否可以存储节点实际数据。
    val指向的缓冲区不必是malloc分配的。只要能存储数据即可。
    见test_link.c文件的funcs.del_first_val函数调用。
作者:
    He kun
完成日期:
    2012-11-02
*/
static void DelFirstVal_Sig(LINK_T sig_attr,v_type_t type, void **val, size_t size)
{
    assert(!LinkEmpty_Sig(sig_attr) && val
        && type == sig_attr->head->data->type
        && size == sig_attr->head->data->val_size);
    sig_node_t *node = sig_attr->head;
    if(type == V_POINT)
    {
        *val = node->data->val;
    }
    else
    {
        Memcpy(*val, node->data->val, size, size);
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
功能描述:
    删除链表头节点，将节点数值存储到新建的vdata指向的存储空间中。
参数说明:
    sig_attr--链表属性空间。
    vdata--指向函数新建的存储抽象数据空间，该空间存储头节点实际数据。
返回值:
    OK--成功:
    !OK--失败。
作者:
    He kun
日期:
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
功能描述:
    获取链表元素个数。
参数说明:
    sig_attr--链表属性空间首地址。
    len--链表长度。
返回值:
    无
作者:
    何昆
日期:
    2012-11-07
*/
static void GetLinkLength_Sig(LINK_T sig_attr, Int32_t *len)
{
    assert(sig_attr);
    *len = sig_attr->len;
}

/*
功能描述:
    将数据追加到链表结尾。
参数说明:
    sig_attr--链表属性空间。
    type--存储节点数据类型。
    val--存储数据的存储区首地址。
    size--存储数据空间大小。    
返回值:
    
作者:
    He kun
日期:
    2012-11-12
*/
static Status AppendVal_Sig(LINK_T sig_attr, v_type_t type, void *val, size_t size)
{
    assert(sig_attr);
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
功能描述:
    将vdata指向的抽象数据类型的结构体追加到链表结尾。
参数说明:
    sig_attr--链表属性空间。
    vdata--要追加的抽象数据类型。
返回值:
    OK--成功
    !OK--失败。 
作者:
    He kun
日期:
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
    funcs->insert_first_val = InsertFirstVal_Sig;
    funcs->insert_first_vdata = InsertFirstVdata_Sig;
    funcs->del_first_val = DelFirstVal_Sig;
    funcs->del_first_vdata = DelFirstVdata_Sig;
    funcs->get_first_val = GetFirstVal_Sig;
    funcs->get_first_vdata = GetFirstVdata_Sig;
    funcs->get_link_length = GetLinkLength_Sig;
    funcs->append_val  = AppendVal_Sig;
    funcs->append_vdata = AppendVdata_Sig;
    funcs->link_empty = LinkEmpty_Sig;
    if(visit == NULL)
    {
        funcs->opt_func.visit = NULL;
#ifdef _DEBUG
        log_msg(LOG_NO_FILE_LINE, "opt_visit未定义，LinkTraverse函数无法使用。");
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


