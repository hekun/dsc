#include <assert.h>
#include "v_data.h"
#include "sys_lib.h"
#include "queue.h"


static Status visitnode_queue(void *val);
static void queue_test(queue_type_t type);


Int32_t main(void)
{
    log_msg(LOG_NO_FILE_LINE, "--------SIGNAL LINK QUEUE TEST--------");
    queue_test(QUEUE_SIGNAL_LINK_LIST);
    //log_msg(LOG_NO_FILE_LINE, "--------DOUBLE LINK QUEUE TEST--------");
    //queue_test(QUEUE_SIGNAL_LINK_LIST);
    //log_msg(LOG_NO_FILE_LINE, "--------CIRCLE LINK QUEUE TEST--------");
    //queue_test(QUEUE_CIRCLE_LINK_LIST);    
    return OK;
}

static void queue_test(queue_type_t type)
{
    Status rc = OK;
    Int32_t i = 0;
    queue_funcs_t q_funcs;
    queue_attr_t Q = NULL;
    RegisterQueueFuncs(&q_funcs,type, visitnode_queue);
    rc = q_funcs.init_queue(&Q, type, visitnode_queue);
    if(rc != OK)
    {
        err_quit(LOG_FILE_LINE,"queue_test:q_funcs.init_queue failed.rc=%d",rc);
    }
    for(i=0; i<5; i++)
    {
        rc = q_funcs.en_queue(Q,V_INT,&i,sizeof(i));
        if(rc != OK)
        {
            err_ret(LOG_FILE_LINE,"queue_test:en_queue fialed. rc=%d",rc);
            break;
        }
    }  
    if(rc == OK)
    {
        Int32_t *p_val = NULL;
        q_funcs.queue_traverse(Q, visitnode_queue);
        q_funcs.de_queue(Q, V_INT, &i, sizeof(i));
        printf("Queue delete data: %d.\n", i);
        printf("After delete, queue:");
        q_funcs.queue_traverse(Q, visitnode_queue);
        printf("The top data:");
        q_funcs.get_head(Q, V_INT, (void **)&p_val, sizeof(*p_val));
        printf("%d.\n",*p_val);
    }
    q_funcs.clear_queue(Q);
    if(!q_funcs.queue_empty(Q))
    {
        printf("Queue is not NULL.\n");
    }
    else
    {
        printf("Empty queue.\n");
    }
    
    q_funcs.destroy_queue(&Q);
    LogoutqueueFuncs(&q_funcs, type);
}


static Status visitnode_queue(void *val)
{
    assert(val);
    printf("%d",*(Int32_t *)val);
    return OK;
}


