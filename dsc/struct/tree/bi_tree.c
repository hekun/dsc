#include <string.h>
#include <assert.h>
#include "g_type.h"
#include "sys_lib.h"
#include "err_num.h"
#include "stack.h"
#include "tree.h"

#define TREE_T tree_attr_t

struct TREE_T
{
    v_data_t *data;
    TREE_T left_child;
    TREE_T right_child;
};

static Status CreateTree_Binary(TREE_T *root, queue_attr_t q_data, queue_funcs_t *q_func);
static Status PreOrderUnrecursion_Binary(TREE_T root, tree_visit visit);
static void   DestroyTree_Binary(TREE_T * root);
static void   PreOrderRecursion_Binary(TREE_T root, tree_visit visit);
static void   MidOrderRecusion_Binary(TREE_T root, tree_visit visit);
static void   PostOrderRecusion_Binary(TREE_T root, tree_visit visit);

static void * GetTreeVal(TREE_T root);

    


/*
功能描述:
    递归方式先序方式创建二叉树。
参数说明:
    root--二叉树根节点二级指针。
    q_data--存储输入的二叉树各节点的实际数据。
返回值:
    OK--创建节点成功。
    !OK--创建节点失败。
作者:
    He kun
日期:
    2012-12-13
*/

static Status CreateTree_Binary(TREE_T *root, queue_attr_t q_data, queue_funcs_t *q_func)
{
    v_data_t *vdata = NULL;
    Status rc = OK;
    
    if(!*root)
    {
        q_func->de_queue_vdata(q_data, &vdata);
        if( get_vdata(vdata) == NULL)
        {
            *root = NULL;
            destroy_vdata(&vdata);
            return rc;
        }
        else
        {
            do
            {
                rc = Malloc((void * *) root,sizeof(**root));
                if(rc != OK)
                {
                    err_ret(LOG_FILE_LINE,"Malloc failed.rc=%d.",rc);
                    break;
                }
                rc = init_vdata(&(*root)->data, vdata->type, vdata->val, vdata->val_size);
                if(rc != OK)
                {
                    Free((void * *)root);
                    err_ret(LOG_FILE_LINE,"set_vdata failed. rc=%d",rc);
                    break;
                }
            }while(0);
        }
        destroy_vdata(&vdata);
        if(rc != OK)
        {
            log_msg(LOG_NO_FILE_LINE, "Create binary tree node failed!.");
            return rc;
        }
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
    stack_attr_t stk = NULL;
    TREE_T cur_root = NULL;
    Stack_funcs_t s_funcs;
    RegisterStackFuncs(&s_funcs, STACK_SIGNAL_LINK_LIST, NULL);
    rc = s_funcs.init_stack(&stk, STACK_SIGNAL_LINK_LIST, NULL);
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
            while(s_funcs.get_top(stk, V_POINT, (void **)&cur_root, sizeof(cur_root)) == OK 
                && cur_root)
            {
                rc = s_funcs.push(stk,V_POINT, cur_root->left_child, sizeof(cur_root->left_child));
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
            s_funcs.pop(stk, V_POINT, (void **)&cur_root, sizeof(cur_root));
            visit(GetTreeVal(cur_root));
            if(s_funcs.stack_empty(stk) == FALSE)
            {
                s_funcs.pop(stk, V_POINT, (void **)&cur_root, sizeof(cur_root));
                visit(GetTreeVal(cur_root));
                s_funcs.push(stk,V_POINT, cur_root->right_child, sizeof(cur_root));
            }
        }
    }while(0);
    s_funcs.destroy_stack(&stk);
    LogoutStackFuncs(&s_funcs, STACK_SIGNAL_LINK_LIST);
    return rc;
}

/*
功能描述:
    递归方式先序遍历二叉树。
参数说明:
    root--二叉树根节点。
    visit--遍历节点操作函数。
返回值:
    TRUE--该节点为空。
    FALSE--该节点有数据。
作者:
    He kun
日期:
    2012-12-20
*/
static void PreOrderRecursion_Binary(TREE_T root, tree_visit visit)
{
    void * val = NULL;
    val = GetTreeVal(root);
    if(val)
    {
        visit(val);
        PreOrderRecursion_Binary(root->left_child, visit);
        PreOrderRecursion_Binary(root->right_child, visit);
    }

}

/*
功能说明:
    中序遍历递归实现。
参数说明:
    root--二叉树根节点。
    visit--根节点操作函数。
返回值:
    无
作者:
    He kun
日期:
    2012-12-20
*/
static void MidOrderRecusion_Binary(TREE_T root, tree_visit visit)
{
    void * val = NULL;
    val = GetTreeVal(root);

    if(val)
    {
        MidOrderRecusion_Binary(root->left_child, visit);
        visit(val);   
        MidOrderRecusion_Binary(root->right_child, visit);
    }

}


/*
功能描述:
    后续遍历递归实现
参数说明:
    root--二叉树根节点。
    visit--根节点操作函数。
返回值:
    无
作者:
    He kun
日期:
    2012-12-21

*/
static void PostOrderRecusion_Binary(TREE_T root, tree_visit visit)
{
    void * val = NULL;
    val = GetTreeVal(root);
    if(val)
    {
        PostOrderRecusion_Binary(root->left_child, visit);
        PostOrderRecusion_Binary(root->right_child, visit);
        visit(val);
    }
}

/*
功能描述:
    获取结点实际数据块首地址。
参数说明:
    root--根节点。
返回值:
    NULL--根节点地址为空。
    !NULL--实际数据块首地址。
作者:
    He kun
日期:
    2012-12-20

*/
static void * GetTreeVal(TREE_T root)
{
    if(root)
    {
        return get_vdata(root->data);
    }
    else
    {
        return NULL;
    }
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
    if(*root)
    {
        DestroyTree_Binary(&(*root)->left_child);
        DestroyTree_Binary(&(*root)->right_child);
        destroy_vdata(&(*root)->data);
        Free((void * *) root);
    }
}

Status RegisterTreeFuncs_Binary(tree_funcs_t *funcs, tree_visit visit)
{
    assert(funcs);
    Status rc = OK;
    funcs->create_tree = CreateTree_Binary;
    funcs->destroy_tree = DestroyTree_Binary;
    if(visit == NULL)
    {
        funcs->opt_funcs.visit = NULL;
#ifdef _DEBUG
        log_msg(LOG_NO_FILE_LINE, "tree_visit未定义，二叉树遍历函数无法使用。");
#endif
        funcs->preorder_unrecursion = NULL;
        funcs->preorder_recursion = NULL;
        funcs->midorder_recursion = NULL;
        funcs->preorder_recursion = NULL;
        funcs->postorder_recusion = NULL;
    }
    else
    {
        funcs->opt_funcs.visit = visit;
        funcs->preorder_unrecursion = PreOrderUnrecursion_Binary;
        funcs->preorder_recursion = PreOrderRecursion_Binary;
        funcs->midorder_recursion = MidOrderRecusion_Binary;
        funcs->postorder_recusion = PostOrderRecusion_Binary;
    }
    return rc;
}


