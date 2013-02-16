#ifndef BI_TREE_INCLUDE
#define BI_TREE_INCLUDE

#include "g_type.h"
#include "v_data.h"
#include "queue.h"

#define BI_TREE_T  bitree_attr_t
typedef struct  BI_TREE_T *BI_TREE_T;


typedef enum BI_TREE_Type_E
{
    UNKNOWN_BITREE,
    THREADED_BINARTY_TREE
}BiTree_Type_t;

/*定义遍历二叉树方式*/
typedef enum VISIT_ORDER_E
{
	UNKNOWN_ORDER = 0,      //未知遍历顺序
	PRE_THREADED_ORDER,	    //先序线索化后进行先序遍历
	IN_THREADED_ORDER,	    //中序线索化后进行中序遍历
	POST_THREADED_ORDER,	//后序线索化后进行后序遍历
	LEVEL_ORDER,            //层序遍历
	PRE_ORDER,
	IN_ORDER,
	POST_ORDER
}visit_order_t;


typedef Status (*tree_visit)      (void * val);
typedef Status (*InitBitree)      (BI_TREE_T *tree, visit_order_t order);
typedef Status (*CreateBiTree)    (BI_TREE_T * tree, visit_order_t order, queue_attr_t q_data, queue_funcs_t * q_func);
typedef void   (*DestroyBiTree)   (BI_TREE_T *tree);
typedef Status (*TraverseBiTree)  (BI_TREE_T tree, tree_visit visit);
typedef Status (*ThreadingBitree) (BI_TREE_T tree, visit_order_t order);
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
    ThreadingBitree threading_bitree;
    bitree_opt_funcs_t opt_funcs;
    
}bitree_funcs_t;

Status RegisterBiTreeFuncs(bitree_funcs_t *funcs, BiTree_Type_t type, tree_visit visit);
void   LogoutBiTreeFuncs (bitree_funcs_t* funcs);



#undef BI_TREE_T

#endif
