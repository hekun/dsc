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
功能描述:
     修改已申请的内存空间大小.
参数说明:
    ptr--已申请的内存空间
    cur_size--已申请的内存空间大小.
    new_size--修改后的内存空间大小.
返回值:
    1.如果new_size = 0,说明该内存空间被释放，等效于free.
    返回NULL;
    2.如果new_size > 0,但realloc()返回NULL.说明realloc调用失败。
    返回原内存空间首地址.
    3.如果new_size < cur_size, 则原申请的内存空间会被截断,
    返回内存空间首地址。    
    4.如果new_size >= cur_size,且realloc成功。
    返回新申请的内存空间首地址，
    如果不成功，返回原内存空间首地址.
    5.如果ptr为空，需要由malloc申请内存空间。
    返回Malloc申请的新空间首地址.    
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
功能描述:
    内存段复制
参数说明:
    dst--目的空间.
    src--要复制的内存段的首地址.
    src_size--要复制的内存段大小.
    dst_size--目的空间大小.
返回值说明:
    OVER_FLOW--目的空间小于源数据空间大小.
    OK--复制成功.
*/
Status Memcpy(void *dst, void *src, size_t src_size, size_t dst_size)
{
    assert(dst != NULL && src != NULL && src_size > 0 && dst_size > 0);
    //防止出现缓冲区溢出
    if(src_size > dst_size)
    {
        err_ret(LOG_NO_FILE_LINE,"Memcpy overflow.");
        return ERR_OVERFLOW;
    }
    //系统库函数memcpy不能正确处理重叠缓冲区(dst,src指向同一个缓冲区)的复制。
    //但memmove可以。因此使用memmove函数.
    memmove(dst, src, src_size);
    return OK;
}


