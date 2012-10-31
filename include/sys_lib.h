#ifndef SYS_LIB_INCLUDE
#define SYS_LIB_INCLUDE

#include <stdio.h>
#include "err_num.h"
#include "g_type.h"


/*��װ��ϵͳ�⺯��*/
Status Malloc(void **ptr,size_t size);
void Free(void **ptr);
void *Realloc(void *ptr, const Uint32_t  cur_size, const Uint32_t  new_size);
/*�Զ���Ĵ�����־�������*/
Status Memcpy(void *dst, void *src, size_t src_size, size_t dst_size);


#define LOG_FILE_LINE __FILE__,__LINE__
#define LOG_NO_FILE_LINE NULL,0


void err_msg (const char*file, const Int32_t  line,const char *text, ...);
void err_quit (const char*file, const Int32_t line,const char *text, ...);
void err_ret (const char*file, const Int32_t  line,const char *text, ...);
void log_msg (const char*file, const Int32_t  line,const char *text, ...);


#endif

