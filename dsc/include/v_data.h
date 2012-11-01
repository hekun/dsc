#ifndef V_DATA_INCLUDE
#define V_DATA_INCLUDE

#include <stdio.h>  //for size_t
#include "g_type.h"
/*����ʵ��ʹ�õ��������ͱ�ʶ*/
typedef enum v_type_E
{
    V_UNKNOWN_TYPE,  /*δ֪���ͣ����ڳ�ʼ��������������*/
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
    V_POINT,          /*C�����ڽ����ͣ�һ��ָ��*/
    V_CUSTOM          /*�û��Զ�����������*/
} v_type_t;
/*�����������ͽṹ����*/
typedef struct v_data_S
{
    void        *val;      		/*ָ��ʵ�����ݵ���ʼ��ַ*/
    size_t      val_size;    	/*�洢ʵ�����ݵ�ַƫ����*/
    v_type_t    type;           /*ʵ����������*/
}v_data_t;


/*
��������:
    ����ͬ��ʵ������ȫ������Ϊһ�����ݽṹ����(v_data_t)��
    ����ʼ���ó����������͡�
����˵��:
    vdata--���ɵĳ����������͡�
    type--���ݵ�ʵ�����͡�
    val--ָ��ʵ���������͵�һ��ָ�롣
        ���ʵ��������C�����ڽ��ķ�ָ�����ͣ�val�洢���������͵ĵ�ַ��
        ���ʵ��������һ��ָ�룬val�洢ָ��ֵ��
        ���ʵ���������������ͣ����Ƚ����ݴ���һ���ṹ����,val�洢�ýṹ���ַ��
    val_size--ʵ���������ʹ�С��
����ֵ:
    OK--�����������ʹ����ɹ���
    ERROR--�����������ʹ���ʧ�ܡ�
ע������:
    ������봴��һ�������������ͣ���������и�ֵ��
    ������val=NULL,val_size=0.
*/
Status init_vdata(v_data_t **vdata, v_type_t type,
                       void *val, size_t val_size);
/*
��������:
    ���ٳ����������͡�
����˵��:
    vdata--���ɵĳ����������͡�
����ֵ:
    OK--���������������ٳɹ���
ע������:
    ��
*/
void destroy_vdata(v_data_t **vdata);
/*
��������:
    ��ʵ����ֵ��ֵ�������������͡�
����˵��:
    vdata--���ɵĳ����������͡�
    type--���ݵ�ʵ�����͡�
    pdata--ָ��ʵ���������͵�һ��ָ�롣
        ���ʵ��������C�����ڽ��ķ�ָ�����ͣ�val�洢���������͵ĵ�ַ��
        ���ʵ��������һ��ָ�룬val�洢ָ��ֵ��
        ���ʵ���������������ͣ����Ƚ����ݴ���һ���ṹ����,val�洢�ýṹ���ַ��
    data_size--ʵ���������ʹ�С��    
����ֵ:
    OK--��ֵ�ɹ���
ע������:
    ��
*/
Status set_vdata(v_data_t *vdata, v_type_t type,
                      void *pdata, size_t data_size);
/*
��������:
    ��ȡ�����������͵���ʼ��ַ�����ڻ�ȡʵ������ֵ��
����˵��:
    vdata--���ɵĳ����������͡�
����ֵ:
	���ش洢ʵ�����ݵ��׵�ַ��
ע������:
    ��
*/
void * get_vdata(v_data_t *vdata);

/*
��������:
    �Ƚ������������������Ƿ��Ӧͬһ��ʵ�����ݡ�
����˵��:
    ��
����ֵ:
    UN_EQUAL--��������������ָ����ǲ�ͬ�����ݡ�
    EQUAL--��������������ָ����ǲ�ͬ�����ݡ�
*/
Status compare_vdata(v_data_t *vdata1, v_data_t *vdata2);

/*
��������:
�жϳ������������Ƿ�Ϊ�ա�
����˵��:

����ֵ:

ע������;

����: ����
����: 2012-10-14
*/
Status empty_vdata(v_data_t *vdata);

#endif

