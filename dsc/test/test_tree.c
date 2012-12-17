#include <assert.h>
#include <string.h>
#include "sys_lib.h"
#include "err_num.h"
#include "tree.h"
/*124##5##36##7##*/
static Status visitnode_tree(void *val);
static Status test_tree(tree_type_t type);

#define BUFFER_SIZE     65535

Int32_t main(void)
{
    log_msg(LOG_NO_FILE_LINE,"--------BINARY TREE TEST--------");
    test_tree(BINART_TREE);
    return OK;
}


static Status test_tree(tree_type_t type)
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
    RegisterQueueFuncs(&queue_funcs, type, NULL);
    queue_funcs.init_queue(&data, type, NULL);
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
        rc = tree_funcs.preorder_unrecursion(root, visitnode_tree);
        if(rc != OK)
        {
            err_ret(LOG_NO_FILE_LINE,"Perorder bintary tree failed. rc=%d.",rc);
            break;
        }
    }while(0);
    tree_funcs.destroy_tree(&root);
    queue_funcs.destroy_queue(&data);
    
    LogoutqueueFuncs(&queue_funcs, type);
    LogoutTreeFuncs(&tree_funcs);
    return rc;
}

static Status visitnode_tree(void *val)
{
    assert(val);
    printf("%d",*(Int32_t *)val);
    return OK;

}


