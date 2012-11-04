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
��������:
    ע��ջ����������Ҫ���õ�����������ֺ�����
����˵��:
    depdf--ջ��������������������������ռ��׵�ַ��
    type--ָ����ջ��֯���͡�
    visit--��ʾջֵ������
����ֵ:
    OK--ע��ɹ���
    !OK--��ȡ������ʧ�ܡ�
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
����˵��:
    ��ʼ����ջ(����������ʽ�洢���ݵ�ջ).
����˵��:
    stack--�洢��ʼ�������ջ���Կռ��ַ��
    type--ָ����ջ����
    visit--ջԪ���������ָ��
����ֵ:
    OK--��ʼ���ɹ���
    !OK--��ʼ��ʧ�ܡ�
ע������:
    *stack--��Ϊ�������ʱ��ҪΪNULL;
    visit--����Ϊ�ա�
����:
    He kun
��������:
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



