#ifndef DS_PDN_ROAMING_HDLR_H
#define DS_PDN_ROAMING_HDLR_H

/*===========================================================================
                      DS_PDN_ROAMING_HDLR.H
DESCRIPTION
This file exposes PDN roaming handler through APIs and data structures.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_pdn_roaming_hdlr.h#1 $
  $DateTime: 2020/03/17 08:03:16 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_pdn_defs.h"
#include "ds_pdn_manager.h"
#include "reg_sim.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*---------------------------------------------------------------------------- 
  Maximum number of Home PLMNs that can be configured in the EFS file
-----------------------------------------------------------------------------*/
#define DS_PDN_ROAMING_MAX_SUPPORTED_HOME_PLMNS 8

/*---------------------------------------------------------------------------- 
  Structure which incorporates a single MCC/MNC info entry
-----------------------------------------------------------------------------*/
typedef struct
{
  uint8  mcc[3];
  uint8  mnc[3];
}ds_pdn_roaming_mcc_mnc_info_s;

/*---------------------------------------------------------------------------- 
  Structure which represents the home PLMN List Information read from the EFS
-----------------------------------------------------------------------------*/
typedef struct
{
  uint8                          num_valid_entries;
                                        /* Number of valid entries in the EFS*/
  ds_pdn_roaming_mcc_mnc_info_s mcc_mnc_info
                                   [DS_PDN_ROAMING_MAX_SUPPORTED_HOME_PLMNS];
                                       /* Home PLMN information stored in
                                          MCC-MNC format                     */
} ds_pdn_roaming_efs_info_s;

typedef struct
{
  uint8                  profile_num;
  boolean                roaming_disallowed_flag;
}ds_pdn_roam_info_type;

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_HDLR_POWERUP_INIT

  DESCRIPTION
  This function performs Powerup Initialization of the Module
  
  PARAMETERS  
  None. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_pdn_roaming_hdlr_powerup_init
(
  void
);

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_GET_STATUS

  DESCRIPTION
  This function tells whether the UE is Roaming or not.
  
  PARAMETERS  
  Subscription Id 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE,  if UE is roaming
  FALSE, otherwise
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_pdn_roaming_get_status
(
  void
);

/*===========================================================================
  FUNCTION ds_pdn_roaming_infoRM_NEW_PLMN_INFO

  DESCRIPTION
  This function is used to notify the Roaming Module of PLMN change.
  
  PARAMETERS  
  Subscription Id
  New PLMN Id
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE,  if UE is roaming
  FALSE, otherwise
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_pdn_roaming_inform_new_plmn_info
(
  sys_plmn_id_s_type     new_plmn_id  
);

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_CONVERT_TO_PLMN_FORMAT

  DESCRIPTION
  This function converts MCC MNC to PLMN Format
  
  PARAMETERS  
  MCC MNC Info
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  PLMN Id
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
sys_plmn_id_s_type ds_pdn_roaming_convert_to_plmn_format
(
  ds_pdn_roaming_mcc_mnc_info_s mcc_mnc_info
);


/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_SET_STATUS

  DESCRIPTION
  This function sets the roaming status.
  
  PARAMETERS  
  Subscription Id
  Roaming Status
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_pdn_roaming_set_status
(
  boolean                status
);

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_CLEAR_WAIT_TIMER_ENTRY

  DESCRIPTION
  This function is called to clear the wait timer corresponding to the given
  profile.
  
  PARAMETERS  
  Profile Num
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/ 
void ds_pdn_roaming_clear_wait_timer_entry
(
  uint8 profile_num  
);

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_FLUSH_WAIT_TIMER_ENTRY

  DESCRIPTION
  This function is called to clear the entire wait timer list.
  
  PARAMETERS  
  None.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/
void ds_pdn_roaming_flush_wait_timer_list
(
  void
);

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_UPDATE_PLMN_INFO_ON_SUBS_READY

  DESCRIPTION
  This function is invoked when subscription ready ev is received.
  This function checks if EHPLMN NV is enabled and if enabled calls
  roaming_inform_new_plnn_onfo which will then take action to perform
  blocking/unblocking.
  
  PARAMETERS  
  Subscription Id
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_pdn_roaming_update_plmn_info_on_subs_ready
(
  void
);

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_ADVERTISE_APNS_BLOCKED_DUE_TO_ROAMING

  DESCRIPTION
  This function advertises all Roaming Blocked APNs to its clients
  
  PARAMETERS  
  Subscription id
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/  
void ds_pdn_roaming_advertise_apns_blocked_due_to_roaming
(  
  boolean                advertise_empty_list
);

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_PROCESS_DISALLOWED_FLAG_CHG_CMD

  DESCRIPTION
  This function is used to process the change in Roaming Disallowed Flag
  
  PARAMETERS  
  Profile Number
  Roaming Disallowed Flag
  Subscription Id
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/
void ds_pdn_roaming_process_disallowed_flag_chg_cmd
(
  uint8                  profile_num,
  boolean                roaming_disallowed_flag
);

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_IS_APN_DISALLOWED

  DESCRIPTION
  This function checks whether PDN Connection to given APN is allowed or not.
  
  PARAMETERS
  APN Name
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE, if PDN Connection is allowed
  FALSE, otherwise
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_pdn_roaming_is_apn_disallowed
(
  char    *apn
);

/*===========================================================================
  FUNCTION DS_PDN_ROAMING_HDLR_CLEAR_INFO

  DESCRIPTION
  This function performs resetting of the roaming module info.
  
  PARAMETERS  
  None. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/
void ds_pdn_roaming_hdlr_clear_info
(
  void
);

#endif /* DS_PDN_ROAMING_HDLR_H */

