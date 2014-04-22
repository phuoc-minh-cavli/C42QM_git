/*===========================================================================
                      DS_3GPP_CC_SP_THROT_SM.H
DESCRIPTION
This file defines cc sp throttling APIs.

Copyright (c) 2022 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //depot/SBA/users/lteswint/ebf/mpl/BF-ENG-FR_56024-MPSS.CT.1.0-00681-9205_LTENBGE_TEST-2/modem_proc/dataiot/3gpp/pdnmgr/src/ds_3gpp_dam.c#1 $
  $DateTime: 2022/06/28 06:42:25 $$Author: lteswint $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/06/22    pbajaj     First checkin
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/


#include "ds_pdn_defs.h"


#ifdef FEATURE_DATA_CC_SP_THROTTLING

/*===========================================================================

                           MACRO DEFINITIONS

===========================================================================*/
#define DS_3GPP_CC_SP_THROT_MAX_FAIL_COUNT 3
#define DS_3GPP_CC_SP_THROT_2_HOURS_TIMER  7200*1000  /*in milliseconds*/  
#define DS_3GPP_CC_SP_THROT_TIMER          10

/*===========================================================================
FUNCTION      DS_3GPP_CC_SP_THROT_READ_CONFIG_FROM_EFS

DESCRIPTION   This function will read from the efs file all the necessary 
              data & fill the PDN throttle config structure. Only valid data 
              is populated.

              EFS File Format - Param_Name:Param_Val;
              For example     - FAILURE_TIMER_1:0;   
 
PARAMATERS   None 
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp_cc_sp_throt_read_config_from_efs
(
  void
);

/*===========================================================================
FUNCTION      DS_3GPP_CC_SP_THROT_GET_TIMER 

DESCRIPTION   This function provides the PDN throttling failure timer value

PARAMETERS    counter - Throttling counter used as an index

RETURN VALUE  PDN throttle Failure Timer value in msec

SIDE EFFECTS  None
===========================================================================*/
uint32 ds_3gpp_cc_sp_throt_get_timer 
(
  uint16 cause_code, 
  uint8 counter
);


/*===========================================================================
FUNCTION      DS_3GPP_CC_SP_THROT_GET_TOKEN_ID

DESCRIPTION   This function returns the token id associated with each pdn
              throttle config parameter. 

PARAMETERS    char *from - start of the token (param name)
              char *to   - end of the token (param name)
              *token_id  - Pointer in which Token ID is returned

DEPENDENCIES  None

RETURN VALUE  0  - success
              -1 - failure

SIDE EFFECTS  None
===========================================================================*/
int16 ds_3gpp_cc_sp_throt_get_token_id
(  
  char *from,
  char *to,
  uint8 *token_id
);

/*===========================================================================
FUNCTION      DS_3GPP_CC_SP_THROT_IS_CAUSE_CODE_CONFIGURED

DESCRIPTION   This function return true if given cause code is configured in 
              cc_sp_throttle_timer.txt file

PARAMETERS    cause code - PDP activation reject cause

RETURN VALUE  true- if cause code configured, false otherwise

SIDE EFFECTS  None
SIDE EFFECTS  None
===========================================================================*/
boolean ds_3gpp_cc_sp_throt_is_cause_code_configured
(
  uint16 cause_code
);

/*===========================================================================
FUNCTION      DS_3GPP_CC_SP_THROT_SET_DEFAULT_CONFIG

DESCRIPTION   This function used to set the PDN throttling failure timer value

PARAMETERS    counter - Throttling counter used as an index

RETURN VALUE  PDN throttle Failure Timer value in msec

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp_cc_sp_throt_set_default_config
(
  void
);

/*===========================================================================
FUNCTION      DS_3GPP_CC_SP_THROT_GET_PLMN_UNBLOCK_TIMER_VAL

DESCRIPTION   This function provides the plmn unblock timer value
              configured in EFS

PARAMETERS    

RETURN VALUE  plmn unblock timer value in msecs

SIDE EFFECTS  None
===========================================================================*/
uint32 ds_3gpp_cc_sp_throt_get_plmn_unblock_timer_val
(
   void
);

/*===========================================================================
FUNCTION      DS_3GPP_CC_SP_SET_PLMN_UNBLOCK_TIMER_VAL

DESCRIPTION   This function used to plmn unblock timer configured in EFS.

DEPENDENCIES  None.

RETURN VALUE  Integer value
             
SIDE EFFECTS  None.
===========================================================================*/
void ds_3gpp_cc_sp_set_plmn_unblock_timer_val
(
   uint32 timer_val
);

#ifdef FEATURE_DATA_LTE 
/*===========================================================================
FUNCTION DS_3GPP_CC_SP_INIT

DESCRIPTION
  This function does the powerup initialization for the CC SP module.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_cc_sp_init
(
   void
);/*ds_3gpp_cc_sp_init*/

/*===========================================================================
FUNCTION DS_3GPP_THROT_ENABLE_CC_SP_THROTTLING

DESCRIPTION
  This function enables cc_sp_throttling

PARAMETERS  
  Flag - TRUE / FALSE
 
DEPENDENCIES 
  None.
  
RETURN VALUE 
boolean
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_throt_sm_set_cc_sp_throttling
(
  boolean                 value
);

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_IS_CC_SP_THROTTLING_ENABLED

DESCRIPTION
  This function returns the NV item value of the cc_sp_throttling

PARAMETERS  
None
DEPENDENCIES 
  None.
  
RETURN VALUE 
boolean
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_throt_sm_is_cc_sp_throttling_enabled
(
  void
);

/*===========================================================================
FUNCTION DS_3GPP_CC_SP_INFO_TBL_PER_SUBS_INIT

DESCRIPTION
  This function initializes throttle info tbl to get timer value

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_cc_sp_info_tbl_init
(
  void
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_CLR_CC_SP_T3402_TIMER

DESCRIPTION
  This function is used to clear T3402 timer running for CC SP
  

PARAMETERS: 
  None.
 
DEPENDENCIES
  None.

RETURN VALUE
 
SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_cc_sp_clr_timer
(
  void
);

/*===========================================================================
FUNCTION  DS_3GPP_PDN_CLEAR_CC_SP_THROT_STATE_FOR_APN_ON_CURRENT_PLMN

DESCRIPTION
  This function is used to clear CC SP throttling of the matching APN
  on the current PLMN
  

PARAMETERS: 
  None.
 
DEPENDENCIES
  None.

RETURN VALUE
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_clear_cc_sp_throt_state_for_apn_on_current_plmn
(
  byte                   *apn
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SET_CCSP_PLMN_BLOCK_TIMER

DESCRIPTION
  This function is used to set 2 HR PLMN blocking timer.
  This is called in UMTS when a PLMN is blocked on LTE and on UMTS
  infinite throttling occurs.

PARAMETERS: 
  None.
 
DEPENDENCIES
  None.

RETURN VALUE
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_cc_sp_throt_set_plmn_unblock_timer
(
  uint32                 throt_tmr_val
);

/*===========================================================================
FUNCTION DS_EPS_PDN_THROT_HANDLE_PDN_CONN_REJ
 
DESCRIPTION
  This function gets called when attach is rejected and given cause code is present in EFS configuration file
  
PARAMETERS   : 
  1. rejected_pdn_cntx_p 
  
DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_cc_sp_throt_handle_pdn_conn_rej
( 
  ds_pdn_mgr_s                *rejected_pdn_mgr_p, 
  esm_cause_enum_T                 esm_cause,
  ds_net_down_reason_type    down_reason
);


/*===========================================================================
FUNCTION  DS_EPS_PDN_THROT_T3402_TIMER_EXP_CB

DESCRIPTION
  This function posts a cmd to take specific action based on tr3402 timer expiry

PARAMETERS
   call back data ptr - which points to t3402 timer table            
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_pdn_throt_t3402_timer_exp_cb
(
     unsigned long callback_data 
);

/*===========================================================================
FUNCTION  DS_EPS_PDN_THROT_T3402_TIMER_EXPIRY_CMD_HDLR

DESCRIPTION
  This function resets the sm on timer expiry

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/
void ds_cc_sp_throt_t3402_timer_expiry_cmd_hdlr
(
  unsigned long client_data_ptr
);

/*============================================================================
FUNCTION      DS_3GPP_CC_SP_RESET_THROTTLING

DESCRIPTION
  This function takes care of clearing CC_SP throttling information as
  part of timer expiry/RAU/TAU update. It also takes care of informing
  external clients with the ublocked APNs
  
PARAMETERS  
  callback_data - points to the  throttling state machine.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None
  
===========================================================================*/
void ds_3gpp_cc_sp_reset_throttling
(
  void
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_IS_ANY_APN_THROTTLED_FOR_MAX_FAILURE

DESCRIPTION
  This function is used to check of any APN is throttled for max count 
  

PARAMETERS: 
  None.
 
DEPENDENCIES
  None.

RETURN VALUE
  true - if any APN is throttled and fail count reached maximum
   flase - otherwise.
 
SIDE EFFECTS
  None.

===========================================================================*/

boolean ds_3gpp_cc_sp_throt_is_any_apn_throttled_for_max_failure
(
  void
);

/*===========================================================================
FUNCTION DS_3GPP_CC_SP_IS_PLMN_UNBLOCK_TIMER_RUNNING

DESCRIPTION
  This function is used to check if plmn unblock timer is running

PARAMETERS: 
  subs_id.
 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE/FALSE
 
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_cc_sp_is_plmn_unblock_timer_running
(
  void
);
#endif/*FEATURE_LTE*/
#endif /* #ifdef FEATURE_DATA_CC_SP_THROTTLING */
