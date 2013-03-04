/*
本文件实现二叉树的创建，遍历的非递归实现。
*/
#include <string.h>
#include <assert.h>
#include "sys_lib.h"
#include "err_num.h"
#include "stack.h"
#include "bitree.h"
#include "bitree_unrec.h"

#define BI_TREE_T bitree_attr_t


typedef struct binode_S
{
    v_data_t    *data;
    struct binode_S *left_child;
    struct binode_S *right_child;
    struct binode_S *parent;    //指向双亲节点。如果没有双亲节点。指向NULL.
}bit_node_t;

struct BI_TREE_T
{
    bit_node_t       *root;  //存储二叉树根节点。
    visit_order_t  order;  //存储当前遍历顺序。 
    
};

static void   InsertLeftChild_Unrec(bit_node_t *node, bit_node_t *parent);
static void   InsertRightChild_Unrec(bit_node_t *node, bit_node_t *parent);
static Status MakeNode_Unrec(bit_node_t **node, v_type_t type, void *data, size_t size);
static Status InsertBiNodes_Preorder(bit_node_t **node, queue_attr_t q_data, queue_funcs_t *q_func);
static void   DestroyBiNode_Unrec(bit_node_t **root);
static void  *GetBiTreeVal(bit_node_t *root);
static void   PreOrderTraverse_Unrec(bit_node_t *root, tree_visit visit);
static void   InOrderTraverse_Unrec(bit_node_t *root, tree_visit visit);
static void   PostOrderTraverse_Unrec(bit_node_t *root, tree_visit visit);



static Status InitBiTree_Unrec(BI_TREE_T * tree, visit_order_t order);
static Status CreateBiTree_Unrec(BI_TREE_T *tree, visit_order_t order, queue_attr_t q_data, queue_funcs_t *q_func);
static void   DestroyBiTree_Unrec(BI_TREE_T *tree);
static Status BiTreeTraverse_Unrec(BI_TREE_T tree, tree_visit visit);
static Status SetVisitOrder_Unrec(BI_TREE_T tree, visit_order_t order);




/*
功能描述:
    插入左子节点，原左子节点下移一层。
参数说明:
    node--要插入的节点
    parent--要插入左子节点的双亲节点
返回值:
    无
作者:
    He kun
日期:
    2013-02-17
注意事项:
    若插入节点为根节点，则根节点的双亲节点为NULL.
*/
static void InsertLeftChild_Unrec(bit_node_t *node, bit_node_t *parent)
{
    assert(node);
    if(parent)
    {
        node->left_child = parent->left_child;
        if(parent->left_child)
        {
            parent->left_child->parent = node;
        }
        parent->left_child = node;
        
    }
    node->parent = parent;
}

/*
功能描述:
    插入右子节点，原右子节点下移一层。
参数说明:
    node--要插入的节点
    parent--要插入右子节点的双亲节点
返回值:
    无
作者:
    He kun
日期:
    2013-02-18
注意事项:
    若插入节点为根节点，则根节点的双亲节点为NULL.
*/
static void InsertRightChild_Unrec(bit_node_t *node, bit_node_t *parent)
{
    assert(node);
    if(parent)
    {
        node->right_child = parent->right_child;
        if(parent->right_child)
        {
            parent->right_child->parent = node;
        }
        parent->right_child = node;
    }
    node->parent = parent;
}
/*
功能描述:
    创建二叉树节点。
参数说明:
    node--二叉树节点二级指针。
    type--数据的实际类型。
    val--指向实际数据类型的一级指针。
        如果实际数据是C语言内建的非指针类型，val存储该数据类型的地址。
        如果实际数据是一级指针，val存储指针值。
        如果实际数据是其他类型，则先将数据存入一个结构体中,val存储该结构体地址。
    size--实际数据类型大小。
返回值:
    OK--成功。
    !OK--失败。
作者:
    He kun
日期:
    2012-12-27
*/
static Status MakeNode_Unrec(bit_node_t **node, v_type_t type, void *data, size_t size)
{
    assert(!*node);
    Status rc = OK;
    rc = Malloc((void * *) node, sizeof(**node));
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"Malloc failed.rc=%d.",rc);
        return rc;
    }
    rc = init_vdata(&(*node)->data,type, data, size);
    if(rc != OK)
    {
        Free((void * *)node);
        err_ret(LOG_FILE_LINE,"init_vdata failed. rc=%d.",rc);
        return rc;
    }
    (*node)->left_child = (*node)->right_child = NULL;
    (*node)->parent = NULL;
    return rc;
}
/*
功能描述:
    创建二叉树。
参数说明:
    root--二叉树根节点二级指针。
    q_data--存储输入的二叉树各节点的实际数据。
    q_func--队列操作接口函数结构指针    
返回值:
    OK--创建成功。
    !OK--创建子节点失败。
作者:
    He kun
日期:
    2013-02-18
*/

static Status InsertBiNodes_Preorder(bit_node_t **node, queue_attr_t q_data, queue_funcs_t *q_func)
{
    assert(q_func && q_data && !q_func->queue_empty(q_data));
    v_data_t *vdata = NULL;
    Status rc = OK;
    bit_node_t * parent = NULL;
    bit_node_t * child = NULL;
    while( rc == OK && q_func->queue_empty(q_data) == FALSE 
           && (rc = q_func->de_queue_vdata(q_data, &vdata)) == OK)
    {
        //创建根节点和左子树。
        if(get_vdata(vdata) != NULL)
        {
            child = NULL;
            rc = MakeNode_Unrec(&child, vdata->type, vdata->val, vdata->val_size);
            if(rc != OK)
            {
                destroy_vdata(&vdata);
                err_ret(LOG_FILE_LINE,"MakeNode_Unrec failed. rc=%d.",rc);
                break;
            }
            else
            {
                InsertLeftChild_Unrec(child,parent);
                #ifdef _DEBUG
                if(parent != NULL)
                {
                    log_msg(LOG_NO_FILE_LINE, "Create left child %c.And it`s parent is %c.",
                        *(char *)child->data->val, *(char *)child->parent->data->val);
                }
                else
                {
                    log_msg(LOG_NO_FILE_LINE,"Create root node %c.",*(char *)child->data->val);
                }
                #endif
                parent = child;
                if(*node == NULL)
                {
                   *node = parent;
                }
                destroy_vdata(&vdata);
            }
        }
        else
        {
            destroy_vdata(&vdata);  //清除空的左子结点.

            while((q_func->queue_empty(q_data) == FALSE)
                  && (rc = q_func->de_queue_vdata(q_data, &vdata)) == OK)
            {
                if(get_vdata(vdata) == NULL)
                {
                    //若不存在右子节点，则一直向上遍历。
                    destroy_vdata(&vdata);
                    parent = parent->parent;
                }
                else
                {
                    //若存在节点值，则将其指定为parent的右子节点。
                    while(parent && parent->right_child)
                    {
                        parent = parent->parent;
                    }
                    if(!parent)
                    {
                        err_ret(LOG_NO_FILE_LINE,"full bitree.");
                        break;
                    }
                    child = NULL;
                    rc = MakeNode_Unrec(&child, vdata->type, vdata->val, vdata->val_size);
                    if(rc != OK)
                    {
                        destroy_vdata(&vdata);
                        err_ret(LOG_FILE_LINE,"MakeNode_Unrec failed. rc=%d.",rc);
                        break;
                    }
                    else
                    {
                        InsertRightChild_Unrec(child,parent);
                        #ifdef _DEBUG
                        if(parent != NULL)
                        {
                            log_msg(LOG_NO_FILE_LINE, "Create right child %c.And it`s parent is %c.",
                                *(char *)child->data->val, *(char *)child->parent->data->val);
                        }
                        else
                        {
                            log_msg(LOG_NO_FILE_LINE,"Create root node %c.",*(char *)child->data->val);
                        }
                        #endif
                        parent = child;
                        destroy_vdata(&vdata);
                        break;
                    }
                }
            }
        }
    }
    return rc;
}

/*
功能描述:
    创建二叉树属性空间。
参数说明:
    tree--二叉树属性空间首地址。
    order--指定对二叉树的遍历方式。
返回值:
    OK--成功。
    !OK--失败。
作者:
    He kun
日期:
    2012-12-27
*/
static Status InitBiTree_Unrec(BI_TREE_T * tree, visit_order_t order)
{
    assert(!*tree);

    Status rc = OK;
    rc = Malloc((void * *)tree, sizeof(**tree));
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"Malloc failed.rc=%d.",rc);
    }
	(*tree)->root = NULL;
    (*tree)->order = order;
    return rc;
}

/*
功能描述:
    创建二叉树
参数说明:
    tree--指向二叉树属性空间的二级指针。
    order--指定创建二叉树的创建顺序，默认为先序方式创建。
    q_data--存储输入的二叉树各节点的实际数据。
    q_func--队列操作接口函数结构指针
返回值:
    OK--成功。
    ERR_PARAMATER--order参数值不支持。
作者:
    He kun
日期:
    2013-02-21
*/
static Status CreateBiTree_Unrec(BI_TREE_T *tree, visit_order_t order, queue_attr_t q_data, queue_funcs_t *q_func) 
{
    assert(tree);
    Status rc = OK;
    if(!*tree)
    {
        rc = InitBiTree_Unrec(tree, order);
        if(rc != OK)
		{
			err_ret(LOG_NO_FILE_LINE,"init binary tree failed. rc=%d.",rc);
			return rc;
		}
    }
    switch(order)
    {
        case PRE_ORDER:
            rc = InsertBiNodes_Preorder(&(*tree)->root,q_data,q_func);
            if(rc != OK)
            {
                err_ret(LOG_FILE_LINE,"PreCreateBiNode_Unrec failed. rc=%d",rc);
            }
            break;
        default: 
            err_ret(LOG_FILE_LINE,"Unsupported create node order.");
            rc = ERR_PARAMATER;
            break;
    }
    return rc;
}




/*
功能描述:
    使用后续遍历销毁二叉树。
参数说明:
    root--二叉树根节点地址。
返回值:
    无
作者:
    He kun
日期:
    2013-02-21
*/
static void DestroyBiNode_Unrec(bit_node_t **root)
{
    bit_node_t * cur = *root;
    bit_node_t * prior = *root;
    while(cur)
    {
        
        while(1)
        {
            while(cur->left_child)
            {
                cur = cur->left_child;
            }
            if(cur->right_child)
            {
                cur = cur->right_child;
            }
            else
            {
                break;
            }
        }
        prior = cur;
        cur = cur->parent;
        if(cur)
        {
            if(cur->left_child == prior)
            {
                cur->left_child = NULL;
            }
            if(cur->right_child == prior)
            {
                cur->right_child = NULL;
            }
            //销毁左/右子节点。
#ifdef _DEBUG
            printf("Free node %c.\n", *(char *)prior->data->val);
#endif
            destroy_vdata(&prior->data);
            Free((void * *)&prior);  
        }
    }  
    //销毁二叉树根节点。
#ifdef _DEBUG
    printf("Free node %c.\n", *(char *)prior->data->val);
#endif
    destroy_vdata(&prior->data);
    Free((void * *)&prior);    
}

/*
功能描述:
	销毁线索二叉树。
参数说明:
	tree--存储二叉树树形控件的指针。
返回值:
	无
作者:
	He kun
日期:
	2013-01-19
*/
static void DestroyBiTree_Unrec(BI_TREE_T *tree)
{
    assert(*tree && (*tree)->root);
	bit_node_t *root = (*tree)->root;
    DestroyBiNode_Unrec(&root);
	Free((void **)tree);
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
static void * GetBiTreeVal(bit_node_t *root)
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
    非递归实现先序遍历
参数说明:
    root--二叉树根节点地址。
    visit--遍历节点函数指针。
作者:
    He kun
日期:
    2013-01-31
*/
static void PreOrderTraverse_Unrec(bit_node_t *root, tree_visit visit)
{
    assert(root && visit);
    bit_node_t *cur = root;
    bit_node_t *prior = root;
    while(cur)
    {
        while(cur->left_child && cur->left_child != prior)
        {
            //遍历双亲节点.
            visit(GetBiTreeVal(cur));
            prior = cur;
            cur = cur->left_child;
        }
        //遍历左子节点
        visit(GetBiTreeVal(cur));
        while(cur && (!cur->right_child || cur->right_child == prior))
        {
            prior = cur;
            cur = cur->parent;
        }
        if(cur)
        {
            cur = cur->right_child;
        }
    }
}

/*
功能描述:
    非递归实现中序遍历
参数说明:
    root--二叉树根节点地址。
    visit--遍历节点函数指针。
作者:
    He kun
日期:
    2013-02-27
*/
static void InOrderTraverse_Unrec(bit_node_t *root, tree_visit visit)
{
    assert(root && visit);
    bit_node_t *cur = root;
    bit_node_t *prior = root;
    while(cur)
    {
        while(cur->left_child && cur->left_child != prior)
        {
            prior = cur;
            cur = cur->left_child;
        }
        //输出左子节点
        visit(GetBiTreeVal(cur));
        while(cur && (!cur->right_child || cur->right_child == prior))
        {
            prior = cur;
            cur = cur->parent;
            //向上遍历双亲节点。
            if(cur && cur->right_child != prior)
            {
                visit(GetBiTreeVal(cur));
            }
        }
        if(cur)
        {
            cur = cur->right_child;
        }
    }
}

/*
功能描述:
    非递归实现后续遍历。
参数说明:
    root--二叉树根节点地址。
    visit--遍历节点函数指针。
返回值:
    无
作者:
   He kun
日期:
    2013-03-01
*/
static void PostOrderTraverse_Unrec(bit_node_t *root, tree_visit visit)
{
    assert(root && visit);
    bit_node_t * cur =  root;
    bit_node_t * prior = root;
    while(cur)
    {
        
        while(1)
        {
            while(cur->left_child && cur->left_child != prior)
            {
                cur = cur->left_child;
            }
            if(cur->right_child && cur->right_child != prior)
            {
                cur = cur->right_child;
            }
            else
            {
                break;
            }
        }
        //使prior指向子节点，cur指向其双亲节点
        prior = cur;
        cur = cur->parent;
        if(cur)
        {
            //visit子节点
            visit(GetBiTreeVal(prior));
            //对于cur指向的双亲节点而言，若其不存在右子节点，或右子节点
            //已经遍历过，则cur继续向上遍历。prior紧随其后。便于程序开始出
            //查找未遍历过的子节点。
            while( cur && (!cur->right_child || cur->right_child == prior) )
            {
                prior = cur;
                visit(GetBiTreeVal(prior));
                cur = cur->parent;
            }
        }
    }  
}



/*
功能描述:
    二叉树遍历接口函数
参数说明:
    root--二叉树根节点地址。
    visit--遍历节点函数指针。
返回值:
    ERR_PARAMATER--参数错误
    OK--成功
作者:
    He kun
日期:
    2013-02-16
*/
static Status BiTreeTraverse_Unrec(BI_TREE_T tree, tree_visit visit)
{
    assert(tree && visit);
    Status rc = OK;
    bit_node_t *root = tree->root;
    
    switch(tree->order)
    {
        case PRE_ORDER:
            PreOrderTraverse_Unrec(root, visit);
            break;
        case IN_ORDER:
            InOrderTraverse_Unrec(root, visit);
            break;
        case POST_ORDER:
            PostOrderTraverse_Unrec(root, visit);
            break;
        default:
            log_msg(LOG_FILE_LINE, "Unsuppored visit order!");
            rc = ERR_PARAMATER;
            break;
    }
    return rc;
}

static Status SetVisitOrder_Unrec(BI_TREE_T tree, visit_order_t order)
{
    assert(tree);
    Status rc = OK;
    switch(order)
    {
        case PRE_ORDER:
        case IN_ORDER:
        case POST_ORDER:
            tree->order = order;
            break;
        default:
            tree->order = UNKNOWN_ORDER;
            rc = ERR_PARAMATER;
            break;
    }
    return rc;
}

/*
功能描述:
    注册二叉树操作函数
参数说明:
    funcs--存储注册函数存储空间
    visit--遍历每个节点的操作函数。
返回值:
    无
作者:
    He kun
日期:
    2013-01-22
*/
Status RegisterBiTreeFuncs_Unrec(bitree_funcs_t *funcs, tree_visit visit)
{
    assert(funcs);
    funcs->init_bitree = InitBiTree_Unrec;
    funcs->create_bitree = CreateBiTree_Unrec;
    funcs->destroy_bitree = DestroyBiTree_Unrec;
    funcs->set_visit_order = SetVisitOrder_Unrec;
    funcs->threading_bitree = NULL;
    if(visit)
    {
        funcs->opt_funcs.visit = visit;
        funcs->traverse_bitree = BiTreeTraverse_Unrec;
    }
    else
    {
        funcs->opt_funcs.visit = NULL;
        funcs->traverse_bitree = NULL;
    }
    return OK;
    
}


