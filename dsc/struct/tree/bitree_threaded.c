#include <string.h>
#include <assert.h>
#include "sys_lib.h"
#include "err_num.h"
#include "stack.h"
#include "bitree.h"
#include "bitree_threaded.h"


#define BI_TREE_T bitree_attr_t
typedef enum PointTag_E
{
    Link,   //表示指针存储地址为子节点地址。
    Thread  //表示指针存储地址为前驱/后继结点地址。
} PointTag_t;

typedef struct thr_binode_S
{
    v_data_t    *data;
    struct thr_binode_S *left_child;
    struct thr_binode_S *right_child;
    struct thr_binode_S *parent;//指向双亲节点。如果没有双亲节点。指向NULL.仅用于后续线索化中。
    Int32_t     is_visited;  //本节点是否被访问过。true--已经执行过visit函数。false--尚未执行过visit函数
    PointTag_t  LTag;   //左子树标记.1--left_child指向前驱结点，0--left_child指向左子树
    PointTag_t  RTag;   //右子树标记.1--right_child指向后继结点，0--right_child指向右子树
}thr_binode_t;


struct BI_TREE_T
{
    thr_binode_t * root;  //存储二叉树根节点。
    visit_order_t  order; //存储当前遍历顺序。 
    
};

static Status CreateBiNode(thr_binode_t **root, queue_attr_t q_data, queue_funcs_t *q_func);
static Status MakeNode_Thr(thr_binode_t **root, v_type_t type, void *data, size_t size);
static void   InOrderThreading(thr_binode_t *root, thr_binode_t **prior);
static void   PreOrderThreading(thr_binode_t *root, thr_binode_t **prior);
static void   PostOrderThreading(thr_binode_t *root, thr_binode_t **prior);
static void*  GetBiTreeVal(thr_binode_t *root);
static void   InOrderTraverse_Rec(thr_binode_t *root, tree_visit visit);
static void   PreOrderTraverse_Rec(thr_binode_t *root, tree_visit visit);
static void   PostOrderTraverse_Rec(thr_binode_t *root, tree_visit visit);
static void   InOrderTraverse_Thr(thr_binode_t *root,tree_visit visit);
static void   PreOrderTraverse_Thr(thr_binode_t *root, tree_visit visit);
static void   PostOrderTraverse_Thr(thr_binode_t *root, tree_visit visit);
#ifdef _DEBUG
static Status visitnode_tree_stc(void *val);
#endif



static void   UnThreading(thr_binode_t * root);
static void   DestroyBiNode(thr_binode_t **root);


static Status InitBiTree_Thr(BI_TREE_T * tree, visit_order_t order);
static Status CreateBiTree_Thr(BI_TREE_T * tree, visit_order_t order, queue_attr_t q_data, queue_funcs_t * q_func);
static Status ThreadingBitree_Thr(BI_TREE_T tree, visit_order_t order);
static Status BiTreeTraverse_Thr(BI_TREE_T tree, tree_visit visit);
static void   DestroyBiTree_Thr(BI_TREE_T *tree);




/*
功能描述:
    创建线索二叉树节点。
参数说明:
    root--二叉树节点二级指针。
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
static Status MakeNode_Thr(thr_binode_t **root, v_type_t type, void *data, size_t size)
{
    assert(!*root);
    Status rc = OK;
    rc = Malloc((void * *) root, sizeof(**root));
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"Malloc failed.rc=%d.",rc);
        return rc;
    }
    rc = init_vdata(&(*root)->data,type, data, size);
    if(rc != OK)
    {
        Free((void * *)root);
        err_ret(LOG_FILE_LINE,"init_vdata failed. rc=%d.",rc);
        return rc;
    }
    (*root)->left_child = (*root)->right_child = NULL;
    (*root)->LTag = (*root)->RTag = Link;
    (*root)->parent = NULL;
    (*root)->is_visited = 0;
    return rc;
}

/*
功能描述:
    递归先序方式创建二叉树。并非线索二叉树。
参数说明:
    root--二叉树根节点二级指针。
    q_data--存储输入的二叉树各节点的实际数据。
    q_func--队列操作接口函数结构指针
返回值:
    OK--创建节点成功。
    !OK--创建节点失败。
作者:
    He kun
日期:
    2012-12-13
*/
static Status CreateBiNode(thr_binode_t **root, queue_attr_t q_data, queue_funcs_t *q_func)
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
			rc = MakeNode_Thr(root, vdata->type, vdata->val, vdata->val_size);
			if (rc != OK)
			{
				err_ret(LOG_FILE_LINE, "MakeNode_Thr failed. rc=%d.",rc);
			}
        }
        destroy_vdata(&vdata);
        if(rc != OK)
        {
            log_msg(LOG_NO_FILE_LINE, "Create binary tree node failed!.");
            return rc;
        }
    }

    CreateBiNode(&(*root)->left_child, q_data, q_func);
    CreateBiNode(&(*root)->right_child, q_data, q_func);
    return OK;
}

static Status CreateBiTree_Thr(BI_TREE_T * tree, visit_order_t order,queue_attr_t q_data, queue_funcs_t *q_func) 
{
    Status rc = OK;
	if(!*tree)
	{
		rc = InitBiTree_Thr(tree,order);
		if(rc != OK)
		{
			err_ret(LOG_NO_FILE_LINE,"init binary tree failed. rc=%d.",rc);
			return rc;
		}
	}
	//创建基本的二叉树。
	rc = CreateBiNode(&(*tree)->root, q_data, q_func);
	if(rc != OK)
	{
		err_ret(LOG_FILE_LINE,"CreateBiNode failed. rc=%d.",rc);
		return rc;
	}
    //根据order值对二叉树选择不同的线索化方式。
    ThreadingBitree_Thr(*tree, order);
	return rc;
}



/*
功能描述:
    创建线索二叉树属性空间。
参数说明:
    tree--线索二叉树属性空间首地址。
    order--指定对二叉树的线索化方式。
返回值:
    OK--成功。
    !OK--失败。
作者:
    He kun
日期:
    2012-12-27
*/
static Status InitBiTree_Thr(BI_TREE_T * tree, visit_order_t order)
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
    将二叉树中序线索化。
参数说明:
    root--二叉树根节点。
    prior--root线索化之后指向的前驱节点地址。
返回值:
    无
作者:
    He kun
日期:
    2013-01-29
*/
static void InOrderThreading(thr_binode_t *root, thr_binode_t **prior)
{
    if(root)
    {
        InOrderThreading(root->left_child, prior);
        if(!root->left_child)
        {
            root->LTag = Thread;
            root->left_child = *prior;
        }
        if(*prior && !(*prior)->right_child)
        {
            (*prior)->RTag = Thread;
            (*prior)->right_child = root;
        }
        *prior = root;
        InOrderThreading(root->right_child, prior);
    }
}
/*
功能描述:
    将二叉树先序线索化
参数说明:
    root--二叉树根节点
    prior--存储线索化后root指向的前驱节点。初始值是NULL.
返回值:
    无
作者:
    He kun
日期:
    2013-02-01
*/

static void PreOrderThreading(thr_binode_t *root, thr_binode_t **prior)
{
    if(root)
    {
        if(!root->left_child)
        {
            root->LTag = Thread;
            root->left_child = *prior;
        }
        if(*prior && !(*prior)->right_child)
        {
            (*prior)->RTag = Thread;
            (*prior)->right_child = root;
        }
        *prior = root;
        if(root->LTag == Link)
        {
            PreOrderThreading(root->left_child,prior);
        }
        if(root->RTag == Link)
        {
            PreOrderThreading(root->right_child,prior);
        }
        
    }
}

/*
功能描述:
    将二叉树后序线索化
参数说明:
    root--二叉树根节点
    prior--存储线索化后root指向的前驱节点。初始值是NULL.
返回值:
    无
作者:
    He kun
日期:
    2013-02-01
*/
static void PostOrderThreading(thr_binode_t *root, thr_binode_t **prior)
{
    if(root)
    {
        if(root->LTag == Link)
        {
            if(root->left_child)
            {
                root->left_child->parent = root;
            }
            PostOrderThreading(root->left_child, prior);
        }
        if(root->RTag == Link)
        {
            if(root->right_child)
            {
                root->right_child->parent = root;
            }
            PostOrderThreading(root->right_child, prior);
        }
        if(!root->left_child)
        {
            root->LTag = Thread;
            root->left_child = *prior;
        }
        if(*prior && !(*prior)->right_child)
        {
            (*prior)->RTag = Thread;
            (*prior)->right_child = root;
        }
        *prior = root;
    }
}


/*
功能描述:
    清理线索化后的二叉树。使其恢复成线索化之前的二叉树。
参数说明:
    root--二叉树的根节点地址。
返回值:
    无
作者:
    He kun
日期:
    2013-01-29
*/

static void UnThreading(thr_binode_t *root)
{
        if(root->LTag == Thread)
        {
            root->left_child = NULL;
            root->LTag = Link;
        }
        if(root->RTag == Thread)
        {
            root->right_child = NULL;
            root->RTag = Link;
        }
        if(root->parent != NULL)
        {
            root->parent = NULL;
        }
        root->is_visited = FALSE;
        if(root->left_child)
        {
            UnThreading(root->left_child);
        }
        if(root->right_child)
        {
            UnThreading(root->right_child);
        }
}


/*
功能描述:
    线索化二叉树
参数说明:
    tree--线索二叉树属性空间首地址。
返回值:
    OK--成功。
    ERR_PARAMATER--UNKNOWN_ORDER。
作者:
    He kun
日期:
    2013-01-30
*/
static Status ThreadingBitree_Thr(BI_TREE_T tree, visit_order_t order)
{
    assert(tree && tree->root);
    Status rc = OK;
    thr_binode_t *root = tree->root;
    thr_binode_t *prior = NULL;
    UnThreading(root);
    switch(order)
    {
        case IN_THREADED_ORDER: 
            InOrderThreading(root, &prior);
            prior->RTag = Thread;
            prior->right_child = NULL;
            break;
        case PRE_THREADED_ORDER:
            PreOrderThreading(root, &prior);
            prior->RTag = Thread;
            prior->right_child = NULL;
            break;
        case POST_THREADED_ORDER:
            PostOrderThreading(root, &prior);
            break;
        case IN_ORDER:
        case PRE_ORDER:
        case POST_ORDER:
            UnThreading(root);
            break;
        default:
            log_msg(LOG_FILE_LINE, "Unsupported visit order!");
            rc = ERR_PARAMATER;
            break;
    }
    if(rc == OK)
    {
        tree->order = order;
    }
    else
    {
        tree->order = UNKNOWN_BITREE;
    }
    return rc;
    
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
static void * GetBiTreeVal(thr_binode_t *root)
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
    递归实现中序遍历
参数说明:
    root--二叉树根节点地址。
    visit--遍历节点函数指针。
作者:
    He kun
日期:
    2013-01-31
*/
static void InOrderTraverse_Rec(thr_binode_t *root, tree_visit visit)
{
    if(root)
    {
        InOrderTraverse_Rec(root->left_child, visit);        
        visit(GetBiTreeVal(root));
        InOrderTraverse_Rec(root->right_child, visit);
    }
}

/*
功能描述:
    遍历中序线索化二叉树
参数说明:
    root--中序线索化二叉树根节点。
    visit--对每个节点执行的操作。
返回值:
    无
作者:
    He kun
日期:
    2013-01-31
*/
static void InOrderTraverse_Thr(thr_binode_t *root,tree_visit visit)
{
    assert(root && visit);
    thr_binode_t *node = root;
    while(node)
    {
        while(node->LTag == Link)
        {
            node = node->left_child;
        }
        visit(GetBiTreeVal(node));
        while(node->RTag == Thread && node->right_child)
        {
            node = node->right_child;
            visit(GetBiTreeVal(node));
        }
        node = node->right_child;
    }
}



/*
功能描述:
    递归实现先序遍历
参数说明:
    root--二叉树根节点地址。
    visit--遍历节点函数指针。
作者:
    He kun
日期:
    2013-01-31
*/
static void PreOrderTraverse_Rec(thr_binode_t *root, tree_visit visit)
{
    if(root)
    {
        visit(GetBiTreeVal(root));
        PreOrderTraverse_Rec(root->left_child,visit);
        PreOrderTraverse_Rec(root->right_child,visit);
    }
}

static void PreOrderTraverse_Thr(thr_binode_t *root, tree_visit visit)
{
    assert(root && visit);
    thr_binode_t *node = root;
    while(node)
    {
        while(node->LTag == Link)
        {
            visit(GetBiTreeVal(node));
            node = node->left_child;
        }  
        visit(GetBiTreeVal(node));
        if(node->RTag == Thread && node->right_child)
        {
            node = node->right_child;
            visit(GetBiTreeVal(node));
        }
        node = node->right_child;
    }
    
}

/*
功能描述:
    递归实现后序遍历
参数说明:
    root--二叉树根节点地址。
    visit--遍历节点函数指针。
作者:
    He kun
日期:
    2013-01-31
*/
static void PostOrderTraverse_Rec(thr_binode_t*root, tree_visit visit)
{
    if(root)
    {
        PostOrderTraverse_Rec(root->left_child, visit);
        PostOrderTraverse_Rec(root->right_child, visit);
        visit(GetBiTreeVal(root));
    }
}


static void PostOrderTraverse_Thr(thr_binode_t *root, tree_visit visit)
{
    assert(root && visit);
    thr_binode_t * node = root;
    while(node)
    {
        if(node->left_child && node->left_child->is_visited == FALSE)
        {
            node = node->left_child;//如果存在左子树，且尚未遍历，这遍历左子树。
        }
        else if(node->right_child && node->right_child->is_visited == FALSE)
        {
            node = node->right_child;//如果存在右子树，且尚未遍历，这遍历左子树。
        }
        else
        {
            //对于已遍历过左子树和右子树，或者不存在左，右子树的情况。则输出该节点。
            //并指向其双亲节点。
            visit(GetBiTreeVal(node));
            node->is_visited = TRUE;
            node = node->parent;
            break;
        }
        while(1)
        {
            //由于左，右叶子节点遍历优先级均高于他们的双亲节点，
            //因此，需要遍历左右子树的节点，并找到优先级最高的那个结点。
            while(node->LTag == Link)
            {
                node = node->left_child;
            }
            if(node->RTag == Link)
            {
               node = node->right_child;
            }
            else
            {
                break;
            }
        }
        //输出优先级最高的叶子节点。
        visit(GetBiTreeVal(node));
        node->is_visited = TRUE;
        //输出次优先级节点，可以是根节点及右子节点。
        while(node->right_child && node->RTag == Thread)
        {
            node = node->right_child;
            visit(GetBiTreeVal(node));
            node->is_visited = TRUE;
        }
        node = node->parent;
    }
}



static Status BiTreeTraverse_Thr(BI_TREE_T tree, tree_visit visit)
{
    assert(tree && visit);
    Status rc = OK;
    thr_binode_t *root = tree->root;
    
    switch(tree->order)
    {
        case PRE_ORDER:
            PreOrderTraverse_Rec(root, visit);
            break;
        case POST_ORDER:
            PostOrderTraverse_Rec(root, visit);
            break;
        case IN_ORDER:
            InOrderTraverse_Rec(root, visit);
            break;
        case IN_THREADED_ORDER:
            InOrderTraverse_Thr(root, visit);
            break;
        case PRE_THREADED_ORDER:
            PreOrderTraverse_Thr(root, visit);
            break;
        case POST_THREADED_ORDER:
            PostOrderTraverse_Thr(root, visit);
            break;
        default:
            log_msg(LOG_FILE_LINE, "Unsuppored order type!");
            rc = ERR_PARAMATER;
            break;
    }
    return rc;
}


/*
功能描述:
    销毁线索二叉树。
参数说明:
    root--二叉树根节点地址。
返回值:
    无
作者:
    He kun
日期:
    2012-12-14
*/
static void DestroyBiNode(thr_binode_t **root)
{
    thr_binode_t * cur = *root;
    thr_binode_t * prior = *root;
    
    while(cur)
    {
        while(cur->LTag == Link)
        {
            cur = cur->left_child;
        }
        prior = cur;
        while(cur->RTag == Thread && cur->right_child)
        {
            cur = cur->right_child;
            #ifdef _DEBUG
            printf("Free node %c.\n", *(char *)prior->data->val);
            #endif
            destroy_vdata(&prior->data);
            Free((void * *)&prior);
            prior = cur;
        }
        cur = cur->right_child;
        #ifdef _DEBUG
        printf("Free node %c.\n", *(char *)prior->data->val);
        #endif
        destroy_vdata(&prior->data);
        Free((void * *)&prior);
        prior = cur;
    }    
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
static void DestroyBiTree_Thr(BI_TREE_T *tree)
{
    assert(*tree && (*tree)->root);
	thr_binode_t *root = (*tree)->root;
    if((*tree)->order != IN_THREADED_ORDER)
    {
        ThreadingBitree_Thr(*tree, IN_THREADED_ORDER);
        #ifdef _DEBUG
        log_msg(LOG_NO_FILE_LINE,"Before Destroy , inorder threaded visit:");
        BiTreeTraverse_Thr(*tree, visitnode_tree_stc);
        #endif
    }    
    DestroyBiNode(&root);
	Free((void **)tree);
}

/*
功能描述:
    注册线索二叉树操作函数
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
Status RegisterBiTreeFuncs_threaded(bitree_funcs_t *funcs, tree_visit visit)
{
    assert(funcs);
    funcs->init_bitree = InitBiTree_Thr;
    funcs->create_bitree = CreateBiTree_Thr;
    funcs->destroy_bitree = DestroyBiTree_Thr;
    funcs->threading_bitree = ThreadingBitree_Thr;
    if(visit)
    {
        funcs->opt_funcs.visit = visit;
        funcs->traverse_bitree = BiTreeTraverse_Thr;
    }
    else
    {
        funcs->opt_funcs.visit = NULL;
        funcs->traverse_bitree = NULL;
    }
    return OK;
    
}

#ifdef _DEBUG
static Status visitnode_tree_stc(void *val)
{
    if(!val)
    {
        printf("NULL\n");
    }
    else
    {
        printf("%c\n",*(Char8_t *)val);
    }
    
    return OK;

}
#endif


