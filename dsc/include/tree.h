#ifndef TREE_INCLUDE
#define TREE_INCLUDE

#include "g_type.h"
#include "v_data.h"

#define TREE_T tree_attr_t
typedef struct TREE_T *TREE_T;

typedef enum tree_type_E
{
    UNKNOWN_TREE,
    BINARY_TREE    //¶þ²æÊ÷
}tree_type_t;

typedef Status  (*tree_visit) (void *val);





#undef TREE_T
#endif
