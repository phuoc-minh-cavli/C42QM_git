
/*===========================================================================

* Copyright (c) 2012 Qualcomm Technologies Incorporated.  
* All Rights Reserved.
* QUALCOMM Proprietary and Confidential.
*
*                      EDIT HISTORY FOR FILE
*                      
*  This section contains comments describing changes made to the module.
*  Notice that changes are listed in reverse chronological order.
*  
*
*
* when       who     what, where, why
* --------       ---       ---------------------------------------------------------
* 1/28/19   Hema   Intial version for Tiny modem
  ===========================================================================*/



#include "corebsp_rpm.h"



mpss_rpm_handle_t* mpss_kvp_init(rpm_resource_type resource)
{
    mpss_rpm_handle_t * handle = (mpss_rpm_handle_t*)malloc(sizeof(mpss_rpm_handle_t));
   handle->resource = resource;
    handle->kvp = kvp_create(0);   
    return handle;
}

void mpss_kvp_deinit(mpss_rpm_handle_t* handle)
{
    kvp_destroy(handle->kvp);      
    handle->resource = 0;
    free(handle);
}


void  mpss_rpm_resource_request(mpss_rpm_handle_t* handle ,resource_state state ,resource_kvp_type value  )
{
    int rpm_msg_id = 0;
    
    kvp_clear(handle->kvp);
    kvp_put(handle->kvp , value , sizeof(state),(void*)&state);
    
    rpm_msg_id = rpm_post_request(RPM_ACTIVE_SET, handle->resource , 0, handle->kvp);
    
    if(rpm_msg_id > 0)
    {
        rpm_barrier(rpm_msg_id);
    }
}