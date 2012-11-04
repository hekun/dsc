#include <assert.h>
#include <strings.h>
#include "err_num.h"
#include "sys_lib.h"
#include "link.h"
#include "stack.h"

#define STACK_T  stack_attr_t
typedef struct depand_funcs_S
{
    InitLink    init_link;
    DestroyLink destroy_link;
    
}depand_funcs_t;

struct STACK_T
{
    link_attr_t attr;
    depand_funcs_t depdf;
};
static Status RegisterDepdFuncs_Stack(depand_funcs_t * s_depdf, stack_type_t type, opt_visit visit);
static Status InitStack_Link(STACK_T * stack, stack_type_t type, opt_visit visit);
static Status DestroyStack_Link(STACK_T * stack);



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
*/
static Status RegisterDepdFuncs_Stack(depand_funcs_t *s_depdf,stack_type_t type, opt_visit visit)
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

static void DestroyStack_Link(STACK_T *stack)
{
    if(*stack)
    {
        if((*stack)->attr)
        {
            (*stack)->depdf.destroy_link((*stack)->attr);
        }
        Free((void * *) stack);
    }
}

void RegisterStackFuncs_Link(Stack_funcs_t * stk_funcs, stack_type_t type, opt_visit visit)
{
    assert(type != UNKNOWN_STACK);
    stk_funcs->destroy_stack = DestroyStack_Link;
    stk_funcs->init_stack = InitStack_Link;
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



