#ifndef TREE_INCLUDE
#define TREE_INCLUDE

#include "g_type.h"
#include "v_data.h"
#include "queue.h"

#define BI_TREE_T  bitree_attr_t
typedef struct  BI_TREE_T *BI_TREE_T;


typedef enum BI_TREE_Type_E
{
    UNKNOWN_BITREE,
    THREADED_BINART_TREE
}BiTree_Type_t;

/*定义遍历二叉树方式*/
typedef enum VISIT_ORDER_E
{
	UNKNOWN_ORDER = 0,//未知遍历顺序
	PRE_ORDER,	//先序遍历
	MID_ORDER,	//中序遍历
	POST_ORDER,	//后续遍历
	LEVEL_ORDER,//层序遍历
	THREAD_ORDER,//线索化顺序遍历
	THREAD_REVERSE//线索化逆序遍历
}visit_order_t;


typedef Status (*tree_visit)    (void * val);
typedef Status (*InitBitree)    (BI_TREE_T *tree);
typedef Status (*CreateBiTree)  (BI_TREE_T *tree, queue_attr_t q_data, queue_funcs_t *q_func);
typedef void   (*DestroyBiTree) (BI_TREE_T *tree);
typedef void   (*TraverseBiTree)(BI_TREE_T tree, tree_visit visit, visit_order_t order);

typedef struct tree_opt_funcs_S
{
    tree_visit visit;
}bitree_opt_funcs_t;


typedef struct tree_funcs_S
{
    InitBitree    init_bitree;
    CreateBiTree  create_bitree;
    DestroyBiTree destroy_bitree;
    TraverseBiTree traverse_bitree;
    bitree_opt_funcs_t opt_funcs;
    
}bitree_funcs_t;

Status RegisterBiTreeFuncs(bitree_funcs_t *funcs, BiTree_Type_t type, tree_visit visit);
void   LogoutBiTreeFuncs (bitree_funcs_t* funcs);



#undef BI_TREE_T

#endif
