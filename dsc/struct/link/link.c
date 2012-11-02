#include <assert.h>
#include <strings.h>
#include "sys_lib.h"
#include "sig_link.h"
#include "dob_link.h"
#include "cic_link.h"
#include "link.h"

void LogoutLinkFuncs(link_funcs_t *funcs)
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

Status RegisterLinkFuncs(link_funcs_t *funcs,link_type_t type, link_opt_funcs_t *ofunc)
{
    Status rc = OK;
    assert(funcs);
    switch(type)
    {
        case SIGNAL_LINK_LIST:
            RegisterLinkFuncs_Sig(funcs, ofunc->visit);
            break;
		case CIRCLE_LINK_LIST:
			RegisterLinkFuncs_Cic(funcs, ofunc->visit);
			break;
        case DOUBLE_LINK_LIST:
            RegisterLinkFuncs_Dob(funcs, ofunc->visit);
            break;
        default: 
            log_msg(LOG_FILE_LINE,"您需要指定一种数据结构类型!\n");
            rc = ERR_PARAMATER;
            break;
    }
    return rc;
}








