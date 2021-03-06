#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "err_num.h"
#include "sys_lib.h"
#include "v_data.h"

/*
功能描述:
    初始化抽象数据空间。
参数说明:
    
返回值:

作者:
    He kun
日期:
    2012-12-06
注意事项:
    1.创建一个无实际数据的抽象数据空间的方法。
    v_data_t *data=NULL;
    init_vdata(&data, V_UNKNOWN_TYPE, NULL, 0);
    2.若type=V_POINT类型，data->val存储指针值。
    如果type!=V_POINT类型，data->val会指向一个新建的堆空间，并将实际数据
    复制到该堆空间中。若没有实际数据，则不建立堆空间。
    即等价于init_vdata(&data, V_UNKNOWN_TYPE, NULL, 0);
*/

Status init_vdata(v_data_t **vdata, v_type_t type,
                void *val, size_t val_size)
{
    Status rc = OK;
    v_data_t *tmp_vdata = *vdata;
    if(tmp_vdata == NULL)
    {
        rc = Malloc((void * *) &tmp_vdata, sizeof(v_data_t));
        if(rc != OK)
        {
            err_ret(LOG_FILE_LINE, "Malloc failed.");
            return ERROR;
        }
#ifdef _DEBUG
        log_msg(LOG_NO_FILE_LINE, "Malloc v_Data");
#endif
        tmp_vdata->type = V_UNKNOWN_TYPE;
        tmp_vdata->val = NULL;
        tmp_vdata->val_size = 0;
    }
    else
    {
        log_msg(LOG_NO_FILE_LINE, "init_vdata exist.");
    }
    switch(type)
    {
        case V_INT:
        case V_UNSIGNED_INT:
        case V_CHAR:
        case V_UNSIGNED_CHAR:
        case V_LONG:
        case V_SHORT:
        case V_UNSIGNED_SHORT:
        case V_UNSIGNED_LONG:
        case V_FLOAT:
        case V_DOUBLE:
        case V_LONG_DOUBLE:
        case V_UNSIGNED_LONG_DOUBLE:
        case V_CUSTOM:
            if((val != NULL) && (val_size > 0))
            {
                tmp_vdata->val_size = val_size;
                rc = Malloc((void * *) &(tmp_vdata->val), val_size);
                if(rc != OK)
                {
                    err_ret(LOG_FILE_LINE, "Malloc failed.");
                    return ERROR;
                }
#ifdef _DEBUG
                log_msg(LOG_NO_FILE_LINE, "Malloc vdata->val");
#endif
                Memcpy(tmp_vdata->val, val, tmp_vdata->val_size, val_size);
            }
            break;
        case V_POINT:
            tmp_vdata->val = val;
            tmp_vdata->val_size = val_size;
            break;
        case V_UNKNOWN_TYPE: break;
        default: break;
    }
    tmp_vdata->type = type;
    *vdata = tmp_vdata;
    return OK;
}

/*
功能说明:
    销毁抽象数据空间。
作者:
    He kun
日期:
    2012-12-06
注意事项:
    若存储有实际数据，需要在销毁前获取指针值。
    否则实际数据会被销毁。
*/
void destroy_vdata(v_data_t **vdata)
{
    if(!empty_vdata(*vdata) && ((*vdata)->type != V_POINT))
    {
        Free((void * *)&(*vdata)->val);
#ifdef _DEBUG
        log_msg(LOG_NO_FILE_LINE, "FREE vdata->val");
#endif
    }
    (*vdata)->type = V_UNKNOWN_TYPE;
    (*vdata)->val_size = 0;
    Free((void * *)vdata);
#ifdef _DEBUG
    log_msg(LOG_NO_FILE_LINE, "FREE vdata");
#endif
}
/*
功能描述:
    向抽象数据类型中赋值。
参数说明:

返回值:

作者:
    He kun
日期:
    2012-12-06
注意事项:

*/

Status set_vdata(v_data_t *vdata, v_type_t type,
                      void *pdata, size_t data_size)
{
    assert(type != V_UNKNOWN_TYPE);
    Status rc = OK;
    void *new_space = NULL;

    //vdata->type=V_UNKNOWN_TYPE,且type!=V_UNKNOWN_TYPE.
    //建立抽象数据类型。并赋予新值。
    if(vdata->type == V_UNKNOWN_TYPE && type != V_UNKNOWN_TYPE)
    {
        if((vdata->val != NULL) || (vdata->val_size != 0))
        {
            err_ret(LOG_FILE_LINE, "vdata error.");
            return ERROR;
        }
        vdata->val_size = data_size;
        vdata->type = type;
        if(type == V_POINT)
        {
            vdata->val = pdata;
        }
        else if(pdata && data_size > 0)
        {
            rc = Malloc((void * *) &(vdata->val), data_size);
            if(rc != OK)
            {
                err_ret(LOG_FILE_LINE, "Malloc failed.");
                return rc;
            }
#ifdef _DEBUG
            log_msg(LOG_NO_FILE_LINE, "Malloc vdata->val");
#endif	
            rc = Memcpy(vdata->val, pdata, vdata->val_size, data_size);
            if(rc != OK)
            {
                Free((void * *) &vdata->val);
                err_ret(LOG_FILE_LINE, "Malloc failed.");
                return rc;
            } 
        }
        else
        {
            err_ret(LOG_FILE_LINE,"val , data_size error.");
            return rc;
        }

    }
    //当vdata->type != V_UNKNOWN_TYPE时，则赋值前提是两类型一致。
    else if(vdata->type == type)
    {
        //对于指针类型，不必创建val指向的空间。直接存储该地址即可。
        if(type == V_POINT)
        {
            vdata->val = pdata;
            vdata->val_size = data_size;            
        }
        //对于非指针类型，在init_vdata时已经创建。
        //如果init_vdata时创建的空间不足时，调用realloc调整内存空间大小后在复制。
        else    
        {
            if(vdata->val_size < data_size) 
            {
                new_space = Realloc(vdata->val, vdata->val_size, data_size);
                if(new_space == vdata->val)
                {
                    err_ret(LOG_FILE_LINE, "Realloc failed.");
                    return ERROR;
                }  
            }
            vdata->val_size = data_size;
            rc = Memcpy(vdata->val, pdata, vdata->val_size, data_size);
            if(rc != OK)
            {
                Free((void * *) &vdata->val);
                err_ret(LOG_FILE_LINE, "Malloc failed.");
                return rc;
            } 
        }
    }
    else
    {
        err_ret(LOG_FILE_LINE, "Type error");
        return ERR_PARAMATER;
    }
    return OK;
}

void * get_vdata(v_data_t *vdata)
{
    if(!vdata)
    {
        return NULL;
    }
	if((vdata->val != NULL) && (vdata->val_size >0) && (vdata->type != V_UNKNOWN_TYPE))
	{
    	return vdata->val;
	}
	return NULL;
}


Status compare_vdata(v_data_t *vdata1, v_data_t *vdata2)
{
    Status rc = UN_EQUAL;
    if(vdata1 == vdata2 && vdata1->type == vdata2->type
        && vdata1->val == vdata2->val 
        && vdata1->val_size == vdata2->val_size)
    {
        rc = EQUAL;
    }
    return rc;
}

Status empty_vdata(v_data_t *vdata)
{
    Status rc = FALSE;
    if(vdata && vdata->type != V_UNKNOWN_TYPE
        && !vdata->val && vdata->val_size == 0)
    {
        rc = TRUE;
    }
    return rc;
}





