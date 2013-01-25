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
    PointTag_t  LTag;   //左子树标记.1--left_child指向前驱结点，0--left_child指向左子树
    PointTag_t  RTag;   //右子树标记.1--right_child指向后继结点，0--right_child指向右子树
}thr_binode_t;


struct BI_TREE_T
{
    thr_binode_t * head;  //指向线索二叉树头结点。
    thr_binode_t * tail;  //指向线索二叉树尾节点。
    Int32_t        len;	  //节点个数。
    thr_binode_t * root;  //存储二叉树根节点。
};
/*本文件使用的函数*/
static Status CreateBiNode(thr_binode_t **root, queue_attr_t q_data, queue_funcs_t *q_func);
static Status MakeNode_Threaded(thr_binode_t * * root, v_type_t type, void * data, size_t size);
static void   InOrderThreading(thr_binode_t * cur, thr_binode_t ** prior);
static void   DestroyBiNode(thr_binode_t **root);
static void*  GetBiTreeVal(thr_binode_t *root);
static void   LinkOrderTraverse(BI_TREE_T tree, tree_visit visit);
static void   LinkReverseTraverse(BI_TREE_T tree, tree_visit visit);
static void   PreOrderTraver(BI_TREE_T tree, tree_visit visit);


/*对外支持的函数*/
static Status InitBiTree_Threaded(BI_TREE_T * tree);
static Status CreateBiTree_Thread(BI_TREE_T * tree, queue_attr_t q_data, queue_funcs_t * q_func);
static void   DestroyBiTree_Threaded(BI_TREE_T *tree);
static void   TraverseBitree_Threaded(BI_TREE_T tree, tree_visit visit, visit_order_t order);




/*
功能描述:
    创建线索二叉树属性空间。
参数说明:
    tree--线索二叉树属性空间首地址。
返回值:
    OK--成功。
    !OK--失败。
作者:
    He kun
日期:
    2012-12-27
*/
static Status InitBiTree_Threaded(BI_TREE_T * tree)
{
    assert(!*tree);

    Status rc = OK;
    rc = Malloc((void * *)tree, sizeof(**tree));
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"Malloc failed.rc=%d.",rc);
    }
	(*tree)->len = 0;
	(*tree)->head = (*tree)->tail = (*tree)->root = NULL;
    return rc;
}

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
static Status MakeNode_Threaded(thr_binode_t **root, v_type_t type, void *data, size_t size)
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
    return rc;
}


static void InOrderThreading(thr_binode_t *cur, thr_binode_t **prior)
{
    if(cur)
    {
        InOrderThreading(cur->left_child, prior);
        if(!cur->left_child)
        {
            cur->LTag = Thread;
            cur->left_child = *prior;
        }
        if(*prior && !(*prior)->right_child)
        {
            (*prior)->RTag = Thread;
            (*prior)->right_child = cur;
        }
        *prior = cur;
        InOrderThreading(cur->right_child, prior);
    }
}

/*
功能描述:
    递归先序方式创建二叉树。并非线索二叉树。
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
			rc = MakeNode_Threaded(root, vdata->type, vdata->val, vdata->val_size);
			if (rc != OK)
			{
				err_ret(LOG_FILE_LINE, "MakeNode_Threaded failed. rc=%d.",rc);
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

static Status CreateBiTree_Thread(BI_TREE_T * tree, queue_attr_t q_data, queue_funcs_t *q_func) 
{
    Status rc = OK;
	if(!*tree)
	{
		rc = InitBiTree_Threaded(tree);
		if(rc != OK)
		{
			err_ret(LOG_NO_FILE_LINE,"init binary tree failed. rc=%d.",rc);
			return rc;
		}
	}
	q_func->queue_length(q_data,&(*tree)->len);
	//创建基本的二叉树。
	rc = CreateBiNode(&(*tree)->root, q_data, q_func);
	if(rc != OK)
	{
		err_ret(LOG_FILE_LINE,"CreateBiNode failed. rc=%d.",rc);
		return rc;
	}
	//获取二叉树最左端叶子节点地址，作为线索链表的头结点
	thr_binode_t *prior = (*tree)->root;
	while(prior->left_child)
	{
		prior = prior->left_child;
	}
	(*tree)->head = prior;

	//最左端叶子节点的左子树指针指向NULL.
	//最右端叶子节点的右指针指向NULL.
	//线索二叉树实行空间节点指向最左端和最右端叶子节点。
	prior = NULL;
	InOrderThreading((*tree)->root, &prior);
	//此时prior指向最右端的叶子节点。
	prior->right_child = NULL;
	prior->RTag = Thread;
	(*tree)->tail = prior;
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
    //1.仅有一个节点。

    //2.有多个节点。
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
static void DestroyBiTree_Threaded(BI_TREE_T *tree)
{
	if((*tree)->head)
	{
		DestroyBiNode(&(*tree)->head);
	}
	Free((void **)tree);
}

/*
功能描述:
    基于线索二叉树实现。中序，先序，后序，层序遍历。非递归实现。
参数说明:
    tree--线索二叉树属性空间地址。
    visit--遍历每个节点使用的操作函数指针。
    order--执行遍历次序。
返回值:

作者:
	He kun
日期:
	2013-01-19
*/
static void TraverseBitree_Threaded(BI_TREE_T tree, tree_visit visit, visit_order_t order)
{
	assert(tree && visit && order != UNKNOWN_ORDER);
	switch(order)
	{
		case PRE_ORDER:
			PreOrderTraver(tree, visit);
			break;
		case THREAD_ORDER:
			LinkOrderTraverse(tree, visit);
			break;
        case THREAD_REVERSE:
            LinkReverseTraverse(tree, visit);
            break;
		default: 
			break;
	}
}
/*
功能描述:
    先序遍历线索化二叉树。
参数说明:
    root--二叉树根节点。
    visit--遍历节点操作函数。
返回值:
	无
作者:
    He kun
日期:
    2013-01-19
*/
static void PreOrderTraver(BI_TREE_T tree, tree_visit visit)
{
	thr_binode_t *node = tree->root;

	while(node)
	{
		visit(GetBiTreeVal(node));
		//向左遍历左子树，直到遇到叶子节点。
		while(node->LTag == Link)
		{
			node = node->left_child;
            visit(GetBiTreeVal(node));
		}
		//从叶子节点开始向上遍历，直到遇到遍历的节点存在右子树。
		while(node->RTag == Thread && node->right_child)
		{
			node = node->right_child;
		}
		//以右子树作为根节点重复上述遍历流程。
		node = node->right_child;
	}
}

static void PostOrderTraver(BI_TREE_T tree, tree_visit visit)
{
    
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
	对线索二叉树进行顺序，倒序遍历
参数说明:
	tree--线索二叉树属性空间首地址。
	visit--各节点执行的遍历函数指针。
返回值:
	无
作者:
	He kun
日期:
	2013-01-19
*/
static void LinkOrderTraverse(BI_TREE_T tree, tree_visit visit)
{
    assert(tree && visit);
    thr_binode_t * cur = tree->root;
    while(cur)
    {
        while(cur->LTag == Link)
        {
            cur = cur->left_child;
        }
        visit(GetBiTreeVal(cur));
        while(cur->RTag == Thread && cur->right_child)
        {
            cur = cur->right_child;
            visit(GetBiTreeVal(cur));
        }
        cur = cur->right_child;
    }

}

/*
功能描述:
    倒序遍历线索化二叉树。
参数说明:
	tree--线索二叉树属性空间首地址。
	visit--各节点执行的遍历函数指针。
返回值:
    无
作者:
    He kun
日期:
    2013-01-22
*/
static void LinkReverseTraverse(BI_TREE_T tree, tree_visit visit)
{
    assert(tree && visit);
    thr_binode_t *cur = tree->root;
    while(cur)
    {
        while(cur->RTag == Link)
        {
            cur = cur->right_child;
        }
        visit(GetBiTreeVal(cur));
        while(cur->LTag == Thread && cur->left_child)
        {
            cur = cur->left_child;
            visit(GetBiTreeVal(cur));
        }
        cur = cur->left_child;
    }

    
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
    funcs->init_bitree = InitBiTree_Threaded;
    funcs->create_bitree = CreateBiTree_Thread;
    funcs->destroy_bitree = DestroyBiTree_Threaded;
    if(visit)
    {
        funcs->opt_funcs.visit = visit;
        funcs->traverse_bitree = TraverseBitree_Threaded;
    }
    else
    {
        funcs->opt_funcs.visit = NULL;
        funcs->traverse_bitree = NULL;
    }
    return OK;
    
}
