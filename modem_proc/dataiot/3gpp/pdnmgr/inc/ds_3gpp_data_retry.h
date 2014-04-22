#ifndef DS_3GPP_DATA_RETRY_H
#define DS_3GPP_DATA_RETRY_H

/*===========================================================================
                      DS_3GPP_DATA_RETRY.H
DESCRIPTION
This file declares headers for data retry throttling APIs.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_3gpp_data_retry.h#2 $
  $DateTime: 2020/04/22 03:56:17 $$Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd     First checkin
===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "ds_pdn_defs.h"
#include "ds_3gpp_pdn_throttle_sm.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================
FUNCTION  DS_3GPP_DATA_RETRY_POWER_UP_INIT      

DESCRIPTION
  This function performs power up initialization of the Data Retry
  Throttling module

PARAMETERS
  None.
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_data_retry_power_up_init
(
  void 
);

/*===========================================================================
FUNCTION  DS_3GPP_DATA_RETRY_INIT      

DESCRIPTION
  This function performs initialization of the Data Retry
  Throttling module

PARAMETERS
  None.
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_data_retry_init
(
  void
);

/*===========================================================================
FUNCTION  DS_3GPP_DATA_RETRY_IS_ENABLED     

DESCRIPTION
  This function checks whether Data Retry is enabled.

PARAMETERS
  None
 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if enabled
  FALSE, otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_data_retry_is_enabled
(
  void
);

/*===========================================================================
FUNCTION DS_3GPP_DATA_RETRY_HANDLE_REJECT_WITH_T3396_INACTIVE
 
DESCRIPTION
  This function handles the reject case where T3396 timer is inactive
  
PARAMETERS
  1. RAT Based PDN Throt Pointer
  2. Throt Identification pointer
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_data_retry_handle_reject_with_t3396_inactive
(
  ds_3gpp_pdn_throt_rat_based_s      *rat_based_pdn_throt_p,
  ds_3gpp_pdn_throt_ident_info_type  *throt_ident_info_p
);

/*===========================================================================
  FUNCTION DS_3GPP_DATA_RETRY_HANDLE_EMM_CAUSE

  DESCRIPTION
  This function handles service request throttling for PDN Connectivity
  rejects with EMM Cause. 
  
  PARAMETERS
  mode_info_p            - Mode Info
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None. 
===========================================================================*/
void ds_3gpp_data_retry_handle_emm_cause
(
  ds_bearer_deact_params_s         *mode_info_p
);

/*===========================================================================
  FUNCTION ds_pdn_throt_check_backoff_throt_down_reason

  DESCRIPTION
  This function checks if back off throttling can be applied to the input 
  cause code. 
  
  PARAMETERS
  ds_net_down_reason_type : Net Down reason
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None. 
===========================================================================*/
boolean ds_3gpp_data_retry_check_backoff_throt_downreason
(
  ds_net_down_reason_type        down_reason
);

/*===========================================================================
  FUNCTION ds_3gpp_data_retry_random_backoff_check

  DESCRIPTION
  This function checks if random backoff throttling can be applied.
  
  PARAMETERS
  ds_net_down_reason_type   down_reason
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None. 
===========================================================================*/

boolean ds_3gpp_data_retry_random_backoff_check(
  ds_net_down_reason_type        down_reason
);

/*===========================================================================
  FUNCTION    ds_3gpp_data_retry_handle_backoff_throt

  DESCRIPTION 
  This function handles random backoff throttling for relevant cause codes
  across RATS.
  
  PARAMETERS
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None. 
===========================================================================*/
boolean ds_3gpp_data_retry_handle_backoff_throt
(
  ds_pdn_mgr_s                      *pdn_cntx_p,
  ds_bearer_deact_params_s          *mode_info_p,
	sys_sys_mode_e_type                sys_mode,
  ds_net_down_reason_type            down_reason

);

/*===========================================================================
  FUNCTION ds_3gpp_block_lte_plmn_for_backofftime

  DESCRIPTION
  This function blocks PLMN for LTE on hour based schema. 
  
  PARAMETERS
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None. 
===========================================================================*/
void ds_3gpp_block_lte_plmn_for_backofftime ();

/*==========================================================================
  FUNCTION DS_3GPP_DATA_RETRY_INC_FAILURE_CNTR

  DESCRIPTION
  This would increment the failure counter based on data retry spec. 
  
  PARAMETERS
  void* throt_sm_void_ptr,
  void* cb_data
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None. 
===========================================================================*/
void ds_3gpp_data_retry_inc_failure_cntr
(
  void* throt_sm_void_ptr,
  void* cb_data
);


#endif /* DS_3GPP_DATA_RETRY_H */
