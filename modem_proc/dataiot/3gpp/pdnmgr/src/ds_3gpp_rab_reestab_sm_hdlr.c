 /*!
  @file
  ds_3gpp_rab_reestab_sm_hdlr.c

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2009-2017 QUALCOMM Technologies Incorporated. All Rights Reserved

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

#include "ds_3gpp_rab_reestab_sm_hdlr.h"
#include "dsmsgr.h"
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/src/ds_3gpp_rab_reestab_sm_hdlr.c#2 $

when         who     what, where, why
--------     ---     --------------------------------------------------------
3/20/17    vb        RAB restablishment handling

===========================================================================*/

/*------------------------------------------------------------------------- 
  STATE TRANSITION EXPLANATION
 
  DS_3GPP_RAB_REQ_NOT_PENDING_STATE
  *************************************************
  This state is set when
  1. UE is powered up
  2. When RAB reestablish indication is received
  3. When random timer(0.1-0.4 sec timer) expires
  4. When throttle timer expires
  5. When RRC deprioritization guard timer expires
  *************************************************
 
  DS_3GPP_RAB_REQ_PENDING_STATE
  *************************************************
  This state is set when
  1. RAB REESTABLISH REQ is sent out to the network
  *************************************************
 
  DS_3GPP_RAB_REJ_RAND_TMR_STATE
  *************************************************
  This state is set when
  1. RAB REESTABLISH REJ is received and
  0.1 - 0.4 s random timer is started
  *************************************************
 
  DS_3GPP_RAB_REJ_RRC_BARRING_STATE
  *************************************************
  This state is set when
  1. RRC Barring staer ind from RRC is received
  2. RAB REESTAB REJ is received with
  deprior started set to TRUE
  *************************************************
 
  DS_3GPP_RAB_REJ_THROTTLED_STATE
  *************************************************
  1. When RAB REESTAB REJ is received and throttle
  timer starts
  *************************************************
  -------------------------------------------------------------------------*/

static ds_3gpp_rab_state_e 
       ds_3gpp_rab_reestab_state = DS_3GPP_RAB_REQ_NOT_PENDING_STATE;



/*===========================================================================
FUNCTION DS_3GPP_RAB_REESTAB_GET_STATE

DESCRIPTION
   Get the current rab reestablish state
 
PARAMETERS
    
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
ds_3gpp_rab_state_e ds_3gpp_rab_reestab_get_state()
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  return ds_3gpp_rab_reestab_state;

}/*ds_3gpp_rab_reestab_get_state*/


/*===========================================================================
FUNCTION DS_3GPP_RAB_REESTAB_SET_STATE

DESCRIPTION
   Set the current rab reestablish state
 
PARAMETERS
    
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static void ds_3gpp_rab_reestab_set_state
(
   ds_3gpp_rab_state_e  rab_state,
   void         *user_data
)
{
  uint32       timer_val = (uint32)user_data;
  boolean      timer_running = ds_bearer_mgr_get_rab_reest_timer();
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 
 DS_MSG2(MSG_LEGACY_HIGH," ds_3gpp_rab_reestab_set_state ds_3gpp_rab_reestab_state %d, rab_state %d",
 ds_3gpp_rab_reestab_state, rab_state);
 
  do
  {
    ds_3gpp_rab_reestab_state = rab_state;

    switch(rab_state)
    {
      case DS_3GPP_RAB_REJ_RAND_TMR_STATE:
      case DS_3GPP_RAB_REJ_RRC_BARRING_STATE:
      case DS_3GPP_RAB_REJ_THROTTLED_STATE:
      /*--------------------------------------------------------------
        If Timer is running, clear the timer before starting it again.
        -------------------------------------------------------------*/
        if(timer_running)
        {
          ds_bearer_mgr_cancel_rab_reest_timer();
        }
        DS_MSG1(MSG_LEGACY_HIGH, "Starting back off timer for %d msec",timer_val);
        ds_bearer_mgr_start_rab_reest_timer(timer_val);
        break;

      case DS_3GPP_RAB_REQ_NOT_PENDING_STATE:
        /*-----------------------------------------------------------
        If Timer is running 
        1. clear the timer. 
          This is needed when RRC barring stop indication is received 
          The 5s guard timer will have to be stopped. 
        2. Also send out another RAB request
        -------------------------------------------------------------*/
        if(timer_running)
        {
          ds_bearer_mgr_cancel_rab_reest_timer();
          ds_bearer_mgr_proc_reorig_dorm_tmr_exp_cmd();
        }

      default:
        break;
    }
  }while(0); 

  return;
}/*ds_3gpp_rab_reestab_set_state*/

/*===========================================================================
FUNCTION DS_3GPP_RAB_REESTAB_RESET_STATE

DESCRIPTION
   resets RAB state, This function is to be called when
   Srv req throttling is reset
 
PARAMETERS 
    
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_rab_reestab_reset_state()
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_3gpp_rab_reestab_state = DS_3GPP_RAB_REQ_NOT_PENDING_STATE;

  return;

}/*ds_3gpp_rab_reestab_reset_state*/




/*===========================================================================
FUNCTION DS_3GPP_RAB_REESTAB_TRANSITION_STATE

DESCRIPTION
   Perform rab reestab state transition 
 
PARAMETERS 
   Rab procedure that is undergoing
   user data - Timer value
   subs_id
    
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_rab_reestab_transition_state_ev
(
  ds_3gpp_rab_proc_e     rab_proc,
  void                  *user_data
)
{
  ds_local_err_val_e_type    err_val = DS_LOCAL_ERR_VAL_NONE;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 DS_MSG2(MSG_LEGACY_HIGH,"ds_3gpp_rab_reestab_transition_state_ev ds_3gpp_rab_reestab_state %d, rab_proc %d",
 ds_3gpp_rab_reestab_state, rab_proc);
  do
  {
    switch(ds_3gpp_rab_reestab_state) 
    {
      case DS_3GPP_RAB_REQ_NOT_PENDING_STATE:
        switch(rab_proc)
        {
           case DS_3GPP_PROC_RAB_REESTAB_REQ:
            ds_3gpp_rab_reestab_set_state(DS_3GPP_RAB_REQ_PENDING_STATE,
                                          user_data);
            break;

           default:
             err_val = DS_LOCAL_ERR_VAL_0;
             break;
        }
        break;

      case DS_3GPP_RAB_REQ_PENDING_STATE:
        switch(rab_proc)
        {
           case DS_3GPP_PROC_RAB_REESTAB_IND:
             ds_3gpp_rab_reestab_set_state(DS_3GPP_RAB_REQ_NOT_PENDING_STATE,
                                           user_data);

           case DS_3GPP_PROC_RAB_REESTAB_RAND_RETRY:
             ds_3gpp_rab_reestab_set_state(DS_3GPP_RAB_REJ_RAND_TMR_STATE,
                                           user_data);
             break;

           case DS_3GPP_PROC_RAB_REESTAB_REJ_DEPRIOR:
           case DS_3GPP_PROC_RAB_REESTAB_RRC_BARRING_START_IND:
             ds_3gpp_rab_reestab_set_state(DS_3GPP_RAB_REJ_RRC_BARRING_STATE,
                                           user_data);
             break;

           case DS_3GPP_PROC_RAB_REESTAB_THROTTLED:
                 ds_3gpp_rab_reestab_set_state(
                                           DS_3GPP_RAB_REJ_THROTTLED_STATE,
                                           user_data);
           default:
             err_val = DS_LOCAL_ERR_VAL_1;
             break;
        }
        break;

      case DS_3GPP_RAB_REJ_RAND_TMR_STATE:
        switch(rab_proc)
        {
           case DS_3GPP_PROC_RAB_REESTAB_RAND_TIMER_EXP:
             ds_3gpp_rab_reestab_set_state(DS_3GPP_RAB_REQ_NOT_PENDING_STATE,
                                           user_data);
             break;

           case DS_3GPP_PROC_RAB_REESTAB_RRC_BARRING_START_IND:
             ds_3gpp_rab_reestab_set_state(DS_3GPP_RAB_REJ_RRC_BARRING_STATE,
                                           user_data);
             break;

           default:
             err_val = DS_LOCAL_ERR_VAL_2;
             break;
        }
        break;

      case DS_3GPP_RAB_REJ_RRC_BARRING_STATE:
        switch(rab_proc)
        {
           case DS_3GPP_PROC_RAB_REESTAB_RAND_TIMER_EXP:
           case DS_3GPP_PROC_RAB_REESTAB_RRC_BARRING_STOP_IND:
             ds_3gpp_rab_reestab_set_state(DS_3GPP_RAB_REQ_NOT_PENDING_STATE,
                                           user_data);
             break;

           case DS_3GPP_PROC_RAB_REESTAB_THROTTLED:
                 ds_3gpp_rab_reestab_set_state(
                                           DS_3GPP_RAB_REJ_THROTTLED_STATE,
                                           user_data);
           default:
             err_val = DS_LOCAL_ERR_VAL_3;
             break;
        }
        break;

      case DS_3GPP_RAB_REJ_THROTTLED_STATE:
        switch(rab_proc)
        {
          case DS_3GPP_PROC_RAB_REESTAB_RAND_TIMER_EXP:
            ds_3gpp_rab_reestab_set_state(DS_3GPP_RAB_REQ_NOT_PENDING_STATE,
                                           user_data);
          default:
            err_val = DS_LOCAL_ERR_VAL_4;
            break;
        }

      default:
        err_val = DS_LOCAL_ERR_VAL_5;
        break;
    }
  }while(0);

  if(err_val != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_ERROR_LOG_2("Invalid rab transition"
                       "rab_state:=d::Proc_state:=d",
                        ds_3gpp_rab_reestab_state,
                        rab_proc);
  }

  return;
} /*ds_3gpp_rab_reestab_transition_state*/

/*===========================================================================
  FUNCTION DS_3GPP_PERFORM_RRC_BARRING

  DESCRIPTION
  This function processes rrc barring start stop commands
  
  PARAMETERS
  Deprior status - start/stop rrc barring
  subs id 
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_perform_rrc_barring
(
  lte_rrc_cell_deprior_sts_e    deprior_status
)
{
  ds_local_err_val_e_type    err_val = DS_LOCAL_ERR_VAL_NONE;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  do
  {
    switch (deprior_status)
    {
      case LTE_RRC_CELL_DEPRIOR_START:
      {
        ds_3gpp_rab_reestab_transition_state_ev(
                      DS_3GPP_PROC_RAB_REESTAB_RRC_BARRING_START_IND,
                      (void *)DS_3GPP_RAB_GUARD_TIMER_INTERVAL);
        break;
      }

      case  LTE_RRC_CELL_DEPRIOR_END:
      {
        ds_3gpp_rab_reestab_transition_state_ev(
                      DS_3GPP_PROC_RAB_REESTAB_RRC_BARRING_STOP_IND,
                      NULL);
        break;
      }

      default:
      {
        err_val = DS_LOCAL_ERR_VAL_0;
        break;
      }
    }

  }while(0); 

  DS_ERROR_LOG_2("ds_3gpp_perform_rrc_barring:deprior_status:=d returns =d",
                      deprior_status,err_val);
  return;
} /* ds_3gpp_perform_rrc_barring */

/*===========================================================================
FUNCTION DS_3GPP_LTE_RRC_CELL_DEPRIOR_STATUS_IND

DESCRIPTION
   This is used by RRC to notify DS rrc barring has started

PARAMETERS 
  void 
 
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

static void ds_3gpp_lte_rrc_cell_deprior_status_ind
(
  msgr_umid_type              msgr_type,
  const msgr_hdr_struct_type *dsmsg_ptr
)
{
  ds_local_err_val_e_type      local_err = DS_LOCAL_ERR_VAL_NONE;
  lte_rrc_cell_deprior_status_ind_s *payload_ptr = NULL;
/*--------------------------------------------------------------------------------*/

  do
  {

    payload_ptr  =  (lte_rrc_cell_deprior_status_ind_s *)dsmsg_ptr;

    if(payload_ptr == NULL)
    {
      local_err = DS_LOCAL_ERR_VAL_0;
      break;
    }

    ds_3gpp_perform_rrc_barring(payload_ptr->status);

  }while(0); 

  if(local_err != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_3gpp_lte_rrc_cell_deprior_status_ind retruns %d",
                       local_err);
  }
  return;
}/*ds_3gpp_lte_rrc_cell_deprior_status_ind*/


/*===========================================================================
FUNCTION DS_3GPP_RAB_REESTAB_SM_HDLR_INIT

DESCRIPTION
 Init rab reestab sm hdlr

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_rab_reestab_sm_hdlr_init
(
   void 
)
{
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 /*--------------------------------------------------------------------------
    Register for RRC BARRING START/STOP IND
  --------------------------------------------------------------------------*/
  dsmsgrrcv_reg_hdlr(LTE_RRC_CELL_DEPRIOR_STATUS_IND,
                       (dsmsgrcv_msg_hdlr_f)
                       ds_3gpp_lte_rrc_cell_deprior_status_ind);
                      

}/*ds_3gpp_rab_reestab_sm_hdlr_init*/



