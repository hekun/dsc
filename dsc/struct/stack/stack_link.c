#include <assert.h>
#include <strings.h>
#include "err_num.h"
#include "sys_lib.h"
#include "link.h"
#include "stack.h"

#define STACK_T  stack_attr_t
typedef struct depand_funcs_S
{
    InitLink        init_link;
    DestroyLink     destroy_link;
    InsertFirstVal  insert_first_val;
    InsertFirstVdata insert_first_vdata;
    DelFirstVal     del_first_val;
    DelFirstVdata   del_first_vdata;
    ClearLink       clear_link;
    LinkEmpty       link_empty;
    GetFirstVal     get_first_val;
    GetFirstVdata   get_first_vdata;
    GetLinkLength   get_link_length;
    LinkTraverse    link_traverse;
}stack_depdf_t;

struct STACK_T
{
    link_attr_t attr;
    stack_depdf_t depdf;
};
static Status RegisterDepdFuncs_Stack(stack_depdf_t * s_depdf, stack_type_t type, opt_visit visit);
static Status InitStack_Link(STACK_T * stack, stack_type_t type, opt_visit visit);
static void DestroyStack_Link(STACK_T * stack);
static Status Push_Link(STACK_T stack, v_type_t type, void * val, size_t size);
static Status Push_Vdata_Link(STACK_T stack, v_data_t *vdata);
static Status StackTraverse_Link (STACK_T stack, stack_visit visit);
static void Pop_Link(STACK_T stack, v_type_t type, void **val, size_t size);
static Status Pop_Vdata_Link(STACK_T stack, v_data_t **vdata);
static Status GetTop_Link(STACK_T stack, v_type_t type, void **val, size_t size);
static Status GetTopVdata_Link(STACK_T stack, v_data_t **vdata);
static void ClearStack_Link(STACK_T stack);
static Status StackEmpty_Link(STACK_T stack);
static void StackLength_Link(STACK_T stack, Int32_t *Length);
/*
功能描述:
    注册栈操作函数需要调用的链表操作部分函数。
参数说明:
    depdf--栈操作函数依赖的链表操作函数空间首地址。
    type--指定链栈组织类型。
    visit--显示栈值函数。
返回值:
    OK--注册成功。
    !OK--获取链表函数失败。
作者:
    He kun
开发日期:
    2012-11-04
*/
static Status RegisterDepdFuncs_Stack(stack_depdf_t *s_depdf,stack_type_t type, opt_visit visit)
{
    Status rc = OK;
    link_funcs_t l_func;
    link_opt_funcs_t optf;
    optf.visit = visit;
    switch(type)
    {
        case STACK_CIRCLE_LINK_LIST:
        case STACK_DOUBLE_LINK_LIST:
        case STACK_SIGNAL_LINK_LIST:
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
        s_depdf->insert_first_val = l_func.insert_first_val;
        s_depdf->del_first_val = l_func.del_first_val;
        s_depdf->clear_link = l_func.clear_link;
        s_depdf->link_empty = l_func.link_empty;
        s_depdf->get_first_val = l_func.get_first_val;
        s_depdf->get_link_length = l_func.get_link_length;
        s_depdf->link_traverse = l_func.link_traverse;
        s_depdf->insert_first_vdata = l_func.insert_first_vdata;
        s_depdf->get_first_vdata = l_func.get_first_vdata;
        s_depdf->del_first_vdata = l_func.del_first_vdata;
    }
    return rc;
}


/*
功能说明:
    初始化链栈(基于链表形式存储数据的栈).
参数说明:
    stack--存储初始化后的链栈属性空间地址。
    type--指定链栈类型
    visit--栈元素输出函数指针
返回值:
    OK--初始化成功。
    !OK--初始化失败。
注意事项:
    *stack--作为输入参数时，要为NULL;
    visit--可以为空。
作者:
    He kun
开发日期:
    2012-11-04
*/
static Status InitStack_Link(STACK_T *stack, stack_type_t type, opt_visit visit)
{
    assert(!*stack);
    Status rc = OK;
    rc = Malloc((void * *) stack, sizeof(**stack));
	if(rc != OK)
	{
		err_ret(LOG_FILE_LINE,"InitStack_Link:Malloc failed.");
		return rc;
	}
    STACK_T tmp_stk = *stack;
    rc = RegisterDepdFuncs_Stack(&tmp_stk->depdf, type, visit);
    if(rc != OK)
    {
        Free((void * *) stack);
        err_ret(LOG_FILE_LINE,"InitStack_Link:RegisterDepdFuncs_Stack failed.rc=%d",rc);
        return rc;
    }
    rc = tmp_stk->depdf.init_link(&tmp_stk->attr);
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"InitStack_Link:init_link failed.rc=%d",rc);
        Free((void * *) stack);
        return rc;
    }
    return rc;
}
/*
功能描述:
    销毁整个链栈及链栈属性空间。
参数说明:
    stack--已存在的链栈属性空间。
返回值:
    无
作者:
    He kun
日期:
    2012-11-05
*/
static void DestroyStack_Link(STACK_T *stack)
{
    if(*stack)
    {
        if((*stack)->attr)
        {
            (*stack)->depdf.destroy_link(&(*stack)->attr);
        }
        Free((void * *) stack);
    }
}

/*
功能描述:
    向链栈中压入数据
参数说明:
    stack--已存在的链栈属性空间。
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
    2012-11-05
*/
static Status Push_Link(STACK_T stack, v_type_t type, void *val, size_t size)
{
    assert(stack && stack->attr);
    Status rc = OK;
    rc = stack->depdf.insert_first_val(stack->attr, type, val, size);
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"Push_link:insert_first_val failed. rc=%d.",rc);
    }
    return rc;
}

/*
功能描述:
    将抽象数据压入栈中。
参数说明:
    stack--已存在的链栈属性空间。
    vdata--指向抽象数据的首地址。
返回值:
    OK--成功;!OK--失败;
作者:
    He kun
日期:
    2012-12-03
*/
static Status Push_Vdata_Link(STACK_T stack, v_data_t *vdata)
{
    assert(stack && stack->attr);
    Status rc = OK;

    rc = stack->depdf.insert_first_vdata(stack->attr, vdata);
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"Push_Vdata_link: insert_first_vdata failed. rc=%d.",rc);
    }
    return rc;
}

/*
功能描述:
    从链栈中弹出首个数据。
参数说明:
    stack--已存在的链栈属性空间。
    type--存储节点数据类型。
    val--存储数据存储区首地址。
    size--存储数据空间大小。
返回值:
    无
注意事项:
    type,size用于检测val指向的存储空间是否可以存储实际数据。
    val指向的缓冲区不必是malloc分配的。只要能存储数据即可。
    见test_link.c文件的funcs.del_first_val函数调用。
作者:
    He kun
日期:
    2012-11-05
*/
static void Pop_Link(STACK_T stack,v_type_t type, void **val, size_t size)
{
    assert(stack && stack->attr);
    stack->depdf.del_first_val(stack->attr,type,val,size);
}

/*
功能描述:
    从栈中取出抽象数据
参数说明:
    stack--已存在的链栈属性空间。
    vdata--初始值为NULL,成功则返回韩式新建的存储有实际数据的抽象数据首地址。
返回值:
    OK--成功
    !OK--失败
作者:
    He kun
日期:
    2012-12-03
*/

static Status Pop_Vdata_Link(STACK_T stack, v_data_t **vdata)
{
    assert(stack && stack->attr);
    
    return stack->depdf.del_first_vdata(stack->attr, vdata);
}

/*
功能描述:
    获取链栈栈顶数据。
参数说明:
    stack--已存在的链栈属性空间。
    type--存储节点数据类型。
    val--存储链栈数据首地址。
    size--存储数据空间大小。    
返回值:
    ERR_EMPTY_LIST--空链栈。
    OK--获取栈顶数据成功。
注意事项:
    无。
作者:
    何昆
日期:
    2012-11-05
*/
static Status GetTop_Link(STACK_T stack, v_type_t type, void **val, size_t size)
{
    assert(stack && stack->attr);
    if(StackEmpty_Link(stack))
    {
        err_ret(LOG_NO_FILE_LINE,"empty stack.");
        return ERR_EMPTY_LIST;
    }
    stack->depdf.get_first_val(stack->attr, type, val, size);
    return OK;
}

/*
功能描述:
    获取链栈栈顶数据。
参数说明:
    stack--已存在的链栈属性空间。
    vdata--存储抽象数据空间首地址。
返回值:
    ERR_EMPTY_LIST--空链栈。
    OK--获取栈顶数据成功。
注意事项:
    无。
作者:
    He kun
日期:
    2012-11-05
*/

static Status GetTopVdata_Link(STACK_T stack, v_data_t **vdata)
{
    assert(stack && stack->attr);
    if(StackEmpty_Link(stack))
    {
        err_ret(LOG_NO_FILE_LINE,"empty stack.");
        return ERR_EMPTY_LIST;
    }
    stack->depdf.get_first_vdata(stack->attr, vdata);
    return OK;
}
/*
功能描述:
    输出链栈所有数据。
参数说明:
    stack--已存在的链栈属性空间。
    visit--输出链栈数据值到终端函数指针。
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
static Status StackTraverse_Link (STACK_T stack, stack_visit visit)
{
    assert(stack && stack->attr);
    Status rc = OK;
    rc = stack->depdf.link_traverse(stack->attr, visit);
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"StackTraverse:link_traverse failed.rc=%d",rc);
    }
    return rc;
}
/*
功能描述:
    检测链栈是否为空。
参数说明:
    stack--指向已建立的链栈属性空间。
返回值:
    TRUE--空链栈。
    FALSE--非空链栈。
作者:
    He kun
日期:
    2012-11-05
*/
static Status StackEmpty_Link(STACK_T stack)
{
    assert(stack && stack->attr);
    return stack->depdf.link_empty(stack->attr);
}

/*
功能描述:
    清除链栈的所有数据。
参数说明:
    stack--已建立的链栈属性空间的指针。
返回值:
    无
作者:
    He kun
日期:
    2012-11-05
*/
static void ClearStack_Link(STACK_T stack)
{
    assert(stack);
    if(StackEmpty_Link(stack) != TRUE)
    {
        stack->depdf.clear_link(stack->attr);
    }
}
/*
功能描述:
    获取链栈元素总数。
参数说明:
    stack--链栈属性空间首地址。
    Length--存储链栈元素总数。
返回值:
    无
作者:
    何昆
日期:
    2012-11-07
*/
static void StackLength_Link(STACK_T stack, Int32_t *Length)
{
    assert(stack);
    stack->depdf.get_link_length(stack->attr, Length);
}

void RegisterStackFuncs_Link(Stack_funcs_t * stk_funcs, stack_type_t type, stack_visit visit)
{
    assert(type != UNKNOWN_STACK);
    stk_funcs->destroy_stack = DestroyStack_Link;
    stk_funcs->init_stack = InitStack_Link;
    stk_funcs->pop = Pop_Link;
    stk_funcs->pop_vdata = Pop_Vdata_Link;
    stk_funcs->push = Push_Link;
    stk_funcs->push_vdata = Push_Vdata_Link;
    stk_funcs->clear_stack = ClearStack_Link;
    stk_funcs->stack_empty = StackEmpty_Link;
    stk_funcs->stack_length = StackLength_Link;
    stk_funcs->get_top = GetTop_Link;
    stk_funcs->get_top_vdata = GetTopVdata_Link;
    
    if(visit == NULL)
    {
        stk_funcs->opt_func.visit = NULL;
#ifdef _DEBUG
        log_msg(LOG_NO_FILE_LINE, "opt_visit未定义，StackTraverse函数无法使用。");
#endif
		stk_funcs->stack_traverse = NULL;
    }
    else
    {
        stk_funcs->opt_func.visit = visit;
        stk_funcs->stack_traverse = StackTraverse_Link;
    }
}

void LogoutStackFuncs_Link(Stack_funcs_t * stk_funcs)
{
    if(stk_funcs == NULL)
    {
        log_msg(LOG_NO_FILE_LINE,"The stack funcs is NULL.");
    }
    else
    {
        bzero(stk_funcs, sizeof(*stk_funcs));
    }
}



