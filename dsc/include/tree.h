#ifndef TREE_INCLUDE
#define TREE_INCLUDE

#include "g_type.h"
#include "v_data.h"
#include "queue.h"

#define TREE_T tree_attr_t
typedef struct TREE_T *TREE_T;

typedef enum tree_type_E
{
    UNKNOWN_TREE,
    BINARY_TREE    //二叉树
}tree_type_t;

typedef enum visit_order_E
{
    UNKOWN_ORDER,
    BIT_PRE_ORDER,      //二叉树先序遍历
    //BIT_MID_ORDER,    //二叉树中序遍历
    //BIT_LAST_ORDER    //二叉树后续遍历
}visit_order_t;

typedef Status  (*tree_visit) (void *val);
typedef Status  (*InitTree) (TREE_T *T, visit_order_t order);
typedef Status  (*CreateTree)(TREE_T T, queue_attr_t q_data, queue_funcs_t * q_funcs, 
                              void * val, size_t val_size, v_type_t type);
typedef void    (*DestroyTree) (TREE_T *T);
typedef void    (*TraverseTree) (TREE_T T, visit_order_t order, tree_visit visit);


typedef struct tree_opt_funcs_S
{
    tree_visit            visit;		//显示结点信息函数
} tree_opt_funcs_t;

typedef struct Tree_funcs_S
{
    InitTree    init_tree;
    CreateTree  create_tree;
    DestroyTree destroy_tree;
    TraverseTree traverse_tree;
    tree_opt_funcs_t opt_funcs;//可选函数，该函数是否设置，会影响其他函数是否有效。
}Tree_funcs_t;


void RegisterTreeFuncs(Tree_funcs_t *funcs, tree_type_t type, tree_visit visit);
void LogoutTreeFuncs(Tree_funcs_t *funcs);
#undef TREE_T
#endif
