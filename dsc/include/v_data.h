#ifndef V_DATA_INCLUDE
#define V_DATA_INCLUDE

#include <stdio.h>  //for size_t
#include "g_type.h"
/*定义实际使用的数据类型标识*/
typedef enum v_type_E
{
    V_UNKNOWN_TYPE,  /*未知类型，用于初始化抽象数据类型*/
    V_CHAR,
    V_UNSIGNED_CHAR,
    V_INT,
    V_UNSIGNED_INT,
    V_SHORT,
    V_UNSIGNED_SHORT,
    V_LONG,
    V_UNSIGNED_LONG,
    V_FLOAT,
    V_DOUBLE,
    V_LONG_DOUBLE,
    V_UNSIGNED_LONG_DOUBLE,
    V_POINT,          /*C语言内建类型，一级指针*/
    V_CUSTOM          /*用户自定义数据类型*/
} v_type_t;
/*抽象数据类型结构定义*/
typedef struct v_data_S
{
    void        *val;      		/*指向实际数据的起始地址*/
    size_t      val_size;    	/*存储实际数据地址偏移量*/
    v_type_t    type;           /*实际数据类型*/
}v_data_t;


/*
功能描述:
    将不同的实际数据全部抽象为一种数据结构类型(v_data_t)。
    并初始化该抽象数据类型。
参数说明:
    vdata--生成的抽象数据类型。
    type--数据的实际类型。
    val--指向实际数据类型的一级指针。
        如果实际数据是C语言内建的非指针类型，val存储该数据类型的地址。
        如果实际数据是一级指针，val存储指针值。
        如果实际数据是其他类型，则先将数据存入一个结构体中,val存储该结构体地址。
    val_size--实际数据类型大小。
返回值:
    OK--抽象数据类型创建成功。
    ERROR--抽象数据类型创建失败。
注意事项:
    如果仅想创建一个抽象数据类型，不对其进行赋值。
    则设置val=NULL,val_size=0.
*/
Status init_vdata(v_data_t **vdata, v_type_t type,
                       void *val, size_t val_size);
/*
功能描述:
    销毁抽象数据类型。
参数说明:
    vdata--生成的抽象数据类型。
返回值:
    OK--抽象数据类型销毁成功。
注意事项:
    无
*/
void destroy_vdata(v_data_t **vdata);
/*
功能描述:
    将实际数值赋值给抽象数据类型。
参数说明:
    vdata--生成的抽象数据类型。
    type--数据的实际类型。
    pdata--指向实际数据类型的一级指针。
        如果实际数据是C语言内建的非指针类型，val存储该数据类型的地址。
        如果实际数据是一级指针，val存储指针值。
        如果实际数据是其他类型，则先将数据存入一个结构体中,val存储该结构体地址。
    data_size--实际数据类型大小。    
返回值:
    OK--赋值成功。
注意事项:
    无
*/
Status set_vdata(v_data_t *vdata, v_type_t type,
                      void *pdata, size_t data_size);
/*
功能描述:
    获取抽象数据类型的起始地址，用于获取实际数据值。
参数说明:
    vdata--生成的抽象数据类型。
返回值:
	返回存储实际数据的首地址。
注意事项:
    无
*/
void * get_vdata(v_data_t *vdata);

/*
功能描述:
    比较两个抽象数据类型是否对应同一个实际数据。
参数说明:
    略
返回值:
    UN_EQUAL--两抽象数据类型指向的是不同的数据。
    EQUAL--两抽象数据类型指向的是不同的数据。
*/
Status compare_vdata(v_data_t *vdata1, v_data_t *vdata2);

/*
功能描述:
判断抽象数据类型是否为空。
参数说明:

返回值:

注意事项;

作者: 何昆
日期: 2012-10-14
*/
Status empty_vdata(v_data_t *vdata);

#endif

