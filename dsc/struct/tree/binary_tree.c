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
    bit_node_t *root;   //指向根节点
    Int32_t     depth;  //树的深度
    Int32_t     amount;    //节点总数
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
功能描述:
    先序遍历二叉树，对每个节点执行visit函数。
参数说明:
    bit--已经存在的二叉树属性空间。
返回值:
    
作者:
    He kun
日期:
    2012-11-20
*/

static Status PreOrderTraverse_Binary(bit_node_t *root, tree_visit visit)
{
    if(root)
    {
        //优先处理根节点。
        visit(root->data->val);
        //一直向左移动，直至遇到最左端的叶子节点。此时，返回FALSE.
        if(PreOrderTraverse_Binary(root->left_child,visit) == FALSE)
        {
            //指向root的右子节点，再以该节点为根，一直向左，直至遇到
            //最左端的叶子节点。如果不存在右子节点，说明根节点和及其
            //左右子树均已遍历完毕。此时函数返回TRUE.
            if(PreOrderTraverse_Binary(root->right_child, visit) == FALSE)
            {
                return TRUE;
            }
        }
        return FALSE;
    }
    return FALSE;
}


