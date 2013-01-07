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


