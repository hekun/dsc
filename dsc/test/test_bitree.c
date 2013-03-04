#include <assert.h>
#include <string.h>
#include "sys_lib.h"
#include "err_num.h"
#include "bitree.h"
/*
test cases:
124##5##36##7##
12#3##45###
1#2#3##
*/
static Status visitnode_tree(void *val);
static Status BiTreeTest(BiTree_Type_t type);

#define BUFFER_SIZE     65535

Int32_t main(void)
{
    log_msg(LOG_NO_FILE_LINE,"--------BINARY TREE TEST--------");
    //BiTreeTest(THREADED_BINARTY_TREE);
    BiTreeTest(PARENT_BINARY_TREE);
    return OK;
}


static Status BiTreeTest(BiTree_Type_t type)
{
    Status rc = OK;
    Int32_t i = 0;
    Char8_t buffer[BUFFER_SIZE];
    bitree_attr_t tree = NULL;
    queue_attr_t data = NULL;
    bitree_funcs_t bitree_funcs;
    queue_funcs_t queue_funcs;
    memset(buffer,'\0',sizeof(buffer));
    RegisterBiTreeFuncs(&bitree_funcs, type, visitnode_tree);
    RegisterQueueFuncs(&queue_funcs, QUEUE_SIGNAL_LINK_LIST, NULL);
    queue_funcs.init_queue(&data, QUEUE_SIGNAL_LINK_LIST, NULL);
    printf("Input val string:\n");
    scanf("%s",buffer);
    while(buffer[i] != '\0')
    {
        if(buffer[i] == '#')
        {
            rc = queue_funcs.en_queue(data, V_UNKNOWN_TYPE, NULL, 0);
            if(rc != OK)
            {
                err_ret(LOG_FILE_LINE,"en_queue failed.rc=%d.",rc);
                break;
            }
        }
        else if(buffer[i] != ' ')
        {
            rc = queue_funcs.en_queue(data, V_CHAR, &buffer[i], sizeof(i));
            if(rc != OK)
            {
                err_ret(LOG_FILE_LINE,"en_queue failed.rc=%d.",rc);
                break;
            }
        }
        i++;
    }
    do
    {
        rc = bitree_funcs.create_bitree(&tree,PRE_ORDER, data, &queue_funcs);
        if(rc != OK)
        {
            err_ret(LOG_NO_FILE_LINE,"create binary tree failed. rc=%d.",rc);
            break;
        }
        log_msg(LOG_NO_FILE_LINE, "Binary tree pre Order Unrecursion visit:");
        bitree_funcs.set_visit_order(tree, PRE_ORDER);
        bitree_funcs.traverse_bitree(tree, visitnode_tree);
        
        log_msg(LOG_NO_FILE_LINE, "Binary tree In Order Unrecursion visit:");
        bitree_funcs.set_visit_order(tree, IN_ORDER);
        bitree_funcs.traverse_bitree(tree, visitnode_tree);
        
        log_msg(LOG_NO_FILE_LINE, "Binary tree Post Order Unrecursion visit:");
        bitree_funcs.set_visit_order(tree, POST_ORDER);
        bitree_funcs.traverse_bitree(tree, visitnode_tree);

        
    }while(0);
    bitree_funcs.destroy_bitree(&tree);
    queue_funcs.destroy_queue(&data);
    
    LogoutqueueFuncs(&queue_funcs, type);
    LogoutBiTreeFuncs(&bitree_funcs);
    return rc;
}

static Status visitnode_tree(void *val)
{
    if(!val)
    {
        printf("NULL\n");
    }
    else
    {
        printf("%c\n",*(Char8_t *)val);
    }
    
    return OK;

}


