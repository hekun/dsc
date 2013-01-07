#ifndef TREE_INCLUDE
#define TREE_INCLUDE

#include "g_type.h"
#include "v_data.h"
#include "queue.h"

#define TREE_T  tree_attr_t
typedef struct  TREE_T *TREE_T;


typedef enum tree_type_E
{
    UNKNOWN_TREE,
    BINART_TREE
}tree_type_t;


typedef Status (*tree_visit) (void * val);
typedef Status (*CreateBiTree) (TREE_T *root, queue_attr_t q_data, queue_funcs_t *q_func);
typedef void   (*DestroyBiTree) (TREE_T *root);
typedef void   (*PreOrderRecursion) (TREE_T root, tree_visit visit);
typedef void   (*MidOrderRecusion)  (TREE_T root, tree_visit visit);
typedef void   (*PostOrderRecusion) (TREE_T root, tree_visit visit);
typedef Status (*LevelOrderTraverse)(TREE_T root,tree_visit visit);

typedef struct tree_opt_funcs_S
{
    tree_visit visit;
}tree_opt_funcs_t;


typedef struct tree_funcs_S
{
    CreateBiTree  create_bitree;
    DestroyBiTree destroy_bitree;
    PreOrderRecursion   preorder_recursion;
    MidOrderRecusion    midorder_recursion;
    PostOrderRecusion   postorder_recusion;
    LevelOrderTraverse  level_order;
    tree_opt_funcs_t opt_funcs;
    
}tree_funcs_t;

Status RegisterTreeFuncs(tree_funcs_t *funcs, tree_type_t type, tree_visit visit);
void   LogoutTreeFuncs (tree_funcs_t* funcs);



#undef TREE_T

#endif
