#ifndef DS_3GPP_PDN_LIMIT_HDLR_H
#define DS_3GPP_PDN_LIMIT_HDLR_H
/*===========================================================================
                      DS_PDN_THROT_SPEC.H
DESCRIPTION
This file exposes PDN spec throttling through APIs and data structures.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_3gpp_pdn_limit_hdlr.h#1 $
  $DateTime: 2020/03/17 08:03:16 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "ds_pdn_defs.h"
#include "ds_3gpp_pdn_throttle_sm.h"


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/* Maximum number of entries in the PDN Limit Table.*/
#define DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES DS_3GPP_MAX_PDN_CONTEXT
#define DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES

/* Directory & File Length for Persistent Throttling */
#define DS_3GPP_PDN_LIMIT_THROT_DIR_LEN 25

#define DS_3GPP_PDN_LIMIT_THROT_FILE_LEN_MAX 31

/*============================================================================
  Structure which stores the information read from EFS.
  It tells us whether PDN Limit handling is enabled or not. 			
  By default it is disabled.			
    																	
  It also tells us the PDN Req Throttling Time in mins.
============================================================================*/
typedef PACKED struct PACKED_POST
{
  boolean                  pdn_limit_is_enabled;
  uint16                   pdn_req_throttling_time;
}ds_3gpp_pdn_limit_efs_info_s;

/*===========================================================================
  EFS File Element Structure: Used to make throttling info persistant across
  cycle
=============================================================================*/

typedef struct
{
  boolean           valid;
  uint32            cookie;
  byte              apn_name[DS_PROFILE_APN_STRING_LEN+1];        /*APN name */
  uint64            timer_val;                    /*PDN Req Wait timer value  */
  uint64            timestamp;                   /*time when timer was started*/
} throt_info_pdn_limit_timer_type_dyn_s;

typedef struct
{
 throt_info_pdn_limit_timer_type_dyn_s    *throt_info_pdn_limit_timer_type_dyn_p;
} ds_3gpp_pdn_limit_throt_info_pdn_limit_timer_type;

/*=========================================================================== 
  Local Structure which stores the retrieved EFS info along with sampling timer
  & IMSI information
=============================================================================*/

typedef struct
{
  /* Structure to store PDN Limit throt timer config values              */ 
  ds_3gpp_pdn_limit_throt_info_pdn_limit_timer_type   
                                    pdn_limit_timer_tbl
                                    [DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES];                                                                            
             
  ps_timer_handle_type              sampling_timer;       /* Sampling timer  */
  uint32                            sampling_timer_val;   /* Sampling timer val*/
  nv_ehrpd_imsi_type                imsi_data;            /* IMSI data         */

  boolean                           imsi_data_avail;/* IMSI data available flag */

  char                              efs_file_name   /* EFS File to store */
     [DS_3GPP_PDN_LIMIT_THROT_DIR_LEN+1+DS_3GPP_PDN_LIMIT_THROT_FILE_LEN_MAX+1];

} ds_3gpp_pdn_limit_throt_info_type;

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_POWERUP_INIT

  DESCRIPTION
  This function performs Powerup Initialization of the Module
  
  PARAMETERS
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_powerup_init
(
  void
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_IS_ENABLED

  DESCRIPTION
  This function checks whether PDN Limit handling is enabled.
  
  PARAMETERS  
  None. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE if enabled. FALSE otherwise
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_pdn_limit_is_enabled
(
  void
);


/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_FIND_ENTRY

  DESCRIPTION
  This function finds entry corresponding at a given APN in PDN Limit Table.
  
  PARAMETERS  
  APN Name
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Index into the PDN limit Table.
  Returns DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY if cannot find an entry.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
uint8 ds_3gpp_pdn_limit_tbl_find_entry
(
  byte                   *apn
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_UPDATE_APN_NAME

  DESCRIPTION
  This function updates the APN Name in PDN Limit Table.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  New APN name.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_tbl_update_apn_name
(
  uint8                  index,
  char*                  apn
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_UPDATE_MAX_PDN_CONN_TIME

  DESCRIPTION
  This function updates the Max PDN Conn Time in PDN Limit Table.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  New Max PDN Conn Time
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_update_max_pdn_conn_time
(
  uint8                  index,
  uint16                 max_pdn_conn_time
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_UPDATE_MAX_PDN_CONN_PER_BLOCK

  DESCRIPTION
  This function updates the Max PDN Conn Per Block in PDN Limit Table.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  New Max PDN Conn Per Block.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_update_max_pdn_conn_per_blk
(
  uint8 index,
  uint16 max_pdn_conn_per_blk
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_UPDATE_PDN_REQ_WAIT_TIME

  DESCRIPTION
  This function updates the PDN Req Wait Time in PDN Limit Table.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  New PDN Req Wait Time.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_update_pdn_req_wait_time
(
  uint8 index,
  uint16 pdn_req_wait_time
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_START_PDN_CONN_TMR

  DESCRIPTION
  This function starts the PDN Conn Timer if the PDN throttling timer
  is not running and if the PDN Conn Timer has not yet started.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_start_pdn_conn_tmr
(
  uint8                   index
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_START_PDN_WAIT_TMR

  DESCRIPTION
  This function starts the PDN Req Wait Timer.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/

void ds_3gpp_pdn_limit_start_pdn_wait_tmr
(
  uint8                  index
);


/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_INCREMENT_PDN_CONN_CNTR

  DESCRIPTION
  This function increments the PDN Connection Counter.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_increment_pdn_conn_cntr
(
  uint8                   index
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_RESET_PDN_CONN_CNTR

  DESCRIPTION
  This function resets the PDN Connection Counter.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_reset_pdn_conn_cntr
(
  uint8                  index
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_CALL_BRINGUP_IS_ALLOWED

  DESCRIPTION
  This function checks whether call bringup is allowed for the given APN.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE if call is allowed.
  FALSE otherwise.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_pdn_limit_call_bringup_is_allowed
(
  uint8                  index
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_GET_CURRENT_PDN_CONN_TMR_VAL

  DESCRIPTION
  This function checks gets the current PDN Connection Timer Value.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Current PDN Conn Timer Value.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
uint32 ds_3gpp_pdn_limit_get_current_pdn_conn_tmr_val
(
  uint8                  index
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_IS_PDN_REQ_WAIT_TIMER_IN_PROGRESS

  DESCRIPTION
  This function checks whether the PDN Req Wait Timer is running.
  
  PARAMETERS  
  Index into the PDN Limit Table.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE or FALSE based on whether PDN Wait Timer is running
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_pdn_limit_is_pdn_req_wait_timer_in_progress
(
  uint8                  index
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_REMOVE_ENTRY
  DESCRIPTION
  This function removes entry at index 0 from PDN Limit Table.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_tbl_remove_entry
(
  uint8                  index
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PROCESS_PDN_CONN_TMR_EXPIRY_CMD

  DESCRIPTION
  This function processes the PDN Connection Timer expiry event.
  
  PARAMETERS  
  Callback data. This will be the index into the PDN Limit Table.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_process_pdn_conn_tmr_expiry_cmd
(
  void* cb_data
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PROCESS_PDN_REQ_THROT_TMR_EXPIRY_CMD

  DESCRIPTION
  This function processes the PDN Req Throttle Timer Expiry Cmd.
  
  PARAMETERS  
  Callback data. This will be the index into the PDN Limit Table.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_process_pdn_req_throt_tmr_expiry_cmd
(
  void*                   throt_sm_ptr
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PROCESS_PDN_WAIT_TMR_EXPIRY_CMD

  DESCRIPTION
  This function processes the PDN Wait Timer Expiry Cmd.
  
  PARAMETERS  
  Callback data. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_process_pdn_wait_tmr_expiry_cmd
(
  void*                   throt_sm_ptr
);

/*===========================================================================
FUNCTION  DS_3GPP_PDN_LIMIT_CLR_PDN_CONN_TMR 

DESCRIPTION
  This function clears the PDN connection timer

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_limit_clr_pdn_conn_tmr
(
   uint8                             index
);

/*==============================================================================
FUNCTION DS_3GPP_PDN_CNTXT_VALIDATE_AND_UPDATE_PDN_LIMIT_ENTRY
 
DESCRIPTION
  This function initially checks if pdn limit parameters bring passed are non-
  default , 
    If Yes: Add an entry in PDN limit table
    If NO : Check all other profile for non-default pdn limit parameter
            with same apn
            If Yes : Add an entry in PDN limit table with non-default parameters
            If No  : Add an entry in PDN limit table default parameters
 
  Assumptions made 
  1: Non-default Profile PDN limit parameter will be updated over default Profile
     PDN limit parameters with the same APN
  2. First non-default Profile PDN limit parameters will be chosen over multiple
     non-default Profile parameter given they have same APN

PARAMETERS 
  uint8  Profile_id :  Profile ID being passed
  byte profiel_apn  :  APN name of Profile ID being passed
 
DEPENDENCIES 
  None. 

RETURN VALUE 
  boolean DS3G_FAILURE : Failure to update PDN Limit Entry
          DS3G_SUCCESS : Sucessfully update PDN Limit Entry
 
SIDE EFFECTS 
  None 

==============================================================================*/
boolean ds_3gpp_pdn_cntxt_validate_and_update_pdn_limit_entry
(
 uint8                  profile_id,
 uint8                  *handle_id
);

/*==============================================================================
FUNCTION DS_3GPP_PDN_LIMIT_IMSI_INFO_AVAIL_CMD_HDLR
 
DESCRIPTION
  This function process the IMSI information available CMD so that to read IMSI
  information and perform power-up throttling if needed

PARAMETERS 
  subs_id           :  Subscription ID
 
DEPENDENCIES 
  None. 

RETURN VALUE 
  None.
 
SIDE EFFECTS 
  None 

==============================================================================*/
void ds_3gpp_pdn_limit_imsi_info_avail_cmd_hdlr
(
  void
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PROCESS_SAMPLING_TIMER_EXP_CMD

  DESCRIPTION
  This function process the Sampling Timer expiry command by:
  1. Writing a new sample to EFS File
  2. Starting the sampling Timer again
  
  PARAMETERS
  None. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_process_sampling_timer_exp_cmd
(
  void
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PROCESS_PDN_REQ_TMR_START_CMD

  DESCRIPTION
  This function does the required processing when PDN request wait timer
  starts like:
 
  1. Writing into EFS ro make it persistant
  2. Start the sampling timer so that samples are recorded frequently
 
  
  PARAMETERS
  throt_sm_ptr:               Throttle SM pointer
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_process_pdn_req_tmr_start_cmd
(
  void                                     *throt_sm_ptr
);

#endif /* DS_3GPP_PDN_LIMIT_HDLR_H */


