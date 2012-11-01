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
    空节点设置: MakeNode_Sig(&node,V_UNKNOWN_TYPE, NULL, 0);
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
功能描述:
    释放节点数据
参数说明:

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
    1.创建存储循环链表属性的空间，并初始化。
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
功能描述:
    判断单链表是否是空表。
参数说明:
    sig_attr--已存在的单链表属性结点。
    pos--指定用于读取数据的结点地址。
    index--数据存储空间索引。
    elem--要保存的结点地址。
返回值:
    FALSE--非空单链表
    TRUE--只包含属性结点的空链表。
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
功能描述:
    将单链表重置为空表，并释放原链表结点空间
参数说明:
    sig_attr--单链表属性空间地址。
返回值:
    OK--清空单链表成功。
    !OK--清空单链表失败。
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
功能描述:销毁链表的属性空间及链表包含的所有节点。
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
功能描述:
    向单链表插入一个头结点值.
参数说明:
    sig_attr--单链表属性空间地址。
    val--指向实际数据的起始地址。
    type--实际数据类型.
    val_size--实际数据长度
返回值:
    OK--插入头结点成功。
    !OK--插入头结点失败。
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
    1.如果属性空间为空，则更新属性空间的所有成员。
    */
    if(ListEmpty_Sig(sig_attr))
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
    对每个结点调用visit()函数。显示整个单链表内容。
参数说明:
    sig_attr--已存在的单链表属性结点。
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


