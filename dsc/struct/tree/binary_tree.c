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


typedef struct TREE_T
{
    bit_node_t *root;   //ָ����ڵ�
    Int32_t     depth;  //�������
    Int32_t     amount;    //�ڵ�����
}

static Status InitTree_Binary(TREE_T *bit)
{
    Status rc = OK;
    if(*bit != NULL)
    {
        log_msg(LOG_FILE_LINE, "binaray tree attr exist.");
        return OK;        
    }
    rc = Malloc((void * *)bit, sizeof(**bit));
    if(rc != OK)
    {
        err_ret(LOG_FILE_LINE,"malloc failed,rc=%d.",rc);
        return rc;        
    }
    TREE_T tmp_t = *bit;
    tmp_t->root = NULL;
    tmp_t->depth = 0;
    tmp_t->amount = 0;
#ifdef _DEBUG
    log_msg(LOG_NO_FILE_LINE, "Malloc binaray tree.");
#endif
    return rc; 
}

/*
��������:
    �����������������ÿ���ڵ�ִ��visit������
����˵��:
    bit--�Ѿ����ڵĶ��������Կռ䡣
����ֵ:
    
����:
    He kun
����:
    2012-11-20
*/

static Status PreOrderTraverse_Binary(bit_node_t *root, tree_visit visit)
{
    if(root)
    {
        //���ȴ�����ڵ㡣
        visit(root->data->val);
        //һֱ�����ƶ���ֱ����������˵�Ҷ�ӽڵ㡣��ʱ������FALSE.
        if(PreOrderTraverse_Binary(root->left_child,visit) == FALSE)
        {
            //ָ��root�����ӽڵ㣬���Ըýڵ�Ϊ����һֱ����ֱ������
            //����˵�Ҷ�ӽڵ㡣������������ӽڵ㣬˵�����ڵ�ͼ���
            //�����������ѱ�����ϡ���ʱ��������TRUE.
            if(PreOrderTraverse_Binary(root->right_child, visit) == FALSE)
            {
                return TRUE;
            }
        }
        return FALSE;
    }
    return FALSE;
}


