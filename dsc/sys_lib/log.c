#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>
#include "g_type.h"

#define B_TRUE   1
#define B_FALSE  0
#define LINE_LEN 256


static void err_common (Int32_t flag, const char *text, va_list args);

/*
    打印系统错误信息，退出程序。
*/
void err_msg (const char*file,const Int32_t line, const char *text, ...)
{
    va_list arg;
    char buf[LINE_LEN];
    memset(buf,'\0',sizeof(buf));
    
    if((file != NULL) && (line>0))
    {
        snprintf(buf,sizeof(buf),"%s:%d %s",file,line,text);
    }
    else
    {
        memcpy(buf, text, strlen(text)+1);
    }
    va_start (arg, text);
    err_common (B_TRUE, text, arg);
    va_end (arg);
    exit (1);
}
/*
    打印自定义错误信息，退出程序。
*/
void err_quit(const char*file,const Int32_t line, const char *text,...)
{
    va_list arg;
    char buf[LINE_LEN];
    memset(buf,'\0',sizeof(buf));
    if((file != NULL) && (line>0))
    {
        snprintf(buf,sizeof(buf),"%s:%d %s",file,line,text);
    }
    else
    {
        memcpy(buf,text, strlen(text)+1);
    }
    va_start (arg, text);
    err_common (B_FALSE, text, arg);
    va_end (arg);
    exit (1);
}

/*
    打印系统出错信息，但不退出程序。
*/
void err_ret (const char*file,const Int32_t line, const char *text,...)
{
    va_list arg;
    char buf[LINE_LEN];
    memset(buf,'\0',sizeof(buf));
    if((file != NULL) && (line>0))
    {
        snprintf(buf,sizeof(buf),"%s:%d %s",file,line,text);
    }
    else
    {
        memcpy(buf,text, strlen(text)+1);
    }
    va_start (arg, text);
    err_common (B_TRUE, text, arg);
    va_end (arg);
}

/*
    打印日志信息。
*/
void log_msg (const char*file,const Int32_t line,const char *text, ...)
{
    va_list arg;
    char buf[LINE_LEN];
    memset(buf,'\0',sizeof(buf));
    if((file != NULL) && (line>0))
    {
        snprintf(buf,sizeof(buf),"%s:%d %s",file,line,text);
    }
    else
    {
        memcpy(buf,text,strlen(text));
    }
    va_start (arg, text);
    err_common (B_FALSE, buf, arg);
    va_end (arg);
}

static void err_common (Int32_t flag,const char *text, va_list args)
{
    Int32_t   old_errno;
    Int32_t   n;
    char buf [LINE_LEN];

    old_errno = errno;
    n = vsnprintf (buf, sizeof (buf), text, args);
    if (flag == B_TRUE)
        snprintf (buf + n, sizeof (buf) - n, ": %s", strerror (old_errno));
    strcat (buf, "\n");

    fflush (stdout);
    fprintf (stderr, "%s", buf);
    fflush (stderr);

}



