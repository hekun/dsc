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


typedef Status  (*tree_visit) (TREE_T root);
typedef Status (*CreateTree) (TREE_T root, queue_attr_t *q_data, queue_funcs_t *q_func);
typedef Status (*DestroyTree) (v_data_t **vdata);
typedef Status (*PreOrderUnrecursion) (TREE_T root, tree_visit visit);

typedef struct tree_opt_funcs_S
{
    tree_visit visit;
}tree_opt_funcs_t;


typedef struct tree_funcs_S
{
    CreateTree  create_tree;
    DestroyTree destroy_tree;
    PreOrderUnrecursion preorder_unrecursion;
    tree_opt_funcs_t opt_funcs;
    
}tree_funcs_t;

Status RegisterTreeFuncs(tree_funcs_t *funcs, tree_type_t type, tree_visit visit);
void   LogoutTreeFuncs (tree_funcs_t* funcs);



#undef TREE_T

#endif
