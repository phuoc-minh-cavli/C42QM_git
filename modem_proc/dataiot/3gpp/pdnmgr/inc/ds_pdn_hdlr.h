#ifndef DS_PDN_HDLR_H
#define DS_PDN_HDLR_H

/*===========================================================================
                      DS_PDN_HDLR.H
DESCRIPTION
This file exposes PDN handler through APIs and data structures.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_pdn_hdlr.h#1 $
  $DateTime: 2020/03/17 08:03:16 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "ds_pdn_defs.h"
#include "ds_3gpp_throttle_sm.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define DS_PDN_AC_CLASS_12_to_14    0x70
#define DS_PDN_AC_CLASS_11_or_15    0x88


typedef enum
{
  DS_PDN_LTE_NOT_SET = 0,
  DS_PDN_LTE_DISABLED,
  DS_PDN_LTE_ENABLED,
  DS_PDN_LTE_EV_MAX
}ds_pdn_lte_status;

/*===========================================================================
TYPEDEF ds_pdn_t3346_status_type

DESCRIPTION
  This is the type that holds times t3346 status sent by cm
===========================================================================*/
typedef struct
{

  sys_nas_t3346_status_e_type  t3346_status;
  /** < Indicate the status of timer T3346  */

  sys_sys_mode_e_type          active_rat;
  /** < Indicates the RAT for which timer t3346 status is sent  */

} ds_pdn_t3346_status_type;

/*-----------------------------------------------------------------------------
  This structure contains all UE level information for 3GPP
-----------------------------------------------------------------------------*/
typedef struct
{
  boolean                           attach_is_initiated; 
  boolean                           attach_is_complete;
  sys_sys_mode_e_type               internal_sys_mode; 
  sys_sys_mode_e_type               last_full_srv_mode;
  boolean                           data_suspend_flag;
  boolean                           sent_ps_data_available;

/*---------------------------------------------------------------------------
Structure to track the t3346 status from cm
---------------------------------------------------------------------------*/
  ds_pdn_t3346_status_type         t3346_status;
  
  ds_pdn_lte_status                 current_ds_lte_flag;

  sys_plmn_id_s_type                current_plmn;

  sys_neg_ciot_cap                  ciot_cap;  // CIOT capability info
/*---------------------------------------------------------------------------
  Below list tracks the blocked PLMNs at any given point. If a PLMN 
  gets unblocked it would be removed from this list.
---------------------------------------------------------------------------*/
  list_type                         blocked_plmn_list;
  struct
  {
    uint8                           max_simul_active_pdp_context;
    sys_plmn_id_s_type              plmn_id;
  }max_simul_active_pdp_info[DS_3GPP_THROT_RAT_MAX];
} ds_pdn_hdlr_info_type;

/*===========================================================================
FUNCTION ds_pdn_hdlr_clean_up_params
 
DESCRIPTION
  This function cleans up all the parameters owned by PDN HDLR
 
PARAMETERS 
  None

DEPENDENCIES 
  None

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None
===========================================================================*/
void ds_pdn_hdlr_cleanup_params
(
  void
);

/*===========================================================================
FUNCTION ds_pdn_hdlr_init
 
DESCRIPTION
  This function initializes all system specific parmeters for 3GPP MH
 
PARAMETERS 
  None

DEPENDENCIES 
  None

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None
===========================================================================*/
void ds_pdn_hdlr_init
(
  void
);

/*===========================================================================
FUNCTION ds_pdn_hdlr_get_current_network_mode

DESCRIPTION
  This function returns the current mode within the 3GPP RATs. This mode
  is maintained by the 3GPP mode handler and is used to dispatch mode
  specific functions

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  sys_sys_mode_e_type - current mode

SIDE EFFECTS
  None.
===========================================================================*/
sys_sys_mode_e_type ds_pdn_hdlr_get_current_network_mode
( 
  void
);

/*===========================================================================
FUNCTION ds_pdn_hdlr_set_network_mode

DESCRIPTION
  This function updates the network mode maintained inside the 3GPP mode
  handler.

PARAMETERS
  network_mode - the mode to which we have to update.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_hdlr_set_network_mode
( 
  sys_sys_mode_e_type     network_mode
);

/*===========================================================================
FUNCTION ds_pdn_hdlr_get_ps_data_suspend_flag

DESCRIPTION
  This function returns the flag indicating if PS data is suspended.

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  boolean : TRUE if set, FALSE otehrwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_hdlr_get_ps_data_suspend_flag
( 
  void
);

/*===========================================================================
FUNCTION ds_pdn_hdlr_get_attach_initiated_flag

DESCRIPTION
  This function returns the flag indicating if attach is initiated

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  boolean : TRUE if set, FALSE otehrwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_hdlr_get_attach_initiated_flag
( 
  void
);

/*===========================================================================
FUNCTION ds_pdn_hdlr_set_attach_initiated_flag

DESCRIPTION
  This function sets the flag indicating if attach is initiated

PARAMETERS
  boolean : TRUE if set, FALSE otherwise

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_hdlr_set_attach_initiated_flag
( 
  boolean flag
);

/*===========================================================================
FUNCTION ds_pdn_hdlr_get_attach_complete_flag

DESCRIPTION
  This function returns the flag indicating if attach is complete

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  boolean : TRUE if set, FALSE otehrwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_hdlr_get_attach_complete_flag
( 
  void
);

/*===========================================================================
FUNCTION ds_pdn_hdlr_set_attach_complete_flag

DESCRIPTION
  This function sets the flag indicating if attach is complete

PARAMETERS
  boolean : TRUE if set, FALSE otherwise

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_hdlr_set_attach_complete_flag
( 
  boolean flag
);

/*===========================================================================
FUNCTION ds_pdn_hdlr_get_sent_ps_data_avail_flag

DESCRIPTION
  This function returns the flag indicating if PS data available 
  indication was sent.

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  boolean : TRUE if set, FALSE otehrwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_hdlr_get_sent_ps_data_avail_flag
( 
  void
);

/*===========================================================================
FUNCTION ds_pdn_hdlr_set_sent_ps_data_avail_flag

DESCRIPTION
  This function sets the flag indicating if PS data available 
  indication was sent.

PARAMETERS
  boolean : TRUE if set, FALSE otehrwise

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_hdlr_set_sent_ps_data_avail_flag
( 
  boolean   sent_ps_data_available
);

/*===========================================================================
FUNCTION ds_pdn_hdlr_get_t3346_status

DESCRIPTION
  This function returns the T3346 status from NAS

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  ds_pdn_t3346_status_type

SIDE EFFECTS
  None.
===========================================================================*/
ds_pdn_t3346_status_type ds_pdn_hdlr_get_t3346_status
( 
  void
);

/*===========================================================================
FUNCTION  ds_pdn_hdlr_is_ps_data_avail_ind_allowed

DESCRIPTION
  This function queries srv_status maintained by ds3gmgr to check if
  we need to flow control the bearer and not send PS DATA AVAILABLE ind
  to CM.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_hdlr_is_ps_data_avail_ind_allowed
(
  void
);

/*===========================================================================
FUNCTION ds_pdn_hdlr_get_blocked_plmn_list

DESCRIPTION
  This function returns the blocked PLMN list.

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  list_type 

SIDE EFFECTS
  None.
===========================================================================*/
list_type* ds_pdn_hdlr_get_blocked_plmn_list
( 
  void
);

/*===========================================================================
FUNCTION ds_pdn_hdlr_get_current_plmn

DESCRIPTION
  This function fetches the current PLMN ID

PARAMETERS
  plmn_id_ptr - pointer to PLMN id to be returned

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_hdlr_get_current_plmn
(
  sys_plmn_id_s_type*     plmn_ptr
);

/*===========================================================================
FUNCTION ds_pdn_hdlr_set_current_plmn

DESCRIPTION
  This function sets the current PLMN ID

PARAMETERS
  plmn_id_ptr - pointer to PLMN id to be returned

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_hdlr_set_current_plmn
(
  emm_plmn_change_ind_type *payload
);

/*===========================================================================
FUNCTION ds_pdn_plmn_change_hdlr

DESCRIPTION
  This Handler is triggered while PLMN  changes
  
PARAMETERS  
  plmn_id - PLMN ID to be checked
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE  - If PLMN is non-NULL
  FALSE - otherwise

SIDE EFFECTS 
  None.  
===========================================================================*/
void ds_pdn_hdlr_plmn_change_hdlr
(
  emm_plmn_change_ind_type *payload
);

/*===========================================================================
FUNCTION ds_pdn_hdlr_is_plmn_non_null

DESCRIPTION
  This function checks if the PLMN passed into this function is non-NULL
  
PARAMETERS  
  plmn_id - PLMN ID to be checked
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE  - If PLMN is non-NULL
  FALSE - otherwise

SIDE EFFECTS 
  None.  
===========================================================================*/
boolean ds_pdn_hdlr_is_plmn_non_null
(
  sys_plmn_id_s_type plmn_id
);

/*===========================================================================
FUNCTION      ds_pdn_hdlr_get_hplmn_id

DESCRIPTION   This function returns the HPLMN ID.
              Note: memory has to be allocated by user.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_pdn_hdlr_get_hplmn_id
(
  sys_plmn_id_s_type      *hplmn_id_p
);

/*===========================================================================
FUNCTION      ds_pdn_hdlr_get_ehplmn_list

DESCRIPTION   This function returns the EHPLMN List
              Note: memory has to be allocated by user.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_pdn_hdlr_get_ehplmn_list
(
  reg_sim_plmn_list_s_type  *ehplmn_list_p
);

/*===========================================================================
FUNCTION      ds_pdn_hdlr_is_plmn_in_ehplmn_list

DESCRIPTION   This function checks whether the given PLMN is in EHPLMN List

DEPENDENCIES  None

RETURN VALUE  TRUE, if PLMN is in EHPLMN List
                       FALSE, otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds_pdn_hdlr_is_plmn_in_ehplmn_list
(
  sys_plmn_id_s_type      plmn_id
);


/*===========================================================================
  FUNCTION DS_3GPP_EXTRACT_MCC_FROM_PLMN_ID
  
  DESCRIPTION
    This function extracts MCC from PLMN ID
    
  PARAMETERS  
   sys_plmn_id_s_type          *current_plmn_id
   uint16                      *ret_mcc (MCC ptr)
   uint16                      *ret_mnc (MNC ptr)
      
  DEPENDENCIES 
    None.
    
  RETURN VALUE 
    void
     
  SIDE EFFECTS 
    None.
    
===========================================================================*/
void ds_pdn_hdlr_extract_mcc_mnc_from_plmn_id
(
   sys_plmn_id_s_type          *current_plmn_id,
   uint16                      *ret_mcc,
   uint16                      *ret_mnc
);

/*===========================================================================
  FUNCTION ds_pdn_hdlr_ps_data_suspend_resume_hdlr
  
  DESCRIPTION
    This function handles suspend/resume indication from CM
    
  PARAMETERS  
   boolean - PS data suspend/resume flag
                  TRUE - If PS data is suspended,
                  FALSE - otherwise
      
  DEPENDENCIES 
    None.
    
  RETURN VALUE 
    None
     
  SIDE EFFECTS 
    None.    
===========================================================================*/
void ds_pdn_hdlr_ps_data_suspend_resume_hdlr
(
  boolean  suspend_flag
);

/*===========================================================================
FUNCTION ds_pdn_hdlr_is_ue_in_ac11to15_on_plmnid

DESCRIPTION
  This function checks whether the ue is in ac 11 to 15 in
  the plmn id specified. This function returns true if 
  one of the  below scenarios is True 
 
  a) This function checks if the ue is in access class 11 or 15
     and the plmn specified matches with the home plmn or ehlpm list.

  b) This function checks if the ue is in access class 12 to 15  and 
     the plmn's  mcc part matches with the home plmn 

PARAMETERS
  None
  
DEPENDENCIES
  None.

RETURN VALUE  
  boolean - TRUE/FALSE
 
SIDE EFFECTS 
  None. 
===========================================================================*/
boolean ds_pdn_hdlr_is_ue_in_ac11to15_on_plmnid
(
  sys_plmn_id_s_type       current_plmn_id
);

/*===========================================================================
FUNCTION ds_pdn_hdlr_is_ue_in_ac11to15

DESCRIPTION
  This function checks whether the ue is in ac 11 to 15 on the current 
  PLMN. This function returns true if 
  one of the  below scenarios is True 
 
  a)This function checks if the ue is in access class 11 or 15
  and the ue's current plmn matches with the home plmn or ehlpm list.

  b)This function checks if the ue is in access class 12 to 15  and 
   the ue's current plmn's mcc part matches with the home plmn 

PARAMETERS
  None
  
DEPENDENCIES
  None.

RETURN VALUE  
  boolean - TRUE/FALSE
 
SIDE EFFECTS 
  None. 

===========================================================================*/
boolean ds_pdn_hdlr_is_ue_in_ac11to15
(
  void
);

/*===========================================================================
FUNCTION      ds_pdn_hdlr_set_cp_ciot_cap

DESCRIPTION   This function sets the CIOT Capability Information.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_pdn_hdlr_set_cp_ciot_cap
(
  sys_neg_ciot_cap       neg_ciot_cap
);


/*===========================================================================
FUNCTION  DS_PDN_HDLR_SYS_CHANGE_HANDLER

DESCRIPTION

  Called by 3G DSMGR when the serving system changes.


PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_hdlr_sys_change_handler
(
  sys_sys_mode_e_type    old_mode,
  sys_sys_mode_e_type    new_mode,
  sys_srv_domain_e_type  old_srv_domain,
  sys_srv_domain_e_type  new_srv_domain
);

/*===========================================================================
FUNCTION      ds_pdn_at_init_service_mode

DESCRIPTION   This function sets service mode for AT.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_pdn_at_init_service_mode
(
  void
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_GET_MAX_SIMUL_ACTIVE_PDP_CONTEXT

DESCRIPTION
  This function gets the maximum number of sumultanous active pdp contexts for
  the current RAT and PLMN.

PARAMETERS 
  rat_type 
  subs_id          : Subscription Id

DEPENDENCIES
  None.

RETURN VALUE  
  Number of simultanous active pdp contexts.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 ds_3gpp_pdn_get_max_simul_active_pdp_context
(
  ds_3gpp_throt_rat_e_type           rat_type
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_SET_MAX_SIMUL_ACTIVE_PDP_CONTEXT

DESCRIPTION
  This function sets the maximum number of sumiltanous active pdp contexts 
  for the current PLMN and RAT.
  pdn connection.

PARAMETERS 
  rat_type 
  max_active_pdp_context
  subs_id          : Subscription Id 
  
DEPENDENCIES
  None.

RETURN VALUE  
  void

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_pdn_set_max_simul_active_pdp_context
(
  ds_3gpp_throt_rat_e_type           rat_type,
  uint8                              max_active_pdp_context
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_RESET_MAX_SIMUL_ACTIVE_PDP_CONTEXT

DESCRIPTION
  This function sets the maximum number of sumiltanous active pdp contexts
  for the current PLMN and RAT.
  pdn connection.

PARAMETERS
  max_active_pdp_context
  subs_id          : Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_pdn_reset_max_simul_active_pdp_context
(
  void  
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_RESET_MAX_SIMUL_ACTIVE_PDP_CONTEXT_ON_PLMN_CHANGE

DESCRIPTION
  This function sets the maximum number of sumiltanous active pdp
  contexts for the current pdn connection.

PARAMETERS
  max_active_pdp_context
  subs_id
  
DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_pdn_reset_max_simul_active_pdp_context_on_plmn_change
(
  ds_3gpp_throt_rat_e_type               rat_type,
  sys_plmn_id_s_type                     plmn_id
);


/*===========================================================================
FUNCTION DS_3GPP_PDN_PROCESS_NEW_PLMN

DESCRIPTION
  This wrapper is to invoke a set of functions to advertise/clear blocked
  APN when PLMN changes.

PARAMETERS
  subs_id       -- Subscription ID where mode_pref_change occurred.
  plmn_id       -- new PLMN 
  network_mode  -- current RAT

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/

void ds_3gpp_pdn_process_new_plmn
(
  sys_plmn_id_s_type      plmn_id,
  sys_sys_mode_e_type     network_mode
);

/*===========================================================================
FUNCTION DS3GPP_SET_DS_LTE_FLAG

DESCRIPTION
  This function sets the lte flag based on whether we have disabled or 
  enabled LTE from ds module.   This is based on the boolean variable 

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE


SIDE EFFECTS
  None.

===========================================================================*/
void ds3gpp_set_ds_lte_flag
(
  boolean                 lte_flag
);

/*===========================================================================
FUNCTION DS3GPP_IS_LTE_FLAG_DISABLED

DESCRIPTION
  This function returns TRUE if LTE Flag is disabled

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_is_lte_flag_disabled
(
  void
);

#endif /* DS_PDN_HDLR_H */
