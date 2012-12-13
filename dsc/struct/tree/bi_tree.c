#include <string.h>
#include <assert.h>
#include "g_type.h"
#include "sys_lib.h"
#include "err_num.h"
#include "tree.h"

#define TREE_T tree_attr_t

struct TREE_T
{
    v_data_t *data;
    TREE_T left_child;
    TREE_T right_child;
};


/*
功能描述:
    递归方式创建二叉树。
参数说明:
    
返回值:
    
作者:
    He kun
日期:
    2012-12-13
*/

static Status CreateBiTree(TREE_T root, queue_attr_t*data, queue_funcs_t *q_func)
{
    if(!root)
    {
        q_func->de_queue();
    }
}

/*
功能描述:
    非递归方式先序遍历二叉树
参数说明:
    root--二叉树根节点
返回值:

作者:
    He kun
日期:
    2012-12-04
*/
static Status PreOrderTraverseUnrecursion(TREE_T root, tree_visit visit)
{
    assert(root);
    Status rc = OK;
    stack_attr_t stk;
    TREE_T cur_addr = NULL;
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
        rc = s_funcs.push(stk, V_POINT, root, sizeof(root));
        if(rc != OK)
        {
            err_ret(LOG_FILE_LINE,"push node failed.rc=%d.",rc);
            break;
        }

        while(s_funcs.stack_empty(stk) == FALSE)
        {
            while(s_funcs.get_top(stk,V_POINT, (void **)&cur_addr, sizeof(cur_addr)) == OK 
                && cur_addr)
            {
                rc = s_funcs.push(stk,V_POINT, cur_addr->left_child, sizeof(cur_addr->left_child));
                if(rc != OK)
                {
                    err_ret(LOG_FILE_LINE,"push node failed.rc=%d.",rc);
                    break;
                }              
            }
            if(rc != OK)
            {
                break;
            }
            
            rc = s_funcs.pop(stk,V_POINT, (void **)&cur_addr, sizeof(cur_addr));
            if( rc != OK)
            {
                err_ret(LOG_FILE_LINE,"Pop node failed.rc=%d.",rc);
                break;
            }
            if(s_funcs.stack_empty(stk) == FALSE)
            {
                s_funcs.pop(stk, V_POINT, (void **)&cur_addr, sizeof(cur_addr));
                visit(get_vdata(cur_addr->data));
                s_funcs.push(stk,V_POINT, cur_addr->right_child, sizeof(cur_addr));
            }
        }
    }while(0);
    s_funcs.destroy_stack(&stk);
    LogoutStackFuncs(&s_funcs, STACK_SIGNAL_LINK_LIST);
}







