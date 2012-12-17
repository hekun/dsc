#include <assert.h>
#include <string.h>
#include "sys_lib.h"
#include "bi_tree.h"
#include "tree.h"


Status RegisterTreeFuncs(tree_funcs_t * funcs, tree_type_t type, tree_visit visit)
{
    assert(funcs);
    Status rc = OK;
    switch(type)
    {
        case BINART_TREE:
            RegisterTreeFuncs_Binary(funcs, visit);
            break;
        default:
            log_msg(LOG_NO_FILE_LINE,"the 2th parameter error.");
            rc = ERROR;
            break;
    }
    return rc;
}


void LogoutTreeFuncs(tree_funcs_t* funcs)
{
    if(funcs == NULL)
    {
        log_msg(LOG_NO_FILE_LINE,"LogoutLinkFuncs:Link list class is free.");
    }

    else
    {
        bzero(funcs,sizeof(*funcs));
    }
    
}

