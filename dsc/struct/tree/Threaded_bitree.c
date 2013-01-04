#include "tree.h"

#define TREE_T tree_attr_t

typedef enum 
{
    Link,   //表示指针存储地址为子节点。
    Thread  //表示指针存储地址为前驱/后继结点。
} PointTag;

struct TREE_T
{
    v_data_t *  data;
    TREE_T      left_child;
    TREE_T      right_child;
    PointTag    LTag;   //left_child指向的地址类型。
    PointTag    RTag;   //right_child指向的地址类型。
}


