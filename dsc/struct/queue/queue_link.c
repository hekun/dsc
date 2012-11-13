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
功能描述:
    注册链队列操作函数需要调用的链表操作部分函数。
参数说明:
    depdf--栈操作函数依赖的链表操作函数空间首地址。
    type--指定链队列组织类型。
    visit--显示栈值函数。
返回值:
    OK--注册成功。
    !OK--获取链表函数失败。
作者:
    He kun
开发日期:
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
功能说明:
    初始化链队列(基于链表形式存储数据的队列).
参数说明:
    Q--存储初始化后的链队列属性空间地址。
    type--指定链队列类型
    visit--链队列元素输出函数指针
返回值:
    OK--初始化成功。
    !OK--初始化失败。
注意事项:
    *Q--作为输入参数时，要为NULL;
    visit--可以为空。
作者:
    He kun
开发日期:
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
功能描述:
    销毁整个链队列及链队列属性空间。
参数说明:
    Q--已存在的链队列属性空间。
返回值:
    无
作者:
    He kun
日期:
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
功能描述:
    向链队列中压入数据
参数说明:
    Q--已存在的链队列属性空间。
    type--数据的实际类型。
    val--指向实际数据类型的一级指针。
        如果实际数据是C语言内建的非指针类型，val存储该数据类型的地址。
        如果实际数据是一级指针，val存储指针值。
        如果实际数据是其他类型，则先将数据存入一个结构体中,val存储该结构体地址。
    val_size--实际数据类型大小。
返回值:
    OK--成功；
    !OK--失败;
作者:
    He kun
日期:
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
功能描述:
    删除队列头元素
参数说明:
    Q--已存在的链队列属性空间。
    type--数据的实际类型。
    val--指向实际数据类型的一级指针。
        如果实际数据是C语言内建的非指针类型，val存储该数据类型的地址。
        如果实际数据是一级指针，val存储指针值。
        如果实际数据是其他类型，则先将数据存入一个结构体中,val存储该结构体地址。
    val_size--实际数据类型大小。
返回值:
    OK--成功.
    !OK--失败。
作者:
    He kun
日期:
    2012-11-13

*/
static void DeQueue_Link(QUEUE_T Q, v_type_t type, void *data, size_t size)
{
    assert(Q && data);

    Q->depdf.del_first_data(Q->attr,type,data,size);
}

/*
功能描述:
    获取链队列头节点数据。
参数说明:
    Q--已存在的链队列属性空间。
    type--
返回值:

作者:
    He kun
日期:
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
功能描述:
    检测链队列是否为空。
参数说明:
    Q--指向已建立的链队列属性空间。
返回值:
    TRUE--空链队列。
    FALSE--非空链队列。
作者:
    He kun
日期:
    2012-11-13
*/
static Status QueueEmpty_Link(QUEUE_T Q)
{
    assert(Q && Q->attr);
    return Q->depdf.link_empty(Q->attr);
}

/*
功能描述:
    清除链队列的所有数据。
参数说明:
    Q--已建立的链队列属性空间的指针。
返回值:
    无
作者:
    He kun
日期:
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
功能描述:
    获取链队列元素总数。
参数说明:
    Q--链队列属性空间首地址。
    Length--存储链队列元素总数。
返回值:
    无
作者:
    何昆
日期:
    2012-11-07
*/

static void QueueLength_Link(QUEUE_T Q, Int32_t *Length)
{
    assert(Q);
    Q->depdf.get_link_length(Q->attr, Length);
}

/*
功能描述:
    输出链队列所有数据。
参数说明:
    Q--已存在的链队列属性空间。
    visit--输出链队列数据值到终端函数指针。
返回值:
    OK--成功.
    !OK--失败。
注意事项:
    无
作者:
    He kun
日期:
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
功能描述:
    注册链队列接口函数
参数说明:
    q_funcs--已存在的接口函数存储空间首地址。
    visit--对链队列每个元素进行操作的函数指针。
返回值:
    OK--成功.
    !OK--失败。
注意事项:
    无
作者:
    He kun
日期:
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
        log_msg(LOG_NO_FILE_LINE, "opt_visit未定义，QueueTraverse函数无法使用。");
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



