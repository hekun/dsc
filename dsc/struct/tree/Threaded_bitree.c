#include <string.h>
#include <assert.h>
#include "sys_lib.h"
#include "err_num.h"
#include "tree.h"

#define TREE_T tree_attr_t

typedef enum 
{
    Link,   //表示指针存储地址为子节点。
    Thread  //表示指针存储地址为前驱/后继结点。
} PointTag;

typedef struct thr_binode_S
{
    v_data_t *  data;
    TREE_T      left_child;
    TREE_T      right_child;
    PointTag    LTag;   //left_child指向的地址类型。
    PointTag    RTag;   //right_child指向的地址类型。
}thr_binode_t;


struct TREE_T
{
    thr_binode_t *root; //指向二叉树的根
    thr_binode_t *head; //指向线索二叉树的头节点。
    thr_binode_t *tail; //指向线索二叉树的尾节点。
    Int32_t       len;  //节点总数。
};

/*
功能描述:
    创建线索二叉树节点。
参数说明:
    node--存储新建的线索二叉树节点
    type--数据的实际类型。
    val--指向实际数据类型的一级指针。
        如果实际数据是C语言内建的非指针类型，val存储该数据类型的地址。
        如果实际数据是一级指针，val存储指针值。
        如果实际数据是其他类型，则先将数据存入一个结构体中,val存储该结构体地址。
    size--实际数据类型大小。

返回值:

作者:
    He kun
日期:
    2013-01-05
*/
static Status MakeBiThrNode(thr_binode_t ** node, v_type_t type, void *data, size_t size)
{
    assert(!node);
    Status rc = OK;
    v_data_t *vdata = NULL;
    rc = init_vdata(&vdata, type, data, size);
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"init_vdata failed.rc=%d.",rc);
        return rc;
    }
    rc = Malloc((void * *) node, sizeof(**node));
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"Malloc failed. rc=%d.",rc);
        return rc;
    }
    node->data = vdata;
    node->left_child = node->right_child = NULL;
    node->LTag = node->RTag = Link;
    return rc;
}

/*
功能描述:
    初始化线索二叉树属性空间。
参数说明:
    BiThrTree--存储新建的属性空间。
返回值:
    OK--成功。 !OK--失败。
作者:
    He kun
日期:
    2013-01-07
*/
static Status initTree_BiThr(TREE_T *BiThrTree)
{
    assert(!BiThrTree);
    Status rc = OK;
    rc = Malloc((void * *)BiThrTree, sizeof(**BiThrTree));
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"Malloc failed.rc=%d.",rc);
        return rc;
    }
    (*BiThrTree)->head = (*BiThrTree)->tail = NULL;
    (*BiThrTree)->root = NULL;
    (*BiThrTree)->len = 0;
    return rc;
}


static Status CreateBiTree_Thr(TREE_T BiThrTree, queue_attr_t q_data, queue_funcs_t *q_func)
{
    Status rc = OK;




    return rc;
}

/*
功能描述:
    线索化二叉树。
参数说明:
    root--二叉树根节点。
    prior--指向二叉树根节点的属性空间域指针。
返回值:
    无。
作者:
    He kun
日期:
    2013-01-07
*/
static void InThreading(thr_binode_t *root, thr_binode_t **prior)
{
    if(root)
    {
        InThreading(root->left_child, prior);
        if(!root->left_child)
        {
            root->LTag = Thread;
            root->left_child = *prior;
        }
        if(!(*prior)->right_child)
        {
            (*prior)->RTag = Thread;
            (*prior)->right_child = root;
        }
        *prior = root;
        InThreading(root->right_child,prior);
    }
}


