#include <string.h>
#include <assert.h>
#include "g_type.h"
#include "sys_lib.h"
#include "err_num.h"
#include "tree.h"

#define TREE_T tree_attr_t

typedef struct binary_tree_node_S
{
    v_data_t *data;
    struct tree_node_S *left_child;
    struct tree_node_S *right_child;
}binary_tree_node_t;

struct TREE_T
{
    binary_tree_node_t *root;
};

/*
功能描述:

参数说明:

返回值:

作者:
    He kun
日期:
    2012-12-04
*/
static Status PreOrderTraverseUnrecursion(TREE_T T, tree_visit visit)
{
    assert(T && T->root);
    Status rc = OK;
    stack_attr_t stk;
    binary_tree_node_t *cur_addr = NULL;
    binary_tree_node_t *node = NULL;
    Stack_funcs_t s_funcs;
    RegisterStackFuncs(&s_funcs, STACK_SIGNAL_LINK_LIST, NULL);
    rc = Malloc((void **)&node, sizeof(*node));
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"malloc node failed. rc=%d.",rc);
        return rc;
    }
    rc = s_funcs.init_stack(&stk);
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"init_stack failed. rc=%d.",rc);
        Free((void * *)&node);
        LogoutStackFuncs(&s_funcs, STACK_SIGNAL_LINK_LIST);
        return rc;
    }

    do
    {
        rc = s_funcs.push(stk, V_POINT, T->root, sizeof(*T->root));
        if(rc != OK)
        {
            err_ret(LOG_FILE_LINE,"push node failed.rc=%d.",rc);
            break;
        }

        while(s_funcs.stack_empty(stk) == FALSE)
        {
            while(s_funcs.get_top(stk,V_POINT, &cur_addr, sizeof(cur_addr)) == OK 
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
            
            rc = s_funcs.pop(stk,V_POINT, node, sizeof(*node));
            if( rc != OK)
            {
                err_ret(LOG_FILE_LINE,"Pop node failed.rc=%d.",rc);
                break;
            }
            if(s_funcs.stack_empty(stk) == FALSE)
            {
                s_funcs.pop(stk, V_CUSTOM, node, sizeof(*node));
                visit(get_vdata(node->data));
                s_funcs.push(stk,V_CUSTOM, node->right_child, sizeof(*node));
            }
        }
    }while(0);
    Free((void * *) &node);
    s_funcs.destroy_stack(&stk);
    LogoutStackFuncs(&s_funcs, STACK_SIGNAL_LINK_LIST);
}







