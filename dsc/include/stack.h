#ifndef STACK_INCLUDE
#define STACK_INCLUDE

#include "g_type.h"
#include "v_data.h"

#define STACK_T  stack_attr_t
typedef struct  STACK_T *STACK_T;
typedef enum stack_type_E
{
    UNKNOWN_STACK,
    STACK_SIGNAL_LINK_LIST,
    STACK_DOUBLE_LINK_LIST,
    STACK_CIRCLE_LINK_LIST
}stack_type_t;

typedef Status  (*stack_visit) (void *val);
typedef Status  (*InitStack)(STACK_T *stack, stack_type_t type, stack_visit visit);
typedef void    (*DestroyStack)(STACK_T *stack);
typedef Status  (*ClearStack)(STACK_T stack);
typedef Status  (*StackEmpty)(STACK_T stack);
typedef void    (*StackLength)(STACK_T stack, Int32_t *elem_total);
typedef Status  (*GetTop)(STACK_T stack, v_type_t type, void **val, size_t size);
typedef Status  (*Push) (STACK_T stack, v_type_t type, void *val, size_t size);
typedef Status  (*Pop) (STACK_T stack, v_type_t type, void **val, size_t size);
typedef Status  (*StackTraverse) (STACK_T stack, stack_visit vist);

typedef struct stack_opt_funcs_S
{
    stack_visit            visit;		//显示结点信息函数
} stack_opt_funcs_t;


typedef struct Stack_funcs_S
{
	InitStack           init_stack;
    DestroyStack        destroy_stack;
    ClearStack          clear_stack;
    StackEmpty          stack_empty;
    StackLength         stack_length;
    GetTop              get_top;
    Push                push;
    Pop                 pop;
    StackTraverse       stack_traverse;
    stack_opt_funcs_t   opt_func;//可选函数，该函数是否设置，会影响其他函数是否有效。
}Stack_funcs_t;

void  RegisterStackFuncs(Stack_funcs_t * stk_funcs, stack_type_t type, stack_visit visit);
void    LogoutStackFuncs(Stack_funcs_t * funcs, stack_type_t type);


#undef STACK_T

#endif

