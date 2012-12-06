#ifndef TREE_INCLUDE
#define TREE_INCLUDE

#define TREE_T  tree_attr_t
typedef struct  TREE_T *TREE_T;

typedef Status  (*tree_visit) (void *val);



#undef TREE_T

#endif
