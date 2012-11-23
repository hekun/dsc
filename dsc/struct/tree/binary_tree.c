#include <string.h>
#include <assert.h>
#include "g_type.h"
#include "sys_lib.h"
#include "err_num.h"
#include "tree.h"



#define TREE_T tree_attr_t

typedef struct bit_node_S
{
    v_data_t *data;
    struct bit_node_S *left_child;
    struct bit_node_S *right_child;
}bit_node_t;


struct TREE_T
{
    bit_node_t  *root;       //指向根节点
    Int32_t     depth;      //树的深度
    Int32_t     amount;     //节点总数
    visit_order_t order;    //遍历节点顺序
};
static Status InitTree_Binary(TREE_T *T, visit_order_t order);
static Status MakeNode_Binary(bit_node_t **root,void *val, v_type_t type, size_t size);
static Status PreOrderInsert(bit_node_t **root, queue_attr_t q_data, queue_funcs_t * q_funcs,
                             void *val, size_t val_size, v_type_t type);
static Status CreateTree_Binary(TREE_T T, queue_attr_t q_data, queue_funcs_t * q_funcs,
                                void *val, size_t val_size, v_type_t type);
static Status PreOrderVisit_Binary(bit_node_t *root, tree_visit visit);
static void   FreeNode_Binary(bit_node_t **root);
static void   DestroyTree_Binary(TREE_T * T);
static void   TraverseTree_Binary(TREE_T T, tree_visit visit);



static Status InitTree_Binary(TREE_T *T, visit_order_t order)
{
    Status rc = OK;
    if(*T != NULL)
    {
        log_msg(LOG_FILE_LINE, "binaray tree attr exist.");
        return OK;        
    }
    rc = Malloc((void * *)T, sizeof(**T));
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"malloc failed,rc=%d.",rc);
        return rc;        
    }
    TREE_T tmp_t = *T;
    tmp_t->root = NULL;
    tmp_t->depth = 0;
    tmp_t->amount = 0;
    tmp_t->order = order;
#ifdef _DEBUG
    log_msg(LOG_NO_FILE_LINE, "Malloc binaray tree.");
#endif
    return rc; 
}

/*
功能描述:
    先序遍历二叉树，对每个节点执行visit函数。
参数说明:
    root--已经存在的二叉树属性空间。
返回值:
    FALSE--该节点无数据，其双亲节点为叶子节点。
    TRUE--根节点及左右子树遍历完毕。
算法描述:
    从根节点开始，一直向左遍历，对每个遍历的节点都执行visit操作。直至遇到空节点。
    如果，空节点的兄弟节点非空，则从空节点的兄弟节点开始，一路向左，继续遍历。
    否则，返回true。根据递归特性，这样的操作会一直想上延伸，直至二叉树的根节点。
    当整个遍历结束之后，返回TRUE.
作者:
    He kun
日期:
    2012-11-20
*/
static Status PreOrderVisit_Binary(bit_node_t *root, tree_visit visit)
{
    if(root)
    {
        visit(root->data->val);
        PreOrderVisit_Binary(root->left_child,visit);
        PreOrderVisit_Binary(root->right_child,visit);
        return TRUE;
    }
    return FALSE;
}
/*
功能描述:
    遍历二叉树所有结点。
参数说明:
    T--二叉树属性空间首地址。
    order--遍历顺序选项。
    visit--对每个结点执行操作函数。
返回值:

作者:
    He kun
日期:
    2012-11-23
*/

static void TraverseTree_Binary(TREE_T T, tree_visit visit)
{
    assert(T);
    switch(T->order)
    {
        case BIT_PRE_ORDER:
            PreOrderVisit_Binary(T->root, visit);
            break;
        default:
            break;
    }
}

/*
功能描述:
    将队列中的数据插入到二叉树中。
参数说明:
    T--二叉树属性空间首地址。
    q_data--存储数据的队列。
    q_funcs--基于队列操作的函数结构指针。
    val--存储从队列中取出，并插入到二叉树中的数据地址。
    val_size--存储数据的长度。
    type--存储数据的类型。    
返回值:
    OK--成功。
作者:
    He kun
日期:
    2012-11-21
*/
static Status CreateTree_Binary(TREE_T T, queue_attr_t q_data, queue_funcs_t * q_funcs,
                                void *val, size_t val_size, v_type_t type)
{
    assert(T && T->root);
    
    switch(T->order)
    {
        case BIT_PRE_ORDER:
            PreOrderInsert(&T->root,q_data, q_funcs, val, val_size, type);
            break;
        //case BIT_MID_ORDER:

        //    break;
        //case BIT_LAST_PRDER:

        //    break;
        default:
            err_ret(LOG_NO_FILE_LINE,"CreateTree_Binary:visit order error.");
            break;
    }
    return OK;
}

/*
功能描述:
    先序遍历方式插入节点到二叉树中。
参数说明:
    root--二叉树根节点。
    q_data--存储数据的队列。
    q_funcs--基于队列操作的函数结构指针。
    val--存储从队列中取出，并插入到二叉树中的数据地址。
    val_size--存储数据的长度。
    type--存储数据的类型。
返回值:
    OK--插入数据操作完成。
    !OK--插入数据失败。
作者:
    He kun
日期:
    2012-11-22 
*/

static Status PreOrderInsert(bit_node_t **root, queue_attr_t q_data, queue_funcs_t * q_funcs,
                             void *val, size_t val_size, v_type_t type)
{
    Status rc = OK;
    if(q_funcs->queue_empty(q_data) == TRUE)
    {
        log_msg(LOG_NO_FILE_LINE,"PreOrderInsert:queue_empty failed.");
        return rc;
    }
    q_funcs->de_queue(q_data, type, val, val_size);
    if(val==NULL || type == V_UNKNOWN_TYPE || val_size == 0)
    {
        *root = NULL;
    }
    else
    {
        rc = MakeNode_Binary(root, val, type, val_size);
        if(rc != OK)
        {
            log_msg(LOG_NO_FILE_LINE,"PreOrderInsert:MakeNode_Binary failed.");
            return rc;
        }
        PreOrderInsert(&(*root)->left_child, q_data, q_funcs, val, val_size,type);
        PreOrderInsert(&(*root)->right_child, q_data, q_funcs, val, val_size, type);
    }
    return OK;
}

/*
功能描述:
    创建一个二叉树节点。
参数说明:
    root--存储节点首地址。
    val--指向实际数据的起始地址。
    type--实际数据类型.
    val_size--实际数据长度
返回值:
    OK--成功。
    ERROR--失败。
作者:
    He kun
日期:
    2012-11-22
*/

static Status MakeNode_Binary(bit_node_t **root,void *val, v_type_t type, size_t size)
{
    v_data_t *vdata = NULL;
    Status rc = OK;
    rc = Malloc((void * *)root, sizeof(**root));
    if(rc == OK)
    {
        rc = init_vdata(&vdata, type, val, size);
        if(rc != OK)
        {
            Free((void * *)root);
            log_msg(LOG_NO_FILE_LINE, "MakeNode_Binary: init_vdata failed.");
            rc = ERROR;
        }
        else
        {
            (*root)->data = vdata;
        }
    }
    return rc;
}

/*
功能描述:
    销毁根节点及其子树。
参数说明:
    root--根节点二级指针。
返回值:
    无
作者:
    He kun
日期:
    2012-11-23
*/
static void FreeNode_Binary(bit_node_t **root)
{
    if(*root)
    {
        FreeNode_Binary(&(*root)->left_child);
        FreeNode_Binary(&(*root)->right_child);
        destroy_vdata(&(*root)->data);
        Free((void * *) root);
    }
    else
    {
        return;
    }
}
/*
功能描述:
    销毁二叉树
参数说明:
    T--二叉树属性空间二级指针。
返回值:
    无
作者:
    He kun
日期:
    2012-11-23
*/
static void DestroyTree_Binary(TREE_T *T)
{
    if(*T)
    {
        if((*T)->root)
        {
            FreeNode_Binary(&(*T)->root);
        }
        Free((void * *)T);
    }
}



void RegisterTreeFuncs_Binary(Tree_funcs_t * funcs, tree_type_t type, tree_visit visit)
{
    assert(funcs);
    funcs->create_tree = CreateTree_Binary;
    funcs->destroy_tree = DestroyTree_Binary;
    funcs->init_tree = InitTree_Binary;
    if(visit == NULL)
    {
        funcs->opt_funcs.visit = NULL;
#ifdef _DEBUG
        log_msg(LOG_NO_FILE_LINE, "opt_visit未定义，traverse_tree函数无法使用。");
#endif
		funcs->traverse_tree = NULL;
    }
    else
    {
        funcs->opt_funcs.visit = visit;
        funcs->traverse_tree = TraverseTree_Binary;
    }
}

void LogoutTreeFuncs_Binary(Tree_funcs_t *funcs)
{
    if(funcs == NULL)
    {
        log_msg(LOG_NO_FILE_LINE,"LogoutLinkFuncs:Link list class is free.");
    }

    else
    {
        bzero(funcs,sizeof(*funcs));
    }
}


