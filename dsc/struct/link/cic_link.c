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
    空节点设置: MakeNode_Cic(&node,V_UNKNOWN_TYPE, NULL, 0);
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
功能描述:
    释放节点数据
参数说明:
    p--释放节点的二级指针
返回值:

注意事项:
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
功能说明:
    创建链表属性空间，该属性空间无节点
参数说明:
    link--存储新建链表属性空间。
返回值:
    OK--创建成功。!OK--创建失败
注意事项:
    无。
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
    cic_attr--已存在的链表属性结点。
返回值:
    FALSE--非空链表
    TRUE--只包含属性结点的空链表。
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
功能描述:
    将链表重置为空表，并释放原链表结点空间
参数说明:
    cic_attr--链表属性空间地址。
返回值:
    OK--清空链表成功。
    !OK--清空链表失败。
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
功能描述:销毁链表的属性空间及链表包含的所有节点。
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
功能描述:
    向链表插入一个头结点值.
参数说明:
    cic_attr--链表属性空间地址。
    val--指向实际数据的起始地址。
    type--实际数据类型.
    val_size--实际数据长度
返回值:
    OK--插入头结点成功。
    !OK--插入头结点失败。
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
    1.如果属性空间为空，则更新属性空间的所有成员。
    */
    if(LinkEmpty_Cic(cic_attr))
    {
        cic_attr->head = node;
        cic_attr->tail = node;
        node->next = node;	/*链表尾结点指针指向头结点。*/
    }
    /*
    2.如果属性空间非空，则更新属性空间头结点和结点个数。
    */
    else
    {
        node->next = cic_attr->head;
        cic_attr->head = node;
        cic_attr->tail->next = node;/*链表尾结点指针指向头结点。*/
    }
    cic_attr->len++;
    return rc;
}

/*
功能描述:
    从链表中断开头节点，获取头节点中的数据指针。
参数说明:
    cic_attr--链表属性空间。
    type--存储节点数据类型。
    val--存储数据存储区首地址。
    size--存储数据空间大小。
返回值:
    无
注意事项:
    type,size用于检测val指向的存储空间是否可以存储节点实际数据。
    val指向的缓冲区不必是malloc分配的。只要能存储数据即可。
    见test_link.c文件的funcs.del_first_data函数调用。
作者:
    He kun
完成日期:
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
功能描述:
    获取链表中的头节点数据。
参数说明:
    cic_attr--链表属性空间首地址。
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
功能描述:
    获取链表元素个数。
参数说明:
    cic_attr--链表属性空间首地址。
    len--链表长度。
返回值:
    无
作者:
    何昆
日期:
    2012-11-07
*/
static void GetLinkLength_Cic(LINK_T cic_attr, Int32_t *len)
{
    assert(cic_attr);
    *len = cic_attr->len;
}

/*
功能描述:
    将数据追加到链表结尾。
参数说明:
    cic_attr--链表属性空间。
    type--存储节点数据类型。
    val--存储数据的存储区首地址。
    size--存储数据空间大小。    
返回值:
    
作者:
    He kun
日期:
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
功能描述:
    对每个结点调用visit()函数。显示整个链表内容。
参数说明:
    cic_attr--已存在的链表属性结点。
    vist--每个结点都执行的函数指针。
返回值:
    无
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
        log_msg(LOG_NO_FILE_LINE, "opt_visit未定义，LinkTraverse函数无法使用。");
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

