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
    Link,   //��ʾָ��洢��ַΪ�ӽڵ��ַ��
    Thread  //��ʾָ��洢��ַΪǰ��/��̽���ַ��
} PointTag_t;

typedef struct thr_binode_S
{
    v_data_t    *data;
    struct thr_binode_S *left_child;
    struct thr_binode_S *right_child;
    PointTag_t  LTag;   //���������.1--left_childָ��ǰ����㣬0--left_childָ��������
    PointTag_t  RTag;   //���������.1--right_childָ���̽�㣬0--right_childָ��������
}thr_binode_t;


struct BI_TREE_T
{
    thr_binode_t * head;  //ָ������������ͷ��㡣
    thr_binode_t * tail;  //ָ������������β�ڵ㡣
    Int32_t        len;	  //�ڵ������
    thr_binode_t * root;  //�洢���������ڵ㡣
};
/*���ļ�ʹ�õĺ���*/
static Status CreateBiNode(thr_binode_t **root, queue_attr_t q_data, queue_funcs_t *q_func);
static Status MakeNode_Threaded(thr_binode_t * * root, v_type_t type, void * data, size_t size);
static void   InOrderThreading(thr_binode_t * cur, thr_binode_t ** prior);
static void   DestroyBiNode(thr_binode_t **root);
static void*  GetBiTreeVal(thr_binode_t *root);
static void   LinkOrderTraverse(BI_TREE_T tree, tree_visit visit);
static void   LinkReverseTraverse(BI_TREE_T tree, tree_visit visit);
static void   PreOrderTraver(BI_TREE_T tree, tree_visit visit);


/*����֧�ֵĺ���*/
static Status InitBiTree_Threaded(BI_TREE_T * tree);
static Status CreateBiTree_Thread(BI_TREE_T * tree, queue_attr_t q_data, queue_funcs_t * q_func);
static void   DestroyBiTree_Threaded(BI_TREE_T *tree);
static void   TraverseBitree_Threaded(BI_TREE_T tree, tree_visit visit, visit_order_t order);




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
��������:
    �ݹ�����ʽ����������������������������
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
	//���������Ķ�������
	rc = CreateBiNode(&(*tree)->root, q_data, q_func);
	if(rc != OK)
	{
		err_ret(LOG_FILE_LINE,"CreateBiNode failed. rc=%d.",rc);
		return rc;
	}
	//��ȡ�����������Ҷ�ӽڵ��ַ����Ϊ���������ͷ���
	thr_binode_t *prior = (*tree)->root;
	while(prior->left_child)
	{
		prior = prior->left_child;
	}
	(*tree)->head = prior;

	//�����Ҷ�ӽڵ��������ָ��ָ��NULL.
	//���Ҷ�Ҷ�ӽڵ����ָ��ָ��NULL.
	//����������ʵ�пռ�ڵ�ָ������˺����Ҷ�Ҷ�ӽڵ㡣
	prior = NULL;
	InOrderThreading((*tree)->root, &prior);
	//��ʱpriorָ�����Ҷ˵�Ҷ�ӽڵ㡣
	prior->right_child = NULL;
	prior->RTag = Thread;
	(*tree)->tail = prior;
	return rc;
}

/*
��������:
    ����������������
����˵��:
    root--���������ڵ��ַ��
����ֵ:
    ��
����:
    He kun
����:
    2012-12-14
*/
static void DestroyBiNode(thr_binode_t **root)
{
    thr_binode_t * cur = *root;
    thr_binode_t * prior = *root;
    //1.����һ���ڵ㡣

    //2.�ж���ڵ㡣
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
��������:
	����������������
����˵��:
	tree--�洢���������οؼ���ָ�롣
����ֵ:
	��
����:
	He kun
����:
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
��������:
    ��������������ʵ�֡��������򣬺��򣬲���������ǵݹ�ʵ�֡�
����˵��:
    tree--�������������Կռ��ַ��
    visit--����ÿ���ڵ�ʹ�õĲ�������ָ�롣
    order--ִ�б�������
����ֵ:

����:
	He kun
����:
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
��������:
    ���������������������
����˵��:
    root--���������ڵ㡣
    visit--�����ڵ����������
����ֵ:
	��
����:
    He kun
����:
    2013-01-19
*/
static void PreOrderTraver(BI_TREE_T tree, tree_visit visit)
{
	thr_binode_t *node = tree->root;

	while(node)
	{
		visit(GetBiTreeVal(node));
		//���������������ֱ������Ҷ�ӽڵ㡣
		while(node->LTag == Link)
		{
			node = node->left_child;
            visit(GetBiTreeVal(node));
		}
		//��Ҷ�ӽڵ㿪ʼ���ϱ�����ֱ�����������Ľڵ������������
		while(node->RTag == Thread && node->right_child)
		{
			node = node->right_child;
		}
		//����������Ϊ���ڵ��ظ������������̡�
		node = node->right_child;
	}
}

static void PostOrderTraver(BI_TREE_T tree, tree_visit visit)
{
    
}


/*
��������:
    ��ȡ���ʵ�����ݿ��׵�ַ��
����˵��:
    root--���ڵ㡣
����ֵ:
    NULL--���ڵ��ַΪ�ա�
    !NULL--ʵ�����ݿ��׵�ַ��
����:
    He kun
����:
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
��������:
	����������������˳�򣬵������
����˵��:
	tree--�������������Կռ��׵�ַ��
	visit--���ڵ�ִ�еı�������ָ�롣
����ֵ:
	��
����:
	He kun
����:
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
��������:
    ���������������������
����˵��:
	tree--�������������Կռ��׵�ַ��
	visit--���ڵ�ִ�еı�������ָ�롣
����ֵ:
    ��
����:
    He kun
����:
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
��������:
    ע��������������������
����˵��:
    funcs--�洢ע�ắ���洢�ռ�
    visit--����ÿ���ڵ�Ĳ���������
����ֵ:
    ��
����:
    He kun
����:
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
