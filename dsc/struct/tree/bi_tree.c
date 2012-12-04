#include <string.h>
#include <assert.h>
#include "g_type.h"
#include "sys_lib.h"
#include "err_num.h"
#include "tree.h"

#define TREE_T tree_attr_t
struct TREE_T
{
    v_data_t    *data;
    TREE_T      left_child;
    TREE_T      right_child;
}

/*
功能描述:

参数说明:

返回值:

作者:
    He kun
日期:
    2012-12-04
*/
static Status PreOrderTraverseUnrecursion(TREE_T T)
{
    assert(T);
    Status rc = OK;
    stack_attr_t stk;
    Stack_funcs_t s_funcs;
    RegisterStackFuncs(&s_funcs, STACK_SIGNAL_LINK_LIST, NULL);

    rc = s_funcs.init_stack(&stk);
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"init_stack failed. rc=%d.",rc);
        LogoutStackFuncs(&s_funcs, STACK_SIGNAL_LINK_LIST);
        return rc;
    }
    do
    {
        rc = s_funcs.push_vdata(stk, T->data);
        
        while(s_funcs.get_top_vdata(stk, &T->data) && T->data)
        {
            
        }
    }while(0);
    s_funcs.destroy_stack(&stk);
    LogoutStackFuncs(&s_funcs, STACK_SIGNAL_LINK_LIST);
}







