#include "tree.h"

#define TREE_T tree_attr_t

typedef enum 
{
    Link,   //��ʾָ��洢��ַΪ�ӽڵ㡣
    Thread  //��ʾָ��洢��ַΪǰ��/��̽�㡣
} PointTag;

struct TREE_T
{
    v_data_t *  data;
    TREE_T      left_child;
    TREE_T      right_child;
    PointTag    LTag;   //left_childָ��ĵ�ַ���͡�
    PointTag    RTag;   //right_childָ��ĵ�ַ���͡�
}


