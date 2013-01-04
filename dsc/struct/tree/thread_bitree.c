#include <string.h>
#include <assert.h>
#include "sys_lib.h"
#include "err_num.h"
#include "stack.h"
#include "tree.h"


#define TREE_T tree_attr_t
typedef enum
{
    Link,   //ָ��
    Thread  //����
} PointerTag;

typedef struct thr_binode_S
{
    v_data_t    *data;
    TREE_T      left_child;
    TREE_T      right_child;
    PointerTag  LTag;   //���������.1--left_childָ��ǰ����㣬0--left_childָ��������
    PointerTag  RTag;   //���������.1--right_childָ���̽�㣬0--right_childָ��������
}thr_binode_t;


struct TREE_T
{
    thr_binode_t * head;  //ָ������������ͷ��㡣
    thr_binode_t * tail;  //ָ������������β�ڵ㡣
    Int32_t        len;	  //�ڵ������
    thr_binode_t * root;  //�洢���������ڵ㡣
};

static Status InitTree_Threaded(TREE_T * tree);

/*
��������:
    �����������������Կռ䡣
����˵��:
    tree--�������������Կռ��׵�ַ��
����ֵ:
    OK--�ɹ���
    !OK--ʧ�ܡ�
����:
    He kun
����:
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
��������:
    ���������������ڵ㡣
����˵��:
    root--�������ڵ����ָ�롣
    type--���ݵ�ʵ�����͡�
    val--ָ��ʵ���������͵�һ��ָ�롣
        ���ʵ��������C�����ڽ��ķ�ָ�����ͣ�val�洢���������͵ĵ�ַ��
        ���ʵ��������һ��ָ�룬val�洢ָ��ֵ��
        ���ʵ���������������ͣ����Ƚ����ݴ���һ���ṹ����,val�洢�ýṹ���ַ��
    size--ʵ���������ʹ�С��
����ֵ:
    OK--�ɹ���
    !OK--ʧ�ܡ�
����:
    He kun
����:
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


