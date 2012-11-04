#include "v_data.h"
#include "sys_lib.h"
#include "stack.h"


static Status visitnode_stack(void *val);


Int32_t main(void)
{
    //log_msg(LOG_NO_FILE_LINE, "--------SIGNAL LINK STACK TEST--------");
    //stack_test(SIGNAL_LINK_LIST);
    log_msg(LOG_NO_FILE_LINE, "--------DOUBLE LINK STACK TEST--------");
    stack_test(STACK_SIGNAL_LINK_LIST);
    //log_msg(LOG_NO_FILE_LINE, "--------CIRCLE LINK STACK TEST--------");
    //stack_test(CIRCLE_LINK_LIST);    
    return OK;
}
static void stack_test(stack_type_t type)
{
    Status rc = OK;
    Stack_funcs_t stk_funcs;
    stack_attr_t stk = NULL;  
    RegisterStackFuncs(&stk_funcs, type, visitnode_stack);
    rc = stk_funcs.init_stack(&stk,type,visitnode_stack);
    if(rc != OK)
    {
        err_quit(LOG_FILE_LINE,"stack_test:stk_funcs.init_stack faled. rc=%d",rc);
    }
    stk_funcs.destroy_stack(&stk);
    LogoutStackFuncs(&stk_funcs, type);
}


static Status visitnode_stack(void *val)
{
    assert(val);
    printf("%d",*(Int32_t *)val);
    return OK;
}


