#ifndef TREE_INCLUDE
#define TREE_INCLUDE

#define TREE_T  tree_attr_t
typedef struct  TREE_T *TREE_T;

typedef Status  (*tree_visit) (TREE_T root);



#undef TREE_T

#endif
