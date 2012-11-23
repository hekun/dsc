#include <assert.h>
#include <strings.h>
#include "sys_lib.h"
#include "binary_tree.h"
#include "tree.h"


void RegisterTreeFuncs(Tree_funcs_t *funcs, tree_type_t type, tree_opt_funcs_t *ofunc)
{
    Status rc = OK;
     assert(funcs);
     switch(type)
     {
         case BINARY_TREE:
             RegisterTreeFuncs_Binary(funcs, type, ofunc->visit);
             break;
         default: 
             log_msg(LOG_FILE_LINE,"您需要指定一种数据结构类型!\n");
             rc = ERR_PARAMATER;
             break;
     }
     return rc;

}

void LogoutTreeFuncs(Tree_funcs_t *funcs)
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


