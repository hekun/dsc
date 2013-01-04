#include <string.h>
#include <assert.h>
#include "sys_lib.h"
#include "err_num.h"
#include "stack.h"
#include "tree.h"


#define TREE_T tree_attr_t
typedef enum
{
    Link,   //指针
    Thread  //线索
} PointerTag;

typedef struct thr_binode_S
{
    v_data_t    *data;
    TREE_T      left_child;
    TREE_T      right_child;
    PointerTag  LTag;   //左子树标记.1--left_child指向前驱结点，0--left_child指向左子树
    PointerTag  RTag;   //右子树标记.1--right_child指向后继结点，0--right_child指向右子树
}thr_binode_t;


struct TREE_T
{
    thr_binode_t * head;  //指向线索二叉树头结点。
    thr_binode_t * tail;  //指向线索二叉树尾节点。
    Int32_t        len;	  //节点个数。
    thr_binode_t * root;  //存储二叉树根节点。
};

static Status InitTree_Threaded(TREE_T * tree);

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
static Status InitTree_Threaded(TREE_T * tree, v_type_t type, void *data, size_t size)
{
    assert(!*tree);

    Status rc = OK;
    rc = Malloc((void * *)tree, sizeof(**tree));
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"Malloc failed.rc=%d.",rc);
        
    }
    rc = MakeRoot_Threaded(&tree->root, type, data, size);
    if(rc != OK)
    {
        Free((void * *)tree);
        err_ret(LOG_FILE_LINE,"MakeRoot_Threaded failed. rc=%d.",rc);
    }
	if(data == NULL)
	{
		tree->len = 0;
		tree->head = tree->tail = NULL;
	}
	else
	{
		tree->head = tree->tail = tree->root;
		tree->len++;
	}
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
static Status MakeRoot_Threaded(thr_binode_t **root, v_type_t type, void *data, size_t size)
{
    assert(!*root);
    Status rc = OK;
    rc = Malloc((void * *) root, sizeof(**root));
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"Malloc failed.rc=%d.",rc);
        return rc;
    }
    rc = init_vdata(&root->data,type, data, size);
    if(rc != OK)
    {
        Free((void * *)root);
        err_ret(LOG_FILE_LINE,"init_vdata failed. rc=%d.",rc);
        return rc;
    }
    root->left_child = root->right_child = NULL;
    root->LTag = root->RTag = Link;
    return rc;
}


static Status CreateTree_Threaded(TREE_T * tree) 
{
    Status rc = OK;
	

    
    
	return rc;
}

static void InOrderThreading(thr_binode_t *cur, thr_binode_t *prior)
{
    if(cur)
    {
        InOrderThreading(cur->left_child,prior);
        if(!cur->left_child)
        {
            cur->LTag = Thread;
            cur->left_child = prior;
        }
        if(!prior->right_child)
        {
            prior->RTag = Thread;
            prior->right_child = cur;
        }
        prior = cur;
        InOrderThreading(cur->right_child, prior);
    }
}


