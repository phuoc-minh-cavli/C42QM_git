#ifndef DS_3GPP_SRV_REQ_THROT_CONFIG_H
#define DS_3GPP_SRV_REQ_THROT_CONFIG_H
/*===========================================================================
                      DS_3GPP_SRV_REQ_THROTTLING_CONFIG.H
DESCRIPTION
This file defines service request throttling config header files.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_3gpp_srv_req_throt_config.h#1 $
  $DateTime: 2020/03/17 08:03:16 $$Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd     First checkin
===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "ds_pdn_defs.h"


/*===========================================================================
FUNCTION      DS_3GPP_SRV_REQ_READ_THROTTLE_CONFIG_FROM_EFS

DESCRIPTION   This function will read from the efs file all the necessary 
              data & fill the srv req throttle config structure per subscription.
              Only valid data is populated.

              EFS File Format - Param_Name:Failure_Value:Min_Random_Timer:MAx_Random_Timer;
              For example     - FAILURE_TIMER_1:0:2:3;
 
PARAMATERS   None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp_srv_req_read_throttle_config_from_efs
(
  void
);

/*===========================================================================
FUNCTION DS_3GPP_SRV_REQ_THROT_GET_TIMER_CB

DESCRIPTION
  This function is used to get Service request throttling timer value

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  Throttle Timer value

SIDE EFFECTS
  None.
===========================================================================*/
uint32 ds_3gpp_srv_req_throt_get_throt_timer_cb
(
  void* throt_sm_void_ptr,
  void* cb_data
);

#ifdef TEST_FRAMEWORK
#error code not present
#endif
#endif /* DS_3GPP_SRV_REQ_THROT_CONFIG_H */
