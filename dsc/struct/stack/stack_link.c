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
��������:
    ע��ջ����������Ҫ���õ�����������ֺ�����
����˵��:
    depdf--ջ��������������������������ռ��׵�ַ��
    type--ָ����ջ��֯���͡�
    visit--��ʾջֵ������
����ֵ:
    OK--ע��ɹ���
    !OK--��ȡ������ʧ�ܡ�
����:
    He kun
��������:
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
/*
��������:
    ����������ջ����ջ���Կռ䡣
����˵��:
    stack--�Ѵ��ڵ���ջ���Կռ䡣
����ֵ:
    ��
����:
    He kun
����:
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
��������:
    ����ջ��ѹ������
����˵��:
    stack--�Ѵ��ڵ���ջ���Կռ䡣
    type--���ݵ�ʵ�����͡�
    val--ָ��ʵ���������͵�һ��ָ�롣
        ���ʵ��������C�����ڽ��ķ�ָ�����ͣ�val�洢���������͵ĵ�ַ��
        ���ʵ��������һ��ָ�룬val�洢ָ��ֵ��
        ���ʵ���������������ͣ����Ƚ����ݴ���һ���ṹ����,val�洢�ýṹ���ַ��
    val_size--ʵ���������ʹ�С��
����ֵ:
    OK--�ɹ���
    !OK--ʧ��;
����:
    He kun
����:
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
��������:
    ����������ѹ��ջ�С�
����˵��:
    stack--�Ѵ��ڵ���ջ���Կռ䡣
    vdata--ָ��������ݵ��׵�ַ��
����ֵ:
    OK--�ɹ�;!OK--ʧ��;
����:
    He kun
����:
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
��������:
    ����ջ�е����׸����ݡ�
����˵��:
    stack--�Ѵ��ڵ���ջ���Կռ䡣
    type--�洢�ڵ��������͡�
    val--�洢���ݴ洢���׵�ַ��
    size--�洢���ݿռ��С��
����ֵ:
    ��
ע������:
    type,size���ڼ��valָ��Ĵ洢�ռ��Ƿ���Դ洢ʵ�����ݡ�
    valָ��Ļ�����������malloc����ġ�ֻҪ�ܴ洢���ݼ��ɡ�
    ��test_link.c�ļ���funcs.del_first_val�������á�
����:
    He kun
����:
    2012-11-05
*/
static void Pop_Link(STACK_T stack,v_type_t type, void **val, size_t size)
{
    assert(stack && stack->attr);
    stack->depdf.del_first_val(stack->attr,type,val,size);
}

/*
��������:
    ��ջ��ȡ����������
����˵��:
    stack--�Ѵ��ڵ���ջ���Կռ䡣
    vdata--��ʼֵΪNULL,�ɹ��򷵻غ�ʽ�½��Ĵ洢��ʵ�����ݵĳ��������׵�ַ��
����ֵ:
    OK--�ɹ�
    !OK--ʧ��
����:
    He kun
����:
    2012-12-03
*/

static Status Pop_Vdata_Link(STACK_T stack, v_data_t **vdata)
{
    assert(stack && stack->attr);
    
    return stack->depdf.del_first_vdata(stack->attr, vdata);
}

/*
��������:
    ��ȡ��ջջ�����ݡ�
����˵��:
    stack--�Ѵ��ڵ���ջ���Կռ䡣
    type--�洢�ڵ��������͡�
    val--�洢��ջ�����׵�ַ��
    size--�洢���ݿռ��С��    
����ֵ:
    ERR_EMPTY_LIST--����ջ��
    OK--��ȡջ�����ݳɹ���
ע������:
    �ޡ�
����:
    ����
����:
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
��������:
    ��ȡ��ջջ�����ݡ�
����˵��:
    stack--�Ѵ��ڵ���ջ���Կռ䡣
    vdata--�洢�������ݿռ��׵�ַ��
����ֵ:
    ERR_EMPTY_LIST--����ջ��
    OK--��ȡջ�����ݳɹ���
ע������:
    �ޡ�
����:
    He kun
����:
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
��������:
    �����ջ�������ݡ�
����˵��:
    stack--�Ѵ��ڵ���ջ���Կռ䡣
    visit--�����ջ����ֵ���ն˺���ָ�롣
����ֵ:
    OK--�ɹ�.
    !OK--ʧ�ܡ�
ע������:
    ��
����:
    He kun
����:
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
��������:
    �����ջ�Ƿ�Ϊ�ա�
����˵��:
    stack--ָ���ѽ�������ջ���Կռ䡣
����ֵ:
    TRUE--����ջ��
    FALSE--�ǿ���ջ��
����:
    He kun
����:
    2012-11-05
*/
static Status StackEmpty_Link(STACK_T stack)
{
    assert(stack && stack->attr);
    return stack->depdf.link_empty(stack->attr);
}

/*
��������:
    �����ջ���������ݡ�
����˵��:
    stack--�ѽ�������ջ���Կռ��ָ�롣
����ֵ:
    ��
����:
    He kun
����:
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
��������:
    ��ȡ��ջԪ��������
����˵��:
    stack--��ջ���Կռ��׵�ַ��
    Length--�洢��ջԪ��������
����ֵ:
    ��
����:
    ����
����:
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
        log_msg(LOG_NO_FILE_LINE, "opt_visitδ���壬StackTraverse�����޷�ʹ�á�");
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



