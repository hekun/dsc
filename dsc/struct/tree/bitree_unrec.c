/*
���ļ�ʵ�ֶ������Ĵ����������ķǵݹ�ʵ�֡�
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
    struct binode_S *parent;    //ָ��˫�׽ڵ㡣���û��˫�׽ڵ㡣ָ��NULL.
}bit_node_t;

struct BI_TREE_T
{
    bit_node_t       *root;  //�洢���������ڵ㡣
    visit_order_t  order;  //�洢��ǰ����˳�� 
    
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
��������:
    �������ӽڵ㣬ԭ���ӽڵ�����һ�㡣
����˵��:
    node--Ҫ����Ľڵ�
    parent--Ҫ�������ӽڵ��˫�׽ڵ�
����ֵ:
    ��
����:
    He kun
����:
    2013-02-17
ע������:
    ������ڵ�Ϊ���ڵ㣬����ڵ��˫�׽ڵ�ΪNULL.
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
��������:
    �������ӽڵ㣬ԭ���ӽڵ�����һ�㡣
����˵��:
    node--Ҫ����Ľڵ�
    parent--Ҫ�������ӽڵ��˫�׽ڵ�
����ֵ:
    ��
����:
    He kun
����:
    2013-02-18
ע������:
    ������ڵ�Ϊ���ڵ㣬����ڵ��˫�׽ڵ�ΪNULL.
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
��������:
    �����������ڵ㡣
����˵��:
    node--�������ڵ����ָ�롣
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
��������:
    ������������
����˵��:
    root--���������ڵ����ָ�롣
    q_data--�洢����Ķ��������ڵ��ʵ�����ݡ�
    q_func--���в����ӿں����ṹָ��    
����ֵ:
    OK--�����ɹ���
    !OK--�����ӽڵ�ʧ�ܡ�
����:
    He kun
����:
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
        //�������ڵ����������
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
            destroy_vdata(&vdata);  //����յ����ӽ��.

            while((q_func->queue_empty(q_data) == FALSE)
                  && (rc = q_func->de_queue_vdata(q_data, &vdata)) == OK)
            {
                if(get_vdata(vdata) == NULL)
                {
                    //�����������ӽڵ㣬��һֱ���ϱ�����
                    destroy_vdata(&vdata);
                    parent = parent->parent;
                }
                else
                {
                    //�����ڽڵ�ֵ������ָ��Ϊparent�����ӽڵ㡣
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
��������:
    �������������Կռ䡣
����˵��:
    tree--���������Կռ��׵�ַ��
    order--ָ���Զ������ı�����ʽ��
����ֵ:
    OK--�ɹ���
    !OK--ʧ�ܡ�
����:
    He kun
����:
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
��������:
    ����������
����˵��:
    tree--ָ����������Կռ�Ķ���ָ�롣
    order--ָ�������������Ĵ���˳��Ĭ��Ϊ����ʽ������
    q_data--�洢����Ķ��������ڵ��ʵ�����ݡ�
    q_func--���в����ӿں����ṹָ��
����ֵ:
    OK--�ɹ���
    ERR_PARAMATER--order����ֵ��֧�֡�
����:
    He kun
����:
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
��������:
    ʹ�ú����������ٶ�������
����˵��:
    root--���������ڵ��ַ��
����ֵ:
    ��
����:
    He kun
����:
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
            //������/���ӽڵ㡣
#ifdef _DEBUG
            printf("Free node %c.\n", *(char *)prior->data->val);
#endif
            destroy_vdata(&prior->data);
            Free((void * *)&prior);  
        }
    }  
    //���ٶ��������ڵ㡣
#ifdef _DEBUG
    printf("Free node %c.\n", *(char *)prior->data->val);
#endif
    destroy_vdata(&prior->data);
    Free((void * *)&prior);    
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
static void DestroyBiTree_Unrec(BI_TREE_T *tree)
{
    assert(*tree && (*tree)->root);
	bit_node_t *root = (*tree)->root;
    DestroyBiNode_Unrec(&root);
	Free((void **)tree);
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
��������:
    �ǵݹ�ʵ���������
����˵��:
    root--���������ڵ��ַ��
    visit--�����ڵ㺯��ָ�롣
����:
    He kun
����:
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
            //����˫�׽ڵ�.
            visit(GetBiTreeVal(cur));
            prior = cur;
            cur = cur->left_child;
        }
        //�������ӽڵ�
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
��������:
    �ǵݹ�ʵ���������
����˵��:
    root--���������ڵ��ַ��
    visit--�����ڵ㺯��ָ�롣
����:
    He kun
����:
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
        //������ӽڵ�
        visit(GetBiTreeVal(cur));
        while(cur && (!cur->right_child || cur->right_child == prior))
        {
            prior = cur;
            cur = cur->parent;
            //���ϱ���˫�׽ڵ㡣
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
��������:
    �ǵݹ�ʵ�ֺ���������
����˵��:
    root--���������ڵ��ַ��
    visit--�����ڵ㺯��ָ�롣
����ֵ:
    ��
����:
   He kun
����:
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
        //ʹpriorָ���ӽڵ㣬curָ����˫�׽ڵ�
        prior = cur;
        cur = cur->parent;
        if(cur)
        {
            //visit�ӽڵ�
            visit(GetBiTreeVal(prior));
            //����curָ���˫�׽ڵ���ԣ����䲻�������ӽڵ㣬�����ӽڵ�
            //�Ѿ�����������cur�������ϱ�����prior������󡣱��ڳ���ʼ��
            //����δ���������ӽڵ㡣
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
��������:
    �����������ӿں���
����˵��:
    root--���������ڵ��ַ��
    visit--�����ڵ㺯��ָ�롣
����ֵ:
    ERR_PARAMATER--��������
    OK--�ɹ�
����:
    He kun
����:
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
��������:
    ע���������������
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


