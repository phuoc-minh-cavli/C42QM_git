/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S
                
                A T   C O M M A N D

                V O I C E  I N T E R F A C E

GENERAL DESCRIPTION
  This Module has the Voice command related State Machine.
  It has Call Back and event handlers. It also contains internal
  State machine
  
EXTERNALIZED FUNCTIONS INTERNAL TO DSAT UNIT
  dsatvoice_init
    Initiate voice call related vaiables.

  dsatvoice_call_event_connect
    This function is called by dsatvoice_cm_call_event_handler when CM 
    report the CM_CALL_EVENT_CONNECT event.

  dsatvoice_cmd_answer_cb_handler
    This function is the handler for CM_CALL_CMD_ANSWER cb cmd.

  dsatvoice_voice_call_abort_handler
    This function is called by dsatact_call_abort_handler to abort
    voice dial command.

  dsatvoice_call_event_incom
    This function is called by dsatcmif_cm_call_event_handler when CM 
    report the CM_CALL_EVENT_INCOM event.

  dsatvoice_call_event_end
    This function is call by dsatcmif_cm_call_event_handler when CM report 
    the CM_CALL_EVENT_END event.

  dsatvoice_call_event_orig
    This function is called when CM reports the CM_CALL_EVENT_ORIG event.

  dsatvoice_timer_expired_handler
    This function is the handler function for the RI timer expired cmd.

  dsatvoice_go_to_connecting_cmd_ext
    Change a call ID to DSAT_VOICE_STATE_CONNECTING state and set active command.

  dsatvoice_go_to_idle_state
    Change a call's state to DSAT_VOICE_STATE_IDLE.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   Copyright (c) 2001 - 2016 Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/interface/atcop/src/dsatvoice_ex.c#1 $ $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $ $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/28/16   skc     Added support for AT+VTS before connect event.
06/27/14   tk/sc   Initial revision (created file for Dynamic ATCoP).

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "dsati.h"
#include "dsatact.h"
#include "dsatctab.h"
#include "dsatvoice.h"
#include "dsatcmif.h"
#include "dsatparm.h"
#include "ds3gsiolib.h"

#include "rex.h"

#include "amssassert.h"
#include "err.h"
#include "msg.h"
#include "dsat_v.h"

#ifdef FEATURE_ETSI_PBM
#include "dsatetsime.h"
#endif /* FEATURE_ETSI_PBM */

#ifdef FEATURE_DSAT_ETSI_MODE
#include "dsatetsicall.h"
#include "dsatetsictab.h"
#include "dsatetsipkt.h"
#include "dsatetsime.h"
#include "dsatetsicmif.h"
#endif /* FEATURE_DSAT_ETSI_MODE */

#include "cm_v.h"
#include "dsatdl.h"

/*===========================================================================

            PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains public definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/
/* current call's call_id, still required for +CHV command */
LOCAL cm_call_id_type voice_call_id = CM_CALL_ID_INVALID;

rex_timer_type dsatvoice_ring_timer = {0};
rex_timer_type dsatvoice_ring_timer_2 = {0};
rex_timer_type dsatvoice_voice_connection_timer = {0};
rex_timer_type dsatvoice_voice_connection_timer_2 = {0};
rex_timer_type dsatvoice_data_connection_timer = {0};
rex_timer_type dsatvoice_data_connection_timer_2 = {0};

cm_call_id_type dsatvoice_voice_call_id = 0;
cm_call_id_type dsatvoice_data_call_id = {CM_CALL_ID_INVALID};
sys_sys_mode_e_type dsatvoice_sys_mode;
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

#define VOICE_DEASSERT_INTERVAL 100;
#define VOICE_ASSERT_INTERVAL  5900;

/* State machine variables for number of possible calls:
   voice_state_e_type defines possible states,
   voice_state array is indexed by call ID,
    */
voice_state_type voice_state[CM_CALL_ID_MAX];

/* RING associated variables */
//@sanjaykc: TODO: grouped to structure for common variables
LOCAL uint8 ring_counter;
LOCAL boolean ri_asserted;  

/*-------------------------------------------------------------------------
    Protypes for local functions:
-------------------------------------------------------------------------*/
LOCAL void voice_ring_te
(
  sys_sys_mode_e_type         sys_mode
);

LOCAL void voice_connec_completion_timeout
(
  void
);

LOCAL void data_connec_completion_timeout
(
  void
);


/*-------------------------------------------------------------------------
    External Function Definitions
-------------------------------------------------------------------------*/
/*===========================================================================

FUNCTION DSATVOICE_INIT

DESCRIPTION
  Initiate voice call related vaiables.
  It includes the ring timer.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatvoice_init( void )
{
  cm_call_id_type         call_id;

  /* Initialize voice call state machine state variables */  
  for ( call_id = 0; call_id < CM_CALL_ID_MAX; call_id++ )
  {
    dsatvoice_go_to_idle_state( call_id );
  }

  dsatvoice_voice_call_id = CM_CALL_ID_INVALID;
} /* dsatvoice_init */


/*===========================================================================

FUNCTION DSATVOICE_CALL_EVENT_END

DESCRIPTION
  This function is call by dsatcmif_cm_call_event_handler when CM report 
  the CM_CALL_EVENT_END event.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_NO_CARRIER : if the current state is not DSAT_VOICE_STATE_IDLE 
    and this call was initiated by ATD/ATA.
  DSAT_ASYNC_EVENT: otherwise.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvoice_call_event_end
(
  const ds_at_call_info_s_type    *call_info_ptr
  /* A pointer to a call state information structure */
)
{
  dsat_result_enum_type result = DSAT_ASYNC_EVENT;
  dsatetsicall_call_state_info  *call_val = NULL;
  
  call_val = dsat_get_base_addr(DSAT_CALL_VALS, TRUE);
  if (NULL == call_val)
  {
    return DSAT_ASYNC_EVENT;
  }  
  DS_ASSERT( NULL != call_info_ptr );
  
  /* Only care about voice call and emergency call */
  if ( dsatcmif_is_voice_call_type (call_info_ptr->call_type) )
  {
    /* If call is waiting to be answered but no command has been issued */
    if( voice_state[call_info_ptr->call_id].state == DSAT_VOICE_STATE_WAITING )
    {
      voice_stop_ring();
      result = DSAT_ASYNC_EVENT;
    }
    /* Else if call was attempted by ATD or ATA */
    else if( VOICE_CMD_NONE != 
             voice_state[call_info_ptr->call_id].cmd_active )
    {
      if (( voice_state[call_info_ptr->call_id].state == 
                                   DSAT_VOICE_STATE_CONNECTING ) &&
          ( VOICE_CMD_ATD != voice_state[call_info_ptr->call_id].cmd_active ) &&
          ( VOICE_CMD_ATS7 != voice_state[call_info_ptr->call_id].cmd_active ))
      {
        result = DSAT_OK;
      }
      else
      {
#ifdef FEATURE_DSAT_ETSI_MODE
        DS_AT_MSG2_HIGH("call cause = %d cause = %d",
         call_val->dsatetsicmif_call_end_status.error_info.cs_domain.end_status,
        call_val->dsatetsicmif_call_end_status.error_info.cs_domain.cc_cause.cause_value);
        if (call_val->dsatetsicmif_call_end_status.error_info.cs_domain.cc_cause.cause_value == USER_BUSY)
        {
          result = DSAT_BUSY;
        }
        else
#endif /* FEATURE_DSAT_ETSI_MODE */
        {
          result = DSAT_NO_CARRIER;
        }
        /* Need to explicitly send unsolicited response for Serializer and 
             reset afterward */
        if (FALSE == voice_state[call_info_ptr->call_id].cmd_pending)
        {
          dsatrsp_send_urc(result);
          result = DSAT_ASYNC_EVENT;
        } 
      }
      
      /* Send response only if the command is still abortable */
      if ( ( dsatcmdp_abortable_state == NOT_ABORTABLE) &&
           ( VOICE_CMD_ABORT == 
            voice_state[call_info_ptr->call_id].cmd_active ) )
      {
        result = DSAT_ASYNC_EVENT;
      }
    }
    /* else: no special action for else */

    /* Initialize state */
    dsatvoice_go_to_idle_state( call_info_ptr->call_id );
  }
  
  return result;

} /* dsatvoice_call_event_end */


/*===========================================================================

FUNCTION DSATVOICE_CALL_EVENT_INCOM

DESCRIPTION
  This function is called by dsatcmif_cm_call_event_handler when CM 
  report the CM_CALL_EVENT_INCOM event.

  If the call type is VOICE, it rings the TE and goes to WAITING_FOR_ANSWER
  state.

DEPENDENCIES

RETURN VALUE
  DSAT_ASYNC_EVENT : Tell the async event handler this event is not
                      associated with an AT command line.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvoice_call_event_incom
(
  const ds_at_call_info_s_type    *call_info_ptr
  /* A pointer to a call state information structure */
)
{
  ASSERT( NULL != call_info_ptr );
  
  if ( call_info_ptr->call_type == CM_CALL_TYPE_VOICE ||
        call_info_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
  {
  
    cm_call_id_type call_id;
    
    /* Update state */
    voice_state[call_info_ptr->call_id].state = DSAT_VOICE_STATE_WAITING;
    voice_state[call_info_ptr->call_id].cmd_active = VOICE_CMD_NONE;
    voice_state[call_info_ptr->call_id].call_mode = call_info_ptr->dsat_mode_info.
info_type;

    for ( call_id = 0; call_id < CM_CALL_ID_MAX; call_id++ )
    {
      /* If active or waiting call ID other than one just set is found... */
      if ( (voice_state[call_id].state == DSAT_VOICE_STATE_ACTIVE ||
            voice_state[call_id].state == DSAT_VOICE_STATE_WAITING) &&
           call_id != call_info_ptr->call_id )
      {
        return DSAT_ASYNC_EVENT;
      }
    }
    /* ring TE */
#ifndef FEATURE_DSAT_CUST
    voice_ring_te(call_info_ptr->sys_mode);
#endif /* FEATURE_DSAT_CUST */

  }  /* else do nothing */

  /* This event is an async event but it is not associate
     with an AT command line.  Returning DSAT_ASYNC_EVENT
     tell the master async handler, dsat_process_async_cmd,
     not try to process the rest tokens of an AT command line */
  return DSAT_ASYNC_EVENT;
} /* dsatvoice_call_event_incom */


/*===========================================================================

FUNCTION DSATVOICE_CALL_EVENT_HO_COMPLETE

DESCRIPTION

  function will be called for call hand over complete 
  Calls will move  from IP to GW

DEPENDENCIES

RETURN VALUE
  DSAT_ASYNC_EVENT : Tell the async event handler this event is not
                      associated with an AT command line.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvoice_call_event_ho_complete
(
  const ds_at_call_info_s_type    *call_info_ptr
  /* A pointer to a call state information structure */
)
{
  cm_call_id_type     call_id;    
  dsat_num_item_type  call_id_found = FALSE;
    
  DS_ASSERT( NULL != call_info_ptr );
  
    /* Update state */
  if(dsatcmif_is_voice_call_type (call_info_ptr->call_type ))
  {
    for ( call_id = 0; call_id < CM_CALL_ID_MAX; call_id++ )
    {
      /* If active or waiting call ID other than one just set is found... */
      if ( call_id == call_info_ptr->call_id )
      {
        voice_state[call_info_ptr->call_id].call_mode = call_info_ptr->dsat_mode_info.info_type;
        call_id_found = TRUE;    
      }
    }
    if(FALSE == call_id_found)
    {
      voice_state[call_info_ptr->call_id].cmd_active = VOICE_CMD_NONE;
      voice_state[call_info_ptr->call_id].call_mode = call_info_ptr->dsat_mode_info.info_type;
      voice_state[call_info_ptr->call_id].state = DSAT_VOICE_STATE_ACTIVE;
    }
  }
  DS_AT_MSG3_HIGH("evt_ho:Hand over complete for call id = %d call mode = %d call id found = %d",
                       call_info_ptr->call_id,
                       call_info_ptr->dsat_mode_info.info_type,call_id_found);
  return DSAT_ASYNC_EVENT;
} /* dsatvoice_call_event_ho_complete*/


/*===========================================================================

FUNCTION DSATVOICE_CALL_EVENT_CONNECT

DESCRIPTION
  This function is called by dsatvoice_cm_call_event_handler when CM 
  report the CM_CALL_EVENT_CONNECT event.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK          : if connecting call was initiated by ATD/ATA
  DSAT_ASYNC_EVENT : if the call type is not VOICE or connecting call was 
                     not initiated by ATD/ATA

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvoice_call_event_connect
(
  const ds_at_call_info_s_type    *call_info_ptr
  /* A pointer to a call state information structure */
)
{
  dsat_result_enum_type result = DSAT_ASYNC_EVENT;

  DS_ASSERT( NULL != call_info_ptr );
  
  /* Only care about voice calls */
  if( dsatcmif_is_voice_call_type (call_info_ptr->call_type ) )
  {
    if ( voice_state[call_info_ptr->call_id].state == DSAT_VOICE_STATE_WAITING)
    {
      voice_stop_ring();
    }
    else if(( voice_state[call_info_ptr->call_id].state == 
                                       DSAT_VOICE_STATE_CONNECTING ) &&
            ( voice_state[call_info_ptr->call_id].cmd_active == 
                                                        VOICE_CMD_ATA ))
    {
      voice_state[call_info_ptr->call_id].cmd_pending = FALSE;
      result = DSAT_OK;
    }

    /* Promote to active state */
    voice_state[call_info_ptr->call_id].state = DSAT_VOICE_STATE_ACTIVE;
  }

  return result;

} /* dsatvoice_call_event_connect */

/*===========================================================================

FUNCTION DSATVOICE_CALL_EVENT_ORIG

DESCRIPTION
  This function is called when CM reports the CM_CALL_EVENT_ORIG event.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK          : if current state is WAITING_FOR_ANSWER or DIALING
                     state, go to CONNECTED state
  DSAT_ASYNC_EVENT : if the call type is not VOICE

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvoice_call_event_orig
(
  const ds_at_call_info_s_type    *call_info_ptr
  /* A pointer to a call state information structure */
)
{
  dsat_result_enum_type result = DSAT_ASYNC_EVENT;
  dsatetsicall_call_state_info  *call_val = NULL;

  call_val = dsat_get_base_addr(DSAT_CALL_VALS, TRUE);
  if (NULL == call_val)
  {
    return DSAT_ASYNC_EVENT;
  }
  
  DS_ASSERT( NULL != call_info_ptr );
  
  if ( (call_info_ptr->call_type == CM_CALL_TYPE_VOICE) ||
           (call_info_ptr->call_type == CM_CALL_TYPE_CS_DATA) ||
           (call_info_ptr->call_type == CM_CALL_TYPE_PS_DATA) )
  {
       dsatact_track_sequence_number(
                                     call_info_ptr->call_id,
                                     CM_CALL_EVENT_ORIG,
                                     call_info_ptr->call_type);
  }
  /* only care about voice calls */
  if ( dsatcmif_is_voice_call_type (call_info_ptr->call_type) )
  {
    /* set state to connecting, if this origination is the result of an ATD 
       command, its handler will set ATD owned to true */
    voice_state[call_info_ptr->call_id].state = DSAT_VOICE_STATE_CONNECTING;
    voice_state[call_info_ptr->call_id].call_mode = call_info_ptr->dsat_mode_info.info_type;

    if ( (call_info_ptr->call_type == CM_CALL_TYPE_VOICE) ||
         (call_info_ptr->call_type == CM_CALL_TYPE_EMERGENCY))
    {
      /* Add call id to call list before connect */
      call_val->call_info_before_connect.call_id = call_info_ptr->call_id;
      call_val->call_info_before_connect.info_type = call_info_ptr->dsat_mode_info.info_type;
    }
  }

  return result;

} /* dsatvoice_call_event_orig */

/*===========================================================================

FUNCTION DSATVOICE_CMD_ANSWER_CB_HANDLER

DESCRIPTION
  This function is the handler for CM_CALL_CMD_ANSWER cb cmd.

  If status shows cm_call_cmd_answer failed, return NO CARRIER to TE and
  go back to IDLE_STATE.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD : no error.
  DSAT_NO_CARRIER: there is error.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvoice_cmd_answer_cb_handler
(
  cm_call_cmd_err_e_type    call_cmd_err
  /* Call command error code, indicating whether command
  ** is rejected and the reason */
)
{
  /* Default result returned if this is not ATCOP call
       ( no DSAT_VOICE_STATE_CONNECTING call ID found) */
  dsat_result_enum_type result = DSAT_ASYNC_EVENT;

  DS_AT_MSG1_MED("Answer cb called: %d", call_cmd_err);

  if(call_cmd_err == CM_CALL_CMD_ERR_NOERR)
  {
    /* If no error wait for call events */
    result = DSAT_ASYNC_CMD;
  }
  else
  {
    cm_call_id_type call_id;

    /* If error on attempting to answer look for 
        DSAT_VOICE_STATE_CONNECTING call ID... */
    for ( call_id = 0; call_id < CM_CALL_ID_MAX; call_id++ )
    {
      /* If DSAT_VOICE_STATE_CONNECTING call ID is found 
         (there should be only one)... */
      if ( voice_state[call_id].state == DSAT_VOICE_STATE_CONNECTING )
      {
        /* Return NO CARRIER only if ATA initiated connect attempt */
        if( VOICE_CMD_ATA == voice_state[call_id].cmd_active )
        {
          result = DSAT_NO_CARRIER;
        }

        /* Initialize state */
        dsatvoice_go_to_idle_state( call_id );
      } 
    }
  }

  return result;
} /* dsatvoice_cmd_answer_cb_handler */


/*===========================================================================

FUNCTION  DSATVOICE_TIMER_EXPIRED_HANDLER 

DESCRIPTION
  This function is the handler function for the RI timer expired cmd.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_EVENT

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvoice_timer_expired_handler
(
  ds_command_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsat_timer_id_e_type *timer_id_ptr = dsat_get_cmd_payload_ptr(cmd_ptr);

  switch (*timer_id_ptr)
  {
    case DSAT_TIMER_VOICE_RING:
      /* continue ring te */
#ifndef FEATURE_DSAT_CUST
      {
        dsat_timer_s_type *t_ptr = &dsatutil_timer_table[DSAT_TIMER_VOICE_RING];
        if(t_ptr->user_data != NULL)
        { 
          voice_ring_te(*((sys_sys_mode_e_type *)(t_ptr->user_data))); 
        }
      }
#endif /* FEATURE_DSAT_CUST */
      break;
    case DSAT_TIMER_VOICE_CALL_TIMEOUT:
      /* Voice S7 timer timed out */      
      voice_connec_completion_timeout();
      break;
    case DSAT_TIMER_DATA_CALL_TIMEOUT:
      /* Data S7 timer timed out */      
      data_connec_completion_timeout();
      break;
    default:
      ERR_FATAL("Illegal timer_id: %d", (int)*timer_id_ptr,0,0);
  }

  /* This event is an async event but it is not associate
     with an AT command line.  Returning DSAT_ASYNC_EVENT
     tell the master async handler, dsat_process_async_cmd,
     not try to process the rest tokens of an AT command line */
  return DSAT_ASYNC_EVENT;
} /* dsatvoice_timer_expired_handler */

/*===========================================================================

FUNCTION DSATVOICE_VOICE_CALL_ABORT_HANDLER

DESCRIPTION
  This function is called by dsatact_call_abort_handler to abort
  currently DSAT_VOICE_STATE_CONNECTING voice dial command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatvoice_voice_call_abort_handler
(
  void
)
{
  boolean                 call_status;
  cm_call_id_type         call_id;

  /* If error on attempting to answer look for DSAT_VOICE_STATE_CONNECTING call ID... */
  for ( call_id = 0; call_id < CM_CALL_ID_MAX; call_id++ )
  {
    /* If DSAT_VOICE_STATE_CONNECTING call ID is found (there should be only one)... */
    if ( voice_state[call_id].state == DSAT_VOICE_STATE_CONNECTING )
    {
      call_status = dsatcmif_end_calls(1, &call_id);
      if (voice_state[call_id].cmd_active != VOICE_CMD_ATS7)
      {
        voice_state[call_id].cmd_active = VOICE_CMD_ABORT;
      }
      if(!call_status)
      {
        DS_AT_MSG0_ERROR("ATCOP's end call request is denied by CM");
      }
      return;
    } 
  }

  DS_AT_MSG0_ERROR("ATD/ATA abort handler and no connecting call");
} /* dsatvoice_voice_call_abort_handler */

/*===========================================================================

FUNCTION DSATVOICE_GO_TO_IDLE_STATE

DESCRIPTION
  Change to IDLE_STATE state.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatvoice_go_to_idle_state
(
  cm_call_id_type         call_id
)
{
  voice_state[call_id].state = DSAT_VOICE_STATE_IDLE;
  voice_state[call_id].cmd_active = VOICE_CMD_NONE;
  voice_state[call_id].cmd_pending = FALSE;
  voice_state[call_id].call_mode = CM_CALL_MODE_INFO_NONE;

  /* Maintain variable required by 1x code */
  if ( call_id == voice_call_id )
  {
    voice_call_id = CM_CALL_ID_INVALID;
  }
}/* dsatvoice_go_to_idle_state */



/*===========================================================================

FUNCTION dsatvoice_go_to_connecting_cmd_ext

DESCRIPTION
  Change a call ID to DSAT_VOICE_STATE_CONNECTING state and set active command.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatvoice_go_to_connecting_cmd_ext
(
  cm_call_id_type         call_id,
  voice_cmd_e_type        cmd_active
)
{
  voice_state[call_id].state = DSAT_VOICE_STATE_CONNECTING;
  voice_state[call_id].cmd_active = cmd_active;
  voice_state[call_id].cmd_pending = TRUE;
}/* dsatvoice_go_to_connecting_cmd_ext */



/*-------------------------------------------------------------------------
    LOCAL Function Definitions
-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION VOICE_RING_TE

DESCRIPTION
  Ring TE and activate auto anwser depending on S0 value.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL void voice_ring_te
(
  sys_sys_mode_e_type         sys_mode
)
{
  boolean call_answered = FALSE;
  dword duration = 0;
  cm_call_id_type call_id;

  for ( call_id = 0; call_id < CM_CALL_ID_MAX; call_id++ )
  {
    /* If waiting call ID is found... */
    if ( voice_state[call_id].state == DSAT_VOICE_STATE_WAITING )
    {
      if ( ri_asserted )
      {
        duration = VOICE_DEASSERT_INTERVAL;
        ri_asserted = FALSE;

        if( (dsat_num_item_type)dsatutil_get_val(
                                   DSAT_SREG_S0_IDX,0,NUM_TYPE) != 0 )
        {
          if(++ring_counter == (dsat_num_item_type)dsatutil_get_val(
                                         DSAT_SREG_S0_IDX,0,NUM_TYPE))
          {
              voice_stop_ring();
              /* S0 times ring reached, answer the call */
              if(dsatcmif_answer_voice_call(call_id,TRUE,voice_state[call_id].call_mode))
              {
                /* we are not waiting for an ATA anymore */
                dsatvoice_go_to_connecting_cmd_ext(call_id, VOICE_CMD_ATS0);
                call_answered = TRUE;
              }
              else
              {
                /* no buffer to send the request */
                DS_AT_MSG0_ERROR("ATCOP's auto answer-call is denied by CM");
                dsat_send_result_code(DSAT_NO_CARRIER);
                dsatvoice_go_to_idle_state( call_id );
                call_answered = TRUE;
              }
          } /* ring counter reach auto-answer num */
        } /* don't do auto answer */
      } /* if ( ri_asserted ) */
      else /* ri is not asserted */
      {
        /* Send the RING result code to the TE. */

        if(DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.voice_send_ring_result_fp))
        {
          dsatdl_vtable.voice_send_ring_result_fp(voice_state[call_id].call_mode);
        }
        
        duration = VOICE_ASSERT_INTERVAL;
        ri_asserted = TRUE;
      }

      if( !call_answered )
      {
       dsat_timer_s_type *t_ptr = &dsatutil_timer_table[DSAT_TIMER_VOICE_RING];

       if(t_ptr->user_data != NULL)
       {
         *(sys_sys_mode_e_type*)(t_ptr->user_data) = sys_mode;
       }
        /*---------------------------------------------------------------------
          Call was not answered, re-start the ring timer.
        ---------------------------------------------------------------------*/
        ps_timer_start(t_ptr->timer_handle,duration);
      }

      return;
    } /* If waiting call ID is found... */
  }

  DS_AT_MSG0_ERROR("voice_ring_te() called and no DSAT_VOICE_STATE_WAITING call found");

  voice_stop_ring();
  return;
} /* voice_ring_te */


/*===========================================================================

FUNCTION VOICE_STOP_RING

DESCRIPTION
  Stop ringing TE.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void voice_stop_ring
(
  void
)
{
  /* stop ringing */
  ps_timer_cancel(dsatutil_timer_table[DSAT_TIMER_VOICE_RING].timer_handle);
  ring_counter = 0;
  ri_asserted = FALSE;
} /* voice_stop_ring */

/*===========================================================================

FUNCTION VOICE_CONNEC_COMPLETION_TIMEOUT

DESCRIPTION
  Calls the Abort handler for a voice Call due to the expiry of S7 timer.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void voice_connec_completion_timeout
(
  void
)
{
  dsati_mode_e_type current_mode = dsatcmdp_get_current_mode();

  DS_AT_MSG0_HIGH("VOICE S7 Timer Expired ");

#ifdef  FEATURE_DSAT_ETSI_MODE
  if ( IS_ETSI_MODE(current_mode) )
#endif /* FEATURE_DSAT_ETSI_MODE */
  {
    cm_call_id_type call_id;

    /* look for CONNECTING call ID... */
    for ( call_id = 0; call_id < CM_CALL_ID_MAX; call_id++ )
    {
      /* If CONNECTING call ID is found (there should be only one)... */
      if ( voice_state[call_id].state == DSAT_VOICE_STATE_CONNECTING )
      {
        voice_state[call_id].cmd_active = VOICE_CMD_ATS7;
        break;
      }
    }
    
    dsatvoice_voice_call_abort_handler();
  }
} /* voice_connec_completion_timeout */

/*===========================================================================

FUNCTION DATA_CONNEC_COMPLETION_TIMEOUT

DESCRIPTION
  Calls the Abort handler for a Data Call due to the expiry of S7 timer.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void data_connec_completion_timeout
(
  void
)
{
  dsati_mode_e_type current_mode = dsatcmdp_get_current_mode();
  dsatetsicall_call_state_info *call_val = NULL;

  DS_AT_MSG0_HIGH("DATA S7 Timer Expired ");

#ifdef FEATURE_DSAT_ETSI_MODE
  if ( IS_ETSI_MODE(current_mode) )
#endif /* FEATURE_DSAT_ETSI_MODE */
  {
    call_val = dsat_get_base_addr(DSAT_CALL_VALS, TRUE);
    if (NULL  == call_val)
    {
      return;
    }
    if((call_val->dsatetsicall_s7_info.data_s7_abort_handler != NULL) )
    {
      call_val->dsatetsicall_s7_info.data_s7_abort_handler();
      call_val->dsatetsicall_s7_info.data_s7_abort_handler = NULL;
      call_val->dsatetsicall_s7_info.is_profile_id_valid = FALSE;
      call_val->dsatetsicall_s7_info.profile_id = 0;
    }
#ifdef FEATURE_DSAT_ETSI_MODE
    else if ( TRUE == dsat_pdp_state.pending )
    {
      cm_end_params_s_type end_info;
      boolean cm_result = TRUE;
      dsat_timer_s_type *t_ptr = NULL;
      t_ptr = &dsatutil_timer_table[DSAT_TIMER_DATA_CALL_TIMEOUT];

      DS_ASSERT(NULL != t_ptr && t_ptr->user_data != NULL);
      memset((void*)&end_info, 0, sizeof( cm_end_params_s_type));
      end_info.info_type = CM_CALL_MODE_INFO_GW_PS;
      end_info.end_params.gw_ps_end.cause = SM_REGULAR_DEACTIVATION;
      end_info.call_id = *(cm_call_id_type *)(t_ptr->user_data);

      cm_result = cm_mm_call_cmd_end ( dsatcmif_call_cmd_cb_func,
                                       NULL,
                                       dsatcm_client_id,
                                       1,
                                       &end_info
                                     );
      
      if (!cm_result)
      {
        DS_AT_MSG0_ERROR("Problem invoking CM API on deactivation");
      }

    }
#endif /* FEATURE_DSAT_ETSI_MODE */
  }
} /* data_connec_completion_timeout */

/*-------------------------------------------------------------------------*/
