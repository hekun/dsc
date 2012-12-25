#include <assert.h>
#include <string.h>
#include "sys_lib.h"
#include "err_num.h"
#include "tree.h"
/*124##5##36##7##*/
static Status visitnode_tree(void *val);
static Status tree_test(tree_type_t type);

#define BUFFER_SIZE     65535

Int32_t main(void)
{
    log_msg(LOG_NO_FILE_LINE,"--------BINARY TREE TEST--------");
    tree_test(BINART_TREE);
    return OK;
}


static Status tree_test(tree_type_t type)
{
    Status rc = OK;
    Int32_t i = 0;
    Char8_t buffer[BUFFER_SIZE];
    tree_attr_t root = NULL;
    queue_attr_t data = NULL;
    tree_funcs_t tree_funcs;
    queue_funcs_t queue_funcs;
    memset(buffer,'\0',sizeof(buffer));
    RegisterTreeFuncs(&tree_funcs, type, visitnode_tree);
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
            rc = queue_funcs.en_queue(data, V_INT, &buffer[i], sizeof(i));
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
        rc = tree_funcs.create_tree(&root, data, &queue_funcs);
        if(rc != OK)
        {
            err_ret(LOG_NO_FILE_LINE,"create binary tree failed. rc=%d.",rc);
            break;
        }
        log_msg(LOG_NO_FILE_LINE, "Binary tree Pre Order recursion:");
        tree_funcs.preorder_recursion(root, visitnode_tree);
        log_msg(LOG_NO_FILE_LINE, "Binary tree mid Order recursion:");
        tree_funcs.midorder_recursion(root, visitnode_tree);
        log_msg(LOG_NO_FILE_LINE, "Binary tree Post Order recursion:");
        tree_funcs.postorder_recusion(root, visitnode_tree);
        log_msg(LOG_NO_FILE_LINE, "Binary tree level Order:");
        tree_funcs.level_order(root, visitnode_tree);


    }while(0);
    tree_funcs.destroy_tree(&root);
    queue_funcs.destroy_queue(&data);
    
    LogoutqueueFuncs(&queue_funcs, type);
    LogoutTreeFuncs(&tree_funcs);
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


