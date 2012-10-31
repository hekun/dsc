#ifndef LINK_INCLUDE
#define LINK_INCLUDE

#include "g_type.h"
#include "v_data.h"



#define LINK_T  link_attr_t
typedef struct  LINK_T *LINK_T;
typedef enum link_type_E
{
    UNKNOWN_LINK,
    SIGNAL_LINK_LIST,
    DOUBLE_LINK_LIST,
    CIRCLE_LINK_LIST
}link_type_t;


typedef Status  (*InitLink) (LINK_T * link);
typedef Status  (*ListEmpty) (LINK_T link);
typedef void    (*ClearLink) (LINK_T link);
typedef void    (*DestroyLink) (LINK_T *link);
typedef Status  (*InsertFirstData) (LINK_T link, v_type_t type, void * val, size_t size);
typedef Status  (*opt_visit) (void *val);
typedef Status  (*LinkTraverse) (LINK_T link, opt_visit visit);




typedef struct link_opt_funcs_S
{
    opt_visit            visit;		//显示结点信息函数
} link_opt_funcs_t;


typedef struct link_funcs_S
{
	InitLink            init_link;
    ListEmpty           link_empty;
    ClearLink           clear_link;
	DestroyLink         destroy_link;
	InsertFirstData     insert_first_data;
    LinkTraverse        link_traverse;
    link_opt_funcs_t    opt_func;//可选函数，该函数是否设置，会影响其他函数是否有效。
}link_funcs_t;

Status  RegisterLinkFuncs(link_funcs_t *funcs,link_type_t type, link_opt_funcs_t *ofunc);
void    LogoutLinkFuncs(link_funcs_t *funcs);


#undef LINK_T
#endif 

