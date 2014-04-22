/*===========================================================================

  Copyright (c) 2019 Qualcomm Technologies Incorporated.  
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef COREBSP_RPM_H
#define COREBSP_RPM_H

#include <stdlib.h>

#include "rpm.h"
#include "rpmclient.h"
#include "kvp.h"


typedef struct
{
    rpm_resource_type resource;
    kvp_t *kvp;
}mpss_rpm_handle_t;

/*Add new key value pair as per need*/
typedef enum
{
    /*KEY  <========================>Measuring param in  ASCII*/
    KEY_MICRO_VOLT                              = 0x7675,  //uv - voltage
    CLK_KEY_KHZ                                      = 0x007A484B,  // 'KHz'
    KEY_SOFTWARE_ENABLE                    = 0x6E657773 ,// swen - software enable
}resource_kvp_type;


typedef unsigned int resource_state;

/*
  *    Input param:
  *    
  *      resource_state        : New Value/state to be set to the  resource 
  *      resource_kvp_type  : Value measuring param 
  *
  *      THIS API CAN BE USED TO VOTE FOR RPM HANDLED RESOURCES WHERE NPA IS UNAVAILABLE
  *      FOR EXAMPLE PAGE ONLY MODEM IMAGE
  */
void  mpss_rpm_resource_request(mpss_rpm_handle_t* handle ,resource_state state ,resource_kvp_type value  );

void mpss_kvp_deinit(mpss_rpm_handle_t* handle);

mpss_rpm_handle_t* mpss_kvp_init(rpm_resource_type resource);

#endif // COREBSP_RPM_H

