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
    bit_node_t  *root;       //ָ����ڵ�
    Int32_t     depth;      //�������
    Int32_t     amount;     //�ڵ�����
    visit_order_t order;    //�����ڵ�˳��
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
��������:
    �����������������ÿ���ڵ�ִ��visit������
����˵��:
    root--�Ѿ����ڵĶ��������Կռ䡣
����ֵ:
    FALSE--�ýڵ������ݣ���˫�׽ڵ�ΪҶ�ӽڵ㡣
    TRUE--���ڵ㼰��������������ϡ�
�㷨����:
    �Ӹ��ڵ㿪ʼ��һֱ�����������ÿ�������Ľڵ㶼ִ��visit������ֱ�������սڵ㡣
    ������սڵ���ֵܽڵ�ǿգ���ӿսڵ���ֵܽڵ㿪ʼ��һ·���󣬼���������
    ���򣬷���true�����ݵݹ����ԣ������Ĳ�����һֱ�������죬ֱ���������ĸ��ڵ㡣
    ��������������֮�󣬷���TRUE.
����:
    He kun
����:
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
��������:
    �������������н�㡣
����˵��:
    T--���������Կռ��׵�ַ��
    order--����˳��ѡ�
    visit--��ÿ�����ִ�в���������
����ֵ:

����:
    He kun
����:
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
��������:
    �������е����ݲ��뵽�������С�
����˵��:
    T--���������Կռ��׵�ַ��
    q_data--�洢���ݵĶ��С�
    q_funcs--���ڶ��в����ĺ����ṹָ�롣
    val--�洢�Ӷ�����ȡ���������뵽�������е����ݵ�ַ��
    val_size--�洢���ݵĳ��ȡ�
    type--�洢���ݵ����͡�    
����ֵ:
    OK--�ɹ���
����:
    He kun
����:
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
��������:
    ���������ʽ����ڵ㵽�������С�
����˵��:
    root--���������ڵ㡣
    q_data--�洢���ݵĶ��С�
    q_funcs--���ڶ��в����ĺ����ṹָ�롣
    val--�洢�Ӷ�����ȡ���������뵽�������е����ݵ�ַ��
    val_size--�洢���ݵĳ��ȡ�
    type--�洢���ݵ����͡�
����ֵ:
    OK--�������ݲ�����ɡ�
    !OK--��������ʧ�ܡ�
����:
    He kun
����:
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
��������:
    ����һ���������ڵ㡣
����˵��:
    root--�洢�ڵ��׵�ַ��
    val--ָ��ʵ�����ݵ���ʼ��ַ��
    type--ʵ����������.
    val_size--ʵ�����ݳ���
����ֵ:
    OK--�ɹ���
    ERROR--ʧ�ܡ�
����:
    He kun
����:
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
��������:
    ���ٸ��ڵ㼰��������
����˵��:
    root--���ڵ����ָ�롣
����ֵ:
    ��
����:
    He kun
����:
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
��������:
    ���ٶ�����
����˵��:
    T--���������Կռ����ָ�롣
����ֵ:
    ��
����:
    He kun
����:
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
        log_msg(LOG_NO_FILE_LINE, "opt_visitδ���壬traverse_tree�����޷�ʹ�á�");
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


