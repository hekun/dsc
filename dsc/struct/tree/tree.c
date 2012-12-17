#include <assert.h>
#include <string.h>
#include "tree.h"
#include "bi_tree.h"


Status RegisterTreeFuncs(tree_funcs_t * funcs, tree_type_t type, tree_visit visit)
{
    assert(funcs);
    switch(type)
    {
        case BINART_TREE:
            RegisterTreeFuncs_Binary(funcs, visit);
            break;
        default:
            log_msg(LOG_NO_FILE_LINE,"the 2th parameter error.");
            break;
    }
}
Status LogoutTreeFuncs(tree_funcs_t* funcs)
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

