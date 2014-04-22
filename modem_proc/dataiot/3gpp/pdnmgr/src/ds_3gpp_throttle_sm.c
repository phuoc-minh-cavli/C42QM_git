/*!
  @file
  ds_3gpp_throttle_sm.c

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2018 - 2020 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/src/ds_3gpp_throttle_sm.c#4 $

when         who     what, where, why
--------     ---     --------------------------------------------------------
09/05/11     ss      Changes to support RPM.
08/30/10     hs      Initial version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"

//#include "ds_3gppi_utils.h"
#include "modem_mem.h"
#include "ds_3gpp_throttle_sm.h"
#include "ds_3gpp_pdn_throttle_sm.h"
#include "ds_pdn_nv_manager.h"
//#include "dsutil.h"
#include "ran.h"
#include "ps_utils.h"
//#include "ds3gmgr.h"
//#include "ds3gmshif.h"
#include "ds_pdn_hdlr.h"
#include "sys_v.h"
#include "dssysmgr.h"
//#include "ds3geventmgr.h"
#include "ds3gpp_api.h"
#include "ds_3gpp_pdn_throt_config.h"
#include "ds_3gpp_srv_req_throt_config.h"
#include "ds_3gpp_srv_req_throt_sm.h"
#include "ds_3gpp_pdn_throttle_sm.h"
#include "ds_3gpp_cc_sp_throt_sm.h"

#include <stringl/stringl.h>
#include "ds_pdn_manager.h"

#ifdef FEATURE_DATA_RPM
#include "ds_3gpp_rpm.h"
#endif /* FEATURE_DATA_RPM */
#include "cm_gw.h"

#include "ds_pdn_apm.h"
#include "ds_eps_pdn_manager.h"
#include "ds_3gpp_pdn_limit_hdlr.h"
#include "ds_3gpp_network_override_hdlr.h"
#include "ds_3gpp_pdn_throt_rules.h"
#include "ds_defs.h"

/*===========================================================================

                         GLOBAL VARIABLES

===========================================================================*/

/*-----------------------------------------------------------------------------
  Global critical section needed to protect the SM info.
  The timer call back modified the state info of the Throttle SM. Since this
  happens in a context that is different from the DS task context, we have to
  protect access to the SM info.
-----------------------------------------------------------------------------*/
ds_crit_sect_type ds_3gppi_throttle_sm_crit_sect;

/*------------------------------------------------------------------------------
  This is the flag that indicates if the power up procedures have been completed
  for the core state machine.
------------------------------------------------------------------------------*/
static boolean ds_3gppi_is_core_sm_init_performed = FALSE;

/*---------------------------------------------------------------------------------
  This table stores information specific to PDN and Service Request
  Throttle module.
  tmr_exp_f:                 Function to be invoked when Throttle Timer expires
  tmr_start_f:               Function to be invoked when Throttle Timer starts
  get_tmr_val_f:             Function to fetch the Throttle timer value
-----------------------------------------------------------------------------------*/
static ds_3gpp_throt_sm_info_tbl_s
         *ds_3gpp_throt_sm_info_tbl_p[DS_3GPP_THROT_TYPE_MAX];  

/*---------------------------------------------------------------------------------
  This macro checks whether the SM Type is valid
-----------------------------------------------------------------------------------*/
#define DS_3GPP_THROT_VALIDATE_SM_TYPE(sm_type) \
          (sm_type >= DS_3GPP_THROT_TYPE_MAX || sm_type < DS_3GPP_THROT_TYPE_MIN)? \
		        FALSE: TRUE

/*---------------------------------------------------------------------------
  Specifies the no. & size of PDN throttle config params
  10 failure timer(for global throttling)
---------------------------------------------------------------------------*/
#define DS_UTIL_MAX_NUM_THROTTLE_PARAMS 10 
#define DS_UTIL_MAX_THROTTLE_PARAM_NAME_SIZE 20


/*-------------------------------------------------------------------------- 
  This structure is used to determine the retry timer and at what intervals
  should the retry occur
  --------------------------------------------------------------------------*/
typedef struct
{
  uint16                                 retry_timer_val;
  ps_timer_handle_type                   sys_sel_retry_timer; 
  cm_plmn_blocking_info_s_type           plmn_blocking_info;
  cm_cmd_user_pref_update_type           user_pref_params;
  ds_3gpp_plmn_sys_sel_retry_e_type      retry_type;
}ds_3gpp_plmn_sys_sel_retry_sm_s;

static ds_3gpp_plmn_sys_sel_retry_sm_s ds_3gpp_plmn_sys_sel_retry_sm = {NULL};

/*---------------------------------------------------------------------------
  An array of strings to map the throttle name string (mentioned in EFS 
  file) to a token ID. This token ID will be used to modify the correct entry
  in the internal data structure. The token ID is the index of the string 
  entry in the array.
---------------------------------------------------------------------------*/
static char ds_util_throt_param_name
      [DS_UTIL_MAX_NUM_THROTTLE_PARAMS][DS_UTIL_MAX_THROTTLE_PARAM_NAME_SIZE] = 
                                                {"FAILURE_TIMER_1",
                                                 "FAILURE_TIMER_2",
                                                 "FAILURE_TIMER_3",
                                                 "FAILURE_TIMER_4",
                                                 "FAILURE_TIMER_5",
                                                 "FAILURE_TIMER_6",
                                                 "FAILURE_TIMER_7",
                                                 "FAILURE_TIMER_8",
                                                 "FAILURE_TIMER_9",
                                                 "FAILURE_TIMER_10",
                                                };
/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================
FUNCTION DS_3GPP_THROT_GET_SM_INFO_TABLE

DESCRIPTION
  This function is used by Core Throttle SM to fetch handle to the
  info table corresponding to the SM Type(PDN Or Service Request).
  The table specifies attributes such as start timer callback,
  stop timer callback, Get Timer Value Callback

PARAMETERS   :
  throt_sm_type: SM Type (PDN Or Service Request)
  subs_id:       Subscription Id

DEPENDENCIES
  None

RETURN VALUE
  Handle to the info table

SIDE EFFECTS
  None
  
===========================================================================*/
static ds_3gpp_throt_sm_info_tbl_s* ds_3gpp_throt_get_sm_info_tbl
(
  ds_3gpp_throt_sm_type    throt_sm_type
);

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_TIMER_CB
 
DESCRIPTION
  This function is the timer call back for the core state machine timer.
  This changes the state of the SM from throttled to unthrottled. Since this
  runs in the timer context
  
  
PARAMETERS  
  callback_data - points to the core throtlling state machine.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  If anything more than such light operation (changing the state of the SM)
  were needed, post a command and process in DS context instead of working in
  the timer context.
  
===========================================================================*/
void ds_3gppi_throt_sm_timer_cb
(
  void* callback_data
);

/*--------------------------------------------------------------------------
  If the global throttling NV is not enabled, pdn_throttle.failure_timers from 
  7 - 10 will be updated with the 6th failure timer value. so even if we increase 
  counter to 10 for 7-10 we will get timer value as 6th failure value only
----------------------------------------------------------------------------*/
#define DS_3GPP_THROTTLE_SM_INCREMENT_FAILURE_COUNTER(count) \
  {\
    if (count < DS_3GPP_PDN_THROTTLE_MAX_FAILURE_TIMERS)\
      count++;\
  }

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
LOCAL boolean ds_3gpp_throt_sm_is_plmn_non_null
(
  sys_plmn_id_s_type plmn_id
);

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_COMMON_RULE_CONDITION

DESCRIPTION
  This function defines the conditions under which the common throttling
  rule should pass
  
PARAMETERS  
  pdn_cntx_p             - PDN Context
  mode_info_p            - Mode Info
  current_bearer_mode    - Sys Mode
  down_reason            - PS Net Down Reason
    
DEPENDENCIES 
  None
  
RETURN VALUE 
  TRUE  - If the rule passes
  FALSE - Otherwise

SIDE EFFECTS 
  None.
  
===========================================================================*/
static boolean ds_3gpp_throt_sm_common_rule_condition
(
  ds_pdn_mgr_s                  *pdn_cntx_p,
  ds_bearer_deact_params_s  *mode_info_p,
	sys_sys_mode_e_type            sys_mode,
  ds_net_down_reason_type        down_reason
);

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_COMMON_RULE_ACTION

DESCRIPTION
  This function defines the actions to take when the common throttling
  rule passes
  
PARAMETERS  
  pdn_cntx_p             - PDN Context
  mode_info_p            - Mode Info
  current_bearer_mode    - Sys Mode
  down_reason            - PS Net Down Reason
    
DEPENDENCIES 
  None
  
RETURN VALUE 
  TRUE  - If action is successful
  FALSE - Otherwise

SIDE EFFECTS 
  None
  
===========================================================================*/
static boolean ds_3gpp_throt_sm_common_rule_action
(
  ds_pdn_mgr_s              *pdn_cntx_p,
  ds_bearer_deact_params_s      *mode_info_p,
	sys_sys_mode_e_type            sys_mode,
  ds_net_down_reason_type        down_reason
);

/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_INIT

DESCRIPTION
  This function initializes the throttling state machine
  
PARAMETERS  
  None.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.

SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_throt_sm_init(void)
{
  ds_pdn_throt_rule_s      rule;
  /*-------------------------------------------------------------------*/
  DS_INIT_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
  rule.condition_f = ds_3gpp_throt_sm_common_rule_condition;
  rule.action_f = ds_3gpp_throt_sm_common_rule_action;

  ds_pdn_throt_rule_register(DS_PDN_THROT_RULE_COMMON, &rule);

  if(ds_3gppi_is_core_sm_init_performed == FALSE)
  {
    DS_MSG0(MSG_LEGACY_HIGH,"Core throttling SM init - done only once");
    /*--------------------------------------------------------------------------
      Set the boolean to TRUE so that we do not do initialization when a 2nd
      core sm is created.
    --------------------------------------------------------------------------*/
    ds_3gppi_is_core_sm_init_performed = TRUE;
    ds_3gpp_plmn_sys_sel_retry_sm.sys_sel_retry_timer = ps_timer_alloc(
                                     ds_3gpp_plmn_sys_sel_retry_timer_expiry,0);

  }
  else
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Core throttling SM init done already. Ignore");
  }
  
  return;
}/*ds_3gpp_throt_sm_init*/


/*===========================================================================
FUNCTION DS_3GPP_THROT_UPDATE_PLMN_BLK_INTERVAL_AFTER_RETRY

DESCRIPTION
  This function returns the value for which PLN blocking has to be performed
  after retry
 
PARAMETERS  
  void
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  boolean - Indicates if a retry must be performed or not.
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
static boolean ds_3gpp_throt_update_plmn_blk_interval_and_retry
(
   void
)
{
  int32 blk_interval = 0;
  boolean ret_val = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  blk_interval =  ds_3gpp_plmn_sys_sel_retry_sm.plmn_blocking_info.
  blocking_interval - (ds_3gpp_plmn_sys_sel_retry_sm.retry_timer_val *
                       DS_3GPP_THROT_SECOND_TO_MILISECOND_MULTIPLIER);

  if (blk_interval > 0)
  {
    ds_3gpp_plmn_sys_sel_retry_sm.plmn_blocking_info.blocking_interval = 
      blk_interval;
    ret_val = TRUE;
  }

  DS_MSG3(MSG_LEGACY_HIGH,"ds_3gpp_plmn_update_plmn_blk_interval_and_retry returns %d",
                    ret_val,0,0);
  return ret_val;

}

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_TIMER_CB

DESCRIPTION
  This function is the timer call back for the core state machine timer.
  This changes the state of the SM from throttled to unthrottled. Since this
  runs in the timer context
  
PARAMETERS  
  callback_data - points to the core throtlling state machine.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  If anything more than such light operation (changing the state of the SM)
  were needed, post a command and process in DS context instead of working in
  the timer context.
  
===========================================================================*/
void ds_3gppi_throt_sm_timer_cb
(
  void* callback_data
)
{
  ds_command_type    *cmd_buf     = NULL;
  unsigned long  *cb_data_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_buf = ds_allocate_cmd_buf(sizeof(unsigned long));
  if( (NULL == cmd_buf) || (NULL == cmd_buf->cmd_payload_ptr) )
  {
    return;
  }
  else
  {
    cmd_buf->hdr.cmd_id = DS_CMD_3GPP_THROT_TMR_EXP;
    cb_data_ptr = (unsigned long*)cmd_buf->cmd_payload_ptr;
    *cb_data_ptr = (unsigned long)callback_data;
    ds_put_cmd(cmd_buf);
  }
} /* ds_3gppi_throt_sm_timer_cb */

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
LOCAL void ds_3gppi_throt_sm_timer_exp_cmd_hdlr
(
  unsigned long callback_data
)
{
  ds_3gppi_throttle_sm_s                *throt_sm_ptr = NULL;
  ds_3gpp_throt_sm_info_tbl_s           *cb_tbl_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  throt_sm_ptr = (ds_3gppi_throttle_sm_s*) *((unsigned long*)callback_data);

  if(throt_sm_ptr != NULL &&
     throt_sm_ptr->cookie == DS_3GPP_THROT_SM_COOKIE)
  {
    DS_ENTER_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);

    throt_sm_ptr->is_throttled = FALSE;

    cb_tbl_ptr = ds_3gpp_throt_get_sm_info_tbl(throt_sm_ptr->throt_sm_type);

    if (cb_tbl_ptr != NULL && cb_tbl_ptr->tmr_exp_cb !=NULL)
    {
      cb_tbl_ptr->tmr_exp_cb(throt_sm_ptr, throt_sm_ptr->tmr_exp_cb_data_p);
    }

    DS_LEAVE_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
  }
  else
  {
    DS_MSG0(MSG_LEGACY_ERROR,"NULL CB Data in throttle timer CB");
  }
} /* ds_3gppi_throt_sm_timer_exp_cmd_hdlr */

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
  void                         *parent_ptr
)
{
  ds_3gppi_throttle_sm_s* throt_sm_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*----------------------------------------------------------------------------
    Allocate a core state machine from heap. Return pointer to this core SM.
  ----------------------------------------------------------------------------*/

  DS_MSG0(MSG_LEGACY_HIGH,"Get core throttling SM ");
  DS_SYSTEM_HEAP_MEM_ALLOC(throt_sm_ptr,sizeof(ds_3gppi_throttle_sm_s),
                              ds_3gppi_throttle_sm_s*);
  if(throt_sm_ptr != NULL)
  {
    /*--------------------------------------------------------------------------
      Initialize the state machine with the correct values.
    --------------------------------------------------------------------------*/
    throt_sm_ptr->throt_sm_type = throt_sm_type;
    throt_sm_ptr->is_throttled = FALSE;
    throt_sm_ptr->failure_count = 0;
    throt_sm_ptr->throt_tmr_val = 0;
    throt_sm_ptr->tmr_exp_cb_data_p = NULL;
    throt_sm_ptr->parent_ptr = parent_ptr;
    throt_sm_ptr->cookie = DS_3GPP_THROT_SM_COOKIE;
    throt_sm_ptr->lwr_lyr_failure_cnt = 0;

    memscpy((void*)&(throt_sm_ptr->plmn_id),
    sizeof(sys_plmn_id_s_type),
           (void*)&(plmn_id), 
           sizeof(sys_plmn_id_s_type));

    /*--------------------------------------------------------------------------
      Initialize the throttling timer
      The call back data will be the pointer to the core state machine
    --------------------------------------------------------------------------*/
    throt_sm_ptr->throttle_timer = ps_timer_alloc(ds_3gppi_throt_sm_timer_cb,
                                                   (void*)throt_sm_ptr);

    memset(&throt_sm_ptr->user_data,0,sizeof(throt_sm_ptr->user_data));
    throt_sm_ptr->user_data.auth_info_p = NULL;
  }
  return throt_sm_ptr;

} /* ds_3gppi_throt_sm_get_core_throttle_sm */

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
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*----------------------------------------------------------------------------
    Free the core state machine
  ----------------------------------------------------------------------------*/
  if (throt_sm_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_HIGH,"NULL throttle_sm_ptr");
    return;
  }

  if(throt_sm_ptr->user_data.auth_info_p != NULL)
  {
    DS_SYSTEM_HEAP_MEM_FREE(throt_sm_ptr->user_data.auth_info_p);
    throt_sm_ptr->user_data.auth_info_p = NULL;
  }
  PS_TIMER_FREE_HANDLE(throt_sm_ptr->throttle_timer);
  memset(throt_sm_ptr, 0, sizeof(ds_3gppi_throttle_sm_s));
  DS_SYSTEM_HEAP_MEM_FREE(throt_sm_ptr);
} /* ds_3gppi_throt_sm_free_core_state_machine */

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
  ds_3gppi_throttle_sm_s              *throt_sm_ptr,
  ds_3gpp_throt_rab_info_s            *rab_throt_info,
  ds_3gpp_throttle_sm_cb_data_type    *cb_data_p
)
{
  ds_3gpp_throt_sm_info_tbl_s   *cb_tbl_ptr = NULL;
  void                          *tmr_start_cb_data_p = NULL;
  void                          *get_tmr_val_cb_data_p = NULL;
  void                          *tmr_exp_cb_data_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(throt_sm_ptr != NULL)
  {
    DS_MSG3(MSG_LEGACY_HIGH,"is_throt_flag: %d, fail_count: %d, timer_val: %d",
                      throt_sm_ptr->is_throttled, throt_sm_ptr->failure_count,
                      throt_sm_ptr->throt_tmr_val);
    /*--------------------------------------------------------------------------
      All operations in this function are to be done in a critical section.
      This is because we will be changing the info in the SM. Since this can
      happen from different context, we want to ensure that the SM is in a 
      consistent state
    --------------------------------------------------------------------------*/
    DS_ENTER_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);

    if (cb_data_p != NULL)
    {
      tmr_start_cb_data_p = cb_data_p->tmr_start_cb_data_p;
      get_tmr_val_cb_data_p = cb_data_p->get_tmr_val_cb_data_p;
      tmr_exp_cb_data_p = cb_data_p->tmr_exp_cb_data_p;
    }

    throt_sm_ptr->tmr_exp_cb_data_p = tmr_exp_cb_data_p;


    /*--------------------------------------------------------------------------
        Get the timer value so that we can start the throttle timer
    --------------------------------------------------------------------------*/
    throt_sm_ptr->throt_tmr_val = 0; 

    cb_tbl_ptr = 
        ds_3gpp_throt_get_sm_info_tbl(throt_sm_ptr->throt_sm_type);
     
    /*------------------------------------------------------------------------ 
      Currently inc_cntr_cb is registered only for srv req throttling
      For PDN throttling simply increment failure counter.
      ------------------------------------------------------------------------*/
    if (cb_tbl_ptr != NULL && cb_tbl_ptr->inc_cntr_cb != NULL)
    {
      cb_tbl_ptr->inc_cntr_cb(throt_sm_ptr,(void *)rab_throt_info);
    }

    if (cb_tbl_ptr != NULL && cb_tbl_ptr->get_tmr_val_cb != NULL)
    {
      throt_sm_ptr->throt_tmr_val = 
        cb_tbl_ptr->get_tmr_val_cb(throt_sm_ptr, get_tmr_val_cb_data_p);
    }

    if(throt_sm_ptr->throt_tmr_val > 0)
    {
      /*------------------------------------------------------------------------
        Change the state of the SM to throttled
      ------------------------------------------------------------------------*/
      throt_sm_ptr->is_throttled = TRUE;

      if (cb_tbl_ptr != NULL && cb_tbl_ptr->tmr_start_cb !=NULL)
      {
	    	cb_tbl_ptr->tmr_start_cb(throt_sm_ptr, tmr_start_cb_data_p);
      }

      if (throt_sm_ptr->throt_tmr_val != DS_3GPP_PDN_THROT_INFINITE_THROT)
      {
        /*------------------------------------------------------------------------
          Now start the timer with the value that was received
        ------------------------------------------------------------------------*/
        ps_timer_start(throt_sm_ptr->throttle_timer, 
                            throt_sm_ptr->throt_tmr_val);
        DS_MSG1(MSG_LEGACY_HIGH,"Started throttling timer for %d ms",
                         throt_sm_ptr->throt_tmr_val);

      }

    } /* if throttling timer greater than zero */
    DS_LEAVE_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);

    DS_MSG3(MSG_LEGACY_HIGH,"is_throt_flag: %d, fail_count: %d, timer_val: %d",
                      throt_sm_ptr->is_throttled, throt_sm_ptr->failure_count,
                      throt_sm_ptr->throt_tmr_val);
  }
  else
  {
    DS_MSG0(MSG_LEGACY_HIGH,"Null pointer in core sm feed failure fn. Ignore");
  }
} /* ds_3gppi_throt_sm_feed_failure_to_core_sm */

/*===========================================================================
FUNCTION DS_3GPPI_GET_IS_THROTTLED_FLAG

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
)
{
  boolean ret_val = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (throt_sm_ptr)
  {
    DS_ENTER_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
    ret_val = throt_sm_ptr->is_throttled;
    DS_LEAVE_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
  }

 return ret_val;
} /* ds_3gppi_throt_sm_get_is_throttled_flag */

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
)
{
  if (throt_sm_ptr != NULL)
  {
    DS_ENTER_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
    throt_sm_ptr->is_throttled = is_throttled;
    DS_LEAVE_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
    DS_MSG1(MSG_LEGACY_HIGH,"IS_THROTTLED flag set to %d",is_throttled);
  }
  else
  {
    DS_MSG0(MSG_LEGACY_ERROR,"NULL argument passed in set is_throttled. Ignore");
  }
} /* ds_3gppi_throt_sm_set_is_throttled_flag */

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
)
{
  boolean ret_val = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(throt_sm_ptr != NULL)
  {
    DS_ENTER_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
    *failure_count_ptr = throt_sm_ptr->failure_count;
    DS_LEAVE_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
    ret_val = TRUE;
  }
  else
  {
    DS_MSG0(MSG_LEGACY_ERROR,"NULL pointer in core sm get count function.");
  }

  return ret_val;
} /* ds_3gppi_throt_sm_get_failure_count */

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
)
{
  boolean ret_val = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(throt_sm_ptr != NULL)
  {
    DS_ENTER_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
    throt_sm_ptr->failure_count = failure_count;
    DS_LEAVE_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
    ret_val = TRUE;
  }
  else
  {
    DS_MSG0(MSG_LEGACY_ERROR,"NULL argument passed in set fail count. Ignore");
  }
  return ret_val;
} /* ds_3gppi_throt_sm_set_failure_count */

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
)
{
  uint32 ret_val = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_ENTER_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
  
  if(throt_sm_ptr != NULL)
  {
    ret_val = throt_sm_ptr->throt_tmr_val;
  }
  
  DS_LEAVE_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);

  return ret_val;
} /* ds_3gppi_throt_sm_get_throttle_timer_val */

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
)
{
  sys_plmn_id_s_type ret_val;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset((void*)&ret_val,0,sizeof(sys_plmn_id_s_type));

  if (throt_sm_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"NULL throttle_sm_ptr");
    return ret_val;
  }
  DS_ENTER_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
  ret_val = throt_sm_ptr->plmn_id;
  DS_LEAVE_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);

  return ret_val;
} /* ds_3gppi_throt_sm_get_plmn_id */

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
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (throt_sm_ptr != NULL)
  {
    DS_ENTER_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
    throt_sm_ptr->throt_tmr_val = timer_val;
    DS_LEAVE_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
    DS_MSG1(MSG_LEGACY_HIGH,"Throttle timer value set to %d",timer_val);
  }
  else
  {
    DS_MSG0(MSG_LEGACY_ERROR,"NULL argument passed in set throttle_timer_val. Ignore");
  }
} /* ds_3gppi_throt_sm_set_throttle_timer_val */

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
)
{
  uint32 ret_val = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_ENTER_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
  if(throt_sm_ptr->throt_tmr_val == DS_3GPP_PDN_THROT_INFINITE_THROT)
  {
    ret_val = DS_3GPP_PDN_THROT_INFINITE_THROT;
  }
  else
  {
    ret_val = ps_timer_remaining(throt_sm_ptr->throttle_timer);
  }

  DS_LEAVE_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);

  return ret_val;
} /* ds_3gppi_throt_sm_get_remaining_throttle_time */

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
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(throt_sm_ptr != NULL)
  {
    DS_MSG_SPRINTF_1(MSG_LEGACY_HIGH, 
                            "Clearing the is_throt_flag in core sm for apn: %s",
                            throt_sm_ptr->apn);

    DS_ENTER_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
    throt_sm_ptr->is_throttled = FALSE;
    DS_LEAVE_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
    ps_timer_cancel(throt_sm_ptr->throttle_timer);
  }
  else
  {
    DS_MSG0(MSG_LEGACY_ERROR,"NULL throttle_sm_ptr");
  }
} /* ds_3gppi_throt_sm_clear_throttle_timer */

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
boolean ds_3gpp_throt_sm_is_plmn_non_null(sys_plmn_id_s_type plmn_id)
{
  boolean            ret_val = FALSE;
  sys_plmn_id_s_type zero_plmn_id;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset((void*)&zero_plmn_id, 0, sizeof(sys_plmn_id_s_type));

  if(memcmp((void*)&(plmn_id),
            (void*)&(zero_plmn_id), 
            sizeof(sys_plmn_id_s_type)) != 0)
  {
    DS_MSG0(MSG_LEGACY_HIGH,"PLMN being inspected is non-zero");
    ret_val = TRUE;
  }
  else
  {
    DS_MSG0(MSG_LEGACY_HIGH,"PLMN being inspected is zero");
  }
  return ret_val;
} /* ds_3gpp_throt_sm_is_plmn_non_null */


/*========================================================================= 
  FUNCTION ds_3gpp_throt_sm_reset_core_throt_sm

DESCRIPTION
  This function resets the throttle statae machine 
  paramaters asscoiated with release 10 paramters in case of per plmn
  
PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine

DEPENDENCIES 
  None.

RETURN VALUE 

   
SIDE EFFECTS 
  None.

===========================================================================*/

void ds_3gpp_throt_sm_reset_core_throt_sm
(
  void*  throt_sm_ptr
)
{
  ds_3gppi_throttle_sm_s *throt_sm_local_ptr = NULL;
  throt_sm_local_ptr = (ds_3gppi_throttle_sm_s*)throt_sm_ptr;

  ds_3gppi_throt_sm_set_is_throttled_flag(
     throt_sm_local_ptr,
     FALSE);

  ds_3gppi_throt_sm_set_throttle_timer_val(
     throt_sm_local_ptr,
     0);
} /* ds_3gpp_throt_sm_reset_core_throt_sm */

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
)
{
   ds_3gpp_throt_user_data  *throt_user_data_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(throt_sm_ptr != NULL)
  {
    DS_ENTER_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
    throt_user_data_p = &throt_sm_ptr->user_data;
    DS_LEAVE_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
  }
  else
  {
    DS_MSG0(MSG_LEGACY_ERROR,"NULL argument passed in clear_user_data. Ignore");
  }

  return throt_user_data_p;
} /* ds_3gppi_throt_sm_get_user_data */

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_COMMON_RULE_CONDITION

DESCRIPTION
  This function defines the conditions under which the common throttling
  rule should pass
  
PARAMETERS  
  pdn_cntx_p             - PDN Context
  mode_info_p            - Mode Info
  current_bearer_mode    - Sys Mode
  down_reason            - PS Net Down Reason
    
DEPENDENCIES 
  None
  
RETURN VALUE 
  TRUE  - If the rule passes
  FALSE - Otherwise

SIDE EFFECTS 
  None.
  
===========================================================================*/
static boolean ds_3gpp_throt_sm_common_rule_condition
(
  ds_pdn_mgr_s                  *pdn_cntx_p,
  ds_bearer_deact_params_s  *mode_info_p,
	sys_sys_mode_e_type            sys_mode,
  ds_net_down_reason_type        down_reason
)
{
  boolean ret_val = FALSE;
  /*--------------------------------------------------------*/

  switch (down_reason)
  {
    case DS_NET_DOWN_REASON_SM_NAS_SRV_REQ_FAILURE:
    {
      /* For SRV Request Failure, processing under common
         rule is applicable only if Sys mode is UMTS */
      if (sys_mode != SYS_SYS_MODE_LTE)
      {
        ret_val = TRUE;
      }
      break;
    }

    default:
    {
      break;
    }
  }

  return ret_val;
} /* ds_3gpp_throt_sm_common_rule_condition */

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_COMMON_RULE_ACTION

DESCRIPTION
  This function defines the actions to take when the common throttling
  rule passes
  
PARAMETERS  
  pdn_cntx_p             - PDN Context
  mode_info_p            - Mode Info
  current_bearer_mode    - Sys Mode
  down_reason            - PS Net Down Reason
    
DEPENDENCIES 
  None
  
RETURN VALUE 
  TRUE  - If action is successful
  FALSE - Otherwise

SIDE EFFECTS 
  None
  
===========================================================================*/
static boolean ds_3gpp_throt_sm_common_rule_action
(
  ds_pdn_mgr_s                  *pdn_cntx_p,
  ds_bearer_deact_params_s  *mode_info_p,
	sys_sys_mode_e_type            sys_mode,
  ds_net_down_reason_type        down_reason
)
{
  void                   *sm_ptr = NULL;
  /*---------------------------------------------------------*/

  switch (down_reason)
  {
    case DS_NET_DOWN_REASON_SM_NAS_SRV_REQ_FAILURE:
    {
      sm_ptr = ds_3gpp_srv_req_get_throt_sm(TRUE);
      
      if(ds_3gpp_srv_req_throt_get_is_throttled_flag(sm_ptr) == FALSE)
      {
        /*----------------------------------------------------------------------
          Now feed the failure into the system wide throttling state machine.
          Get the timer value to use from the sm
          Call this function with argument TRUE meaning we need to create a sm
          if it does not already exists. 
        ----------------------------------------------------------------------*/
        ds_3gpp_srv_req_throt_feed_failure(sm_ptr,NULL);
      }
      break;
    }

    default:
    {
      //Take no action
      break;
    }
  }

  return TRUE;
} /* ds_3gpp_throt_sm_common_rule_action */

/*===========================================================================
FUNCTION DS_3GPP_THROT_GET_SM_INFO_TABLE

DESCRIPTION
  This function is used by Core Throttle SM to fetch handle to the
  info table corresponding to the SM Type(PDN Or Service Request).
  The table specifies attributes such as start timer callback,
  stop timer callback, Get Timer Value Callback

PARAMETERS   :
  throt_sm_type: SM Type (PDN Or Service Request)
  subs_id:       Subscription Id

DEPENDENCIES
  None

RETURN VALUE
  Handle to the info table

SIDE EFFECTS
  None
  
===========================================================================*/
static ds_3gpp_throt_sm_info_tbl_s* ds_3gpp_throt_get_sm_info_tbl
(
  ds_3gpp_throt_sm_type    throt_sm_type
)
{
  if (DS_3GPP_THROT_VALIDATE_SM_TYPE(throt_sm_type) == FALSE)
  {
	  return NULL;
  }

  return ds_3gpp_throt_sm_info_tbl_p[throt_sm_type];
} /* ds_3gpp_throt_get_sm_info_tbl */

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
  None.
  
RETURN VALUE 
  None.
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_throt_fill_cb_data
(
  ds_3gpp_throttle_sm_cb_data_type *cb_data_p,
  void                             *tmr_start_cb_data_p,
  void                             *tmr_exp_cb_data_p,
  void                             *tmr_get_val_cb_data_p 
)
{
  if (cb_data_p != NULL)
  {
    cb_data_p->tmr_start_cb_data_p = tmr_start_cb_data_p;
    cb_data_p->tmr_exp_cb_data_p = tmr_exp_cb_data_p;
    cb_data_p->get_tmr_val_cb_data_p = tmr_get_val_cb_data_p;
  }
} /* ds_3gpp_throt_fill_cb_data */

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
  ds_3gpp_throt_sm_info_tbl_s     *cb_tbl_ptr,
  ds_3gpp_throt_sm_type            throt_sm_type
)
{

  ds_local_err_val_e_type err_val = DS_LOCAL_ERR_VAL_NONE;
  
  if (DS_3GPP_THROT_VALIDATE_SM_TYPE(throt_sm_type) == FALSE)
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  if (cb_tbl_ptr == NULL) 
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }

  if (ds_3gpp_throt_sm_info_tbl_p[throt_sm_type] == NULL)
  {
		DS_SYSTEM_HEAP_MEM_ALLOC(ds_3gpp_throt_sm_info_tbl_p[throt_sm_type],
		        sizeof(ds_3gpp_throt_sm_info_tbl_s), ds_3gpp_throt_sm_info_tbl_s *);

    if (ds_3gpp_throt_sm_info_tbl_p[throt_sm_type] != NULL)
    {
      memset ((void*)ds_3gpp_throt_sm_info_tbl_p[throt_sm_type],
    	        0, sizeof(ds_3gpp_throt_sm_info_tbl_s));
    
      memscpy ((void*)ds_3gpp_throt_sm_info_tbl_p[throt_sm_type], 
    	         sizeof(ds_3gpp_throt_sm_info_tbl_s),
    	        (void*)cb_tbl_ptr,
    	        sizeof(ds_3gpp_throt_sm_info_tbl_s));
    }
    else
    {
      err_val = DS_LOCAL_ERR_VAL_2;  
    }
  }
  else
  {
    err_val = DS_LOCAL_ERR_VAL_3;  
  }
func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_ERROR,"ds_3gpp_throt_register_sm_info_tbl err_val: %d",err_val);
  return;
} /* ds_3gpp_throt_register_sm_info_tbl */


/*===========================================================================
FUNCTION      DS_UTIL_GET_THROTTLE_TOKEN_ID

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
  char  *from,
  char  *to,
  uint8 *token_id
)
{
  int16 ret_val = -1; /* return value */
  uint8 i;            /* counter for number of params */
  uint8 length;       /* length of the token (param name) */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the length of the string that is passed */
  if  ( from > to )  
  {
    DS_MSG0(MSG_LEGACY_ERROR, "Invalid Parameters");
    return ret_val;
  }

  /* Assumption that the token length will not be greater than 255 */
  length =(uint8)(to-from);

  for (i=0; i<DS_UTIL_MAX_NUM_THROTTLE_PARAMS; i++)
  {
    if (length == strlen(ds_util_throt_param_name[i]))
    {
      if (0 == strncasecmp(from,ds_util_throt_param_name[i],length))
      {
        *token_id = i;
        ret_val = 0;
        break;
      }
    }
  }
  return ret_val;
} /* ds_util_get_throttle_token_id */

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
)
{
  list_type				 *list_ptr = NULL;
  boolean 		 	         ret = FALSE;
  ds_3gpp_pdn_cntxt_blocked_plmn_type    *curr_blocked_plmn_ptr;
  /*-----------------------------------------------------------------------*/

  list_ptr = ds_pdn_hdlr_get_blocked_plmn_list();
  if (list_ptr == NULL||list_size(list_ptr) == 0)
  {
    return FALSE;
  }

  curr_blocked_plmn_ptr = 
      (ds_3gpp_pdn_cntxt_blocked_plmn_type*)list_peek_front( list_ptr );

  while (curr_blocked_plmn_ptr != NULL)
  {
    if ( memcmp(&(curr_blocked_plmn_ptr->plmn_id),
                &plmn_id,
                sizeof(sys_plmn_id_s_type)) == 0)
    {
        ret = TRUE;
        break;
    }
          
    /* Get the next item in the list */
    curr_blocked_plmn_ptr = 
     list_peek_next(list_ptr, &(curr_blocked_plmn_ptr->link));
  }
  DS_MSG1(MSG_LEGACY_HIGH,"ds_3gpp_pdn_cntxt_is_plmn_blocked returns %d",ret);
  return ret;
}/*ds_3gpp_pdn_cntxt_is_plmn_blocked*/

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
)
{
  list_type                            *list_ptr = NULL;
  boolean                               ret = FALSE;
  ds_3gpp_pdn_cntxt_blocked_plmn_type  *curr_blocked_plmn_ptr = NULL;
  /*-----------------------------------------------------------------------*/
  
  list_ptr = ds_pdn_hdlr_get_blocked_plmn_list();
  if (list_ptr == NULL||list_size(list_ptr) == 0)
  {
    return FALSE;
  }

  curr_blocked_plmn_ptr =
      (ds_3gpp_pdn_cntxt_blocked_plmn_type*)list_peek_front( list_ptr );

  while (curr_blocked_plmn_ptr != NULL)
  {
    if ( memcmp(&(curr_blocked_plmn_ptr->plmn_id),
                &plmn_id,
                sizeof(sys_plmn_id_s_type)) == 0)
    {
      if (curr_blocked_plmn_ptr->cause == cause)
      {
        ret = TRUE;
        break;
      }
    }

    /* Get the next item in the list */
    curr_blocked_plmn_ptr =
     list_peek_next(list_ptr, &(curr_blocked_plmn_ptr->link));
  }
  DS_MSG1(MSG_LEGACY_HIGH,"ds_3gpp_pdn_cntxt_is_plmn_blocked returns %d",ret);
  return ret;
}/*ds_3gpp_pdn_cntxt_is_plmn_blocked*/

/*===========================================================================
FUNCTION      DS_3GPP_ADD_PLMN_TO_BLOCKED_LIST

DESCRIPTION
  This util function adds the PLMN that is being passed to it to the
  blocked list if not already blocked. 

PARAMETERS
  plmn_id_ptr - pointer to the PLMN that needs to be blocked
  subs_id     - Subscription ID
 
RETURN VALUE
  TRUE  --- If PLMN was successfully blocked
  FALSE --- Otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean ds_3gpp_add_plmn_to_blocked_list
(
  sys_plmn_id_s_type             plmn_id,
  sys_block_plmn_cause_e_type    cause
)
{
  list_type                             *list_ptr = NULL;
  ds_3gpp_pdn_cntxt_blocked_plmn_type   *curr_blocked_plmn_ptr, 
                                        *new_blocked_plmn_ptr;
  boolean                                plmn_found =  FALSE;  
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
  list_ptr = ds_pdn_hdlr_get_blocked_plmn_list();

    if (list_ptr == NULL)
  {
    return FALSE;
  }
  
    curr_blocked_plmn_ptr =
       (ds_3gpp_pdn_cntxt_blocked_plmn_type*)list_peek_front( list_ptr );

    while (curr_blocked_plmn_ptr != NULL)
    {
      if ( memcmp(&(curr_blocked_plmn_ptr->plmn_id),
                  &plmn_id,
                  sizeof(sys_plmn_id_s_type)) == 0)
      {
        plmn_found = TRUE;
        break;
      }

      /* Get the next item in the list */
      curr_blocked_plmn_ptr =
         list_peek_next(list_ptr, &(curr_blocked_plmn_ptr->link));
    } /* Loop over all blocked list*/

  /* Add PLMN to blocked list if not present */
  if (!plmn_found)
  {
    DS_SYSTEM_HEAP_MEM_ALLOC(new_blocked_plmn_ptr,sizeof(ds_3gpp_pdn_cntxt_blocked_plmn_type),
                             ds_3gpp_pdn_cntxt_blocked_plmn_type *);

    if (new_blocked_plmn_ptr != NULL)
    {
      new_blocked_plmn_ptr->plmn_id = plmn_id;
      new_blocked_plmn_ptr->cause = cause;

      DS_MSG0(MSG_LEGACY_HIGH,"REL_13_DBG: Blocking PLMN");
      ds3g_print_plmn_id(&plmn_id);

      /*----------------------------------------------------------------------
        Push the list item into the list
      ----------------------------------------------------------------------*/
      list_push_front(list_ptr, &(new_blocked_plmn_ptr->link));
      return TRUE;
    }
    else
    {
      DS_MSG0(MSG_LEGACY_HIGH,"REL_13_DBG: Memory allocation failed for blocking PLMN");
      return FALSE;
    }
  }

  return FALSE;
} /* ds_3gpp_add_plmn_to_blocked_list */

/*===========================================================================
FUNCTION      DS_3GPP_FREE_PLMN_IF_BLOCKED

DESCRIPTION
    This util function performs the actions of retrieving a blocked PLMN
    and deallocating the memory associated with it

PARAMETERS
  PLMN ptr that needs to be checked and freed if blocked currently

RETURN VALUE
  TRUE  --- If PLMN was blocked
  FALSE --- Otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean ds_3gpp_free_plmn_if_blocked
(
  sys_plmn_id_s_type         *plmn_id_ptr
)
{
  list_type                             *list_ptr = NULL;
  ds_3gpp_pdn_cntxt_blocked_plmn_type   *curr_blocked_plmn_ptr;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
  list_ptr = ds_pdn_hdlr_get_blocked_plmn_list();

  if (list_ptr != NULL)
  {
    curr_blocked_plmn_ptr =
       (ds_3gpp_pdn_cntxt_blocked_plmn_type*)list_peek_front( list_ptr );

    while (curr_blocked_plmn_ptr != NULL)
    {
      if ( memcmp(&(curr_blocked_plmn_ptr->plmn_id),
                  plmn_id_ptr,
                  sizeof(sys_plmn_id_s_type)) == 0)
      {        
        list_pop_item(list_ptr, &(curr_blocked_plmn_ptr->link) );
        DS_SYSTEM_HEAP_MEM_FREE(curr_blocked_plmn_ptr);   

        DS_MSG0(MSG_LEGACY_HIGH,"REL_13_DBG: Freeing PLMN");
        ds3g_print_plmn_id(plmn_id_ptr);

        return TRUE;  
      }

      /* Get the next item in the list */
      curr_blocked_plmn_ptr =
         list_peek_next(list_ptr, &(curr_blocked_plmn_ptr->link));
    } /* Loop over all blocked list*/
  }

  return FALSE;
}/* ds_3gpp_free_plmn_if_blocked */

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
)
{
    
  boolean                              ret = FALSE, 
                                       plmn_blocked = FALSE;
  uint8                                failure_count = DS_THROT_LOG_PARAM_NA;
  sys_plmn_list_s_type                *eplmn_list_p;
  uint8                                eplmn_idx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*--------------------------------------------------------------------------
   Here we check if the passed PLMN is already blocked. If it is, we do not
   need to block it again.
   If it is not blocked, we block the current PLMN and add it to the blocked PLMN
   list.
  ----------------------------------------------------------------------------*/

  DS_MSG2(MSG_LEGACY_HIGH,"Trying to block PLMN for %d msecs , cause: %d",
                    throttle_timer_val, cause);

  ds3g_print_plmn_id(&plmn_id);

  if (ds_3gpp_pdn_cntxt_is_plmn_blocked_with_cause(plmn_id,SYS_BLOCK_PLMN_CAUSE_PRIORITY))
  {
    return FALSE;
  }

  if(ds_3gpp_plmn_prep_for_lte_blocking_and_log 
      (plmn_id, throttle_timer_val, failure_count, detach, cause))
  {
    DS_MSG0(MSG_LEGACY_HIGH,"Updating blocked PLMN List");

    plmn_blocked = ds_3gpp_add_plmn_to_blocked_list(plmn_id, cause);

    /* If a PLMN is blocked then check and block all ePLMNs if present */
    if (ds_pdn_nv_manager_is_rel_13_or_above_supported() && (plmn_blocked))
    {
      /* Block all ePLMNs under the parent PLMN ID if present */
      eplmn_list_p = ds_3gppi_throt_get_eplmn_list_per_plmn(
         plmn_id, DS_3GPP_THROT_RAT_LTE);

      if (eplmn_list_p)
      {
        for (eplmn_idx = 0; eplmn_idx < eplmn_list_p->length; eplmn_idx++)
      {
          /* Inform CM to block the ePLMN */
          if(ds_3gpp_plmn_prep_for_lte_blocking_and_log(
              eplmn_list_p->plmn[eplmn_idx], throttle_timer_val, 
              failure_count, detach, cause))
        {
            (void)ds_3gpp_add_plmn_to_blocked_list(
                eplmn_list_p->plmn[eplmn_idx], cause);
        }
      }
      }

      /* If there are any ePLMNs in the ntw override handler ePLMN list, then
         block them*/
      eplmn_list_p = ds_3gpp_ntw_override_get_eplmn_list_per_plmn(
         plmn_id, DS_3GPP_THROT_RAT_LTE);

      if (eplmn_list_p)
      {
        for (eplmn_idx = 0; eplmn_idx < eplmn_list_p->length; eplmn_idx++)
        {
          /* Inform CM to block the ePLMN */
          if(ds_3gpp_plmn_prep_for_lte_blocking_and_log(
              eplmn_list_p->plmn[eplmn_idx], throttle_timer_val, 
              failure_count, detach, cause))
        {
            (void)ds_3gpp_add_plmn_to_blocked_list(
                eplmn_list_p->plmn[eplmn_idx], cause);
      }
    }
  }
    } /* plmn_blocked */

    ret = plmn_blocked;

  } /* ds_3gpp_plmn_prep_for_lte_blocking_and_log */
  else
  {
    ds3g_print_plmn_id(&plmn_id);
    ret = FALSE;
    //"PLMN blocking failed ");
  }

  return ret;
} /* ds_3gpp_pdn_cntxt_disable_lte_per_plmn_selection_ex */

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
)
{
  boolean                                   ret = FALSE;
  boolean                                   found_plmn = FALSE;
  uint16                                    request_pref = 
                                              CM_PLMN_BLOCKING_PREF_PLMN_UNBLOCKING_ONLY;
  sys_plmn_list_s_type                     *eplmn_list_p;
  uint8                                     eplmn_idx;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*--------------------------------------------------------------------------   
    Here we check if the passed PLMN is unblocked. If it is not blocked, 
    no need to unblock again.
  ----------------------------------------------------------------------------*/

  DS_MSG0(MSG_LEGACY_HIGH,"Trying to unblock PLMN ");

  if (ds_3gpp_pdn_cntxt_is_plmn_blocked_with_cause(plmn_id, 
                                                   SYS_BLOCK_PLMN_CAUSE_PRIORITY))
  {
    request_pref = CM_PLMN_BLOCKING_PREF_PLMN_UNBLOCKING_FORCE_PREF;
  }

  ret = ds3g_msh_enable_lte_plmn_blocking_on_plmn_id_ex2
          (plmn_id,request_pref);

  ds3g_print_plmn_id(&plmn_id);

  if (ret)
  {
    /* Unblock PLMN ID passed to this function if present in blocked
       list */
    found_plmn = ds_3gpp_free_plmn_if_blocked(&plmn_id);

    if (ds_pdn_nv_manager_is_rel_13_or_above_supported() && (found_plmn))
    {
      /* Unblock all ePLMNs under the parent PLMN ID if present */
      eplmn_list_p = ds_3gppi_throt_get_eplmn_list_per_plmn(
         plmn_id, DS_3GPP_THROT_RAT_LTE);

      if (eplmn_list_p)
      {
        for (eplmn_idx = 0; eplmn_idx < eplmn_list_p->length; eplmn_idx++)
        {
          /* Inform CM to unblock the ePLMN */
          if(ds3g_msh_enable_lte_plmn_blocking_on_plmn_id_ex2(
             eplmn_list_p->plmn[eplmn_idx], request_pref))
          {
            (void)ds_3gpp_free_plmn_if_blocked(&eplmn_list_p->plmn[eplmn_idx]);
        }
      }
    }

      eplmn_list_p = ds_3gpp_ntw_override_get_eplmn_list_per_plmn(
         plmn_id, DS_3GPP_THROT_RAT_LTE);

      /* If there are any ePLMNs in the ntw override handler ePLMN list, then
         unblock them */
      if (eplmn_list_p)
      {
        for (eplmn_idx = 0; eplmn_idx < eplmn_list_p->length; eplmn_idx++)
        {
          /* Inform CM to unblock the ePLMN */
          if(ds3g_msh_enable_lte_plmn_blocking_on_plmn_id_ex2(
             eplmn_list_p->plmn[eplmn_idx], request_pref))
          {
            (void)ds_3gpp_free_plmn_if_blocked(&eplmn_list_p->plmn[eplmn_idx]);
    }
  }
    }
    } /* found_plmn */
  }
  else
  {
    DS_MSG0(MSG_LEGACY_ERROR,"PLMN unblocking failed ");
    ds3g_print_plmn_id(&plmn_id);
  }

  return ret;

} /* ds_3gpp_pdn_cntxt_enable_lte_plmn_blocking_on_plmn_id_ex */

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
)
{
   ds_3gpp_pdn_cntxt_blocked_plmn_type   *curr_blocked_plmn_ptr;
   ds_3gpp_pdn_cntxt_blocked_plmn_type   *next_blocked_plmn_ptr;
   list_type                             *list_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - */

  /*--------------------------------------------------------------------------   
    Here we loop through all the blocked PLMNs and unblock them one by one.
  ----------------------------------------------------------------------------*/

  list_ptr = ds_pdn_hdlr_get_blocked_plmn_list();

  if (list_ptr != NULL)
  {
    curr_blocked_plmn_ptr = 
      (ds_3gpp_pdn_cntxt_blocked_plmn_type*)list_peek_front( list_ptr );
    
    while ( curr_blocked_plmn_ptr != NULL )
    {
      next_blocked_plmn_ptr = 
        list_peek_next(list_ptr, &(curr_blocked_plmn_ptr->link));

      ds_3gpp_pdn_cntxt_enable_lte_plmn_blocking_on_plmn_id_ex
        (curr_blocked_plmn_ptr->plmn_id);

      curr_blocked_plmn_ptr = next_blocked_plmn_ptr;
    }
  }
} /* ds_3gpp_pdn_cntxt_enable_lte_unblock_all_plmns */


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
)
{
   ds_3gpp_pdn_cntxt_blocked_plmn_type   *curr_blocked_plmn_ptr;
   ds_3gpp_pdn_cntxt_blocked_plmn_type   *next_blocked_plmn_ptr;
   list_type                             *list_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - */

  /*--------------------------------------------------------------------------   
    Here we loop through all the blocked PLMNs and unblock them one by one.
  ----------------------------------------------------------------------------*/

  list_ptr = ds_pdn_hdlr_get_blocked_plmn_list();

  if (list_ptr != NULL)
  {
    curr_blocked_plmn_ptr = 
      (ds_3gpp_pdn_cntxt_blocked_plmn_type*)list_peek_front( list_ptr );
    
    while ( curr_blocked_plmn_ptr != NULL )
    {
      next_blocked_plmn_ptr = 
        list_peek_next(list_ptr, &(curr_blocked_plmn_ptr->link));

      list_pop_item(list_ptr, &(curr_blocked_plmn_ptr->link) );

      DS_SYSTEM_HEAP_MEM_FREE(curr_blocked_plmn_ptr);

      curr_blocked_plmn_ptr = next_blocked_plmn_ptr;
    }
  }
} /* ds_3gpp_pdn_cntxt_flush_blocked_plmn_list */

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
)
{
  boolean                        found_plmn = FALSE;
  sys_plmn_list_s_type          *eplmn_list_p;
  uint8                          eplmn_idx;
  uint16                         request_pref = 
                                 CM_PLMN_BLOCKING_PREF_PLMN_UNBLOCKING_ONLY;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  found_plmn = ds_3gpp_pdn_cntxt_is_plmn_blocked(unblock_plmn_id);
  if(found_plmn)
  {
    if(ds3g_msh_enable_lte_plmn_blocking_on_plmn_id_ex2(unblock_plmn_id,
                                                  request_pref))
    {
      found_plmn = ds_3gpp_free_plmn_if_blocked(&unblock_plmn_id);
    }
    else
    {
      found_plmn = FALSE;
    }
  }

  if (ds_pdn_nv_manager_is_rel_13_or_above_supported() && (found_plmn))
  {
    /* Unblock all ePLMNs under the parent PLMN ID if present */
    eplmn_list_p = ds_3gppi_throt_get_eplmn_list_per_plmn(
        unblock_plmn_id, DS_3GPP_THROT_RAT_LTE);

    if (eplmn_list_p)
    {
      for (eplmn_idx = 0; eplmn_idx < eplmn_list_p->length; eplmn_idx++)
      {
        /* Inform CM to unblock the ePLMN */
        if(ds3g_msh_enable_lte_plmn_blocking_on_plmn_id_ex2(
              eplmn_list_p->plmn[eplmn_idx], request_pref))
        {
          (void)ds_3gpp_free_plmn_if_blocked(&eplmn_list_p->plmn[eplmn_idx]);
        }
      }
    }

    eplmn_list_p = ds_3gpp_ntw_override_get_eplmn_list_per_plmn(
        unblock_plmn_id, DS_3GPP_THROT_RAT_LTE);

    /* If there are any ePLMNs in the ntw override handler ePLMN list, then
       unblock them */
    if (eplmn_list_p)
    {
      for (eplmn_idx = 0; eplmn_idx < eplmn_list_p->length; eplmn_idx++)
      {
        /* Inform CM to unblock the ePLMN */
        if(ds3g_msh_enable_lte_plmn_blocking_on_plmn_id_ex2(
              eplmn_list_p->plmn[eplmn_idx], request_pref))
        {
          (void)ds_3gpp_free_plmn_if_blocked(&eplmn_list_p->plmn[eplmn_idx]);
        }
      }
    }
  } /* found_plmn */
} /* ds_3gpp_pdn_cntxt_flush_plmn_from_list */

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
)
{

  byte                                    def_sock_prof = 0;
  ds_pdn_apm_req_pay_load                 apm_payload = {0};
  ds_pdn_apm_ext_error_code                   dsd_result = DS_PDN_APM_SUCCESS;
  ds_pdn_cntxt_attach_profile_status_type attach_profile_status = 
                                            DS_PDN_CNTXT_ATTACH_PROFILE_AVAIL;
  ds_profile_type_s                       prof_info = {{0}};
 /*----------------------------------------------------------------------------*/

  do
  {
    dsd_result = ds_pdn_apm_req( DS_PDN_APM_ATTACH_APN_AVAIL_REQ, 
                                     &apm_payload);
     
    if(dsd_result != DS_PDN_APM_SUCCESS)
    {
      DS_MSG0(MSG_LEGACY_ERROR,"APM should always return success for this request:api has gone bad "
                         "DS_DSD_APM_ATTACH_APN_AVAIL_REQ ");
      attach_profile_status = DS_PDN_CNTXT_ATTACH_PROFILE_UNKNOWN;
      break;
    }

    if (DS_PDN_APM_ATTACH_APN_AVAILABLE == apm_payload.attach_apn_avail)
    {
      DS_MSG0(MSG_LEGACY_HIGH,"Atleast one attach APN is available");
      attach_profile_status = DS_PDN_CNTXT_ATTACH_PROFILE_AVAIL;
    }
    else if (DS_PDN_APM_ATTACH_APN_BLOCKED == apm_payload.attach_apn_avail )
    {
      DS_MSG0(MSG_LEGACY_HIGH,"All Attach APNs are blocked");
      attach_profile_status = DS_PDN_CNTXT_ATTACH_PROFILE_UNAVAIL;
    }

    else if((DS_PDN_APM_EFS_CONFIG_ERROR == apm_payload.attach_apn_avail)|| 
             (DS_PDN_APM_PROF_CONFIG_ERROR == apm_payload.attach_apn_avail))
    {    
      def_sock_prof = ds_profile_get_profile_number_for_embeded_calls();
      if(ds_profile_setting_from_profile_id(def_sock_prof,&prof_info) == 
                                               DS_PROFILE_RESULT_SUCCESS)
      {
          if(
#ifdef FEATURE_DATA_CC_SP_THROTTLING
(ds_3gpp_throt_sm_is_cc_sp_throttling_enabled() == FALSE)&&
#endif /* FEATURE_DATA_CC_SP_THROTTLING */
                 (FALSE == (ds_3gpp_pdn_throt_is_apn_throttled(
                             (byte*)prof_info.context.apn, DS_PDN_INSTANCE_MAX,
                             NULL, FALSE))))
          {
            DS_MSG1(MSG_LEGACY_HIGH,"Default socket profile %d is not throttled",
                              def_sock_prof);        
            attach_profile_status = DS_PDN_CNTXT_ATTACH_PROFILE_AVAIL;
          }
          else
          {
            DS_MSG1(MSG_LEGACY_HIGH,"Default socket profile: %d has APN throttled, "
                              "No APN to attach", def_sock_prof);
            attach_profile_status = DS_PDN_CNTXT_ATTACH_PROFILE_UNAVAIL;
          }
      }
      else
      {
        DS_MSG0(MSG_LEGACY_ERROR,"Cannot find profile info from default profile number "
                           "Assume Attach profile is unavailable");
        attach_profile_status = DS_PDN_CNTXT_ATTACH_PROFILE_UNAVAIL;
      }
    } //attach_apn_avail
    else
    {
      DS_MSG0(MSG_LEGACY_HIGH,"APM returned known error. Do-not block/unblock PLMN");
      attach_profile_status = DS_PDN_CNTXT_ATTACH_PROFILE_UNKNOWN;
    }

  } while (0);
  return attach_profile_status;
} /* ds_3gpp_pdn_cntxt_check_if_attach_profile_available */

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
)
{
  boolean     detach_and_block_plmn = FALSE;
  
  if(FALSE == ds_bearer_mgr_is_active_bearer_cntxt())
  { 
    if (ds_3gpp_pdn_cntxt_check_if_attach_profile_available() == 
          DS_PDN_CNTXT_ATTACH_PROFILE_UNAVAIL)
    {
      detach_and_block_plmn = TRUE;
    }
  } //is_active_bearer_cntxt

  if (detach_and_block_plmn)
  {
    ds_3gpp_pdn_cntxt_detach_and_block_plmn(TRUE);
  }
} /* ds_3gpp_pdn_cntxt_detach_and_block_plmn_if_attach_profile_unavailable */

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
)
{
  boolean                               block_plmn = FALSE;
  
  if(FALSE == ds_bearer_mgr_is_active_bearer_cntxt())
  { 
    if (ds_3gpp_pdn_cntxt_check_if_attach_profile_available() == 
          DS_PDN_CNTXT_ATTACH_PROFILE_UNAVAIL)
    {
      block_plmn = TRUE;
    }
  } //is_active_bearer_cntxt

  if (block_plmn)
  {
    ds_3gpp_pdn_cntxt_block_plmn();
  }
} /* ds_3gpp_pdn_cntxt_disable_lte_if_attach_profile_unavailable */

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
)
{
  boolean unblock_plmn = FALSE;
 
  if (ds_3gpp_pdn_cntxt_check_if_attach_profile_available() 
        == DS_PDN_CNTXT_ATTACH_PROFILE_AVAIL)
  {
    unblock_plmn = TRUE;
  }

  if (unblock_plmn)
  {    
    ds_3gpp_pdn_cntxt_enable_lte_unblock_all_plmns();
  }
} /* ds_3gpp_pdn_cntxt_enable_lte_if_attach_profile_available */

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
)
{
  ds_3gpp_pdn_cntxt_blocked_plmn_type   *curr_blocked_plmn_ptr;
  ds_3gpp_pdn_cntxt_blocked_plmn_type   *next_blocked_plmn_ptr;
  list_type                             *list_ptr = NULL;
  ds_pdn_apm_lte_attach_pdn_list_type    attach_list = {0};
  /*---------------------------------------------------------------------------*/

  ds_pdn_apm_get_attach_pdn_list(&attach_list);
  /*--------------------------------------------------------------------------
   Here we loop through all the blocked PLMNs and unblock them one by one.
  ----------------------------------------------------------------------------*/
  list_ptr = ds_pdn_hdlr_get_blocked_plmn_list();
  if (list_ptr != NULL)
  {
    curr_blocked_plmn_ptr = 
       (ds_3gpp_pdn_cntxt_blocked_plmn_type*)list_peek_front( list_ptr );
    while (curr_blocked_plmn_ptr != NULL )
    {
      next_blocked_plmn_ptr = 
          list_peek_next(list_ptr, &(curr_blocked_plmn_ptr->link));
      if (ds_3gpp_pdn_cntxt_check_if_attach_profile_available_plmn_id(
                   curr_blocked_plmn_ptr->plmn_id,
                   attach_list)) 
      {
         ds_3gpp_pdn_cntxt_enable_lte_plmn_blocking_on_plmn_id_ex
             (curr_blocked_plmn_ptr->plmn_id);
      }

      curr_blocked_plmn_ptr = next_blocked_plmn_ptr;
    }
  }
} 
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
)
{
  ds_3gpp_pdn_throt_apn_list_type      *blocked_apn_list = NULL;
  uint8                                 num_blocked_apn = 0;
  byte                                  prof_apn_name[DS_PROFILE_APN_STRING_LEN+1];
  ds_3gpp_pdn_throt_apn_list_type       *temp_apn_list = NULL;
  boolean                               attach_profile_blocked = FALSE; 
  boolean                               plmn_unblock = FALSE;
  ds_profile_status_etype               result = DS_PROFILE_RESULT_FAIL;
  int                                   index = 0;
  int                                   profile_index=0;  
  int                                   blocked_index=0;
  ds_profile_type_s                     profile_info = {{0}};

  DS_SYSTEM_HEAP_MEM_ALLOC(blocked_apn_list,DS_MAX_APN_NUM*sizeof(ds_3gpp_pdn_throt_apn_list_type),
                           ds_3gpp_pdn_throt_apn_list_type*);
  
  if (blocked_apn_list != NULL) 
  {
    memset(blocked_apn_list, 0, sizeof(ds_3gpp_pdn_throt_apn_list_type));
    ds_3gpp_pdn_throt_get_blocked_apn_list_on_plmn_id(
                    blocked_apn_list,
                    &num_blocked_apn, 
                    plmn_id,
                    TRUE);
  }


  for(index=0;index < attach_list.attach_pdn_profile_list_num;index++) 
  {
    attach_profile_blocked = FALSE; 
    memset(prof_apn_name,0,DS_PROFILE_APN_STRING_LEN+1);
    profile_index= attach_list.attach_pdn_profile_list[index];
    
    result = ds_profile_setting_from_profile_id(profile_index,&profile_info);    
    if( result != DS_PROFILE_RESULT_SUCCESS )
    {
      DS_MSG2(MSG_LEGACY_ERROR,"Err %d in getting APN info. Prof: %d. Cont.",
                                     result,profile_index);
      plmn_unblock = TRUE;
      continue;
    }

    strlcpy((char*)prof_apn_name,profile_info.context.apn,
              DS_PROFILE_APN_STRING_LEN+1);
    
    temp_apn_list=blocked_apn_list;
    
    for (blocked_index=0;blocked_index < num_blocked_apn;blocked_index++) 
    {

      DS_MSG_SPRINTF_2(MSG_LEGACY_HIGH,"Attach List Apn name %s Blocked List Apn Name  %s"
                               ,prof_apn_name,temp_apn_list->apn);
      
      if ((strncasecmp((char*)(temp_apn_list->apn),
           (char*)prof_apn_name,
           DS_PROFILE_APN_STRING_LEN+1)==0) &&
          (temp_apn_list->attach_allowed == FALSE))
      {
        attach_profile_blocked = TRUE; 
      }
      temp_apn_list++;
    }
    if (attach_profile_blocked == FALSE) 
    {
      plmn_unblock = TRUE;
      break;
    }
     
  }
  if (attach_list.attach_pdn_profile_list_num == 0) 
  {
    plmn_unblock = TRUE;
  }

  DS_SYSTEM_HEAP_MEM_FREE(blocked_apn_list);
  return plmn_unblock;
}

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
)
{
  uint32                throttle_timer_val = 0;
  sys_plmn_id_s_type    plmn_id;
  /*-----------------------------------------------------------------------*/

  throttle_timer_val = 
    ds_3gpp_pdn_throt_lte_disable_throttle_timer_value();

  if (throttle_timer_val != 0)
  {
    ds_pdn_hdlr_get_current_plmn(&plmn_id);

    if(ds_3gpp_pdn_cntxt_disable_lte_per_plmn_selection_ex
         (plmn_id,throttle_timer_val,detach, SYS_BLOCK_PLMN_CAUSE_OTHERS))
    {
      DS_MSG0(MSG_LEGACY_HIGH,"Plmn Blocking as all the Lte Attach"
                        "Profiles are blocked on current PLMN");
    }
  }
} /* ds_3gpp_pdn_cntxt_detach_and_block_plmn */

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
)
{
  ds_3gpp_pdn_cntxt_detach_and_block_plmn( FALSE);
} /* ds_3gpp_pdn_cntxt_block_plmn */


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
)
{
  list_type                             *list_ptr = NULL;
  uint32                                 size = 0;
  /*-----------------------------------------------------------------------*/

  list_ptr = ds_pdn_hdlr_get_blocked_plmn_list();

  if (list_ptr != NULL)
  {
    size = list_size(list_ptr);
  }
  DS_MSG1(MSG_LEGACY_HIGH,"Number of blocked PLMNs = %d ", size);
  return size;
} /* ds_3gpp_pdn_cntxt_get_blocked_plmn_list_size */

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
)
{
  uint8                mcc[3] = {0};
  uint8                mnc[3] = {0};
  sys_plmn_id_s_type  *current_plmn_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(current_plmn == NULL)
  {
    DS_MSG0(MSG_LEGACY_LOW, "PLMN ID pointer is NULL, returning");
    return;
  }

  current_plmn_id = (sys_plmn_id_s_type *)current_plmn;

  mcc[0] = (current_plmn_id->identity[0] & 0x0F);
  mcc[1] = ((current_plmn_id->identity[0] & 0xF0) >> 4);
  mcc[2] = (current_plmn_id->identity[1] & 0x0F);

  mnc[0] = (current_plmn_id->identity[2] & 0x0F);
  mnc[1] = ((current_plmn_id->identity[2] & 0xF0) >> 4);
  mnc[2] = ((current_plmn_id->identity[1] & 0xF0) >> 4);

  DS_MSG6(MSG_LEGACY_HIGH,"PLMN ID: MCC: %d%d%d MNC: %d%d%d",
            mcc[0], mcc[1], mcc[2],
            mnc[0], mnc[1], mnc[2]);

  return;
} /* ds3g_print_plmn_id() */

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
  ds_3gpp_throt_rab_info_s            *rab_throt_info
)
{
  boolean throt_is_needed = FALSE;

  do
  {
    if(lte_call_info_ptr == NULL || rab_throt_info==NULL)
    {
      DS_MSG0(MSG_LEGACY_ERROR,"Input Params NULL. ");
      return throt_is_needed;
    }
    rab_throt_info->lower_layer_failure = FALSE;
    rab_throt_info->throttle_needed = FALSE;

    DS_MSG2(MSG_LEGACY_HIGH,"EMM Failure Cause Type: %d, "
                    "EMM Failure Detailed Reason: %d", 
                    lte_call_info_ptr->emm_failure_cause.cause_type,
                    lte_call_info_ptr->emm_failure_cause.cause.est_status);

    /*-------------------------------------------------------------------
       Check the EMM Cause type and EMM Detailed Cause if necessary
       DS to interpret all the below EMM failure cause types
       as Lower layer failures
       Cause types are 
       LTE_NAS_IRAT_CONN_EST_FAILURE --> and cause code is not
                                    LTE_RRC_CONN_EST_FAILURE_CONN_REJECT
       LTE_NAS_IRAT_UL_MSG_CNF_FAILURE
       LTE_NAS_IRAT_LOWER_LAYER_FAILURE 
    ---------------------------------------------------------------------*/

    switch (lte_call_info_ptr->emm_failure_cause.cause_type)
    {
      case LTE_NAS_IRAT_CONN_EST_FAILURE:
      {
        rab_throt_info->lower_layer_failure = TRUE;
        switch (lte_call_info_ptr->emm_failure_cause.cause.est_status)
        {
          case LTE_RRC_CONN_EST_FAILURE_RF_UNAVAILABLE:
          {
            throt_is_needed = FALSE;
            break;
          }

          case LTE_RRC_CONN_EST_FAILURE_CONN_REJECT:
          {
            rab_throt_info->lower_layer_failure = FALSE;
            if((!ds_pdn_nv_manager_get_regular_pdn_throttling_nv()) || 
               (ds_3gpp_srv_req_throt_t3402_is_enabled()))
            {
              throt_is_needed = FALSE;
            }
            else
            {
              throt_is_needed = TRUE;
            }
            break;
          } 

          case LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED:
          {
            if (ds_3gpp_srv_req_throt_t3402_is_enabled() ||
                lte_call_info_ptr->emm_failure_cause.access_barring_rem_time == 
                    ACCESS_BARRING_INFINITE_RETRY_TIME)
            { 
              throt_is_needed = FALSE;
            }
            else
            {
              /* Keep the legacy(default) behavior */
              throt_is_needed = TRUE;
            }
            break;
          }

          case LTE_RRC_CONN_EST_FAILURE_TIMER_EXPIRED:
          {
            if (ds_3gpp_srv_req_throt_t3402_is_enabled()) 
            {
              throt_is_needed = FALSE; 
            }
            else  
            {
              throt_is_needed = TRUE;
            }
            break;
          }

          case LTE_RRC_CONN_EST_FAILURE_TAI_CHANGE:
          {
            throt_is_needed = FALSE;
            break;
          }

          default:
          {
            throt_is_needed = TRUE;
            break;
          }
        } /* switch case for detailed causes corresponding to LTE_NAS_IRAT_CONN_EST_FAILURE*/
        break;
      } /* Cause Type: LTE_NAS_IRAT_CONN_EST_FAILURE */

      case LTE_NAS_IRAT_CONN_REL:
        throt_is_needed = TRUE;
        break;

      case LTE_NAS_IRAT_LOWER_LAYER_FAILURE:
        rab_throt_info->lower_layer_failure = TRUE;
        throt_is_needed = TRUE;
        break;

      /*----------------------------------------------------------------------- 
        For REL12+, no throttling is needed when T3417 timer expiry is received
        NAS would perform the desired 1min throttling
       -----------------------------------------------------------------------*/
      case LTE_NAS_IRAT_T3417_EXPIRY:
         if(ds_pdn_nv_manager_is_rel_12_or_above_supported())
         {
           rab_throt_info->lower_layer_failure = FALSE;
           throt_is_needed = FALSE;
         }
         else
         {
           rab_throt_info->lower_layer_failure = TRUE;
           throt_is_needed = TRUE;
         }
        break;

      case LTE_NAS_IRAT_UL_MSG_CNF_FAILURE:
        rab_throt_info->lower_layer_failure = TRUE;
        throt_is_needed = FALSE;
        break;

      case LTE_NAS_IRAT_SR_REJECT:
      {
        switch(lte_call_info_ptr->emm_failure_cause.cause.emm_cause)
        {
          case LTE_NAS_ILLEGAL_UE:
          case LTE_NAS_ILLEGAL_ME:
          case LTE_NAS_EPS_SERVICES_NOT_ALLOWED:
          case LTE_NAS_UE_ID_CANNOT_BE_DERIVED_BY_NW:
          case LTE_NAS_IMPLICITLY_DETACHED:
          case LTE_NAS_PLMN_NOT_ALLOWED:
          case LTE_NAS_TRACKING_AREA_NOT_ALLOWED:
          case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
          case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
          case LTE_NAS_CS_DOMAIN_NOT_AVAILABLE:
          case LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG:
          case LTE_NAS_CS_DOMAIN_TEMP_NOT_AVAILABLE:
            throt_is_needed = FALSE;
            break;
          default:
            throt_is_needed = TRUE;
            break;
        } /* switch case for detailed causes corresponding to LTE_NAS_IRAT_SR_REJECT  */
       
        break;
      } /* Cause Type: LTE_NAS_IRAT_SR_REJECT */
      default:
      {
        throt_is_needed = FALSE;
        break;
      }
    } /* switch (lte_call_info_ptr->emm_failure_cause.cause_type) */
  } while(0); 

  DS_MSG1(MSG_LEGACY_HIGH,"EMM Cause Throt needed is %d.", throt_is_needed);

  rab_throt_info->throttle_needed = throt_is_needed;
  return throt_is_needed;
}

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
  uint16             request_pref
)
{
  sys_plmn_id_s_type                 zero_plmn_id;
  cm_plmn_blocking_info_s_type       plmn_blocking_info;
  cm_ph_cmd_cb_f_type                *sys_sel_cb_f_ptr = NULL;
  ds_3gpp_plmn_sys_sel_retry_e_type  retry_type = 
                                             DS_3GPP_LTE_PLMN_UNBLOCK_RETRY;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - */
  
  memset(&plmn_blocking_info,0,sizeof(cm_plmn_blocking_info_s_type));
  memset((void*)&zero_plmn_id, 0, sizeof(sys_plmn_id_s_type));
  if(memcmp((void*)&(plmn_id),
            (void*)&(zero_plmn_id), 
            sizeof(sys_plmn_id_s_type)) == 0)
  {
    DS_MSG3(MSG_LEGACY_HIGH,"PLMN being inspected is zero",0,0,0);
    return FALSE;
  }
  DS_MSG1(MSG_LEGACY_HIGH,"ds3g_msh_enable_lte_plmn_blocking_on_plmn_id_ex:preference :%d", request_pref);
  
  /*----------------------------------------------------------------------
   Populate the contents of the PLMN blocking structure
  ----------------------------------------------------------------------*/
  plmn_blocking_info.request_pref =(cm_plmn_blocking_pref_e_type)request_pref;
     
  memscpy((void*)&(plmn_blocking_info.plmn),
          sizeof(sys_plmn_id_s_type),
          (void*)&(plmn_id),
          sizeof(sys_plmn_id_s_type));
  
    /*---------------------------------------------------------------------- 
    Populate the sys_sel_cb_f_ptr for RETRY
    Also at this point the retry sm must always be cleared because DS
    has no other notifications from CM/NAS when the PLMN is unblocked.
    ---------------------------------------------------------------------*/
  sys_sel_cb_f_ptr = (cm_ph_cmd_cb_f_type *)ds3g_get_sys_sel_cb_fn_ptr();
  
  if (sys_sel_cb_f_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_HIGH,"sys_sel_cb_f_ptr is NULL");
  }
  
  ds_3gpp_plmn_sys_sel_clr_retry_sm();
  
  ds_3gpp_plmn_sys_sel_set_retry_type(retry_type);
  
  ds_3gpp_plmn_sys_sel_set_blk_info(&plmn_blocking_info);
  
  if(cm_ph_cmd_detach_and_plmn_blocking(sys_sel_cb_f_ptr, 
                                        NULL, 
                                        dssysmgr_get_cm_client_id(),
                                        plmn_blocking_info) != TRUE)
  {
    DS_MSG3(MSG_LEGACY_ERROR,"Error while invoking CM API to unblock PLMN", 
                        0,0,0);
    return FALSE;
  }
  
  return TRUE;

}

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
)
{
  sys_plmn_id_s_type                     zero_plmn_id;
  cm_plmn_blocking_info_s_type           plmn_blocking_info;
  ds_3gpp_plmn_sys_sel_retry_e_type      retry_type = DS_3GPP_LTE_PLMN_BLOCK_RETRY;
  boolean                                ret_val = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  memset((void*)&zero_plmn_id, 0, sizeof(sys_plmn_id_s_type));
  
  if(memcmp((void*)&(plmn_id),
            (void*)&(zero_plmn_id), 
            sizeof(sys_plmn_id_s_type)) == 0)
  {
    return ret_val;
  }
  
  memset((void*)&plmn_blocking_info, 0,
                                    sizeof(cm_plmn_blocking_info_s_type));
  
  /*----------------------------------------------------------------------
    Populate the contents of the PLMN blocking structure
  ----------------------------------------------------------------------*/
  if (detach)
  {
    plmn_blocking_info.request_pref = CM_PLMN_BLOCKING_PREF_LTE_DETACH_PLMN_BLOCKING;
  }
  else
  {
    plmn_blocking_info.request_pref = CM_PLMN_BLOCKING_PREF_PLMN_BLOCKING_ONLY;
  }
  
  plmn_blocking_info.cause = cause;
  plmn_blocking_info.blocking_interval = timer_val;
    
  memscpy((void*)&(plmn_blocking_info.plmn),sizeof(sys_plmn_id_s_type),
          (void*)&(plmn_id),sizeof(sys_plmn_id_s_type));
  
  
  ds_3gpp_plmn_sys_sel_clr_retry_sm();
  
  ds_3gpp_plmn_sys_sel_set_retry_type(retry_type);
  
  ds_3gpp_plmn_sys_sel_set_blk_info(&plmn_blocking_info);
  
  #ifdef FEATURE_DATA_LTE
  ret_val = ds3g_msh_disable_lte_per_plmn_ex(plmn_blocking_info);
  #endif /* FEATURE_DATA_LTE */
  
  return ret_val;

}

/*===========================================================================

FUNCTION DS3G_MSH_DISABLE_LTE_SELECTION_EX

DESCRIPTION
  This function disables the selection of LTE network. This function in turn
  invokes the CM API that carries out the desired function

PARAMETERS
  Subscription ID

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
)
{
    cm_rat_acq_order_pref_s_type  rat_acq_order_pref;
    boolean            ret_val = FALSE;
    cm_ph_cmd_cb_f_type  *sys_sel_cb_f_ptr = NULL;
    ds_3gpp_plmn_sys_sel_retry_e_type  retry_type = DS_3GPP_LTE_DISABLE_RETRY;
    cm_cmd_user_pref_update_type user_pref;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - -*/
  
    user_pref.client_id = dssysmgr_get_cm_client_id();
  
    memset(&user_pref, 0, sizeof(cm_cmd_user_pref_update_type));
  
    user_pref.mode_pref = CM_MODE_PREF_DISABLE_LTE;
    user_pref.pref_term = CM_PREF_TERM_PWR_CYCLE;
  
    rat_acq_order_pref.type = CM_ACQ_ORDER_TYPE_GW;
    rat_acq_order_pref.acq_order.gw_acq_order = CM_GW_ACQ_ORDER_PREF_NO_CHANGE;
  
    user_pref.band_pref = CM_BAND_PREF_NO_CHANGE;
#if (LTE_BAND_NUM)
    user_pref.lte_band_pref = SYS_LTE_BAND_MASK_CONST_NO_CHG;
#else
    user_pref.lte_band_pref = CM_BAND_PREF_NO_CHANGE;
#endif
    user_pref.tds_band_pref = CM_BAND_PREF_NO_CHANGE;
    user_pref.prl_pref = CM_PRL_PREF_NO_CHANGE;
    user_pref.roam_pref = CM_ROAM_PREF_NO_CHANGE;
    user_pref.hybr_pref = CM_HYBR_PREF_NO_CHANGE;
    user_pref.srv_domain_pref = CM_SRV_DOMAIN_PREF_NO_CHANGE;
    user_pref.network_sel_mode_pref = CM_NETWORK_SEL_MODE_PREF_NO_CHANGE;
    user_pref.ue_usage_setting = SYS_UE_USAGE_SETTING_NO_CHANGE;
    user_pref.voice_domain_pref = SYS_VOICE_DOMAIN_PREF_NO_CHANGE;
    user_pref.lte_disable_cause = CM_LTE_DISABLE_CAUSE_PERMANENT_DS;
    user_pref.csg_id = SYS_CSG_ID_INVALID;
    user_pref.manual_rat = SYS_RAT_NONE;
  
   /*---------------------------------------------------------------------- 
    Populate the sys_sel_cb_f_ptr for RETRY
    Also at this point the retry sm must always be cleared because DS
    has no other notifications from CM/NAS when the PLMN is unblocked.
    ---------------------------------------------------------------------*/
    sys_sel_cb_f_ptr = (cm_ph_cmd_cb_f_type *)ds3g_get_sys_sel_cb_fn_ptr();
  
    if (sys_sel_cb_f_ptr == NULL)
    {
      DS_MSG0(MSG_LEGACY_HIGH,"sys_sel_cb_f_ptr is NULL");
    }
  
    ds_3gpp_plmn_sys_sel_clr_retry_sm();
  
    ds_3gpp_plmn_sys_sel_set_retry_type(retry_type);

    ds_3gpp_plmn_sys_set_user_pref_params(&user_pref);
      
    /*------------------------------------------------------------------------
      Call the CM API to disable the selection of LTE. This is done 
      by changing the service domain pref to CM_MODE_PREF_DISABLE_LTE
   ------------------------------------------------------------------------*/
    ret_val = cm_user_pref_update_req(&user_pref,
                   sys_sel_cb_f_ptr,
                   NULL);
  
    DS_MSG1(MSG_LEGACY_HIGH,
                    "CM API to disable LTE band returned: %d for sub id %d ",
                    ret_val);
  
    if (ret_val)
    {
      ds3gpp_set_ds_lte_flag(FALSE);
    }
    return;
}

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
)
{
  cm_client_id_type  ds3g_cm_client_id;
  boolean            ret_val = FALSE;
  cm_cmd_user_pref_update_type user_pref;
  cm_ph_cmd_cb_f_type               *sys_sel_cb_f_ptr = NULL;
  ds_3gpp_plmn_sys_sel_retry_e_type   retry_type = DS_3GPP_LTE_ENABLE_RETRY;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - -*/
  
  ds3g_cm_client_id = dssysmgr_get_cm_client_id();
  
  
  memset(&user_pref, 0, sizeof(cm_cmd_user_pref_update_type));
  
  user_pref.mode_pref = CM_MODE_PREF_ENABLE_LTE;
  user_pref.pref_term = CM_PREF_TERM_PWR_CYCLE;  
  user_pref.band_pref = CM_BAND_PREF_NO_CHANGE;
  #if (LTE_BAND_NUM)
  user_pref.lte_band_pref = SYS_LTE_BAND_MASK_CONST_NO_CHG;
  #else
  user_pref.lte_band_pref = CM_BAND_PREF_NO_CHANGE;
  #endif
  user_pref.tds_band_pref = CM_BAND_PREF_NO_CHANGE;
  user_pref.prl_pref = CM_PRL_PREF_NO_CHANGE;
  user_pref.roam_pref = CM_ROAM_PREF_NO_CHANGE;
  user_pref.hybr_pref = CM_HYBR_PREF_NO_CHANGE;
  user_pref.srv_domain_pref = CM_SRV_DOMAIN_PREF_NO_CHANGE;
  user_pref.network_sel_mode_pref = CM_NETWORK_SEL_MODE_PREF_NO_CHANGE;
  user_pref.ue_usage_setting = SYS_UE_USAGE_SETTING_NO_CHANGE;
  user_pref.voice_domain_pref = SYS_VOICE_DOMAIN_PREF_NO_CHANGE;
  user_pref.lte_disable_cause = CM_LTE_DISABLE_CAUSE_NO_CHANGE;
  user_pref.csg_id = SYS_CSG_ID_INVALID;
  user_pref.manual_rat = SYS_RAT_NONE;
  
  /*---------------------------------------------------------------------- 
    Populate the sys_sel_cb_f_ptr for RETRY
    Also at this point the retry sm must always be cleared because DS
    has no other notifications from CM/NAS when the PLMN is unblocked.
  ---------------------------------------------------------------------*/
  sys_sel_cb_f_ptr = (cm_ph_cmd_cb_f_type *)ds3g_get_sys_sel_cb_fn_ptr();
  
  if (sys_sel_cb_f_ptr == NULL)
  {
    DS_ERROR_LOG_0("sys_sel_cb_f_ptr is NULL");
  }
  
  ds_3gpp_plmn_sys_sel_clr_retry_sm();
  
  ds_3gpp_plmn_sys_sel_set_retry_type(retry_type);

  ds_3gpp_plmn_sys_set_user_pref_params(&user_pref);
  /*------------------------------------------------------------------------
    Call the CM API to enable the selection of LTE. This is done by
    changing the service domain pref to CM_MODE_PREF_ENABLE_LTE
  ------------------------------------------------------------------------*/
  if(ds3gpp_is_lte_flag_disabled())
  {
    ret_val = cm_user_pref_update_req(&user_pref,
                 sys_sel_cb_f_ptr,
                 NULL);
  
    DS_MSG1(MSG_LEGACY_HIGH,
                    "CM API to enable LTE selection returned:%d ",
                    ret_val);
  
    if (ret_val)
    {
      ds3gpp_set_ds_lte_flag(TRUE);
    }
  
  }
  else
  {
    DS_MSG1(MSG_LEGACY_HIGH,
                    "CM API Ignore to enable LTE %d for subs_id",
                    ret_val);
  }
  return;
}

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
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG0(MSG_LEGACY_HIGH,"sys_sel_clr_retry_sm");

  ps_timer_cancel(ds_3gpp_plmn_sys_sel_retry_sm.sys_sel_retry_timer);

  /*------------------------------------------------------------------------ 
    Do not memset ds_3gpp_plmn_sys_sel_retry_sm because retry timer will
    not be initialized
  ------------------------------------------------------------------------*/
  memset((void *)&ds_3gpp_plmn_sys_sel_retry_sm.plmn_blocking_info,0,
         sizeof(cm_plmn_blocking_info_s_type)); 

  ds_3gpp_plmn_sys_sel_retry_sm.retry_timer_val = 0;

  ds_3gpp_plmn_sys_sel_retry_sm.retry_type = DS_3GPP_NO_SYS_SEL_RETRY;
}

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
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ds_3gpp_plmn_sys_sel_retry_sm.retry_type = retry_type;
  return;

}

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
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (plmn_blocking_info_p == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"sys_sel_params_p is NULL");
    return;
  }

  memscpy((void*)&(ds_3gpp_plmn_sys_sel_retry_sm.plmn_blocking_info),
          sizeof(cm_plmn_blocking_info_s_type),
          (void*)(plmn_blocking_info_p), 
          sizeof(cm_plmn_blocking_info_s_type));

}

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
)
{

  boolean ret_val = FALSE;
  cm_ph_cmd_cb_f_type *sys_sel_cb_f_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*---------------------------------------------------------------------- 
    Populate the sys_sel_cb_f_ptr for RETRY
    Also at this point the retry sm must always be cleared because DS
    has no other notifications from CM/NAS when the PLMN is unblocked.
  ---------------------------------------------------------------------*/
  sys_sel_cb_f_ptr = (cm_ph_cmd_cb_f_type *)ds3g_get_sys_sel_cb_fn_ptr();

  if (sys_sel_cb_f_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"sys_sel_cb_f_ptr is NULL");
  }

  DS_MSG3(MSG_LEGACY_ERROR,"Blocking Plmn for timer:%d subs_id:%d req_pref:%d",
                      plmn_blocking_info.blocking_interval,
                      plmn_blocking_info.asubs_id,
                      plmn_blocking_info.request_pref);

    if(cm_ph_cmd_detach_and_plmn_blocking(sys_sel_cb_f_ptr,NULL,
                                          dssysmgr_get_cm_client_id(),
                                          plmn_blocking_info) != TRUE)
    {
      DS_MSG0(MSG_LEGACY_ERROR,"Error while invoking CM API to block PLMN");
      return ret_val;
    }

  ret_val = TRUE;
  return ret_val;
}

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
)
{
  uint16 retry_timer_val = 0;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  retry_timer_val = ds_3gpp_plmn_sys_sel_retry_sm.retry_timer_val;

  /*---------------------------------------------------------------------
    Retry mechanism is
    1,2,4,8,16,16,16 until retry timer is reset
  -----------------------------------------------------------------------*/
  if (retry_timer_val < DS_3GPP_SYS_SEL_RETRY_MAX_VAL)
  {
    if (retry_timer_val == 0 )
    {
      retry_timer_val = 1;
    }
    else
    {
      retry_timer_val = (retry_timer_val << 1); /* 2 ^ retry_timer_val */
    }
  }

  ds_3gpp_plmn_sys_sel_retry_sm.retry_timer_val = retry_timer_val;

  DS_MSG1(MSG_LEGACY_HIGH,"Retry Timer value in secs:%d",retry_timer_val);

  retry_timer_val = retry_timer_val * DS_3GPP_THROT_SECOND_TO_MILISECOND_MULTIPLIER;

  /*------------------------------------------------------------------------ 
    Start the Retry timer 
  -------------------------------------------------------------------------*/
  ps_timer_start(ds_3gpp_plmn_sys_sel_retry_sm.sys_sel_retry_timer, 
                        retry_timer_val);

  return;
}

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
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ds_3gpp_plmn_sys_sel_retry_sm.retry_type == DS_3GPP_LTE_DISABLE_RETRY)
  {
    DS_MSG0(MSG_LEGACY_HIGH,"LTE disabled");
    ds3gpp_set_ds_lte_flag(FALSE); 
  }
  else if(ds_3gpp_plmn_sys_sel_retry_sm.retry_type ==
            DS_3GPP_LTE_ENABLE_RETRY)
  {
    DS_MSG0(MSG_LEGACY_HIGH,"LTE enabled");
    ds3gpp_set_ds_lte_flag(TRUE); 
  }

  if (ds_3gpp_plmn_sys_sel_retry_sm.retry_timer_val > 0)
  {
    DS_MSG0(MSG_LEGACY_HIGH,"Clearing sys sel retry timer");
    ds_3gpp_plmn_sys_sel_clr_retry_sm();
  }

  return;
}

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
)
{
  cm_ph_cmd_cb_f_type *sys_sel_cb_f_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*---------------------------------------------------------------------- 
    Populate the sys_sel_cb_f_ptr for RETRY
    Also at this point the retry sm must always be cleared because DS
    has no other notifications from CM/NAS when the PLMN is unblocked.
  ---------------------------------------------------------------------*/
  sys_sel_cb_f_ptr = (cm_ph_cmd_cb_f_type *)ds3g_get_sys_sel_cb_fn_ptr();

  if (sys_sel_cb_f_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_HIGH,"sys_sel_cb_f_ptr is NULL");
  }


  DS_MSG1(MSG_LEGACY_HIGH,"ds_3gpp_plmn_sys_sel_retry_timer_expiry_cmd_hdlr:"
                    "Retry:%d",ds_3gpp_plmn_sys_sel_retry_sm.retry_type);

  switch (ds_3gpp_plmn_sys_sel_retry_sm.retry_type)
  {

    case DS_3GPP_LTE_PLMN_BLOCK_RETRY:
      /*----------------------------------------------------------------- 
         Reduce the timer interval with the retry time that has elapsed
       ------------------------------------------------------------------*/
      if(ds_3gpp_throt_update_plmn_blk_interval_and_retry())
      {
    #ifdef FEATURE_DATA_LTE
        if(ds3g_msh_disable_lte_per_plmn_ex(ds_3gpp_plmn_sys_sel_retry_sm.
                                            plmn_blocking_info) != TRUE)
        {
           DS_MSG0(MSG_LEGACY_ERROR,"Error while invoking CM API to block PLMN");
        }
   #endif /* FEATURE_DATA_LTE */
      }
      else
      {
        DS_MSG0(MSG_LEGACY_HIGH,"No PLMN BLOCKING RETRY");
      } 
      break;

    case DS_3GPP_LTE_DISABLE_RETRY:
      if(cm_user_pref_update_req(&ds_3gpp_plmn_sys_sel_retry_sm.user_pref_params,
                 sys_sel_cb_f_ptr,
                 NULL)!= TRUE)
      {
        DS_MSG0(MSG_LEGACY_ERROR,"Error while invoking CM API to disable LTE");
      }
      break;

    case DS_3GPP_LTE_ENABLE_RETRY:
      if(cm_user_pref_update_req(&ds_3gpp_plmn_sys_sel_retry_sm.user_pref_params,
                 sys_sel_cb_f_ptr,
                 NULL)!= TRUE)
      {
        DS_MSG0(MSG_LEGACY_ERROR,"Error while invoking CM API to disable LTE");
      }
      break;

    case DS_3GPP_LTE_PLMN_UNBLOCK_RETRY:
 #ifdef FEATURE_DATA_LTE
     if(ds3g_msh_disable_lte_per_plmn_ex(ds_3gpp_plmn_sys_sel_retry_sm.
                                            plmn_blocking_info) != TRUE)
     {
       DS_MSG0(MSG_LEGACY_ERROR,"Error while invoking CM API to block PLMN");
     }
 #endif /* FEATURE_DATA_LTE */
     break;

    default:
      DS_MSG0(MSG_LEGACY_ERROR,"Incorrect sys_sel retry type");
      break;
  }

  return;
}


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
  void* cb_data
)
{
  ds_command_type *cmd_buf = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG0(MSG_LEGACY_HIGH,"In ds_3gpp_plmn_sys_sel_retry_timer_expiry()");

  if((cmd_buf = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO)) == NULL)
  {
    return;
  }
  else
  {
    cmd_buf->hdr.cmd_id = DS_CMD_3GPP_SYS_SEL_RETRY_TIMER_EXP;
    ds_put_cmd(cmd_buf);
  }

  return;
}


/*===========================================================================
FUNCTION  DS_3GPP_PLMN_SYS_SEL_SET_SYS_SEL_PREF_PARAMS

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
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (user_pref_params_p == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"sys_sel_params_p is NULL");
    return;
  }

  memscpy((void*)&(ds_3gpp_plmn_sys_sel_retry_sm.user_pref_params),
          sizeof(cm_cmd_user_pref_update_type),
          (void*)(user_pref_params_p), 
          sizeof(cm_cmd_user_pref_update_type));
}
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
)
{
  do
  {
    if (throt_sm_ptr == NULL)
    {
      break;
    }

    if(throt_sm_ptr->failure_count < DS_3GPP_PDN_THROTTLE_MAX_FAILURE_TIMERS)
    {
      throt_sm_ptr->failure_count++;
    }

  }while(0); 
}
