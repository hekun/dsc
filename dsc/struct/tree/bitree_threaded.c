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
    struct thr_binode_S *parent;//ָ��˫�׽ڵ㡣���û��˫�׽ڵ㡣ָ��NULL.�����ں����������С�
    Int32_t     is_visited;  //���ڵ��Ƿ񱻷��ʹ���true--�Ѿ�ִ�й�visit������false--��δִ�й�visit����
    PointTag_t  LTag;   //���������.1--left_childָ��ǰ����㣬0--left_childָ��������
    PointTag_t  RTag;   //���������.1--right_childָ���̽�㣬0--right_childָ��������
}thr_binode_t;


struct BI_TREE_T
{
    thr_binode_t * root;  //�洢���������ڵ㡣
    visit_order_t  order; //�洢��ǰ����˳�� 
    
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
��������:
    �ݹ�����ʽ����������������������������
����˵��:
    root--���������ڵ����ָ�롣
    q_data--�洢����Ķ��������ڵ��ʵ�����ݡ�
    q_func--���в����ӿں����ṹָ��
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
	//���������Ķ�������
	rc = CreateBiNode(&(*tree)->root, q_data, q_func);
	if(rc != OK)
	{
		err_ret(LOG_FILE_LINE,"CreateBiNode failed. rc=%d.",rc);
		return rc;
	}
    //����orderֵ�Զ�����ѡ��ͬ����������ʽ��
    ThreadingBitree_Thr(*tree, order);
	return rc;
}



/*
��������:
    �����������������Կռ䡣
����˵��:
    tree--�������������Կռ��׵�ַ��
    order--ָ���Զ���������������ʽ��
����ֵ:
    OK--�ɹ���
    !OK--ʧ�ܡ�
����:
    He kun
����:
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
��������:
    ��������������������
����˵��:
    root--���������ڵ㡣
    prior--root������֮��ָ���ǰ���ڵ��ַ��
����ֵ:
    ��
����:
    He kun
����:
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
��������:
    ������������������
����˵��:
    root--���������ڵ�
    prior--�洢��������rootָ���ǰ���ڵ㡣��ʼֵ��NULL.
����ֵ:
    ��
����:
    He kun
����:
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
��������:
    ������������������
����˵��:
    root--���������ڵ�
    prior--�洢��������rootָ���ǰ���ڵ㡣��ʼֵ��NULL.
����ֵ:
    ��
����:
    He kun
����:
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
��������:
    ������������Ķ�������ʹ��ָ���������֮ǰ�Ķ�������
����˵��:
    root--�������ĸ��ڵ��ַ��
����ֵ:
    ��
����:
    He kun
����:
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
��������:
    ������������
����˵��:
    tree--�������������Կռ��׵�ַ��
����ֵ:
    OK--�ɹ���
    ERR_PARAMATER--UNKNOWN_ORDER��
����:
    He kun
����:
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
    �ݹ�ʵ���������
����˵��:
    root--���������ڵ��ַ��
    visit--�����ڵ㺯��ָ�롣
����:
    He kun
����:
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
��������:
    ��������������������
����˵��:
    root--�������������������ڵ㡣
    visit--��ÿ���ڵ�ִ�еĲ�����
����ֵ:
    ��
����:
    He kun
����:
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
��������:
    �ݹ�ʵ���������
����˵��:
    root--���������ڵ��ַ��
    visit--�����ڵ㺯��ָ�롣
����:
    He kun
����:
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
��������:
    �ݹ�ʵ�ֺ������
����˵��:
    root--���������ڵ��ַ��
    visit--�����ڵ㺯��ָ�롣
����:
    He kun
����:
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
            node = node->left_child;//�������������������δ�������������������
        }
        else if(node->right_child && node->right_child->is_visited == FALSE)
        {
            node = node->right_child;//�������������������δ�������������������
        }
        else
        {
            //�����ѱ������������������������߲��������������������������ýڵ㡣
            //��ָ����˫�׽ڵ㡣
            visit(GetBiTreeVal(node));
            node->is_visited = TRUE;
            node = node->parent;
            break;
        }
        while(1)
        {
            //��������Ҷ�ӽڵ�������ȼ����������ǵ�˫�׽ڵ㣬
            //��ˣ���Ҫ�������������Ľڵ㣬���ҵ����ȼ���ߵ��Ǹ���㡣
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
        //������ȼ���ߵ�Ҷ�ӽڵ㡣
        visit(GetBiTreeVal(node));
        node->is_visited = TRUE;
        //��������ȼ��ڵ㣬�����Ǹ��ڵ㼰���ӽڵ㡣
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


