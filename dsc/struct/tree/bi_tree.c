#include <string.h>
#include <assert.h>
#include "sys_lib.h"
#include "err_num.h"
#include "stack.h"
#include "tree.h"

#define TREE_T tree_attr_t

struct TREE_T
{
    v_data_t *data;
    TREE_T left_child;
    TREE_T right_child;
};

static Status CreateTree_Binary(TREE_T *root, queue_attr_t q_data, queue_funcs_t *q_func);
static void   DestroyTree_Binary(TREE_T * root);
static void   MidOrderRecusion_Binary(TREE_T root, tree_visit visit);
static void   PostOrderRecusion_Binary(TREE_T root, tree_visit visit);
static void   PreOrderRecursion_Binary(TREE_T root, tree_visit visit);
static Status LevelOrderTraverse_Binary(TREE_T root,tree_visit visit);

static void * GetTreeVal(TREE_T root);

    


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
static Status CreateTree_Binary(TREE_T *root, queue_attr_t q_data, queue_funcs_t *q_func)
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
            do
            {
                rc = Malloc((void * *) root,sizeof(**root));
                if(rc != OK)
                {
                    err_ret(LOG_FILE_LINE,"Malloc failed.rc=%d.",rc);
                    break;
                }
                rc = init_vdata(&(*root)->data, vdata->type, vdata->val, vdata->val_size);
                if(rc != OK)
                {
                    Free((void * *)root);
                    err_ret(LOG_FILE_LINE,"set_vdata failed. rc=%d",rc);
                    break;
                }
            }while(0);
        }
        destroy_vdata(&vdata);
        if(rc != OK)
        {
            log_msg(LOG_NO_FILE_LINE, "Create binary tree node failed!.");
            return rc;
        }
    }

    CreateTree_Binary(&(*root)->left_child, q_data, q_func);
    CreateTree_Binary(&(*root)->right_child, q_data, q_func);
    return OK;
}


/*
��������:
    �ݹ鷽ʽ���������������
����˵��:
    root--���������ڵ㡣
    visit--�����ڵ����������
����ֵ:
    TRUE--�ýڵ�Ϊ�ա�
    FALSE--�ýڵ������ݡ�
����:
    He kun
����:
    2012-12-20
*/
static void PreOrderRecursion_Binary(TREE_T root, tree_visit visit)
{
    void * val = NULL;
    val = GetTreeVal(root);
    if(val)
    {
        visit(val);
        PreOrderRecursion_Binary(root->left_child, visit);
        PreOrderRecursion_Binary(root->right_child, visit);
    }

}

/*
����˵��:
    ��������ݹ�ʵ�֡�
����˵��:
    root--���������ڵ㡣
    visit--���ڵ����������
����ֵ:
    ��
����:
    He kun
����:
    2012-12-20
*/
static void MidOrderRecusion_Binary(TREE_T root, tree_visit visit)
{
    void * val = NULL;
    val = GetTreeVal(root);

    if(val)
    {
        MidOrderRecusion_Binary(root->left_child, visit);
        visit(val);   
        MidOrderRecusion_Binary(root->right_child, visit);
    }

}


/*
��������:
    ���������ݹ�ʵ��
����˵��:
    root--���������ڵ㡣
    visit--���ڵ����������
����ֵ:
    ��
����:
    He kun
����:
    2012-12-21
*/
static void PostOrderRecusion_Binary(TREE_T root, tree_visit visit)
{
    void * val = NULL;
    val = GetTreeVal(root);
    if(val)
    {
        PostOrderRecusion_Binary(root->left_child, visit);
        PostOrderRecusion_Binary(root->right_child, visit);
        visit(val);
    }
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
static void * GetTreeVal(TREE_T root)
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
static void DestroyTree_Binary(TREE_T *root)
{
    if(*root)
    {
        DestroyTree_Binary(&(*root)->left_child);
        DestroyTree_Binary(&(*root)->right_child);
        destroy_vdata(&(*root)->data);
        Free((void * *) root);
    }
}

/*
��������:
    ���ö���ʵ�ֶ������Ĳ�α���
����˵��:
    root--���������ڵ㡣
    visit--���ڵ����������    
����ֵ:
    OK--��������ɹ���
    !OK--�������ʧ�ܡ�
����:
    He kun
����:
    2012-12-25
*/
static Status  LevelOrderTraverse_Binary(TREE_T root,tree_visit visit)
{
    assert(root && visit);
    Status rc = OK;
    tree_attr_t node_addr = NULL;
    queue_attr_t q = NULL;
    queue_attr_t output = NULL;
    queue_funcs_t q_funcs;
    
    
    RegisterQueueFuncs(&q_funcs, QUEUE_SIGNAL_LINK_LIST, NULL);
    rc = q_funcs.init_queue(&q, QUEUE_SIGNAL_LINK_LIST,NULL);
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"init_queue failed.rc=%d.",rc);
        return rc;
    }    
    rc = q_funcs.init_queue(&output,QUEUE_SIGNAL_LINK_LIST,NULL);
    if(rc != OK)
    {
        q_funcs.destroy_queue(&q);
        err_ret(LOG_FILE_LINE,"destroy_queue failed. rc=%d.",rc);
        return rc;
    }

    do
    {
        rc = q_funcs.en_queue(q, V_POINT, root, sizeof(root));
        if(rc != OK)
        {
            err_ret(LOG_FILE_LINE,"en_queue failed. rc=%d.",rc);
            break;
        }
        while(q_funcs.queue_empty(q) == FALSE)
        {
            q_funcs.de_queue(q, V_POINT, (void **)&node_addr, sizeof(node_addr));
            rc = q_funcs.en_queue(output, V_POINT, node_addr, sizeof(node_addr));
            if(rc != OK)
            {
                err_ret(LOG_FILE_LINE,"en_queue failed, rc=%d.",rc);
                break;
            }
            if(node_addr->left_child)
            {
                rc = q_funcs.en_queue(q, V_POINT, node_addr->left_child, sizeof(node_addr->left_child));
                if(rc != OK)
                {
                    break;
                }
            }
            if(node_addr->right_child)
            {
                rc= q_funcs.en_queue(q,V_POINT, node_addr->right_child, sizeof(node_addr->right_child));
                if(rc != OK)
                {
                    err_ret(LOG_FILE_LINE,"en_queue failed, rc=%d.",rc);
                    break;
                }

            }
        }
    }while(0);
    while(q_funcs.queue_empty(output) == FALSE)
    {
        q_funcs.de_queue(output, V_POINT, (void **)&node_addr, sizeof(node_addr));
        visit(GetTreeVal(node_addr));
    }
    
    q_funcs.clear_queue(q);
    q_funcs.clear_queue(output);
    q_funcs.destroy_queue(&q);
    q_funcs.destroy_queue(&output);
    return rc;
}




Status RegisterTreeFuncs_Binary(tree_funcs_t *funcs, tree_visit visit)
{
    assert(funcs);
    Status rc = OK;
    funcs->create_tree = CreateTree_Binary;
    funcs->destroy_tree = DestroyTree_Binary;
    if(visit == NULL)
    {
        funcs->opt_funcs.visit = NULL;
#ifdef _DEBUG
        log_msg(LOG_NO_FILE_LINE, "tree_visitδ���壬���������������޷�ʹ�á�");
#endif
        funcs->preorder_recursion = NULL;
        funcs->midorder_recursion = NULL;
        funcs->preorder_recursion = NULL;
        funcs->postorder_recusion = NULL;
        funcs->level_order = NULL;
    }
    else
    {
        funcs->opt_funcs.visit = visit;
        funcs->preorder_recursion = PreOrderRecursion_Binary;
        funcs->midorder_recursion = MidOrderRecusion_Binary;
        funcs->postorder_recusion = PostOrderRecusion_Binary;
        funcs->level_order = LevelOrderTraverse_Binary;
    }
    return rc;
}


