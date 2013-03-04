#include <assert.h>
#include <strings.h>
#include "err_num.h"
#include "sys_lib.h"
#include "bitree.h"
//#include "bitree_threaded.h"
#include "bitree_unrec.h"

Status RegisterBiTreeFuncs(bitree_funcs_t *funcs, BiTree_Type_t type, tree_visit visit)
{
    assert(funcs);
    Status rc = OK;
    switch(type)
    {
        //case THREADED_BINARTY_TREE:
        //    RegisterBiTreeFuncs_threaded(funcs, visit);
        //    break;
        case PARENT_BINARY_TREE:
            RegisterBiTreeFuncs_Unrec(funcs, visit);
            break;
        default:
            rc = ERR_PARAMATER;
            break;
    }
    return rc;
}
void LogoutBiTreeFuncs(bitree_funcs_t *funcs)
{
    assert(funcs);
    if(funcs == NULL)
    {
        log_msg(LOG_NO_FILE_LINE,"LogoutBiTreeFuncs:Binary tree class has freeed.");
    }

    else
    {
        bzero(funcs,sizeof(*funcs));
    }
}

