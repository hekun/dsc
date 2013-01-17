#include <string.h>
#include <assert.h>
#include "sys_lib.h"
#include "err_num.h"
#include "tree.h"

#define TREE_T tree_attr_t

typedef enum 
{
    Link,   //��ʾָ��洢��ַΪ�ӽڵ㡣
    Thread  //��ʾָ��洢��ַΪǰ��/��̽�㡣
} PointTag;

typedef struct thr_binode_S
{
    v_data_t *  data;
    TREE_T      left_child;
    TREE_T      right_child;
    PointTag    LTag;   //left_childָ��ĵ�ַ���͡�
    PointTag    RTag;   //right_childָ��ĵ�ַ���͡�
}thr_binode_t;


struct TREE_T
{
    thr_binode_t *root; //ָ��������ĸ�
    thr_binode_t *head; //ָ��������������ͷ�ڵ㡣
    thr_binode_t *tail; //ָ��������������β�ڵ㡣
    Int32_t       len;  //�ڵ�������
};

static Status MakeBiNode_Thr(thr_binode_t ** node, v_type_t type, void *data, size_t size);



/*
��������:
    ���������������ڵ㡣
����˵��:
    node--�洢�½��������������ڵ�
    type--���ݵ�ʵ�����͡�
    val--ָ��ʵ���������͵�һ��ָ�롣
        ���ʵ��������C�����ڽ��ķ�ָ�����ͣ�val�洢���������͵ĵ�ַ��
        ���ʵ��������һ��ָ�룬val�洢ָ��ֵ��
        ���ʵ���������������ͣ����Ƚ����ݴ���һ���ṹ����,val�洢�ýṹ���ַ��
    size--ʵ���������ʹ�С��

����ֵ:

����:
    He kun
����:
    2013-01-05
*/
static Status MakeBiNode_Thr(thr_binode_t ** node, v_type_t type, void *data, size_t size)
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
��������:
    ��ʼ���������������Կռ䡣
����˵��:
    BiThrTree--�洢�½������Կռ䡣
����ֵ:
    OK--�ɹ��� !OK--ʧ�ܡ�
����:
    He kun
����:
    2013-01-07
*/
static Status initBitree_Thr(TREE_T *BiThrTree)
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


static Status CreateBiTree_Thr(TREE_T BiTreeThr, queue_attr_t q_data, queue_funcs_t *q_func)
{
    Status rc = OK;
    rc = CreateBiNode(&BiTreeThr->root, q_data, q_func);
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"CreateBiNode failed. rc=%d.",rc);
        DestroyBiNode(BiTreeThr->root);
        return rc;
    }
    q_func->queue_length(q_data, &BiTreeThr->len);
    thr_binode_t * prior = NULL;
    while(prior->left_child)
    {
        prior = prior->left_child;
    }
    BiTreeThr->head = prior;
    if(BiTreeThr->root)
    {
        prior = NULL;
        InThreading(BiTreeThr->root, &prior);
        BiTreeThr->tail = prior;
    }
    return rc;
}

/*
��������:
    ��������������
����˵��:
    root--���������ڵ㡣
    prior--ָ����������ڵ�����Կռ���ָ�롣
����ֵ:
    �ޡ�
����:
    He kun
����:
    2013-01-07
*/
static void InThreading(thr_binode_t *root, thr_binode_t **prior)
{
    if(root)
    {
        InThreading(root->left_child, prior);
        if(root && !root->left_child)
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
        InThreading(root->right_child,prior);
    }
}

/*
��������:
    �ݹ鷽ʽ����ʽ������������
����˵��:
    root--���������ڵ����ָ�롣
    q_data--�洢����Ķ��������ڵ��ʵ�����ݡ�
����ֵ:
    OK--�����ڵ�ɹ���
    !OK--�����ڵ�ʧ�ܡ�
����:
    He kun
����:
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
            rc = MakeBiNode_Thr(root,vdata->type, vdata->val, vdata->val_size);
            if(rc != OK)
            {
                err_ret(LOG_FILE_LINE,"MakeBiNode_Thr failed.rc=%d.",rc);
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

/*
��������:
    ���ں���������ʽ���ٶ�������
����˵��:
    root--���������ڵ��ַ��
����ֵ:
    ��
����:
    He kun
����:
    2012-12-14
*/
static void DestroyBiNode(thr_binode_t *root)
{
    if(*root)
    {
        DestroyBiNode(&(*root)->left_child);
        DestroyBiNode(&(*root)->right_child);
        destroy_vdata(&(*root)->data);
        Free((void * *) root);
    }
}


