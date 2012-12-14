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

static Status CreateTree_Binary(TREE_T *root, queue_attr_t*q_data, queue_funcs_t *q_func)
{
    v_data_t *vdata = NULL;
    Status rc = OK;
    
    if(!root)
    {
        q_func->de_queue_vdata(q_data, &vdata);
        if( get_vdata(vdata) == NULL)
        {
            *root = NULL;
            log_msg(LOG_NO_FILE_LINE, "Create Binary tree NULL node success!");
            return OK;
        }
        else
        {
            rc = Malloc((void * *) root,sizeof(**root));
            if(rc != OK)
            {
                err_ret(LOG_FILE_LINE,"Malloc failed.rc=%d.",rc);
                return rc;
            }
            rc = set_vdata((*root)->data, vdata->type, vdata->val, vdata->val_size);
            if(rc != OK)
            {
                Free((void * *)root);
                err_ret(LOG_FILE_LINE,"set_vdata failed. rc=%d",rc);
                return rc;
            }
        }
        log_msg(LOG_NO_FILE_LINE, "Create binary tree node success!");
    }

    CreateTree_Binary(&(*root)->left_child, q_data, q_func);
    CreateTree_Binary(&(*root)->right_child, q_data, q_func);
    return OK;
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
static Status PreOrderUnrecursion_Binary(TREE_T root, tree_visit visit)
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


/*
功能描述:
    基于后续遍历方式销毁二叉树。
参数说明:
    root--二叉树根节点地址。
返回值:
    无
作者:
    He kun
日期:
    2012-12-14
*/
static void DestroyTree_Binary(TREE_T *root)
{
    if(root)
    {
        DestroyTree_Binary(&(*root)->left_child);
        DestroyTree_Binary(&(*root)->right_child);
        destroy_vdata(&(*root)->data);
        Free((void * *) root);
    }
}




