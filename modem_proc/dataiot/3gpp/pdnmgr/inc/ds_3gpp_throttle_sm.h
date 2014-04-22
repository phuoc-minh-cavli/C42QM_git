/*!
  @file
  ds_3gpp_throttle_sm.h

  @brief
  Internal utility functions and routines

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2009-2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_3gpp_throttle_sm.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/05/11   ss      Changes to support RPM.
08/30/10   hs      Initial version

===========================================================================*/

#ifndef DS_3GPP_THROTTLE_SM_H
#define DS_3GPP_THROTTLE_SM_H

#define DS_THROT_LOG_PARAM_NA          0x0F

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "rex.h"
#include "sys.h"
#include "ds_pdn_hdlr.h"
#include "ds_sys_event.h"
#include "ds_pdn_manager.h"
#include "ds_eps_pdn_manager.h"
#include "ds_profile.h"
#include "ds_crit_sect.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define DS_3GPP_THROT_TIMER_MIN_SECOND 1
#ifndef TEST_FRAMEWORK
#define DS_3GPP_THROT_TIMER_MAX_SECOND 15
#else
#error code not present
#endif

#define DS_3GPP_THROT_RANDOM_TIMER_MIN 1
#define DS_3GPP_THROT_RANDOM_TIMER_MAX 10

#define DS_3GPP_THROT_SECOND_TO_MILISECOND_MULTIPLIER 1000
#define DS_3GPP_THROT_MINUTE_TO_MILISECOND_MULTIPLIER 60000
#define DS_3GPP_THROT_HOUR_TO_MILLISECOND_MULTIPLIER  3600000

#define DS_3GPP_THROT_SM_COOKIE 0xbeabeabe

/*------------------------------------------------------------------------------
  Maximum number of PLMNs we would support simultaneously
------------------------------------------------------------------------------*/
#define DS_3GPP_MAX_PLMN_SUPPORTED 8

/*---------------------------------------------------------------------------
  Converting a value in seconds to mili seconds
---------------------------------------------------------------------------*/
#define DS_SEC_TO_MSEC(val_sec) ((val_sec) * 1000)

/*-----------------------------------------------------------------------
  This enumeration will indicate the kind of Throttling (PDN or Service Request) 
  being applied. 
------------------------------------------------------------------------*/
typedef enum
{
  DS_3GPP_THROT_TYPE_MIN = 0,
  DS_3GPP_THROT_TYPE_PDN = DS_3GPP_THROT_TYPE_MIN,
  DS_3GPP_THROT_TYPE_SRV_REQ,
  DS_3GPP_THROT_TYPE_MAX
} ds_3gpp_throt_sm_type;

/*------------------------------------------------------------------------------
  This enumeration lists the conditions under which throttling maybe reset
------------------------------------------------------------------------------*/
typedef enum
{
  DS_3GPP_THROT_RESET_TYPE_LPM,
  DS_3GPP_THROT_RESET_TYPE_CARD_REFRESH,
  DS_3GPP_THROT_RESET_TYPE_CARD_MAX
} ds_3gpp_throt_sm_reset_type;

/*------------------------------------------------------------------------------
  Function pointer type for core SM timer start/stop callbacks 
------------------------------------------------------------------------------*/
typedef void (*ds_3gppi_throttle_sm_timer_ev_cb_f) (void* throt_sm_ptr, 
                                                    void* user_data);

/*------------------------------------------------------------------------------
  Functon pointer type for fetching the correct Throttling Timer Value
------------------------------------------------------------------------------*/
typedef uint32 (*ds_3gppi_throttle_sm_get_value_f)  (void* throt_sm_ptr, 
                                                     void* user_data);

/*------------------------------------------------------------------------------ 
  Function pointer for module-specific checks on whether
  throttling can be cleared upon low power mode
  ------------------------------------------------------------------------------*/
typedef boolean (*ds_3gppi_throttle_clear_on_lpm_f) (void* throt_sm_ptr,
                                                     void* user_data);

/*------------------------------------------------------------------------------
  Functon pointer type for incrementing throttle count
------------------------------------------------------------------------------*/
typedef void (*ds_3gppi_throttle_sm_inc_cnt_f)  (void* throt_sm_ptr, 
                                                 void* user_data);

/*------------------------------------------------------------------------------
  Struct to contain information about user_data
  ------------------------------------------------------------------------------*/
typedef struct
{
  ds_net_down_reason_type                  cause_code;
  ds_profile_auth_type                     *auth_info_p;
}ds_3gpp_throt_user_data;

/*------------------------------------------------------------------------------
  This structure is used to implement the throttling state machine
  This is a generic state machine and anyone who is interested in using the
  throttling algorithm can make use of this module.
------------------------------------------------------------------------------*/
typedef struct
{
  boolean                            is_throttled;  /* boolean indicating if throttling is ON */
  uint8                              failure_count; /* Numer of attempts rejected by the 
                                                                                      network */
  ps_timer_handle_type               throttle_timer;/* Timer to start when the SM is in 
                                                                                     throttle */
  uint32                             throt_tmr_val; /* The value that the throt timer has to be
                                                                                        set to*/
  sys_plmn_id_s_type                 plmn_id;       /* PLMN that this SM is applicable to     */
                                    
  byte                               apn[DS_PROFILE_APN_STRING_LEN + 1]; /* APN for this SM */


  ds_3gpp_throt_user_data            user_data; /*To contain any particular user_data*/

  uint16                             throttle_sys_mask;/* current sys-mode mask */

  uint32                             cookie;

  uint32                             min_random_timer_val;/*user configured min/max random timer values*/

  uint32                             max_random_timer_val;/*user configured min/max random timer values*/

  void                              *tmr_exp_cb_data_p;

  void                              *parent_ptr;

  ds_3gpp_throt_sm_type              throt_sm_type;

  uint8                              lwr_lyr_failure_cnt;

} ds_3gppi_throttle_sm_s;

/*-------------------------------------------------------------------------
  This structure is used to store the Generic Callback tables registered
  by PDN Throttle SM and Service Request Throttle SM
-------------------------------------------------------------------------*/

typedef struct
{
  ds_3gppi_throttle_sm_timer_ev_cb_f   tmr_exp_cb;                                   
  ds_3gppi_throttle_sm_timer_ev_cb_f   tmr_start_cb;                                   
  ds_3gppi_throttle_sm_get_value_f     get_tmr_val_cb;
  ds_3gppi_throttle_sm_inc_cnt_f       inc_cntr_cb;
} ds_3gpp_throt_sm_info_tbl_s;

/*-------------------------------------------------------------------------
  This structure is used to store the Callback data when 
  feeding failure  into the Core Throttle State machine.
  Core Throttle SM will pass the callback data to the 
  relevant throttling module (PDN or Service Request Throttle 
  module) when invoking the timer start/expiry/get timer callbacks.
-------------------------------------------------------------------------*/
typedef struct
{
  void* tmr_start_cb_data_p;
  void* tmr_exp_cb_data_p;
  void* get_tmr_val_cb_data_p;
} ds_3gpp_throttle_sm_cb_data_type;


extern ds_crit_sect_type ds_3gppi_throttle_sm_crit_sect;

#define DS_3GPP_SYS_SEL_RETRY_MAX_VAL 16

typedef enum
{
  DS_3GPP_NO_SYS_SEL_RETRY,
  DS_3GPP_LTE_ENABLE_RETRY,
  DS_3GPP_LTE_DISABLE_RETRY,
  DS_3GPP_LTE_PLMN_BLOCK_RETRY,
  DS_3GPP_LTE_PLMN_UNBLOCK_RETRY
}ds_3gpp_plmn_sys_sel_retry_e_type;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================
FUNCTION DS_3GPP_THROT_REGISTER_SM_INFO_TABLE

DESCRIPTION
  This function is used by PDN and Service request throttling modules
  to register their respective info tables, which specify attributes 
  such as start timer callback, stop timer callback, Get Timer Value Callback

PARAMETERS   :
  cb_tbl_ptr        - Pointer to the Info Table
  pdn_throt_sm_type - Type of Throttling (PDN or Service request)
  subs_id           - Subscription Id

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_throt_register_sm_info_tbl
(
  ds_3gpp_throt_sm_info_tbl_s    *cb_tbl_ptr,
  ds_3gpp_throt_sm_type          throt_sm_type
);


/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_INIT

DESCRIPTION
  This function initializes the core throttling state machine
  
PARAMETERS  
  None.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.

SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_throt_sm_init
(
  void
);



/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_CREATE_CORE_THROTTLE_SM

DESCRIPTION
  This function gets a new throttle state machine instance to a client. It
  also initializes all the fields appropriately.

  This is the internals of state machine implementation - Will also be refered
  to as the core state machine
  
PARAMETERS  
  plmn_id:         PLMN Id
  subs_id:         Subscription Id
  throt_sm_type:   Throttle SM Type (PDN or SRV request)
  parent_ptr       Pointer to the parent of the Core Throttle SM 
                   being allocated

DEPENDENCIES 
  None.
  
RETURN VALUE 
  Pointer to the newly obtained throttle SM.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
ds_3gppi_throttle_sm_s* ds_3gppi_throt_sm_create_core_throttle_sm
(
  sys_plmn_id_s_type           plmn_id,
  ds_3gpp_throt_sm_type        throt_sm_type,
  void                        *parent_ptr
);



/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_FREE_CORE_STATE_MACHINE

DESCRIPTION
  This function destroys a throttle SM that a client has been using. This can
  be called whenever throttling is stopped.

  This is the internals of state machine implementation - Will also be refered
  to as the core state machine

PARAMETERS  
  throt_sm_ptr - Pointer to the throttling SM to be released.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gppi_throt_sm_free_core_state_machine
(
  ds_3gppi_throttle_sm_s* throt_sm_ptr
);

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_FEED_FAILURE_TO_CORE_SM

DESCRIPTION
  This function is used to apply throttling. This performs the following
  operations:
  1. Set the throttle SM to THROTTLED state
  2. Increment the counter corresponding to the number of rejects
  3. Get the throttle timer value based on the number of rejects
  4. Start the throttling timer
  
  This is the internals of state machine implementation - Will also be refered
  to as the core state machine

PARAMETERS  
  throt_sm_ptr              Pointer to the throttling SM to be released.
  cb_data_p                 CB Data to be passed when performing throttling
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gppi_throt_sm_feed_failure_to_core_sm
(
  ds_3gppi_throttle_sm_s             *throt_sm_ptr,
  ds_3gpp_throt_rab_info_s           *rab_throt_info,
  ds_3gpp_throttle_sm_cb_data_type   *cb_data_p
);

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_GET_IS_THROTTLED_FLAG

DESCRIPTION
  This function examines if a throttling SM is in throttled or unthrottled
  state
  
  This is the internals of state machine implementation - Will also be refered
  to as the core state machine

PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE  - If the SM is in throttled state
  FALSE - If the SM is not in throttled state
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gppi_throt_sm_get_is_throttled_flag
(
  ds_3gppi_throttle_sm_s* throt_sm_ptr
);

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_GET_PLMN_ID

DESCRIPTION
  This function returns the plmn_id for the core SM

PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  sys_plmn_id_s_type - plmn_id of the core SM. 

SIDE EFFECTS 
  None.
  
===========================================================================*/
sys_plmn_id_s_type ds_3gppi_throt_sm_get_plmn_id
(
  ds_3gppi_throttle_sm_s* throt_sm_ptr  
);

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_GET_FAILURE_COUNT

DESCRIPTION
  This function is used to get the count of failed attempts that is maintained
  by the core state machine. Note that such an API will not be exposed from
  any wrapper. Failures will always be fed into the wrapper. The wrapper has
  the logic needed to increment counter values.
  
  This is the internals of state machine implementation - Will also be refered
  to as the core state machine

PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
  failure_count_ptr - out param pointing to the failure count
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE  - if the incoming params were non-NULL
  FALSE - incoming params NULL
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gppi_throt_sm_get_failure_count
(
  ds_3gppi_throttle_sm_s *throt_sm_ptr,
  uint8                  *failure_count_ptr
);

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_SET_FAILURE_COUNT

DESCRIPTION
  This function is used to set the count of failed attempts that is maintained
  by the core state machine. Note that such an API will not be exposed from
  any wrapper. Failures will always be fed into the wrapper. The wrapper has
  the logic needed to increment counter values.
  
  This is the internals of state machine implementation - Will also be refered
  to as the core state machine

PARAMETERS  
  throt_sm_ptr  - pointer to the throttling state machine
  failure_count - failure_count to be stored in the core sm
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE  - On success
  FALSE - On any failure
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gppi_throt_sm_set_failure_count
(
  ds_3gppi_throttle_sm_s *throt_sm_ptr,
  uint8                  failure_count
);

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_GET_THROTTLE_TIMER_VAL

DESCRIPTION
  This function returns the timer value for which throttling is to be done.
  
  This is the internals of state machine implementation - Will also be refered
  to as the core state machine

PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  uint32 - the value that the throttling timer was started with.  

SIDE EFFECTS 
  None.
  
===========================================================================*/
uint32 ds_3gppi_throt_sm_get_throttle_timer_val
(
  ds_3gppi_throttle_sm_s* throt_sm_ptr  
);

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_GET_REMAINING_THROTTLE_TIME

DESCRIPTION
  This function returns the remaining time in milliseconds for which
  throttling occurs
  
  This is the internals of state machine implementation - Will also be refered
  to as the core state machine

PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  uint32 - remaining throttle time.  

SIDE EFFECTS 
  None.
  
===========================================================================*/
uint32 ds_3gppi_throt_sm_get_remaining_throttle_time
(
  ds_3gppi_throttle_sm_s* throt_sm_ptr  
);

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_CLEAR_THROTTLE_TIMER

DESCRIPTION
  This function clears the throttling timer for the specified core throt_sm.

PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine

DEPENDENCIES 
  None.

RETURN VALUE 
  None.

SIDE EFFECTS 
  None.
===========================================================================*/
void ds_3gppi_throt_sm_clear_throttle_timer
(
  ds_3gppi_throttle_sm_s* throt_sm_ptr
);

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_IS_PLMN_NON_NULL

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
boolean ds_3gpp_throt_sm_is_plmn_non_null
(
  sys_plmn_id_s_type plmn_id
);

/*========================================================================= 
  FUNCTION ds_3gpp_throt_sm_reset_core_throt_sm

DESCRIPTION
  This function resets the throttle statae machine 
  paramaters asscoiated with release 10 paramters 
  
PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine

DEPENDENCIES 
  None.

RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.

===========================================================================*/


void ds_3gpp_throt_sm_reset_core_throt_sm
(
  void*   throt_sm_ptr
);

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_SET_THROTTLE_TIMER_VAL

DESCRIPTION
  This function sets the timer value for which throttling is to be done.
  
  This is the internals of state machine implementation - Will also be
  referred to as the core state machine
 
PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
  timer_val    - Value to be set
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gppi_throt_sm_set_throttle_timer_val
(
  ds_3gppi_throttle_sm_s* throt_sm_ptr,
  uint32                  timer_val 
);

/*===========================================================================
FUNCTION DS_3GPPI_SET_IS_THROTTLED_FLAG

DESCRIPTION
  This function sets a throttling SM to throttled or unthrottled
  state
  
  This is the internals of state machine implementation - Will also be
  referred to as the core state machine

PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
  is_throttled - boolean value to set the is_throttled flag
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None. 
 
SIDE EFFECTS 
  None.
  
===========================================================================*/

void ds_3gppi_throt_sm_set_is_throttled_flag
(
  ds_3gppi_throttle_sm_s *throt_sm_ptr,
  boolean                 is_throttled
);

/*===========================================================================
FUNCTION DS_3GPPI_GET_USER_DATA

DESCRIPTION
  This function returns the user_data contained in the core throttle sm.
  
  This is the internals of state machine implementation - Will also be
  referred to as the core state machine

PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  ds_3gpp_throt_user_data 
 
SIDE EFFECTS 
  None.
  
===========================================================================*/

ds_3gpp_throt_user_data *ds_3gppi_throt_sm_get_user_data
(
  ds_3gppi_throttle_sm_s *throt_sm_ptr
);

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_ADVERTISE_BLOCKED_APNS_TO_MPPM

DESCRIPTION
  This function sends the currently blocked list of of APNs to MPPM
  
PARAMETERS 
  void

DEPENDENCIES
  None.

RETURN VALUE 
  void 
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_throt_sm_advertise_blocked_apns_to_mppm_on_plmn
(
  sys_plmn_id_s_type               plmn_id
);

/*===========================================================================
FUNCTION DS_3GPP_THROT_FILL_CB_DATA

DESCRIPTION
  This function is used to populate the Callback Data structure that will be
  passed on to Core throttle SM when performing throttling. Core Throttle
  SM will pass the callback data to the relevant throttling module (PDN or
  Service Request Throttle module) when invoking the timer
  start/expiry/get timer callbacks.

PARAMETERS
  cb_data_p:             CB Data to be populated
  tmr_start_cb_data_p:   Timer Start Callback Data
  tmr_exp_cb_data_p:     Timer Expiry Callback Data
  tmr_get_val_cb_data_p: Get Timer Callback Data 
    
DEPENDENCIES 
  None
  
RETURN VALUE 
  None
 
SIDE EFFECTS 
  None
  
===========================================================================*/
void ds_3gpp_throt_fill_cb_data
(
  ds_3gpp_throttle_sm_cb_data_type *cb_data_p,
  void                             *tmr_start_cb_data_p,
  void                             *tmr_exp_cb_data_p,
  void                             *tmr_get_val_cb_data_p 
);

/*===========================================================================
FUNCTION      ds_util_get_throttle_token_id

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
int16 ds_util_get_throttle_token_id
(
  char *from,
  char *to,
  uint8 *token_id
);
//aks
#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_IS_PLMN_BLOCKED

DESCRIPTION
  This function is used to check if the given PLMN is blocked due to t3396 throttling
  the PLMN to blocked PLMN List.

PARAMETERS
  PLMN Id
  Subscription Id

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if PLMN is blocked
  FALSE, otherwise

SIDE EFFECTS
None
===========================================================================*/
boolean ds_3gpp_pdn_cntxt_is_plmn_blocked
(
  sys_plmn_id_s_type	 plmn_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_IS_PLMN_BLOCKED_WITH_CAUSE

DESCRIPTION
  This function is used to check if the given PLMN is blocked due to t3396 throttling
  the PLMN to blocked PLMN List.

PARAMETERS
  PLMN Id
  Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if PLMN is blocked
  FALSE, otherwise

SIDE EFFECTS
None
===========================================================================*/
boolean ds_3gpp_pdn_cntxt_is_plmn_blocked_with_cause
(
  sys_plmn_id_s_type             plmn_id,
  sys_block_plmn_cause_e_type    cause
);


/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_DISABLE_LTE_PER_PLMN_SELECTION_EX

DESCRIPTION
  This function disables LTE per PLMN for the given PLMN Id. It also adds
  the PLMN to blocked PLMN List.

PARAMETERS
  PLMN Id
  Timer Value for which PLMN will be blocked
  Subscription Id

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if PLMN Disabled successfully
  FALSE, if PLMN Disable failed or PLMN already disabled

SIDE EFFECTS
===========================================================================*/
boolean ds_3gpp_pdn_cntxt_disable_lte_per_plmn_selection_ex
(
   sys_plmn_id_s_type      plmn_id,
   uint32                  throttle_timer_val,
   boolean                 detach,
   sys_block_plmn_cause_e_type    cause
);

/*===========================================================================
FUNCTION DS_3PP_PDN_CNTXT_ENABLE_LTE_PLMN_BLOCKING_ON_PLMN_ID_EX

DESCRIPTION
  This function enables tyhe given PLMN and removes it from the
  blocked PLMN List.  
PARAMETERS  
  PLMN id
  Susbcription id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE, if PLMN Unblocked successfully
  FALSE, if PLMN was already unblocked or PLMN unblock failed.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_pdn_cntxt_enable_lte_plmn_blocking_on_plmn_id_ex
(
  sys_plmn_id_s_type     plmn_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_LTE_UNBLOCK_ALL_PLMNS

DESCRIPTION
  This function unblocks all the blocked PLMNs.

PARAMETERS
 sys_modem_as_id_e_type subs_id - SUBS_ID 

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_pdn_cntxt_enable_lte_unblock_all_plmns
(
  void
);


/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_FLUSH_BLOCKED_PLMN_LIST

DESCRIPTION
  This function frees the Blocked PLMN List.

PARAMETERS
 sys_modem_as_id_e_type subs_id - SUBS_ID 

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_pdn_cntxt_flush_blocked_plmn_list
(
  void
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_FLUSH_PLMN_FROM_LIST

DESCRIPTION
  This function frees the PLMN specified in the blocking List.
PARAMETERS 
 sys_plmn_id_s_type unblock_plmn_id  - UNBLOCK PLMN ID
 sys_modem_as_id_e_type subs_id - SUBS_ID 

DEPENDENCIES
  None.

RETURN VALUE  
  None.
SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_pdn_cntxt_flush_plmn_from_list
(
  sys_plmn_id_s_type unblock_plmn_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDNT_CNTXT_CHECK_IF_ATTACH_PROFILE_AVAILABLE
DESCRIPTION
  This function checks if Attach Profile is Available. 
  We check both case where attach profile list is configured and not configured.
PARAMETERS  
  Subscription Id
DEPENDENCIES 
  None.  
RETURN VALUE 
  DS_PDN_CNTXT_ATTACH_PROFILE_UNKNOWN - If Attach profile status is unknown.
  DS_PDN_CNTXT_ATTACH_PROFILE_AVAIL - If Attach profile is available
  DS_PDN_CNTXT_ATTACH_PROFILE_UNAVAIL - If Attach profile is not available
SIDE EFFECTS 
  None.
===========================================================================*/
ds_pdn_cntxt_attach_profile_status_type
  ds_3gpp_pdn_cntxt_check_if_attach_profile_available
(
  void
);

/*===========================================================================
FUNCTION DS_3GPP_PDNT_CNTXT_DETACH_AND_BLOCK_PLMN_IF_ATTACH_PROFILE_UNAVAILABLE
DESCRIPTION
  This function disables lte if there are no active bearers and all the attach apn's
  are blocked.we check both case where attach profile list is configured and not configured.
PARAMETERS  
  None.
DEPENDENCIES 
  None.  
RETURN VALUE 
  None
SIDE EFFECTS 
  None.
===========================================================================*/
void ds_3gpp_pdn_cntxt_detach_and_block_plmn_if_attach_profile_unavailable
(
  void
);

/*===========================================================================
FUNCTION DS_3GPP_PDNT_CNTXT_DISABLE_LTE_IF_ATTACH_PROFILE_UNAVAILABLE
DESCRIPTION
  This function disables lte if there are no active bearers and all the attach apn's
  are blocked.we check both case where attach profile list is configured and not configured.
PARAMETERS  
  None.
DEPENDENCIES 
  None.  
RETURN VALUE 
  None
SIDE EFFECTS 
  None.
===========================================================================*/
void ds_3gpp_pdn_cntxt_disable_lte_if_attach_profile_unavailable
(
  void
);

/*===========================================================================
FUNCTION DS_3PP_PDN_CNTXT_ENABLE_LTE_IF_ATTACH_PROFILE_AVAILABALE

DESCRIPTION
  This function enabled lte if attach apn's are available.
  we check both case where attach profile list is configured
  and not configured (default socket profile).
  
PARAMETERS  
  None.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None
   
SIDE EFFECTS 
  None.
  
===========================================================================*/

void  ds_3gpp_pdn_cntxt_enable_lte_if_attach_profile_available
(
  void
);

/*===========================================================================
FUNCTION DS_3PP_PDN_CNTXT_ENABLE_LTE_EACH_PLMNID

DESCRIPTION
  This function enabled lte on each blocked plmn
  if atleast one attach apn are available. on the blocked plmn list

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void  ds_3gpp_pdn_cntxt_enable_lte_each_plmnid
(
  void
);

/*===========================================================================
FUNCTION DS_3PP_PDN_CNTXT_IF_ATTACH_PROFILE_AVAILABLE_PLMN_ID

DESCRIPTION
  This function checks if the attach profile is available on the passed plmnid and attach list combination

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_pdn_cntxt_check_if_attach_profile_available_plmn_id
(
  sys_plmn_id_s_type                           plmn_id,
  ds_pdn_apm_lte_attach_pdn_list_type          attach_list
);

/*===========================================================================
FUNCTION DS_3GPP_PDNT_CNTXT_DETACH_AND_BLOCK_PLMN
DESCRIPTION
  This function performs LTE detach and blocks the current PLMN.

PARAMETERS  
  Subscription Id

DEPENDENCIES 
  None.  

RETURN VALUE 
  None

SIDE EFFECTS 
  None.
===========================================================================*/
void ds_3gpp_pdn_cntxt_detach_and_block_plmn
(
  boolean                   detach
);

/*===========================================================================
FUNCTION DS_3GPP_PDNT_CNTXT_BLOCK_PLMN
DESCRIPTION
  This function function blocks the current PLMN.

PARAMETERS  
  Subscription Id

DEPENDENCIES 
  None.  

RETURN VALUE 
  None

SIDE EFFECTS 
  None.
===========================================================================*/
void ds_3gpp_pdn_cntxt_block_plmn
(
  void
);


/*===========================================================================
FUNCTION DS3G_MSH_GET_BLOCKED_PLMN_LIST_SIZE

DESCRIPTION
  This function returns the number of Blocked PLMNs.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  Number of Blocked PLMNs

SIDE EFFECTS
  None
===========================================================================*/
uint32 ds_3gpp_pdn_cntxt_get_blocked_plmn_list_size
(
  void
);

/*===========================================================================
FUNCTION      DS3G_PRINT_PLMN_ID

DESCRIPTION   This function prints the plmn_id in MCC-MNC format.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_print_plmn_id
(
  void  *current_plmn
);

#endif //FEATURE_DATA_LTE

/*========================================================================== 
FUNCTION DS_EPS_BEARER_CNTXT_EXAMINE_IF_EMM_CAUSE_THROT_NEEDED

DESCRIPTION 
  This function checks whether EMM cause throttling is needed or not.
    
PARAMETERS
  LTE Call Info Ptr
  
DEPENDENCIES
  None.
                  
RETURN VALUE
  TRUE, if throttling needed
  FALSE, otherwise

SIDE EFFECTS 
  None
===========================================================================*/
boolean ds_eps_bearer_cntxt_examine_if_emm_cause_throt_needed
(
  ds_bearer_deact_params_s  *lte_call_info_ptr,
  ds_3gpp_throt_rab_info_s  *rab_throt_info
);

/*===========================================================================
FUNCTION DS3G_MSH_ENABLE_LTE_PLMN_BLOCKING_ON_PLMN_ID_EX

DESCRIPTION
  This function resets the lte plmn blocking list on the plmn id passed. This function in turn
  invokes the CM API that carries out the desired function

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_msh_enable_lte_plmn_blocking_on_plmn_id_ex2
(
  sys_plmn_id_s_type plmn_id,
  uint16                 request_pref
);

/*===========================================================================
FUNCTION DS_3GPP_PLMN_PREP_FOR_LTE_BLOCKING_AND_LOG

DESCRIPTION
  This function prepares all the necessary variables, throttle retry sm's
  before calling ds3gmshif function to actually block/disable LTE
 
PARAMETERS  
  sys_plmn_id_s_type      plmn_id,
  uint32                  timer_val,
  sys_modem_as_id_e_type  subs_id,
  uint8                   failure_count
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE                    If the function succeeds
  FALSE                   Otherwise
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_plmn_prep_for_lte_blocking_and_log
(
  sys_plmn_id_s_type      plmn_id,
  uint32                  timer_val,
  uint8                   failure_count,
  boolean                        detach,
  sys_block_plmn_cause_e_type    cause
);

/*===========================================================================

FUNCTION DS3G_MSH_DISABLE_LTE_SELECTION_EX

DESCRIPTION
  This function disables the selection of LTE network. This function in turn
  invokes the CM API that carries out the desired function

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_msh_disable_lte_selection_ex
(
  void
);

/*===========================================================================

FUNCTION DS3G_MSH_ENABLE_LTE_SELECTION_EX

DESCRIPTION
  This function enables the selection of LTE network. This function in turn
  invokes the CM API that carries out the desired function

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_msh_enable_lte_selection_ex
(
  void
);

/*===========================================================================
FUNCTION  DS_3GPP_PLMN_SYS_SEL_CLR_RETRY_SM

DESCRIPTION
  This function clears sys_sel_retry_sm 

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_plmn_sys_sel_clr_retry_sm
(
   void
);

/*===========================================================================
FUNCTION  DS_3GPP_PLMN_SYS_SEL_SET_RETRY_TYPE

DESCRIPTION
  This function sets the retry type

PARAMETERS
  ds_3gpp_throt_sys_sel_retry_e_type     retry_type           
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_plmn_sys_sel_set_retry_type
(
  ds_3gpp_plmn_sys_sel_retry_e_type     retry_type
);

/*===========================================================================
FUNCTION  DS_3GPP_PLMN_SYS_SEL_SET_BLK_INFO

DESCRIPTION
  This function sets the PLMN BLK INFO, This is needed to 
  recalculate the retry time when a disable fails.

PARAMETERS 
  PLMN blocking info BLOB 
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_plmn_sys_sel_set_blk_info
(
   cm_plmn_blocking_info_s_type *plmn_blocking_info_p
);

/*===========================================================================
FUNCTION DS3G_MSH_DISABLE_LTE_PER_PLMN_EX

DESCRIPTION
  This function disables the lte on the plmn id passed for timer value .
  This function in turn invokes the CM API that carries out the desired function

PARAMETERS 
 cm_plmn_blocking_info_s_type       plmn_blocking_info; 

DEPENDENCIES
  None.

RETURN VALUE  
  boolean - TRUE (blocking succesful) FALSE (blocking unsuccesful)

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_msh_disable_lte_per_plmn_ex
(
  cm_plmn_blocking_info_s_type       plmn_blocking_info
);

/*===========================================================================
FUNCTION DS_3GPP_PLMN_SYS_SEL_PREF_ERR_CMD_HDLR
 
DESCRIPTION
  This function handles when sys selection enable/disable has resulted in
  an error, It establishes a retry mechanism to enable/disable LTE
  
PARAMETERS   : 
  None

DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_plmn_sys_sel_pref_err_cmd_hdlr
(
  void
);

/*===========================================================================
FUNCTION DS_3GPP_PLMN_SM_SYS_SEL_PREF_NO_ERR_CMD_HDLR
 
DESCRIPTION
  This function handles when sys selection enable/disable has resulted in
  an no error, This function clears all the retry/timer values
  
PARAMETERS   : 
  None

DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_plmn_sys_sel_pref_no_err_cmd_hdlr
(
  void
);

/*===========================================================================
FUNCTION  DS_3GPP_PLMN_SYS_SEL_RETRY_TIMER_EXPIRY_CMD_HDLR

DESCRIPTION
  This function would retry to enable/disable sys selection

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_plmn_sys_sel_retry_timer_expiry_cmd_hdlr
(
   void
);

/*===========================================================================
FUNCTION  DS_3GPP_PLMN_SYS_SEL_RETRY_TIMER_EXPIRY

DESCRIPTION
  This function clears sys_sel_retry_sm 

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_plmn_sys_sel_retry_timer_expiry
(
  void *cb_data
);

/*===========================================================================
FUNCTION  DS_3GPP_PLMN_SYS_SET_USER_PREF_PARAMS

DESCRIPTION
  This function sets the sys_sel_pref_params, This is needed to 
  send out the LTE disable retry request

PARAMETERS 
  sys_sel_pref_params_p 
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_plmn_sys_set_user_pref_params
(
   cm_cmd_user_pref_update_type *user_pref_params_p
);

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_TIMER_EXP_CMD_HDLR
 
DESCRIPTION
  This function is used to process the throttling timer expiry
  
  
PARAMETERS  
  callback_data - points to the core throtlling state machine.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  
===========================================================================*/
void ds_3gppi_throt_sm_timer_exp_cmd_hdlr
(
  unsigned long callback_data
);

/*===========================================================================
FUNCTION DS_3GPP_THROTTLE_SM_INCREMENT_FAILURE_COUNTER

DESCRIPTION
  This function is used to increment the failure timer

PARAMETERS   :

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void ds_3gpp_throttle_sm_increment_failure_counter
(
   ds_3gppi_throttle_sm_s              *throt_sm_ptr
);

#endif /* DS_3GPP_THROTTLE_SM_H */

