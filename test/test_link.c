#include <assert.h>
#include "g_type.h"
#include "sys_lib.h"
#include "v_data.h"
#include "sys_lib.h"
#include "link.h"


static void link_test(link_type_t type);

static Status visitnode_link(void *val);

Int32_t main(void)
{
    log_msg(LOG_NO_FILE_LINE, "--------SIGNAL LINK TEST--------");
    link_test(SIGNAL_LINK_LIST);
    return OK;
}
static void link_test(link_type_t type)
{
    Status rc = OK;
    Int32_t i = 0;
    link_funcs_t funcs;
    link_opt_funcs_t optf;
    optf.visit = visitnode_link;
    RegisterLinkFuncs(&funcs, type, &optf);    
    link_attr_t link1 = NULL;
    rc = funcs.init_link(&link1);
    if(rc != OK)
    {
        err_quit(LOG_FILE_LINE,"funcs.init_link failed. rc=%d",rc);
    }
    for(i=0; i<5; i++)
    {
        rc = funcs.insert_first_data(link1,V_INT,(void *)&i,sizeof(i));
        if(rc != OK)
        {
            funcs.destroy_link(&link1);
            err_quit(LOG_FILE_LINE,"funcs.insert_first_data,rc=%d.",rc);
        }
    }
    printf("link1 list:\n");
    funcs.link_traverse(link1,visitnode_link);
    funcs.destroy_link(&link1);
    funcs.
    LogoutLinkFuncs(&funcs);

}



static Status visitnode_link(void *val)
{
    assert(val);
    printf(" %d",*(Int32_t *)val);
    return OK;
}



