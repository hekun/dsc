#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <assert.h>
#include "sys_lib.h"



Status Malloc(void **ptr,size_t size)
{
    if(size == 0)
    {
        return ERR_PARAMATER;
    }
    if((*ptr = malloc(sizeof(char)*size)) == NULL)
    {
        return ERR_MALLOC;
    }
    bzero(*ptr,size);
    return OK;
}

void Free(void **ptr)
{
    free(*ptr);
    *ptr = NULL;
}

/*
��������:
     �޸���������ڴ�ռ��С.
����˵��:
    ptr--��������ڴ�ռ�
    cur_size--��������ڴ�ռ��С.
    new_size--�޸ĺ���ڴ�ռ��С.
����ֵ:
    1.���new_size = 0,˵�����ڴ�ռ䱻�ͷţ���Ч��free.
    ����NULL;
    2.���new_size > 0,��realloc()����NULL.˵��realloc����ʧ�ܡ�
    ����ԭ�ڴ�ռ��׵�ַ.
    3.���new_size < cur_size, ��ԭ������ڴ�ռ�ᱻ�ض�,
    �����ڴ�ռ��׵�ַ��    
    4.���new_size >= cur_size,��realloc�ɹ���
    ������������ڴ�ռ��׵�ַ��
    ������ɹ�������ԭ�ڴ�ռ��׵�ַ.
    5.���ptrΪ�գ���Ҫ��malloc�����ڴ�ռ䡣
    ����Malloc������¿ռ��׵�ַ.    
*/
void *Realloc(void *ptr, const Uint32_t  cur_size, const Uint32_t  new_size)
{

    void *tmp = NULL;
    if(ptr == NULL)
    {
        Malloc(&tmp, new_size);
        return tmp;
    }
    if((new_size > 0) && (new_size < cur_size))
    {
        err_quit(LOG_FILE_LINE,"realloc memary reduce.");
    }
    if ( ( tmp = realloc(ptr, new_size)) == NULL)
    {
        if(new_size >= cur_size)
        {
            err_ret(LOG_FILE_LINE,"realloc failed.");
            return ptr;
        }
        if(new_size == 0)
        {
            err_ret(LOG_NO_FILE_LINE,"free memory");
            return NULL;
        }
    }
    return tmp;

}

/*
��������:
    �ڴ�θ���
����˵��:
    dst--Ŀ�Ŀռ�.
    src--Ҫ���Ƶ��ڴ�ε��׵�ַ.
    src_size--Ҫ���Ƶ��ڴ�δ�С.
    dst_size--Ŀ�Ŀռ��С.
����ֵ˵��:
    OVER_FLOW--Ŀ�Ŀռ�С��Դ���ݿռ��С.
    OK--���Ƴɹ�.
*/
Status Memcpy(void *dst, void *src, size_t src_size, size_t dst_size)
{
    assert(dst != NULL && src != NULL && src_size > 0 && dst_size > 0);
    //��ֹ���ֻ��������
    if(src_size > dst_size)
    {
        err_ret(LOG_NO_FILE_LINE,"Memcpy overflow.");
        return ERR_OVERFLOW;
    }
    //ϵͳ�⺯��memcpy������ȷ�����ص�������(dst,srcָ��ͬһ��������)�ĸ��ơ�
    //��memmove���ԡ����ʹ��memmove����.
    memmove(dst, src, src_size);
    return OK;
}


