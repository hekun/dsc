#include <assert.h>
#include "v_data.h"
#include "sys_lib.h"
#include "stack.h"


static Status visitnode_stack(void *val);
static void stack_test(stack_type_t type);


Int32_t main(void)
{
    log_msg(LOG_NO_FILE_LINE, "--------SIGNAL LINK STACK TEST--------");
    stack_test(STACK_SIGNAL_LINK_LIST);
    log_msg(LOG_NO_FILE_LINE, "--------DOUBLE LINK STACK TEST--------");
    stack_test(STACK_SIGNAL_LINK_LIST);
    log_msg(LOG_NO_FILE_LINE, "--------CIRCLE LINK STACK TEST--------");
    stack_test(STACK_CIRCLE_LINK_LIST);    
    return OK;
}
static void stack_test(stack_type_t type)
{
    Status rc = OK;
    Int32_t i = 0;
    Stack_funcs_t stk_funcs;
    stack_attr_t stk = NULL;  
    RegisterStackFuncs(&stk_funcs, type, visitnode_stack);
    rc = stk_funcs.init_stack(&stk,type,visitnode_stack);
    if(rc != OK)
    {
        err_quit(LOG_FILE_LINE,"stack_test:stk_funcs.init_stack faled. rc=%d",rc);
    }

    for(i=0; i<5; i++)
    {
        rc = stk_funcs.push(stk, V_INT, &i, sizeof(i));
        if(rc != OK)
        {
            err_ret(LOG_FILE_LINE,"stack_test:push fialed. rc=%d",rc);
            break;
        }
    }
    if(rc == OK)
    {
        Int32_t *p_val = NULL;
        stk_funcs.stack_traverse(stk, visitnode_stack);
        stk_funcs.pop(stk, V_INT, &i, sizeof(i));
        printf("Stack pop data: %d.\n",i);
        printf("After Pop , stack:");
        stk_funcs.stack_traverse(stk, visitnode_stack);
        printf("The top data:");
        stk_funcs.get_top(stk, V_INT,(void **)&p_val, sizeof(*p_val));
        printf("%d.\n", *p_val);
    }
    stk_funcs.clear_stack(stk);
    if(!stk_funcs.stack_empty(stk))
    {
        printf("Stack is Not empty.\n");
    }
    printf("empty stack.\n");    
    stk_funcs.destroy_stack(&stk);
    LogoutStackFuncs(&stk_funcs, type);
}


static Status visitnode_stack(void *val)
{
    assert(val);
    printf("%d",*(Int32_t *)val);
    return OK;
}


