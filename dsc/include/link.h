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
typedef Status  (*LinkEmpty) (LINK_T link);
typedef void    (*ClearLink) (LINK_T link);
typedef void    (*DestroyLink) (LINK_T *link);
typedef Status  (*InsertFirstVal) (LINK_T link, v_type_t type, void * val, size_t size);
typedef Status  (*InsertFirstVdata) (LINK_T link, v_data_t *vdata);
typedef Status  (*opt_visit) (void *val);
typedef Status  (*LinkTraverse) (LINK_T link, opt_visit visit);
typedef void    (*DelFirstVal) (LINK_T link,v_type_t type, void *val, size_t size);
typedef Status  (*DelFirstVdata) (LINK_T link, v_data_t **vdata);
typedef void    (*GetFirstVal) (LINK_T link,v_type_t type, void **val, size_t size);
typedef void    (*GetLinkLength) (LINK_T link, Int32_t *len);
typedef void    (*GetFirstVdata) (LINK_T link, v_data_t **vdata);
typedef Status  (*AppendVal)   (LINK_T link, v_type_t type, void *val, size_t size);
typedef Status  (*AppendVdata) (LINK_T link, v_data_t *vdata);
typedef struct link_opt_funcs_S
{
    opt_visit            visit;		//显示结点信息函数
} link_opt_funcs_t;


typedef struct link_funcs_S
{
	InitLink            init_link;
    LinkEmpty           link_empty;
    ClearLink           clear_link;
	DestroyLink         destroy_link;
	InsertFirstVal      insert_first_val;
    InsertFirstVdata    insert_first_vdata;
    LinkTraverse        link_traverse;
    DelFirstVal         del_first_val;
    DelFirstVdata       del_first_vdata;
    GetFirstVal         get_first_val;
    GetFirstVdata       get_first_vdata;
    GetLinkLength       get_link_length;
    AppendVal           append_val;
    AppendVdata         append_vdata;
    link_opt_funcs_t    opt_func;//可选函数，该函数是否设置，会影响其他函数是否有效。
}link_funcs_t;

Status  RegisterLinkFuncs(link_funcs_t *funcs,link_type_t type, link_opt_funcs_t *ofunc);
void    LogoutLinkFuncs(link_funcs_t *funcs);


#undef LINK_T
#endif 

